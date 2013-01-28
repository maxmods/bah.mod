/*
 * $Id: cdk_params.h,v 1.7 2012/03/21 21:15:30 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDK_PARAMS_H
#define CDK_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CDK_H
#define CDKINCLUDES
#include <cdk.h>
#undef CDKINCLUDES
#endif

/*
 * Copyright 2003-2005,2012 Thomas E. Dickey
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

#define MAX_CDK_PARAMS 256

/*
 * CDKparseParams() knows about these options and will decode them into
 * the CDK_PARAMS struct.  They are the most generally useful for positioning
 * a widget.
 */
#define CDK_MIN_PARAMS	"NSX:Y:"
#define CDK_CLI_PARAMS	"NSX:Y:H:W:"

/*
 * Use this exit code rather than -1 for cli programs which have reported an
 * error.  Actually EXIT_FAILURE would be better, but the shell script samples
 * all are written to assume that the exit code can be used to indicate a
 * button number, etc.
 */
#define CLI_ERROR	255

/*
 * This records the values that CDKparseParams() decodes using getopt():
 */
typedef struct CDK_PARAMS {
   char *	allParams[MAX_CDK_PARAMS];
   bool		Box;
   bool		Shadow;
   int		hValue;
   int		wValue;
   int		xValue;
   int		yValue;
} CDK_PARAMS;

/*
 * Parse the given argc/argv command-line, with the options passed to
 * getopt()'s 3rd parameter.
 */
void CDKparseParams (
		int		/* argc */,
		char **		/* argv */,
		CDK_PARAMS *	/* params */,
		const char *	/* options */);

/*
 * Parse the string as one of CDK's positioning keywords, or an actual
 * position.
 */
int CDKparsePosition (
		const char *	/* string */);

/*
 * Retrieve an integer (or boolean) option value from the parsed command-line.
 */
int CDKparamNumber (
		CDK_PARAMS *	/* params */,
		int		/* option */);

/*
 * Retrieve an optional integer (or boolean) value from the parsed command-line.
 */
int CDKparamNumber2 (
		CDK_PARAMS *	/* params */,
		int		/* option */,
		int		/* missing */);

/*
 * Retrieve a string option value from the parsed command-line.
 */
char * CDKparamString (
		CDK_PARAMS *	/* params */,
		int		/* option */);

/*
 * Retrieve an optional string option value from the parsed command-line.
 */
char * CDKparamString2 (
		CDK_PARAMS *	/* params */,
		int		/* option */,
		const char *	/* missing */);

/*
 * Retrieve an integer (or boolean) option value from the parsed command-line.
 */
int CDKparamValue (
		CDK_PARAMS *	/* params */,
		int		/* option */,
		int		/* missing */);

#ifdef __cplusplus
}
#endif

#endif /* CDK_PARAMS_H */
#endif /* CDKINCLUDES */
