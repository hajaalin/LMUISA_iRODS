acGetCellomicsUser(*mdb,*user) {
    msiExecCmd("cellomics-mdb-get-user.sh",*mdb,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    acLog("acGetCellomicsUser");
    acLog(*tmp);
    acLog("acGetCellomicsUser");
    #msiStrchop(*tmp,*tmp2);
    msiStrchop(*tmp,*user);
}
acGetCellomicsAssay(*mdb,*assay) {
    acLog("acGetCellomicsAssay: "++*mdb);
    msiExecCmd("echo.sh","*mdb","null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    acLog("acGetCellomicsAssay: echo"++*tmp);
    msiExecCmd("cellomics-mdb-get-assay.sh",*mdb,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    msiStrchop(*tmp,*assay);
}
acGetCellomicsFormFactor(*mdb,*ff) {
    msiExecCmd("cellomics-mdb-get-formfactor.sh",*mdb,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    msiStrchop(*tmp,*ff);
}
acGetCellomicsObjective(*mdb,*objective) {
    msiExecCmd("cellomics-mdb-get-objective.sh",*mdb,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    #msiStrchop(*tmp,*tmp2);
    msiStrchop(*tmp,*objective);
}

acApplyCellomicsAVUs(*mdb) {
    acLog("acApplyCellomicsAVUs: *mdb");
    
    msiIsData(*mdb,*dataID,*foo);
    acGetDataPathForDataID(*dataID,*dataPath);
    
    # find collection
    msiSplitPath(*mdb,*coll,*file);
    
    acGetCellomicsAssay(*dataPath,*assay);
    acGetCellomicsFormFactor(*dataPath,*ff);
    acGetCellomicsObjective(*dataPath,*obj);
    acGetCellomicsUser(*dataPath,*user);
    msiAddKeyVal(*KVPairs,"CellomicsAssay",*assay);
    msiAddKeyVal(*KVPairs,"CellomicsFormFactor",*ff);
    msiAddKeyVal(*KVPairs,"CellomicsObjective",*obj);
    msiAddKeyVal(*KVPairs,"CellomicsUser",*user);
    
    msiAssociateKeyValuePairsToObj(*KVPairs,*coll,"-C"); 
    #acAssociateKeyValuePairsToColl(*KVPairs,*coll); 
}
