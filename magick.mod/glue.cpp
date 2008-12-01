/*
 Copyright (c) 2008 Bruce A Henderson
 
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

#include <list>
#include <Magick++.h>

class MaxMImage;
class MaxMGeometry;
class MaxMColor;

using namespace Magick;

extern "C" {

#include "blitz.h"

	BBObject * _bah_magick_TMCoderInfo__create(BBString * name, BBString * description, bool isReadable, bool isWritable, bool isMultiFrame);
	void _bah_magick_TMCoderInfo__addToList(BBObject * tlist, BBObject * info);

	MaxMImage * bmx_magick_createfromspec(BBString * imageSpec);
	MaxMImage * bmx_magick_image_createfromblob(Blob * blob);
	MaxMImage * bmx_magick_create();

	void bmx_magick_image_adaptivethreshold(MaxMImage * image, const unsigned int width, const unsigned int height, unsigned offset);
	void bmx_magick_image_addnoise(MaxMImage * image, const NoiseType noiseType);
	void bmx_magick_image_addnoisechannel(MaxMImage * image, const ChannelType channel, const NoiseType noiseType);
	void bmx_magick_image_affinetransform(MaxMImage * image, const DrawableAffine &affine );
	void bmx_magick_image_annotate(MaxMImage * image, BBString * text, MaxMGeometry * location );
	void bmx_magick_image_blur(MaxMImage * image, const double radius, const double sigma);
	void bmx_magick_image_blurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma );
	void bmx_magick_image_border(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_channel(MaxMImage * image, const ChannelType channel);
	void bmx_magick_image_channeldepth(MaxMImage * image, const ChannelType channel, const unsigned int depth);
	unsigned int bmx_magick_image_getchanneldepth(MaxMImage * image, const ChannelType channel);
	void bmx_magick_image_charcoal(MaxMImage * image, const double radius, const double sigma);
	void bmx_magick_image_chop(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_colorize(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, MaxMColor * penColor);
	void bmx_magick_image_colorizecolor(MaxMImage * image, const unsigned int opacity, MaxMColor * penColor);
	void bmx_magick_image_comment(MaxMImage * image, BBString * comment);
	void bmx_magick_image_compose(MaxMImage * image, const CompositeOperator compose);
	CompositeOperator bmx_magick_image_getcomposite(MaxMImage * image);
	void bmx_magick_image_flip(MaxMImage * image);
	void bmx_magick_image_flop(MaxMImage * image);
	void bmx_magick_image_rotate(MaxMImage * image, double degrees);
	
	void bmx_magick_image_medianfilter(MaxMImage * image, const double radius);
	void bmx_magick_image_minify(MaxMImage * image);
	void bmx_magick_image_modulate(MaxMImage * image, const double brightness, const double saturation, const double hue);
	void bmx_magick_image_motionblur(MaxMImage * image, const double radius, const double sigma, const double angle);
	void bmx_magick_image_negate(MaxMImage * image, const bool grayscale);
	void bmx_magick_image_normalize(MaxMImage * image);
	void bmx_magick_image_oilpaint(MaxMImage * image, double radius);
	void bmx_magick_image_writefile(MaxMImage * image, BBString * imageSpec);

	void bmx_magick_image_getsize(MaxMImage * image, unsigned int * width, unsigned int * height);
	unsigned int bmx_magick_image_getdepth(MaxMImage * image);
	void bmx_magick_image_writedata(MaxMImage * image, const int x, const int y, const unsigned int columns, const unsigned int rows, BBString * map, const StorageType type, void *pixels);
	void bmx_magick_image_opacity(MaxMImage * image, unsigned int value);
	void bmx_magick_image_size(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_sizetxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_read(MaxMImage * image, BBString * imageSpec);
	void bmx_magick_image_readgeom(MaxMImage * image, MaxMGeometry * geometry, BBString * imageSpec);
	void bmx_magick_image_readgeomtxt(MaxMImage * image, BBString * geometry, BBString * imageSpec);

	Blob * bmx_magick_blob_createfromdata(void * data, int size);

	void bmx_magick_coderinfolist(BBObject * tlist, CoderInfo::MatchType isReadable, CoderInfo::MatchType isWritable, CoderInfo::MatchType isMultiFrame);

}



class MaxMImage
{
public:
	MaxMImage()
		: magickImage()
	{
	}
	
	MaxMImage(Image image)
		: magickImage(image)
	{
	}
	
	~MaxMImage()
	{
	}
	
	Image & image() {
		return magickImage;
	}

private:
	Image magickImage;
};

class MaxMGeometry
{
public:
	MaxMGeometry()
		: magickGeometry()
	{
	}
	
	MaxMGeometry(Geometry geometry)
		: magickGeometry(geometry)
	{
	}
	
	~MaxMGeometry()
	{
	}
	
	Geometry & geometry() {
		return magickGeometry;
	}

private:
	Geometry magickGeometry;
};

class MaxMColor
{
public:
	MaxMColor()
		: magickColor()
	{
	}
	
	MaxMColor(Color color)
		: magickColor(color)
	{
	}
	
	~MaxMColor()
	{
	}
	
	Color & color() {
		return magickColor;
	}

private:
	Color magickColor;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxMImage * bmx_magick_createfromspec(BBString * imageSpec) {
	char *p = bbStringToCString( imageSpec );
	
	Image image(p);
	MaxMImage * mImage = new MaxMImage(image);

	bbMemFree( p );
	return mImage;
}

MaxMImage * bmx_magick_image_createfromblob(Blob * blob) {
	Image image(*blob);
	return new MaxMImage(image);
}

MaxMImage * bmx_magick_create() {
	return new MaxMImage();
}

void bmx_magick_image_adaptivethreshold(MaxMImage * image, const unsigned int width, const unsigned int height, unsigned offset ) {
	image->image().adaptiveThreshold(width, height, offset);
}


void bmx_magick_image_addnoise(MaxMImage * image, const NoiseType noiseType) {
	image->image().addNoise(noiseType);
}

void bmx_magick_image_addnoisechannel(MaxMImage * image, const ChannelType channel, const NoiseType noiseType) {
	image->image().addNoiseChannel(channel, noiseType);
}

void bmx_magick_image_affinetransform(MaxMImage * image, const DrawableAffine &affine ) {
//	image->image9).affineTransform(
}

void bmx_magick_image_annotate(MaxMImage * image, BBString * text, MaxMGeometry * location ) {

}

void bmx_magick_image_blur(MaxMImage * image, const double radius, const double sigma) {
	image->image().blur(radius, sigma);
}

void bmx_magick_image_blurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma ) {
	image->image().blurChannel(channel, radius, sigma);
}
    
void bmx_magick_image_border(MaxMImage * image, MaxMGeometry * geometry) {
	image->image().border(geometry->geometry());
}

void bmx_magick_image_channel(MaxMImage * image, const ChannelType channel) {
	image->image().channel(channel);
}

void bmx_magick_image_channeldepth(MaxMImage * image, const ChannelType channel, const unsigned int depth) {
	image->image().channelDepth(channel, depth);
}

unsigned int bmx_magick_image_getchanneldepth(MaxMImage * image, const ChannelType channel) {
	return image->image().channelDepth(channel);
}

void bmx_magick_image_charcoal(MaxMImage * image, const double radius, const double sigma) {
	image->image().charcoal(radius, sigma);
}

void bmx_magick_image_chop(MaxMImage * image, MaxMGeometry * geometry) {
	image->image().chop(geometry->geometry());
}

void bmx_magick_image_colorize(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, MaxMColor * penColor) {
	image->image().colorize(opacityRed, opacityGreen, opacityBlue, penColor->color());
}

void bmx_magick_image_colorizecolor(MaxMImage * image, const unsigned int opacity, MaxMColor * penColor) {
	image->image().colorize(opacity, penColor->color());
}

void bmx_magick_image_comment(MaxMImage * image, BBString * comment) {
	char *p = bbStringToCString( comment );
	image->image().comment(p);
	bbMemFree( p );
}

void bmx_magick_image_compose(MaxMImage * image, const CompositeOperator compose) {
	image->image().compose(compose);
}

CompositeOperator bmx_magick_image_getcomposite(MaxMImage * image) {
	return image->image().compose();
}


void bmx_magick_image_medianfilter(MaxMImage * image, const double radius) {
	image->image().medianFilter(radius);
}

void bmx_magick_image_minify(MaxMImage * image) {
	image->image().minify();
}

void bmx_magick_image_modulate(MaxMImage * image, const double brightness, const double saturation, const double hue ) {
	image->image().modulate(brightness, saturation, hue);
}

void bmx_magick_image_motionblur(MaxMImage * image, const double radius, const double sigma, const double angle ) {
	image->image().motionBlur(radius, sigma, angle);
}

void bmx_magick_image_negate(MaxMImage * image, const bool grayscale) {
	image->image().negate(grayscale);
}

void bmx_magick_image_normalize(MaxMImage * image) {
	image->image().normalize();
}
	

void bmx_magick_image_oilpaint(MaxMImage * image, double radius) {
	image->image().oilPaint(radius);
}


void bmx_magick_image_writefile(MaxMImage * image, BBString * imageSpec) {
	char *p = bbStringToCString( imageSpec );
	image->image().write(p);
	bbMemFree( p );
}

void bmx_magick_image_getsize(MaxMImage * image, unsigned int * width, unsigned int * height) {
	Geometry g(image->image().size());
	*width = g.width();
	*height = g.height();
}

unsigned int bmx_magick_image_getdepth(MaxMImage * image) {
	return image->image().depth();
}

void bmx_magick_image_writedata(MaxMImage * image, const int x, const int y, const unsigned int columns,
		const unsigned int rows, BBString * map, const StorageType type, void *pixels) {
	char *p = bbStringToCString( map );
	image->image().write(x, y, columns, rows, p, type, pixels);
	bbMemFree(p);
}

void bmx_magick_image_opacity(MaxMImage * image, unsigned int value) {
	image->image().opacity(value);
}

void bmx_magick_image_flip(MaxMImage * image) {
	image->image().flip();
}

void bmx_magick_image_flop(MaxMImage * image) {
	image->image().flop();
}

void bmx_magick_image_rotate(MaxMImage * image, double degrees) {
	image->image().rotate(degrees);
}

void bmx_magick_image_size(MaxMImage * image, MaxMGeometry * geometry) {
	image->image().size(geometry->geometry());
}

void bmx_magick_image_sizetxt(MaxMImage * image, BBString * geometry) {
	char *p = bbStringToCString( geometry );
	image->image().size(p);
	bbMemFree(p);
}

void bmx_magick_image_read(MaxMImage * image, BBString * imageSpec) {
	char *p = bbStringToCString( imageSpec );
	image->image().read(p);
	bbMemFree(p);
}

void bmx_magick_image_readgeom(MaxMImage * image, MaxMGeometry * geometry, BBString * imageSpec) {
	char *p = bbStringToCString( imageSpec );
	image->image().read(geometry->geometry(), p);
	bbMemFree(p);
}

void bmx_magick_image_readgeomtxt(MaxMImage * image, BBString * geometry, BBString * imageSpec) {
	char *g = bbStringToCString( geometry );
	char *i = bbStringToCString( imageSpec );
	image->image().read(g, i);
	bbMemFree(g);
	bbMemFree(i);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Blob * bmx_magick_blob_createfromdata(void * data, int size) {
	return new Blob(data, size);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_magick_coderinfolist(BBObject * tlist, CoderInfo::MatchType isReadable, CoderInfo::MatchType isWritable, CoderInfo::MatchType isMultiFrame) {
	std::list<CoderInfo> coderList; 
	coderInfoList(&coderList, isReadable, isWritable, isMultiFrame);
	
	std::list<CoderInfo>::iterator entry = coderList.begin(); 
	while( entry != coderList.end() ) {
		BBObject * info = _bah_magick_TMCoderInfo__create(
				bbStringFromCString(entry->name().c_str()), bbStringFromCString(entry->description().c_str()),
				entry->isReadable(), entry->isWritable(), entry->isMultiFrame());
		_bah_magick_TMCoderInfo__addToList(tlist, info);
		entry++;
	}
}

