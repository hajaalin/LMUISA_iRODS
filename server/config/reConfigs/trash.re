#
# This file contains various rules needed to implement the trash policy.
#

# Define the time how long a data object can stay in trash before expiring.
# 1 hour = 60 x 60s = 3600s
# 1 day = 24 x 3600s = 86400s
# 1 month = 31 x 86400s = 2678400s
# 3 months = 3 x 2678400s = 8035200s
# 6 months = 6 x 2678400s = 16070400s 
acGetMaxTrashTime(*maxTrashTime) {*maxTrashTime = "120";}

# Creates timestamps that show when data was put in trash and when it will expire
acCreateTrashTimestamps(*KVPairs) {
    acGetMaxTrashTime(*MAXTRASHTIME);
    msiGetIcatTime(*trashEntry,"unix");
    msiGetIcatTime(*trashEntryH,"human");
    *trashExpiry = int(*trashEntry) + int(*MAXTRASHTIME);
    *trashExpiry = str(*trashExpiry);
    msiSystemToHumanTime(*trashExpiry,*trashExpiryH);
    msiAddKeyVal(*KVPairs,"DATA_TRASH_TIME",*trashEntry);
    msiAddKeyVal(*KVPairs,"DATA_TRASH_TIME_H",*trashEntryH);
    msiAddKeyVal(*KVPairs,"DATA_TRASH_EXPIRY",*trashExpiry);
    msiAddKeyVal(*KVPairs,"DATA_TRASH_EXPIRY_H",*trashExpiryH);
}

# get the TRASHTIME of an object in the trashbin
acGetTrashTime(*dataID,*trashtime) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'DATA_TRASH_TIME' AND DATA_ID = '*dataID'",*trashtime);}
acGetTrashExpiry(*dataID,*trashtime) {
    acGetValueForDataAttribute("META_DATA_ATTR_NAME = 'DATA_TRASH_EXPIRY' AND DATA_ID = '*dataID'",*trashtime);}

acSetTrashTimestamps(*destObject) {
    acCreateTrashTimestamps(*KVPairs);
    acLog("acSetTrashTimestamps 1");
    msiIsColl(*destObject,*collID,*status);
    acLog("acSetTrashTimestamps 2 collID "++str(*collID)++" status "++str(*status));
    if (*collID > 0) {
        acLog("acSetTrashTimestamps 3 on collection "++*destObject);
        acAssociateTrashTimeStampsToColl(*destObject); 
        #acAssociateKeyValuePairsToColl(*KVPairs,*destObject); 
        acLog("acSetTrashTimestamps 4");}        
    else {
        acLog("acSetTrashTimestamps 5 on dataobj "++*destObject);
        msiSetKeyValuePairsToObj(*KVPairs,*destObject,"-d");
        acLog("acSetTrashTimestamps 7");}
}

acAssociateTrashTimeStampsToCollFuturistic(*Coll) {
    foreach(*obj in collection(*Coll)) {
        acLog("jee");
    }
}

acAssociateTrashTimeStampsToColl(*Coll) {
    acCreateTrashTimestamps(*KVPairs);
    msiSetKeyValuePairsToObj(*KVPairs,*Coll,"-C");

    *Work = ``{
        msiIsData(*Objects,*dataID,*foo);
        if(int(*dataID) > 0) {
            acCreateTrashTimestamps(*KVPairs);
            msiGetObjectPath(*Objects,*objPath,*bar);
            msiSetKeyValuePairsToObj(*KVPairs,*objPath,"-d");
        }
    }``;
    
    msiCollectionSpider(*Coll,*Objects,*Work,*status);
}

acAssociateKeyValuePairsToColl(*KVPairs,*Coll) {
   msiAssociateKeyValuePairsToObj(*KVPairs,*Coll,"-C");

   *Work2 = ``{
       msiCopyAVUMetadata(``++"'*Coll'"++``,*Objects,*Status);
   }``;

   msiCollectionSpider(*Coll,*Objects,*Work2,*status);
}

# Checks if a data object in trash is expired
acTrashExpired(*obj,*expired) {
    acLog("acTrashExpired: "++*obj);
    msiIsData(*obj,*dataID,*foo);
    acLog("acTrashExpired: "++*dataID);
    if (int(*dataID) == 0) {
        *expired = "NO";
    }
    else {
        acGetTrashExpiry(*dataID,*trashExpiry);
        msiGetIcatTime(*icatTime,"unix");
        acLog("acTrashExpired: "++*icatTime++" "++*trashExpiry);
        if (int(*icatTime) > int(*trashExpiry) ) {
            *expired = "YES";} 
        else {
            *expired = "NO";}
    }
}

acSetTrashACL(*destObject) {
    msiIsColl(*destObject,*collID,*status);
    if (*collID > 0) {
        acLog("acSetTrashACL: on collection "++*destObject);
        msiSetACL("recursive","inherit","lmu-staff",*destObject):::acLog("acSetTrashACL:fail1");
        msiSetACL("recursive","own","lmu-staff",*destObject):::acLog("acSetTrashACL:fail2");
        msiSetACL("recursive","inherit",$userNameClient,*destObject):::acLog("acSetTrashACL:fail3");
        msiSetACL("recursive","read",$userNameClient,*destObject):::acLog("acSetTrashACL:fail4");}
    else {
        acLog("acSetTrashACL: on dataobj "++*destObject);
        msiSetACL("default","own","lmu-staff",*destObject):::acLog("acSetTrashACL:fail5");
        msiSetACL("default","read",$userNameClient,*destObject):::acLog("acSetTrashACL:fail6");}
}

