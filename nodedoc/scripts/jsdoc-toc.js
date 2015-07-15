(function($) {
    // TODO: make the node ID configurable
    var treeNode = $('#jsdoc-toc-nav');

    // initialize the tree
    treeNode.tree({
        autoEscape: false,
        closedIcon: '&#x21e2;',
        data: [{"label":"<a href=\"global.html\">Globals</a>","id":"global","children":[]},{"label":"<a href=\"HierarchMarkov.html\">HierarchMarkov</a>","id":"HierarchMarkov","children":[]},{"label":"<a href=\"NearestNeighborAD.html\">NearestNeighborAD</a>","id":"NearestNeighborAD","children":[]},{"label":"<a href=\"RidgeReg.html\">RidgeReg</a>","id":"RidgeReg","children":[]},{"label":"<a href=\"SVC.html\">SVC</a>","id":"SVC","children":[]},{"label":"<a href=\"SVR.html\">SVR</a>","id":"SVR","children":[]},{"label":"<a href=\"module-datasets.html\">datasets</a>","id":"module:datasets","children":[]},{"label":"<a href=\"module-fs.html\">fs</a>","id":"module:fs","children":[{"label":"<a href=\"module-fs.FIn.html\">FIn</a>","id":"module:fs.FIn","children":[]},{"label":"<a href=\"module-fs.FOut.html\">FOut</a>","id":"module:fs.FOut","children":[]}]},{"label":"<a href=\"module-ht.html\">ht</a>","id":"module:ht","children":[{"label":"<a href=\"module-ht.IntFltMap.html\">IntFltMap</a>","id":"module:ht.IntFltMap","children":[]},{"label":"<a href=\"module-ht.IntIntMap.html\">IntIntMap</a>","id":"module:ht.IntIntMap","children":[]},{"label":"<a href=\"module-ht.IntStrMap.html\">IntStrMap</a>","id":"module:ht.IntStrMap","children":[]},{"label":"<a href=\"module-ht.StrFltMap.html\">StrFltMap</a>","id":"module:ht.StrFltMap","children":[]},{"label":"<a href=\"module-ht.StrIntMap.html\">StrIntMap</a>","id":"module:ht.StrIntMap","children":[]},{"label":"<a href=\"module-ht.StrStrMap.html\">StrStrMap</a>","id":"module:ht.StrStrMap","children":[]}]},{"label":"<a href=\"module-la.html\">la</a>","id":"module:la","children":[{"label":"<a href=\"module-la.BoolVector.html\">BoolVector</a>","id":"module:la.BoolVector","children":[]},{"label":"<a href=\"module-la.IntVector.html\">IntVector</a>","id":"module:la.IntVector","children":[]},{"label":"<a href=\"module-la.Matrix.html\">Matrix</a>","id":"module:la.Matrix","children":[]},{"label":"<a href=\"module-la.SparseMatrix.html\">SparseMatrix</a>","id":"module:la.SparseMatrix","children":[]},{"label":"<a href=\"module-la.SparseVector.html\">SparseVector</a>","id":"module:la.SparseVector","children":[]},{"label":"<a href=\"module-la.StrVector.html\">StrVector</a>","id":"module:la.StrVector","children":[]},{"label":"<a href=\"module-la.Vector.html\">Vector</a>","id":"module:la.Vector","children":[]}]},{"label":"<a href=\"module-qm.html\">qm</a>","id":"module:qm","children":[{"label":"<a href=\"module-qm.Base.html\">Base</a>","id":"module:qm.Base","children":[]},{"label":"<a href=\"module-qm.FeatureSpace.html\">FeatureSpace</a>","id":"module:qm.FeatureSpace","children":[]},{"label":"<a href=\"module-qm.Iterator.html\">Iterator</a>","id":"module:qm.Iterator","children":[]},{"label":"<a href=\"module-qm.Record.html\">Record</a>","id":"module:qm.Record","children":[]},{"label":"<a href=\"module-qm.RecordSet.html\">RecordSet</a>","id":"module:qm.RecordSet","children":[]},{"label":"<a href=\"module-qm.Store.html\">Store</a>","id":"module:qm.Store","children":[]},{"label":"<a href=\"module-qm.StreamAggr.html\">StreamAggr</a>","id":"module:qm.StreamAggr","children":[]}]},{"label":"<a href=\"module-statistics.html\">statistics</a>","id":"module:statistics","children":[]}],
        openedIcon: ' &#x21e3;',
        saveState: true,
        useContextMenu: false
    });

    // add event handlers
    // TODO
})(jQuery);
