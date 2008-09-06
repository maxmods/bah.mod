/*
   +----------------------------------------------------------------------+   
   |                 OCILIB - C Wrapper for Oracle OCI                    |
   +----------------------------------------------------------------------+
   |              Website : http://orclib.sourceforge.net                 |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: oci_import.h, v 2.5.1 09:05 24/07/2008 Vince $
 * ------------------------------------------------------------------------ */

#ifndef OCILIB_OCI_IMPORT_H_INCLUDED 
#define OCILIB_OCI_IMPORT_H_INCLUDED 

#ifdef OCI_IMPORT_LINKAGE


/* this is necessary because some Oracle headers do NOT include C++ checks !
   By example, the file orid.h is not protected !
*/

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#include <oci.h>
#include <orid.h>

#ifdef  __cplusplus
}
#endif

#ifdef _WINDOWS
#pragma comment(lib, "oci.lib") 
#endif

#else

#include "oci_loader.h"
#include "oci_types.h"
#include "oci_def.h"
#include "oci_api.h"

#ifndef OCI_DL
  #if defined(_WINDOWS)
    #define OCI_DL                   oci.dll
  #elif defined(__APPLE__)
    #define OCI_DL                   libclntsh.dylib
  #elif defined(__hppa)
    #define OCI_DL                   libclntsh.sl
  #else
    #define OCI_DL                   libclntsh.so
  #endif
#endif

#ifdef OCI_METADATA_UNICODE
  #define OCI_DL_GET(l) L#l
#else
  #define OCI_DL_GET(l) #l
#endif

#define OCI_DL_CVT(l) OCI_DL_GET(l)
#define OCI_DL_NAME   OCI_DL_CVT(OCI_DL)


static OCIENVCREATE                 OCIEnvCreate                 = NULL;

static OCISERVERATTACH              OCIServerAttach              = NULL;
static OCISERVERDETACH              OCIServerDetach              = NULL;

static OCIHANDLEALLOC               OCIHandleAlloc               = NULL;
static OCIHANDLEFREE                OCIHandleFree                = NULL;

static OCIDESCRIPTORALLOC           OCIDescriptorAlloc           = NULL;
static OCIDESCRIPTORFREE            OCIDescriptorFree            = NULL;

static OCISESSIONBEGIN              OCISessionBegin              = NULL;
static OCISESSIONEND                OCISessionEnd                = NULL;

static OCIPASSWORDCHANGE            OCIPasswordChange            = NULL;

static OCIBINDBYPOS                 OCIBindByPos                 = NULL;
static OCIBINDBYNAME                OCIBindByName                = NULL;
static OCIBINDDYNAMIC               OCIBindDynamic               = NULL;
static OCIBINDOBJECT                OCIBindObject                = NULL;

static OCIDEFINEBYPOS               OCIDefineByPos               = NULL;
static OCIDEFINEOBJECT              OCIDefineObject              = NULL;

static OCISTMTPREPARE               OCIStmtPrepare               = NULL;
static OCISTMTEXECUTE               OCIStmtExecute               = NULL;
static OCISTMTFETCH                 OCIStmtFetch                 = NULL;

static OCISTMTGETPIECEINFO          OCIStmtGetPieceInfo          = NULL;
static OCISTMTSETPIECEINFO          OCIStmtSetPieceInfo          = NULL;

static OCIPARAMGET                  OCIParamGet                  = NULL;
static OCIPARAMSET                  OCIParamSet                  = NULL;

static OCITRANSSTART                OCITransStart                = NULL;
static OCITRANSDETACH               OCITransDetach               = NULL;
static OCITRANSPREPARE              OCITransPrepare              = NULL;
static OCITRANSFORGET               OCITransForget               = NULL;

static OCITRANSCOMMIT               OCITransCommit               = NULL;
static OCITRANSROLLBACK             OCITransRollback             = NULL;

static OCIERRORGET                  OCIErrorGet                  = NULL;

static OCILOBCREATETEMPORARY        OCILobCreateTemporary        = NULL;
static OCILOBFREETEMPORARY          OCILobFreeTemporary          = NULL;
static OCILOBISTEMPORARY            OCILobIsTemporary            = NULL;
static OCILOBAPPEND                 OCILobAppend                 = NULL;
static OCILOBCOPY                   OCILobCopy                   = NULL;
static OCILOBGETLENGTH              OCILobGetLength              = NULL;
static OCILOBREAD                   OCILobRead                   = NULL;
static OCILOBWRITE                  OCILobWrite                  = NULL;
static OCILOBTRIM                   OCILobTrim                   = NULL;
static OCILOBERASE                  OCILobErase                  = NULL;
static OCILOBOPEN                   OCILobOpen                   = NULL;
static OCILOBCLOSE                  OCILobClose                  = NULL;
static OCILOBLOCATORASSIGN          OCILobLocatorAssign          = NULL;
static OCILOBASSIGN                 OCILobAssign                 = NULL;
static OCILOBISEQUAL                OCILobIsEqual                = NULL;

