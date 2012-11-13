#
# This file contains rules needed to convert a Leica Matrix Screener output directory tree
# to one directory containing all the data. Individual channels and z-slices are collected
# into multi-channel stacks. The naming of the stacks reflects the well and field location.
#

# run a embedPython microservice to add metadata into Leica experiment--* folder (*inputDir)
# this already defines where the renamed copy will be created (*outputRoot)
acLeica2FlatDirAddMetadata(*coll,*res) {
    *pyScript = "/opt/iRODS/iRODS_3.0/server/bin/cmd/embedPython/lmuisa.py";
    *methName = "msiLeica2FlatDir";
    *outputRoot =  "/"++$rodsZoneClient++"/home/"++$userNameClient++"/Conversions";
    
    msiPyInitialize;
    msiLocalPython3(*pyScript,*methName,"noRecursionTest",*coll,*outputRoot,*res);
    msiPyFinalize;
}

# helper rules to get metadata values related to Leica -> flat drirectory conversion
acGetFlatDirStackName(*dataID,*name) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'leica2flatdir.stackName' AND DATA_ID = '*dataID'",*name);}
acGetFlatDirColl(*dataID,*name) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'leica2flatdir.collection' AND DATA_ID = '*dataID'",*name);}
acGetFlatDirDataObj(*dataID,*name) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'leica2flatdir.dataObject' AND DATA_ID = '*dataID'",*name);}

# creates a renamed copy of a Leica image file
acLeica2FlatDirStitchStack(*Object) {
    msiIsData(*Object,*dataID,*foo);
    if(int(*dataID) > 0) {
        # get the input object path in string format
        msiGetObjectPath(*Object,*leicaObjPath,*foo);
        #acLog("acLeica2FlatDirStitchStack: found data "++*leicaObjPath);
    
        # Only run the conversion for one file in the folder containing the stack.
        # This will automatically include all others.
        if (*leicaObjPath like "\*Z00--C00.ome.tif") {
            # get the name of the converted object from metadata
            acGetFlatDirDataObj(*dataID,*stackObjPath);
            acLog("acLeica2FlatDirStitchStack: 0 "++*dataID );
            acLog("acLeica2FlatDirStitchStack: 0.5 "++*dataID++" "++*stackObjPath );
        
            # 
            # Find the physical file paths
            acGetVaultPath("li1-tike1", *vault);
            acLog("acLeica2FlatDirStitchStack: 1"++*leicaObjPath++" "++$rodsZoneProxy++" "++*vault );
            msiStrReplace(*leicaObjPath,$rodsZoneProxy,*vault,1,*leicaFilePath);
            acLog("acLeica2FlatDirStitchStack: 2"++*stackObjPath++" "++$rodsZoneProxy++" "++*vault );
            msiStrReplace(*stackObjPath,$rodsZoneProxy,*vault,1,*stackFilePath);
            acLog("acLeica2FlatDirStitchStack: 3"++*leicaFilePath );
            acLog("acLeica2FlatDirStitchStack: 4"++*stackFilePath );
            
            # determine output collection and directory
            msiSplitPath(*stackObjPath,*outColl,*outObj);
            msiSplitPath(*stackFilePath,*outDir,*outFile);
            # create output collection and directory
            msiCollCreate(*outColl,"1",*status);
            msiExecCmd("mkdir","-p *outDir","null","null","null",*execCmdOut);

            
            msiExecCmd("bfconvert","-overwrite -merge -stitch *leicaFilePath *stackFilePath","null","null","null",*execCmdOut);
            msiPhyPathReg(*stackObjPath,"li1-tike1",*stackFilePath,"null",*status);        
        }
    }
}

# creates a renamed copy of a Leica experiment--* folder
acLeica2FlatDirCopyColl(*Coll) {
   *Work = ``{
        acLeica2FlatDirStitchStack(*Objects );
    }``;
    msiCollectionSpider(*Coll,*Objects,*Work,*status);
}

# performs the entire conversion
acLeica2FlatDir(*coll) {
    acLeica2FlatDirAddMetadata(*coll,*res);
    acLeica2FlatDirCopyColl(*coll);
}
 
