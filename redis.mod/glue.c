/*
  Copyright (c) 2016 Bruce A Henderson
  All rights reserved.
 
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the auther nor the names of its contributors may be used to 
        endorse or promote products derived from this software without specific
        prior written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "hiredis.h"
#include "brl.mod/blitz.mod/blitz.h"

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif


BBObject * CB_PREF(bah_redis_TRedisReply__create)(redisReply * reply, int type, long long value, size_t elements);



BBString * bmx_redis_reply_getstring(redisReply * reply) {

	BBString * s = &bbEmptyString;

	if (reply->len > 0) {
		s = bbStringFromUTF8String(reply->str);
	}

	return s;
}

BBObject * bmx_redis_reply_element(redisReply * reply, int index) {

	redisReply * element = reply->element[index];

	return CB_PREF(bah_redis_TRedisReply__create)(element, element->type, element->integer, element->elements);

}

int bmx_redis_err(redisContext * ctx) {
	return ctx->err;
}

BBString * bmx_redis_errstr(redisContext * ctx) {
	return bbStringFromCString(ctx->errstr);
}

/* ----------------------------------------------------- */

static BBObject * build_reply(redisReply * reply) {
	if (reply) {
		return CB_PREF(bah_redis_TRedisReply__create)(reply, reply->type, reply->integer, reply->elements);
	} else {
		return &bbNullObject;
	}
}

BBObject * bmx_redis_command_ss(redisContext * ctx, char * cmd, char * txt) {
	return build_reply(redisCommand(ctx, "%s %s", cmd, txt));
}

BBObject * bmx_redis_command_s(redisContext * ctx, char * cmd) {
	return build_reply(redisCommand(ctx, cmd));
}

BBObject * bmx_redis_command_si(redisContext * ctx, char * cmd, int arg) {
	return build_reply(redisCommand(ctx, "%s %d", cmd, arg));
}

BBObject * bmx_redis_command_ssi(redisContext * ctx, char * cmd, char * txt, int num) {
	return build_reply(redisCommand(ctx, "%s %s %d", cmd, txt, num));
}

BBObject * bmx_redis_command_sss(redisContext * ctx, char * cmd, char * txt1, char * txt2) {
	return build_reply(redisCommand(ctx, "%s %s %s", cmd, txt1, txt2));
}

BBObject * bmx_redis_command_sssi(redisContext * ctx, char * cmd, char * txt1, char * txt2, int num) {
	return build_reply(redisCommand(ctx, "%s %s %s %d", cmd, txt1, txt2, num));
}

BBObject * bmx_redis_command_ssii(redisContext * ctx, char * cmd, char * txt1, int num1, int num2) {
	return build_reply(redisCommand(ctx, "%s %s %d %d", cmd, txt1, num1, num2));
}

BBObject * bmx_redis_command_ssis(redisContext * ctx, char * cmd, char * txt1, int num, char * txt2) {
	return build_reply(redisCommand(ctx, "%s %s %d %s", cmd, txt1, num, txt2));
}

BBObject * bmx_redis_command_ssds(redisContext * ctx, char * cmd, char * txt1, double num, char * txt2) {
	return build_reply(redisCommand(ctx, "%s %s %f %s", cmd, txt1, num, txt2));
}

BBObject * bmx_redis_command_ssdd(redisContext * ctx, char * cmd, char * txt1, double num1, double num2) {
	return build_reply(redisCommand(ctx, "%s %s %f %f", cmd, txt1, num1, num2));
}

BBObject * bmx_redis_command_ssss(redisContext * ctx, char * cmd, char * txt1, char * txt2, char * txt3) {
	return build_reply(redisCommand(ctx, "%s %s %s %s", cmd, txt1, txt2, txt3));
}

BBObject * bmx_redis_command_sssss(redisContext * ctx, char * cmd, char * txt1, char * txt2, char * txt3, char * txt4) {
	return build_reply(redisCommand(ctx, "%s %s %s %s %s", cmd, txt1, txt2, txt3, txt4));
}

BBObject * bmx_redis_command_a(redisContext * ctx, char * cmd) {
	return build_reply(redisCommand(ctx, cmd));
}


/* ----------------------------------------------------- */