static OCILOBFILEOPEN               OCILobFileOpen               = NULL;
static OCILOBFILECLOSE              OCILobFileClose              = NULL;
static OCILOBFILECLOSEALL           OCILobFileCloseAll           = NULL;
static OCILOBFILEISOPEN             OCILobFileIsOpen             = NULL;
static OCILOBFILEEXISTS             OCILobFileExists             = NULL;
static OCILOBFIELGETNAME            OCILobFileGetName            = NULL;
static OCILOBFILESETNAME            OCILobFileSetName            = NULL;
static OCILOBLOADFROMFILE           OCILobLoadFromFile           = NULL;
static OCILOBWRITEAPPEND            OCILobWriteAppend            = NULL;

static OCILOBCOPY2                  OCILobCopy2                  = NULL;
static OCILOBERASE2                 OCILobErase2                 = NULL;
static OCILOBGETLENGTH2             OCILobGetLength2             = NULL;
static OCILOBLOADFROMFILE2          OCILobLoadFromFile2          = NULL;
static OCILOBREAD2                  OCILobRead2                  = NULL;
static OCILOBTRIM2                  OCILobTrim2                  = NULL;
static OCILOBWRITE2                 OCILobWrite2                 = NULL;
static OCILOBWRITEAPPEND2           OCILobWriteAppend2           = NULL;

static OCISERVERVERSION             OCIServerVersion             = NULL;
static OCIBREAK                     OCIBreak                     = NULL;

static OCIATTRGET                   OCIAttrGet                   = NULL;
static OCIATTRSET                   OCIAttrSet                   = NULL;

static OCIDATEASSIGN                OCIDateAssign                = NULL;
static OCIDATETOTEXT                OCIDateToText                = NULL;
static OCIDATEFROMTEXT              OCIDateFromText              = NULL;
static OCIDATECOMPARE               OCIDateCompare               = NULL;
static OCIDATEADDMONTHS             OCIDateAddMonths             = NULL;
static OCIDATEADDDAYS               OCIDateAddDays               = NULL;
static OCIDATELASTDAY               OCIDateLastDay               = NULL;
static OCIDATEDAYSBETWEEN           OCIDateDaysBetween           = NULL;
static OCIDATEZONETOZONE            OCIDateZoneToZone            = NULL;
static OCIDATENEXTDAY               OCIDateNextDay               = NULL;
static OCIDATECHECK                 OCIDateCheck                 = NULL;
static OCIDATESYSDATE               OCIDateSysDate               = NULL;

static OCIDESCRIBEANY               OCIDescribeAny               = NULL;

static OCIINTERVALASSIGN            OCIIntervalAssign            = NULL;  
static OCIINTERVALCHECK             OCIIntervalCheck             = NULL;
static OCIINTERVALCOMPARE           OCIIntervalCompare           = NULL;
static OCIINTERVALFROMTEXT          OCIIntervalFromText          = NULL;
static OCIINTERVALTOTEXT            OCIIntervalToText            = NULL;
static OCIINTERVALFROMTZ            OCIIntervalFromTZ            = NULL;
static OCIINTERVALGETDAYSECOND      OCIIntervalGetDaySecond      = NULL;
static OCIINTERVALGETYEARMONTH      OCIIntervalGetYearMonth      = NULL;
static OCIINTERVALSETDAYSECOND      OCIIntervalSetDaySecond      = NULL;
static OCIINTERVALSETYEARMONTH      OCIIntervalSetYearMonth      = NULL;
static OCIINTERVALSUBTRACT          OCIIntervalSubtract          = NULL;
static OCIINTERVALADD               OCIIntervalAdd               = NULL;

static OCIDATETIMEASSIGN            OCIDateTimeAssign            = NULL;  
static OCIDATETIMECHECK             OCIDateTimeCheck             = NULL;
static OCIDATETIMECOMPARE           OCIDateTimeCompare           = NULL;
static OCIDATETIMECONSTRUCT         OCIDateTimeConstruct         = NULL;
static OCIDATETIMECONVERT           OCIDateTimeConvert           = NULL;
static OCIDATETIMEFROMARRAY         OCIDateTimeFromArray         = NULL;
static OCIDATETIMETOARRAY           OCIDateTimeToArray           = NULL;
static OCIDATETIMEFROMTEXT          OCIDateTimeFromText          = NULL;
static OCIDATETIMETOTEXT            OCIDateTimeToText            = NULL;
static OCIDATETIMEGETDATE           OCIDateTimeGetDate           = NULL;
static OCIDATETIMEGETTIME           OCIDateTimeGetTime           = NULL;
static OCIDATETIMEGETTIMEZONENAME   OCIDateTimeGetTimeZoneName   = NULL;
static OCIDATETIMEGETTIMEZONEOFFSET OCIDateTimeGetTimeZoneOffset = NULL;
static OCIDATETIMEINTERVALADD       OCIDateTimeIntervalAdd       = NULL;
static OCIDATETIMEINTERVALSUB       OCIDateTimeIntervalSub       = NULL;
static OCIDATETIMESUBTRACT          OCIDateTimeSubtract          = NULL;
static OCIDATETIMESYSTIMESTAMP      OCIDateTimeSysTimeStamp      = NULL;

