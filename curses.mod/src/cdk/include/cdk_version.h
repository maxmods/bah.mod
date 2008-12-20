/*
 * $Id: cdk_version.hin,v 1.1 2002/07/20 00:26:36 tom Exp $
 */

#ifndef CDK_VERSION_H
#define CDK_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define CDK_VERSION_MAJOR "5"
#define CDK_VERSION_MINOR "0"
#define CDK_VERSION_PATCH "20060507"

/*
 * Runtime to return the same version information.
 */
char * CDKVersion (void);

#ifdef __cplusplus
}
#endif

#endif /* CDK_VERSION_H */
