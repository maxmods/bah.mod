/*
 Copyright 2008,2009 Bruce A Henderson

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
#include "mfcpch.h"
#include "applybox.h"
#include "control.h"
#include "tessvars.h"
#include "tessedit.h"
#include "pageres.h"
#include "imgs.h"
#include "varabled.h"
#include "tprintf.h"
#include "tesseractmain.h"
#include "notdll.h"

#include "ocrclass.h"
#include "baseapi.h"


extern "C" {

#include "blitz.h"

	BBString * _bah_tesseract_Tess__convertUTF8ToMax(char * text);

	void bmx_tess_init(BBString * datapath, BBString * lang, bool numericMode);
	BBString * bmx_tess_rect(const unsigned char* imagedata, int bytes_per_pixel, int bytes_per_line,
		int left, int top, int width, int height);
	bool bmx_tess_setvariable(BBString * variable, BBString * value);
	void bmx_tess_clear();
	void bmx_tess_end();
	int bmx_tess_isvalidword(BBString * word);
}


void bmx_tess_init(BBString * datapath, BBString * lang, bool numericMode) {
	char * d = bbStringToCString(datapath);
	char * p = bbStringToCString(lang);
	
	TessBaseAPI::SimpleInit(d, p, numericMode);

	bbMemFree(d);
	bbMemFree(p);
}

BBString * bmx_tess_rect(const unsigned char* imagedata, int bytes_per_pixel, int bytes_per_line,
		int left, int top, int width, int height) {

	char * text = TessBaseAPI::TesseractRect(imagedata, bytes_per_pixel, bytes_per_line, left, top, width, height);
	BBString * s = _bah_tesseract_Tess__convertUTF8ToMax(text);
	if (text) {
		delete []text;
	}
	return s;
}

bool bmx_tess_setvariable(BBString * variable, BBString * value) {
	char * d = bbStringToCString(variable);
	char * p = bbStringToCString(value);
	bool res = TessBaseAPI::SetVariable(d, p);
	bbMemFree(d);
	bbMemFree(p);
	return res;
}

void bmx_tess_clear() {
	TessBaseAPI::ClearAdaptiveClassifier();
}

void bmx_tess_end() {
	TessBaseAPI::End();
}

int bmx_tess_isvalidword(BBString * word) {
	char * p = bbStringToCString(word);
	int res = TessBaseAPI::IsValidWord(p);
	bbMemFree(p);
	return res;
}


