/*
 Copyright 2008-2011 Bruce A Henderson

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
#include "control.h"
#include "tessvars.h"
#include "tessedit.h"
#include "pageres.h"
#include "imgs.h"
#include "tprintf.h"
#include "tesseractmain.h"
#include "notdll.h"

#include "ocrclass.h"
#include "baseapi.h"

extern "C" {

#include "blitz.h"

	tesseract::TessBaseAPI * bmx_tess_new();
	void bmx_tess_free(tesseract::TessBaseAPI * api);
	int bmx_tess_init(tesseract::TessBaseAPI * api, BBString * datapath, BBString * lang, int engineMode);
	BBString * bmx_tess_rect(tesseract::TessBaseAPI * api, const unsigned char* imagedata, int bytes_per_pixel, int bytes_per_line,
		int left, int top, int width, int height);
	int bmx_tess_setvariable(tesseract::TessBaseAPI * api, BBString * variable, BBString * value);
	void bmx_tess_clearadaptiveclassifier(tesseract::TessBaseAPI * api);
	void bmx_tess_end(tesseract::TessBaseAPI * api);
	int bmx_tess_isvalidword(tesseract::TessBaseAPI * api, BBString * word);
	int bmx_tess_setvariableifinit(tesseract::TessBaseAPI * api, BBString * name, BBString * value);
	int bmx_tess_getintvariable(tesseract::TessBaseAPI * api, BBString * name, int * value);
	int bmx_tess_getboolvariable(tesseract::TessBaseAPI * api, BBString * name, int * value);
	int bmx_tess_getdoublevariable(tesseract::TessBaseAPI * api, BBString * name, double * value);
	BBString * bmx_tess_getstringvariable(tesseract::TessBaseAPI * api, BBString * name);

}

tesseract::TessBaseAPI * bmx_tess_new() {
	return new tesseract::TessBaseAPI();
}

void bmx_tess_free(tesseract::TessBaseAPI * api) {
	delete api;
}

int bmx_tess_init(tesseract::TessBaseAPI * api, BBString * datapath, BBString * lang, int engineMode) {
	char * d = bbStringToCString(datapath);
	char * p = bbStringToCString(lang);
	
	int result = api->Init(d, p, static_cast<tesseract::OcrEngineMode>(engineMode));

	bbMemFree(d);
	bbMemFree(p);
	
	return result;
}

BBString * bmx_tess_rect(tesseract::TessBaseAPI * api, const unsigned char* imagedata, int bytes_per_pixel, int bytes_per_line,
		int left, int top, int width, int height) {

	char * text = api->TesseractRect(imagedata, bytes_per_pixel, bytes_per_line, left, top, width, height);
	BBString * s = bbStringFromUTF8String(text);
	if (text) {
		delete []text;
	}
	return s;
}

int bmx_tess_setvariable(tesseract::TessBaseAPI * api, BBString * name, BBString * value) {
	char * d = bbStringToCString(name);
	char * p = bbStringToCString(value);
	int res = static_cast<int>(api->SetVariable(d, p));
	bbMemFree(d);
	bbMemFree(p);
	return res;
}

void bmx_tess_clearadaptiveclassifier(tesseract::TessBaseAPI * api) {
	api->ClearAdaptiveClassifier();
}

void bmx_tess_end(tesseract::TessBaseAPI * api) {
	api->End();
}

int bmx_tess_isvalidword(tesseract::TessBaseAPI * api, BBString * word) {
	char * p = bbStringToCString(word);
	int res = api->IsValidWord(p);
	bbMemFree(p);
	return res;
}

int bmx_tess_setvariableifinit(tesseract::TessBaseAPI * api, BBString * name, BBString * value) {
	char * d = bbStringToCString(name);
	char * p = bbStringToCString(value);
	int res = static_cast<int>(api->SetVariableIfInit(d, p));
	bbMemFree(d);
	bbMemFree(p);
	return res;
}

int bmx_tess_getintvariable(tesseract::TessBaseAPI * api, BBString * name, int * value) {
	char * d = bbStringToCString(name);
	int res = static_cast<int>(api->GetIntVariable(d, value));
	bbMemFree(d);
	return res;
}

int bmx_tess_getboolvariable(tesseract::TessBaseAPI * api, BBString * name, int * value) {
	bool v;
	char * d = bbStringToCString(name);
	int res = static_cast<int>(api->GetBoolVariable(d, &v));
	*value = v;
	bbMemFree(d);
	return res;
}

int bmx_tess_getdoublevariable(tesseract::TessBaseAPI * api, BBString * name, double * value) {
	char * d = bbStringToCString(name);
	int res = static_cast<int>(api->GetDoubleVariable(d, value));
	bbMemFree(d);
	return res;
}

BBString * bmx_tess_getstringvariable(tesseract::TessBaseAPI * api, BBString * name) {
	BBString * s = &bbEmptyString;
	char * d = bbStringToCString(name);
	const char * v = api->GetStringVariable(d);
	if (v) {
		s = bbStringFromUTF8String(v);
	}
	bbMemFree(d);
	return s;
}


