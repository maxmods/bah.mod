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

	MaxSSHSession * bmx_libssh2_session_create(BBObject * handle);
	int bmx_libssh2_session_startup(MaxSSHSession * session, int socket);
	int bmx_libssh2_session_free(MaxSSHSession * session);
	
	int bmx_libssh2_userauth_keyboard_interactive(MaxSSHSession * session, BBString * username);

	BBString * bmx_libssh2_kbdint_prompt_gettext(LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt);
	int bmx_libssh2_kbdint_prompt_echo(LIBSSH2_USERAUTH_KBDINT_PROMPT * prompt);

	void bmx_libssh2_kbdint_response_settext(LIBSSH2_USERAUTH_KBDINT_RESPONSE * response, BBString * text);

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


