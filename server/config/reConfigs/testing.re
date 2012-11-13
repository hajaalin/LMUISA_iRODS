#
# This file contains various test rules.
#

# python testing
acPythonWorks(*pyScript) {
    msiPyInitialize;
    msiLocalPython(*pyScript,"testH0","noRecursionTest");
    msiPyFinalize;}
    
testH0 {
    msiPyInitialize;
    msiLocalPython("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py", "testH0", "noRecursionTest");
    msiPyFinalize; }
testH1(*res) {
    msiPyInitialize;
    msiLocalPython1("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","testH1","noRecursionTest",*res);
    msiPyFinalize;}
testH2(*inp,*res) {
    msiPyInitialize;
    msiLocalPython2("/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py","testH2","noRecursionTest",*inp,*res);
    msiPyFinalize;}

 
# for testing delayExec
acTestDelay(*coll) {
    acLog("acTestDelay");
    delay("<PLUSET>120s</PLUSET><EF>120s DOUBLE 2 TIMES</EF>") {
        acTrackLeicaCollTest(*coll); }
}


acJoo(*Coll) {
    msiIsReadyHcsColl(*Coll,*CollReady,*foo);
    acLog("acJoo: "++*CollReady);
    if (*CollReady == "READY") {
        acLeica2CellomicsCopyColl(*Coll);}
}

# for testing the ifExec 
acTrackLeicaCollTest(*coll) {
    acLog("acTrackLeicaCollTest");
    msiIsReadyHcsCollRandom(*coll,*ready,*foo);
    acLog("Before ifExec "++*ready++" READY");
    if (*ready == "READY") {
        acLog("Collection "++*coll++" ready for conversion");
        succeed;}
    else {
        acLog("Collection "++*coll++" not ready.");
        fail;}
}
