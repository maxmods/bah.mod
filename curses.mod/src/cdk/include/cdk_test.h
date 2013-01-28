/*
 * $Id: cdk_test.h,v 1.5 2008/11/01 17:07:37 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDK_TEST_H
#define CDK_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cdk.h>

/*
 * Copyright 2005,2008 Thomas E. Dickey
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
 * The whole point of this header is to define ExitProgram(), which is used for
 * leak-checking when ncurses's _nc_free_and_exit() function is available. 
 * Invoking that rather than 'exit()' tells ncurses to free all of the
 * "permanent" memory leaks, making analysis much simpler.
 */
#ifdef HAVE_NC_ALLOC_H

#ifndef HAVE_LIBDBMALLOC
#define HAVE_LIBDBMALLOC 0
#endif

#ifndef HAVE_LIBDMALLOC
#define HAVE_LIBDMALLOC 0
#endif

#ifndef HAVE_LIBMPATROL
#define HAVE_LIBMPATROL 0
#endif

#include <nc_alloc.h>

#else

#if defined(NCURSES_VERSION) && defined(HAVE__NC_FREE_AND_EXIT)
/* nc_alloc.h normally not installed */
extern void _nc_free_and_exit(int) GCC_NORETURN;
#define ExitProgram(code) _nc_free_and_exit(code)
#endif

#endif /* HAVE_NC_ALLOC_H */

#ifndef ExitProgram
#define ExitProgram(code) exit(code)
#endif

#ifdef __cplusplus
}
#endif

#endif /* CDK_TEST_H */
#endif /* CDKINCLUDES */
