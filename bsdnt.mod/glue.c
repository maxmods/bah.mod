/*
Copyright (c) 2018 Bruce A Henderson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "zz.h"
#include "brl.mod/blitz.mod/blitz.h"

void bmx_zz_init(zz_ptr zz) {
	zz_init(zz);
}

void bmx_zz_clear(zz_ptr zz) {
	zz_clear(zz);
}

void bmx_zz_set(zz_ptr zz, zz_ptr o) {
	zz_set(zz, o);
}

void bmx_zz_seti(zz_ptr zz, int value) {
	zz_seti(zz, value);
}

BBString * bmx_zz_get_str(zz_ptr zz) {
	char * s = zz_get_str(zz);
	BBString * ret = bbStringFromUTF8String(s);
	free(s);
	return ret;
}

void bmx_zz_set_str(zz_ptr zz, BBString * value) {
	if (value == &bbEmptyString) {
		return;
	}
	
	char * s = bbStringToUTF8String(value);
	zz_set_str(zz, s);
	bbMemFree(s);
}

void bmx_zz_add(zz_ptr n, zz_ptr zz, zz_ptr o) {
	zz_add(n, zz, o);
}

void bmx_zz_sub(zz_ptr n, zz_ptr zz, zz_ptr o) {
	zz_sub(n, zz, o);
}

void bmx_zz_mul(zz_ptr n, zz_ptr zz, zz_ptr o) {
	zz_mul(n, zz, o);
}

void bmx_zz_div(zz_ptr n, zz_ptr zz, zz_ptr o) {
	zz_div(n, zz, o);
}

void bmx_zz_divrem(zz_ptr n,  zz_ptr r, zz_ptr zz, zz_ptr o) {
	zz_divrem(n, r, zz, o);
}

void bmx_zz_addi(zz_ptr n, zz_ptr zz, int o) {
	zz_addi(n, zz, o);
}

void bmx_zz_subi(zz_ptr n, zz_ptr zz, int o) {
	zz_subi(n, zz, o);
}

void bmx_zz_muli(zz_ptr n, zz_ptr zz, int o) {
	zz_muli(n, zz, o);
}

int bmx_zz_divremi(zz_ptr n, zz_ptr zz, int o) {
	return zz_divremi(n, zz, o);
}

void bmx_zz_neg(zz_ptr n, zz_ptr zz) {
	zz_neg(n, zz);
}

int bmx_zz_equali(zz_ptr zz, int o) {
	return zz_equali(zz, o);
}

int bmx_zz_equal(zz_ptr zz, zz_ptr o) {
	return zz_equal(zz, o);
}

void bmx_zz_zero(zz_ptr zz) {
	zz_zero(zz);
}

int bmx_zz_is_zero(zz_ptr zz) {
	return zz_is_zero(zz);
}
