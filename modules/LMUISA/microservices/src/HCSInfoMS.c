/**
 * @file HCSInfoMS.c
 *
 */

/*** Copyright (c), University of Helsinki            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

#include "HCSInfoMS.h"
#include "math.h" // 20111020


/**
 * \fn msiIsReadyHcsColl(msParam_t *targetPath, msParam_t *collId, msParam_t *status, ruleExecInfo_t *rei)
 *
 * \brief Checks if it seems that all files from a HCS imaging session have been uploaded.
 *
 * \module LMUISA
 *
 * \since 2.1
 *
 * \author  Harri J채채linoja
 * \date    2011-07-06
 *
 * \remark 
 *
 * \note This microservice takes an iRODS path and returns the corresponding collection ID,
 *    or zero if the object is not a collection or does not exist.
 *    Avoid path names ending with '/' as they can be misparsed by lower level routines
 *    (eg: use /tempZone/home instead of /tempZone/home/).
 *
 * \usage
 * As seen in modules/ERA/test/isColl.ir
 * 
 * testrule||msiIsColl(*collPath, *collID, *Status)##writePosInt(stdout, *collID)##writeLine(stdout, "")|nop
 * *collPath=$1
 * ruleExecOut
 *
 * \param[in] targetPath - An msParam_t of any type whose inOutStruct is a string (the object's path).
 * \param[out] collId - an INT_MS_T containing the collection ID.
 * \param[out] status - an INT_MS_T containing the operation status.
 * \param[in,out] rei - The RuleExecInfo structure that is automatically
 *    handled by the rule engine. The user does not include rei as a
 *    parameter in the rule invocation.
 *
 * \DolVarDependence None
 * \DolVarModified None
 * \iCatAttrDependence None
 * \iCatAttrModified None
 * \sideeffect None
 *
 * \return integer
 * \retval 0 on success
 * \pre None
 * \post None
 * \sa None
 * \bug  no known bugs
**/
int
msiIsReadyHcsColl(msParam_t *collPath, msParam_t *collReady, msParam_t *status, ruleExecInfo_t *rei)
{
    RE_TEST_MACRO ("    Calling msiIsReadyHcsColl")

	char *collPathStr;				/* for parsing input path */
	collInp_t collInpCache, *outCollInp;	/* for parsing collection input param */
	keyValPair_t *res;			/* for passing out results */
	
    char collQCond[2*MAX_NAME_LEN];		/* for condition in rsGenquery() */
    char condStr[MAX_NAME_LEN];
	genQueryInp_t genQueryInp;		/* for query inputs */
	genQueryOut_t *genQueryOut;		/* for query results */

    rodsLong_t ctime,mtime,ctimeLatest;		/* to store CREATE_TIME */
    time_t now;
    rodsLong_t aveWriteTime;
    char dname[MAX_NAME_LEN];
    char ctimeL[30]; // 20111020
    
    char *resultStringToken;		/* for parsing key-value pairs from genQueryOut */
	char *oldValStr, newValStr[21];		/* for parsing key-value pairs from genQueryOut */
	rodsLong_t newVal;			/* for parsing key-value pairs from genQueryOut */
	sqlResult_t *sqlResult;			/* for parsing key-value pairs from genQueryOut */
	int i;					/* for parsing key-value pairs from genQueryOut */

	/* For testing mode when used with irule --test */
	RE_TEST_MACRO ("    Calling msiIsReadyHcsColl")
	
	
	/* Sanity test */
	if (rei == NULL || rei->rsComm == NULL) {
			rodsLog (LOG_ERROR, "msiIsColl: input rei or rsComm is NULL.");
			return (SYS_INTERNAL_NULL_INPUT_ERR);
	}
	
	
	/* Check for NULL input */
    if (!collPath) 
    {
    	rei->status = USER__NULL_INPUT_ERR;
		rodsLog (LOG_ERROR, "msiIsColl: input collPath error. status = %d", rei->status);
		return (rei->status);
    }
    
    
    /* parse inpParam1: our target collection */
	rei->status = parseMspForCollInp (collPath, &collInpCache, &outCollInp, 0);
	
	if (rei->status < 0) {
		rodsLog (LOG_ERROR, "msiIsReadyHcsColl: input collPath error. status = %d", rei->status);
		return (rei->status);
	}

    
    /* allocate memory for our result struct */
	res = (keyValPair_t*)malloc(sizeof(keyValPair_t));
	memset (res, 0, sizeof (keyValPair_t));


    
	/* Wanted fields. We use coll_id to do a join query on r_data_main and r_coll_main */
	memset (&genQueryInp, 0, sizeof (genQueryInp));
	addInxIval (&genQueryInp.selectInp, COL_D_CREATE_TIME, ORDER_BY_DESC);
	addInxIval (&genQueryInp.selectInp, COL_D_MODIFY_TIME, ORDER_BY_DESC);
	addInxIval (&genQueryInp.selectInp, COL_DATA_NAME, 1);

    
	/* Make condition for getting all objects under a collection */
	genAllInCollQCond (outCollInp->collName, collQCond);
	addInxVal (&genQueryInp.sqlCondInp, COL_COLL_NAME, collQCond);
    
    //snprintf (condStr, LONG_NAME_LEN, "like '%s'", "*.tif");
    //addInxVal (&genQueryInp.sqlCondInp, COL_DATA_NAME, condStr);

	genQueryInp.maxRows = 100;

    
    /* Query */
	rei->status  = rsGenQuery (rei->rsComm, &genQueryInp, &genQueryOut);


	/* Zero counters, in case no rows found */
	ctime = 0;
    ctimeLatest = 0;
    aveWriteTime = 0;
    mtime = 0;
    now = time(&now);

	/* Parse results. */
	if (rei->status == 0) {
	
		/* for each row */
		for (i=0;i<genQueryOut->rowCnt;i++) {
	
            /* get COL_D_CREATE_TIME result */
            sqlResult = getSqlResultByInx (genQueryOut, COL_D_CREATE_TIME);
            ctime = atoll(sqlResult->value + i*sqlResult->len);
            if (i==0) {
                /* The latest time comes first because the results are sorted that way*/
                ctimeLatest = ctime;
            }
            aveWriteTime = (rodsLong_t)ceil(((double)ctimeLatest - (double)ctime) / (i + 1));
            getLocalTimeFromRodsTime(sqlResult->value + i*sqlResult->len,ctimeL);
		
            /* get COL_D_MODIFY_TIME result */
            sqlResult = getSqlResultByInx (genQueryOut, COL_D_MODIFY_TIME);
            mtime = atoll(sqlResult->value + i*sqlResult->len);
		
            /* get COL_DATA_NAME result */
            sqlResult = getSqlResultByInx (genQueryOut, COL_DATA_NAME);
            strncpy(dname,sqlResult->value + i*sqlResult->len,sizeof(dname));

            //rodsLog (LOG_NOTICE, "msiIsReadyHcsColl: dname = %s, ctime = %d (%s), ctime0 = %d, aveWriteTime = %f, now = %d",dname,ctime,ctimeL,ctimeLatest,aveWriteTime, now);
        }
        
        rodsLog (LOG_NOTICE, "msiIsReadyHcsColl: ctime = %d (%s), ctimeLatest = %d, aveWriteTime = %d, now = %d",ctime,ctimeL,ctimeLatest,aveWriteTime, now);

        /* Rule to decide if the collection might be complete*/
        rodsLog (LOG_NOTICE, "msiIsReadyHcsColl: %d > %d + 10 * %d",now,ctimeLatest,aveWriteTime);
        rodsLog (LOG_NOTICE, "msiIsReadyHcsColl: %d > %d",now,ctimeLatest + 10*aveWriteTime);
        if (now > ctimeLatest + 10 * aveWriteTime) {
            rodsLog (LOG_NOTICE, "msiIsReadyHcsColl: READY");
            fillStrInMsParam (collReady, "READY");
       } else {
            rodsLog (LOG_NOTICE, "msiIsReadyHcsColl: NOTREADY");
            fillStrInMsParam (collReady, "NOTREADY");
       }
            
    }
    
	/* Return collection ID and operation status */
	fillIntInMsParam (status, rei->status);

	/* Done */
	return rei->status;
}


