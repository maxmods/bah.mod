' Copyright (c) 2007-2011 Bruce A Henderson
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

Import BRL.Blitz

?linux
Import "source.bmx"
?macos
Import "source.bmx"
?win32
Import "source.bmx"
'Import "lib/freeimage.a"
?

Import "src/*.h"
'Import "src/FreeImage/*.h"


Import "freeimageglue.cpp"


Extern

	Function bmx_freeimage_new:Byte Ptr(handle:Object)
	Function bmx_freeimage_delete(handle:Byte Ptr)
	
	Function bmx_FreeImage_GetFileTypeFromHandle:Int(handle:Byte Ptr)
	Function bmx_freeimage_loadImage(handle:Byte Ptr)
	Function bmx_freeimage_getImage:Byte Ptr(handle:Byte Ptr)

	Function bmx_freeimage_GetScanline:Byte Ptr(handle:Byte Ptr,scanline:Int)

	
	Function bmx_freeimage_GetWidth:Int(handle:Byte Ptr)
	Function bmx_freeimage_GetHeight:Int(handle:Byte Ptr)
	Function bmx_freeimage_GetBPP:Int(handle:Byte Ptr)
	Function bmx_freeimage_GetColorsUsed:Int(handle:Byte Ptr)
	Function bmx_freeimage_GetPitch:Int(handle:Byte Ptr)
	
	Function bmx_freeimage_Rescale:Byte Ptr(handle:Byte Ptr, width:Int, height:Int, filter:Int)
	Function bmx_freeimage_setBitmap(handle:Byte Ptr, bitmap:Byte Ptr)
	Function bmx_freeimage_getBitmap:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_MakeThumbnail:Byte Ptr(handle:Byte Ptr, size:Int)

	Function bmx_freeimage_self_convertTo32Bits(handle:Byte Ptr)
	Function bmx_freeimage_self_convertTo24Bits(handle:Byte Ptr)
	Function bmx_freeimage_self_convertTo8Bits(handle:Byte Ptr)
	Function bmx_freeimage_isTransparent:Int(handle:Byte Ptr)
	Function bmx_freeimage_convertToRGBF:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_convertTo32Bits:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_convertTo24Bits:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_convertTo8Bits:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_convertToGreyscale:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_convertTo4Bits:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_ConvertTo16Bits555:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_ConvertTo16Bits565:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_ColorQuantize:Byte Ptr(handle:Byte Ptr, quantize:Int)
	Function bmx_freeimage_ConvertToType:Byte Ptr(handle:Byte Ptr, dstType:Int, scaleLinear:Int)
	Function bmx_freeimage_convertToRGB16:Byte Ptr(handle:Byte Ptr)
	
	Function bmx_freeimage_Rotate:Byte Ptr(handle:Byte Ptr, angle:Double, color:Byte Ptr)
	Function bmx_freeimage_RotateEx:Byte Ptr(handle:Byte Ptr, angle:Double, xShift:Double, yShift:Double, xOrigin:Double, yOrigin:Double, useMask:Int)
	
	Function bmx_freeimage_FlipHorizontal(handle:Byte Ptr)
	Function bmx_freeimage_FlipVertical(handle:Byte Ptr)
	
	Function bmx_freeimage_AdjustGamma(handle:Byte Ptr, Gamma:Double)
	Function bmx_freeimage_AdjustBrightness(handle:Byte Ptr, percentage:Double)
	Function bmx_freeimage_AdjustContrast(handle:Byte Ptr, percentage:Double)
	Function bmx_freeimage_Invert(handle:Byte Ptr)
	Function bmx_freeimage_GetChannel:Byte Ptr(handle:Byte Ptr, channel:Int)
	
	Function bmx_freeimage_Copy:Byte Ptr(handle:Byte Ptr, x0:Int, y0:Int, x1:Int, y1:Int)
	Function bmx_freeimage_Paste:Int(handle:Byte Ptr, source:Byte Ptr, x:Int, y:Int, alpha:Int)
	
	Function bmx_freeimage_GetColorType:Int(handle:Byte Ptr)
	
	Function bmx_freeimage_Clone:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_Save:Int(handle:Byte Ptr, format:Int, flags:Int)
	
	Function bmx_freeimage_Dither:Byte Ptr(handle:Byte Ptr, algo:Int)
	Function bmx_freeimage_Threshold:Byte Ptr(handle:Byte Ptr, t:Byte)
	
	Function bmx_freeimage_ConvertFromRawBits:Byte Ptr(pixels:Byte Ptr, width:Int, height:Int, pitch:Int, ..
		bpp:Int, redMask:Int, greenMask:Int, blueMask:Int)
	
	Function bmx_freeimage_ToneMapping:Byte Ptr(handle:Byte Ptr, algorithm:Int, param1:Double, param2:Double)
	Function bmx_freeimage_TmoDrago03:Byte Ptr(handle:Byte Ptr, Gamma:Double, exposure:Double)
	Function bmx_freeimage_TmoReinhard05:Byte Ptr(handle:Byte Ptr, intensity:Double, contrast:Double)
	
	Function bmx_freeimage_GetMetadata:Byte Ptr(handle:Byte Ptr, model:Int, tag:String)
	Function bmx_freeimage_GetMetadataCount:Int(handle:Byte Ptr, model:Int)
	Function bmx_freeimage_SetMetadata:Int(handle:Byte Ptr, model:Int, key:String, tag:Byte Ptr)

	Function bmx_multifreeimage_new:Byte Ptr(handle:Object, filename:String, readOnly:Int, createNew:Int)
	Function bmx_MultiFreeImage_GetFileType:Int(handle:Byte Ptr)
	Function bmx_multifreeimage_loadImage(handle:Byte Ptr)
	Function bmx_multifreeimage_newImage(handle:Byte Ptr, format:Int)
	Function bmx_multifreeimage_pageCount:Int(handle:Byte Ptr)
	Function bmx_multifreeimage_append(handle:Byte Ptr, image:Byte Ptr)
	Function bmx_multifreeimage_insert(handle:Byte Ptr, image:Byte Ptr, page:Int)
	Function bmx_multifreeimage_deletePage(handle:Byte Ptr, page:Int)
	Function bmx_multifreeimage_lockPage:Byte Ptr(handle:Byte Ptr, page:Int)
	Function bmx_multifreeimage_unlockPage(handle:Byte Ptr, image:Byte Ptr, changed:Int)
	Function bmx_multifreeimage_movePage:Int(handle:Byte Ptr, source:Int, target:Int)
	Function bmx_multifreeimage_close:Int(handle:Byte Ptr)
	Function bmx_multifreeimage_delete(handle:Byte Ptr)

	Function bmx_freeimagetag_free(handle:Byte Ptr)
	Function bmx_freeimagetag_getkey:String(handle:Byte Ptr)
	Function bmx_freeimagetag_getdescription:String(handle:Byte Ptr)
	Function bmx_freeimagetag_getid:Int(handle:Byte Ptr)
	Function bmx_freeimagetag_gettype:Int(handle:Byte Ptr)
	Function bmx_freeimagetag_getcount:Int(handle:Byte Ptr)
	Function bmx_freeimagetag_getlength:Int(handle:Byte Ptr)
	Function bmx_freeimagetag_getvalue:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimagetag_setkey:Int(handle:Byte Ptr, key:String)
	Function bmx_freeimagetag_setdescription:Int(handle:Byte Ptr, description:String)
	Function bmx_freeimagetag_setid:Int(handle:Byte Ptr, id:Int)
	Function bmx_freeimagetag_settype:Int(handle:Byte Ptr, tagType:Int)
	Function bmx_freeimagetag_setcount:Int(handle:Byte Ptr, count:Int)
	Function bmx_freeimagetag_setlength:Int(handle:Byte Ptr, length:Int)
	Function bmx_freeimagetag_setvalue:Int(handle:Byte Ptr, value:Byte Ptr)
	Function bmx_freeimagetag_create:Byte Ptr()
	Function bmx_freeimagetag_clone:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimagetag_tagtostring:String(handle:Byte Ptr, model:Int, make:String)

	Function FreeImage_GetFIFCount:Int()
	Function FreeImage_GetFIFExtensionList:Byte Ptr(fif:Int)
	Function FreeImage_GetFIFDescription:Byte Ptr(fif:Int)
	Function FreeImage_FIFSupportsReading:Int(fif:Int)
	Function FreeImage_FIFSupportsWriting:Int(fif:Int)
	Function FreeImage_GetFIFFromFilename:Int(filename:Byte Ptr)
	Function FreeImage_GetFIFFromFilenameU:Int(filename:Short Ptr)
	Function bmx_freeimage_AdjustCurve:Int(handle:Byte Ptr, lut:Byte Ptr, channel:Int)
	Function bmx_freeimage_AdjustColors:Int(handle:Byte Ptr, brightness:Double, contrast:Double, Gamma:Double, invert:Int)
	Function bmx_freeimage_JPEGCrop:Int(source:String, dest:String, _left:Int, _top:Int, _right:Int, _bottom:Int)
	Function bmx_freeimage_PreMultiplyWithAlpha:Int(handle:Byte Ptr)
	Function bmx_freeimage_enlargeCanvas:Byte Ptr(handle:Byte Ptr, _left:Int, _top:Int, _right:Int, _bottom:Int, color:Byte Ptr, options:Int)
	Function bmx_freeimage_HasBackgroundColor:Int(handle:Byte Ptr)
	Function bmx_freeimage_getBackgroundColor:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimage_setBackgroundColor:Int(handle:Byte Ptr, color:Byte Ptr)
	Function bmx_freeimage_getPixelColor:Byte Ptr(handle:Byte Ptr, x:Int, y:Int)
	Function bmx_freeimage_setPixelColor(handle:Byte Ptr, x:Int, y:Int, color:Byte Ptr)

	Function bmx_freeimagemetadata_FindFirstMetadata:Byte Ptr(model:Int, image:Byte Ptr, tag:Byte Ptr)
	Function bmx_freeimagemetadata_FindNextMetadata:Byte Ptr(handle:Byte Ptr)
	Function bmx_freeimagemetadata_free(handle:Byte Ptr)

	Function bmx_rgbquad_free(handle:Byte Ptr)
	Function bmx_rgbquad_create:Byte Ptr(r:Int, g:Int, b:Int, a:Int)
	Function bmx_rgbquad_red:Int(handle:Byte Ptr)
	Function bmx_rgbquad_green:Int(handle:Byte Ptr)
	Function bmx_rgbquad_blue:Int(handle:Byte Ptr)
	Function bmx_rgbquad_reserved:Int(handle:Byte Ptr)
	Function bmx_rgbquad_setrgba(handle:Byte Ptr, r:Int, g:Int, b:Int, a:Int)
	Function bmx_rgbquad_setred(handle:Byte Ptr, r:Int)
	Function bmx_rgbquad_setgreen(handle:Byte Ptr, g:Int)
	Function bmx_rgbquad_setblue(handle:Byte Ptr, b:Int)
	Function bmx_rgbquad_setalpha(handle:Byte Ptr, a:Int)

