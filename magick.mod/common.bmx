' Copyright (c) 2008 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import "source.bmx"


Extern

	Function bmx_magick_createfromspec:Byte Ptr(imageSpec:String)
	Function bmx_magick_image_createfromblob:Byte Ptr(blob:Byte Ptr)
	Function bmx_magick_create:Byte Ptr()

	Function bmx_magick_image_adaptivethreshold(handle:Byte Ptr, width:Int, height:Int, offset:Int)
	Function bmx_magick_image_addnoise(handle:Byte Ptr, noiseType:Int)
	Function bmx_magick_image_addnoisechannel(handle:Byte Ptr, channel:Int, noiseType:Int)
	'function bmx_magick_image_affinetransform(handle:byte ptr, const DrawableAffine &affine )
	Function bmx_magick_image_annotate(handle:Byte Ptr, text:String, location:Byte Ptr )
	Function bmx_magick_image_annotatetxt(handle:Byte Ptr, text:String, location:String)
	Function bmx_magick_image_blur(handle:Byte Ptr, radius:Double, sigma:Double)
	Function bmx_magick_image_blurchannel(handle:Byte Ptr, channel:Int, radius:Double, sigma:Double )
	Function bmx_magick_image_border(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_bordertxt(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_channel(handle:Byte Ptr, channel:Int)
	Function bmx_magick_image_channeldepth(handle:Byte Ptr, channel:Int, depth:Int)
	Function bmx_magick_image_getchanneldepth:Int(handle:Byte Ptr, channel:Int)
	Function bmx_magick_image_charcoal(handle:Byte Ptr, radius:Double, sigma:Double)
	Function bmx_magick_image_chop(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_choptxt(handle:Byte Ptr, geometry:String)
	Function bmx_magick_image_colorize(handle:Byte Ptr, opacityRed:Int, opacityGreen:Int, opacityBlue:Int, penColor:Byte Ptr)
	Function bmx_magick_image_colorizetxt(handle:Byte Ptr, opacityRed:Int, opacityGreen:Int, opacityBlue:Int, penColor:String)
	Function bmx_magick_image_colorizecolor(handle:Byte Ptr, opacity:Int, penColor:Byte Ptr)
	Function bmx_magick_image_colorizecolortxt(handle:Byte Ptr, opacity:Int, penColor:String)
	Function bmx_magick_image_comment(handle:Byte Ptr, comment:String)
	Function bmx_magick_image_compose(handle:Byte Ptr, compose:Int)
	Function bmx_magick_image_copy:Byte Ptr(handle:Byte Ptr)
	Function bmx_magick_image_getcomposite:Int(handle:Byte Ptr)
	Function bmx_magick_image_flip(handle:Byte Ptr)
	Function bmx_magick_image_flop(handle:Byte Ptr)
	Function bmx_magick_image_rotate(handle:Byte Ptr, degrees:Double)

	Function bmx_magick_image_oilpaint(handle:Byte Ptr, radius:Double)
	Function bmx_magick_image_writefile(handle:Byte Ptr, imageSpec:String)

	Function bmx_magick_image_getsize(handle:Byte Ptr, width:Int Ptr, height:Int Ptr)
	Function bmx_magick_image_getdepth:Int(handle:Byte Ptr)
	Function bmx_magick_image_writedata(handle:Byte Ptr, x:Int, y:Int, width:Int, height:Int, map:String, _type:Int, pixels:Byte Ptr)
	Function bmx_magick_image_opacity(handle:Byte Ptr, value:Int)
	Function bmx_magick_image_size(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_sizetxt(handle:Byte Ptr, geometry:String)
	Function bmx_magick_image_read(handle:Byte Ptr, imageSpec:String)
	Function bmx_magick_image_readgeom(handle:Byte Ptr, geometry:Byte Ptr, imageSpec:String)
	Function bmx_magick_image_readgeomtxt(handle:Byte Ptr, geometry:String, imageSpec:String)
	Function bmx_magick_image_zoom(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_zoomtxt(handle:Byte Ptr, geometry:String)
	Function bmx_magick_image_pixelcolor(handle:Byte Ptr, x:Int, y:Int, color:Byte Ptr)
	Function bmx_magick_image_pixelcolortxt(handle:Byte Ptr, x:Int, y:Int, color:String)
	Function bmx_magick_image_strokecolor(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_magick_image_strokecolortxt(handle:Byte Ptr, color:String)
	Function bmx_magick_image_colormap(handle:Byte Ptr, index:Int, color:Byte Ptr)
	Function bmx_magick_image_colormaptxt(handle:Byte Ptr, index:Int, color:String)
	Function bmx_magick_image_backgroundcolor(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_magick_image_backgroundcolortxt(handle:Byte Ptr, color:String)
	Function bmx_magick_image_bordercolor(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_magick_image_bordercolortxt(handle:Byte Ptr, color:String)
	Function bmx_magick_image_crop(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_croptxt(handle:Byte Ptr, geometry:String)
	Function bmx_magick_image_cyclecolormap(handle:Byte Ptr, amount:Int)
	Function bmx_magick_image_despeckle(handle:Byte Ptr)
	Function bmx_magick_image_edge(handle:Byte Ptr, radius:Double)
	Function bmx_magick_image_emboss(handle:Byte Ptr, radius:Double, sigma:Double)
	Function bmx_magick_image_enhance(handle:Byte Ptr)
	Function bmx_magick_image_equalize(handle:Byte Ptr)
	Function bmx_magick_image_erase(handle:Byte Ptr)

	Function bmx_magick_image_adjoin(handle:Byte Ptr, flag:Int)
	Function bmx_magick_image_getadjoin:Int(handle:Byte Ptr)
	Function bmx_magick_image_antialias(handle:Byte Ptr, flag:Int)
	Function bmx_magick_image_getantialias:Int(handle:Byte Ptr)
	Function bmx_magick_image_animationdelay(handle:Byte Ptr, _delay:Int)
	Function bmx_magick_image_getanimationdelay:Int(handle:Byte Ptr)
	Function bmx_magick_image_animationiterations(handle:Byte Ptr, iterations:Int)
	Function bmx_magick_image_getanimationiterations:Int(handle:Byte Ptr)
	Function bmx_magick_image_attribute(handle:Byte Ptr, name:String, value:String)
	Function bmx_magick_image_getattribute:String(handle:Byte Ptr, name:String)
	Function bmx_magick_image_backgroundtexture(handle:Byte Ptr, backgroundTexture:String)
	Function bmx_magick_image_getbackgroundtexture:String(handle:Byte Ptr)
	Function bmx_magick_image_getbasecolumns:Int(handle:Byte Ptr)
	Function bmx_magick_image_getbasefilename:String(handle:Byte Ptr)
	Function bmx_magick_image_getbaserows:Int(handle:Byte Ptr)
	Function bmx_magick_image_cachethreshold(threshold:Int)
	Function bmx_magick_image_chromablueprimary(handle:Byte Ptr, x:Double, y:Double)
	Function bmx_magick_image_getchromablueprimary(handle:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_magick_image_chromagreenprimary(handle:Byte Ptr, x:Double, y:Double)
	Function bmx_magick_image_getchromagreenprimary(handle:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_magick_image_chromaredprimary(handle:Byte Ptr, x:Double, y:Double)
	Function bmx_magick_image_getchromaredprimary(handle:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_magick_image_chromawhitepoint(handle:Byte Ptr, x:Double, y:Double)
	Function bmx_magick_image_getchromawhitepoint(handle:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_magick_image_classtype(handle:Byte Ptr, class:Int)
	Function bmx_magick_image_colorfuzz(handle:Byte Ptr, fuzz:Double)

	Function bmx_magick_image_contrast(handle:Byte Ptr, sharpen:Int)
	Function bmx_magick_image_floodfillcolorcc(handle:Byte Ptr, x:Int, y:Int, fillColor:Byte Ptr, borderColor:Byte Ptr)
	Function bmx_magick_image_floodfillcolorsc(handle:Byte Ptr, x:Int, y:Int, fillColor:String, borderColor:Byte Ptr)
	Function bmx_magick_image_floodfillcolorcs(handle:Byte Ptr, x:Int, y:Int, fillColor:Byte Ptr, borderColor:String)
	Function bmx_magick_image_floodfillcolorss(handle:Byte Ptr, x:Int, y:Int, fillColor:String, borderColor:String)
	Function bmx_magick_image_frame(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_frametxt(handle:Byte Ptr, geometry:String)
	Function bmx_magick_image_framebevel(handle:Byte Ptr, width:Int, height:Int, innerBevel:Int, outerBevel:Int)
	Function bmx_magick_image_gamma(handle:Byte Ptr, g:Double)
	Function bmx_magick_image_gammargb(handle:Byte Ptr, r:Double, g:Double, b:Double)
	Function bmx_magick_image_gaussianblur(handle:Byte Ptr, width:Double, sigma:Double)
	Function bmx_magick_image_gaussianblurchannel(handle:Byte Ptr, channel:Int, radius:Double, sigma:Double)
	Function bmx_magick_image_implode(handle:Byte Ptr, factor:Double)
	Function bmx_magick_image_label(handle:Byte Ptr, text:String)
	Function bmx_magick_image_level(handle:Byte Ptr, blackPoint:Double, whitePoint:Double, midPoint:Double)
	Function bmx_magick_image_levelchannel(handle:Byte Ptr, channel:Int, blackPoint:Double, whitePoint:Double, midPoint:Double)
	Function bmx_magick_image_magnify(handle:Byte Ptr)
	Function bmx_magick_image_medianfilter(handle:Byte Ptr, radius:Double)
	Function bmx_magick_image_minify(handle:Byte Ptr)
	Function bmx_magick_image_modifyImage(handle:Byte Ptr)
	Function bmx_magick_image_modulate(handle:Byte Ptr, brightness:Double, saturation:Double, hue:Double)
	Function bmx_magick_image_motionblur(handle:Byte Ptr, radius:Double, sigma:Double, angle:Double)
	Function bmx_magick_image_negate(handle:Byte Ptr, grayscale:Int)
	Function bmx_magick_image_normalize(handle:Byte Ptr)
	Function bmx_magick_image_ping(handle:Byte Ptr, imageSpec:String)
	Function bmx_magick_image_quantize(handle:Byte Ptr, measureError:Int)
	Function bmx_magick_image_reducenoise(handle:Byte Ptr, order:Double)
	Function bmx_magick_image_roll(handle:Byte Ptr, columns:Int, rows:Int)
	Function bmx_magick_image_shade(handle:Byte Ptr, azimuth:Double, elevation:Double, colorShading:Int)
	Function bmx_magick_image_sharpen(handle:Byte Ptr, radius:Double, sigma:Double)
	Function bmx_magick_image_sharpenchannel(handle:Byte Ptr, channel:Int, radius:Double, sigma:Double)
	Function bmx_magick_image_shear(handle:Byte Ptr, xShearAngle:Double, yShearAngle:Double)
	Function bmx_magick_image_solarize(handle:Byte Ptr, factor:Double)
	Function bmx_magick_image_spread(handle:Byte Ptr, amount:Int)
	Function bmx_magick_image_swirl(handle:Byte Ptr, degrees:Double)
	Function bmx_magick_image_threshold(handle:Byte Ptr, value:Double)
	Function bmx_magick_image_trim(handle:Byte Ptr)
	Function bmx_magick_image_unsharpmask(handle:Byte Ptr, radius:Double, sigma:Double, amount:Double, threshold:Double)
	Function bmx_magick_image_unsharpmaskchannel(handle:Byte Ptr, channel:Int, radius:Double, sigma:Double, amount:Double, threshold:Double)
	Function bmx_magick_image_wave(handle:Byte Ptr, amplitude:Double, wavelength:Double)

	Function bmx_magick_blob_createfromdata:Byte Ptr(data:Byte Ptr, size:Int)
	
	Function bmx_magick_coderinfolist(list:Object, isReadable:Int, isWritable:Int, isMultiFrame:Int)

End Extern



Const CHANNEL_UNDEFINEDCHANNEL:Int = 0
Const CHANNEL_REDCHANNEL:Int = 1     ' RGB Red channel */
Const CHANNEL_CYANCHANNEL:Int = 2    ' CMYK Cyan channel */
Const CHANNEL_GREENCHANNEL:Int = 3   ' RGB Green channel */
Const CHANNEL_MAGENTACHANNEL:Int = 4 ' CMYK Magenta channel */
Const CHANNEL_BLUECHANNEL:Int = 5    ' RGB Blue channel */
Const CHANNEL_YELLOWCHANNEL:Int = 6  ' CMYK Yellow channel */
Const CHANNEL_OPACITYCHANNEL:Int = 7 ' Opacity channel */
Const CHANNEL_BLACKCHANNEL:Int = 8   ' CMYK Black (K) channel */
Const CHANNEL_MATTECHANNEL:Int = 9   ' Same as Opacity channel (deprecated) */
Const CHANNEL_ALLCHANNELS:Int = 10   ' Color channels */
Const CHANNEL_GRAYCHANNEL:Int = 11   ' Color channels represent an intensity. */

Const CLASS_UNDEFINEDCLASS:Int = 0
Const CLASS_DIRECTCLASS:Int = 1
Const CLASS_PSEUDOCLASS:Int = 2

Const COMPLIANCE_UNDEFINEDCOMPLIANCE:Int = $0000
Const COMPLIANCE_NOCOMPLIANCE:Int = $0000
Const COMPLIANCE_SVGCOMPLIANCE:Int = $0001
Const COMPLIANCE_X11COMPLIANCE:Int = $0002
Const COMPLIANCE_XPMCOMPLIANCE:Int = $0004
Const COMPLIANCE_ALLCOMPLIANCE:Int = $FFFF

Rem
bbdoc: Unset value.
End Rem
Const COMPOSITE_UNDEFINEDCOMPOSITEOP:Int = 0
Rem
bbdoc: The result is the union of the the two image shapes with the composite image obscuring image in the region of overlap.
End Rem
Const COMPOSITE_OVERCOMPOSITEOP:Int = 1
Rem
bbdoc: The result is a simply composite image cut by the shape of image. None of the image data of image is included in the result.
End Rem
Const COMPOSITE_INCOMPOSITEOP:Int = 2
Rem
bbdoc: The resulting image is composite image with the shape of image cut out.
End Rem
Const COMPOSITE_OUTCOMPOSITEOP:Int = 3
Rem
bbdoc: The result is the same shape as image @image, with composite image obscuring image there the image shapes overlap.
about: Note that this differs from COMPOSITE_OVERCOMPOSITEOP because the portion of composite image outside of image's shape does
not appear in the result.
End Rem
Const COMPOSITE_ATOPCOMPOSITEOP:Int = 4
Rem
bbdoc: The result is the image data from both composite image and image that is outside the overlap region.
about: The overlap region will be blank.
End Rem
Const COMPOSITE_XORCOMPOSITEOP:Int = 5
Rem
bbdoc: The result is just the sum of the  image data.
about: Output values are cropped to 255 (no overflow). This operation is independent of the matte channels.
End Rem
Const COMPOSITE_PLUSCOMPOSITEOP:Int = 6
Rem
bbdoc: The result of composite image - image, with overflow cropped to zero.
about: The matte chanel is ignored (set to 255, full coverage).
End Rem
Const COMPOSITE_MINUSCOMPOSITEOP:Int = 7
Rem
bbdoc: The result of composite image + image, with overflow wrapping around (mod 256).
End Rem
Const COMPOSITE_ADDCOMPOSITEOP:Int = 8
Rem
bbdoc: The result of composite image - image, with underflow wrapping around (mod 256).
about: The add and subtract operators can be used to perform reverible transformations.
End Rem
Const COMPOSITE_SUBTRACTCOMPOSITEOP:Int = 9
Rem
bbdoc: The result of abs(composite image - image).
about: This is useful for comparing two very similar images.
End Rem
Const COMPOSITE_DIFFERENCECOMPOSITEOP:Int = 10
Rem
bbdoc: The result of change-image * base-image.
about: This is useful for the creation of drop-shadows.
End Rem
Const COMPOSITE_MULTIPLYCOMPOSITEOP:Int = 11
Rem
bbdoc: The result image shaded by composite image.
End Rem
Const COMPOSITE_BUMPMAPCOMPOSITEOP:Int = 12
Rem
bbdoc: The resulting image is image replaced with composite image.
about: Here the matte information is ignored.
End Rem
Const COMPOSITE_COPYCOMPOSITEOP:Int = 13
Rem
bbdoc: The resulting image is the red layer in image replaced with the red layer in composite image.
about: The other layers are copied untouched.
End Rem
Const COMPOSITE_COPYREDCOMPOSITEOP:Int = 14
Rem
bbdoc: The resulting image is the green layer in image replaced with the green layer in composite image.
about: The other layers are copied untouched.
End Rem
Const COMPOSITE_COPYGREENCOMPOSITEOP:Int = 15
Rem
bbdoc: The resulting image is the blue layer in image replaced with the blue layer in composite image.
about: The other layers are copied untouched.
End Rem
Const COMPOSITE_COPYBLUECOMPOSITEOP:Int = 16
Rem
bbdoc: The resulting image is the matte layer in image replaced with the matte layer in composite image.
about: The other layers are copied untouched.
<p>
The image compositor requires a matte, or alpha channel in the image for some operations. This extra channel usually
defines a mask which represents a sort of a cookie-cutter for the image. This is the case when matte is 255 (full coverage)
for pixels inside the shape, zero outside, and between zero and 255 on the boundary.  For certain operations, if image does
not have a matte channel, it is initialized with 0 for any pixel matching in color to pixel location (0,0), otherwise 255
(to work properly borderWidth must be 0).
</p>
End Rem
Const COMPOSITE_COPYOPACITYCOMPOSITEOP:Int = 17
Rem
bbdoc: Opacity channels are set to transparent.
End Rem
Const COMPOSITE_CLEARCOMPOSITEOP:Int = 18
Rem
bbdoc: 
End Rem
Const COMPOSITE_DISSOLVECOMPOSITEOP:Int = 19
Rem
bbdoc: 
End Rem
Const COMPOSITE_DISPLACECOMPOSITEOP:Int = 20
Rem
bbdoc: 
End Rem
Const COMPOSITE_MODULATECOMPOSITEOP:Int = 21
Rem
bbdoc: 
End Rem
Const COMPOSITE_THRESHOLDCOMPOSITEOP:Int = 22
Rem
bbdoc: 
End Rem
Const COMPOSITE_NOCOMPOSITEOP:Int = 23
Rem
bbdoc: 
End Rem
Const COMPOSITE_DARKENCOMPOSITEOP:Int = 24
Rem
bbdoc: 
End Rem
Const COMPOSITE_LIGHTENCOMPOSITEOP:Int = 25
Rem
bbdoc: 
End Rem
Const COMPOSITE_HUECOMPOSITEOP:Int = 26
Rem
bbdoc: 
End Rem
Const COMPOSITE_SATURATECOMPOSITEOP:Int = 27
Rem
bbdoc: 
End Rem
Const COMPOSITE_COLORIZECOMPOSITEOP:Int = 28
Rem
bbdoc: 
End Rem
Const COMPOSITE_LUMINIZECOMPOSITEOP:Int = 29
Rem
bbdoc: 
End Rem
Const COMPOSITE_SCREENCOMPOSITEOP:Int = 30 ' NOT YET IMPLEMENTED */
Rem
bbdoc: 
End Rem
Const COMPOSITE_OVERLAYCOMPOSITEOP:Int = 31  ' NOT YET IMPLEMENTED */
Rem
bbdoc: 
End Rem
Const COMPOSITE_COPYCYANCOMPOSITEOP:Int = 32
Rem
bbdoc: 
End Rem
Const COMPOSITE_COPYMAGENTACOMPOSITEOP:Int = 33
Rem
bbdoc: 
End Rem
Const COMPOSITE_COPYYELLOWCOMPOSITEOP:Int = 34
Rem
bbdoc: 
End Rem
Const COMPOSITE_COPYBLACKCOMPOSITEOP:Int = 35
Rem
bbdoc: 
End Rem
Const COMPOSITE_DIVIDECOMPOSITEOP:Int = 36

Const COMPRESSION_UNDEFINEDCOMPRESSION:Int = 0
Const COMPRESSION_NOCOMPRESSION:Int = 1
Const COMPRESSION_BZIPCOMPRESSION:Int = 2
Const COMPRESSION_FAXCOMPRESSION:Int = 3
Const COMPRESSION_GROUP4COMPRESSION:Int = 4
Const COMPRESSION_JPEGCOMPRESSION:Int = 5
Const COMPRESSION_LOSSLESSJPEGCOMPRESSION:Int = 6
Const COMPRESSION_LZWCOMPRESSION:Int = 7
Const COMPRESSION_RLECOMPRESSION:Int = 8
Const COMPRESSION_ZIPCOMPRESSION:Int = 9

Const DISPOSE_UNDEFINEDDISPOSE:Int = 0
Const DISPOSE_NONEDISPOSE:Int = 1
Const DISPOSE_BACKGROUNDDISPOSE:Int = 2
Const DISPOSE_PREVIOUSDISPOSE:Int = 3

Const ENDIAN_UNDEFINEDENDIAN:Int = 0
Const ENDIAN_LSBENDIAN:Int = 1            ' "little" endian */
Const ENDIAN_MSBENDIAN:Int = 2            ' "big" endian */
Const ENDIAN_NATIVEENDIAN:Int = 3         ' native endian */

Const FILTER_UNDEFINEDFILTER:Int = 0
Const FILTER_POINTFILTER:Int = 1
Const FILTER_BOXFILTER:Int = 2
Const FILTER_TRIANGLEFILTER:Int = 3
Const FILTER_HERMITEFILTER:Int = 4
Const FILTER_HANNINGFILTER:Int = 5
Const FILTER_HAMMINGFILTER:Int = 6
Const FILTER_BLACKMANFILTER:Int = 7
Const FILTER_GAUSSIANFILTER:Int = 8
Const FILTER_QUADRATICFILTER:Int = 9
Const FILTER_CUBICFILTER:Int = 10
Const FILTER_CATROMFILTER:Int = 11
Const FILTER_MITCHELLFILTER:Int = 12
Const FILTER_LANCZOSFILTER:Int = 13
Const FILTER_BESSELFILTER:Int = 14
Const FILTER_SINCFILTER:Int = 15

Const GEOMETRY_NOVALUE:Int = $0000
Const GEOMETRY_XVALUE:Int = $0001
Const GEOMETRY_YVALUE:Int = $0002
Const GEOMETRY_WIDTHVALUE:Int = $0004
Const GEOMETRY_HEIGHTVALUE:Int = $0008
Const GEOMETRY_ALLVALUES:Int = $000F
Const GEOMETRY_XNEGATIVE:Int = $0010
Const GEOMETRY_YNEGATIVE:Int = $0020
Const GEOMETRY_PERCENTVALUE:Int = $1000
Const GEOMETRY_ASPECTVALUE:Int = $2000
Const GEOMETRY_LESSVALUE:Int = $4000
Const GEOMETRY_GREATERVALUE:Int = $8000
Const GEOMETRY_AREAVALUE:Int = $10000


Const GRAVITY_FORGETGRAVITY:Int = 0
Const GRAVITY_NORTHWESTGRAVITY:Int = 1
Const GRAVITY_NORTHGRAVITY:Int = 2
Const GRAVITY_NORTHEASTGRAVITY:Int = 3
Const GRAVITY_WESTGRAVITY:Int = 4
Const GRAVITY_CENTERGRAVITY:Int = 5
Const GRAVITY_EASTGRAVITY:Int = 6
Const GRAVITY_SOUTHWESTGRAVITY:Int = 7
Const GRAVITY_SOUTHGRAVITY:Int = 8
Const GRAVITY_SOUTHEASTGRAVITY:Int = 9
Const GRAVITY_STATICGRAVITY:Int = 10

Const IMAGE_UNDEFINEDTYPE:Int = 0
Const IMAGE_BILEVELTYPE:Int = 1
Const IMAGE_GRAYSCALETYPE:Int = 2
Const IMAGE_GRAYSCALEMATTETYPE:Int = 3
Const IMAGE_PALETTETYPE:Int = 4
Const IMAGE_PALETTEMATTETYPE:Int = 5
Const IMAGE_TRUECOLORTYPE:Int = 6
Const IMAGE_TRUECOLORMATTETYPE:Int = 7
Const IMAGE_COLORSEPARATIONTYPE:Int = 8
Const IMAGE_COLORSEPARATIONMATTETYPE:Int = 9
Const IMAGE_OPTIMIZETYPE:Int = 10

Const INTERLACE_UNDEFINEDINTERLACE:Int = 0
Const INTERLACE_NOINTERLACE:Int = 1
Const INTERLACE_LINEINTERLACE:Int = 2
Const INTERLACE_PLANEINTERLACE:Int = 3
Const INTERLACE_PARTITIONINTERLACE:Int = 4

Const MONTAGE_UNDEFINEDMODE:Int = 0
Const MONTAGE_FRAMEMODE:Int = 1
Const MONTAGE_UNFRAMEMODE:Int = 2
Const MONTAGE_CONCATENATEMODE:Int = 3

Const NOISE_UNIFORMNOISE:Int = 0
Const NOISE_GAUSSIANNOISE:Int = 1
Const NOISE_MULTIPLICATIVEGAUSSIANNOISE:Int = 2
Const NOISE_IMPULSENOISE:Int = 3
Const NOISE_LAPLACIANNOISE:Int = 4
Const NOISE_POISSONNOISE:Int = 5

                                                  ' Line direction / Frame Direction */
                                                  ' -------------- / --------------- */
Const ORIENTATION_UNDEFINEDORIENTATION:Int = 0    ' Unknown        / Unknown         */
Const ORIENTATION_TOPLEFTORIENTATION:Int = 1      ' Left to right  / Top to bottom   */
Const ORIENTATION_TOPRIGHTORIENTATION:Int = 2     ' Right to left  / Top to bottom   */
Const ORIENTATION_BOTTOMRIGHTORIENTATION:Int = 3  ' Right to left  / Bottom to top   */
Const ORIENTATION_BOTTOMLEFTORIENTATION:Int = 4   ' Left to right  / Bottom to top   */
Const ORIENTATION_LEFTTOPORIENTATION:Int = 5      ' Top to bottom  / Left to right   */
Const ORIENTATION_RIGHTTOPORIENTATION:Int = 6     ' Top to bottom  / Right to left   */
Const ORIENTATION_RIGHTBOTTOMORIENTATION:Int = 7  ' Bottom to top  / Right to left   */
Const ORIENTATION_LEFTBOTTOMORIENTATION:Int = 8   ' Bottom to top  / Left to right   */

Const PREVIEW_UNDEFINEDPREVIEW:Int = 0
Const PREVIEW_ROTATEPREVIEW:Int = 1
Const PREVIEW_SHEARPREVIEW:Int = 2
Const PREVIEW_ROLLPREVIEW:Int = 3
Const PREVIEW_HUEPREVIEW:Int = 4
Const PREVIEW_SATURATIONPREVIEW:Int = 5
Const PREVIEW_BRIGHTNESSPREVIEW:Int = 6
Const PREVIEW_GAMMAPREVIEW:Int = 7
Const PREVIEW_SPIFFPREVIEW:Int = 8
Const PREVIEW_DULLPREVIEW:Int = 9
Const PREVIEW_GRAYSCALEPREVIEW:Int = 10
Const PREVIEW_QUANTIZEPREVIEW:Int = 11
Const PREVIEW_DESPECKLEPREVIEW:Int = 12
Const PREVIEW_REDUCENOISEPREVIEW:Int = 13
Const PREVIEW_ADDNOISEPREVIEW:Int = 14
Const PREVIEW_SHARPENPREVIEW:Int = 15
Const PREVIEW_BLURPREVIEW:Int = 16
Const PREVIEW_THRESHOLDPREVIEW:Int = 17
Const PREVIEW_EDGEDETECTPREVIEW:Int = 18
Const PREVIEW_SPREADPREVIEW:Int = 19
Const PREVIEW_SOLARIZEPREVIEW:Int = 20
Const PREVIEW_SHADEPREVIEW:Int = 21
Const PREVIEW_RAISEPREVIEW:Int = 22
Const PREVIEW_SEGMENTPREVIEW:Int = 23
Const PREVIEW_SWIRLPREVIEW:Int = 24
Const PREVIEW_IMPLODEPREVIEW:Int = 25
Const PREVIEW_WAVEPREVIEW:Int = 26
Const PREVIEW_OILPAINTPREVIEW:Int = 27
Const PREVIEW_CHARCOALDRAWINGPREVIEW:Int = 28
Const PREVIEW_JPEGPREVIEW:Int = 29

Const RENDERING_UNDEFINEDINTENT:Int = 0
Const RENDERING_SATURATIONINTENT:Int = 1
Const RENDERING_PERCEPTUALINTENT:Int = 2
Const RENDERING_ABSOLUTEINTENT:Int = 3
Const RENDERING_RELATIVEINTENT:Int = 4

Const RESOLUTION_UNDEFINEDRESOLUTION:Int = 0
Const RESOLUTION_PIXELSPERINCHRESOLUTION:Int = 1
Const RESOLUTION_PIXELSPERCENTIMETERRESOLUTION:Int = 2

