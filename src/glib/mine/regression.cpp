/**
 * Copyright (c) 2015, Jozef Stefan Institute, Quintelligence d.o.o. and contributors
 * All rights reserved.
 * 
 * This source code is licensed under the FreeBSD license found in the
 * LICENSE file in the root directory of this source tree.
 */

using namespace TRegression;

///////////////////////////////////////////
// Logistic Regression
TLogReg::TLogReg(const double& _Lambda, const bool _IncludeIntercept, const bool _Verbose):
		Lambda(_Lambda),
		WgtV(),
		IncludeIntercept(_IncludeIntercept),
		Verbose(_Verbose),
		Notify(Verbose ? TNotify::StdNotify : TNotify::NullNotify) {}

TLogReg::TLogReg(TSIn& SIn):
		Lambda(TFlt(SIn)),
		WgtV(SIn),
		IncludeIntercept(TBool(SIn)),
		Verbose(TBool(SIn)),
		Notify(nullptr) {

	Notify = Verbose ? TNotify::StdNotify : TNotify::NullNotify;
}

void TLogReg::Save(TSOut& SOut) const {
	TFlt(Lambda).Save(SOut);
	WgtV.Save(SOut);
	TBool(IncludeIntercept).Save(SOut);
	TBool(Verbose).Save(SOut);
}


void TLogReg::Fit(const TFltVV& _X, const TFltV& y, const double& Eps) {
	TFltVV X(_X);

	if (IncludeIntercept) {
		// add 1s into the last row
		X.AddXDim();
		for (int i = 0; i < X.GetCols(); i++) {
			X(X.GetRows()-1, i) = 1;
		}
	}

	const int NInst = X.GetCols();
	const int Dim = X.GetRows();
	const int OrigDim = IncludeIntercept ? Dim-1 : Dim;
	EAssertR(NInst == y.Len(), "TLogReg::Fit the number of instances in X.GetCols() and y.Len() do not match");
	// minimize the following objective function:
	// L(w) = (sum(log(1 + exp(w*x_i)) - y_i*w*x_i) + lambda*beta*beta'/2) / m
	// using Newton-Raphson algorithm:
	// w <- w - H^(-1)(w)*g(w)
	// g(w) = (X*(s(beta*x) - y)' + lambda*beta')
	// H(w) = X*W*X^(-1) + lambda*I
	// where H is the Hessian at point w, g is the gradient of the objective function at point w
	// W is a diagonal matrix defined as W_ii = p_i(1 - p_i)

	// temporary variables
	TFltV ProbV(NInst, NInst);					// vector of probabilities
	TFltV PrevProbV(NInst, NInst);				// vector of probs in the previous step, used to terminate the procedure
	TFltV DeltaWgtV(Dim, Dim);					// the step used to update the weights
	TFltV YMinP(NInst, NInst);
	TFltV GradV(Dim, Dim);						// gradient
	TFltVV XTimesW(Dim, NInst);					// temporary variable to compute (X*W)*X'
	TFltVV H(Dim, Dim);							// Hessian
	TFltVV X_t(X.GetCols(), X.GetRows());	TLinAlg::Transpose(X, X_t);		// the transposed instance matrix
	TVec<TIntFltKdV> WgtColSpVV(NInst, NInst);	// weight matrix

	// generate weight matrix with only ones on the diagonal
	// so you don't recreate all the object every iteration
	for (int i = 0; i < NInst; i++) {
		WgtColSpVV[i].Add(TIntFltKd(i, 1));
	}

	WgtV.Gen(Dim);

	// perform the algorithm
	double Diff = TFlt::NInf;
	double AbsDiff;
	int k = 1;
	do {
		if (k % 10 == 0) {
			Notify->OnNotifyFmt(TNotifyType::ntInfo, "Step: %d, diff: %.3f", k, Diff);
		}

		// compute the probabilities p_i = 1 / (1 + exp(-w*x_i)) and
		// compute the weight matrix diagonal W_ii = p_i(1 - p_i)
		TLinAlg::Multiply(X_t, WgtV, ProbV);
		for (int i = 0; i < NInst; i++) {
			ProbV[i] = 1 / (1 + TMath::Power(TMath::E, -ProbV[i]));
			WgtColSpVV[i][0].Dat = ProbV[i]*(1 - ProbV[i]);
		}

		// compute the Hessian H = X*W*X' + lambda*I
		// 1) compute X*W
		TLinAlg::Multiply(X, WgtColSpVV, XTimesW);
		// 2) compute H = (X*W)*X'
		TLinAlg::Multiply(XTimesW, X_t, H);
		// 3) add lambda to the diagonal of H, exclude the punishment for the intercept
		for (int i = 0; i < OrigDim; i++) {
			H(i,i) += Lambda;
		}

		// compute the gradient g(w) = X*(y - p)' + lambda * w
		// 1) compute (y - p)
		TLinAlg::LinComb(1, y, -1, ProbV, YMinP);
		// 2) compute X*(y - p)
		TLinAlg::Multiply(X, YMinP, GradV);
		// 3) add lambda * w, exclude the punishment for the intercept
		for (int i = 0; i < OrigDim; i++) {
			GradV[i] += Lambda*WgtV[i];
		}

		// compute delta_w = H(w) \ (g(w))
#ifdef LAPACKE
		const TFlt SingEps = 1e-10;

		if (H.GetRows() == 1) {	// fix for a bug in SVD factorization
			DeltaWgtV[0] = GradV[0] / H(0,0);
		} else {
			TLinAlg::SVDSolve(H, DeltaWgtV, GradV, SingEps);
		}
#else
		throw TExcept::New("Should include LAPACKE!!");
#endif

		if (TFlt::IsNan(TLinAlg::Norm(DeltaWgtV))) {
			Notify->OnNotifyFmt(TNotifyType::ntInfo, "Got NaNs while fitting logistic regression! The weights could still be OK.");
			break;
		}

		// update the current weight vector
		for (int i = 0; i < Dim; i++) {
			WgtV[i] += DeltaWgtV[i];
		}

		// recompute the termination criteria and store the probabilities for
		// the next iteration
		Diff = TFlt::NInf;
		for (int i = 0; i < NInst; i++) {
			AbsDiff = TFlt::Abs(PrevProbV[i] - ProbV[i]);
			if (AbsDiff > Diff) { Diff = AbsDiff; }

			PrevProbV[i] = ProbV[i];
		}

		k++;
	} while (Diff > Eps);

	Notify->OnNotifyFmt(TNotifyType::ntInfo, "Converged. Diff: %.5f", Diff);
}

