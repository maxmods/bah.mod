/*
 Copyright (c) 2009-20018 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of the author nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

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
#include <libssh2.h>
#include <libssh2_sftp.h>

class MaxSSHSession;

#ifdef BMX_NG
#define CB_PREF(func) func
#else
#define CB_PREF(func) _##func
#endif

extern "C" {

#include "blitz.h"

	void CB_PREF(bah_libssh2_TSSHSession__kbdCallback)(BBObject * session, BBString * name, BBString * instruction, 
		BBArray * prompts, BBArray * responses);

	BBArray * CB_PREF(bah_libssh2_TSSHSession__newPrompts)(int size);
	void CB_PREF(bah_libssh2_TSSHSession__setPrompt)(BBArray * array, int index, const LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt);
	BBArray * CB_PREF(bah_libssh2_TSSHSession__newResponses)(int size);
	void CB_PREF(bah_libssh2_TSSHSession__setResponse)(BBArray * array, int index, LIBSSH2_USERAUTH_KBDINT_RESPONSE * response);
	int bmx_libssh2_session_disconnect(MaxSSHSession * session, BBString * description);

	MaxSSHSession * bmx_libssh2_session_create(BBObject * handle);
	int bmx_libssh2_session_startup(MaxSSHSession * session, int socket);
	int bmx_libssh2_session_free(MaxSSHSession * session);

	LIBSSH2_CHANNEL * bmx_libssh2_channel_open(MaxSSHSession * session);
	
	int bmx_libssh2_userauth_keyboard_interactive(MaxSSHSession * session, BBString * username);
	int bmx_libssh2_userauth_password(MaxSSHSession * session, BBString * username, BBString * password);
	int bmx_libssh2_userauth_authenticated(MaxSSHSession * session);

	BBString * bmx_libssh2_kbdint_prompt_gettext(LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt);
	int bmx_libssh2_kbdint_prompt_echo(LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt);

	void bmx_libssh2_kbdint_response_settext(LIBSSH2_USERAUTH_KBDINT_RESPONSE * response, BBString * text);

	int bmx_libssh2_channel_setenv(LIBSSH2_CHANNEL * channel, BBString * name, BBString * value);
	int bmx_libssh2_channel_requestpty(LIBSSH2_CHANNEL * channel, BBString * term);
	int bmx_libssh2_channel_shell(LIBSSH2_CHANNEL * channel);
	int bmx_libssh2_channel_exec(LIBSSH2_CHANNEL * channel, BBString * message);
	int bmx_libssh2_channel_subsystem(LIBSSH2_CHANNEL * channel, BBString * message);
	int bmx_libssh2_channel_processstartup(LIBSSH2_CHANNEL * channel, BBString * request, BBString * message);
	void bmx_libssh2_channel_free(LIBSSH2_CHANNEL * channel);
	int bmx_libssh2_channel_read(LIBSSH2_CHANNEL * channel, char * buffer, int size);
	int bmx_libssh2_channel_read_stderr(LIBSSH2_CHANNEL * channel, char * buffer, int size);
	int bmx_libssh2_channel_write(LIBSSH2_CHANNEL * channel, char * buffer, int size);
	int bmx_libssh2_channel_write_stderr(LIBSSH2_CHANNEL * channel, char * buffer, int size);
	int bmx_libssh2_channel_eof(LIBSSH2_CHANNEL * channel);
	void bmx_libssh2_channel_set_blocking(LIBSSH2_CHANNEL * channel, int blocking);
	int bmx_libssh2_channel_pollread(LIBSSH2_CHANNEL * channel, int extended);
	int bmx_libssh2_channel_sendeof(LIBSSH2_CHANNEL * channel);
	int bmx_libssh2_channel_waitclosed(LIBSSH2_CHANNEL * channel);
	int bmx_libssh2_channel_waiteof(LIBSSH2_CHANNEL * channel);
	int bmx_libssh2_channel_getexitstatus(LIBSSH2_CHANNEL * channel);
	LIBSSH2_CHANNEL * bmx_libssh2_channel_directtcpip(MaxSSHSession * session, BBString * host, int port, BBString * shost, int sport);

	LIBSSH2_SFTP * bmx_libssh2_sftp_init(MaxSSHSession * session);
	int bmx_libssh2_sftp_shutdown(LIBSSH2_SFTP * sftp);
	int bmx_libssh2_sftp_last_error(LIBSSH2_SFTP * sftp);
	LIBSSH2_CHANNEL * bmx_libssh2_sftp_get_channel(LIBSSH2_SFTP * sftp);
	int bmx_libssh2_sftp_rename(LIBSSH2_SFTP * sftp, BBString * sourcefile, BBString * destfile, int flags);
	int bmx_libssh2_sftp_mkdir(LIBSSH2_SFTP * sftp, BBString * path, int mode);
	int bmx_libssh2_sftp_rmdir(LIBSSH2_SFTP * sftp, BBString * path);
	LIBSSH2_SFTP_HANDLE * bmx_libssh2_sftp_open(LIBSSH2_SFTP * sftp, BBString * filename, int flags, int mode, int openType);

	size_t bmx_libssh2_sftp_read(LIBSSH2_SFTP_HANDLE * handle, char * buf, size_t length);
	size_t bmx_libssh2_sftp_write(LIBSSH2_SFTP_HANDLE * handle, char * buf, size_t count);
	size_t bmx_libssh2_sftp_tell(LIBSSH2_SFTP_HANDLE * handle);
	void bmx_libssh2_sftp_seek64(LIBSSH2_SFTP_HANDLE * handle, BBInt64 offset);
	int bmx_libssh2_sftp_fsync(LIBSSH2_SFTP_HANDLE * handle);
	int bmx_libssh2_sftp_close_handle(LIBSSH2_SFTP_HANDLE * handle);

}

class MaxSSHSession
{
public:

	MaxSSHSession(BBObject * handle)
		: maxHandle(handle)
	{
		session = libssh2_session_init_ex(NULL, NULL, NULL, this);
	}
	
	~MaxSSHSession()
	{
	}
	
	LIBSSH2_SESSION * Session() {
		return session;
	}
	
	BBObject * Handle() {
		return maxHandle;
	}

private:
	BBObject * maxHandle;
	LIBSSH2_SESSION * session;
};

// ***************************************************

void _interactive_callback(const char *name, int name_len, 
             const char *instruction, int instruction_len, int num_prompts,
             const LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
             LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
             void **abstract) {

	BBArray * _prompts = CB_PREF(bah_libssh2_TSSHSession__newPrompts)(num_prompts);
	BBArray * _responses = CB_PREF(bah_libssh2_TSSHSession__newResponses)(num_prompts);
	
	for (int i = 0; i < num_prompts; i++) {
		CB_PREF(bah_libssh2_TSSHSession__setPrompt)(_prompts, i, prompts + i);
		CB_PREF(bah_libssh2_TSSHSession__setResponse)(_responses, i, responses + i);
	}

	CB_PREF(bah_libssh2_TSSHSession__kbdCallback)(((MaxSSHSession*)*abstract)->Handle(), bbStringFromBytes(name, name_len),
		bbStringFromBytes(instruction, instruction_len), _prompts, _responses);

}

// ***************************************************

MaxSSHSession * bmx_libssh2_session_create(BBObject * handle) {
	return new MaxSSHSession(handle);
}

int bmx_libssh2_session_startup(MaxSSHSession * session, int socket) {
	return libssh2_session_startup(session->Session(), socket);
}

int bmx_libssh2_session_free(MaxSSHSession * session) {
	return libssh2_session_free(session->Session());
}

int bmx_libssh2_userauth_keyboard_interactive(MaxSSHSession * session, BBString * username) {
	char *p = bbStringToCString( username );

	int ret = libssh2_userauth_keyboard_interactive(session->Session(), p, &_interactive_callback);

	bbMemFree( p );
	return ret;
}

LIBSSH2_CHANNEL * bmx_libssh2_channel_open(MaxSSHSession * session) {
	return libssh2_channel_open_session(session->Session());
}

int bmx_libssh2_session_disconnect(MaxSSHSession * session, BBString * description) {
	char *d = bbStringToCString( description );
	int ret = libssh2_session_disconnect(session->Session(), d);
	bbMemFree( d );
	return ret;
}

int bmx_libssh2_userauth_password(MaxSSHSession * session, BBString * username, BBString * password) {
	char *u = bbStringToCString( username );
	char *p = bbStringToCString( password );

	int ret = libssh2_userauth_password(session->Session(), u, p);

	bbMemFree( u );
	bbMemFree( p );
	return ret;
}

int bmx_libssh2_userauth_authenticated(MaxSSHSession * session) {
	return libssh2_userauth_authenticated(session->Session());
}

// ***************************************************

BBString * bmx_libssh2_kbdint_prompt_gettext(LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt) {
	return bbStringFromCString(prompt->text);
}

int bmx_libssh2_kbdint_prompt_echo(LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt) {
	return static_cast<int>(prompt->echo);
}

// ***************************************************

void bmx_libssh2_kbdint_response_settext(LIBSSH2_USERAUTH_KBDINT_RESPONSE * response, BBString * text) {
	char *p = bbStringToCString( text );
	response->text = strdup(p);
	response->length = strlen(p);
	bbMemFree( p );
}


// ***************************************************

int bmx_libssh2_channel_setenv(LIBSSH2_CHANNEL * channel, BBString * name, BBString * value) {
	char *n = bbStringToCString( name );
	char *v = bbStringToCString( value );
	int ret = libssh2_channel_setenv(channel, n, v);
	bbMemFree(n);
	bbMemFree(v);
	return ret;
}

int bmx_libssh2_channel_requestpty(LIBSSH2_CHANNEL * channel, BBString * term) {
	char *t = bbStringToCString( term );
	int ret = libssh2_channel_request_pty(channel, t);
	bbMemFree(t);
	return ret;
}

int bmx_libssh2_channel_shell(LIBSSH2_CHANNEL * channel) {
	return libssh2_channel_shell(channel);
}

int bmx_libssh2_channel_exec(LIBSSH2_CHANNEL * channel, BBString * message) {
	char *m = bbStringToCString( message );
	int ret = libssh2_channel_exec(channel, m);
	bbMemFree(m);
	return ret;
}

int bmx_libssh2_channel_subsystem(LIBSSH2_CHANNEL * channel, BBString * message) {
	char *m = bbStringToCString( message );
	int ret = libssh2_channel_subsystem(channel, m);
	bbMemFree(m);
	return ret;
}

int bmx_libssh2_channel_processstartup(LIBSSH2_CHANNEL * channel, BBString * request, BBString * message) {
	char *r = bbStringToCString( request );
	char *m = bbStringToCString( message );
	int ret = libssh2_channel_process_startup(channel, r, strlen(r), m, strlen(m));
	bbMemFree(r);
	bbMemFree(m);
	return ret;
}

void bmx_libssh2_channel_free(LIBSSH2_CHANNEL * channel) {
	libssh2_channel_free(channel);
}

int bmx_libssh2_channel_read(LIBSSH2_CHANNEL * channel, char * buffer, int size) {
	return static_cast<int>(libssh2_channel_read(channel, buffer, static_cast<size_t>(size)));
}

int bmx_libssh2_channel_read_stderr(LIBSSH2_CHANNEL * channel, char * buffer, int size) {
	return static_cast<int>(libssh2_channel_read_stderr(channel, buffer, static_cast<size_t>(size)));
}

int bmx_libssh2_channel_write(LIBSSH2_CHANNEL * channel, char * buffer, int size) {
	return static_cast<int>(libssh2_channel_write(channel, buffer, static_cast<size_t>(size)));
}

int bmx_libssh2_channel_write_stderr(LIBSSH2_CHANNEL * channel, char * buffer, int size) {
	return static_cast<int>(libssh2_channel_write_stderr(channel, buffer, static_cast<size_t>(size)));
}

int bmx_libssh2_channel_eof(LIBSSH2_CHANNEL * channel) {
	return libssh2_channel_eof(channel);
}

void bmx_libssh2_channel_set_blocking(LIBSSH2_CHANNEL * channel, int blocking) {
	libssh2_channel_set_blocking(channel, blocking);
}

int bmx_libssh2_channel_pollread(LIBSSH2_CHANNEL * channel, int extended) {
	return libssh2_poll_channel_read(channel, extended);
}

int bmx_libssh2_channel_sendeof(LIBSSH2_CHANNEL * channel) {
	return libssh2_channel_send_eof(channel);
}

int bmx_libssh2_channel_waitclosed(LIBSSH2_CHANNEL * channel) {
	return libssh2_channel_wait_closed(channel);
}

int bmx_libssh2_channel_waiteof(LIBSSH2_CHANNEL * channel) {
	return libssh2_channel_wait_eof(channel);
}

int bmx_libssh2_channel_getexitstatus(LIBSSH2_CHANNEL * channel) {
	return libssh2_channel_get_exit_status(channel);
}

LIBSSH2_CHANNEL * bmx_libssh2_channel_directtcpip(MaxSSHSession * session, BBString * host, int port, BBString * shost, int sport) {

	LIBSSH2_CHANNEL * channel;
	
	char *h = bbStringToCString( host );
	char * sh = 0;
	if (shost != &bbEmptyString) {
		sh = bbStringToCString( shost );
	}
	
	if (sh) {
		channel = libssh2_channel_direct_tcpip_ex(session->Session(), h, port, sh, sport);
	} else {
		channel = libssh2_channel_direct_tcpip(session->Session(), h, port);
	}
	
	bbMemFree(h);
	if (sh) bbMemFree(sh);
	
	return channel;
}


// ***************************************************

LIBSSH2_SFTP * bmx_libssh2_sftp_init(MaxSSHSession * session) {
	return libssh2_sftp_init(session->Session());
}

int bmx_libssh2_sftp_shutdown(LIBSSH2_SFTP * sftp) {
	return libssh2_sftp_shutdown(sftp);
}

int bmx_libssh2_sftp_last_error(LIBSSH2_SFTP * sftp) {
	return libssh2_sftp_last_error(sftp);
}

LIBSSH2_CHANNEL * bmx_libssh2_sftp_get_channel(LIBSSH2_SFTP * sftp) {
	return libssh2_sftp_get_channel(sftp);
}

int bmx_libssh2_sftp_rename(LIBSSH2_SFTP * sftp, BBString * sourcefile, BBString * destfile, int flags) {
	char *s = bbStringToUTF8String( sourcefile );
	char *d = bbStringToUTF8String( destfile );
	int res = libssh2_sftp_rename_ex(sftp, s, strlen(s), d, strlen(d), flags);
	bbMemFree(d);
	bbMemFree(s);
	return res;
}

int bmx_libssh2_sftp_mkdir(LIBSSH2_SFTP * sftp, BBString * path, int mode) {
	char *p = bbStringToUTF8String( path );
	int res = libssh2_sftp_mkdir(sftp, p, mode);
	bbMemFree(p);
	return res;
}

int bmx_libssh2_sftp_rmdir(LIBSSH2_SFTP * sftp, BBString * path) {
	char *p = bbStringToUTF8String( path );
	int res = libssh2_sftp_rmdir(sftp, p);
	bbMemFree(p);
	return res;
}

LIBSSH2_SFTP_HANDLE * bmx_libssh2_sftp_open(LIBSSH2_SFTP * sftp, BBString * filename, int flags, int mode, int openType) {
	char *f = bbStringToUTF8String( filename );
	LIBSSH2_SFTP_HANDLE * handle = libssh2_sftp_open_ex(sftp, f, strlen(f), flags, mode, openType);
	bbMemFree(f);
	return handle;
}

// ***************************************************

size_t bmx_libssh2_sftp_read(LIBSSH2_SFTP_HANDLE * handle, char * buf, size_t length) {
	return libssh2_sftp_read(handle, buf, length);
}

size_t bmx_libssh2_sftp_write(LIBSSH2_SFTP_HANDLE * handle, char * buf, size_t count) {
	return libssh2_sftp_write(handle, buf, count);
}

size_t bmx_libssh2_sftp_tell(LIBSSH2_SFTP_HANDLE * handle) {
	return libssh2_sftp_tell(handle);
}

void bmx_libssh2_sftp_seek64(LIBSSH2_SFTP_HANDLE * handle, BBInt64 offset) {
	libssh2_sftp_seek64(handle, offset);
}

int bmx_libssh2_sftp_fsync(LIBSSH2_SFTP_HANDLE * handle) {
	return libssh2_sftp_fsync(handle);
}

int bmx_libssh2_sftp_close_handle(LIBSSH2_SFTP_HANDLE * handle) {
	return libssh2_sftp_close_handle(handle);
}