End Extern

'?win32
'Extern "win32"
'?linux
'Extern
'?macos
Extern
'?	
	Function FreeImage_Initialise(def:Int = 0)
	Function FreeImage_DeInitialise()
	
End Extern


Const FIF_UNKNOWN:Int = -1
Const FIF_BMP:Int = 0
Const FIF_ICO:Int = 1
Const FIF_JPEG:Int = 2
Const FIF_JNG:Int = 3
Const FIF_KOALA:Int = 4
Const FIF_LBM:Int = 5
Const FIF_IFF:Int = FIF_LBM
Const FIF_MNG:Int = 6
Const FIF_PBM:Int = 7
Const FIF_PBMRAW:Int = 8
Const FIF_PCD:Int = 9
Const FIF_PCX:Int = 10
Const FIF_PGM:Int = 11
Const FIF_PGMRAW:Int = 12
Const FIF_PNG:Int = 13
Const FIF_PPM:Int = 14
Const FIF_PPMRAW:Int = 15
Const FIF_RAS:Int = 16
Const FIF_TARGA:Int = 17
Const FIF_TIFF:Int = 18
Const FIF_WBMP:Int = 19
Const FIF_PSD:Int = 20
Const FIF_CUT:Int = 21
Const FIF_XBM:Int = 22
Const FIF_XPM:Int = 23
Const FIF_DDS:Int = 24
Const FIF_GIF:Int = 25
Const FIF_HDR:Int = 26
Const FIF_FAXG3:Int = 27
Const FIF_SGI:Int = 28
Const FIF_EXR:Int = 29
Const FIF_J2K:Int = 30
Const FIF_JP2:Int = 31
Const FIF_PFM:Int = 32
Const FIF_PICT:Int = 33
Const FIF_RAW:Int = 34