int
msiIsReadyHcsCollRandom(msParam_t *collPath, msParam_t *collReady, msParam_t *status, ruleExecInfo_t *rei)
{
    RE_TEST_MACRO ("    Calling msiIsReadyHcsCollRandom")

   int random = rand();
    double d = (double)random/RAND_MAX;
    rodsLog(LOG_NOTICE,"msiIsReadyHcsCollRandom: %d, %d,%lf", random, RAND_MAX, d);
    if (d > 0.9) {
        rodsLog (LOG_NOTICE, "msiIsReadyHcsCollRandom: READY");
        fillStrInMsParam (collReady, "READY");
   } else {
        rodsLog (LOG_NOTICE, "msiIsReadyHcsCollRandom: NOTREADY");
        fillStrInMsParam (collReady, "NOTREADY");
   }

   /* Return collection ID and operation status */
	fillIntInMsParam (status, rei->status);


    return rei->status;
}

int 
msiWriteOpenDisallowed(ruleExecInfo_t *rei)
{
    RE_TEST_MACRO ("    Calling msiWriteOpenDisallowed")

    rei->status = FILE_WRITE_ERR;

    return (rei->status);
}


int 
msiStrReplace(msParam_t *in, msParam_t *find, msParam_t *replace, msParam_t *count, msParam_t *out, ruleExecInfo_t *rei)
{
 	char *inStr;				/* for parsing input string */
 	char *findStr;				/* for parsing string to find*/
 	char *replaceStr;				/* for parsing string to replace with */
    size_t patcnt;       /*For parsing pattern count*/
	
    RE_TEST_MACRO ("    Calling msiStrReplace")

	/* Sanity test */
	if (rei == NULL || rei->rsComm == NULL) {
			rodsLog (LOG_ERROR, "msiStrReplace: input rei or rsComm is NULL.");
			return (SYS_INTERNAL_NULL_INPUT_ERR);
	}
	
	
	/* Check for NULL input */
    if (!in || !find || !replace || ! count || !out) 
    {
    	rei->status = USER__NULL_INPUT_ERR;
		rodsLog (LOG_ERROR, "msiStrReplace: input error. status = %d", rei->status);
		return (rei->status);
    }
    
	
	/* Parse input  'in'*/
	inStr = (char *) in->inOutStruct;
	if ( !inStr || !strlen(inStr) || strlen(inStr) > MAX_NAME_LEN )
	{
		rei->status = USER_INPUT_PATH_ERR;
		rodsLog (LOG_ERROR, "msiStrReplace: input 'in' error. status = %d", rei->status);
		return (rei->status);	
	}
    
	/* Parse input 'find'*/
	findStr = (char *) find->inOutStruct;
	if ( !findStr || !strlen(findStr) || strlen(findStr) > MAX_NAME_LEN )
	{
		rei->status = USER_INPUT_PATH_ERR;
		rodsLog (LOG_ERROR, "msiStrReplace: input 'find' error. status = %d", rei->status);
		return (rei->status);	
	}
    
	/* Parse input 'replace'*/
	replaceStr = (char *) replace->inOutStruct;
//	if ( !replaceStr || !strlen(replaceStr) || strlen(replaceStr) > MAX_NAME_LEN )
	if ( !replaceStr || strlen(replaceStr) > MAX_NAME_LEN )
	{
		rei->status = USER_INPUT_PATH_ERR;
		rodsLog (LOG_ERROR, "msiStrReplace: input 'replace' error. status = %d", rei->status);
		return (rei->status);	
	}

	/* Parse input 'count'*/
	patcnt = (size_t) atoi((const char *)replace->inOutStruct);
//	if ( !patcnt)
//	{
//		rei->status = USER_INPUT_PATH_ERR;
//		rodsLog (LOG_ERROR, "msiStrReplace: input 'count' error. status = %d", rei->status);
//		return (rei->status);	
//	}
    
    size_t const replen = strlen(replaceStr);
    size_t const patlen = strlen(findStr);
    size_t const orilen = strlen(inStr);
    
    const char * oriptr;
    const char * patloc;
    
    // zero count means to replace all
    // find how many times the findStr occurs in the inStr string
    if (patcnt == 0) {
        for ((oriptr = inStr); patloc = strstr(oriptr, findStr); oriptr = patloc + patlen)
        {
            patcnt++;
        }
    }
    
    {
    // allocate memory for the new string
    size_t const retlen = orilen + patcnt * (replen - patlen);
    char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );
    
    if (returned != NULL)
    {
      // copy the inStr string, 
      // replacing all the instances of the findStr
      char * retptr = returned;
      for (oriptr = inStr; patloc = strstr(oriptr, findStr); oriptr = patloc + patlen)
      {
        size_t const skplen = patloc - oriptr;
        // copy the section until the occurence of the findStr
        strncpy(retptr, oriptr, skplen);
        retptr += skplen;
        // copy the replaceStr 
        strncpy(retptr, replaceStr, replen);
        retptr += replen;
      }
      // copy the rest of the string.
      strcpy(retptr, oriptr);
    }
    //return returned;


    fillStrInMsParam (out, returned);

    rei->status = 0;

    return (rei->status);
    }
}

