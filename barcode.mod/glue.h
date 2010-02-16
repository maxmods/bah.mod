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
#include "zxing/MultiFormatReader.h"
#include "zxing/LuminanceSource.h"

#include "zxing/common/GlobalHistogramBinarizer.h"
#include "zxing/Result.h"

class MaxResult;

extern "C" {

#include "blitz.h"

	zxing::MultiFormatReader * bmx_barcode_multiFormatReader_create();
	void bmx_barcode_multiformatreader_free(zxing::MultiFormatReader * reader);
	MaxResult * bmx_barcode_multiFormatReader_decode(zxing::MultiFormatReader * reader, unsigned char * pixels, int width, int height);

	BBString * bmx_barcode_result_getText(MaxResult * result);
	int bmx_barcode_result_getBarcodeFormat(MaxResult * result);
	void bmx_barcode_result_free(MaxResult * result);
	BBArray * bmx_barcode_result_getResultPoints(MaxResult * result);

}



class MaxResult
{
public:
	MaxResult(zxing::Ref<zxing::Result> r);
	~MaxResult();

	zxing::Ref<zxing::Result> result;
};

class MaxBitmapSource : public zxing::LuminanceSource
{
public:
	MaxBitmapSource(unsigned char * p, int w, int h);
	~MaxBitmapSource();

	int getWidth();
	int getHeight();
	unsigned char getPixel(int x, int y);
	unsigned char* copyMatrix();

private:
	int width;
	int height;
	unsigned char * pixels;
};