Const FILTER_BOX:Int = 0	' Box, pulse, Fourier window, 1st order (constant) b-spline
Const FILTER_BICUBIC:Int = 1	' Mitchell & Netravali's two-param cubic filter
Const FILTER_BILINEAR:Int = 2	' Bilinear filter
Const FILTER_BSPLINE:Int = 3	' 4th order (cubic) b-spline
Const FILTER_CATMULLROM:Int = 4	' Catmull-Rom spline, Overhauser spline
Const FILTER_LANCZOS3:Int = 5	' Lanczos3 filter

Const FICC_RGB:Int = 0	' Use red, green And blue channels
Const FICC_RED:Int = 1	' Use red channel
Const FICC_GREEN:Int = 2	' Use green channel
Const FICC_BLUE:Int = 3	' Use blue channel
Const FICC_ALPHA:Int = 4	' Use alpha channel
Const FICC_BLACK:Int = 5	' Use black channel
Const FICC_REAL:Int = 6	' Complex images: use real part
Const FICC_IMAG:Int = 7	' Complex images: use imaginary part
Const FICC_MAG:Int = 8	' Complex images: use magnitude
Const FICC_PHASE:Int = 9		' Complex images: use phase

Const FIC_MINISWHITE:Int = 0		' Min value is white
Const FIC_MINISBLACK:Int = 1		' Min value is black
Const FIC_RGB:Int = 2		' RGB color model
Const FIC_PALETTE:Int = 3		' color map indexed
Const FIC_RGBALPHA:Int = 4		' RGB color model with alpha channel
Const FIC_CMYK:Int = 5		' CMYK color model

