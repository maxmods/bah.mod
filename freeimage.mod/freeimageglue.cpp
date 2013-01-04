/*
 Copyright (c) 2007-2013 Bruce A Henderson
 
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

#include <blitz.h>

#include "FreeImage.h"
#include "Plugin.h"
#include "CacheFile.h"

#include <stdio.h>

class MaxFreeImage;
class MaxMultiFreeImage;
class MaxFreeImageTag;
class MaxRGBQUAD;

extern "C" {

	MaxFreeImage * bmx_freeimage_new(void * handle);
	void bmx_freeimage_delete(MaxFreeImage * freeimage);

	
	unsigned _bah_freeimage_TFreeImage_read(void * maxHandle, void *buffer, int n);
	long _bah_freeimage_TFreeImage_tell(void * maxHandle);
	void _bah_freeimage_TFreeImage_seek(void * maxHandle, int pos);
	unsigned _bah_freeimage_TFreeImage_write(void * maxHandle, void *buffer, int n);
	long _bah_freeimage_TFreeImage_size(void * maxHandle);
	
	int bmx_FreeImage_GetFileTypeFromHandle(MaxFreeImage * freeimage);
	void bmx_freeimage_loadImage(MaxFreeImage * freeimage, int flags);
	BYTE * bmx_freeimage_getImage(MaxFreeImage * freeimage);
	
	BYTE * bmx_freeimage_GetScanline(MaxFreeImage * freeimage, int line);

	
	unsigned bmx_freeimage_GetWidth(MaxFreeImage * freeimage);
	unsigned bmx_freeimage_GetHeight(MaxFreeImage * freeimage);
	unsigned bmx_freeimage_GetBPP(MaxFreeImage * freeimage);
	unsigned bmx_freeimage_GetColorsUsed(MaxFreeImage * freeimage);
	unsigned bmx_freeimage_GetPitch(MaxFreeImage * freeimage);
	
	void _bah_freeimage_TFreeImage_error(int format, const char * message);
	
	FIBITMAP * bmx_freeimage_Rescale(MaxFreeImage * freeimage, int width, int height, FREE_IMAGE_FILTER filter);
	void bmx_freeimage_setBitmap(MaxFreeImage * freeimage, FIBITMAP * newbitmap);
	FIBITMAP * bmx_freeimage_getBitmap(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_MakeThumbnail(MaxFreeImage * freeimage, int size);
	FIBITMAP * bmx_freeimage_GetThumbnail(MaxFreeImage * freeimage);
	
	void bmx_freeimage_self_convertTo32Bits(MaxFreeImage * freeimage);
	void bmx_freeimage_self_convertTo24Bits(MaxFreeImage * freeimage);
	void bmx_freeimage_self_convertTo8Bits(MaxFreeImage * freeimage);
	BOOL bmx_freeimage_isTransparent(MaxFreeImage * freeimage);
	
	FIBITMAP * bmx_freeimage_convertToRGBF(MaxFreeImage * freeimage);	
	FIBITMAP * bmx_freeimage_convertTo32Bits(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_convertTo24Bits(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_convertTo8Bits(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_convertToGreyscale(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_convertTo4Bits(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_ConvertTo16Bits555(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_ConvertTo16Bits565(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_ColorQuantize(MaxFreeImage * freeimage, FREE_IMAGE_QUANTIZE quantize);
	FIBITMAP * bmx_freeimage_ConvertToType(MaxFreeImage * freeimage, FREE_IMAGE_TYPE destType, int scaleLinear);
	FIBITMAP * bmx_freeimage_convertToRGB16(MaxFreeImage * freeimage);	

	
	FIBITMAP * bmx_freeimage_Rotate(MaxFreeImage * freeimage, double angle, RGBQUAD * color);
	FIBITMAP * bmx_freeimage_RotateEx(MaxFreeImage * freeimage, double angle, double xShift, double yShift, double xOrigin, double yOrigin, BOOL useMask);
	
	void bmx_freeimage_FlipHorizontal(MaxFreeImage * freeimage);
	void bmx_freeimage_FlipVertical(MaxFreeImage * freeimage);
	
	void bmx_freeimage_AdjustGamma(MaxFreeImage * freeimage, double gamma);
	void bmx_freeimage_AdjustBrightness(MaxFreeImage * freeimage, double percentage);
	void bmx_freeimage_AdjustContrast(MaxFreeImage * freeimage, double percentage);
	void bmx_freeimage_Invert(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_GetChannel(MaxFreeImage * freeimage, FREE_IMAGE_COLOR_CHANNEL channel);
	BOOL bmx_freeimage_SetChannel(MaxFreeImage * freeimage, MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel);
	FIBITMAP * bmx_freeimage_GetComplexChannel(MaxFreeImage * freeimage, FREE_IMAGE_COLOR_CHANNEL channel);
	BOOL bmx_freeimage_SetComplexChannel(MaxFreeImage * freeimage, MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel);
	
	FIBITMAP * bmx_freeimage_Copy(MaxFreeImage * freeimage, int x0, int y0, int x1, int y1);
	BOOL bmx_freeimage_Paste(MaxFreeImage * freeimage, MaxFreeImage * source, int x, int y, int alpha);

	FREE_IMAGE_COLOR_TYPE bmx_freeimage_GetColorType(MaxFreeImage * freeimage);
	
	FIBITMAP * bmx_freeimage_Clone(MaxFreeImage * freeimage);
	BOOL bmx_freeimage_Save(MaxFreeImage * freeimage, FREE_IMAGE_FORMAT format, int flags);
	
	FIBITMAP * bmx_freeimage_Dither(MaxFreeImage * freeimage, FREE_IMAGE_DITHER algo);
	FIBITMAP * bmx_freeimage_Threshold(MaxFreeImage * freeimage, BYTE t);
	
	FIBITMAP * bmx_freeimage_ConvertFromRawBits(BYTE *bits, int width, int height, int pitch,
		unsigned bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask);
		
	FIBITMAP * bmx_freeimage_ToneMapping(MaxFreeImage * freeimage, FREE_IMAGE_TMO algorithm, double param1, double param2);
	FIBITMAP * bmx_freeimage_TmoDrago03(MaxFreeImage * freeimage, double gamma, double exposure);
	FIBITMAP * bmx_freeimage_TmoReinhard05(MaxFreeImage * freeimage, double intensity, double contrast);
	
	MaxFreeImageTag * bmx_freeimage_GetMetadata(MaxFreeImage * freeimage, FREE_IMAGE_MDMODEL model, BBString * tag);
	unsigned bmx_freeimage_GetMetadataCount(MaxFreeImage * freeimage, FREE_IMAGE_MDMODEL model);
	BOOL bmx_freeimage_SetMetadata(MaxFreeImage * freeimage, FREE_IMAGE_MDMODEL model, BBString * key, MaxFreeImageTag * tag);

	void _bah_freeimage_TMultiFreeImage_error(int format, const char * message);
	MaxMultiFreeImage * bmx_multifreeimage_new(void * handle, BBString * filename, BOOL readOnly, BOOL createNew);
	int bmx_MultiFreeImage_GetFileType(MaxMultiFreeImage * freeimage);
	void bmx_multifreeimage_loadImage(MaxMultiFreeImage * freeimage, int flags);
	void bmx_multifreeimage_newImage(MaxMultiFreeImage * freeimage, int format);
	int bmx_multifreeimage_pageCount(MaxMultiFreeImage * freeimage);
	void bmx_multifreeimage_append(MaxMultiFreeImage * freeimage, MaxFreeImage * image);
	void bmx_multifreeimage_insert(MaxMultiFreeImage * freeimage, MaxFreeImage * image, int page);
	void bmx_multifreeimage_deletePage(MaxMultiFreeImage * freeimage, int page);
	FIBITMAP * bmx_multifreeimage_lockPage(MaxMultiFreeImage * freeimage, int page);
	void bmx_multifreeimage_unlockPage(MaxMultiFreeImage * freeimage, MaxFreeImage * image, BOOL changed);
	BOOL bmx_multifreeimage_movePage(MaxMultiFreeImage * freeimage, int source, int target);
	BOOL bmx_multifreeimage_close(MaxMultiFreeImage * freeimage);
	void bmx_multifreeimage_delete(MaxMultiFreeImage * freeimage);

	void bmx_freeimagetag_free(MaxFreeImageTag * tag);
	BBString * bmx_freeimagetag_getkey(MaxFreeImageTag * tag);
	BBString * bmx_freeimagetag_getdescription(MaxFreeImageTag * tag);
	int bmx_freeimagetag_getid(MaxFreeImageTag * tag);
	int bmx_freeimagetag_gettype(MaxFreeImageTag * tag);
	int bmx_freeimagetag_getcount(MaxFreeImageTag * tag);
	int bmx_freeimagetag_getlength(MaxFreeImageTag * tag);
	const void * bmx_freeimagetag_getvalue(MaxFreeImageTag * tag);
	BOOL bmx_freeimagetag_setkey(MaxFreeImageTag * tag, BBString * key);
	BOOL bmx_freeimagetag_setdescription(MaxFreeImageTag * tag, BBString * description);
	BOOL bmx_freeimagetag_setid(MaxFreeImageTag * tag, int id);
	BOOL bmx_freeimagetag_settype(MaxFreeImageTag * tag, int tagType);
	BOOL bmx_freeimagetag_setcount(MaxFreeImageTag * tag, int count);
	BOOL bmx_freeimagetag_setlength(MaxFreeImageTag * tag, int length);
	BOOL bmx_freeimagetag_setvalue(MaxFreeImageTag * tag, void * value);

	BOOL bmx_freeimage_AdjustCurve(MaxFreeImage * freeimage, BYTE * lut, int channel);
	BOOL bmx_freeimage_AdjustColors(MaxFreeImage * freeimage, double brightness, double contrast, double Gamma, int invert);
	BOOL bmx_freeimage_JPEGCrop(BBString * source, BBString * dest, int _left, int _top, int _right, int _bottom);
	BOOL bmx_freeimage_PreMultiplyWithAlpha(MaxFreeImage * freeimage);
	FIBITMAP * bmx_freeimage_enlargeCanvas(MaxFreeImage * freeimage, int _left, int _top, int _right, int _bottom, MaxRGBQUAD * color, int options);
	BOOL bmx_freeimage_HasBackgroundColor(MaxFreeImage * freeimage);
	MaxRGBQUAD * bmx_freeimage_getBackgroundColor(MaxFreeImage * freeimage);
	BOOL bmx_freeimage_setBackgroundColor(MaxFreeImage * freeimage, MaxRGBQUAD * color);
	MaxRGBQUAD * bmx_freeimage_getPixelColor(MaxFreeImage * freeimage, int x, int y);
	void bmx_freeimage_setPixelColor(MaxFreeImage * freeimage, int x, int y, MaxRGBQUAD * color);

	MaxFreeImageTag * bmx_freeimagetag_create();
	MaxFreeImageTag * bmx_freeimagetag_clone(MaxFreeImageTag * tag);
	BBString * bmx_freeimagetag_tagtostring(MaxFreeImageTag * tag, FREE_IMAGE_MDMODEL model, BBString * make);

	FIMETADATA * bmx_freeimagemetadata_FindFirstMetadata(FREE_IMAGE_MDMODEL model, MaxFreeImage * freeimage, MaxFreeImageTag * tag);
	MaxFreeImageTag * bmx_freeimagemetadata_FindNextMetadata(FIMETADATA * handle);
	void bmx_freeimagemetadata_free(FIMETADATA * handle);

	void bmx_rgbquad_free(MaxRGBQUAD * quad);
	MaxRGBQUAD * bmx_rgbquad_create(int r, int g, int b, int a);
	int bmx_rgbquad_red(MaxRGBQUAD * quad);
	int bmx_rgbquad_green(MaxRGBQUAD * quad);
	int bmx_rgbquad_blue(MaxRGBQUAD * quad);
	int bmx_rgbquad_reserved(MaxRGBQUAD * quad);
	void bmx_rgbquad_setrgba(MaxRGBQUAD * quad, int r, int g, int b, int a);
	void bmx_rgbquad_setred(MaxRGBQUAD * quad, int r);
	void bmx_rgbquad_setgreen(MaxRGBQUAD * quad, int g);
	void bmx_rgbquad_setblue(MaxRGBQUAD * quad, int b);
	void bmx_rgbquad_setalpha(MaxRGBQUAD * quad, int a);

	FREE_IMAGE_TYPE bmx_freeimage_getImageType(MaxFreeImage * freeimage);
	BOOL bmx_freeimage_hasPixels(MaxFreeImage * freeimage);

}

// ++++++++++++++++++++++++++++++++++++++++++++

unsigned DLL_CALLCONV bmx_stream_read(void *buffer, unsigned size, unsigned count, fi_handle handle) {
//printf("read = %d, %d\n", size, count);fflush(stdout);

	// convert to number of bytes to read...
	unsigned fullSize = count * size;
	unsigned actual = _bah_freeimage_TFreeImage_read(handle, buffer, fullSize);

	if ((count != 0) && (actual == fullSize / count)) {
		return count;
	}

	return (actual <= 0) ? 0 : fullSize;
}

unsigned DLL_CALLCONV bmx_stream_write(void *buffer, unsigned size, unsigned count, fi_handle handle) {

	// convert to number of bytes to write...
	unsigned fullSize = count * size;
	unsigned actual = _bah_freeimage_TFreeImage_write(handle, buffer, count * size);
	if ((count != 0) && (actual == fullSize / count)) {
		return count;
	}
	return (actual <= 0) ? 0 : fullSize;
}

long DLL_CALLCONV bmx_stream_tell(fi_handle handle) {
	return _bah_freeimage_TFreeImage_tell(handle);
//printf("tell = %d\n", i);fflush(stdout);
//	return i;
}

int DLL_CALLCONV bmx_stream_seek(fi_handle handle, long offset, int origin) {
//printf("offset, origin = %d, %d\n", offset, origin);fflush(stdout);
	// need to point the offset relative to 0...
	if (origin == SEEK_CUR) {
		offset = bmx_stream_tell(handle) + offset;
	} else if (origin == SEEK_END) {
		offset = _bah_freeimage_TFreeImage_size(handle) - offset;
	}

	_bah_freeimage_TFreeImage_seek(handle, offset);
	return 0;
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	_bah_freeimage_TFreeImage_error((int)fif, message);
}

void MultiFreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) {
	_bah_freeimage_TMultiFreeImage_error((int)fif, message);
}

// +++++++++++++++++++++++++++++++++++++++

class MaxRGBQUAD
{
public:

	MaxRGBQUAD()
	{
		quad = new RGBQUAD;
	}

	MaxRGBQUAD(int r, int g, int b, int a)
	{
		quad = new RGBQUAD;
		quad->rgbRed = r;
		quad->rgbGreen = g;
		quad->rgbBlue = b;
		quad->rgbReserved = a;
	}
	
	MaxRGBQUAD(RGBQUAD & q)
	{
		quad = new RGBQUAD;
		
		quad->rgbRed = q.rgbRed;
		quad->rgbGreen = q.rgbGreen;
		quad->rgbBlue = q.rgbBlue;
		quad->rgbReserved = q.rgbReserved;
	}

	
	~MaxRGBQUAD()
	{
		delete quad;
	}
	
	RGBQUAD * Quad() {
		return quad;
	}

private:
	RGBQUAD * quad;
};

// ++++++++++++++++++++++++++++++++++++++++++

void bmx_rgbquad_free(MaxRGBQUAD * quad) {
	delete quad;
}

MaxRGBQUAD * bmx_rgbquad_create(int r, int g, int b, int a) {
	return new MaxRGBQUAD(r, g, b, a);
}

int bmx_rgbquad_red(MaxRGBQUAD * quad) {
	return static_cast<int>(quad->Quad()->rgbRed);
}

int bmx_rgbquad_green(MaxRGBQUAD * quad) {
	return static_cast<int>(quad->Quad()->rgbGreen);
}

int bmx_rgbquad_blue(MaxRGBQUAD * quad) {
	return static_cast<int>(quad->Quad()->rgbBlue);
}

int bmx_rgbquad_reserved(MaxRGBQUAD * quad) {
	return static_cast<int>(quad->Quad()->rgbReserved);
}

void bmx_rgbquad_setrgba(MaxRGBQUAD * quad, int r, int g, int b, int a) {
	quad->Quad()->rgbRed = static_cast<BYTE>(r);
	quad->Quad()->rgbGreen = static_cast<BYTE>(g);
	quad->Quad()->rgbBlue = static_cast<BYTE>(b);
	quad->Quad()->rgbReserved = static_cast<BYTE>(a);
}

void bmx_rgbquad_setred(MaxRGBQUAD * quad, int r) {
	quad->Quad()->rgbRed = static_cast<BYTE>(r);
}

void bmx_rgbquad_setgreen(MaxRGBQUAD * quad, int g) {
	quad->Quad()->rgbGreen = static_cast<BYTE>(g);
}

void bmx_rgbquad_setblue(MaxRGBQUAD * quad, int b) {
	quad->Quad()->rgbBlue = static_cast<BYTE>(b);
}

void bmx_rgbquad_setalpha(MaxRGBQUAD * quad, int a) {
	quad->Quad()->rgbReserved = static_cast<BYTE>(a);
}


// ++++++++++++++++++++++++++++++++++++++++++++

class MaxFreeImageTag
{
public:
	MaxFreeImageTag(bool newTag)
		: tag(0), ownsTag(newTag)
	{
		if (newTag) {
			tag = FreeImage_CreateTag();
		}
	}
	
	~MaxFreeImageTag()
	{
		if (ownsTag) {
			FreeImage_DeleteTag(tag);
		}
	}
	
	void SetOwner(bool owner) {
		ownsTag = owner;
	}
	
public:
	FITAG * tag;

private:
	bool ownsTag;
};

class MaxFreeImage
{
public:
	MaxFreeImage(void * handle);
	
	int GetFileTypeFromHandle();
	void loadImage(int flags);
	BYTE * getImage();
	FIBITMAP * getBitmap();
	BYTE * GetScanline(int line);
	unsigned GetWidth();
	unsigned GetHeight();
	unsigned GetBPP();
	unsigned GetColorsUsed();
	unsigned GetPitch();
	FIBITMAP * Rescale(int width, int height, FREE_IMAGE_FILTER filter);
	void setBitmap(FIBITMAP * newbitmap);
	FIBITMAP * MakeThumbnail(int size);
	FIBITMAP * GetThumbnail();
	void selfConvertTo32Bits();
	void selfConvertTo24Bits();
	void selfConvertTo8Bits();
	FIBITMAP * ConvertToRGBF();
	FIBITMAP * ConvertTo32Bits();
	FIBITMAP * ConvertTo8Bits();
	FIBITMAP * ConvertTo24Bits();
	FIBITMAP * ConvertToGreyscale();
	FIBITMAP * ConvertToType(FREE_IMAGE_TYPE dst_type, BOOL scale_linear);
	FIBITMAP * ConvertToRGB16();
	BOOL isTransparent();
	FIBITMAP * Rotate(double angle, RGBQUAD * color);
	FIBITMAP * RotateEx(double angle, double xShift, double yShift, double xOrigin, double yOrigin, BOOL useMask);
	void FlipHorizontal();
	void FlipVertical();
	void AdjustGamma(double gamma);
	void AdjustBrightness(double percentage);
	void AdjustContrast(double percentage);
	void Invert();
	FIBITMAP * GetChannel(FREE_IMAGE_COLOR_CHANNEL channel);
	BOOL SetChannel(MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel);
	FIBITMAP * GetComplexChannel(FREE_IMAGE_COLOR_CHANNEL channel);
	BOOL SetComplexChannel(MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel);
	FIBITMAP * Copy(int x0, int y0, int x1, int y1);
	BOOL Paste(MaxFreeImage * source, int x, int y, int alpha);
	FREE_IMAGE_COLOR_TYPE GetColorType();
	FIBITMAP * Clone();
	BOOL Save(FREE_IMAGE_FORMAT format, int flags);
	FIBITMAP * Dither(FREE_IMAGE_DITHER algo);
	FIBITMAP * ConvertTo4Bits();
	FIBITMAP * ConvertTo16Bits555();
	FIBITMAP * ConvertTo16Bits565();
	FIBITMAP * ColorQuantize(FREE_IMAGE_QUANTIZE quantize);
	FIBITMAP * Threshold(BYTE t);
	FIBITMAP * ToneMapping(FREE_IMAGE_TMO algorithm, double param1, double param2);
	FIBITMAP * TmoDrago03(double gamma, double exposure);
	FIBITMAP * TmoReinhard05(double intensity, double contrast);
	void clearBitmap();
	BOOL isFlipped();
	BOOL GetMetadata(FREE_IMAGE_MDMODEL model, const char *key, FITAG **tag);
	unsigned GetMetadataCount(FREE_IMAGE_MDMODEL model);
	BOOL SetMetadata(FREE_IMAGE_MDMODEL model, const char *key, FITAG *tag);
	FIMETADATA * FindFirstMetadata(FREE_IMAGE_MDMODEL model, FITAG **tag);
	BOOL AdjustCurve(BYTE *LUT, FREE_IMAGE_COLOR_CHANNEL channel);
	BOOL AdjustColors(double brightness, double contrast, double gamma, BOOL invert);
	BOOL PreMultiplyWithAlpha();
	FIBITMAP * EnlargeCanvas(int left, int top, int right, int bottom, const void *color, int options);
	BOOL HasBackgroundColor();
	BOOL GetBackgroundColor(RGBQUAD *bkcolor);
	BOOL SetBackgroundColor(RGBQUAD *bkcolor);
	BOOL GetPixelColor(unsigned x, unsigned y, RGBQUAD *value);
	BOOL SetPixelColor(unsigned x, unsigned y, RGBQUAD *value);
	FREE_IMAGE_TYPE GetImageType();
	BOOL HasPixels();

	~MaxFreeImage()
	{
		if (bitmap) {
			FreeImage_Unload(bitmap);
		}
	}

private:
	void * maxHandle;
	FreeImageIO io;
	FREE_IMAGE_FORMAT format;
	FIBITMAP * bitmap;
	BOOL flipped;
};


MaxFreeImage::MaxFreeImage(void * handle)
	: maxHandle(handle), bitmap(0)
{
	io.read_proc  = bmx_stream_read;
	io.write_proc = bmx_stream_write;
	io.seek_proc  = bmx_stream_seek;
	io.tell_proc = bmx_stream_tell;
	
	flipped = FALSE;
	
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
}


int MaxFreeImage::GetFileTypeFromHandle() {
	format = FreeImage_GetFileTypeFromHandle(&io, maxHandle, 0);
	return (int)format;
}

BOOL MaxFreeImage::isFlipped() {
	return flipped;
}

void MaxFreeImage::loadImage(int flags) {
	bitmap = FreeImage_LoadFromHandle(format, &io, maxHandle, flags);
	
//	if (bitmap) {
//	
//		// since Blitz's coordinate system is different, we need to flip the image...
//		FreeImage_FlipVertical(bitmap);
//		flipped = TRUE;
//		
//	}
}

BYTE * MaxFreeImage::getImage() {
	return FreeImage_GetBits(bitmap);
}

FIBITMAP * MaxFreeImage::getBitmap() {
	return bitmap;
}

BYTE * MaxFreeImage::GetScanline(int line) {
	return FreeImage_GetScanLine(bitmap,line);
}


unsigned MaxFreeImage::GetWidth() {
	return FreeImage_GetWidth(bitmap);
}

unsigned MaxFreeImage::GetHeight() {
	return FreeImage_GetHeight(bitmap);
}

unsigned MaxFreeImage::GetBPP() {
	return FreeImage_GetBPP(bitmap);
}

unsigned MaxFreeImage::GetColorsUsed() {
	return FreeImage_GetColorsUsed(bitmap);
}

unsigned MaxFreeImage::GetPitch() {
	return FreeImage_GetPitch(bitmap);
}

FIBITMAP * MaxFreeImage::Rescale(int width, int height, FREE_IMAGE_FILTER filter) {
	return FreeImage_Rescale(bitmap, width, height, filter);
}

void MaxFreeImage::setBitmap(FIBITMAP * newbitmap) {
	if (bitmap) {
		FreeImage_Unload(bitmap);
	}
	bitmap = newbitmap;
}

void MaxFreeImage::clearBitmap() {
	bitmap = 0;
}

FIBITMAP * MaxFreeImage::MakeThumbnail(int size) {
	return FreeImage_MakeThumbnail(bitmap, size, TRUE);
}

FIBITMAP * MaxFreeImage::GetThumbnail() {
	return FreeImage_GetThumbnail(bitmap);
}

void MaxFreeImage::selfConvertTo32Bits() {
	FIBITMAP * newbitmap = ConvertTo32Bits();
	FreeImage_Unload(bitmap);
	bitmap = newbitmap;
}

void MaxFreeImage::selfConvertTo24Bits() {
	FIBITMAP * newbitmap = ConvertTo24Bits();
	FreeImage_Unload(bitmap);
	bitmap = newbitmap;
}

void MaxFreeImage::selfConvertTo8Bits() {
	FIBITMAP * newbitmap = ConvertTo8Bits();
	FreeImage_Unload(bitmap);
	bitmap = newbitmap;
}

FIBITMAP * MaxFreeImage::ConvertToRGBF() {
	return FreeImage_ConvertToRGBF(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertToRGB16() {
	return FreeImage_ConvertToRGB16(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertTo32Bits() {
	return FreeImage_ConvertTo32Bits(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertTo8Bits() {
	return FreeImage_ConvertTo8Bits(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertTo24Bits() {
	return FreeImage_ConvertTo24Bits(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertToGreyscale() {
	return FreeImage_ConvertToGreyscale(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertToType(FREE_IMAGE_TYPE dst_type, BOOL scale_linear) {
	return FreeImage_ConvertToType(bitmap, dst_type, scale_linear);
}

BOOL MaxFreeImage::isTransparent() {
	return FreeImage_IsTransparent(bitmap);
}

FIBITMAP * MaxFreeImage::Rotate(double angle, RGBQUAD * color) {
	return FreeImage_Rotate(bitmap, angle, color);
}

FIBITMAP * MaxFreeImage::RotateEx(double angle, double xShift, double yShift, double xOrigin, double yOrigin, BOOL useMask) {
	return FreeImage_RotateEx(bitmap, angle, xShift, yShift, xOrigin, yOrigin, useMask);
}

void MaxFreeImage::FlipHorizontal() {
	FreeImage_FlipHorizontal(bitmap);
}

void MaxFreeImage::FlipVertical() {
	FreeImage_FlipVertical(bitmap);
}

void MaxFreeImage::AdjustGamma(double gamma) {
	FreeImage_AdjustGamma(bitmap, gamma);
}

void MaxFreeImage::AdjustBrightness(double percentage) {
	FreeImage_AdjustBrightness(bitmap, percentage);
}

void MaxFreeImage::AdjustContrast(double percentage) {
	FreeImage_AdjustContrast(bitmap, percentage);
}

void MaxFreeImage::Invert() {
	FreeImage_Invert(bitmap);
}

FIBITMAP * MaxFreeImage::GetChannel(FREE_IMAGE_COLOR_CHANNEL channel) {
	return FreeImage_GetChannel(bitmap, channel);
}

BOOL MaxFreeImage::SetChannel(MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel) {
	return FreeImage_SetChannel(bitmap, src->getBitmap(), channel);
}

FIBITMAP * MaxFreeImage::GetComplexChannel(FREE_IMAGE_COLOR_CHANNEL channel) {
	return FreeImage_GetComplexChannel(bitmap, channel);
}

BOOL MaxFreeImage::SetComplexChannel(MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel) {
	return FreeImage_SetComplexChannel(bitmap, src->getBitmap(), channel);
}

FIBITMAP * MaxFreeImage::Copy(int x0, int y0, int x1, int y1) {
	return FreeImage_Copy(bitmap, x0, y0, x1, y1);
}

BOOL MaxFreeImage::Paste(MaxFreeImage * source, int x, int y, int alpha) {
	return FreeImage_Paste(bitmap, source->getBitmap(), x, y, alpha);
}

FREE_IMAGE_COLOR_TYPE MaxFreeImage::GetColorType() {
	return FreeImage_GetColorType(bitmap);
}

FIBITMAP * MaxFreeImage::Clone() {
	return FreeImage_Clone(bitmap);
}

BOOL MaxFreeImage::Save(FREE_IMAGE_FORMAT format, int flags) {
	FIBITMAP * unflippedBitmap;
	BOOL cleanup = FALSE;
	
	// do we need to unflip the image for saving?
	if (flipped) {
		unflippedBitmap = FreeImage_Clone(bitmap);
		FreeImage_FlipVertical(unflippedBitmap);
		cleanup = TRUE;
	} else {
		unflippedBitmap = bitmap;
	}

	BOOL ret = FreeImage_SaveToHandle(format, unflippedBitmap, &io, maxHandle, flags);
	
	if (cleanup) {
		FreeImage_Unload(unflippedBitmap);
	}
	
	return ret;
}

FIBITMAP * MaxFreeImage::Dither(FREE_IMAGE_DITHER algo) {
	return FreeImage_Dither(bitmap, algo);
}

FIBITMAP * MaxFreeImage::ConvertTo4Bits() {
	return FreeImage_ConvertTo4Bits(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertTo16Bits555() {
	return FreeImage_ConvertTo16Bits555(bitmap);
}

FIBITMAP * MaxFreeImage::ConvertTo16Bits565() {
	return FreeImage_ConvertTo16Bits565(bitmap);
}

FIBITMAP * MaxFreeImage::ColorQuantize(FREE_IMAGE_QUANTIZE quantize) {
	return FreeImage_ColorQuantize(bitmap, quantize);
}

FIBITMAP * MaxFreeImage::Threshold(BYTE t) {
	return FreeImage_Threshold(bitmap, t);
}

FIBITMAP * MaxFreeImage::ToneMapping(FREE_IMAGE_TMO algorithm, double param1, double param2) {
	return FreeImage_ToneMapping(bitmap, algorithm, param1, param2);
}

FIBITMAP * MaxFreeImage::TmoDrago03(double gamma, double exposure) {
	return FreeImage_TmoDrago03(bitmap, gamma, exposure);
}

FIBITMAP * MaxFreeImage::TmoReinhard05(double intensity, double contrast) {
	return FreeImage_TmoReinhard05(bitmap, intensity, contrast);
}

BOOL MaxFreeImage::GetMetadata(FREE_IMAGE_MDMODEL model, const char *key, FITAG **tag) {
	return FreeImage_GetMetadata(model, bitmap, key, tag);
}

unsigned MaxFreeImage::GetMetadataCount(FREE_IMAGE_MDMODEL model) {
	return FreeImage_GetMetadataCount(model, bitmap);
}

BOOL MaxFreeImage::SetMetadata(FREE_IMAGE_MDMODEL model, const char *key, FITAG *tag) {
	return FreeImage_SetMetadata(model, bitmap, key, tag);
}

FIMETADATA * MaxFreeImage::FindFirstMetadata(FREE_IMAGE_MDMODEL model, FITAG **tag) {
	return FreeImage_FindFirstMetadata(model, bitmap, tag);
}

BOOL MaxFreeImage::AdjustCurve(BYTE *LUT, FREE_IMAGE_COLOR_CHANNEL channel) {
	return FreeImage_AdjustCurve(bitmap, LUT, channel);
}

BOOL MaxFreeImage::AdjustColors(double brightness, double contrast, double gamma, BOOL invert) {
	return FreeImage_AdjustColors(bitmap, brightness, contrast, gamma, invert);
}

BOOL MaxFreeImage::PreMultiplyWithAlpha() {
	return FreeImage_PreMultiplyWithAlpha(bitmap);
}

FIBITMAP * MaxFreeImage::EnlargeCanvas(int left, int top, int right, int bottom, const void *color, int options) {
	return FreeImage_EnlargeCanvas(bitmap, left, top, right, bottom, color, options);
}

BOOL MaxFreeImage::HasBackgroundColor() {
	return FreeImage_HasBackgroundColor(bitmap);
}

BOOL MaxFreeImage::GetBackgroundColor(RGBQUAD *bkcolor) {
	return FreeImage_GetBackgroundColor(bitmap, bkcolor);
}

BOOL MaxFreeImage::SetBackgroundColor(RGBQUAD *bkcolor) {
	return FreeImage_SetBackgroundColor(bitmap, bkcolor);
}

BOOL MaxFreeImage::GetPixelColor(unsigned x, unsigned y, RGBQUAD *value) {
	return FreeImage_GetPixelColor(bitmap, x, y, value);
}

BOOL MaxFreeImage::SetPixelColor(unsigned x, unsigned y, RGBQUAD *value) {
	return FreeImage_SetPixelColor(bitmap, x, y, value);
}

FREE_IMAGE_TYPE MaxFreeImage::GetImageType() {
	return FreeImage_GetImageType(bitmap);
}

BOOL MaxFreeImage::HasPixels() {
	return FreeImage_HasPixels(bitmap);
}

// ++++++++++++++++++++++++++++++++++++++++++


MaxFreeImage * bmx_freeimage_new(void * handle) {
	return new MaxFreeImage(handle);
}

void bmx_freeimage_delete(MaxFreeImage * freeimage) {
	delete freeimage;
}


int bmx_FreeImage_GetFileTypeFromHandle(MaxFreeImage * freeimage) {

	return freeimage->GetFileTypeFromHandle();
	
}

void bmx_freeimage_loadImage(MaxFreeImage * freeimage, int flags) {
	freeimage->loadImage(flags);
}

BYTE * bmx_freeimage_getImage(MaxFreeImage * freeimage) {
	return freeimage->getImage();
}

BYTE * bmx_freeimage_GetScanline(MaxFreeImage * freeimage, int line) {
	return freeimage->GetScanline(line);
}


unsigned bmx_freeimage_GetWidth(MaxFreeImage * freeimage) {
	return freeimage->GetWidth();
}

unsigned bmx_freeimage_GetHeight(MaxFreeImage * freeimage) {
	return freeimage->GetHeight();
}

unsigned bmx_freeimage_GetBPP(MaxFreeImage * freeimage) {
	return freeimage->GetBPP();
}

unsigned bmx_freeimage_GetColorsUsed(MaxFreeImage * freeimage) {
	return freeimage->GetColorsUsed();
}

unsigned bmx_freeimage_GetPitch(MaxFreeImage * freeimage) {
	return freeimage->GetPitch();
}

FIBITMAP * bmx_freeimage_Rescale(MaxFreeImage * freeimage, int width, int height, FREE_IMAGE_FILTER filter) {
	return freeimage->Rescale(width, height, filter);
}

void bmx_freeimage_setBitmap(MaxFreeImage * freeimage, FIBITMAP * newbitmap) {
	freeimage->setBitmap(newbitmap);
}

FIBITMAP * bmx_freeimage_getBitmap(MaxFreeImage * freeimage) {
	return freeimage->getBitmap();
}

FIBITMAP * bmx_freeimage_MakeThumbnail(MaxFreeImage * freeimage, int size) {
	return freeimage->MakeThumbnail(size);
}

FIBITMAP * bmx_freeimage_GetThumbnail(MaxFreeImage * freeimage) {
	return freeimage->GetThumbnail();
}

void bmx_freeimage_self_convertTo32Bits(MaxFreeImage * freeimage) {
	freeimage->selfConvertTo32Bits();
}

void bmx_freeimage_self_convertTo24Bits(MaxFreeImage * freeimage) {
	freeimage->selfConvertTo24Bits();
}

void bmx_freeimage_self_convertTo8Bits(MaxFreeImage * freeimage) {
	freeimage->selfConvertTo8Bits();
}

FIBITMAP * bmx_freeimage_ConvertToType(MaxFreeImage * freeimage, FREE_IMAGE_TYPE destType, int scaleLinear) {
	freeimage->ConvertToType(destType, scaleLinear);
}

BOOL bmx_freeimage_isTransparent(MaxFreeImage * freeimage) {
	return freeimage->isTransparent();
}

FIBITMAP * bmx_freeimage_Rotate(MaxFreeImage * freeimage, double angle, RGBQUAD * color) {
	return freeimage->Rotate(angle, color);
}

FIBITMAP * bmx_freeimage_RotateEx(MaxFreeImage * freeimage, double angle, double xShift, double yShift, double xOrigin, double yOrigin, BOOL useMask) {
	return freeimage->RotateEx(angle, xShift, yShift, xOrigin, yOrigin, useMask);
}

void bmx_freeimage_FlipHorizontal(MaxFreeImage * freeimage) {
	freeimage->FlipHorizontal();
}

void bmx_freeimage_FlipVertical(MaxFreeImage * freeimage) {
	freeimage->FlipVertical();
}

void bmx_freeimage_AdjustGamma(MaxFreeImage * freeimage, double gamma) {
	freeimage->AdjustGamma(gamma);
}

void bmx_freeimage_AdjustBrightness(MaxFreeImage * freeimage, double percentage) {
	freeimage->AdjustBrightness(percentage);
}

void bmx_freeimage_AdjustContrast(MaxFreeImage * freeimage, double percentage) {
	freeimage->AdjustContrast(percentage);
}

void bmx_freeimage_Invert(MaxFreeImage * freeimage) {
	freeimage->Invert();
}

FIBITMAP * bmx_freeimage_GetChannel(MaxFreeImage * freeimage, FREE_IMAGE_COLOR_CHANNEL channel) {
	return freeimage->GetChannel(channel);
}

BOOL bmx_freeimage_SetChannel(MaxFreeImage * freeimage, MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel) {
	return freeimage->SetChannel(src, channel);
}

FIBITMAP * bmx_freeimage_GetComplexChannel(MaxFreeImage * freeimage, FREE_IMAGE_COLOR_CHANNEL channel) {
	return freeimage->GetComplexChannel(channel);
}

BOOL bmx_freeimage_SetComplexChannel(MaxFreeImage * freeimage, MaxFreeImage * src, FREE_IMAGE_COLOR_CHANNEL channel) {
	return freeimage->SetComplexChannel(src, channel);
}

FIBITMAP * bmx_freeimage_Copy(MaxFreeImage * freeimage, int x0, int y0, int x1, int y1) {
	return freeimage->Copy(x0, y0, x1, y1);
}

BOOL bmx_freeimage_Paste(MaxFreeImage * freeimage, MaxFreeImage * source, int x, int y, int alpha) {
	return freeimage->Paste(source, x, y, alpha);
}

FREE_IMAGE_COLOR_TYPE bmx_freeimage_GetColorType(MaxFreeImage * freeimage) {
	return freeimage->GetColorType();
}

FIBITMAP * bmx_freeimage_convertToRGBF(MaxFreeImage * freeimage) {
	return freeimage->ConvertToRGBF();
}

FIBITMAP * bmx_freeimage_convertToRGB16(MaxFreeImage * freeimage) {
	return freeimage->ConvertToRGB16();
}

FIBITMAP * bmx_freeimage_convertTo32Bits(MaxFreeImage * freeimage) {
	return freeimage->ConvertTo32Bits();
}

FIBITMAP * bmx_freeimage_convertTo24Bits(MaxFreeImage * freeimage) {
	return freeimage->ConvertTo24Bits();
}

FIBITMAP * bmx_freeimage_convertTo8Bits(MaxFreeImage * freeimage) {
	return freeimage->ConvertTo8Bits();
}

FIBITMAP * bmx_freeimage_convertToGreyscale(MaxFreeImage * freeimage) {
	return freeimage->ConvertToGreyscale();
}

FIBITMAP * bmx_freeimage_Clone(MaxFreeImage * freeimage) {
	return freeimage->Clone();
}

BOOL bmx_freeimage_Save(MaxFreeImage * freeimage, FREE_IMAGE_FORMAT format, int flags) {
	return freeimage->Save(format, flags);
}

FIBITMAP * bmx_freeimage_Dither(MaxFreeImage * freeimage, FREE_IMAGE_DITHER algo) {
	return freeimage->Dither(algo);
}

FIBITMAP * bmx_freeimage_ConvertFromRawBits(BYTE *bits, int width, int height, int pitch,
		unsigned bpp, unsigned red_mask, unsigned green_mask, unsigned blue_mask) {

	return FreeImage_ConvertFromRawBits(bits, width, height, pitch, bpp, red_mask, green_mask, 
		blue_mask, TRUE);
		
}

FIBITMAP * bmx_freeimage_convertTo4Bits(MaxFreeImage * freeimage) {
	return freeimage->ConvertTo4Bits();
}

FIBITMAP * bmx_freeimage_ConvertTo16Bits555(MaxFreeImage * freeimage) {
	return freeimage->ConvertTo16Bits555();
}

FIBITMAP * bmx_freeimage_ConvertTo16Bits565(MaxFreeImage * freeimage) {
	return freeimage->ConvertTo16Bits565();
}

FIBITMAP * bmx_freeimage_ColorQuantize(MaxFreeImage * freeimage, FREE_IMAGE_QUANTIZE quantize) {
	return freeimage->ColorQuantize(quantize);
}

FIBITMAP * bmx_freeimage_Threshold(MaxFreeImage * freeimage, BYTE t) {
	return freeimage->Threshold(t);
}

FIBITMAP * bmx_freeimage_ToneMapping(MaxFreeImage * freeimage, FREE_IMAGE_TMO algorithm, double param1, double param2) {
	return freeimage->ToneMapping(algorithm, param1, param2);
}

FIBITMAP * bmx_freeimage_TmoDrago03(MaxFreeImage * freeimage, double gamma, double exposure) {
	return freeimage->TmoDrago03(gamma, exposure);
}

FIBITMAP * bmx_freeimage_TmoReinhard05(MaxFreeImage * freeimage, double intensity, double contrast) {
	return freeimage->TmoReinhard05(intensity, contrast);
}

MaxFreeImageTag * bmx_freeimage_GetMetadata(MaxFreeImage * freeimage, FREE_IMAGE_MDMODEL model, BBString * tag) {
	MaxFreeImageTag * metatag = new MaxFreeImageTag(false);
	
	char * t = bbStringToCString(tag);
	
	BOOL res = freeimage->GetMetadata(model, t, &metatag->tag);
	
	bbMemFree(t);
	
	if (!res) {
		delete metatag;
		return NULL;
	} else {
		return metatag;
	}
}

unsigned bmx_freeimage_GetMetadataCount(MaxFreeImage * freeimage, FREE_IMAGE_MDMODEL model) {
	return freeimage->GetMetadataCount(model);
}

BOOL bmx_freeimage_SetMetadata(MaxFreeImage * freeimage, FREE_IMAGE_MDMODEL model, BBString * key, MaxFreeImageTag * tag) {
	char * k = NULL;
	if (key != &bbEmptyString) {
		k = bbStringToCString(key);
	}
	
	BOOL res;
	
	if (tag) {
		res = freeimage->SetMetadata(model, k, tag->tag);
	} else {
		res = freeimage->SetMetadata(model, k, NULL);
	}
	
	if (k) bbMemFree(k);
	
	return res;
}

BOOL bmx_freeimage_AdjustCurve(MaxFreeImage * freeimage, BYTE * lut, int channel) {
	return freeimage->AdjustCurve(lut, static_cast<FREE_IMAGE_COLOR_CHANNEL>(channel));
}

BOOL bmx_freeimage_AdjustColors(MaxFreeImage * freeimage, double brightness, double contrast, double Gamma, int invert) {
	return freeimage->AdjustColors(brightness, contrast, Gamma, static_cast<BOOL>(invert));
}

BOOL bmx_freeimage_JPEGCrop(BBString * source, BBString * dest, int _left, int _top, int _right, int _bottom) {
#ifdef WIN32
	wchar_t *s = (wchar_t *)bbStringToWString( source );
	wchar_t *d = (wchar_t *)bbStringToWString( dest );
	BOOL res = FreeImage_JPEGCropU(s, d, _left, _top, _right, _bottom);
	bbMemFree(d);
	bbMemFree(s);
	return res;
#else
	char *s = bbStringToCString( source );
	char *d = bbStringToCString( dest );
	BOOL res = FreeImage_JPEGCrop(s, d, _left, _top, _right, _bottom);
	bbMemFree(d);
	bbMemFree(s);
	return res;
#endif
}

BOOL bmx_freeimage_PreMultiplyWithAlpha(MaxFreeImage * freeimage) {
	return freeimage->PreMultiplyWithAlpha();
}

FIBITMAP * bmx_freeimage_enlargeCanvas(MaxFreeImage * freeimage, int _left, int _top, int _right, int _bottom, MaxRGBQUAD * color, int options) {
	return freeimage->EnlargeCanvas(_left, _top, _right, _bottom, color->Quad(), options);
}

BOOL bmx_freeimage_HasBackgroundColor(MaxFreeImage * freeimage) {
	return freeimage->HasBackgroundColor();
}

MaxRGBQUAD * bmx_freeimage_getBackgroundColor(MaxFreeImage * freeimage) {
	MaxRGBQUAD * quad = new MaxRGBQUAD();
	BOOL res = freeimage->GetBackgroundColor(quad->Quad());
	if (!res) {
		delete quad;
		return 0;
	}
	return quad;
}

BOOL bmx_freeimage_setBackgroundColor(MaxFreeImage * freeimage, MaxRGBQUAD * color) {
	return freeimage->SetBackgroundColor(color->Quad());
}

MaxRGBQUAD * bmx_freeimage_getPixelColor(MaxFreeImage * freeimage, int x, int y) {
	MaxRGBQUAD * quad = new MaxRGBQUAD();
	BOOL res = freeimage->GetPixelColor(x, y, quad->Quad());
	if (!res) {
		delete quad;
		return 0;
	}
	return quad;
}

void bmx_freeimage_setPixelColor(MaxFreeImage * freeimage, int x, int y, MaxRGBQUAD * color) {
	freeimage->SetPixelColor(x, y, color->Quad());
}

FREE_IMAGE_TYPE bmx_freeimage_getImageType(MaxFreeImage * freeimage) {
	return freeimage->GetImageType();
}

BOOL bmx_freeimage_hasPixels(MaxFreeImage * freeimage) {
	return freeimage->HasPixels();
}


// ***********************************

class MaxMultiFreeImage
{
public:
	MaxMultiFreeImage(void * handle, char * fname, BOOL ro, BOOL cn);
	
	int GetFileType();
	void loadImage(int flags);
	void newImage(int f);
	int pageCount();
	void append(FIBITMAP * image);
	void insert(FIBITMAP * image, int page);
	void deletePage(int page);
	FIBITMAP * lockPage(int page);
	void unlockPage(FIBITMAP * image, BOOL changed);
	BOOL movePage(int source, int target);
	BOOL close();

	~MaxMultiFreeImage()
	{
		if (bitmap) {
			close();
		}
		if (filename) {
			bbMemFree(filename);
		}
	}

private:
	void * maxHandle;
	FREE_IMAGE_FORMAT format;
	FIMULTIBITMAP * bitmap;
	char *  filename;
	BOOL readOnly;
	BOOL createNew;
};

MaxMultiFreeImage::MaxMultiFreeImage(void * handle, char * fname, BOOL ro, BOOL cn)
	: maxHandle(handle), filename(fname), readOnly(ro), bitmap(0), createNew(cn)
{
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
}

int MaxMultiFreeImage::GetFileType() {
	format = FreeImage_GetFileType(filename, 0);
	return (int)format;
}

void MaxMultiFreeImage::loadImage(int flags) {
	bitmap = FreeImage_OpenMultiBitmap(format, filename, FALSE, readOnly, TRUE, flags);
}

void MaxMultiFreeImage::newImage(int f) {
	format = (FREE_IMAGE_FORMAT)f;
	bitmap = FreeImage_OpenMultiBitmap(format, filename, createNew, readOnly, TRUE, 0);
}

int MaxMultiFreeImage::pageCount() {
	return FreeImage_GetPageCount(bitmap);
}

void MaxMultiFreeImage::append(FIBITMAP * image) {
	FreeImage_AppendPage(bitmap, image);
}

void MaxMultiFreeImage::insert(FIBITMAP * image, int page) {
	FreeImage_InsertPage(bitmap, page, image);
}

void MaxMultiFreeImage::deletePage(int page) {
	FreeImage_DeletePage(bitmap, page);
}

FIBITMAP * MaxMultiFreeImage::lockPage(int page) {
	return FreeImage_LockPage(bitmap, page);
}

void MaxMultiFreeImage::unlockPage(FIBITMAP * image, BOOL changed) {
	FreeImage_UnlockPage(bitmap, image, changed);
}

BOOL MaxMultiFreeImage::movePage(int source, int target) {
	return FreeImage_MovePage(bitmap, target, source);
}

BOOL MaxMultiFreeImage::close() {
	BOOL res = FreeImage_CloseMultiBitmap(bitmap, 0);
	bitmap = 0;
	return res;
}



MaxMultiFreeImage * bmx_multifreeimage_new(void * handle, BBString * filename, BOOL readOnly, BOOL createNew) {
	char *p = bbStringToCString( filename );
	return new MaxMultiFreeImage(handle, p, readOnly, createNew);
}

int bmx_MultiFreeImage_GetFileType(MaxMultiFreeImage * freeimage) {
	return freeimage->GetFileType();
}

void bmx_multifreeimage_loadImage(MaxMultiFreeImage * freeimage, int flags) {
	freeimage->loadImage(flags);
}

void bmx_multifreeimage_newImage(MaxMultiFreeImage * freeimage, int format) {
	freeimage->newImage(format);
}

int bmx_multifreeimage_pageCount(MaxMultiFreeImage * freeimage) {
	return freeimage->pageCount();
}

void bmx_multifreeimage_append(MaxMultiFreeImage * freeimage, MaxFreeImage * image) {
	freeimage->append(image->getBitmap());
}

void bmx_multifreeimage_insert(MaxMultiFreeImage * freeimage, MaxFreeImage * image, int page) {
	freeimage->insert(image->getBitmap(), page);
}

void bmx_multifreeimage_deletePage(MaxMultiFreeImage * freeimage, int page) {
	freeimage->deletePage(page);
}

FIBITMAP * bmx_multifreeimage_lockPage(MaxMultiFreeImage * freeimage, int page) {
	return freeimage->lockPage(page);
}

void bmx_multifreeimage_unlockPage(MaxMultiFreeImage * freeimage, MaxFreeImage * image, BOOL changed) {
	freeimage->unlockPage(image->getBitmap(), changed);
	image->clearBitmap();
}

BOOL bmx_multifreeimage_movePage(MaxMultiFreeImage * freeimage, int source, int target) {
	return freeimage->movePage(source, target);
}

BOOL bmx_multifreeimage_close(MaxMultiFreeImage * freeimage) {
	return freeimage->close();
}

void bmx_multifreeimage_delete(MaxMultiFreeImage * freeimage) {
	delete freeimage;
}

// +++++++++++++++++++++++++++++++++++++++

void bmx_freeimagetag_free(MaxFreeImageTag * tag) {
	delete tag;
}

BBString * bmx_freeimagetag_getkey(MaxFreeImageTag * tag) {
	return bbStringFromCString(FreeImage_GetTagKey(tag->tag));
}

BBString * bmx_freeimagetag_getdescription(MaxFreeImageTag * tag) {
	const char * s = FreeImage_GetTagDescription(tag->tag);
	if (s) {
		return bbStringFromCString(s);
	} else {
		return &bbEmptyString;
	}
}

int bmx_freeimagetag_getid(MaxFreeImageTag * tag) {
	return static_cast<int>(FreeImage_GetTagID(tag->tag));
}

int bmx_freeimagetag_gettype(MaxFreeImageTag * tag) {
	return static_cast<int>(FreeImage_GetTagType(tag->tag));
}

int bmx_freeimagetag_getcount(MaxFreeImageTag * tag) {
	return static_cast<int>(FreeImage_GetTagCount(tag->tag));
}

int bmx_freeimagetag_getlength(MaxFreeImageTag * tag) {
	return static_cast<int>(FreeImage_GetTagLength(tag->tag));
}

const void * bmx_freeimagetag_getvalue(MaxFreeImageTag * tag) {
	return FreeImage_GetTagValue(tag->tag);
}

BOOL bmx_freeimagetag_setkey(MaxFreeImageTag * tag, BBString * key) {
	char * k = bbStringToCString(key);
	BOOL res = FreeImage_SetTagKey(tag->tag, k);
	bbMemFree(k);
	return res;
}

BOOL bmx_freeimagetag_setdescription(MaxFreeImageTag * tag, BBString * description) {
	char * d = bbStringToCString(description);
	BOOL res = FreeImage_SetTagDescription(tag->tag, d);
	bbMemFree(d);
	return res;
}

BOOL bmx_freeimagetag_setid(MaxFreeImageTag * tag, int id) {
	return FreeImage_SetTagID(tag->tag, static_cast<WORD>(id));
}

BOOL bmx_freeimagetag_settype(MaxFreeImageTag * tag, int tagType) {
	return FreeImage_SetTagType(tag->tag, static_cast<FREE_IMAGE_MDTYPE>(tagType));
}

BOOL bmx_freeimagetag_setcount(MaxFreeImageTag * tag, int count) {
	return FreeImage_SetTagCount(tag->tag, static_cast<DWORD>(count));
}

BOOL bmx_freeimagetag_setlength(MaxFreeImageTag * tag, int length) {
	return FreeImage_SetTagLength(tag->tag, static_cast<DWORD>(length));
}

BOOL bmx_freeimagetag_setvalue(MaxFreeImageTag * tag, void * value) {
	return FreeImage_SetTagValue(tag->tag, value);
}


MaxFreeImageTag * bmx_freeimagetag_create() {
	MaxFreeImageTag * metatag = new MaxFreeImageTag(true);
	
	if (metatag->tag) {
		return metatag;
	} else {
		delete metatag;
		return NULL;
	}
	
}

MaxFreeImageTag * bmx_freeimagetag_clone(MaxFreeImageTag * tag) {
	MaxFreeImageTag * metatag = new MaxFreeImageTag(false);
	
	metatag->tag = FreeImage_CloneTag(tag->tag);
	
	if (! metatag->tag) {
		delete metatag;
		return NULL;
	} else {
		metatag->SetOwner(true);
		return metatag;
	}
}

BBString * bmx_freeimagetag_tagtostring(MaxFreeImageTag * tag, FREE_IMAGE_MDMODEL model, BBString * make) {
	const char * s = FreeImage_TagToString(model, tag->tag);
	
	if (s) {
		return bbStringFromCString(s);
	} else {
		return &bbEmptyString;
	}
}

// +++++++++++++++++++++++++++++++++++++++

FIMETADATA * bmx_freeimagemetadata_FindFirstMetadata(FREE_IMAGE_MDMODEL model, MaxFreeImage * freeimage, MaxFreeImageTag * metatag) {
	// delete our temporary tag
	metatag->SetOwner(false);
	FreeImage_DeleteTag(metatag->tag);
	
	return freeimage->FindFirstMetadata(model, &metatag->tag);
}

MaxFreeImageTag * bmx_freeimagemetadata_FindNextMetadata(FIMETADATA * handle) {
	MaxFreeImageTag * metatag = new MaxFreeImageTag(false);

	BOOL res = FreeImage_FindNextMetadata(handle, &metatag->tag);
	
	if (! metatag->tag) {
		delete metatag;
		return NULL;
	} else {
		return metatag;
	}
}

void bmx_freeimagemetadata_free(FIMETADATA * handle) {
	FreeImage_FindCloseMetadata(handle);
}

