/*
 * $Id: cdk_compat.h,v 1.2 2005/12/28 21:45:45 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDK_COMPAT_H
#define CDK_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copyright 2004,2005 Thomas E. Dickey
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 * 	This product includes software developed by Thomas Dickey
 * 	and contributors.
 * 4. Neither the name of Thomas Dickey, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THOMAS DICKEY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DICKEY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Renaming of struct members was done for consistency.  Making old/new
 * versions compatible with renaming is awkward (there may be inadvertant
 * naming collisions), but this is mainly for demonstration purposes.
 */
#define info      list
#define infoSize  listSize
#define itemCount listSize
#define boxMatrix box

/*
 * Include the Cdk header file, which includes everything except for this file.
 */
#include <cdk.h>

/*
 * This is defined via the new header file:
 */
#ifdef CDK_VERSION_PATCH

/*
 * If we included the new Cdk header file, provide definitions for things
 * which have been made obsolete.
 */

#define MAX_LINES	5000

#define WIN_WIDTH(a)	getmaxx(a)
#define WIN_HEIGHT(a)	getmaxy(a)
#define WIN_XPOS(a)	getbegx(a)
#define WIN_YPOS(a)	getbegy(a)

extern int getDirectoryContents (char *directory, char **list, int maxListSize);
extern int readFile (char *filename, char **info, int maxlines);
extern int splitString (char *string, char **items, char splitChar);

#else

/*
 * Provide definitions to allow the applications using the old header to
 * compile using the new header's macros:
 */
#define ObjOf(a)	(a)
#define ScreenOf(a)	(a)

#endif	/* CDK_VERSION_PATCH */

#ifdef __cplusplus
}
#endif

#endif /* CDK_COMPAT_H */
#endif /* CDKINCLUDES */