Const BMP_DEFAULT:Int = 0
Const BMP_SAVE_RLE:Int = 1

Const EXR_DEFAULT:Int = 0      ' save data as half with piz-based wavelet compression
Const EXR_FLOAT:Int = $0001    ' save data as Float instead of as half (Not recommended)
Const EXR_NONE:Int = $0002     ' save with no compression
Const EXR_ZIP:Int = $0004      ' save with zlib compression, in blocks of 16 scan lines
Const EXR_PIZ:Int = $0008      ' save with piz-based wavelet compression
Const EXR_PXR24:Int = $0010    ' save with lossy 24-bit Float compression
Const EXR_B44:Int = $0020      ' save with lossy 44% Float compression - goes To 22% when combined with EXR_LC
Const EXR_LC:Int = $0040       ' save images with one luminance And two chroma channels, rather than as RGB (lossy compression)

Const JPEG_DEFAULT:Int = 0
Const JPEG_FAST:Int = $0001
Const JPEG_ACCURATE:Int = $0002
Const JPEG_CMYK:Int = $0004
Const JPEG_QUALITYSUPERB:Int = $80
Const JPEG_QUALITYGOOD:Int = $0100
Const JPEG_QUALITYNORMAL:Int = $0200
Const JPEG_QUALITYAVERAGE:Int = $0400
Const JPEG_QUALITYBAD:Int = $0800
Const JPEG_PROGRESSIVE:Int = $2000
Const JPEG_SUBSAMPLING_411:Int = $1000		' save with high 4x1 chroma subsampling (4:1:1) 
Const JPEG_SUBSAMPLING_420:Int = $4000		' save with medium 2x2 medium chroma subsampling (4:2:0) - Default value
Const JPEG_SUBSAMPLING_422:Int = $8000		' save with low 2x1 chroma subsampling (4:2:2) 
Const JPEG_SUBSAMPLING_444:Int = $10000	' save with no chroma subsampling (4:4:4)
Const JPEG_OPTIMIZE:Int = $20000		' on saving, compute optimal Huffman coding tables (can reduce a few percent of file size)
Const JPEG_BASELINE:Int = $40000		' save basic JPEG, without metadata Or any markers

Const KOALA_DEFAULT:Int = 0
Const LBM_DEFAULT:Int = 0
Const MNG_DEFAULT:Int = 0
Const PCD_DEFAULT:Int = 0
Const PCD_BASE:Int = 1		' Load the bitmap sized 768 x 512
Const PCD_BASEDIV4:Int = 2		' Load the bitmap sized 384 x 256
Const PCD_BASEDIV16:Int = 3		' Load the bitmap sized 192 x 128
Const PCX_DEFAULT:Int = 0
Const PFM_DEFAULT:Int = 0
Const PICT_DEFAULT:Int = 0

