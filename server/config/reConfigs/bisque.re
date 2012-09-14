# bisque.re
# Bisque related rules for > iRods 3.0
# put this in server/config/reConfigs/bisque.re
# add this file to  reRuleSet directive of iRODS/server/config/server.config (comma separated, no space )
# reRuleSet core,bisque
###########################
# NOTE:
# CHANGE "winwood.iplantcollaborative.org" to the irods server host name
###############################################

acPostProcForCollCreate {
  ON ($collName like "/LMUISA1_test/home/$userNameClient/bisque_data\*") {
        writeLine("serverLog","BISQUE: permitting bisque user RW on"++$collName);
        msiSetACL ('default', 'write', 'bisque', $collName);
        msiSetACL ('recursive', 'inherit', 'null', $collName);
    }
}

acPostProcForPut {
    # default tags for testing
    *tags = '<tag name="description" value="some text"';
    
    if ($objPath like regex ".*/experiment--[0-9]{4}_[0-9]{2}_[0-9]{2}_[0-9]{2}_[0-9]{2}_[0-9]{2}/.*")  {
        acSetMatrixScreenerPlateData($objPath,*res);
        
        # get real AVUs of the object, make into XML
    }

    if ($userNameClient != "bisque" && $objPath like "/LMUISA1_test/home/\*/bisque_data/\*") {
    
    
       writeLine("serverLog","BISQUE: inserting object"++$objPath);
#       delay("<PLUSET>1s</PLUSET><EF>1s REPEAT UNTIL SUCCESS</EF>") {
       delay("<PLUSET>1s</PLUSET>") {
         msiExecCmd("insert2bisque.py", '$objPath $userNameClient', "lmu-omero1.biocenter.helsinki.fi", "null", "null", *cmdOut);
         writeLine("serverLog","BISQUE: inserted object"++$objPath);

       }
    }
}