double TLogReg::Predict(const TFltV& x) const {
	if (IncludeIntercept) {
		TFltV x1(x);	x1.Add(1);
		return PredictWithoutIntercept(x1);
	} else {
		return PredictWithoutIntercept(x);
	}
}

void TLogReg::GetWgtV(TFltV& _WgtV) const {
	_WgtV = WgtV;
	if (IncludeIntercept) {
		_WgtV.DelLast();
	}
}

double TLogReg::PredictWithoutIntercept(const TFltV& x) const {
	if (!Initialized()) { return 0; }
	EAssertR(x.Len() == WgtV.Len(), "Dimension mismatch while predicting!");
	return 1 / (1 + TMath::Power(TMath::E, -TLinAlg::DotProduct(WgtV, x)));
}

///////////////////////////////////////////
// Proportional Hazards model

TPropHazards::TPropHazards(const double& _Lambda, const bool _Verbose):
		Lambda(_Lambda),
		WgtV(),
		Verbose(_Verbose),
		Notify(_Verbose ? TNotify::StdNotify : TNotify::NullNotify) {}

TPropHazards::TPropHazards(TSIn& SIn):
		Lambda(TFlt(SIn)),
		WgtV(SIn),
		Verbose(TBool(SIn)) {
	Notify = Verbose ? TNotify::StdNotify : TNotify::NullNotify;
}

void TPropHazards::Save(TSOut& SOut) const {
	TFlt(Lambda).Save(SOut);
	WgtV.Save(SOut);
	TBool(Verbose).Save(SOut);
}

