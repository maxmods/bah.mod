/*
  Copyright 2010-2013 Bruce A Henderson

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
	: zxing::LuminanceSource(w, h), pixels(p)
{
}

MaxBitmapSource::~MaxBitmapSource()
{
}

zxing::ArrayRef<char> MaxBitmapSource::getRow(int y, zxing::ArrayRef<char> row) const {
  int offset = y * getWidth();
  memcpy(&row[0], &pixels[offset], getWidth());
  return row;
}

zxing::ArrayRef<char> MaxBitmapSource::getMatrix() const {
	int size = getWidth() * getHeight();
	zxing::ArrayRef<char> matrix (size);
	memcpy(&matrix[0], pixels, size);
	return matrix;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

zxing::MultiFormatReader * bmx_barcode_multiFormatReader_create() {
	return new zxing::MultiFormatReader();
}

void bmx_barcode_multiformatreader_free(zxing::MultiFormatReader * reader) {
	delete reader;
}

MaxResult * bmx_barcode_multiFormatReader_decode(zxing::MultiFormatReader * reader, unsigned char * pixels, int width, int height, unsigned int hints) {
	try {

		zxing::Ref<MaxBitmapSource> source(new MaxBitmapSource(pixels, width, height));

		zxing::Ref<zxing::Binarizer> binarizer(NULL);
		binarizer = new zxing::GlobalHistogramBinarizer(source);

		zxing::Ref<zxing::BinaryBitmap> image(new zxing::BinaryBitmap(binarizer));

		zxing::Ref<zxing::Result> result(reader->decode(image, hints));
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

BBArray * bmx_barcode_result_getResultPoints(MaxResult * result) {

	zxing::ArrayRef< zxing::Ref<zxing::ResultPoint> >  points = result->result->getResultPoints();
	int size = points->size() * 2;
	
	BBArray * arr = bbArrayNew1D( "f", size );
	float * f = (float*)BBARRAYDATA(arr, arr->dims);
	int i = 0;
	while (i < size) {
		f[i++] = points[i / 2]->getX();
		f[i++] = points[i / 2]->getY();
	}
	return arr;
}