int 
msiSystemToHumanTime(msParam_t *epoch, msParam_t *human, ruleExecInfo_t *rei)
{
 	time_t epochTime;				/* for parsing input string */
 	char *humanStr;				/* for output string*/

    RE_TEST_MACRO ("    Calling msiSystemToHumanTime")

	/* Sanity test */
	if (rei == NULL || rei->rsComm == NULL) {
			rodsLog (LOG_ERROR, "msiSystemToHumanTime: input rei or rsComm is NULL.");
			return (SYS_INTERNAL_NULL_INPUT_ERR);
	}
	
	
	/* Check for NULL input */
    if (!epoch || !human) 
    {
    	rei->status = USER__NULL_INPUT_ERR;
		rodsLog (LOG_ERROR, "msiSystemToHumanTime: input error. status = %d", rei->status);
		return (rei->status);
    }
    
	
	/* Parse input  'in'*/
	epochTime = (time_t ) atoi((const char *)epoch->inOutStruct);
//	if ( !epochStr || !strlen(epochStr) || strlen(epochStr) > MAX_NAME_LEN )
//	{
//		rei->status = USER_INPUT_PATH_ERR;
//		rodsLog (LOG_ERROR, "msiSystemToHumanTime: input 'in' error. status = %d", rei->status);
//		return (rei->status);	
//	}
    
    //time_t     now;
    struct tm  ts;
    char       buf[80];

    // Get current time
    //time(&now);

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&epochTime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

    fillStrInMsParam (human, buf);
    //printf("%s\n", buf);

    rei->status = 0;

    return (rei->status);

}

