/**
 * @file HCSInfoMS.c
 *
 */

/*** Copyright (c), University of Helsinki            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

#include "URL.h"
#include <curl/curl.h>

int 
msiUrlOpen(msParam_t *inUrl, msParam_t *inData, msParam_t *inAuth, msParam_t *outRes, ruleExecInfo_t *rei) 
{
    char *url;
    char *data;
    char *auth;
    char curlErrBuf[CURL_ERROR_SIZE];
    int status;
    char *logFilename;
    FILE *logFd;

    CURL *curl;
    CURLcode res;
    
    char get[] = "GET";
    char *getPtr = get;
    
    /*  check for input parameters */
    if (inUrl ==  NULL || 
        strcmp(inUrl->type , STR_MS_T) != 0 || 
        inUrl->inOutStruct == NULL)
        return(USER_PARAM_TYPE_ERR);

    if (inData ==  NULL || 
        strcmp(inData->type , STR_MS_T) != 0 || 
        inData->inOutStruct == NULL)
        return(USER_PARAM_TYPE_ERR);

    if (inAuth ==  NULL || 
        strcmp(inAuth->type , STR_MS_T) != 0 || 
        inAuth->inOutStruct == NULL)
        return(USER_PARAM_TYPE_ERR);

    if (outRes ==  NULL || 
        strcmp(outRes->type , STR_MS_T) != 0 || 
        outRes->inOutStruct == NULL)
        return(USER_PARAM_TYPE_ERR);

    url = (char *) inUrl->inOutStruct;
    data = (char *) inData->inOutStruct;
    auth = (char *) inAuth->inOutStruct;
    logFilename = (char *) outRes->inOutStruct;
        
    printf("msiUrlOpen: url: %s\n", url);
    printf("msiUrlOpen: data: %s\n", data);
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug1 %s\n", url);
    //return 0;
    

    /* Do the processing */
    /* opening file and passing i to curl */
    logFd = fopen (logFilename, "w");
    if (logFd ==  0) {
        rodsLog (LOG_ERROR, "msiUrlOpen: open error for logFilename %s", logFilename);
        free(url); free(data); free(auth); free(logFilename);
        return status;
    }

    printf("CURL: Calling with %s\n", url);
    curl = curl_easy_init();
    if(!curl) {
        rodsLog (LOG_ERROR, "msiUrlOpen: Curl Error: Initialization failed\n");
        free(url); free(data); free(auth); free(logFilename);
       return(MSO_OBJ_GET_FAILED);
    }
    
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug2\n");

    curl_easy_setopt(curl, CURLOPT_VERBOSE,true);
    curl_easy_setopt(curl, CURLOPT_STDERR,logFd);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER,curlErrBuf);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug3\n");
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, data);
    
    // Bisque wants to get the data in a GET call, so we need to customize the request:
    // http://curl.haxx.se/mail/lib-2007-12/0099.html
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, getPtr);
    
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug4\n");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, logFd);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, auth);
    
    rodsLog (LOG_NOTICE, "msiUrlOpen: before curl_easy_perform\n %s", url);
    res = curl_easy_perform(curl);
    rodsLog (LOG_NOTICE, "msiUrlOpen: after curl_easy_perform\n %s", url);
    
    if (res != 0) {
        rodsLog (LOG_ERROR, "Curl Error for %s:ErrNum=%i, Msg=%s\n",url,res,curlErrBuf);
        curl_easy_cleanup(curl);
        free(url); free(data); free(auth); free(logFilename);
        return(MSO_OBJ_GET_FAILED);
    }
    curl_easy_cleanup(curl);
    fclose(logFd);
 
    rodsLog (LOG_NOTICE, "msiUrlOpen: CURL: success with %s\n", url);
    
    // Apparently curl_easy_cleanup)curl) takes care of these, if this line is left in place, there will be a crash with "double free".
    //free(url); free(data); free(auth); free(logFilename);

    /* TODO: test if all of these are really freed in curl_easy_cleanup.
    free(url); 
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug5\n");
    free(data); 
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug6\n");
    free(auth); 
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug7\n");
    free(logFilename);
    rodsLog (LOG_NOTICE, "msiUrlOpen: debug8\n");
    */
    return(0);
}
