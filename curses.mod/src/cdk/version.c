#include <cdk.h>

/*
 * $Author: tom $
 * $Date: 2002/07/20 00:30:51 $
 * $Revision: 1.1 $
 */
char * CDKVersion(void)
{
   return CDK_VERSION_MAJOR "." CDK_VERSION_MINOR " - " CDK_VERSION_PATCH;
}