Const PNG_DEFAULT:Int = 0
Const PNG_IGNOREGAMMA:Int = 1		' loading: avoid Gamma correction
Const PNG_Z_BEST_SPEED:Int = $0001	' save using ZLib level 1 compression flag (Default value is 6)
Const PNG_Z_DEFAULT_COMPRESSION:Int = $0006	' save using ZLib level 6 compression flag (Default recommended value)
Const PNG_Z_BEST_COMPRESSION:Int = $0009	' save using ZLib level 9 compression flag (Default value is 6)
Const PNG_Z_NO_COMPRESSION:Int = $0100	' save without ZLib compression
Const PNG_INTERLACED:Int = $0200	' save using Adam7 interlacing (use | To combine with other save flags)

Const PNM_DEFAULT:Int = 0
Const PNM_SAVE_RAW:Int = 0
Const PNM_SAVE_ASCII:Int = 1

Const PSD_DEFAULT:Int = 0
Const PSD_CMYK:Int = 1		' reads tags For separated CMYK (Default is conversion To RGB)
Const PSD_LAB:Int = 2		' reads tags For CIELab (Default is conversion To RGB)

Const RAS_DEFAULT:Int = 0
Const RAW_DEFAULT:Int = 0		' Load the file as linear RGB 48-bit
Const RAW_PREVIEW:Int = 1		' Try To Load the embedded JPEG preview with included Exif Data Or Default To RGB 24-bit
Const RAW_DISPLAY:Int = 2		' Load the file as RGB 24-bit
Const SGI_DEFAULT:Int = 0

Const TARGA_DEFAULT:Int = 0
Const TARGA_LOAD_RGB888:Int = 1       ' If set the loader converts RGB555 And ARGB8888 -> RGB888.
Const TARGA_SAVE_RLE:Int = 2		' If set, the writer saves with RLE compression

Const WBMP_DEFAULT:Int = 0
Const XBM_DEFAULT:Int = 0
Const XPM_DEFAULT:Int = 0

Const TIFF_DEFAULT:Int = 0
Const TIFF_CMYK:Int = $0001
Const TIFF_PACKBITS:Int = $0100
Const TIFF_DEFLATE:Int = $0200
Const TIFF_ADOBE_DEFLATE:Int = $0400
Const TIFF_NONE:Int = $0800
Const TIFF_CCITTFAX3:Int = $1000
Const TIFF_CCITTFAX4:Int = $2000
Const TIFF_LZW:Int = $4000
Const TIFF_JPEG:Int = $8000
Const TIFF_LOGLUV:Int = $10000	' save using LogLuv compression

Const FID_FS:Int = 0
Const FID_BAYER4x4	:Int = 1
Const FID_BAYER8x8	:Int = 2
Const FID_CLUSTER6x6:Int = 3
Const FID_CLUSTER8x8:Int = 4
Const FID_CLUSTER16x16:Int = 5
Const FID_BAYER16x16:Int = 6

Const FIQ_WUQUANT:Int = 0
Const FIQ_NNQUANT:Int = 1

Const FITMO_DRAGO03:Int = 0
Const FITMO_REINHARD05:Int = 1
Const FITMO_FATTAL02:Int = 2 ' Gradient domain high dynamic Range compression (R. Fattal, 2002)

	
Const FIMD_NODATA:Int = -1
Const FIMD_COMMENTS:Int = 0       ' single comment Or keywords
Const FIMD_EXIF_MAIN:Int = 1      ' Exif-TIFF metadata
Const FIMD_EXIF_EXIF:Int = 2      ' Exif-specific metadata
Const FIMD_EXIF_GPS:Int = 3       ' Exif GPS metadata
Const FIMD_EXIF_MAKERNOTE:Int = 4 ' Exif maker note metadata
Const FIMD_EXIF_INTEROP:Int = 5   ' Exif interoperability metadata
Const FIMD_IPTC:Int = 6           ' IPTC/NAA metadata
Const FIMD_XMP:Int = 7            ' Abobe XMP metadata
Const FIMD_GEOTIFF:Int = 8        ' GeoTIFF metadata
Const FIMD_ANIMATION:Int = 9      ' Animation metadata
Const FIMD_CUSTOM:Int = 10        ' Used To attach other metadata types To a dib
Const FIMD_EXIF_RAW:Int = 11      ' Exif metadata as a raw buffer