//int 
//msiDirBasename(msParam_t *in, msParam_t *dir, msParam_t *basename, ruleExecInfo_t *rei)
//{
// 	char *inStr;				/* for parsing input string */
//	
//    RE_TEST_MACRO ("    Calling msiDirBasename")
//
//	/* Sanity test */
//	if (rei == NULL || rei->rsComm == NULL) {
//			rodsLog (LOG_ERROR, "msiDirBasename: input rei or rsComm is NULL.");
//			return (SYS_INTERNAL_NULL_INPUT_ERR);
//	}
//	
//	
//	/* Check for NULL input */
//    if (!in || ! dir || !basename) 
//    {
//    	rei->status = USER__NULL_INPUT_ERR;
//		rodsLog (LOG_ERROR, "msiDirBasename: input error. status = %d", rei->status);
//		return (rei->status);
//    }
//    
//	
//	/* Parse input  'in'*/
//	inStr = (char *) in->inOutStruct;
//	if ( !inStr || !strlen(inStr) || strlen(inStr) > MAX_NAME_LEN )
//	{
//		rei->status = USER_INPUT_PATH_ERR;
//		rodsLog (LOG_ERROR, "msiDirBasename: input 'in' error. status = %d", rei->status);
//		return (rei->status);	
//	}
//        
//    size_t const orilen = strlen(inStr);
//    
//    const char * oriptr;
//    const char * patloc;
//    
//    // zero count means to replace all
//    // find how many times the findStr occurs in the inStr string
//    if (patcnt == 0) {
//        for ((oriptr = inStr); patloc = strstr(oriptr, findStr); oriptr = patloc + patlen)
//        {
//            patcnt++;
//        }
//    }
//    
//    {
//    // allocate memory for the new string
//    size_t const retlen = orilen + patcnt * (replen - patlen);
//    char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );
//    
//    if (returned != NULL)
//    {
//      // copy the inStr string, 
//      // replacing all the instances of the findStr
//      char * retptr = returned;
//      for (oriptr = inStr; patloc = strstr(oriptr, findStr); oriptr = patloc + patlen)
//      {
//        size_t const skplen = patloc - oriptr;
//        // copy the section until the occurence of the findStr
//        strncpy(retptr, oriptr, skplen);
//        retptr += skplen;
//        // copy the replaceStr 
//        strncpy(retptr, replaceStr, replen);
//        retptr += replen;
//      }
//      // copy the rest of the string.
//      strcpy(retptr, oriptr);
//    }
//    //return returned;
//
//
//    fillStrInMsParam (out, returned);
//
//    rei->status = 0;
//
//    return (rei->status);
//    }
//}
