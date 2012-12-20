/*
  Copyright (c) 2012 Bruce A Henderson
 
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
#include <qrencode.h>
#include <errno.h>

extern "C" {

#include "blitz.h"

	unsigned char * bmx_qrcode_getdata(QRcode * code);
	int bmx_qrcode_getwidth(QRcode * code);
	int bmx_qrcode_getversion(QRcode * code);
	void bmx_qrcode_free(QRcode * code);

	QRcode * bmx_qrcode_encodestring(BBString * text, int version, int level, int casesensitive, int encoding);
	QRcode * bmx_qrcode_encodestringMQR(BBString * text, int version, int level, int casesensitive, int encoding);
	QRcode * bmx_qrcode_encodedata(int size, const unsigned char * data, int version, int level);
	QRcode * bmx_qrcode_encodedataMQR(int size, const unsigned char * data, int version, int level);
	
	int bmx_qrcode_lasterror();
}


unsigned char * bmx_qrcode_getdata(QRcode * code) {
	return code->data;
}

int bmx_qrcode_getwidth(QRcode * code) {
	return code->width;
}

int bmx_qrcode_getversion(QRcode * code) {
	return code->version;
}

void bmx_qrcode_free(QRcode * code) {
	QRcode_free(code);
}


QRcode * bmx_qrcode_encodestring(BBString * text, int version, int level, int casesensitive, int encoding) {
	QRcode * code = 0;
	char * t = bbStringToUTF8String(text);
	
	code = QRcode_encodeString(t, version, (QRecLevel)level, (QRencodeMode)encoding, casesensitive);
	
	bbMemFree(t);
	return code;
}

QRcode * bmx_qrcode_encodestringMQR(BBString * text, int version, int level, int casesensitive, int encoding) {
	QRcode * code = 0;
	char * t = bbStringToUTF8String(text);
	
	code = QRcode_encodeStringMQR(t, version, (QRecLevel)level, (QRencodeMode)encoding, casesensitive);
	
	bbMemFree(t);
	return code;
}

QRcode * bmx_qrcode_encodedata(int size, const unsigned char * data, int version, int level) {
	return QRcode_encodeData(size, data, version, (QRecLevel)level);
}

QRcode * bmx_qrcode_encodedataMQR(int size, const unsigned char * data, int version, int level) {
	return QRcode_encodeDataMQR(size, data, version, (QRecLevel)level);
}

int bmx_qrcode_lasterror() {
	return errno;
}
