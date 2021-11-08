/*
  Copyright 2018-2021 Bruce A Henderson
  
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  
      http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#include "mbedtls/net_sockets.h"
#include "mbedtls/x509.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "brl.mod/blitz.mod/blitz.h"

mbedtls_net_context * bmx_mbedtls_net_init() {
	mbedtls_net_context * context = (mbedtls_net_context *)malloc(sizeof(mbedtls_net_context));
	mbedtls_net_init(context);
	return context;
}

void bmx_mbedtls_net_free(mbedtls_net_context * context) {
	mbedtls_net_free(context);
}

void bmx_mbedtls_net_delete(mbedtls_net_context * context) {
	free(context);
}

int bmx_mbedtls_net_bind(mbedtls_net_context * context, BBString * bindIp, BBString * port, int proto) {
	char * b = NULL;
	if (bindIp != &bbEmptyString) {
		b = bbStringToUTF8String(bindIp);
	}
	char * p = NULL;
	if (port != &bbEmptyString) {
		p = bbStringToUTF8String(port);
	}
	int res = mbedtls_net_bind(context, b, p, proto);
	bbMemFree(p);
	bbMemFree(b);
	return res;
}

int bmx_mbedtls_net_connect(mbedtls_net_context * context, BBString * host, BBString * port, int proto) {
	char * h = NULL;
	if (host != &bbEmptyString) {
		h = bbStringToUTF8String(host);
	}
	char * p = NULL;
	if (port != &bbEmptyString) {
		p = bbStringToUTF8String(port);
	}
	int res = mbedtls_net_connect(context, h, p, proto);
	bbMemFree(p);
	bbMemFree(h);
	return res;
}

#ifdef BMX_NG
int bmx_mbedtls_net_recv(mbedtls_net_context * context, char * buf, size_t length) {
#else
int bmx_mbedtls_net_recv(mbedtls_net_context * context, char * buf, int length) {
#endif
	return mbedtls_net_recv(context, buf, length);
}

#ifdef BMX_NG
int bmx_mbedtls_net_recv_timeout(mbedtls_net_context * context, char * buf, size_t length, uint32_t timeout) {
#else
int bmx_mbedtls_net_recv_timeout(mbedtls_net_context * context, char * buf, int length, int timeout) {
#endif
	return mbedtls_net_recv_timeout(context, buf, length, timeout);
}

#ifdef BMX_NG
int bmx_mbedtls_net_send(mbedtls_net_context * context, char * buf, size_t length) {
#else
int bmx_mbedtls_net_send(mbedtls_net_context * context, char * buf, int length) {
#endif
	return mbedtls_net_send(context, buf, length);
}

#ifdef BMX_NG
void bmx_mbedtls_net_usleep(uint32_t usec) {
#else
void bmx_mbedtls_net_usleep(int usec) {
#endif
	mbedtls_net_usleep(usec);
}

int bmx_mbedtls_net_poll(mbedtls_net_context * context, int rw, int timeout) {
	return mbedtls_net_poll(context, rw, timeout);
}

int bmx_mbedtls_net_cbsend(mbedtls_ssl_send_t * send, void * context, char * buf, size_t length) {
	return send(context, buf, length);
}

int bmx_mbedtls_net_cbrecv(mbedtls_ssl_recv_t * recv, void * context, char * buf, size_t length) {
	return recv(context, buf, length);
}


int bmx_mbedtls_net_cbtimeout(mbedtls_ssl_recv_timeout_t * recvtimeout, void * context, char * buf, size_t length, int timeout) {
	return recvtimeout(context, buf, length, timeout);
}

// --------------------------------------------------------

mbedtls_ssl_context * bmx_mbedtls_ssl_init() {
	mbedtls_ssl_context * context = (mbedtls_ssl_context *)malloc(sizeof(mbedtls_ssl_context));
	mbedtls_ssl_init(context);
	return context;
}

void bmx_mbedtls_ssl_free(mbedtls_ssl_context * context) {
	mbedtls_ssl_free(context);
	free(context);
}

#ifdef BMX_NG
int bmx_mbedtls_ssl_read(mbedtls_ssl_context * context, char * buf, size_t length) {
#else
int bmx_mbedtls_ssl_read(mbedtls_ssl_context * context, char * buf, int length) {
#endif
	return mbedtls_ssl_read(context, buf, length);
}

#ifdef BMX_NG
int bmx_mbedtls_ssl_write(mbedtls_ssl_context * context, char * buf, size_t length) {
#else
int bmx_mbedtls_ssl_write(mbedtls_ssl_context * context, char * buf, int length) {
#endif
	return mbedtls_ssl_write(context, buf, length);
}


// --------------------------------------------------------

mbedtls_ssl_config * bmx_mbedtls_ssl_config_init() {
	mbedtls_ssl_config * config = (mbedtls_ssl_config *)malloc(sizeof(mbedtls_ssl_config));
	mbedtls_ssl_config_init(config);
	return config;
}

void bmx_mbedtls_ssl_config_free(mbedtls_ssl_config * config) {
	mbedtls_ssl_config_free(config);
	free(config);
}

void bmx_mbedtls_ssl_conf_rng(mbedtls_ssl_config * config, int (*f_rng)(void *, unsigned char *, size_t), void *rng) {
	mbedtls_ssl_conf_rng(config, f_rng, rng);
}

// --------------------------------------------------------

mbedtls_x509_crt * bmx_mbedtls_x509_crt_init() {
	mbedtls_x509_crt * cert = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
	mbedtls_x509_crt_init(cert);
	return cert;
}

void bmx_mbedtls_x509_crt_free(mbedtls_x509_crt * cert) {
	mbedtls_x509_crt_free(cert);
	free(cert);
}

int bmx_mbedtls_x509_crt_parse(mbedtls_x509_crt * cert, char * buf, int buflen) {
	return mbedtls_x509_crt_parse(cert, buf, buflen);
}

// --------------------------------------------------------

mbedtls_pk_context * bmx_mbedtls_pk_init() {
	mbedtls_pk_context * context = (mbedtls_pk_context *)malloc(sizeof(mbedtls_pk_context));
	mbedtls_pk_init(context);
	return context;
}

void bmx_mbedtls_pk_free(mbedtls_pk_context * context) {
	mbedtls_pk_free(context);
	free(context);
}

int bmx_mbedtls_pk_parse_key(mbedtls_pk_context * context, char * key, int keylen, char * pwd, int pwdlen, int (*f_rng)(void *, unsigned char *, size_t), void *rng) {
	return mbedtls_pk_parse_key(context, key, keylen, pwd, pwdlen, f_rng, rng);
}

int bmx_mbedtls_pk_parse_key_string(mbedtls_pk_context * context, BBString * key, BBString * pwd, int (*f_rng)(void *, unsigned char *, size_t), void *rng) {
	char * k = bbStringToCString(key);
	char * p = NULL;
	
	// mbedtls takes length including the null terminator
	size_t k_len = strlen(k) + 1;
	size_t p_len = 0;
	
	if (pwd != &bbEmptyString) {
		p = bbStringToCString(pwd);
		p_len = strlen(p) + 1;
	}
	
	int res = mbedtls_pk_parse_key(context, k, k_len, p, p_len, f_rng, rng);
	
	bbMemFree(p);
	
	if (pwd != &bbEmptyString) {
		bbMemFree(k);
	}
	
	return res;
}

// --------------------------------------------------------

mbedtls_ctr_drbg_context * bmx_mbedtls_ctr_drbg_init() {
	mbedtls_ctr_drbg_context * context = (mbedtls_ctr_drbg_context *)malloc(sizeof(mbedtls_ctr_drbg_context));
	mbedtls_ctr_drbg_init(context);
	return context;
}

void bmx_mbedtls_ctr_drbg_free(mbedtls_ctr_drbg_context * context) {
	mbedtls_ctr_drbg_free(context);
	free(context);
}

int bmx_mbedtls_ctr_drbg_seed(mbedtls_ctr_drbg_context * context, int (*f_entropy)(void *, unsigned char *, size_t), void *entropy, const unsigned char *custom, size_t len) {
	return mbedtls_ctr_drbg_seed(context, f_entropy, entropy, custom, len);
}

// --------------------------------------------------------

mbedtls_entropy_context * bmx_mbedtls_entropy_init() {
	mbedtls_entropy_context * context = (mbedtls_entropy_context *)malloc(sizeof(mbedtls_entropy_context));
	mbedtls_entropy_init(context);
	return context;
}

void bmx_mbedtls_entropy_free(mbedtls_entropy_context * context) {
	mbedtls_entropy_free(context);
	free(context);
}
