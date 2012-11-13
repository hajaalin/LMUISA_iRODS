acCellomics2Tif(*objPath,*filePath) {
    *dataColl = "/"++$rodsZoneProxy++"/home/"++$userNameClient++"/Data";
    *cellDataColl = "/"++$rodsZoneProxy++"/home/"++$userNameClient++"/CellData";
    *convColl = "/"++$rodsZoneProxy++"/home/"++$userNameClient++"/Conversions";

    acGetVaultPath("li1-tike1", *vault);
    *dataDir = *vault++"/home/"++$userNameClient++"/Data";
    *cellDataDir = *vault++"/home/"++$userNameClient++"/CellData";
    *convDir = *vault++"/home/"++$userNameClient++"/Conversions";
    
    if(*objPath like *dataColl++"/*") then {
        msiStrReplace(*objPath,*dataColl,*convColl,1,*tmp);
        *outObjPath = *tmp
        msiStrReplace(*filePath,*dataDir,*convDir,1,*tmp2);
        *outFilePath = *tmp2
    } else if (*objPath like *cellDataColl++"/*") then {
        msiStrReplace(*objPath,*cellDataColl,*convColl,1,*tmp5);
        *outObjPath = *tmp5
        msiStrReplace(*filePath,*cellDataDir,*convDir,1,*tmp6);
        *outFilePath = *tmp6
    }

    #acLog("acConvertToTiff:2 *outObjPath, *outFilePath");

    msiStrReplace(*outObjPath,".C01",".tif",1,*tmp3);
    *outObjPath = *tmp3
    msiStrReplace(*outFilePath,".C01",".tif",1,*tmp4);
    *outFilePath = *tmp4
   
    # remove previous conversion
    #msiDataObjUnlink("objPath=*outObjPath",*foo);
    #acLog("acConvertToTiff:3 *outObjPath, *outFilePath");
    
    # determine output collection and directory
    msiSplitPath(*outObjPath,*outColl,*outObj);
    msiSplitPath(*outFilePath,*outDir,*outFile);

    #acLog("acConvertToTiff:5 *outColl, *outDir");
    
    # create output collection and directory
    msiCollCreate(*outColl,"1",*status);
    msiExecCmd("mkdir","-p *outDir","null","null","null",*execCmdOut);
    
    acLog("acCellomics2Tif: "++"bfconvert "++"*filePath *outFilePath");
    msiExecCmd("bfconvert","-nogroup -overwrite *filePath *outFilePath","null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    msiGetStderrInExecCmdOut(*execCmdOut,*tmp2);    
    #acLog("acConvertToTiff:bfconvert: "++*tmp);
    #acLog("acConvertToTiff:bfconvert:stderr"++*tmp2);
   
    # register the converted file on resource "li1-tike1"
    msiPhyPathReg(*outObjPath,"li1-tike1",*outFilePath,"null",*status);
}

acCellomics2TifColl(*Coll,*status) {
    acLog("acCellomics2TifColl: *Coll");
   *Work = ``{
       msiIsData(*Objects,*dataID,*foo);
        acGetObjPathForDataID(*dataID,*objPath);
        #acGetDataPathForDataID(*dataID,*dataPath);
        
        # find physical path of replicate on resource "li1-tike1"
        *rodsAttribute = "DATA_PATH"
        *resc = "li1-tike1";
        msiMakeQuery(*rodsAttribute,"DATA_ID = '*dataID' AND RESC_NAME = '*resc'",*attributeQuery);
        msiExecStrCondQuery(*attributeQuery,*queryResults);
        foreach(*queryResults) { 
            msiGetValByKey(*queryResults,*rodsAttribute,*dataPath);}

        if (*objPath like "*.C01") {
            acConvertToTiff(*objPath,*dataPath)
        }
    }``;

    msiCollectionSpider(*Coll,*Objects,*Work,*status);
}
