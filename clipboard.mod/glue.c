/*
  Copyright (c) 2018 Bruce A Henderson
 
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
#include "libclipboard.h"
#include "brl.mod/blitz.mod/blitz.h"

clipboard_c * bmx_clipboard_new_win32(int maxRetries, int retryDelay) {
	clipboard_opts opts;
	opts.win32.max_retries = maxRetries;
	opts.win32.retry_delay = retryDelay;
	return clipboard_new(&opts);
}

clipboard_c * bmx_clipboard_new_x11(int actionTimeout, int transferSize, BBString * displayName) {
	clipboard_opts opts;
	opts.x11.action_timeout = actionTimeout;
	opts.x11.transfer_size = transferSize;
	if (displayName != &bbEmptyString) {
		opts.x11.display_name = bbStringToUTF8String(displayName);
	}
	clipboard_c * clipboard = clipboard_new(&opts);
	bbMemFree(opts.x11.display_name);
	return clipboard;
}

clipboard_c * bmx_clipboard_new() {
	return clipboard_new(NULL);
}

void bmx_clipboard_free(clipboard_c * clipboard) {
	clipboard_free(clipboard);
}

void bmx_clipboard_clear(clipboard_c * clipboard, int clipboardMode) {
	clipboard_clear(clipboard, clipboardMode);
}

int bmx_clipboard_has_ownership(clipboard_c * clipboard, int clipboardMode) {
	return clipboard_has_ownership(clipboard, clipboardMode);
}

BBString * bmx_clipboard_text(clipboard_c * clipboard) {
	char * text = clipboard_text(clipboard);
	if (text) {
		BBString * ret = bbStringFromUTF8String(text);
		free(text);
		return ret;
	}
	
	return &bbEmptyString;
}

BBString * bmx_clipboard_text_ex(clipboard_c * clipboard, int * length, int clipboardMode) {
	char * text = clipboard_text_ex(clipboard, length, clipboardMode);
	if (text) {
		BBString * ret = bbStringFromUTF8String(text);
		free(text);
		return ret;
	}
	
	return &bbEmptyString;
}

int bmx_clipboard_set_text(clipboard_c * clipboard, BBString * src) {
	if (src == &bbEmptyString) {
		return 0;
	}
	char * s = bbStringToUTF8String(src);
	int ret = clipboard_set_text(clipboard, s);
	bbMemFree(s);
	return ret;
}

int bmx_clipboard_set_text_ex(clipboard_c * clipboard, BBString * src, int clipboardMode) {
	if (src == &bbEmptyString) {
		return 0;
	}
	char * s = bbStringToUTF8String(src);
	int ret = clipboard_set_text_ex(clipboard, s, strlen(s), clipboardMode);
	bbMemFree(s);
	return ret;
}


