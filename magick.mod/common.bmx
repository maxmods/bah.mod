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
	Function bmx_magick_image_blur(handle:Byte Ptr, radius:Double, sigma:Double)
	Function bmx_magick_image_blurchannel(handle:Byte Ptr, channel:Int, radius:Double, sigma:Double )
	Function bmx_magick_image_border(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_channel(handle:Byte Ptr, channel:Int)
	Function bmx_magick_image_channeldepth(handle:Byte Ptr, channel:Int, depth:Int)
	Function bmx_magick_image_getchanneldepth:Int(handle:Byte Ptr, channel:Int)
	Function bmx_magick_image_charcoal(handle:Byte Ptr, radius:Double, sigma:Double)
	Function bmx_magick_image_chop(handle:Byte Ptr, geometry:Byte Ptr)
	Function bmx_magick_image_colorize(handle:Byte Ptr, opacityRed:Int, opacityGreen:Int, opacityBlue:Int, penColor:Byte Ptr)
	Function bmx_magick_image_colorizecolor(handle:Byte Ptr, opacity:Int, penColor:Byte Ptr)
	Function bmx_magick_image_comment(handle:Byte Ptr, comment:String)
	Function bmx_magick_image_compose(handle:Byte Ptr, compose:Int)
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

Const COMPOSITE_UNDEFINEDCOMPOSITEOP:Int = 0
Const COMPOSITE_OVERCOMPOSITEOP:Int = 1
Const COMPOSITE_INCOMPOSITEOP:Int = 2
Const COMPOSITE_OUTCOMPOSITEOP:Int = 3
Const COMPOSITE_ATOPCOMPOSITEOP:Int = 4
Const COMPOSITE_XORCOMPOSITEOP:Int = 5
Const COMPOSITE_PLUSCOMPOSITEOP:Int = 6
Const COMPOSITE_MINUSCOMPOSITEOP:Int = 7
Const COMPOSITE_ADDCOMPOSITEOP:Int = 8
Const COMPOSITE_SUBTRACTCOMPOSITEOP:Int = 9
Const COMPOSITE_DIFFERENCECOMPOSITEOP:Int = 10
Const COMPOSITE_MULTIPLYCOMPOSITEOP:Int = 11
Const COMPOSITE_BUMPMAPCOMPOSITEOP:Int = 12
Const COMPOSITE_COPYCOMPOSITEOP:Int = 13
Const COMPOSITE_COPYREDCOMPOSITEOP:Int = 14
Const COMPOSITE_COPYGREENCOMPOSITEOP:Int = 15
Const COMPOSITE_COPYBLUECOMPOSITEOP:Int = 16
Const COMPOSITE_COPYOPACITYCOMPOSITEOP:Int = 17
Const COMPOSITE_CLEARCOMPOSITEOP:Int = 18
Const COMPOSITE_DISSOLVECOMPOSITEOP:Int = 19
Const COMPOSITE_DISPLACECOMPOSITEOP:Int = 20
Const COMPOSITE_MODULATECOMPOSITEOP:Int = 21
Const COMPOSITE_THRESHOLDCOMPOSITEOP:Int = 22
Const COMPOSITE_NOCOMPOSITEOP:Int = 23
Const COMPOSITE_DARKENCOMPOSITEOP:Int = 24
Const COMPOSITE_LIGHTENCOMPOSITEOP:Int = 25
Const COMPOSITE_HUECOMPOSITEOP:Int = 26
Const COMPOSITE_SATURATECOMPOSITEOP:Int = 27
Const COMPOSITE_COLORIZECOMPOSITEOP:Int = 28
Const COMPOSITE_LUMINIZECOMPOSITEOP:Int = 29
Const COMPOSITE_SCREENCOMPOSITEOP:Int = 30 ' NOT YET IMPLEMENTED */
Const COMPOSITE_OVERLAYCOMPOSITEOP:Int = 31  ' NOT YET IMPLEMENTED */
Const COMPOSITE_COPYCYANCOMPOSITEOP:Int = 32
Const COMPOSITE_COPYMAGENTACOMPOSITEOP:Int = 33
Const COMPOSITE_COPYYELLOWCOMPOSITEOP:Int = 34
Const COMPOSITE_COPYBLACKCOMPOSITEOP:Int = 35
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

