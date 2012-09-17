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
    
    msiPyInitialize;
    msiLocalPython2("/opt/iRODS/iRODS_3.1/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerProject","noRecursionTest",*obj,*project);
    msiLocalPython2("/opt/iRODS/iRODS_3.1/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerExperiment","noRecursionTest",*obj,*experiment);
    msiLocalPython2("/opt/iRODS/iRODS_3.1/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerWell","noRecursionTest",*obj,*well);
    msiLocalPython2("/opt/iRODS/iRODS_3.1/server/bin/cmd/embedPython/lmuisa.py","msiGetMatrixScreenerField","noRecursionTest",*obj,*field);
    msiPyFinalize;
    
    msiAddKeyVal(*KVPairs,'project',*project);
    msiAddKeyVal(*KVPairs,'dataset',*experiment);
    msiAddKeyVal(*KVPairs,'well',*well);
    msiAddKeyVal(*KVPairs,'field',*field);
   
   msiAssociateKeyValuePairsToObj(*KVPairs,*obj,"-d");
}

acAVUs2BisqueTags(*obj,*tags) {
    acLog("acAVUs2BisqueTags: "++*obj);
    msiPyInitialize;
    msiLocalPython2("/opt/iRODS/iRODS_3.1/server/bin/cmd/embedPython/lmuisa.py","msiAVUs2BisqueTags","noRecursionTest",*obj,*tags);
    msiPyFinalize;
}
