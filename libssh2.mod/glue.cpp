#include <libssh2.h>

class MaxSSHSession;

extern "C" {

#include "blitz.h"

	void _bah_libssh2_TSSHSession__kbdCallback(BBObject * session, BBString * name, BBString * instruction, 
		BBArray * prompts, BBArray * responses);

	BBArray * _bah_libssh2_TSSHSession__newPrompts(int size);
	void _bah_libssh2_TSSHSession__setPrompt(BBArray * array, int index, const LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt);
	BBArray * _bah_libssh2_TSSHSession__newResponses(int size);
	void _bah_libssh2_TSSHSession__setResponse(BBArray * array, int index, LIBSSH2_USERAUTH_KBDINT_RESPONSE * response);
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

	BBArray * _prompts = _bah_libssh2_TSSHSession__newPrompts(num_prompts);
	BBArray * _responses = _bah_libssh2_TSSHSession__newResponses(num_prompts);
	
	for (int i = 0; i < num_prompts; i++) {
		_bah_libssh2_TSSHSession__setPrompt(_prompts, i, prompts + i);
		_bah_libssh2_TSSHSession__setResponse(_responses, i, responses + i);
	}

	_bah_libssh2_TSSHSession__kbdCallback(((MaxSSHSession*)*abstract)->Handle(), bbStringFromBytes(name, name_len),
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



