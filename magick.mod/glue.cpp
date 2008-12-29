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
	void bmx_magick_image_annotatetxt(MaxMImage * image, BBString * text, BBString * location);
	void bmx_magick_image_blur(MaxMImage * image, const double radius, const double sigma);
	void bmx_magick_image_blurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma );
	void bmx_magick_image_border(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_bordertxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_channel(MaxMImage * image, const ChannelType channel);
	void bmx_magick_image_channeldepth(MaxMImage * image, const ChannelType channel, const unsigned int depth);
	unsigned int bmx_magick_image_getchanneldepth(MaxMImage * image, const ChannelType channel);
	void bmx_magick_image_charcoal(MaxMImage * image, const double radius, const double sigma);
	void bmx_magick_image_chop(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_choptxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_colorize(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, MaxMColor * penColor);
	void bmx_magick_image_colorizetxt(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, BBString * penColor);
	void bmx_magick_image_colorizecolor(MaxMImage * image, const unsigned int opacity, MaxMColor * penColor);
	void bmx_magick_image_colorizecolortxt(MaxMImage * image, const unsigned int opacity, BBString * penColor);
	void bmx_magick_image_comment(MaxMImage * image, BBString * comment);
	void bmx_magick_image_compose(MaxMImage * image, const CompositeOperator compose);
	CompositeOperator bmx_magick_image_getcomposite(MaxMImage * image);
	MaxMImage * bmx_magick_image_copy(MaxMImage * image);
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
	void bmx_magick_image_zoom(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_zoomtxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_pixelcolor(MaxMImage * image, unsigned int x, unsigned int y, MaxMColor * color);
	void bmx_magick_image_pixelcolortxt(MaxMImage * image, unsigned int x, unsigned int y, BBString * color);
	void bmx_magick_image_strokecolor(MaxMImage * image, MaxMColor * color);
	void bmx_magick_image_strokecolortxt(MaxMImage * image, BBString * color);
	void bmx_magick_image_colormap(MaxMImage * image, unsigned int index, MaxMColor * color);
	void bmx_magick_image_colormaptxt(MaxMImage * image, unsigned int index, BBString * color);
	void bmx_magick_image_backgroundcolor(MaxMImage * image, MaxMColor * color);
	void bmx_magick_image_backgroundcolortxt(MaxMImage * image, BBString * color);
	void bmx_magick_image_bordercolor(MaxMImage * image, MaxMColor * color);
	void bmx_magick_image_bordercolortxt(MaxMImage * image, BBString * color);
	void bmx_magick_image_crop(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_croptxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_cyclecolormap(MaxMImage * image, int amount);
	void bmx_magick_image_despeckle(MaxMImage * image);
	void bmx_magick_image_edge(MaxMImage * image, double radius);
	void bmx_magick_image_emboss(MaxMImage * image, double radius, double sigma);
	void bmx_magick_image_enhance(MaxMImage * image);
	void bmx_magick_image_equalize(MaxMImage * image);
	void bmx_magick_image_erase(MaxMImage * image);
	void bmx_magick_image_adjoin(MaxMImage * image, bool flag);
	bool bmx_magick_image_getadjoin(MaxMImage * image);
	void bmx_magick_image_antialias(MaxMImage * image, bool flag);
	bool bmx_magick_image_getantialias(MaxMImage * image);
	void bmx_magick_image_animationdelay(MaxMImage * image, unsigned int delay);
	unsigned int bmx_magick_image_getanimationdelay(MaxMImage * image);
	void bmx_magick_image_animationiterations(MaxMImage * image, unsigned int iterations);
	unsigned int bmx_magick_image_getanimationiterations(MaxMImage * image);
	void bmx_magick_image_attribute(MaxMImage * image, BBString * name, BBString * value);
	BBString * bmx_magick_image_getattribute(MaxMImage * image, BBString * name);
	void bmx_magick_image_backgroundtexture(MaxMImage * image, BBString * backgroundTexture);
	BBString * bmx_magick_image_getbackgroundtexture(MaxMImage * image);
	unsigned int bmx_magick_image_getbasecolumns(MaxMImage * image);
	BBString * bmx_magick_image_getbasefilename(MaxMImage * image);
	unsigned int bmx_magick_image_getbaserows(MaxMImage * image);
	void bmx_magick_image_cachethreshold(int threshold);
	void bmx_magick_image_chromablueprimary(MaxMImage * image, double x, double y);
	void bmx_magick_image_getchromablueprimary(MaxMImage * image, double * x, double * y);
	void bmx_magick_image_chromagreenprimary(MaxMImage * image, double x, double y);
	void bmx_magick_image_getchromagreenprimary(MaxMImage * image, double * x, double * y);
	void bmx_magick_image_chromaredprimary(MaxMImage * image, double x, double y);
	void bmx_magick_image_getchromaredprimary(MaxMImage * image, double * x, double * y);
	void bmx_magick_image_chromawhitepoint(MaxMImage * image, double x, double y);
	void bmx_magick_image_getchromawhitepoint(MaxMImage * image, double * x, double * y);
	void bmx_magick_image_classtype(MaxMImage * image, ClassType classType);
	void bmx_magick_image_colorfuzz(MaxMImage * image, double fuzz);
	void bmx_magick_image_contrast(MaxMImage * image, unsigned int sharpen);
	void bmx_magick_image_floodfillcolorcc(MaxMImage * image, const unsigned int x, const unsigned int y, MaxMColor * fillColor, MaxMColor * borderColor);
	void bmx_magick_image_floodfillcolorsc(MaxMImage * image, const unsigned int x, const unsigned int y, BBString * fillColor, MaxMColor * borderColor);
	void bmx_magick_image_floodfillcolorcs(MaxMImage * image, const unsigned int x, const unsigned int y, MaxMColor * fillColor, BBString * borderColor);
	void bmx_magick_image_floodfillcolorss(MaxMImage * image, const unsigned int x, const unsigned int y, BBString * fillColor, BBString * borderColor);
	void bmx_magick_image_frame(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_frametxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_framebevel(MaxMImage * image, const unsigned int width, const unsigned int height, const int innerBevel, const int outerBevel);
	void bmx_magick_image_gamma(MaxMImage * image, const double g);
	void bmx_magick_image_gammargb(MaxMImage * image, const double r, const double g, const double b);
	void bmx_magick_image_gaussianblur(MaxMImage * image, const double width, const double sigma);
	void bmx_magick_image_gaussianblurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma);
	void bmx_magick_image_implode(MaxMImage * image, const double factor);
	void bmx_magick_image_label(MaxMImage * image, BBString * text);
	void bmx_magick_image_level(MaxMImage * image, const double blackPoint, const double whitePoint, const double midPoint);
	void bmx_magick_image_levelchannel(MaxMImage * image, const ChannelType channel, const double blackPoint, const double whitePoint, const double midPoint);
	void bmx_magick_image_magnify(MaxMImage * image);
	void bmx_magick_image_modifyImage(MaxMImage * image);
	void bmx_magick_image_ping(MaxMImage * image, BBString * imageSpec);
	void bmx_magick_image_quantize(MaxMImage * image, const bool measureError);
	void bmx_magick_image_reducenoise(MaxMImage * image, const double order);
	void bmx_magick_image_roll(MaxMImage * image, const int columns, const int rows);
	void bmx_magick_image_shade(MaxMImage * image, const double azimuth, const double elevation, const bool colorShading);
	void bmx_magick_image_sharpen(MaxMImage * image, const double radius, const double sigma);
	void bmx_magick_image_sharpenchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma);
	void bmx_magick_image_shear(MaxMImage * image, const double xShearAngle, const double yShearAngle);
	void bmx_magick_image_solarize(MaxMImage * image, const double factor);
	void bmx_magick_image_spread(MaxMImage * image, const unsigned int amount);
	void bmx_magick_image_swirl(MaxMImage * image, const double degrees);
	void bmx_magick_image_threshold(MaxMImage * image, const double value);
	void bmx_magick_image_trim(MaxMImage * image);
	void bmx_magick_image_unsharpmask(MaxMImage * image, const double radius, const double sigma, const double amount, const double threshold);
	void bmx_magick_image_unsharpmaskchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma, const double amount, const double threshold);
	void bmx_magick_image_wave(MaxMImage * image, const double amplitude, double const wavelength);

	
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
	char *t = bbStringToCString( text );
	image->image().annotate(t, location->geometry());
	bbMemFree(t);
}

void bmx_magick_image_annotatetxt(MaxMImage * image, BBString * text, BBString * location) {
	char *t = bbStringToCString( text );
	char *l = bbStringToCString( location );
	image->image().annotate(t, l);
	bbMemFree(t);
	bbMemFree(l);
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

void bmx_magick_image_bordertxt(MaxMImage * image, BBString * geometry) {
	char *p = bbStringToCString( geometry );
	image->image().border(p);
	bbMemFree(p);
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

void bmx_magick_image_choptxt(MaxMImage * image, BBString * geometry) {
	char *p = bbStringToCString( geometry );
	image->image().chop(p);
	bbMemFree(p);
}

void bmx_magick_image_colorize(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, MaxMColor * penColor) {
	image->image().colorize(opacityRed, opacityGreen, opacityBlue, penColor->color());
}

void bmx_magick_image_colorizetxt(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, BBString * penColor) {
	char *p = bbStringToCString( penColor );
	image->image().colorize(opacityRed, opacityGreen, opacityBlue, p);
	bbMemFree(p);
}

void bmx_magick_image_colorizecolor(MaxMImage * image, const unsigned int opacity, MaxMColor * penColor) {
	image->image().colorize(opacity, penColor->color());
}

void bmx_magick_image_colorizecolortxt(MaxMImage * image, const unsigned int opacity, BBString * penColor) {
	char *p = bbStringToCString( penColor );
	image->image().colorize(opacity, p);
	bbMemFree(p);
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

MaxMImage * bmx_magick_image_copy(MaxMImage * image) {
	Image copy = image->image();
	return new MaxMImage(copy);
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

void bmx_magick_image_zoom(MaxMImage * image, MaxMGeometry * geometry) {
	image->image().zoom(geometry->geometry());
}

void bmx_magick_image_zoomtxt(MaxMImage * image, BBString * geometry) {
	char *g = bbStringToCString( geometry );
	image->image().zoom(g);
	bbMemFree(g);
}

void bmx_magick_image_pixelcolor(MaxMImage * image, unsigned int x, unsigned int y, MaxMColor * color) {
	image->image().pixelColor(x, y, color->color());
}

void bmx_magick_image_pixelcolortxt(MaxMImage * image, unsigned int x, unsigned int y, BBString * color) {
	char *c = bbStringToCString( color );
	image->image().pixelColor(x, y, c);
	bbMemFree(c);
}

void bmx_magick_image_strokecolor(MaxMImage * image, MaxMColor * color) {
	image->image().strokeColor(color->color());
}

void bmx_magick_image_strokecolortxt(MaxMImage * image, BBString * color) {
	char *c = bbStringToCString( color );
	image->image().strokeColor(c);
	bbMemFree(c);
}

void bmx_magick_image_colormap(MaxMImage * image, unsigned int index, MaxMColor * color) {
	image->image().colorMap(index, color->color());
}

void bmx_magick_image_colormaptxt(MaxMImage * image, unsigned int index, BBString * color) {
	char *c = bbStringToCString( color );
	image->image().colorMap(index, c);
	bbMemFree(c);
}

void bmx_magick_image_backgroundcolor(MaxMImage * image, MaxMColor * color) {
	image->image().backgroundColor(color->color());
}

void bmx_magick_image_backgroundcolortxt(MaxMImage * image, BBString * color) {
	char *c = bbStringToCString( color );
	image->image().backgroundColor(c);
	bbMemFree(c);
}

void bmx_magick_image_bordercolor(MaxMImage * image, MaxMColor * color) {
	image->image().borderColor(color->color());
}

void bmx_magick_image_bordercolortxt(MaxMImage * image, BBString * color) {
	char *c = bbStringToCString( color );
	image->image().borderColor(c);
	bbMemFree(c);
}

void bmx_magick_image_crop(MaxMImage * image, MaxMGeometry * geometry) {
	image->image().crop(geometry->geometry());
}

void bmx_magick_image_croptxt(MaxMImage * image, BBString * geometry) {
	char *p = bbStringToCString( geometry );
	image->image().crop(p);
	bbMemFree(p);
}

void bmx_magick_image_cyclecolormap(MaxMImage * image, int amount) {
	image->image().cycleColormap(amount);
}

void bmx_magick_image_despeckle(MaxMImage * image) {
	image->image().despeckle();
}

void bmx_magick_image_edge(MaxMImage * image, double radius) {
	image->image().edge(radius);
}

void bmx_magick_image_emboss(MaxMImage * image, double radius, double sigma) {
	image->image().emboss(radius, sigma);
}

void bmx_magick_image_enhance(MaxMImage * image) {
	image->image().enhance();
}

void bmx_magick_image_equalize(MaxMImage * image) {
	image->image().equalize();
}

void bmx_magick_image_erase(MaxMImage * image) {
	image->image().erase();
}

void bmx_magick_image_adjoin(MaxMImage * image, bool flag) {
	image->image().adjoin(flag);
}

bool bmx_magick_image_getadjoin(MaxMImage * image) {
	return image->image().adjoin();
}

void bmx_magick_image_antialias(MaxMImage * image, bool flag) {
	image->image().antiAlias(flag);
}

bool bmx_magick_image_getantialias(MaxMImage * image) {
	return image->image().antiAlias();
}

void bmx_magick_image_animationdelay(MaxMImage * image, unsigned int delay) {
	image->image().animationDelay(delay);
}

unsigned int bmx_magick_image_getanimationdelay(MaxMImage * image) {
	return image->image().animationDelay();
}

void bmx_magick_image_animationiterations(MaxMImage * image, unsigned int iterations) {
	image->image().animationIterations(iterations);
}

unsigned int bmx_magick_image_getanimationiterations(MaxMImage * image) {
	return image->image().animationIterations();
}

void bmx_magick_image_attribute(MaxMImage * image, BBString * name, BBString * value) {
	char *n = bbStringToCString( name );
	char *v = bbStringToCString( value );
	image->image().attribute(n, v);
	bbMemFree(n);
	bbMemFree(v);
}

BBString * bmx_magick_image_getattribute(MaxMImage * image, BBString * name) {
	char *n = bbStringToCString( name );
	BBString * att = bbStringFromCString(image->image().attribute(n).c_str());
	bbMemFree(n);
	return att;
}

void bmx_magick_image_backgroundtexture(MaxMImage * image, BBString * backgroundTexture) {
	char *b = bbStringToCString( backgroundTexture );
	image->image().backgroundTexture(b);
	bbMemFree(b);
}

BBString * bmx_magick_image_getbackgroundtexture(MaxMImage * image) {
	return bbStringFromCString(image->image().backgroundTexture().c_str());
}

unsigned int bmx_magick_image_getbasecolumns(MaxMImage * image) {
	return image->image().baseColumns();
}

BBString * bmx_magick_image_getbasefilename(MaxMImage * image) {
	return bbStringFromCString(image->image().baseFilename().c_str());
}

unsigned int bmx_magick_image_getbaserows(MaxMImage * image) {
	return image->image().baseRows();
}

void bmx_magick_image_cachethreshold(int threshold) {
	Image::cacheThreshold(threshold);
}

void bmx_magick_image_chromablueprimary(MaxMImage * image, double x, double y) {
	image->image().chromaBluePrimary(x, y);
}

void bmx_magick_image_getchromablueprimary(MaxMImage * image, double * x, double * y) {
	image->image().chromaBluePrimary(x, y);
}

void bmx_magick_image_chromagreenprimary(MaxMImage * image, double x, double y) {
	image->image().chromaGreenPrimary(x, y);
}

void bmx_magick_image_getchromagreenprimary(MaxMImage * image, double * x, double * y) {
	image->image().chromaGreenPrimary(x, y);
}

void bmx_magick_image_chromaredprimary(MaxMImage * image, double x, double y) {
	image->image().chromaRedPrimary(x, y);
}

void bmx_magick_image_getchromaredprimary(MaxMImage * image, double * x, double * y) {
	image->image().chromaRedPrimary(x, y);
}

void bmx_magick_image_chromawhitepoint(MaxMImage * image, double x, double y) {
	image->image().chromaWhitePoint(x, y);
}

void bmx_magick_image_getchromawhitepoint(MaxMImage * image, double * x, double * y) {
	image->image().chromaWhitePoint(x, y);
}

void bmx_magick_image_classtype(MaxMImage * image, ClassType classType) {
	image->image().classType(classType);
}

void bmx_magick_image_colorfuzz(MaxMImage * image, double fuzz) {
	image->image().colorFuzz(fuzz);
}

void bmx_magick_image_contrast(MaxMImage * image, const unsigned int sharpen) {
	image->image().contrast(sharpen);
}

void bmx_magick_image_floodfillcolorcc(MaxMImage * image, const unsigned int x, const unsigned int y, MaxMColor * fillColor, MaxMColor * borderColor) {
	if (borderColor) {
		image->image().floodFillColor(x, y, fillColor->color(), borderColor->color());
	} else {
		image->image().floodFillColor(x, y, fillColor->color());
	}
}

void bmx_magick_image_floodfillcolorsc(MaxMImage * image, const unsigned int x, const unsigned int y, BBString * fillColor, MaxMColor * borderColor) {
	char *t = bbStringToCString( fillColor );
	if (borderColor) {
		image->image().floodFillColor(x, y, t, borderColor->color());
	} else {
		image->image().floodFillColor(x, y, t);
	}
	bbMemFree(t);
}

void bmx_magick_image_floodfillcolorcs(MaxMImage * image, const unsigned int x, const unsigned int y, MaxMColor * fillColor, BBString * borderColor) {
	char *t = bbStringToCString( borderColor );
	image->image().floodFillColor(x, y, fillColor->color(), t);
	bbMemFree(t);
}

void bmx_magick_image_floodfillcolorss(MaxMImage * image, const unsigned int x, const unsigned int y, BBString * fillColor, BBString * borderColor) {
	char *f = bbStringToCString( fillColor );
	char *b = bbStringToCString( borderColor );

	image->image().floodFillColor(x, y, f, b);
	
	bbMemFree(f);
	bbMemFree(b);
}

void bmx_magick_image_frame(MaxMImage * image, MaxMGeometry * geometry) {
	image->image().frame(geometry->geometry());
}

void bmx_magick_image_frametxt(MaxMImage * image, BBString * geometry) {
	char *t = bbStringToCString( geometry );
	image->image().frame(t);
	bbMemFree(t);
}

void bmx_magick_image_framebevel(MaxMImage * image, const unsigned int width, const unsigned int height, const int innerBevel, const int outerBevel) {
	image->image().frame(width, height, innerBevel, outerBevel);
}

void bmx_magick_image_gamma(MaxMImage * image, const double g) {
	image->image().gamma(g);
}

void bmx_magick_image_gammargb(MaxMImage * image, const double r, const double g, const double b) {
	image->image().gamma(r, g, b);
}

void bmx_magick_image_gaussianblur(MaxMImage * image, const double width, const double sigma) {
	image->image().gaussianBlur(width, sigma);
}

void bmx_magick_image_gaussianblurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma) {
	image->image().gaussianBlurChannel(channel, radius, sigma);
}

void bmx_magick_image_implode(MaxMImage * image, const double factor) {
	image->image().implode(factor);
}

void bmx_magick_image_label(MaxMImage * image, BBString * text) {
	char *t = bbStringToCString( text );
	image->image().label(t);
	bbMemFree(t);
}

void bmx_magick_image_level(MaxMImage * image, const double blackPoint, const double whitePoint, const double midPoint) {
	image->image().level(blackPoint, whitePoint, midPoint);
}

void bmx_magick_image_levelchannel(MaxMImage * image, const ChannelType channel, const double blackPoint, const double whitePoint, const double midPoint) {
	image->image().levelChannel(channel, blackPoint, whitePoint, midPoint);
}

void bmx_magick_image_magnify(MaxMImage * image) {
	image->image().magnify();
}

void bmx_magick_image_modifyImage(MaxMImage * image) {
	image->image().modifyImage();
}

void bmx_magick_image_ping(MaxMImage * image, BBString * imageSpec) {
	char *t = bbStringToCString( imageSpec );
	image->image().ping(t);
	bbMemFree(t);
}

void bmx_magick_image_quantize(MaxMImage * image, const bool measureError) {
	image->image().quantize(measureError);
}

void bmx_magick_image_reducenoise(MaxMImage * image, const double order) {
	image->image().reduceNoise(order);
}

void bmx_magick_image_roll(MaxMImage * image, const int columns, const int rows) {
	image->image().roll(columns, rows);
}

void bmx_magick_image_shade(MaxMImage * image, const double azimuth, const double elevation, const bool colorShading) {
	image->image().shade(azimuth, elevation, colorShading);
}

void bmx_magick_image_sharpen(MaxMImage * image, const double radius, double const sigma) {
	image->image().sharpen(radius, sigma);
}

void bmx_magick_image_sharpenchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma) {
	image->image().sharpenChannel(channel, radius, sigma);
}

void bmx_magick_image_shear(MaxMImage * image, const double xShearAngle, const double yShearAngle) {
	image->image().shear(xShearAngle, yShearAngle);
}

void bmx_magick_image_solarize(MaxMImage * image, const double factor) {
	image->image().solarize(factor);
}

void bmx_magick_image_spread(MaxMImage * image, const unsigned int amount) {
	image->image().spread(amount);
}

void bmx_magick_image_swirl(MaxMImage * image, const double degrees) {
	image->image().swirl(degrees);
}

void bmx_magick_image_threshold(MaxMImage * image, const double value) {
	image->image().threshold(value);
}

void bmx_magick_image_trim(MaxMImage * image) {
	image->image().trim();
}

void bmx_magick_image_unsharpmask(MaxMImage * image, const double radius, const double sigma, const double amount, const double threshold) {
	image->image().unsharpmask(radius, sigma, amount, threshold);
}

void bmx_magick_image_unsharpmaskchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma, const double amount, const double threshold) {
	image->image().unsharpmaskChannel(channel, radius, sigma, amount, threshold);
}

void bmx_magick_image_wave(MaxMImage * image, const double amplitude, const double wavelength) {
	image->image().wave(amplitude, wavelength);
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

