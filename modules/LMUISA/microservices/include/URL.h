/*** Copyright (c), University of Helsinki           ***
 *** For more information please refer to files in the COPYRIGHT directory ***/

/* URL.h - header file for URL.c
 */

#ifndef URLMS_H
#define URLMS_H

#include <curl/curl.h>

#include "apiHeaderAll.h"
#include "objMetaOpr.h"
#include "miscUtil.h"

int msiUrlOpen(msParam_t *url, msParam_t *data, msParam_t *auth, msParam_t *res, ruleExecInfo_t *rei);

#endif	/* URLMS_H */