static OCIARRAYDESCRIPTORFREE       OCIArrayDescriptorFree       = NULL;
static OCICLIENTVERSION             OCIClientVersion             = NULL;

static OCITYPEBYNAME                OCITypeByName                = NULL;

static OCINUMBERTOINT               OCINumberToInt               = NULL;
static OCINUMBERFROMINT             OCINumberFromInt             = NULL;

static OCINUMBERTOREAL              OCINumberToReal              = NULL;
static OCINUMBERFROMREAL            OCINumberFromReal            = NULL;

static OCINUMBERTOTEXT              OCINumberToText              = NULL;

static OCISTRINGPTR                 OCIStringPtr                 = NULL;
static OCISTRINGASSIGNTEXT          OCIStringAssignText          = NULL;
	
static OCIRAWPTR                    OCIRawPtr                    = NULL;
static OCIRAWASSIGNBYTES            OCIRawAssignBytes            = NULL;
static OCIRAWALLOCSIZE              OCIRawAllocSize              = NULL;
	
static OCIOBJECTNEW                 OCIObjectNew                 = NULL;
static OCIOBJECTFREE                OCIObjectFree                = NULL;
static OCIOBJECTSETATTR             OCIObjectSetAttr             = NULL;
static OCIOBJECTGETATTR             OCIObjectGetAttr             = NULL;

static OCITHREADPROCESSINIT         OCIThreadProcessInit         = NULL;
static OCITHREADINIT                OCIThreadInit                = NULL;
static OCITHREADTERM                OCIThreadTerm                = NULL;

static OCITHREADIDINIT              OCIThreadIdInit              = NULL;
static OCITHREADIDDESTROY           OCIThreadIdDestroy           = NULL;
static OCITHREADHNDINIT             OCIThreadHndInit             = NULL;
static OCITHREADHNDDESTROY          OCIThreadHndDestroy          = NULL;
static OCITHREADCREATE              OCIThreadCreate              = NULL;
static OCITHREADJOIN                OCIThreadJoin                = NULL;
static OCITHREADCLOSE               OCIThreadClose               = NULL;

static OCITHREADMUTEXINIT           OCIThreadMutexInit           = NULL;
static OCITHREADMUTEXDESTROY        OCIThreadMutexDestroy        = NULL;
static OCITHREADMUTEXACQUIRE        OCIThreadMutexAcquire        = NULL;
static OCITHREADMUTEXRELEASE        OCIThreadMutexRelease        = NULL;

static OCITHREADKEYINIT             OCIThreadKeyInit             = NULL;
static OCITHREADKEYDESTROY          OCIThreadKeyDestroy          = NULL;
static OCITHREADKEYSET              OCIThreadKeySet              = NULL;
static OCITHREADKEYGET              OCIThreadKeyGet              = NULL;

static OCICONNECTIONPOOLCREATE      OCIConnectionPoolCreate      = NULL;
static OCICONNECTIONPOOLDESTROY     OCIConnectionPoolDestroy     = NULL;

#define OCIDateGetTime(date, hour, min, sec) \
  { \
     *hour = (date)->OCIDateTime.OCITimeHH; \
     *min = (date)->OCIDateTime.OCITimeMI; \
     *sec = (date)->OCIDateTime.OCITimeSS; \
  }

#define OCIDateGetDate(date, year, month, day) \
  { \
     *year = (date)->OCIDateYYYY; \
     *month = (date)->OCIDateMM; \
     *day = (date)->OCIDateDD; \
  }

#define OCIDateSetTime(date, hour, min, sec) \
  { \
     (date)->OCIDateTime.OCITimeHH = hour; \
     (date)->OCIDateTime.OCITimeMI = min; \
     (date)->OCIDateTime.OCITimeSS = sec; \
  }

#define OCIDateSetDate(date, year, month, day) \
  { \
     (date)->OCIDateYYYY = year; \
     (date)->OCIDateMM = month; \
     (date)->OCIDateDD = day; \
  }

#endif

#endif    /* OCILIB_OCI_IMPORT_H_INCLUDED */

