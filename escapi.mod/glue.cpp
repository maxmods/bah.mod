#include "escapi.h"
#include <stdlib.h>

extern "C" {

	struct SimpleCapParams * bmx_escapi_params_new();
	void bmx_escapi_params_free(struct SimpleCapParams * params);
	void bmx_escapi_params_SetWidth(struct SimpleCapParams * params, int width);
	void bmx_escapi_params_SetHeight(struct SimpleCapParams * params, int height);
	void bmx_escapi_params_SetBuffer(struct SimpleCapParams * params, int * buffer);
	int * bmx_escapi_params_Buffer(struct SimpleCapParams * params);

	int bmx_escapi_countCaptureDevices();
	int bmx_escapi_initCapture(unsigned int deviceno, struct SimpleCapParams *aParams);
	void bmx_escapi_deinitCapture(unsigned int deviceno);
	void bmx_escapi_doCapture(unsigned int deviceno);
	int bmx_escapi_isCaptureDone(unsigned int deviceno);
	void bmx_escapi_getCaptureDeviceName(unsigned int deviceno, char *namebuffer, int bufferlength);
	int bmx_escapi_ESCAPIVersion();
	float bmx_escapi_getCapturePropertyValue(unsigned int deviceno, int prop);
	int bmx_escapi_getCapturePropertyAuto(unsigned int deviceno, int prop);
	int bmx_escapi_setCaptureProperty(unsigned int deviceno, int prop, float value, int autoval);

}

struct SimpleCapParams * bmx_escapi_params_new() {
	return (struct SimpleCapParams*)calloc(1, sizeof(struct SimpleCapParams));
}

void bmx_escapi_params_free(struct SimpleCapParams * params) {
	free(params);
}

void bmx_escapi_params_SetWidth(struct SimpleCapParams * params, int width) {
	params->mWidth = width;
}

void bmx_escapi_params_SetHeight(struct SimpleCapParams * params, int height) {
	params->mHeight = height;
}

void bmx_escapi_params_SetBuffer(struct SimpleCapParams * params, int * buffer) {
	params->mTargetBuf = buffer;
}

int * bmx_escapi_params_Buffer(struct SimpleCapParams * params) {
	return params->mTargetBuf;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_escapi_countCaptureDevices() {
	return countCaptureDevices();
}

int bmx_escapi_initCapture(unsigned int deviceno, struct SimpleCapParams *aParams) {
	return initCapture(deviceno, aParams);
}

void bmx_escapi_deinitCapture(unsigned int deviceno) {
	deinitCapture(deviceno);
}

void bmx_escapi_doCapture(unsigned int deviceno) {
	doCapture(deviceno);
}

int bmx_escapi_isCaptureDone(unsigned int deviceno) {
	return isCaptureDone(deviceno);
}

void bmx_escapi_getCaptureDeviceName(unsigned int deviceno, char *namebuffer, int bufferlength) {
	getCaptureDeviceName(deviceno, namebuffer, bufferlength);
}

int bmx_escapi_ESCAPIVersion() {
	return ESCAPIVersion();
}

float bmx_escapi_getCapturePropertyValue(unsigned int deviceno, int prop) {
	return getCapturePropertyValue(deviceno, prop);
}

int bmx_escapi_getCapturePropertyAuto(unsigned int deviceno, int prop) {
	return getCapturePropertyAuto(deviceno, prop);
}

int bmx_escapi_setCaptureProperty(unsigned int deviceno, int prop, float value, int autoval) {
	return setCaptureProperty(deviceno, prop, value, autoval);
}


