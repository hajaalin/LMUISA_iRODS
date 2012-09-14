#
# Rules to add plate metadata to Leica Matrix Screener data.
# 
# The following AVUs will be set:
# project = parent of dataset
# dataset = experiment--....
# well = A01 - H12
# field = 1,2,3,..
#
acSetMatrixScreenerPlateData(*obj,*res) {
    acLog("acSetMatrixScreenerPlateData: "++*obj);
    
    # get the object path
    msiGetObjectPath(*obj,*path,*status);
    acLog(*path);
    
    msiPyInitialize;
    msiLocalPython2("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","testH2","noRecursionTest",*inp,*res);
    msiLocalPython2("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerProject","noRecursionTest",*path,*project);
    msiLocalPython2("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerExperiment","noRecursionTest",*path,*experiment);
    msiLocalPython2("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerWellCode","noRecursionTest",*obj,*well);
    msiLocalPython2("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerFieldIndex","noRecursionTest",*obj,*field);
    msiPyFinalize;
    
    msiAddKeyVal(*KVPairs,'project',*experiment);
    msiAddKeyVal(*KVPairs,'dataset',*experiment);
    msiAddKeyVal(*KVPairs,'well',*well);
    msiAddKeyVal(*KVPairs,'field',*field);
   
   msiAssociateKeyValuePairsToObj(*KVPairs,*obj,"-d");
}