void TPropHazards::Fit(const TFltVV& _X, const TFltV& t, const double& Eps) {
	const int NInst = _X.GetCols();
	const int Dim = _X.GetRows() + 1;	
	EAssertR(NInst == t.Len(), "TPropHazards::Fit the number of instances in X.GetCols() and t.Len() do not match");
	Notify->OnNotifyFmt(TNotifyType::ntInfo, "Fitting proportional hazards model on %d instances ...", NInst);

	TFltVV X(_X.GetRows()+1, NInst);
	for (int ColN = 0; ColN < NInst; ColN++) {
		X(0, ColN) = 1;
		for (int RowN = 0; RowN < _X.GetRows(); RowN++) {
			X(RowN+1, ColN) = _X(RowN, ColN);
		}
	}

	WgtV.Gen(Dim);

	TFltVV X_t(X);	X_t.Transpose();	// TODO slow
	TFltVV XTimesW(X.GetRows(), X.GetCols());
	TFltVV H(Dim, Dim);

	TFltV TempNInstV(NInst, NInst);
	TFltV GradV(Dim, Dim);
	TFltV DeltaWgtV(Dim, Dim);
	TSpVV WgtColSpVV(NInst, NInst);

	double IntensTimesT;

	// generate weight matrix with only ones on the diagonal
	// so you don't recreate all the object every iteration
	for (int i = 0; i < NInst; i++) {
		WgtColSpVV[i].Add(TIntFltKd(i, 1));
	}

	int k = 0;
	double Diff = TFlt::PInf;
	while (Diff > Eps) {
		// construct the intensity vector
		PredictInternal(X, TempNInstV);

		// I) construct the Hessian: X*W*X' + lambda*I
		// prepare W and t .* intens - 1
		for (int i = 0; i < NInst; i++) {
			IntensTimesT = t[i] * TempNInstV[i];
			TempNInstV[i] = IntensTimesT - 1;
			WgtColSpVV[i][0].Dat = IntensTimesT;
		}
		// 1) compute X*W
		TLinAlg::Multiply(X, WgtColSpVV, XTimesW);
		// 2) compute (X*W)*X'
		TLinAlg::Multiply(XTimesW, X_t, H);
		// 3) (X*W*X') + lambda*I, exclude the base hazard
		if (Lambda > 0) {
			for (int i = 1; i < Dim; i++) {
				H(i,i) += Lambda;
			}
		}

		// II) construct the gradient: (t .* intens - 1) * X' + lambda*[0, w(2:end)]
		// 1) (t .* intens - 1) * X'
		TLinAlg::Multiply(X, TempNInstV, GradV);
		// 2) ((t .* intens - 1) * X') + lambda*[0, w(2:end)]
		if (Lambda > 0) {
			for (int i = 1; i < Dim; i++) {
				GradV[i] += Lambda * WgtV[i];
			}
		}

		// III) compute: delta_w = H \ grad
#ifdef LAPACKE
		const TFlt SingEps = 1e-10;

		if (H.GetRows() == 1) {	// fix for a bug in SVD factorization
			DeltaWgtV[0] = GradV[0] / H(0,0);
		} else {
			TLinAlg::SVDSolve(H, DeltaWgtV, GradV, SingEps);
		}
#else
		throw TExcept::New("Should include LAPACKE!!");
#endif

		// IV) w <= w - delta_w
		for (int i = 0; i < Dim; i++) {
			WgtV[i] -= DeltaWgtV[i];
		}

		Diff = TLinAlg::Norm(DeltaWgtV);
		EAssertR(!TFlt::IsNan(Diff), "nans in delta wgt vector!");

		if (++k % 10 == 0) {
			Notify->OnNotifyFmt(TNotifyType::ntInfo, "Step: %d, diff: %.3f", k, Diff);
		}
	}


	Notify->OnNotifyFmt(TNotifyType::ntInfo, "Converged. Diff: %.5f", Diff);
}

double TPropHazards::Predict(const TFltV& x) const {
    // check we even have a model
	if (WgtV.Empty()) { return 0; }
    // check dimensionality
    EAssert((x.Len() + 1) == WgtV.Len());
    
    // apply the model
	double Pred = WgtV[0];
	for (int i = 1; i < WgtV.Len(); i++) {
		Pred += x[i-1]*WgtV[i];
	}

	return exp(Pred);
}

void TPropHazards::GetWgtV(TFltV& _WgtV) const {
	for (int i = 1; i < WgtV.Len(); i++) {
		_WgtV.Add(WgtV[i]);
	}
}

void TPropHazards::PredictInternal(const TFltVV& X, TFltV& IntensV) const {
	const int NInst = X.GetCols();

	TLinAlg::MultiplyT(X, WgtV, IntensV);
	for (int i = 0; i < NInst; i++) {
		IntensV[i] = exp(IntensV[i]);
	}
}

/////////////////////////////////////////////
// Ridge Regression
void TRidgeReg::Fit(const TFltVV& X, const TFltV& y) {
    TNumericalStuff::LeastSquares(X, y, Gamma, WgtV);
}

double TRidgeReg::Predict(const TFltV& x) const {
    EAssertR(x.Len() == WgtV.Len(), "TRegression::TRidgeReg::Predict: model and data dimension mismatch");
    return TLinAlg::DotProduct(x, WgtV);
}



