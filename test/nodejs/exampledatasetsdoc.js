describe('example tests for the datasetsdoc.js file', function () {
describe("Datasets module includes some standard toy datasets. In addition, this module also includes various, number 1", function () {
it("should make test number 1", function () {

     var qm = require('qminer');
     var datasets = qm.datasets;
     
     // Create clean base
     var base = new qm.Base({ mode: 'createClean' });
    
     // Load Iris dataset in async way
     datasets.loadIris(base, function (err, store) {
         if (err) throw err;
         console.log("\nSucessfully loaded database: " + store.name)
         console.log(JSON.stringify(store.first, null, 2));
         base.close();
     });
     
    
});
});
describe("Datasets module includes some standard toy datasets. In addition, this module also includes various, number 2", function () {
it("should make test number 2", function () {

     var qm = require('qminer');
     var datasets = qm.datasets;
     
     // Create clean base
     var base = new qm.Base({ mode: 'createClean' });
    
     // Load Iris in sync way.
     var Iris = datasets.loadIrisSync(base);
     console.log("\nSucessfully loaded database: " + Iris.name)
     console.log(JSON.stringify(Iris.first, null, 2));
     base.close();
    
});
});

});
