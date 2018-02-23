/*
 Copyright (c) 2007-2018, Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "brl.mod/blitz.mod/blitz.h"
#include <stdlib.h>
#include <malloc.h>
#include "SFMT.h"

sfmt_t * bmx_sfmt_init_gen_rand(sfmt_t * sfmt, int seed) {
	if (sfmt == NULL) {
#ifdef WIN32
		sfmt = _aligned_malloc(sizeof(*sfmt), 16);
#else
		sfmt = aligned_malloc(16, sizeof(*sfmt));
#endif
	}
	sfmt_init_gen_rand(sfmt, seed);
	return sfmt;
}

void bmx_sfmt_free(sfmt_t * sfmt) {
#ifdef WIN32
	_aligned_free(sfmt);
#else
	free(sfmt);
#endif
}

double bmx_genrand_real1(sfmt_t * sfmt) {
	return sfmt_genrand_real1(sfmt);
}

double bmx_genrand_real2(sfmt_t * sfmt) {
	return sfmt_genrand_real2(sfmt);
}

double bmx_genrand_real3(sfmt_t * sfmt) {
	return sfmt_genrand_real3(sfmt);
}

#ifdef BMX_NG
void bmx_gen_rand64(sfmt_t * sfmt, BBUInt64 * r) {
	*r = sfmt_genrand_uint64(sfmt);
}
#else
void bmx_gen_rand64(sfmt_t * sfmt, BBInt64 * r) {
	*r = sfmt_genrand_uint64(sfmt);
}
#endif

double bmx_genrand_res53(sfmt_t * sfmt) {
    return sfmt_genrand_res53(sfmt);
}