Const FIDT_NOTYPE:Int = 0     ' placeholder 
Const FIDT_BYTE:Int = 1       ' 8-bit unsigned integer 
Const FIDT_ASCII:Int = 2      ' 8-bit bytes w/ last Byte Null 
Const FIDT_SHORT:Int = 3      ' 16-bit unsigned short
Const FIDT_INT:Int = 4       ' 32-bit unsigned integer 
Const FIDT_RATIONAL:Int = 5   ' 64-bit unsigned fraction 
Const FIDT_SBYTE:Int = 6      ' 8-bit signed byte 
Const FIDT_UNDEFINED:Int = 7  ' 8-bit untyped data 
Const FIDT_SSHORT:Int = 8     ' 16-bit signed short 
Const FIDT_SINT:Int = 9      ' 32-bit signed integer 
Const FIDT_SRATIONAL:Int = 10 ' 64-bit signed fraction 
Const FIDT_FLOAT:Int = 11     ' 32-bit IEEE floating point 
Const FIDT_DOUBLE:Int = 12    ' 64-bit IEEE floating point 
Const FIDT_IFD:Int = 13       ' 32-bit unsigned integer (offset) 
Const FIDT_PALETTE:Int = 14   ' 32-bit RGBQUAD 

Const FIT_UNKNOWN:Int = 0    ' unknown type
Const FIT_BITMAP:Int = 1    ' standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
Const FIT_UINT16:Int = 2    ' array of unsigned short	: unsigned 16-bit
Const FIT_INT16:Int = 3    ' array of short			: signed 16-bit
Const FIT_UINT32:Int = 4    ' array of unsigned long	: unsigned 32-bit
Const FIT_INT32:Int = 5    ' array of long			: signed 32-bit
Const FIT_FLOAT:Int = 6    ' array of float			: 32-bit IEEE floating point
Const FIT_DOUBLE:Int = 7    ' array of double			: 64-bit IEEE floating point
Const FIT_COMPLEX:Int = 8    ' array of FICOMPLEX		: 2 x 64-bit IEEE floating point
Const FIT_RGB16:Int = 9    ' 48-bit RGB image			: 3 x 16-bit
Const FIT_RGBA16:Int = 10    ' 64-bit RGBA image		: 4 x 16-bit
Const FIT_RGBF:Int = 11    ' 96-bit RGB float image	: 3 x 32-bit IEEE floating point
Const FIT_RGBAF:Int = 12	' 128-bit RGBA float image	: 4 x 32-bit IEEE floating point


Const FIJPEG_OP_NONE:Int = 0    ' no transformation
Const FIJPEG_OP_FLIP_H:Int = 1    ' horizontal flip
Const FIJPEG_OP_FLIP_V:Int = 2    ' vertical flip
Const FIJPEG_OP_TRANSPOSE:Int = 3    ' transpose across UL-to-LR axis
Const FIJPEG_OP_TRANSVERSE:Int = 4    ' transpose across UR-to-LL axis
Const FIJPEG_OP_ROTATE_90:Int = 5    ' 90-degree clockwise rotation
Const FIJPEG_OP_ROTATE_180:Int = 6    ' 180-degree rotation
Const FIJPEG_OP_ROTATE_270:Int = 7    ' 270-degree clockwise (or 90 ccw)

Const FI_COLOR_IS_RGB_COLOR	:Int = $00	' RGBQUAD color is a RGB color (contains no valid alpha channel)
Const FI_COLOR_IS_RGBA_COLOR:Int = $01	' RGBQUAD color is a RGBA color (contains a valid alpha channel)
Const FI_COLOR_FIND_EQUAL_COLOR:Int = $02	' For palettized images: lookup equal RGB color from palette
Const FI_COLOR_ALPHA_IS_INDEX:Int = $04	' The color's rgbReserved member (alpha) contains the palette index to be used
Const FI_COLOR_PALETTE_SEARCH_MASK:Int = (FI_COLOR_FIND_EQUAL_COLOR | FI_COLOR_ALPHA_IS_INDEX)	' No color lookup is performed

