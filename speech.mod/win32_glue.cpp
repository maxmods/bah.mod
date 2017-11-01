/*
  Copyright (c) 2010-2017 Bruce A Henderson
 
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
#include <brl.mod/blitz.mod/blitz.h>

#include <windows.h>
#include <sapi.h>

extern "C" {
	ISpVoice * bmx_speech_create();
	void bmx_speech_Release(ISpVoice * voice);
	int bmx_speech_Speak(ISpVoice * voice, LPCWSTR text);
	void bmx_speech_GetStatus(ISpVoice * voice, SPVOICESTATUS * status);
	void bmx_speech_Pause(ISpVoice * voice);
	void bmx_speech_Resume(ISpVoice * voice);
	void bmx_speech_SetVolume(ISpVoice * voice, short volume);
	void bmx_speech_GetVolume(ISpVoice * voice, short * volume);
	void bmx_speech_SetRate(ISpVoice * voice, int rate);
	void bmx_speech_GetRate(ISpVoice * voice, int * rate);
	BBArray * bmx_speech_availableVoices();
	void bmx_speech_setVoice(ISpVoice * voice, LPCWSTR text);
}


ISpVoice * bmx_speech_create() {
	ISpVoice * voice = NULL;
	
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&voice);
	
	return voice;
}

void bmx_speech_Release(ISpVoice * voice) {
	voice->Release();
}

int bmx_speech_Speak(ISpVoice * voice, LPCWSTR text) {
	voice->Speak(text, SPF_ASYNC, 0);
}

void bmx_speech_GetStatus(ISpVoice * voice, SPVOICESTATUS * status) {
	voice->GetStatus(status, 0);
}

void bmx_speech_Pause(ISpVoice * voice) {
	voice->Pause();
}

void bmx_speech_Resume(ISpVoice * voice) {
	voice->Resume();
}

void bmx_speech_SetVolume(ISpVoice * voice, short volume) {
	voice->SetVolume((USHORT)volume);
}

void bmx_speech_GetVolume(ISpVoice * voice, short * volume) {
	voice->GetVolume((USHORT*)volume);
}

void bmx_speech_SetRate(ISpVoice * voice, int rate) {
	voice->SetRate(rate);
}

void bmx_speech_GetRate(ISpVoice * voice, int * rate) {
	voice->GetRate((LONG*)rate);
}

HRESULT bmx_speech_GetCategoryFromId(const WCHAR *category_id, ISpObjectTokenCategory **ret, BOOL fCreateIfNotExist = FALSE) {
    ISpObjectTokenCategory *obj_token_cat;
    HRESULT hres;

    hres = ::CoCreateInstance(CLSID_SpObjectTokenCategory, NULL, CLSCTX_ALL, __uuidof(ISpObjectTokenCategory),
            (void**)&obj_token_cat);
    if (FAILED(hres))
        return hres;

    hres = obj_token_cat->SetId(category_id, fCreateIfNotExist);
    if (FAILED(hres)) {
        obj_token_cat->Release();
        return hres;
    }

    *ret = obj_token_cat;
    return S_OK;
}

BBArray * bmx_speech_availableVoices() {
	BBArray * arr = &bbEmptyArray;
	
	ISpObjectTokenCategory * category;

	HRESULT hr = bmx_speech_GetCategoryFromId(SPCAT_VOICES, &category);
	
	if (SUCCEEDED(hr)) {
		IEnumSpObjectTokens * tokens;

		hr = category->EnumTokens(NULL, NULL, &tokens);

		if (SUCCEEDED(hr)) {
			
			ULONG count;
			tokens->GetCount(&count);
			
			arr = bbArrayNew1D("$", count);
			
			ISpObjectToken * token;

			for (int i = 0; i < count; i++) {

				hr = tokens->Next(1, &token, NULL);
				
				WCHAR * text;
				token->GetStringValue(L"", &text);
				
				((BBString**)BBARRAYDATA(arr, 1))[i] = bbStringFromWString((BBChar*)text);
				
				CoTaskMemFree(text);
				
				token->Release();
			}
			
			tokens->Release();
		}

		category->Release();
	}
	
	return arr;
}

void bmx_speech_setVoice(ISpVoice * voice, LPCWSTR text) {

	ISpObjectTokenCategory * category;

	HRESULT hr = bmx_speech_GetCategoryFromId(SPCAT_VOICES, &category);
	
	if (SUCCEEDED(hr)) {
		IEnumSpObjectTokens * tokens;

		hr = category->EnumTokens(NULL, NULL, &tokens);

		if (SUCCEEDED(hr)) {
			
			ISpObjectToken * token;

			while (SUCCEEDED(hr = tokens->Next(1, &token, NULL))) {
				
				WCHAR * name;
				token->GetStringValue(L"", &name);
				
				if (_wcsicmp(name, text) == 0) {
					voice->SetVoice(token);

					CoTaskMemFree(name);
					token->Release();
					
					break;
				}
				
				CoTaskMemFree(name);
				token->Release();
			}
			
			tokens->Release();
		}

		category->Release();
	}

}

