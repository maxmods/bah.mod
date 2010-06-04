/*
 Copyright (c) 2007-2010 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include <openssl/evp.h>


extern "C" {

#include "blitz.h"

	EVP_CIPHER_CTX * bmx_EVP_CIPHER_CTX_create();
	void bmx_EVP_CIPHER_CTX_delete(EVP_CIPHER_CTX * ctx);

	int bmx_EVP_EncryptInit_ex(EVP_CIPHER_CTX * ctx, EVP_CIPHER * type, ENGINE * impl,
		unsigned char * key, int kLen, unsigned char * iv, int vLen);		

	int bmx_EVP_DecryptInit_ex(EVP_CIPHER_CTX * ctx, EVP_CIPHER * type, ENGINE * impl,
		unsigned char * key, int kLen, unsigned char * iv, int vLen);		

	int bmx_EVP_CipherInit_ex(EVP_CIPHER_CTX * ctx, EVP_CIPHER * type, ENGINE * impl,
		unsigned char * key, int kLen, unsigned char * iv, int vLen, int enc);		


}


EVP_CIPHER_CTX * bmx_EVP_CIPHER_CTX_create() {
	EVP_CIPHER_CTX * ctx = new EVP_CIPHER_CTX;
	EVP_CIPHER_CTX_init(ctx);
	return ctx;
}

void bmx_EVP_CIPHER_CTX_delete(EVP_CIPHER_CTX * ctx) {
	delete ctx;
}

int bmx_EVP_EncryptInit_ex(EVP_CIPHER_CTX * ctx, EVP_CIPHER * type, ENGINE * impl, 
		unsigned char * key, int kLen, unsigned char * iv, int vLen) {
	
	return EVP_EncryptInit_ex(ctx, type, impl, (kLen > 0) ? key : NULL, (vLen > 0) ? iv : NULL);
}

int bmx_EVP_DecryptInit_ex(EVP_CIPHER_CTX * ctx, EVP_CIPHER * type, ENGINE * impl, 
		unsigned char * key, int kLen, unsigned char * iv, int vLen) {
	
	return EVP_DecryptInit_ex(ctx, type, impl, (kLen > 0) ? key : NULL, (vLen > 0) ? iv : NULL);
}

int bmx_EVP_CipherInit_ex(EVP_CIPHER_CTX * ctx, EVP_CIPHER * type, ENGINE * impl, 
		unsigned char * key, int kLen, unsigned char * iv, int vLen, int enc) {
	
	return EVP_CipherInit_ex(ctx, type, impl, (kLen > 0) ? key : NULL, (vLen > 0) ? iv : NULL, enc);
}


// *******************************************************


