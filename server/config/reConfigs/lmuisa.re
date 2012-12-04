
acCreateUserLMUISACollections {
    acLog("acCreateUserLMUISACollections $otherUserName");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"bisque_data");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"tmp");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"trash");
}
acDeleteUserLMUISACollections {
    acLog("acDeleteUserLMUISACollections $otherUserName");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"bisque_data");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"tmp");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"trash");
}
acSetAclForLMUISACollections {
    acLog("acSetAclForLMUISACollections");
    if ($otherUserName != "lmu-staff" && $otherUserName != "lmu-instruments") then {
    acSetAclForData;
    #acSetStaffAclForTrash;
    #acSetUserAclForTrash;
    }
}
    
acSetAclForData {
    acLog("acSetACLForData");
    # staff can read files in user's home collection
    msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName);
    msiSetACL("default","admin:read","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName);
    # instrument accounts can write to user's home collection (maybe needed for batch upload of data from instrument PCs)
    msiSetACL("recursive","admin:inherit","lmu-instruments","/"++$rodsZoneProxy++"/home/"++$otherUserName);
    msiSetACL("default","admin:write","lmu-instruments","/"++$rodsZoneProxy++"/home/"++$otherUserName);    
    msiSetACL("default","admin:read","lmu-instruments","/"++$rodsZoneProxy++"/home/"++$otherUserName);    
    # staff can write to user's tmp folder
    msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/tmp");
    msiSetACL("default","admin:write","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/tmp");
    # the user can read everything in tmp (also files created by staff)
    msiSetACL("recursive","admin:inherit",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/tmp");
    msiSetACL("default","admin:read",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/tmp");
}
#acSetStaffAclForTrash{
    #acLog("acSetStaffACLForTrash");
    #msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/trash"):::acLog("fail1");
    #msiSetACL("default","admin:own","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/trash"):::acLog("fail2");}
#acSetUserAclForTrash{
    #acLog("acSetUserACLForTrash");
    #msiSetACL("recursive","admin:inherit",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/trash");
    #msiSetACL("default","admin:write",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/trash");
    #msiSetACL("default","admin:read",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/trash");}
    


# perform the conversion if the collection is ready
acTrackLeicaColl(*coll) {
    msiIsReadyHcsColl(*coll,*ready,*foo);
    if (*ready == "READY") {
        acLog("acTrackLeicaColl: Collection "++*coll++" ready for conversion.");
        acLeica2Cellomics(*coll);}
    else {
        acLog("acTrackLeicaColl: Collection "++*coll++" not ready.");}
}


# create a scheduled job that observes a Leica experiment--* folder
acCreateTrackerForLeicaColl(*coll) {
    acLog("acCreateTrackerForLeicaColl "++$userNameClient++" "++*coll++" ...");
    delay("<PLUSET>300s</PLUSET><EF>300s DOUBLE 2 TIMES</EF>") {
        acTrackLeicaColl(*coll);}
}




