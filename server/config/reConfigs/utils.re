acLog(*msg) {
    writeLine("serverLog",*msg);}

# get the value of a metadata attribute 
acGetValueForObjectAttribute(*rodsAttribute,*attributeCondition,*attributeValue) {
    msiMakeQuery(*rodsAttribute,*attributeCondition,*attributeQuery);
    msiExecStrCondQuery(*attributeQuery,*queryResults);
    foreach(*queryResults) { 
        msiGetValByKey(*queryResults,*rodsAttribute,*attributeValue);}}
acGetValueForDataAttribute(*attributeCondition,*attributeValue) {
    acGetValueForObjectAttribute("META_DATA_ATTR_VALUE",*attributeCondition,*attributeValue);}
acGetValueForCollectionAttribute(*attributeCondition,*attributeValue) {
    acGetValueForObjectAttribute("META_COLL_ATTR_VALUE",*attributeCondition,*attributeValue);}
acGetValueForResourceAttribute(*attributeCondition,*attributeValue) {
    acGetValueForObjectAttribute("META_RESC_ATTR_VALUE",*attributeCondition,*attributeValue);}
acGetValueForUserAttribute(*attributeCondition,*attributeValue) {
    acGetValueForObjectAttribute("META_USER_ATTR_VALUE",*attributeCondition,*attributeValue);}

    
acGetCollNameForDataID(*dataID,*collName) {
    acGetValueForObjectAttribute("COLL_NAME","DATA_ID = '"++*dataID++"'",*collName);}
    
acGetDataNameForDataID(*dataID,*dataName) {
    acGetValueForObjectAttribute("DATA_NAME","DATA_ID = '*dataID'",*dataName);}

acGetObjPathForDataID(*dataID,*objPath) {
    acGetCollNameForDataID(*dataID,*collName);
    acGetDataNameForDataID(*dataID,*dataName);
    *objPath = *collName++"/"++*dataName;}

acGetDataPathForDataID(*dataID,*dataPath) {
    acGetValueForObjectAttribute("DATA_PATH","DATA_ID = '*dataID'",*dataPath);}
    
acGetCreateTimeForDataID(*dataID,*createTime) {
    acGetValueForObjectAttribute("DATA_CREATE_TIME","DATA_ID = '*dataID'",*createTime);}


# writes to a log file stored in iRODS
acWriteToMyLog(*Buf) {
    msiDataObjOpen("objPath=/LMUISA_b2/home/hajaalin/test/log.txt++++openFlags=O_WRONLY",*FD_log);
    msiDataObjWrite(*FD_log,*Buf,*foo);
    msiDataObjClose(*FD_log,*foo);}

# modifies file permissions
acModPhysPerm(*filePath) {
    #acLog("adModPhysPerm *filePath");
    msiExecCmd("changeGroupPermissionsRecursively.py","-n 7 "++*filePath,"null","null","null","null");}

# recursively modifies file permissions of a  collection
acModPhysPermColl(*Coll,*Resc,*status) {
    acLog("acModPhysPermColl: "++*Coll++" "++*Resc);
    
    *rodsAttribute = "DATA_PATH";
    msiMakeQuery(*rodsAttribute,"COLL_NAME like '*Coll%' AND RESC_NAME = '*Resc'",*attributeQuery);
    msiExecStrCondQuery(*attributeQuery,*queryResults);
    foreach(*queryResults) { 
        msiGetValByKey(*queryResults,*rodsAttribute,*dataPath);
        acLog("acModPhysPermColl: *dataPath");
        acModPhysPerm(*dataPath);
    }

    acLog("acModPhysPermColl: Done");
}


acEcho(*in,*out) {
    msiExecCmd("echo.sh",*in,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*out);
    acLog(*out);}

acBasename(*filename,*basename) {
    msiExecCmd("basename.py",*filename,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    msiGetStderrInExecCmdOut(*execCmdOut,*tmp2);
    msiStrchop(*tmp,*basename);
    acLog("acBasename:basename: "++*basename);
    acLog("acBasename:stderr"++*tmp2);
}

acUnixBasename(*filename,*basename) {
    msiExecCmd("basename",*filename,"null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    msiStrchop(*tmp,*basename);
}

acRandom(*random) {
    msiExecCmd("random.py","null","null","null","null",*execCmdOut);
    msiGetStdoutInExecCmdOut(*execCmdOut,*tmp);
    msiStrchop(*tmp,*random);
}

acGetVaultPath(*Resc, *VaultPath) {
    *rodsAttribute = "RESC_VAULT_PATH"
    msiMakeQuery(*rodsAttribute,"RESC_NAME = '*Resc'",*attributeQuery);
    msiExecStrCondQuery(*attributeQuery,*queryResults);
    foreach(*queryResults) { 
        msiGetValByKey(*queryResults,*rodsAttribute,*VaultPath);}
}
