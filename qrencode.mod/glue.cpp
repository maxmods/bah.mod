
#include <qrencode.h>


extern "C" {

#include "blitz.h"

	unsigned char * bmx_qrcode_getdata(QRcode * code);
	int bmx_qrcode_getwidth(QRcode * code);
	int bmx_qrcode_getversion(QRcode * code);
	void bmx_qrcode_free(QRcode * code);

	QRcode * bmx_qrcode_encodestring(BBString * text, int version, int level, int casesensitive);

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


QRcode * bmx_qrcode_encodestring(BBString * text, int version, int level, int casesensitive) {
	QRcode * code = 0;
	char * t = bbStringToUTF8String(text);
	
	code = QRcode_encodeString(t, version, (QRecLevel)level, QR_MODE_8, casesensitive);
	
	bbMemFree(t);
	return code;
}

