#
# This file contains rules needed to convert a Leica Matrix Screener output directory tree
# to one directory containing all the data. The naming convention follows Cellomics specifications,
# so the resulting collection should be readable by vHCS Toolbox.
#

# run a embedPython microservice to add metadata into Leica experiment--* folder (*inputDir)
# this already defines where the renamed copy for Cellomics will be created (*outputRoot)
acLeica2CellomicsAddMetadata(*coll,*res) {
    acLeica2CellomicsAddMetadata0(
        "/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py",
        "msiLeica2Cellomics",
        *coll,
        "/"++$rodsZoneClient++"/home/"++$userNameClient++"/Conversions",
        *res);}
acLeica2CellomicsAddMetadata0(*pyScript,*methName,*coll,*outputRoot,*res) {
    acLog("acLeica2CellomicsAddMetadata: "++*coll);
    msiPyInitialize;
    msiLocalPython3(*pyScript,*methName,"noRecursionTest",*coll,*outputRoot,*res);
    msiPyFinalize;}


# helper rules to get metadata values related to Leica -> Cellomics conversion
acGetCellomicsName(*dataID,*cname) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'cellomics.imageName' AND DATA_ID = '*dataID'",*cname);}
acGetCellomicsColl(*dataID,*cname) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'cellomics.collection' AND DATA_ID = '*dataID'",*cname);}
acGetCellomicsObj(*dataID,*cname) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'cellomics.dataObject' AND DATA_ID = '*dataID'",*cname);}


# writes the Cellomics name of a Leica image
acWriteCellomicsName(*Object) {
    msiIsData(*Object,*dataID,*foo);
    msiGetObjectPath(*Object,*Path,*foo);
    if (*Path like "\*.tif") {
        acGetCellomicsName(*dataID,*CName);
        acLog(*CName);}
}

# creates a renamed copy of a Leica image file
acLeica2CellomicsCopyFile(*Object) {
    msiIsData(*Object,*dataID,*foo);
    msiGetObjectPath(*Object,*Path,*foo);
    if (*Path like "\*.tif") {
        acGetCellomicsObj(*dataID,*CObject);
        #acLog("Copying to Cellomics "++*Object++" "++*CObject);
        msiDataObjCopy(*Object,*CObject,"null",*cstatus);
    }
}

# creates a renamed copy of a Leica experiment--* folder
acLeica2CellomicsCopyColl(*Coll) {
   *Work = ``{
        acLeica2CellomicsCopyFile(*Objects );
    }``;
    msiCollectionSpider(*Coll,*Objects,*Work,*status);
}

# performs the entire conversion
acLeica2Cellomics(*coll) {
    acLeica2CellomicsAddMetadata(*coll,*res);
    acLeica2CellomicsCopyColl(*coll);}
   
