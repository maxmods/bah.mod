/*
 Copyright (c) 2008-2013 Bruce A Henderson
 
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

	BBObject * _bah_magick_TMException__create(BBString * message);
	BBObject * _bah_magick_TMErrorUndefined__create(BBString * message);
	BBObject * _bah_magick_TMErrorBlob__create(BBString * message);
	BBObject * _bah_magick_TMErrorCache__create(BBString * message);
	BBObject * _bah_magick_TMErrorCoder__create(BBString * message);
	BBObject * _bah_magick_TMErrorConfigure__create(BBString * message);
	BBObject * _bah_magick_TMErrorCorruptImage__create(BBString * message);
	BBObject * _bah_magick_TMErrorDelegate__create(BBString * message);
	BBObject * _bah_magick_TMErrorDraw__create(BBString * message);
	BBObject * _bah_magick_TMErrorFileOpen__create(BBString * message);
	BBObject * _bah_magick_TMErrorImage__create(BBString * message);
	BBObject * _bah_magick_TMErrorMissingDelegate__create(BBString * message);
	BBObject * _bah_magick_TMErrorModule__create(BBString * message);
	BBObject * _bah_magick_TMErrorOption__create(BBString * message);
	BBObject * _bah_magick_TMErrorRegistry__create(BBString * message);
	BBObject * _bah_magick_TMErrorResourceLimit__create(BBString * message);
	BBObject * _bah_magick_TMErrorStream__create(BBString * message);
	BBObject * _bah_magick_TMErrorType__create(BBString * message);
	BBObject * _bah_magick_TMErrorXServer__create(BBString * message);

	BBObject * _bah_magick_TMCoderInfo__create(BBString * name, BBString * description, int isReadable, int isWritable, int isMultiFrame);
	void _bah_magick_TMCoderInfo__addToList(BBObject * tlist, BBObject * info);

	MaxMImage * bmx_magick_createfromspec(BBString * imageSpec);
	MaxMImage * bmx_magick_image_createfromblob(Blob * blob);
	MaxMImage * bmx_magick_create();
	void bmx_magick_InitializeMagick(const char *path);

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
	void bmx_magick_image_negate(MaxMImage * image, const int grayscale);
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
	void bmx_magick_image_adjoin(MaxMImage * image, int flag);
	int bmx_magick_image_getadjoin(MaxMImage * image);
	void bmx_magick_image_antialias(MaxMImage * image, int flag);
	int bmx_magick_image_getantialias(MaxMImage * image);
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
	void bmx_magick_image_quantize(MaxMImage * image, const int measureError);
	void bmx_magick_image_reducenoise(MaxMImage * image, const double order);
	void bmx_magick_image_roll(MaxMImage * image, const int columns, const int rows);
	void bmx_magick_image_shade(MaxMImage * image, const double azimuth, const double elevation, const int colorShading);
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
	void bmx_magick_image_draw(MaxMImage * image, DrawableBase * drawable);

	void bmx_magick_image_compresstype(MaxMImage * image, const CompressionType _type);
	CompressionType bmx_magick_image_getcompresstype(MaxMImage * image);
	void bmx_magick_image_definevalue(MaxMImage * image, BBString * magick, BBString * key, BBString * value);
	BBString * bmx_magick_image_getdefinedvalue(MaxMImage * image, BBString * magick, BBString * key);
	void bmx_magick_image_defineset(MaxMImage * image, BBString * magick, BBString * key, int flag);
	int bmx_magick_image_getdefinedset(MaxMImage * image, BBString * magick, BBString * key);
	void bmx_magick_image_density(MaxMImage * image, MaxMGeometry * geometry);
	void bmx_magick_image_densitytxt(MaxMImage * image, BBString * geometry);
	void bmx_magick_image_depth(MaxMImage * image, const unsigned int depth);
	BBString * bmx_magick_image_directory(MaxMImage * image);
	void bmx_magick_image_endian(MaxMImage * image, const EndianType _endian);
	EndianType bmx_magick_image_getendian(MaxMImage * image);
	void bmx_magick_image_filename(MaxMImage * image, BBString * filename);
	BBString * bmx_magick_image_getfilename(MaxMImage * image);
	int bmx_magick_image_filesize(MaxMImage * image);
	void bmx_magick_image_fillcolor(MaxMImage * image, MaxMColor * color);
	void bmx_magick_image_fillcolortxt(MaxMImage * image, BBString * color);
	void bmx_magick_image_fillrule(MaxMImage * image, const FillRule rule);
	FillRule bmx_magick_image_getfillrule(MaxMImage * image);
	void bmx_magick_image_filtertype(MaxMImage * image, const FilterTypes filterType);
	FilterTypes bmx_magick_image_getfiltertype(MaxMImage * image);
	void bmx_magick_image_font(MaxMImage * image, BBString * font);
	BBString * bmx_magick_image_getfont(MaxMImage * image);
	void bmx_magick_image_fontpointsize(MaxMImage * image, double pointSize);
	double bmx_magick_image_getfontpointsize(MaxMImage * image);
	BBString * bmx_magick_image_getformat(MaxMImage * image);
	double bmx_magick_image_getgamma(MaxMImage * image);
	void bmx_magick_image_gifdisposemethod(MaxMImage * image, const unsigned int disposeMethod);
	unsigned int bmx_magick_image_getgifdisposemethod(MaxMImage * image);
	void bmx_magick_image_interlacetype(MaxMImage * image, const InterlaceType interlace);
	InterlaceType bmx_magick_image_getinterlacetype(MaxMImage * image);
	void bmx_magick_image_isvalid(MaxMImage * image, int isValid);
	int bmx_magick_image_getisvalid(MaxMImage * image);
	BBString * bmx_magick_image_getlabel(MaxMImage * image);
	void bmx_magick_image_strokewidth(MaxMImage * image, double width);
	double bmx_magick_image_getstrokewidth(MaxMImage * image);
	void bmx_magick_image_magick(MaxMImage * image, BBString * magick);
	BBString * bmx_magick_image_getmagick(MaxMImage * image);
	void bmx_magick_image_matte(MaxMImage * image, int matteFlag);
	int bmx_magick_image_getmatte(MaxMImage * image);
	void bmx_magick_image_opaquecc(MaxMImage * image, MaxMColor * opaqueColor, MaxMColor * penColor);
	void bmx_magick_image_opaquecs(MaxMImage * image, MaxMColor * opaqueColor, BBString * penColor);
	void bmx_magick_image_opaquesc(MaxMImage * image, BBString * opaqueColor, MaxMColor * penColor);
	void bmx_magick_image_opaquess(MaxMImage * image, BBString * opaqueColor, BBString * penColor);

	void bmx_magick_image_modulusdepth(MaxMImage * image, int depth);
	int bmx_magick_image_getmodulusdepth(MaxMImage * image);
	void bmx_magick_image_monochrome(MaxMImage * image, int flag);
	int bmx_magick_image_getmonochrome(MaxMImage * image);
	double bmx_magick_image_normalizedmaxerror(MaxMImage * image);
	double bmx_magick_image_normalizedmeanerror(MaxMImage * image);
	void bmx_magick_image_orientation(MaxMImage * image, const OrientationType orientation);
	OrientationType bmx_magick_image_getorientation(MaxMImage * image);
	void bmx_magick_image_quality(MaxMImage * image, const unsigned int value);
	unsigned int bmx_magick_image_getquality(MaxMImage * image);
	void bmx_magick_image_quantizecolors(MaxMImage * image, const unsigned int colors);
	unsigned int bmx_magick_image_getquantizecolors(MaxMImage * image);
	void bmx_magick_image_quantizecolorspace(MaxMImage * image, const ColorspaceType colorSpace);
	ColorspaceType bmx_magick_image_getquantizecolorspace(MaxMImage * image);
	void bmx_magick_image_quantizedither(MaxMImage * image, int flag);
	int bmx_magick_image_getquantizedither(MaxMImage * image);
	void bmx_magick_image_quantizetreedepth(MaxMImage * image, const unsigned int treeDepth);
	unsigned int bmx_magick_image_getquantizetreedepth(MaxMImage * image);
	void bmx_magick_image_renderingintent(MaxMImage * image, const RenderingIntent intent);
	RenderingIntent bmx_magick_image_getrenderingintent(MaxMImage * image);
	void bmx_magick_image_resolutionunits(MaxMImage * image, const ResolutionType units);
	ResolutionType bmx_magick_image_getresolutionunits(MaxMImage * image);
	unsigned int bmx_magick_image_getrows(MaxMImage * image);
	void bmx_magick_image_scene(MaxMImage * image, const unsigned int scene);
	unsigned int bmx_magick_image_getscene(MaxMImage * image);
	BBString * bmx_magick_image_getsignature(MaxMImage * image, int force);
	void bmx_magick_image_strokeantialias(MaxMImage * image, int flag);
	int bmx_magick_image_getstrokeantialias(MaxMImage * image);
	void bmx_magick_image_strokedasharray(MaxMImage * image, BBArray * array);
	BBArray * bmx_magick_image_getstrokedasharray(MaxMImage * image);
	void bmx_magick_image_strokedashoffset(MaxMImage * image, double offset);
	double bmx_magick_image_getstrokedashoffset(MaxMImage * image);
	void bmx_magick_image_strokelinecap(MaxMImage * image, const LineCap lineCap);
	LineCap bmx_magick_image_getstrokelinecap(MaxMImage * image);
	void bmx_magick_image_strokelinejoin(MaxMImage * image, const LineJoin lineJoin);
	LineJoin bmx_magick_image_getstrokelinejoin(MaxMImage * image);
	void bmx_magick_image_strokemiterlimit(MaxMImage * image, const unsigned int miterlimit);
	unsigned int bmx_magick_image_getstrokemiterlimit(MaxMImage * image);
	void bmx_magick_image_strokepattern(MaxMImage * image, MaxMImage * pattern);
	MaxMImage * bmx_magick_image_getstrokepattern(MaxMImage * image);
	void bmx_magick_image_subimage(MaxMImage * image, const unsigned int subImage);
	unsigned int bmx_magick_image_getsubimage(MaxMImage * image);
	void bmx_magick_image_subrange(MaxMImage * image, const unsigned int subRange);
	unsigned int bmx_magick_image_getsubrange(MaxMImage * image);
	void bmx_magick_image_textencoding(MaxMImage * image, BBString * encoding);
	BBString * bmx_magick_image_gettextencoding(MaxMImage * image);
	void bmx_magick_image_tilename(MaxMImage * image, BBString * name);
	BBString * bmx_magick_image_gettilename(MaxMImage * image);
	int bmx_magick_image_gettotalcolors(MaxMImage * image);
	void bmx_magick_image_transformorigin(MaxMImage * image, const double x, const double y);
	void bmx_magick_image_transformrotation(MaxMImage * image, const double angle);
	void bmx_magick_image_transformreset(MaxMImage * image);
	void bmx_magick_image_transformscale(MaxMImage * image, const double sx, const double sy);
	void bmx_magick_image_transformskewx(MaxMImage * image, const double skew);
	void bmx_magick_image_transformskewy(MaxMImage * image, const double skew);
	ImageType bmx_magick_image_gettype(MaxMImage * image);
	double bmx_magick_image_getxresolution(MaxMImage * image);
	double bmx_magick_image_getyresolution(MaxMImage * image);
	void bmx_magick_image_transparent(MaxMImage * image, MaxMColor * color);
	void bmx_magick_image_transparenttxt(MaxMImage * image, BBString * color);
	double bmx_magick_image_getcolorfuzz(MaxMImage * image);
	void bmx_magick_image_colorspace(MaxMImage * image, const ColorspaceType colorSpace);
	ColorspaceType bmx_magick_image_getcolorspace(MaxMImage * image);

	Blob * bmx_magick_blob_createfromdata(void * data, int size);

	void bmx_magick_coderinfolist(BBObject * tlist, CoderInfo::MatchType isReadable, CoderInfo::MatchType isWritable, CoderInfo::MatchType isMultiFrame);
	BBObject * bmx_magick_coderinfo_info(BBString * format);

	DrawableText * bmx_magick_drawable_drawabletext_create(double x, double y, BBString * text);
	void bmx_magick_drawable_drawabletext_delete(DrawableText * drawable);

	DrawableTextAntialias * bmx_magick_drawable_drawabletextantialias_create(int flag);
	void bmx_magick_drawable_drawabletextantialias_delete(DrawableTextAntialias * drawable);

	DrawableFont * bmx_magick_drawable_drawablefont_create(BBString * font);
	void bmx_magick_drawable_drawablefont_delete(DrawableFont * drawable);

	MaxMGeometry * bmx_magick_geometry_create(BBString * geometry);
	MaxMGeometry * bmx_magick_geometry_createwithattributes(unsigned int width, unsigned int height,
		unsigned int xOffset, unsigned int yOffset, int xNegative, int yNegative);
	void bmx_magick_geometry_free(MaxMGeometry * geom);
	void bmx_magick_geometry_setwidth(MaxMGeometry * geom, unsigned int width);
	unsigned int bmx_magick_geometry_getwidth(MaxMGeometry * geom);
	void bmx_magick_geometry_setheight(MaxMGeometry * geom, unsigned int height);
	unsigned int bmx_magick_geometry_getheight(MaxMGeometry * geom);
	void bmx_magick_geometry_setxoffset(MaxMGeometry * geom, unsigned int offset);
	unsigned int bmx_magick_geometry_getxoffset(MaxMGeometry * geom);
	void bmx_magick_geometry_setyoffset(MaxMGeometry * geom, unsigned int offset);
	unsigned int bmx_magick_geometry_getyoffset(MaxMGeometry * geom);
	void bmx_magick_geometry_setxnegative(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_isxnegative(MaxMGeometry * geom);
	void bmx_magick_geometry_setynegative(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_isynegative(MaxMGeometry * geom);
	void bmx_magick_geometry_setpercent(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_getpercent(MaxMGeometry * geom);
	void bmx_magick_geometry_setaspect(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_getaspect(MaxMGeometry * geom);
	void bmx_magick_geometry_setgreater(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_getgreater(MaxMGeometry * geom);
	void bmx_magick_geometry_setless(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_getless(MaxMGeometry * geom);
	void bmx_magick_geometry_setisvalid(MaxMGeometry * geom, int value);
	int bmx_magick_geometry_isvalid(MaxMGeometry * geom);
	BBString * bmx_magick_geometry_tostring(MaxMGeometry * geom);

	
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

	MaxMGeometry(char * geom)
		: magickGeometry(geom)
	{
	}

	MaxMGeometry(unsigned int width, unsigned int height, unsigned int xOff, unsigned int yOff, 
			int xNegative, int yNegative)
		: magickGeometry(width, height, xOff, yOff, static_cast<bool>(xNegative), static_cast<bool>(yNegative))
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

void bmx_magick_InitializeMagick(const char *path) {
	InitializeMagick(path);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_magick_throw_exception(Magick::Exception &e) {
	if (dynamic_cast<Magick::ErrorUndefined*>(&e)) {
		bbExThrow(_bah_magick_TMErrorUndefined__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorBlob*>(&e)) {
		bbExThrow(_bah_magick_TMErrorBlob__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorCache*>(&e)) {
		bbExThrow(_bah_magick_TMErrorCache__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorCoder*>(&e)) {
		bbExThrow(_bah_magick_TMErrorCoder__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorConfigure*>(&e)) {
		bbExThrow(_bah_magick_TMErrorConfigure__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorCorruptImage*>(&e)) {
		bbExThrow(_bah_magick_TMErrorCorruptImage__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorDelegate*>(&e)) {
		bbExThrow(_bah_magick_TMErrorDelegate__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorDraw*>(&e)) {
		bbExThrow(_bah_magick_TMErrorDraw__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorFileOpen*>(&e)) {
		bbExThrow(_bah_magick_TMErrorFileOpen__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorImage*>(&e)) {
		bbExThrow(_bah_magick_TMErrorImage__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorMissingDelegate*>(&e)) {
		bbExThrow(_bah_magick_TMErrorMissingDelegate__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorModule*>(&e)) {
		bbExThrow(_bah_magick_TMErrorModule__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorOption*>(&e)) {
		bbExThrow(_bah_magick_TMErrorOption__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorRegistry*>(&e)) {
		bbExThrow(_bah_magick_TMErrorRegistry__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorResourceLimit*>(&e)) {
		bbExThrow(_bah_magick_TMErrorResourceLimit__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorStream*>(&e)) {
		bbExThrow(_bah_magick_TMErrorStream__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorType*>(&e)) {
		bbExThrow(_bah_magick_TMErrorType__create(bbStringFromCString(e.what())));
	}
	if (dynamic_cast<Magick::ErrorXServer*>(&e)) {
		bbExThrow(_bah_magick_TMErrorXServer__create(bbStringFromCString(e.what())));
	}

	// unknown exception...
	bbExThrow(_bah_magick_TMException__create(bbStringFromCString(e.what())));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MaxMImage * bmx_magick_createfromspec(BBString * imageSpec) {
	char *p = 0;
	try {
		p = bbStringToCString( imageSpec );
		
		Image image(p);
		MaxMImage * mImage = new MaxMImage(image);
	
		bbMemFree( p );
		return mImage;
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

MaxMImage * bmx_magick_image_createfromblob(Blob * blob) {
	try {
		Image image(*blob);
		return new MaxMImage(image);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

MaxMImage * bmx_magick_create() {
	return new MaxMImage();
}

void bmx_magick_image_adaptivethreshold(MaxMImage * image, const unsigned int width, const unsigned int height, unsigned offset ) {
	try {
		image->image().adaptiveThreshold(width, height, offset);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}


void bmx_magick_image_addnoise(MaxMImage * image, const NoiseType noiseType) {
	try {
		image->image().addNoise(noiseType);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_addnoisechannel(MaxMImage * image, const ChannelType channel, const NoiseType noiseType) {
	try {
		image->image().addNoiseChannel(channel, noiseType);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_affinetransform(MaxMImage * image, const DrawableAffine &affine ) {
//	image->image9).affineTransform(
}

void bmx_magick_image_annotate(MaxMImage * image, BBString * text, MaxMGeometry * location ) {
	char *t = 0;
	try {
		t = bbStringToCString( text );
		image->image().annotate(t, location->geometry());
		bbMemFree(t);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_annotatetxt(MaxMImage * image, BBString * text, BBString * location) {
	char *t = 0;
	char *l = 0;
	try {
		t = bbStringToCString( text );
		l = bbStringToCString( location );
		image->image().annotate(t, l);
		bbMemFree(t);
		bbMemFree(l);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		if (l) bbMemFree(l);
		bmx_magick_throw_exception(e);
	}
}


void bmx_magick_image_blur(MaxMImage * image, const double radius, const double sigma) {
	try {
		image->image().blur(radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_blurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma ) {
	try {
		image->image().blurChannel(channel, radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}
    
void bmx_magick_image_border(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().border(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_bordertxt(MaxMImage * image, BBString * geometry) {
	char *p = 0;
	try {
		p = bbStringToCString( geometry );
		image->image().border(p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_channel(MaxMImage * image, const ChannelType channel) {
	try {
		image->image().channel(channel);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_channeldepth(MaxMImage * image, const ChannelType channel, const unsigned int depth) {
	try {
		image->image().channelDepth(channel, depth);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getchanneldepth(MaxMImage * image, const ChannelType channel) {
	try {
		return image->image().channelDepth(channel);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_charcoal(MaxMImage * image, const double radius, const double sigma) {
	try {
		image->image().charcoal(radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_chop(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().chop(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_choptxt(MaxMImage * image, BBString * geometry) {
	char *p = 0;
	try {
		p = bbStringToCString( geometry );
		image->image().chop(p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colorize(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, MaxMColor * penColor) {
	try {
		image->image().colorize(opacityRed, opacityGreen, opacityBlue, penColor->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colorizetxt(MaxMImage * image, const unsigned int opacityRed, const unsigned int opacityGreen, const unsigned int opacityBlue, BBString * penColor) {
	char *p = 0;
	try {
		p = bbStringToCString( penColor );
		image->image().colorize(opacityRed, opacityGreen, opacityBlue, p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colorizecolor(MaxMImage * image, const unsigned int opacity, MaxMColor * penColor) {
	try {
		image->image().colorize(opacity, penColor->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colorizecolortxt(MaxMImage * image, const unsigned int opacity, BBString * penColor) {
	char *p = 0;
	try {
		p = bbStringToCString( penColor );
		image->image().colorize(opacity, p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_comment(MaxMImage * image, BBString * comment) {
	char *p = 0;
	try {
		p = bbStringToCString( comment );
		image->image().comment(p);
		bbMemFree( p );
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_compose(MaxMImage * image, const CompositeOperator compose) {
	try {
		image->image().compose(compose);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

CompositeOperator bmx_magick_image_getcomposite(MaxMImage * image) {
	try {
		return image->image().compose();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

MaxMImage * bmx_magick_image_copy(MaxMImage * image) {
	try {
		Image copy = image->image();
		return new MaxMImage(copy);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_medianfilter(MaxMImage * image, const double radius) {
	try {
		image->image().medianFilter(radius);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_minify(MaxMImage * image) {
	try {
		image->image().minify();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_modulate(MaxMImage * image, const double brightness, const double saturation, const double hue ) {
	try {
		image->image().modulate(brightness, saturation, hue);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_motionblur(MaxMImage * image, const double radius, const double sigma, const double angle ) {
	try {
		image->image().motionBlur(radius, sigma, angle);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_negate(MaxMImage * image, int grayscale) {
	try {
		image->image().negate(static_cast<bool>(grayscale));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_normalize(MaxMImage * image) {
	try {
		image->image().normalize();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}
	

void bmx_magick_image_oilpaint(MaxMImage * image, double radius) {
	try {
		image->image().oilPaint(radius);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}


void bmx_magick_image_writefile(MaxMImage * image, BBString * imageSpec) {
	char *p = 0;
	try {
		p = bbStringToCString( imageSpec );
		image->image().write(p);
		bbMemFree( p );
	} catch (Magick::Exception & e) {
		if (p) bbMemFree( p );
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_getsize(MaxMImage * image, unsigned int * width, unsigned int * height) {
	try {
		Geometry g(image->image().size());
		*width = g.width();
		*height = g.height();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getdepth(MaxMImage * image) {
	try {
		return image->image().depth();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_writedata(MaxMImage * image, const int x, const int y, const unsigned int columns,
		const unsigned int rows, BBString * map, const StorageType type, void *pixels) {
	char *p = 0;
	try {
		p = bbStringToCString( map );
		image->image().write(x, y, columns, rows, p, type, pixels);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_opacity(MaxMImage * image, unsigned int value) {
	try {
		image->image().opacity(value);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_flip(MaxMImage * image) {
	try {
		image->image().flip();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_flop(MaxMImage * image) {
	try {
		image->image().flop();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_rotate(MaxMImage * image, double degrees) {
	try {
		image->image().rotate(degrees);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_size(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().size(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_sizetxt(MaxMImage * image, BBString * geometry) {
	char *p = 0;
	try {
		p = bbStringToCString( geometry );
		image->image().size(p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_read(MaxMImage * image, BBString * imageSpec) {
	char *p = 0;
	try {
		p = bbStringToCString( imageSpec );
		image->image().read(p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_readgeom(MaxMImage * image, MaxMGeometry * geometry, BBString * imageSpec) {
	char *p = 0;
	try {
		p = bbStringToCString( imageSpec );
		image->image().read(geometry->geometry(), p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_readgeomtxt(MaxMImage * image, BBString * geometry, BBString * imageSpec) {
	char *g = 0;
	char *i = 0;
	try {
		g = bbStringToCString( geometry );
		i = bbStringToCString( imageSpec );
		image->image().read(g, i);
		bbMemFree(g);
		bbMemFree(i);
	} catch (Magick::Exception & e) {
		if (g) bbMemFree(g);
		if (i) bbMemFree(i);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_zoom(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().zoom(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_zoomtxt(MaxMImage * image, BBString * geometry) {
	char *g = 0;
	try {
		g = bbStringToCString( geometry );
		image->image().zoom(g);
		bbMemFree(g);
	} catch (Magick::Exception & e) {
		if (g) bbMemFree(g);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_pixelcolor(MaxMImage * image, unsigned int x, unsigned int y, MaxMColor * color) {
	try {
		image->image().pixelColor(x, y, color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_pixelcolortxt(MaxMImage * image, unsigned int x, unsigned int y, BBString * color) {
	char *c = 0;
	try {
		c = bbStringToCString( color );
		image->image().pixelColor(x, y, c);
		bbMemFree(c);
	} catch (Magick::Exception & e) {
		if (c) bbMemFree(c);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokecolor(MaxMImage * image, MaxMColor * color) {
	try {
		image->image().strokeColor(color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokecolortxt(MaxMImage * image, BBString * color) {
	char *c = 0;
	try {
		c = bbStringToCString( color );
		image->image().strokeColor(c);
		bbMemFree(c);
	} catch (Magick::Exception & e) {
		if (c) bbMemFree(c);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colormap(MaxMImage * image, unsigned int index, MaxMColor * color) {
	try {
		image->image().colorMap(index, color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colormaptxt(MaxMImage * image, unsigned int index, BBString * color) {
	char *c = 0;
	try {
		c = bbStringToCString( color );
		image->image().colorMap(index, c);
		bbMemFree(c);
	} catch (Magick::Exception & e) {
		if (c) bbMemFree(c);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_backgroundcolor(MaxMImage * image, MaxMColor * color) {
	try {
		image->image().backgroundColor(color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_backgroundcolortxt(MaxMImage * image, BBString * color) {
	char *c = 0;
	try {
		c = bbStringToCString( color );
		image->image().backgroundColor(c);
		bbMemFree(c);
	} catch (Magick::Exception & e) {
		if (c) bbMemFree(c);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_bordercolor(MaxMImage * image, MaxMColor * color) {
	try {
		image->image().borderColor(color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_bordercolortxt(MaxMImage * image, BBString * color) {
	char *c = 0;
	try {
		c = bbStringToCString( color );
		image->image().borderColor(c);
		bbMemFree(c);
	} catch (Magick::Exception & e) {
		if (c) bbMemFree(c);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_crop(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().crop(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_croptxt(MaxMImage * image, BBString * geometry) {
	char *p = 0;
	try {
		p = bbStringToCString( geometry );
		image->image().crop(p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_cyclecolormap(MaxMImage * image, int amount) {
	try {
		image->image().cycleColormap(amount);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_despeckle(MaxMImage * image) {
	try {
		image->image().despeckle();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_edge(MaxMImage * image, double radius) {
	try {
		image->image().edge(radius);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_emboss(MaxMImage * image, double radius, double sigma) {
	try {
		image->image().emboss(radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_enhance(MaxMImage * image) {
	try {
		image->image().enhance();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_equalize(MaxMImage * image) {
	try {
		image->image().equalize();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_erase(MaxMImage * image) {
	try {
		image->image().erase();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_adjoin(MaxMImage * image, int flag) {
	try {
		image->image().adjoin(static_cast<bool>(flag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getadjoin(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().adjoin());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_antialias(MaxMImage * image, int flag) {
	try {
		image->image().antiAlias(static_cast<bool>(flag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getantialias(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().antiAlias());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_animationdelay(MaxMImage * image, unsigned int delay) {
	try {
		image->image().animationDelay(delay);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getanimationdelay(MaxMImage * image) {
	try {
		return image->image().animationDelay();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_animationiterations(MaxMImage * image, unsigned int iterations) {
	try {
		image->image().animationIterations(iterations);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getanimationiterations(MaxMImage * image) {
	try {
		return image->image().animationIterations();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_attribute(MaxMImage * image, BBString * name, BBString * value) {
	char *n = 0;
	char *v = 0;
	try {
		n = bbStringToCString( name );
		v = bbStringToCString( value );
		image->image().attribute(n, v);
		bbMemFree(n);
		bbMemFree(v);
	} catch (Magick::Exception & e) {
		if (n) bbMemFree(n);
		if (v) bbMemFree(v);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getattribute(MaxMImage * image, BBString * name) {
	char *n = 0;
	try {
		n = bbStringToCString( name );
		BBString * att = bbStringFromCString(image->image().attribute(n).c_str());
		bbMemFree(n);
		return att;
	} catch (Magick::Exception & e) {
		if (n) bbMemFree(n);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_backgroundtexture(MaxMImage * image, BBString * backgroundTexture) {
	char *b = 0;
	try {
		b = bbStringToCString( backgroundTexture );
		image->image().backgroundTexture(b);
		bbMemFree(b);
	} catch (Magick::Exception & e) {
		if (b) bbMemFree(b);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getbackgroundtexture(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().backgroundTexture().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getbasecolumns(MaxMImage * image) {
	try {
		return image->image().baseColumns();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getbasefilename(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().baseFilename().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getbaserows(MaxMImage * image) {
	try {
		return image->image().baseRows();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_cachethreshold(int threshold) {
	try {
		Image::cacheThreshold(threshold);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_chromablueprimary(MaxMImage * image, double x, double y) {
	try {
		image->image().chromaBluePrimary(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_getchromablueprimary(MaxMImage * image, double * x, double * y) {
	try {
		image->image().chromaBluePrimary(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_chromagreenprimary(MaxMImage * image, double x, double y) {
	try {
		image->image().chromaGreenPrimary(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_getchromagreenprimary(MaxMImage * image, double * x, double * y) {
	try {
		image->image().chromaGreenPrimary(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_chromaredprimary(MaxMImage * image, double x, double y) {
	try {
		image->image().chromaRedPrimary(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_getchromaredprimary(MaxMImage * image, double * x, double * y) {
	try {
		image->image().chromaRedPrimary(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_chromawhitepoint(MaxMImage * image, double x, double y) {
	try {
		image->image().chromaWhitePoint(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_getchromawhitepoint(MaxMImage * image, double * x, double * y) {
	try {
		image->image().chromaWhitePoint(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_classtype(MaxMImage * image, ClassType classType) {
	try {
		image->image().classType(classType);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colorfuzz(MaxMImage * image, double fuzz) {
	try {
		image->image().colorFuzz(fuzz);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_contrast(MaxMImage * image, const unsigned int sharpen) {
	try {
		image->image().contrast(sharpen);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_floodfillcolorcc(MaxMImage * image, const unsigned int x, const unsigned int y, MaxMColor * fillColor, MaxMColor * borderColor) {
	try {
		if (borderColor) {
			image->image().floodFillColor(x, y, fillColor->color(), borderColor->color());
		} else {
			image->image().floodFillColor(x, y, fillColor->color());
		}
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_floodfillcolorsc(MaxMImage * image, const unsigned int x, const unsigned int y, BBString * fillColor, MaxMColor * borderColor) {
	char *t = 0;
	try {
		t = bbStringToCString( fillColor );
		if (borderColor) {
			image->image().floodFillColor(x, y, t, borderColor->color());
		} else {
			image->image().floodFillColor(x, y, t);
		}
		bbMemFree(t);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_floodfillcolorcs(MaxMImage * image, const unsigned int x, const unsigned int y, MaxMColor * fillColor, BBString * borderColor) {
	char *t = 0;
	try {
		t = bbStringToCString( borderColor );
		image->image().floodFillColor(x, y, fillColor->color(), t);
		bbMemFree(t);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_floodfillcolorss(MaxMImage * image, const unsigned int x, const unsigned int y, BBString * fillColor, BBString * borderColor) {
	char *f = 0;
	char *b = 0;
	try {
		f = bbStringToCString( fillColor );
		b = bbStringToCString( borderColor );
	
		image->image().floodFillColor(x, y, f, b);
		
		bbMemFree(f);
		bbMemFree(b);
	} catch (Magick::Exception & e) {
		if (f) bbMemFree(f);
		if (b) bbMemFree(b);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_frame(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().frame(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_frametxt(MaxMImage * image, BBString * geometry) {
	char *t = 0;
	try {
		t = bbStringToCString( geometry );
		image->image().frame(t);
		bbMemFree(t);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_framebevel(MaxMImage * image, const unsigned int width, const unsigned int height, const int innerBevel, const int outerBevel) {
	try {
		image->image().frame(width, height, innerBevel, outerBevel);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_gamma(MaxMImage * image, const double g) {
	try {
		image->image().gamma(g);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_gammargb(MaxMImage * image, const double r, const double g, const double b) {
	try {
		image->image().gamma(r, g, b);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_gaussianblur(MaxMImage * image, const double width, const double sigma) {
	try {
		image->image().gaussianBlur(width, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_gaussianblurchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma) {
	try {
		image->image().gaussianBlurChannel(channel, radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_implode(MaxMImage * image, const double factor) {
	try {
		image->image().implode(factor);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_label(MaxMImage * image, BBString * text) {
	char *t = 0;
	try {
		t = bbStringToCString( text );
		image->image().label(t);
		bbMemFree(t);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_level(MaxMImage * image, const double blackPoint, const double whitePoint, const double midPoint) {
	try {
		image->image().level(blackPoint, whitePoint, midPoint);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_levelchannel(MaxMImage * image, const ChannelType channel, const double blackPoint, const double whitePoint, const double midPoint) {
	try {
		image->image().levelChannel(channel, blackPoint, whitePoint, midPoint);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_magnify(MaxMImage * image) {
	try {
		image->image().magnify();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_modifyImage(MaxMImage * image) {
	try {
		image->image().modifyImage();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_ping(MaxMImage * image, BBString * imageSpec) {
	char *t = 0;
	try {
		t = bbStringToCString( imageSpec );
		image->image().ping(t);
		bbMemFree(t);
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_quantize(MaxMImage * image, int measureError) {
	try {
		image->image().quantize(static_cast<bool>(measureError));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_reducenoise(MaxMImage * image, const double order) {
	try {
		image->image().reduceNoise(order);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_roll(MaxMImage * image, const int columns, const int rows) {
	try {
		image->image().roll(columns, rows);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_shade(MaxMImage * image, const double azimuth, const double elevation, int colorShading) {
	try {
		image->image().shade(azimuth, elevation, static_cast<bool>(colorShading));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_sharpen(MaxMImage * image, const double radius, double const sigma) {
	try {
		image->image().sharpen(radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_sharpenchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma) {
	try {
		image->image().sharpenChannel(channel, radius, sigma);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_shear(MaxMImage * image, const double xShearAngle, const double yShearAngle) {
	try {
		image->image().shear(xShearAngle, yShearAngle);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_solarize(MaxMImage * image, const double factor) {
	try {
		image->image().solarize(factor);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_spread(MaxMImage * image, const unsigned int amount) {
	try {
		image->image().spread(amount);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_swirl(MaxMImage * image, const double degrees) {
	try {
		image->image().swirl(degrees);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_threshold(MaxMImage * image, const double value) {
	try {
		image->image().threshold(value);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_trim(MaxMImage * image) {
	try {
		image->image().trim();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_unsharpmask(MaxMImage * image, const double radius, const double sigma, const double amount, const double threshold) {
	try {
		image->image().unsharpmask(radius, sigma, amount, threshold);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_unsharpmaskchannel(MaxMImage * image, const ChannelType channel, const double radius, const double sigma, const double amount, const double threshold) {
	try {
		image->image().unsharpmaskChannel(channel, radius, sigma, amount, threshold);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_wave(MaxMImage * image, const double amplitude, const double wavelength) {
	try {
		image->image().wave(amplitude, wavelength);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_draw(MaxMImage * image, DrawableBase * drawable) {
	try {
		image->image().draw(*drawable);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_compresstype(MaxMImage * image, const CompressionType _type) {
	try {
		image->image().compressType(_type);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

CompressionType bmx_magick_image_getcompresstype(MaxMImage * image) {

	try {
		return image->image().compressType();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_definevalue(MaxMImage * image, BBString * magick, BBString * key, BBString * value) {
	char *m = 0;
	char *k = 0;
	char *v = 0;
	try {
		m = bbStringToCString( magick );
		k = bbStringToCString( key );
		v = bbStringToCString( value );
		image->image().defineValue(m, k, v);
		bbMemFree(m);
		bbMemFree(k);
		bbMemFree(v);
	} catch (Magick::Exception & e) {
		if (m) bbMemFree(m);
		if (k) bbMemFree(k);
		if (v) bbMemFree(v);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getdefinedvalue(MaxMImage * image, BBString * magick, BBString * key) {
	char *m = 0;
	char *k = 0;
	try {
		m = bbStringToCString( magick );
		k = bbStringToCString( key );
		BBString * value = bbStringFromCString(image->image().defineValue(m, k).c_str());
		bbMemFree(m);
		bbMemFree(k);
		return value;
	} catch (Magick::Exception & e) {
		if (m) bbMemFree(m);
		if (k) bbMemFree(k);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_defineset(MaxMImage * image, BBString * magick, BBString * key, int flag) {
	char *m = 0;
	char *k = 0;
	try {
		m = bbStringToCString( magick );
		k = bbStringToCString( key );
		image->image().defineSet(m, k, static_cast<bool>(flag));
		bbMemFree(m);
		bbMemFree(k);
	} catch (Magick::Exception & e) {
		if (m) bbMemFree(m);
		if (k) bbMemFree(k);
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getdefinedset(MaxMImage * image, BBString * magick, BBString * key) {
	char *m = 0;
	char *k = 0;
	try {
		m = bbStringToCString( magick );
		k = bbStringToCString( key );
		int flag = static_cast<int>(image->image().defineSet(m, k));
		bbMemFree(m);
		bbMemFree(k);
		return flag;
	} catch (Magick::Exception & e) {
		if (m) bbMemFree(m);
		if (k) bbMemFree(k);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_density(MaxMImage * image, MaxMGeometry * geometry) {
	try {
		image->image().density(geometry->geometry());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_densitytxt(MaxMImage * image, BBString * geometry) {
	char *g = 0;
	try {
		g = bbStringToCString( geometry );
		image->image().density(g);
		bbMemFree(g);
	} catch (Magick::Exception & e) {
		if (g) bbMemFree(g);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_depth(MaxMImage * image, const unsigned int depth) {
	try {
		image->image().depth(depth);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_directory(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().directory().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_endian(MaxMImage * image, const EndianType _endian) {
	try {
		image->image().endian(_endian);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

EndianType bmx_magick_image_getendian(MaxMImage * image) {
	try {
		return image->image().endian();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_filename(MaxMImage * image, BBString * filename) {
	char *f = 0;
	try {
		f = bbStringToCString( filename );
		image->image().fileName(f);
		bbMemFree(f);
	} catch (Magick::Exception & e) {
		if (f) bbMemFree(f);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getfilename(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().fileName().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_filesize(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().fileSize());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_fillcolor(MaxMImage * image, MaxMColor * color) {
	try {
		image->image().fillColor(color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_fillcolortxt(MaxMImage * image, BBString * color) {
	char *c = 0;
	try {
		c = bbStringToCString( color );
		image->image().fillColor(c);
		bbMemFree(c);
	} catch (Magick::Exception & e) {
		if (c) bbMemFree(c);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_fillrule(MaxMImage * image, const FillRule rule) {
	try {
		image->image().fillRule(rule);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

FillRule bmx_magick_image_getfillrule(MaxMImage * image) {
	try {
		return image->image().fillRule();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_filtertype(MaxMImage * image, const FilterTypes filterType) {
	try {
		image->image().filterType(filterType);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

FilterTypes bmx_magick_image_getfiltertype(MaxMImage * image) {
	try {
		return image->image().filterType();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_font(MaxMImage * image, BBString * font) {
	char *f = 0;
	try {
		f = bbStringToCString( font );
		image->image().font(f);
		bbMemFree(f);
	} catch (Magick::Exception & e) {
		if (f) bbMemFree(f);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getfont(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().font().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_fontpointsize(MaxMImage * image, double pointSize) {
	try {
		image->image().fontPointsize(pointSize);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getfontpointsize(MaxMImage * image) {
	try {
		return image->image().fontPointsize();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getformat(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().format().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getgamma(MaxMImage * image) {
	try {
		return image->image().gamma();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_gifdisposemethod(MaxMImage * image, const unsigned int disposeMethod) {
	try {
		image->image().gifDisposeMethod(disposeMethod);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getgifdisposemethod(MaxMImage * image) {
	try {
		return image->image().gifDisposeMethod();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_interlacetype(MaxMImage * image, const InterlaceType interlace) {
	try {
		image->image().interlaceType(interlace);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

InterlaceType bmx_magick_image_getinterlacetype(MaxMImage * image) {
	try {
		return image->image().interlaceType();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_isvalid(MaxMImage * image, int isValid) {
	try {
		image->image().isValid(static_cast<bool>(isValid));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getisvalid(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().isValid());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getlabel(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().label().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokewidth(MaxMImage * image, double width) {
	try {
		image->image().strokeWidth(width);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getstrokewidth(MaxMImage * image) {
	try {
		return image->image().strokeWidth();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_magick(MaxMImage * image, BBString * magick) {
	char * m = 0;
	try {
		m = bbStringToCString( magick );
		image->image().magick(m);
		bbMemFree(m);
	} catch (Magick::Exception & e) {
		if (m) bbMemFree(m);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getmagick(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().magick().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_matte(MaxMImage * image, int matteFlag) {
	try {
		image->image().matte(static_cast<bool>(matteFlag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getmatte(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().matte());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_opaquecc(MaxMImage * image, MaxMColor * opaqueColor, MaxMColor * penColor) {
	try {
		image->image().opaque(opaqueColor->color(), penColor->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_opaquecs(MaxMImage * image, MaxMColor * opaqueColor, BBString * penColor) {
	char * p = 0;
	try {
		p = bbStringToCString(penColor);
		image->image().opaque(opaqueColor->color(), p);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_opaquesc(MaxMImage * image, BBString * opaqueColor, MaxMColor * penColor) {
	char * o = 0;
	try {
		o = bbStringToCString(opaqueColor);
		image->image().opaque(o, penColor->color());
		bbMemFree(o);
	} catch (Magick::Exception & e) {
		if (o) bbMemFree(o);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_opaquess(MaxMImage * image, BBString * opaqueColor, BBString * penColor) {
	char * o = 0;
	char * p = 0;
	try {
		o = bbStringToCString(opaqueColor);
		p = bbStringToCString(penColor);
		image->image().opaque(o, p);
		bbMemFree(o);
		bbMemFree(p);
	} catch (Magick::Exception & e) {
		if (o) bbMemFree(o);
		if (p) bbMemFree(p);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_modulusdepth(MaxMImage * image, int depth) {
	try {
		image->image().modulusDepth(depth);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getmodulusdepth(MaxMImage * image) {
	try {
		return image->image().modulusDepth();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_monochrome(MaxMImage * image, int flag) {
	try {
		image->image().monochrome(static_cast<bool>(flag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getmonochrome(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().monochrome());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_normalizedmaxerror(MaxMImage * image) {
	try {
		return image->image().normalizedMaxError();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_normalizedmeanerror(MaxMImage * image) {
	try {
		return image->image().normalizedMeanError();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_orientation(MaxMImage * image, const OrientationType orientation) {
	try {
		image->image().orientation(orientation);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

OrientationType bmx_magick_image_getorientation(MaxMImage * image) {
	try {
		return image->image().orientation();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_quality(MaxMImage * image, const unsigned int value) {
	try {
		image->image().quality(value);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getquality(MaxMImage * image) {
	try {
		return image->image().quality();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_quantizecolors(MaxMImage * image, const unsigned int colors) {
	try {
		image->image().quantizeColors(colors);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getquantizecolors(MaxMImage * image) {
	try {
		return image->image().quantizeColors();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_quantizecolorspace(MaxMImage * image, const ColorspaceType colorSpace) {
	try {
		image->image().quantizeColorSpace(colorSpace);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

ColorspaceType bmx_magick_image_getquantizecolorspace(MaxMImage * image) {
	try {
		return image->image().quantizeColorSpace();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_quantizedither(MaxMImage * image, int flag) {
	try {
		image->image().quantizeDither(static_cast<bool>(flag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getquantizedither(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().quantizeDither());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_quantizetreedepth(MaxMImage * image, const unsigned int treeDepth) {
	try {
		image->image().quantizeTreeDepth(treeDepth);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getquantizetreedepth(MaxMImage * image) {
	try {
		return image->image().quantizeTreeDepth();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_renderingintent(MaxMImage * image, const RenderingIntent intent) {
	try {
		image->image().renderingIntent(intent);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

RenderingIntent bmx_magick_image_getrenderingintent(MaxMImage * image) {
	try {
		return image->image().renderingIntent();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_resolutionunits(MaxMImage * image, const ResolutionType units) {
	try {
		image->image().resolutionUnits(units);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

ResolutionType bmx_magick_image_getresolutionunits(MaxMImage * image) {
	try {
		return image->image().resolutionUnits();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getrows(MaxMImage * image) {
	try {
		return image->image().rows();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_scene(MaxMImage * image, const unsigned int scene) {
	try {
		image->image().scene(scene);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getscene(MaxMImage * image) {
	try {
		return image->image().scene();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_getsignature(MaxMImage * image, int force) {
	try {
		return bbStringFromCString(image->image().signature(static_cast<bool>(force)).c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokeantialias(MaxMImage * image, int flag) {
	try {
		image->image().strokeAntiAlias(static_cast<bool>(flag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_getstrokeantialias(MaxMImage * image) {
	try {
		return static_cast<int>(image->image().strokeAntiAlias());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokedasharray(MaxMImage * image, BBArray * array) {
	try {
		int n = array->scales[0];
		double *s=(double*)BBARRAYDATA( array,array->dims );
		
		double arr[n + 1];
		for (int i = 0; i < n; i++) {
			arr[i] = s[i];
		}
		arr[n] = 0;
		image->image().strokeDashArray(arr);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBArray * bmx_magick_image_getstrokedasharray(MaxMImage * image) {
	try {
		const double* p = image->image().strokeDashArray();
		const double* arr = p;
		
		int n = 0;
		while (*p != 0) {
			n++;
			p++;
		}
		BBArray *array = bbArrayNew1D( "d",n );
		double *s = (double*)BBARRAYDATA( array, array->dims );
		for (int i = 0; i < n; i++) {
			s[i] = arr[i];
		}
		
		return array;
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokedashoffset(MaxMImage * image, double offset) {
	try {
		image->image().strokeDashOffset(offset);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getstrokedashoffset(MaxMImage * image) {
	try {
		return image->image().strokeDashOffset();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokelinecap(MaxMImage * image, const LineCap lineCap) {
	try {
		image->image().strokeLineCap(lineCap);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

LineCap bmx_magick_image_getstrokelinecap(MaxMImage * image) {
	try {
		return image->image().strokeLineCap();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokelinejoin(MaxMImage * image, const LineJoin lineJoin) {
	try {
		image->image().strokeLineJoin(lineJoin);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

LineJoin bmx_magick_image_getstrokelinejoin(MaxMImage * image) {
	try {
		return image->image().strokeLineJoin();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokemiterlimit(MaxMImage * image, const unsigned int miterlimit) {
	try {
		image->image().strokeMiterLimit(miterlimit);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getstrokemiterlimit(MaxMImage * image) {
	try {
		return image->image().strokeMiterLimit();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_strokepattern(MaxMImage * image, MaxMImage * pattern) {
	try {
		image->image().strokePattern(pattern->image());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

MaxMImage * bmx_magick_image_getstrokepattern(MaxMImage * image) {
	try {
		return new MaxMImage(image->image().strokePattern());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_subimage(MaxMImage * image, const unsigned int subImage) {
	try {
		image->image().subImage(subImage);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getsubimage(MaxMImage * image) {
	try {
		return image->image().subImage();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_subrange(MaxMImage * image, const unsigned int subRange) {
	try {
		image->image().subRange(subRange);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

unsigned int bmx_magick_image_getsubrange(MaxMImage * image) {
	try {
		return image->image().subRange();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_textencoding(MaxMImage * image, BBString * encoding) {
	char *n = 0;
	try {
		n = bbStringToCString( encoding );
		image->image().textEncoding(n);
		bbMemFree(n);
	} catch (Magick::Exception & e) {
		if (n) bbMemFree(n);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_gettextencoding(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().textEncoding().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_tilename(MaxMImage * image, BBString * name) {
	char *n = 0;
	try {
		n = bbStringToCString( name );
		image->image().tileName(n);
		bbMemFree(n);
	} catch (Magick::Exception & e) {
		if (n) bbMemFree(n);
		bmx_magick_throw_exception(e);
	}
}

BBString * bmx_magick_image_gettilename(MaxMImage * image) {
	try {
		return bbStringFromCString(image->image().tileName().c_str());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

int bmx_magick_image_gettotalcolors(MaxMImage * image) {
	try {
		return image->image().totalColors();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transformorigin(MaxMImage * image, const double x, const double y) {
	try {
		image->image().transformOrigin(x, y);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transformrotation(MaxMImage * image, const double angle) {
	try {
		image->image().transformRotation(angle);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transformreset(MaxMImage * image) {
	try {
		image->image().transformReset();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transformscale(MaxMImage * image, const double sx, const double sy) {
	try {
		image->image().transformScale(sx, sy);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transformskewx(MaxMImage * image, const double skew) {
	try {
		image->image().transformSkewX(skew);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transformskewy(MaxMImage * image, const double skew) {
	try {
		image->image().transformSkewY(skew);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

ImageType bmx_magick_image_gettype(MaxMImage * image) {
	try {
		image->image().type();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getxresolution(MaxMImage * image) {
	try {
		image->image().xResolution();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getyresolution(MaxMImage * image) {
	try {
		return image->image().yResolution();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transparent(MaxMImage * image, MaxMColor * color) {
	try {
		image->image().transparent(color->color());
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_transparenttxt(MaxMImage * image, BBString * color) {
	char *n = 0;
	try {
		n = bbStringToCString( color );
		image->image().transparent(n);
		bbMemFree(n);
	} catch (Magick::Exception & e) {
		if (n) bbMemFree(n);
		bmx_magick_throw_exception(e);
	}
}

double bmx_magick_image_getcolorfuzz(MaxMImage * image) {
	try {
		return image->image().colorFuzz();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_image_colorspace(MaxMImage * image, const ColorspaceType colorSpace) {
	try {
		image->image().colorSpace(colorSpace);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

ColorspaceType bmx_magick_image_getcolorspace(MaxMImage * image) {
	try {
		return image->image().colorSpace();
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Blob * bmx_magick_blob_createfromdata(void * data, int size) {
	try {
		return new Blob(data, size);
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_magick_coderinfolist(BBObject * tlist, CoderInfo::MatchType isReadable, CoderInfo::MatchType isWritable, CoderInfo::MatchType isMultiFrame) {
	try {
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
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

BBObject * bmx_magick_coderinfo_info(BBString * format) {
	char *t = 0;
	try {
		t = bbStringToCString( format );
		CoderInfo info(t);
		BBObject * obj = _bah_magick_TMCoderInfo__create(bbStringFromCString(info.name().c_str()),
					bbStringFromCString(info.description().c_str()), info.isReadable(), info.isWritable(),
					info.isMultiFrame());
		bbMemFree(t);
		return obj;
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DrawableText * bmx_magick_drawable_drawabletext_create(double x, double y, BBString * text) {
	char *t = 0;
	try {
		t = bbStringToCString( text );
		DrawableText * drawable = new DrawableText(x, y, t);
		bbMemFree(t);
		return drawable;
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_drawable_drawabletext_delete(DrawableText * drawable) {
	delete drawable;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DrawableTextAntialias * bmx_magick_drawable_drawabletextantialias_create(int flag) {
	try {
		return new DrawableTextAntialias(static_cast<bool>(flag));
	} catch (Magick::Exception & e) {
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_drawable_drawabletextantialias_delete(DrawableTextAntialias * drawable) {
	delete drawable;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DrawableFont * bmx_magick_drawable_drawablefont_create(BBString * font) {
	char *t = 0;
	try {
		t = bbStringToCString( font );
		DrawableFont * drawable = new DrawableFont(t);
		bbMemFree(t);
		return drawable;
	} catch (Magick::Exception & e) {
		if (t) bbMemFree(t);
		bmx_magick_throw_exception(e);
	}
}

void bmx_magick_drawable_drawablefont_delete(DrawableFont * drawable) {
	delete drawable;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void bmx_magick_geometry_setwidth(MaxMGeometry * geom, unsigned int width) {
	geom->geometry().width(width);
}

unsigned int bmx_magick_geometry_getwidth(MaxMGeometry * geom) {
	return geom->geometry().width();
}

void bmx_magick_geometry_setheight(MaxMGeometry * geom, unsigned int height) {
	geom->geometry().height(height);
}

unsigned int bmx_magick_geometry_getheight(MaxMGeometry * geom) {
	return geom->geometry().height();
}

void bmx_magick_geometry_setxoffset(MaxMGeometry * geom, unsigned int offset) {
	geom->geometry().xOff(offset);
}

unsigned int bmx_magick_geometry_getxoffset(MaxMGeometry * geom) {
	return geom->geometry().xOff();
}

void bmx_magick_geometry_setyoffset(MaxMGeometry * geom, unsigned int offset) {
	geom->geometry().yOff(offset);
}

unsigned int bmx_magick_geometry_getyoffset(MaxMGeometry * geom) {
	return geom->geometry().yOff();
}

void bmx_magick_geometry_setxnegative(MaxMGeometry * geom, int value) {
	geom->geometry().xNegative(static_cast<bool>(value));
}

int bmx_magick_geometry_isxnegative(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().xNegative());
}

void bmx_magick_geometry_setynegative(MaxMGeometry * geom, int value) {
	geom->geometry().yNegative(static_cast<bool>(value));
}

int bmx_magick_geometry_isynegative(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().yNegative());
}

void bmx_magick_geometry_setpercent(MaxMGeometry * geom, int value) {
	geom->geometry().percent(static_cast<bool>(value));
}

int bmx_magick_geometry_getpercent(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().percent());
}

void bmx_magick_geometry_setaspect(MaxMGeometry * geom, int value) {
	geom->geometry().aspect(static_cast<bool>(value));
}

int bmx_magick_geometry_getaspect(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().aspect());
}

void bmx_magick_geometry_setgreater(MaxMGeometry * geom, int value) {
	geom->geometry().greater(static_cast<bool>(value));
}

int bmx_magick_geometry_getgreater(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().greater());
}

void bmx_magick_geometry_setless(MaxMGeometry * geom, int value) {
	geom->geometry().less(static_cast<bool>(value));
}

int bmx_magick_geometry_getless(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().less());
}

void bmx_magick_geometry_setisvalid(MaxMGeometry * geom, int value) {
	geom->geometry().isValid(static_cast<bool>(value));
}

int bmx_magick_geometry_isvalid(MaxMGeometry * geom) {
	return static_cast<int>(geom->geometry().isValid());
}

BBString * bmx_magick_geometry_tostring(MaxMGeometry * geom) {
	std::string s(geom->geometry());
	return bbStringFromCString(s.c_str());
}


void bmx_magick_geometry_free(MaxMGeometry * geom) {
	delete geom;
}

MaxMGeometry * bmx_magick_geometry_create(BBString * geometry) {
	char * g = bbStringToCString(geometry);
	MaxMGeometry * m = new MaxMGeometry(g);
	bbMemFree(g);
	return m;
}

MaxMGeometry * bmx_magick_geometry_createwithattributes(unsigned int width, unsigned int height,
		unsigned int xOffset, unsigned int yOffset, int xNegative, int yNegative) {
	return new MaxMGeometry(width, height, xOffset, yOffset, xNegative, yNegative);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

