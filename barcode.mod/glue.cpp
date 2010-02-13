/*
  Copyright 2010 Bruce A Henderson

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
#include "glue.h"

MaxResult::MaxResult(zxing::Ref<zxing::Result> r)
	: result(r)
{
}

MaxResult::~MaxResult()
{
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxBitmapSource::MaxBitmapSource(unsigned char * p, int w, int h)
	: pixels(p), width(w), height(h)
{
}

MaxBitmapSource::~MaxBitmapSource()
{
}

int MaxBitmapSource::getWidth() {
	return width;
}

int MaxBitmapSource::getHeight() {
	return height;
}

unsigned char MaxBitmapSource::getPixel(int x, int y) {
	return pixels[y * width + x];
}

unsigned char* MaxBitmapSource::copyMatrix() {
	unsigned char* matrix = new unsigned char[width*height];
	memcpy(matrix, pixels, sizeof(unsigned char) * width * height);
	return matrix;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

zxing::MultiFormatReader * bmx_barcode_multiFormatReader_create() {
	return new zxing::MultiFormatReader();
}

void bmx_barcode_multiformatreader_free(zxing::MultiFormatReader * reader) {
	delete reader;
}

MaxResult * bmx_barcode_multiFormatReader_decode(zxing::MultiFormatReader * reader, unsigned char * pixels, int width, int height) {
	try {
	
		zxing::Ref<MaxBitmapSource> source(new MaxBitmapSource(pixels, width, height));
	    
		zxing::Ref<zxing::Binarizer> binarizer(NULL);
		binarizer = new zxing::GlobalHistogramBinarizer(source);
		
		zxing::Ref<zxing::BinaryBitmap> image(new zxing::BinaryBitmap(binarizer));
	
		zxing::Ref<zxing::Result> result(reader->decode(image));
		return new MaxResult(result);
		
	} catch (std::exception & e) {
		bbExThrow((BBObject*)bbStringFromCString(e.what()));
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_barcode_result_getText(MaxResult * result) {
	return bbStringFromUTF8String(result->result->getText()->getText().c_str());
}

int bmx_barcode_result_getBarcodeFormat(MaxResult * result) {
	return static_cast<int>(result->result->getBarcodeFormat());
}

void bmx_barcode_result_free(MaxResult * result) {
	delete result;
}

