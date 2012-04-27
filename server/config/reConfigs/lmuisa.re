
acCreateUserLMUISACollections {
    acLog("acCreateUserLMUISACollections");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Data");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Conversions");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Results");
    acCreateCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Trash");}
acDeleteUserLMUISACollections {
    acLog("acDeleteUserLMUISACollections1");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Data");
    acLog("acDeleteUserLMUISACollections2");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Conversions");
    acLog("acDeleteUserLMUISACollections3");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Results");
    #msiSetACL("default","admin:own",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash")
    #msiSetACL("default","admin:null","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash")
    acLog("acDeleteUserLMUISACollections4");
    acDeleteCollByAdmin("/"++$rodsZoneProxy++"/home/"++$otherUserName,"Trash");}

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
    # staff can read files in user's Data
    msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Data");
    msiSetACL("default","admin:read","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Data");
    # instrument accounts can write to user's Data
    msiSetACL("recursive","admin:inherit","lmu-instruments","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Data");
    msiSetACL("default","admin:write","lmu-instruments","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Data");    
    msiSetACL("default","admin:read","lmu-instruments","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Data");    
    # staff can read files in user's Conversions
    msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Conversions");
    msiSetACL("default","admin:read","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Conversions");
    # staff can write to user's Results folder
    msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Results");
    msiSetACL("default","admin:write","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Results");
    # the user can read everythins in Results (also files created by staff)
    msiSetACL("recursive","admin:inherit",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Results");
    msiSetACL("default","admin:read",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Results");
}
acSetStaffAclForTrash{
    acLog("acSetStaffACLForTrash");
    msiSetACL("recursive","admin:inherit","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash"):::acLog("fail1");
    msiSetACL("default","admin:own","lmu-staff","/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash"):::acLog("fail2");}
acSetUserAclForTrash{
    acLog("acSetUserACLForTrash");
    msiSetACL("recursive","admin:inherit",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash");
    msiSetACL("default","admin:write",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash");
    msiSetACL("default","admin:read",$otherUserName,"/"++$rodsZoneProxy++"/home/"++$otherUserName++"/Trash");}
    


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




