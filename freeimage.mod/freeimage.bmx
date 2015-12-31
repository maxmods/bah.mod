' Copyright (c) 2007-2015 Bruce A Henderson
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

Rem
bbdoc: FreeImage Library
End Rem
Module BaH.FreeImage

ModuleInfo "Version: 1.09"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: FreeImage - FreeImage Public License (FIPL)"
ModuleInfo "Copyright: Wrapper - 2007-2015 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.09"
ModuleInfo "History: Updated to FreeImage 3.17. Adds JXR support."
ModuleInfo "History: Added rescaleRect(), tmoReinhard05Ex(), tmoFattal02(), convertToRGBAF() and convertToRGBA16() methods."
ModuleInfo "History: 1.08"
ModuleInfo "History: Updated to FreeImage 3.16. Adds WebP format."
ModuleInfo "History: Added GetFormatFromName() and FormatSupportsNoPixels() functions."
ModuleInfo "History: Added hasPixels() and getThumbnail() methods."
ModuleInfo "History: LibJPEG, LibPNG and ZLib now built with prefix fi_. Should help with multi-library-version problems."
ModuleInfo "History: 1.07"
ModuleInfo "History: Updated to FreeImage 3.15.4."
ModuleInfo "History: Integrated libraw support into main module. Assumes CDDL licensing."
ModuleInfo "History: Removed MNG support."
ModuleInfo "History: Added flags option for image loading."
ModuleInfo "History: 1.06"
ModuleInfo "History: Updated to FreeImage 3.13.1. Adds PICT format."
ModuleInfo "History: Added new methods."
ModuleInfo "History: Added Metadata tag support."
ModuleInfo "History: Fixed mac x86 rgba ordering."
ModuleInfo "History: Fixed problem where original bitmap was used as the display bitmap, for greyscale images."
ModuleInfo "History: Improved stream read/write."
ModuleInfo "History: Added user error handler callback."
ModuleInfo "History: 1.05"
ModuleInfo "History: Fixed conversion issue with 4-bit paletted images."
ModuleInfo "History: Fixed GC problems when loading large anim images."
ModuleInfo "History: Fixed problem with creation of new multi-page images."
ModuleInfo "History: 1.04"
ModuleInfo "History: Fixed problem with TIFF images not loading if there are more than 1 alpha channel in the image. (fredborg)"
ModuleInfo "History: Added GetBitmap(), GetScanLine() and ConvertToRGBF() methods. (fredborg)"
ModuleInfo "History: Added GetFormatFromFile(), GetLoadFormats() and GetSaveFormats() helper functions. (fredborg)"
ModuleInfo "History: 1.03"
ModuleInfo "History: Updated to FreeImage 3.10.0. Adds OpenEXR and OpenJPG(2000) formats."
ModuleInfo "History: Win32 now compiles as a module, rather than using a DLL :-)"
ModuleInfo "History: Fixed colour problems when saving an image."
ModuleInfo "History: Fixed seek->end not seeking from the end."
ModuleInfo "History: Fixed multi-image bug where filename wasn't kept long enough."
ModuleInfo "History: Fixed TIFF save issue. (A freeimage bug, most likely)"
ModuleInfo "History: Fixed win32 TIFF load issue. (Another freeimage bug, most likely)"
ModuleInfo "History: 1.02"
ModuleInfo "History: Hacked MNG plugin to validate images using the .mng header."
ModuleInfo "History: Re-applied internal LibPNG support. Do not Import Pub.Libpng"
ModuleInfo "History: Loaded TFreeImages bitmaps are now not y-flipped at load-time. Therefore when saving they should now be in the correct orientation."
ModuleInfo "History: New TMultiFreeImage type for multi-page image support (gif, ico and tiff)."
ModuleInfo "History: New LoadAnimFreeImage() function to create a multi-frame TImage from a multi-page image."
ModuleInfo "History: 1.01"
ModuleInfo "History: Added CreateFromPixmap() function."
ModuleInfo "History: Added colorQuantize, dither, threshold, toneMapping, tmoDrago03 and tmoReinhard05 methods."
ModuleInfo "History: Added convertTo 4, 8, 16 methods."
ModuleInfo "History: Added more documentation."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (FreeImage 3.9.3)"

ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DPNG_iTXt_SUPPORTED -DPNG_PREFIX"
ModuleInfo "CC_OPTS: -DHAVE_PROTOTYPES"
ModuleInfo "CC_OPTS: -DFREEIMAGE_LIB"
ModuleInfo "CC_OPTS: -DOPJ_STATIC -DUSE_JPIP -DZ_PREFIX -DFREEIMAGE_JPEG_EXTERNAL_NAMES"
ModuleInfo "CC_OPTS: -DLIBRAW_NODLL -DLIBRAW_LIBRARY_BUILD"
ModuleInfo "CC_OPTS: -D__ANSI__"
?win32
' fixes snprintf not define error on gcc 4.7
ModuleInfo "CC_OPTS: -D_GLIBCXX_USE_C99_DYNAMIC"
?

Import BRL.Pixmap
Import BRL.Max2D
Import BRL.EndianStream
Import BRL.StandardIO

Import "common.bmx"

' NOTES :
'
' Changed FreeImage.h (Mac RGB colour issue)
'    Use FREEIMAGE_COLORORDER_RGB for Mac x86
'
' Changed ImfPxr24Compressor.cpp and ImfZipCompressor.cpp
'    Mods to support zlib prefix change.
'
' Changed zconf.h
'    Use fi_ prefix
'
' Changed pngprefix.h
'    Use fi_ prefix
'
' Changed jpeglib.h
'    Use fi_ prefix

Rem
bbdoc: Loads an image into a #TFreeImage object.
returns: A #TFreeImage object or Null if not found or not a valid image format.
about: Some image loaders can use flags to change the loading behaviour. Use 0 or &lt;TYPE_OF_BITMAP&gt;_DEFAULT (e.g. BMP_DEFAULT, ICO_DEFAULT, etc), to use normal
loading behaviour.
<p>
A list of the available optional loading flags follows:
	<table>
	<tr><th>Bitmap Type</th><th>Flag</th><th>Description</th></tr>
	<tr><td align="center"><b>Any Type</b></td><td>FIF_LOAD_NOPIXELS</td><td>When this flag is supported by a plugin, 
		load only header data and possibly metadata (including embedded thumbnail). When the flag is not supported, pixels are loaded.</td></tr>
	<tr><td align="center"><b>GIF</b></td><td>GIF_DEFAULT</td><td>&nbsp;</td></tr>
	<tr><td align="center">&nbsp;</td><td>GIF_LOAD256</td><td>Load the image as a 256 color image with unused palette entries, 
		if it's 16 or 2 color</td></tr>
	<tr><td align="center">&nbsp;</td><td>GIF_PLAYBACK</td><td>'Play' the GIF to generate each frame (as 32bpp) instead of returning raw
		frame data when loading</td></tr>
	<tr><td align="center"><b>ICO</b></td><td>ICO_MAKEALPHA</td><td>Convert to 32-bit and create an alpha channel from the AND-mask when loading</td></tr>
	<tr><td align="center"><b>JPEG</b></td><td>JPEG_DEFAULT</td><td>Load the file as fast as possible, sacrificing some quality</td></tr>
	<tr><td align="center">&nbsp;</td><td>JPEG_FAST</td><td>Load the file as fast as possible, sacrificing some quality</td></tr>
	<tr><td align="center">&nbsp;</td><td>JPEG_ACCURATE</td><td>Load the file with the best quality, sacrificing some speed</td></tr>
	<tr><td align="center">&nbsp;</td><td>JPEG_CMYK</td><td>This flag will load CMYK bitmaps as 32-bit separated CMYK (use | to combine with other load flags)</td></tr>
	<tr><td align="center">&nbsp;</td><td>JPEG_GREYSCALE</td><td>Load and convert to a 8-bit greyscale image (faster than loading as 24-bit and
		converting to 8-bit)</td></tr>
	<tr><td align="center">&nbsp;</td><td>Integer X such that flags = flags | (X Shl 16)</td><td>Load and resize the file such that size/X = 
		max(width, height)/X will return an image scaled by 2, 4 or 8 (i.e. the most appropriate requested size).</td></tr>
	<tr><td align="center">&nbsp;</td><td>JPEG_EXIFROTATE</td><td>Load and rotate according to Exif 'Orientation' tag if available</td></tr>
	<tr><td align="center"><b>PCD</b></td><td>PCD_DEFAULT</td><td>A PhotoCD picture comes in many sizes. This flag will load the one sized 768 x 512</td></tr>
	<tr><td align="center">&nbsp;</td><td>PCD_BASE</td><td>This flag will load the one sized 768 x 512</td></tr>
	<tr><td align="center">&nbsp;</td><td>PCD_BASEDIV4</td><td>This flag will load the bitmap sized 384 x 256</td></tr>
	<tr><td align="center">&nbsp;</td><td>PCD_BASEDIV16</td><td>This flag will load the bitmap sized 192 x 128</td></tr>
	<tr><td align="center"><b>PNG</b></td><td>PNG_IGNOREGAMMA</td><td>Avoid gamma correction</td></tr>
	<tr><td align="center"><b>PSD</b></td><td>PSD_DEFAULT</td><td>Load and convert to RGB[A]</td></tr>
	<tr><td align="center">&nbsp;</td><td>PSD_CMYK</td><td>Reads tags for separated CMYK (default is conversion to RGB)</td></tr>
	<tr><td align="center">&nbsp;</td><td>PSD_LAB</td><td>Reads tags for CIELab (default is conversion to RGB)</td></tr>
	<tr><td align="center"><b>RAW</b></td><td>RAW_DEFAULT</td><td>Load the file as linear RGB 48-bit</td></tr>
	<tr><td align="center">&nbsp;</td><td>RAW_PREVIEW</td><td>Try to load the embedded JPEG preview with included Exif data or default to RGB 24-bit</td></tr>
	<tr><td align="center">&nbsp;</td><td>RAW_DISPLAY</td><td>Load the file as RGB 24-bit</td></tr>
	<tr><td align="center">&nbsp;</td><td>RAW_HALFSIZE</td><td>Output a half-size color image</td></tr>
	<tr><td align="center"><b>TARGA</b></td><td>TARGA_LOAD_RGB888</td><td>If set the loader converts RGB555 and ARGB8888 -> RGB888</td></tr>
	<tr><td align="center"><b>TIFF</b></td><td>TIFF_CMYK</td><td>This flag will load CMYK bitmaps as separated CMYK (default is conversion to RGB)</td></tr>
	</table>
</p>
End Rem
Function LoadFreeImage:TFreeImage( url:Object, flags:Int = 0)

	Local imgStream:TStream = ReadStream( url )
	If Not imgStream Then
		Return Null
	End If
	
	Local fi:TFreeImage = TFreeImage.CreateFromStream(imgStream, flags)
	
	imgStream.close()
	
	Return fi

End Function

Rem
bbdoc: Loads a multi-page image into a #TMultiFreeImage object.
returns: A #TMultiFreeImage object or Null if not found or not a valid multi-page format.
End Rem
Function LoadMultiFreeImage:TMultiFreeImage( filename:String, readOnly:Int = True, flags:Int = 0 )
	
	Return TMultiFreeImage.CreateFromFile(filename, readOnly, flags)

End Function

Rem
bbdoc: Loads a multi-page image into a #TImage object.
returns: A #TImage object or Null if not found or not a valid multi-page format.
about: See #LoadImage for valid @flags values.
End Rem
Function LoadAnimFreeImage:TImage(filename:String, flags:Int = -1, rMask:Int = 0, gMask:Int = 0, bMask:Int = 0)
	Local mf:TMultiFreeImage = TMultiFreeImage.CreateFromFile(filename)
	If mf Then
		Local image:TImage = mf.toAnimImage(flags, rMask, gMask, bMask)
		mf.close()
		mf.Free()
		Return image
	End If
	
	Return Null
End Function

'
Global _freeImageErrorCallback(imageFormat:Int, message:String)

Rem
bbdoc: Set the user-defined error callback function.
End Rem
Function SetFreeImageErrorHandler(callback(imageFormat:Int, message:String))
	_freeImageErrorCallback = callback
End Function

Rem
bbdoc: A multi-page image.
about: FreeImage currently supports GIF, TIFF and ICO multi-page image formats.
<p>
To modify a page in a multi-page image, it needs to be opened with readOnly set to False.
Use #unlockPage to acquire a specific page image. Make your changes to it, and then use #lockPage to
apply the changes.<br>
When you #close the multi-page image, the changes will be saved.
</p>
End Rem
Type TMultiFreeImage

	Global hasError:Int = False

	' pointer to the current image info
	Field freeImagePtr:Byte Ptr
	
	Field filename:String
	
	Field currentPage:Int = -1
	Field currentImage:TFreeImage
	Field count:Int

	Rem
	bbdoc: Creates a new #TMultiFreeImage from the specified @filename.
	about: Set @readOnly to False, if you intend making changes to pages.
	End Rem
	Function CreateFromFile:TMultiFreeImage(filename:String, readOnly:Int = True, flags:Int = 0)
	
		hasError = False
	
		Local this:TMultiFreeImage = New TMultiFreeImage
		
		this.filename = filename
		this.freeImagePtr = bmx_multifreeimage_new(this, filename, readOnly, False)
		
		If Not this.isValidImage()
			Return Null
		End If
		
		If Not this.Load(flags) Then
			Return Null
		End If
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates a new, empty, multi-page image.
	about: Valid formats are : FIF_ICO, FIF_GIF, or FIF_TIFF.
	End Rem
	Function Create:TMultiFreeImage(filename:String, format:Int)
		Local this:TMultiFreeImage = New TMultiFreeImage
		
		this.filename = filename
		this.freeImagePtr = bmx_multifreeimage_new(this, filename, False, True)
		
		If Not this.newImage(format) Then
			Return Null
		End If
		
		Return this
		
	End Function

	Method isValidImage:Int()
	
		Local format:Int = bmx_MultiFreeImage_GetFileType(freeImagePtr)

		If format <> FIF_UNKNOWN Then
		
			Return True
		
		End If
		
		Return False
	
	End Method
	
	Method Load:Int(flags:Int = 0)

		' actually load the image
		bmx_multifreeimage_loadImage(freeImagePtr, flags)
		
		If hasError Then
			hasError = False
			Return False
		End If
	
		Return True
	End Method
	
	Method newImage:Int(format:Int)

		' create a new multi image
		bmx_multifreeimage_newImage(freeImagePtr, format)
		
		If hasError Then
			hasError = False
			Return False
		End If
	
		Return True
	
	End Method
	
	Function error(format:Int, message:Byte Ptr)
	
		hasError = True
		
		If _freeImageErrorCallback Then
			_freeImageErrorCallback(format, String.fromCString(message))
		Else
			Print "MultiFreeImage : " + String.fromCString(message)
		End If
	
	End Function
	
	Rem
	bbdoc: Resets the internal page position back to the start (ie. zero).
	End Rem
	Method reset()
		currentPage = -1
		hasNext()
	End Method
	
	Rem
	bbdoc: Returns True if there are more pages.
	End Rem
	Method hasNext:Int()
		' initialise !
		If currentPage < 0 Then
			count = pageCount()
			currentPage = 0
		End If
		
		Return currentPage < count
	End Method
	
	Rem
	bbdoc: Gets the next page.
	End Rem
	Method getNext:TFreeImage()
		If hasNext() Then
		
			If currentImage Then
				unlockPage(currentImage)
			End If
			
			currentImage = lockPage(currentPage)
		
			currentPage:+ 1
			
			Return currentImage
		End If
	End Method
	
	Rem
	bbdoc: Returns the number of pages currently available in the multi-paged image.
	End Rem
	Method pageCount:Int()
		Return bmx_multifreeimage_pageCount(freeImagePtr)
	End Method
	
	Rem
	bbdoc: Appends a new page to the end of the image.
	End Rem
	Method appendPage(image:TFreeImage)
		bmx_multifreeimage_append(freeImagePtr, image.freeImagePtr)
	End Method
	
	Rem
	bbdoc: Inserts a new page before the given position.
	about: @page has to be a number smaller than the current number of pages available.
	End Rem
	Method insertPage(image:TFreeImage, page:Int)
		bmx_multifreeimage_insert(freeImagePtr, image.freeImagePtr, page)
	End Method
	
	Rem
	bbdoc: Deletes the page on the given position.
	End Rem
	Method deletePage(page:Int)
		bmx_multifreeimage_deletePage(freeImagePtr, page)
	End Method
	
	Rem
	bbdoc: Locks a page in memory for editing.
	about: The page can now be saved to a different file or inserted into another multi-page image.
	When you are done with the image you <b>must</b> call #unlockPage to give the page back to the multi-page image
	and/or apply any changes made to the page.
	End Rem
	Method lockPage:TFreeImage(page:Int)
		Return TFreeImage.CreateFromBitmap(bmx_multifreeimage_lockPage(freeImagePtr, page))
	End Method
	
	Rem
	bbdoc: Unlocks a previously locked page and gives it back to the multi-page engine.
	about: When the @changed parameter is True, the page is marked changed and the new page data is applied
	in the multi-page image.
	End Rem
	Method unlockPage(image:TFreeImage, changed:Int = False)

		bmx_multifreeimage_unlockPage(freeImagePtr, image.freeImagePtr, changed)

	End Method
	
	Rem
	bbdoc: Moves the source page to the position of the target page.
	about: Returns True on success, False on failure.
	End Rem
	Method movePage:Int(source:Int, target:Int)
		Return bmx_multifreeimage_movePage(freeImagePtr, source, target)
	End Method
	
	Rem
	bbdoc: Closes a previously opened multi-page image and, when the image was not opened read-only, applies any changes made to it.
	End Rem
	Method close:Int()
		Return bmx_multifreeimage_close(freeImagePtr)
	End Method
	
	Rem
	bbdoc: Creates a TImage and fills the frames with all the pages from this #TMultiFreeImage.
	about: The parameters are the same 
	End Rem
	Method toAnimImage:TImage(flags:Int = -1, rMask:Int = 0, gMask:Int = 0, bMask:Int = 0)

		Local count:Int = pageCount()
		Local img:TFreeImage = lockPage(0)
		
		Local pix:TPixmap = img.getPixmap()
		
		If Not pix Return Null
		
		If flags = -1 Then
			flags = MASKEDIMAGE|FILTEREDIMAGE
		End If
		
		Local t:TImage = TImage.Create(pix.width, pix.height, count, flags, rMask, gMask, bMask)

		t.SetPixmap 0,pix.Copy()
		unlockPage(img)
		img.Free()
		
		For Local i:Int = 1 Until count
			img = lockPage(i)
			t.setPixmap(i, img.getPixmap().Copy())
			unlockPage(img)
			img.Free()
		Next
		
		Return t
	End Method
	
	Method Free()
		If currentImage Then
			unlockPage(currentImage)
			currentImage = Null
		End If
		
		If freeImagePtr Then
			bmx_multifreeimage_delete(freeImagePtr)
			freeImagePtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A FreeImage image.
about: The base type for loading and manipulating images with FreeImage.
End Rem
Type TFreeImage

	Global hasError:Int = False

	' pointer to the current image info
	Field freeImagePtr:Byte Ptr
	
	' pointer to the "display" image info
	Field displayImagePtr:Byte Ptr
	
	' the image stream
	Field stream:TStream
	
	' our displayable pixmap
	Field pixmap:TPixmap
	
	' has the image data changed ?
	Field imageChanged:Int = True
	
	Rem
	bbdoc: The image width
	End Rem
	Field width:Int
	
	Rem
	bbdoc: The image height
	End Rem
	Field height:Int
	
	Rem
	bbdoc: Bits per pixel
	End Rem
	Field bitsPerPixel:Int

	Rem
	bbdoc: Image palette size, if appropriate.
	End Rem
	Field paletteSize:Int

	Rem
	bbdoc: The image pitch
	End Rem
	Field pitch:Int

	Rem
	bbdoc: Creates a #TFreeImage from a TStream.
	End Rem
	Function CreateFromStream:TFreeImage(stream:TStream, flags:Int = 0)
	
		hasError = False
	
		Local this:TFreeImage = New TFreeImage
		
		this.stream = stream
		
		this.freeImagePtr = bmx_freeimage_new(this)
		
		If Not this.isValidImage()
			Return Null
		End If
		
		If Not this.Load(flags) Then
			Return Null
		End If
		
		this.init()
		
		Return this
	End Function
	
	Function CreateFromBitmap:TFreeImage(bitmap:Byte Ptr)
	
		hasError = False
		
		' if this isn't Null, it should be a valid bitmap...
		If bitmap Then
		
			Local this:TFreeImage = New TFreeImage
			
			this.freeImagePtr = newImageFromBitmap(this, bitmap)
			
			this.init()
			
			Return this	
		
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: Creates a #TFreeImage from a pixmaps' pixel data.
	End Rem
	Function CreateFromPixmap:TFreeImage(pixmap:TPixmap)
	
		Local redMask:Int
		Local blueMask:Int
		Local greenMask:Int
		
		Local converted:Int = False
		
		' we can't handle an I8 pixmap, so we need to do some magic...
		If pixmap.format = PF_I8 Then
			pixmap = ConvertPixmap(pixmap, PF_BGR888)
			converted = True
		End If
	
		Select pixmap.format
			Case PF_BGR888
				redMask = $FF
				greenMask = $FF00
				blueMask = $FF0000
			Case PF_RGB888
				redMask = $FF0000
				greenMask = $FF00
				blueMask = $FF
			Case PF_BGRA8888
				redMask = $FF000000
				greenMask = $FF0000
				blueMask = $FF00
			Case PF_RGBA8888
				redMask = $FF00
				greenMask = $FF0000
				blueMask = $FF000000
		End Select
		
		
		If Not converted Then
			Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertFromRawBits(pixmap.pixels, ..
					pixmap.width, pixmap.height, pixmap.pitch, BitsPerPixel[pixmap.format], redMask, ..
					greenMask, blueMask))
		Else
			' convert the image back to the correct number of bits...
		
			Local image:TFreeImage = TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertFromRawBits(pixmap.pixels, ..
					pixmap.width, pixmap.height, pixmap.pitch, BitsPerPixel[pixmap.format], redMask, ..
					greenMask, blueMask))

			Return image.convertTo8Bits()
		
		End If
	
	End Function
	
	Function newImageFromBitmap:Byte Ptr(img:TFreeImage, bitmap:Byte Ptr)
		Local freeImagePtr:Byte Ptr = bmx_freeimage_new(img)
		bmx_freeimage_setBitmap(freeImagePtr, bitmap)
		Return freeImagePtr
	End Function
	
	Method Free()
		pixmap = Null
	
		If displayImagePtr And (displayImagePtr <> freeImagePtr) Then
			bmx_freeimage_delete(displayImagePtr)
			displayImagePtr = Null
		End If

		If freeImagePtr Then
			bmx_freeimage_delete(freeImagePtr)
			freeImagePtr = Null
		End If
	End Method
	
	' tidy up our memory
	Method Delete()
		Free()
	End Method
	
	
	Method isValidImage:Int()
	
		Local format:Int = bmx_FreeImage_GetFileTypeFromHandle(freeImagePtr)

		If format <> FIF_UNKNOWN Then
		
			Return True
		
		End If
		
		Return False
	
	End Method
	
	Method Load:Int(flags:Int = 0)

		' actually load the image
		bmx_freeimage_loadImage(freeImagePtr, flags)
		
		If hasError Then
			hasError = False
			Return False
		End If
	
		Return True
	End Method
	
	Method calc()
		width = bmx_freeimage_GetWidth(freeImagePtr)
		height = bmx_freeimage_GetHeight(freeImagePtr)
		bitsPerPixel = bmx_freeimage_GetBPP(freeImagePtr)
		paletteSize = bmx_freeimage_GetColorsUsed(freeImagePtr)
		pitch = bmx_freeimage_GetPitch(freeImagePtr)
	End Method
	
	Method Init()
		
		imageChanged = False
		
		' image info
		calc()
		
		If displayImagePtr And displayImagePtr <> freeImagePtr Then
			bmx_freeimage_delete(displayImagePtr)
		End If
		

		Local format:Int
		
		Select bitsPerPixel
			' we don't support 1 bpp images in BlitzMax... convert to something else.
			Case 1
				If bmx_freeimage_isTransparent(freeImagePtr) Then
					displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo32Bits(freeImagePtr))

					?win32
					format = PF_BGRA8888
					?macos
					format = PF_RGBA8888
					?linux
					format = PF_BGRA8888
					?
				Else
					displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo8Bits(freeImagePtr))
					format = PF_I8
				End If

			Case 4,8
				If bmx_freeimage_isTransparent(freeImagePtr) Then
					displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo32Bits(freeImagePtr))

					?win32
					format = PF_BGRA8888
					?macos
					format = PF_RGBA8888
					?linux
					format = PF_BGRA8888
					?
				Else
					' only convert image if it isn't greyscale
					If bmx_freeimage_GetColorType(freeImagePtr) = FIC_PALETTE Then
						displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo24Bits(freeImagePtr))
						
						?win32
						format = PF_BGR888
						?macos
						format = PF_RGB888
						?linux
						format = PF_BGR888
						?
					Else
						displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo8Bits(freeImagePtr))
						format = PF_I8
					End If
				End If

			Case 24
				displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo24Bits(freeImagePtr))
				?win32
				format = PF_BGR888
				?macos
				format = PF_RGB888
				?linux
				format = PF_BGR888
				?
			Case 32
				displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo32Bits(freeImagePtr))
				?win32
				format = PF_BGRA8888
				?macos
				format = PF_RGBA8888
				?linux
				format = PF_BGRA8888
				?
			Default ' for everything else...
				displayImagePtr = newImageFromBitmap(Self, bmx_freeimage_convertTo24Bits(freeImagePtr))
						
				?win32
				format = PF_BGR888
				?macos
				format = PF_RGB888
				?linux
				format = PF_BGR888
				?
		End Select
		
		bmx_freeimage_FlipVertical(displayImagePtr)
		
		' build a pixmap... based on the "displayable" bitmap
		pixmap = CreateStaticPixmap(bmx_freeimage_getImage(displayImagePtr), ..
				bmx_freeimage_GetWidth(displayImagePtr), ..
				bmx_freeimage_GetHeight(displayImagePtr), ..
				bmx_freeimage_GetPitch(displayImagePtr),  format)
		
	End Method
	
	Method getBitmap:Byte Ptr()
		Return bmx_freeimage_getBitmap(freeImagePtr)
	End Method

	
	Method setBitmap(bitmap:Byte Ptr)
		bmx_freeimage_setBitmap(freeImagePtr, bitmap)
	End Method
	
	Function error(format:Int, message:Byte Ptr)
	
		Local t:String = String.fromCString(message)


		If Not t.contains("Warning:") Then
			hasError = True
		End If

		If _freeImageErrorCallback Then
			_freeImageErrorCallback(format, t)
		Else
			Print "FreeImage: " + t
		End If

	End Function
	
	
	Function read:Int(handler:TFreeImage, c:Byte Ptr, n:Int)

		Return handler.stream.read(c, n)
	
	End Function
	
	Function write:Int(handler:TFreeImage, c:Byte Ptr, n:Int)

		Return handler.stream.write(c, n)

	End Function

?Not ptr64
	Function tell:Int(handler:TFreeImage)
		Return handler.stream.pos()
	End Function
?ptr64
	Function tell:Long(handler:TFreeImage)
		Return handler.stream.pos()
	End Function
?
	Function seek(handler:TFreeImage, pos:Int)
		handler.stream.seek(pos)
	End Function
	
?Not ptr64
	Function size:Int(handler:TFreeImage, pos:Int)
		Return handler.stream.size()
	End Function
?ptr64
	Function size:Long(handler:TFreeImage, pos:Int)
		Return handler.stream.size()
	End Function
?	
	Rem
	bbdoc: Returns the size of the DIB-element of a FIBITMAP in memory
	about: The BITMAPINFOHEADER + palette + data bits (note that this is not the real size of a FIBITMAP, only the size of its DIB-element).
	End Rem
	Method getDIBSize:Int()
		Return bmx_freeimage_GetDIBSize(freeImagePtr)
	End Method
	
	Rem
	bbdoc: Calculates (or at least estimates) the total memory usage of a FreeImage bitmap.
	about: This includes the ICC profile size, the size of the embedded thumbnail (if any), the memory required for
	all the metadata, as well as any FreeImage internal (housekeeping) memory.
	End Rem
	Method getMemorySize:Int()
		Return bmx_freeimage_GetMemorySize(freeImagePtr)
	End Method
	
	Rem
	bbdoc: Gets the @current pixmap for the image.
	about: Note: Once the image data has changed, you should acquire a new pixmap via this method since
	pixmap data is likely to be invalidated.
	End Rem
	Method getPixmap:TPixmap()
	
		If imageChanged Then
		
			Init()
		
		End If
	
		Return pixmap
		
	End Method
	

	Rem
	bbdoc: Gets a memory pointer to the first pixel of a scanline.
	about: For direct access to the freeimage pixel data, can be used to read or write raw data from the freeimage bitmap.
	Make sure you know the pixel format of the bitmap before using it.
	End Rem
	Method GetScanline:Byte Ptr(scanline:Int)
	
		Return bmx_freeimage_GetScanline(freeImagePtr,scanline)
		
	End Method

	
	Rem
	bbdoc: Resamples the image to the desired destination @width and @height using the specified @filter.
	about: The following filters can be used for resampling :
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>FILTER_BOX</td><td>Box, pulse, Fourier window, 1st order (constant) b-spline</td></tr>
	<tr><td>FILTER_BICUBIC</td><td>Mitchell & Netravali's two-param cubic filter</td></tr>
	<tr><td>FILTER_BILINEAR</td><td>Bilinear filter</td></tr>
	<tr><td>FILTER_BSPLINE</td><td>4th order (cubic) b-spline</td></tr>
	<tr><td>FILTER_CATMULLROM</td><td>Catmull-Rom spline, Overhauser spline</td></tr>
	<tr><td>FILTER_LANCZOS3</td><td>Lanczos3 filter</td></tr>
	</table>
	End Rem
	Method rescale:TFreeImage(width:Int, height:Int, filter:Int)
	
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_Rescale(freeImagePtr, width, height, filter))
	
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method rescaleRect:TFreeImage(destWidth:Int, destHeight:Int, srcleft:Int, srcTop:Int, srcRight:Int, srcBottom:Int, filter:Int = FILTER_CATMULLROM, flags:Int = 0)
	
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_RescaleRect(freeImagePtr, destWidth, destHeight, srcleft, srcTop, srcRight, srcBottom, filter, flags))
	
	End Method
	
	Rem
	bbdoc: Creates a thumbnail so that the output fits inside a square of @maxPixelSize, keeping aspect ratio.
	about: Downsampling is done using a bilinear filter (see #rescale).
	End Rem
	Method makeThumbnail:TFreeImage(maxPixelSize:Int)
	
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_MakeThumbnail(freeImagePtr, maxPixelSize))
	
	End Method

	Rem
	bbdoc: Retrieves a link to the thumbnail that may be available in the image.
	about: Some image formats allow a thumbnail image to be embedded together with the output image file.
	When this thumbnail image is present in a file, it is automatically loaded by FreeImage (whatever the loading flag, even when using the FIF_LOAD_NOPIXEL flag).
	Image formats that currently support thumbnail loading are JPEG (Exif or JFIF formats), PSD, EXR, TGA and TIFF.
	End Rem
	Method getThumbnail:TFreeImage()

		Return TFreeImage.CreateFromBitmap(bmx_freeimage_GetThumbnail(freeImagePtr))
	
	End Method

	Rem
	bbdoc: Rotates a standard image (1-, 8-bit greyscale or a 24-, 32-bit color), a RGB(A)16 or RGB(A)F image by means of 3 shears.
	about: The angle of rotation is specified by the @angle parameter in degrees. Rotation occurs around the center of the image area.
	Rotated image retains size and aspect ratio of source image (destination image size is usually bigger), so that this method
	should be used when rotating an image by 90, 180 or 270 degrees.
	End Rem
	Method rotate:TFreeImage(angle:Double, color:TRGBQuad = Null)
	
		If color Then
			Return TFreeImage.CreateFromBitmap(bmx_freeimage_Rotate(freeImagePtr, angle, color.colorPtr))
		Else
			Return TFreeImage.CreateFromBitmap(bmx_freeimage_Rotate(freeImagePtr, angle, Null))
		End If
	
	End Method
	
	Rem
	bbdoc: Performs rotation and / or translation of an image using 3rd order (cubic) B-Spline.
	about: The rotated image will have the same width and height as the source image.<br>
	The @angle of rotation is specified in degrees.<br>
	Horizontal and Vertical image translations (in pixels) are specified by the @xShift and @yShift
	parameters.<br>
	Rotation occurs around the center specified by @xOrigin and @yOrigin.<br>
	When @useMask is True, the irrelevant part of the image is set to a black colour, otherwise,
	a mirroring technique is used to fill irrelevant pixels.
	End Rem
	Method rotateEx:TFreeImage(angle:Double, xShift:Double, yShift:Double, xOrigin:Double, yOrigin:Double, useMask:Int)
	
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_RotateEx(freeImagePtr, angle, xShift, yShift, xOrigin, yOrigin, useMask))
	
	End Method
	
	Rem
	bbdoc: Flip the image horizontally along the vertical axis.
	End Rem
	Method flipHorizontal()
		bmx_freeimage_FlipHorizontal(freeImagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Flip the image vertically along the horizontal axis.
	End Rem
	Method flipVertical()
		bmx_freeimage_FlipVertical(freeImagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method adjustCurve:Int(lut:Byte[], channel:Int)
		Local res:Int = bmx_freeimage_AdjustCurve(freeImagePtr, Varptr lut, channel)
		If res Then
			imageChanged = True
		End If
		Return res
	End Method

	Rem
	bbdoc: Performs a Gamma correction.
	about: A @gamma value of 1, leaves the image alone, less than 1 darkens it, and greater than 1 lightens it.
	End Rem
	Method adjustGamma(Gamma:Double)
		bmx_freeimage_AdjustGamma(freeImagePtr, Gamma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Adjusts the brightness by a certain amount.
	about: The @percentage should be between -100 and 100, where a value of 0 means no change,
	less than 0 makes the image darker, and greater than 0 makes the image lighter.
	End Rem
	Method adjustBrightness(percentage:Double)
		bmx_freeimage_AdjustBrightness(freeImagePtr, percentage)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Adjusts the contrast by a certain amount.
	about: The @percentage should be between -100 and 100, where a value of 0 means no change,
	less than 0 decreases the contrast, and greater than 0 increases the contrast.
	End Rem
	Method adjustConstrast(percentage:Double)
		bmx_freeimage_AdjustContrast(freeImagePtr, percentage)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method adjustColors:Int(brightness:Double, contrast:Double, Gamma:Double, invert:Int = False)
		Local res:Int = bmx_freeimage_AdjustColors(freeImagePtr, brightness, contrast, Gamma, invert)
		If res Then
			imageChanged = True
		End If
		Return res
	End Method
	
	Rem
	bbdoc: Inverts each pixel.
	End Rem
	Method invert()
		bmx_freeimage_Invert(freeImagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Retrieves the red, green, blue or alpha @channel of an image.
	returns: The extracted channel or Null otherwise.
	about: 
	End Rem
	Method getChannel:TFreeImage(channel:Int)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_GetChannel(freeImagePtr, channel))
	End Method
	
	Rem
	bbdoc: Inserts a greyscale image into a RGB[A] image.
	about: @src must have the same width and height.
	End Rem
	Method setChannel:Int(src:TFreeImage, channel:Int)
		Local res:Int = bmx_freeimage_SetChannel(freeImagePtr, src.freeImagePtr, channel)
		If res Then
			imageChanged = True
		End If
		Return res
	End Method
	
	Rem
	bbdoc: Retrieves the real part, imaginary part, magnitude or phase of a complex image (image whose type is FIT_COMPLEX).
	about: The extracted channel as a FIT_DOUBLE image if successful and returns NULL otherwise.
	End Rem
	Method getComplexChannel:TFreeImage(channel:Int)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_GetComplexChannel(freeImagePtr, channel))
	End Method

	Rem
	bbdoc: Sets the real or imaginary part of a complex image (image whose type is FIT_COMPLEX).
	about: @src must have the same width and height.
	End Rem
	Method setComplexChannel:Int(src:TFreeImage, channel:Int)
		Local res:Int = bmx_freeimage_SetComplexChannel(freeImagePtr, src.freeImagePtr, channel)
		If res Then
			imageChanged = True
		End If
		Return res
	End Method

	Rem
	bbdoc: Copy a sub part of the image.
	returns: The copied image if successful, or Null otherwise.
	about: ( @x0, @y0 ) represents the top left of the rectangle to copy. ( @x1, @y1 ) the bottom right.
	End Rem
	Method copy:TFreeImage(x0:Int, y0:Int, x1:Int, y1:Int)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_Copy(freeImagePtr, x0, y0, x1, y1))
	End Method
	
	Rem
	bbdoc: Alpha blend or combine a sub part image with the image.
	returns: True if successful, or False otherwise.
	about: The images are alpha blended if @alpha is between 0 and 255. For values over 255, the two images
	are combined.
	End Rem
	Method paste:Int(source:TFreeImage, x:Int, y:Int, alpha:Int)
		Local ok:Int = bmx_freeimage_Paste(freeImagePtr, source.freeImagePtr, x, y, alpha)
		If ok Then
			imageChanged = True
		End If
		Return ok
	End Method
	
	Rem
	bbdoc: Converts an image to 8-bit greyscale with a linear ramp.
	returns: The converted image.
	about: Contrary to the convertTo8Bits method, 1-, 4- and 8-bit palletised images are correctly converted.
	End Rem
	Method convertToGreyscale:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertToGreyscale(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts an image to 4 Bits.
	returns: The converted image.
	about: If the image was a high-color bitmap (16, 24 or 32-bit) or if it was a monochrome or greyscale
	bitmap (1 or 8-bit), the end result will be a greyscale bitmap, otherwise (1-bit palletised bitmaps)
	it will be a palletised bitmap. A clone of the input bitmap is returned for 4-bit bitmaps.
	End Rem
	Method convertTo4Bits:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertTo4Bits(freeImagePtr))
	End Method

	Rem
	bbdoc: Converts an image to 8 Bits.
	returns: The converted image.
	about: If the image was a high-color bitmap (16, 24 or 32-bit) or if it was a monochrome or greyscale bitmap
	(1 or 4-bit), the end result will be a greyscale bitmap, otherwise (1 or 4-bit palletised bitmaps)
	it will be a palletised bitmap. A clone of the input bitmap is returned for 8-bit bitmaps.
	End Rem
	Method convertTo8Bits:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertTo8Bits(freeImagePtr))
	End Method

	Rem
	bbdoc: Converts an image to 16 Bits, where each pixel has a color pattern of 5 bits red, 5 bits green and 5 bits blue
	returns: The converted image.
	End Rem
	Method convertTo16Bits555:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertTo16Bits555(freeImagePtr))
	End Method

	Rem
	bbdoc: Converts a bitmap to 16 bits, where each pixel has a color pattern of 5 bits red, 6 bits green and 5 bits blue.
	returns: The converted image.
	End Rem
	Method convertTo16Bits565:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertTo16Bits565(freeImagePtr))
	End Method

	Rem
	bbdoc: Converts an image to 24 Bits.
	returns: The converted image.
	End Rem
	Method convertTo24Bits:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertTo24Bits(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts an image to 32 Bits.
	returns: The converted image.
	End Rem
	Method convertTo32Bits:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertTo32Bits(freeImagePtr))
	End Method

	Rem
	bbdoc: Converts an image to RGBF (RGB 32Bit Float).
	returns: The converted image.
	End Rem
	Method convertToRGBF:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertToRGBF(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts a 24- or 32-bit RGB(A) standard image or a 48- or 64-bit RGB(A) image to a FIT_RGBAF type image.
	returns: The converted image.
	about: Conversion is done by copying the source integer pixel values into the destination float pixel values, and
	dividing by the maximum source pixel value (i.e. 255 or 65535) so that the output image is in the range [0..1].
	For 128-bit RGBAF input images, a clone of the input is returned.
	End Rem
	Method convertToRGBAF:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertToRGBAF(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts a bitmap to an unsigned 16-bit RGB image.
	returns: The converted image.
	End Rem
	Method convertToRGB16:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertToRGB16(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts a bitmap to an unsigned 16-bit RGBA image (i.e. image whose type is FIT_RGBA16).
	returns: The converted image.
	about: Standard bitmaps are first converted (if needed) to 32-bit RGBA images and then conversion is done by
	multiplying the 8-bit channel by 256. For 16-bit FIT_RGBA16 images, a clone of the input is returned.
	End Rem
	Method convertToRGBA16:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertToRGBA16(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts an image of any type to type @destType.
	about: When @destType is equal to FIT_BITMAP, the method calls ConvertToStandardType. Otherwise, conversion is
	done using standard C language casting convention. When a conversion is not allowed, a NULL value is returned
	and an error message is thrown (it can be caught using FreeImage_SetOutputMessage). The following conversions
	are currently allowed by the library (other conversions may be added easily if needed):
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>FIT_UNKNOWN</td><td> unknown type </td></tr>
	<tr><td> FIT_BITMAP </td><td>standard image : 1-, 4-, 8-, 16-, 24-, 32-bit</td></tr>
	<tr><td> FIT_UINT16 </td><td> array of unsigned short : unsigned 16-bit </td></tr>
	<tr><td> FIT_INT16 </td><td> array of short : signed 16-bit </td></tr>
	<tr><td> FIT_UINT32 </td><td> array of unsigned long : unsigned 32-bit </td></tr>
	<tr><td> FIT_INT32 </td><td> array of long : signed 32-bit </td></tr>
	<tr><td> FIT_FLOAT </td><td> array of float : 32-bit IEEE floating point </td></tr>
	<tr><td> FIT_DOUBLE </td><td> array of double : 64-bit IEEE floating point </td></tr>
	<tr><td> FIT_COMPLEX </td><td> array of FICOMPLEX : 2 x 64-bit IEEE floating point </td></tr>
	<tr><td> FIT_RGB16 </td><td> 48-bit RGB image : 3 x 16-bit </td></tr>
	<tr><td> FIT_RGBA16 </td><td> 64-bit RGBA image : 4 x 16-bit </td></tr>
	<tr><td> FIT_RGBF </td><td> 96-bit RGB float image : 3 x 32-bit IEEE floating point </td></tr>
	<tr><td> FIT_RGBAF </td><td> 128-bit RGBA float image : 4 x 32-bit IEEE floating point </td></tr>
	</table>
	End Rem
	Method convertToType:TFreeImage(destType:Int, scaleLinear:Int = True)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ConvertToType(freeImagePtr, destType, scaleLinear))
	End Method
	
	Rem
	bbdoc: Quantizes a high-color 24-bit bitmap to an 8-bit palette color bitmap.
	returns: The converted image.
	about: @quantize specifies the color reduction algorithm to be used:
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>FIQ_WUQUANT</td><td>Xiaolin Wu color quantization algorithm</td></tr>
	<tr><td>FIQ_NNQUANT</td><td>NeuQuant neural-net quantization algorithm by Anthony Dekker</td></tr>
	<tr><td>FIQ_LFPQUANT</td><td> Lossless Fast Pseudo-Quantization Algorithm by Carsten Klein </td></tr>
	</table>
	End Rem
	Method colorQuantize:TFreeImage(quantize:Int)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ColorQuantize(freeImagePtr, quantize))
	End Method

	Rem
	bbdoc: Makes an exact reproduction of an existing image, including metadata and attached profile, if any.
	returns: A clone of the image.
	End Rem
	Method clone:TFreeImage()
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_Clone(freeImagePtr))
	End Method
	
	Rem
	bbdoc: Converts image to a 1-bit monochrome bitmap using a dithering algorithm.
	about: @algorithm specifies the dithering algorithm to be used.
	The function first converts the image to a 8-bit greyscale bitmap, then is dithered using one of the
	following algorithms:
	<table>
	<tr><th>Algorithm</th><th>Description</th></tr>
	<tr><td>FID_FS</td><td>Floyd &amp; Steinberg error diffusion algorithm</td></tr>
	<tr><td>FID_BAYER4x4</td><td>Bayer ordered dispersed dot dithering (order 2 - 4x4 -dithering matrix)</td></tr>
	<tr><td>FID_BAYER8x8</td><td>Bayer ordered dispersed dot dithering (order 3 - 8x8 -dithering matrix)</td></tr>
	<tr><td>FID_CLUSTER6x6</td><td>Ordered clustered dot dithering (order 3 - 6x6 matrix)</td></tr>
	<tr><td>FID_CLUSTER8x8</td><td>Ordered clustered dot dithering (order 4 - 8x8 matrix)</td></tr>
	<tr><td>FID_CLUSTER16x16</td><td>Ordered clustered dot dithering (order 8 - 16x16 matrix)</td></tr>
	</table>
	End Rem
	Method dither:TFreeImage(algorithm:Int)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_Dither(freeImagePtr, algorithm))
	End Method
	
	Rem
	bbdoc: Converts image to a 1-bit monochrome bitmap using a threshold @t between [0..255].
	about: The method first converts the bitmap to a 8-bit greyscale bitmap. Then, any brightness level
	that is less than @t is set to zero, otherwise to 1. For 1-bit input bitmaps, the method clones the
	input bitmap and builds a monochrome palette.
	End Rem
	Method threshold:TFreeImage(t:Byte)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_Threshold(freeImagePtr, t))
	End Method

	Rem
	bbdoc: Saves the image to @url, which can be a filename or Stream.
	returns: True if successful, or False otherwise.
	about: @format defines the image type to save.<br>
	@flags can be used to change the behaviour or enable a feature for saving. Or leave out for default behaviour.<br>
	A list of valid image formats and optional flags follows:
	<table>
	<tr><th>Format</th><th>Flag</th><th>Description</th></tr>
	<tr><td align="center"><b>FIF_BMP</b><br>Windows Bitmap</td><td>BMP_DEFAULT</td><td>Save without any compression</td></tr>
	<tr><td>&nbsp;</td><td>BMP_SAVE_RLE</td><td>Compress the bitmap using RLE when saving</td></tr>
	<tr><td align="center"><b>FIF_GIF</b><br>Graphics Interchange Format</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	<tr><td align="center"><b>FIF_HDR</b><br>High Dynamic Range</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	<tr><td align="center"><b>FIF_ICO</b><br>Windows Icon</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	<tr><td align="center"><b>FIF_JPEG</b><br>JPEG - JFIF Compliant</td><td>JPEG_DEFAULT</td><td>Saves with good quality (75:1)</td></tr>
	<tr><td>&nbsp;</td><td>JPEG_QUALITYSUPERB</td><td>Saves with superb quality (100:1)</td></tr>
	<tr><td>&nbsp;</td><td>JPEG_QUALITYGOOD</td><td>Saves with good quality (75:1)</td></tr>
	<tr><td>&nbsp;</td><td>JPEG_QUALITYNORMAL</td><td>Saves with normal quality (50:1)</td></tr>
	<tr><td>&nbsp;</td><td>JPEG_QUALITYAVERAGE</td><td>Saves with average quality (25:1)</td></tr>
	<tr><td>&nbsp;</td><td>JPEG_QUALITYBAD</td><td>Saves with bad quality (10:1)</td></tr>
	<tr><td>&nbsp;</td><td>Int X in [0..100]</td><td>Save with quality X:1</td></tr>
	<tr><td>&nbsp;</td><td>JPEG_PROGRESSIVE</td><td>Saves as a progressive JPEG file (use | to combine with JPEG quality flags)</td></tr>
	<tr><td align="center"><b>FIF_PBM</b><br>Portable Bitmap</td><td>PNM_DEFAULT</td><td>Saves the bitmap as a binary file</td></tr>
	<tr><td>&nbsp;</td><td>PNM_SAVE_RAW</td><td>Saves the bitmap as a binary file</td></tr>
	<tr><td>&nbsp;</td><td>PNM_SAVE_ASCII</td><td>Saves the bitmap as an ASCII file</td></tr>
	<tr><td align="center"><b>FIF_PGM</b><br>Portable Greymap</td><td>PNM_DEFAULT</td><td>Saves the bitmap as a binary file</td></tr>
	<tr><td>&nbsp;</td><td>PNM_SAVE_RAW</td><td>Saves the bitmap as a binary file</td></tr>
	<tr><td>&nbsp;</td><td>PNM_SAVE_ASCII</td><td>Saves the bitmap as an ASCII file</td></tr>
	<tr><td align="center"><b>FIF_PNG</b><br>Portable Network Graphics</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	<tr><td align="center"><b>FIF_PPM</b><br>Portable Pixelmap</td><td>PNM_DEFAULT</td><td>Saves the bitmap as a binary file</td></tr>
	<tr><td>&nbsp;</td><td>PNM_SAVE_RAW</td><td>Saves the bitmap as a binary file</td></tr>
	<tr><td>&nbsp;</td><td>PNM_SAVE_ASCII</td><td>Saves the bitmap as an ASCII file</td></tr>
	<tr><td align="center"><b>FIF_TARGA</b><br>Truevision Targa</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	<tr><td align="center"><b>FIF_TIFF</b><br>Tagged Image File Format</td><td>TIFF_DEFAULT</td><td>Save using CCITTFAX4 compression for 1-bit bitmaps and LZW compression for any other bitmaps</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_CMYK</td><td>Stores tags for separated CMYK (use | to combine with TIFF compression flags)</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_PACKBITS</td><td>Save using PACKBITS compression.</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_DEFLATE</td><td>Save using DEFLATE compression (also known as ZLIB compression)</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_ADOBE_DEFLATE</td><td>Save using ADOBE DEFLATE compression</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_NONE</td><td>Save without any compression</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_CCITTFAX3</td><td>Save using CCITT Group 3 fax encoding</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_CCITTFAX4</td><td>Save using CCITT Group 4 fax encoding</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_LZW</td><td>Save using LZW compression</td></tr>
	<tr><td>&nbsp;</td><td>TIFF_JPEG</td><td>Save using JPEG compression (8-bit greyscale and 24-bit only. Default to LZW for other bitdepths).</td></tr>
	<tr><td align="center"><b>FIF_WBMP</b><br>Wireless Bitmap</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	<tr><td align="center"><b>FIF_XPM</b><br>X11 Pixmap Format</td><td>&nbsp;</td><td>&nbsp;</td></tr>
	</table>	
	End Rem
	Method save:Int(url:Object, format:Int, flags:Int = 0)

		Local imgStream:TStream = WriteStream( url )
		If Not imgStream Then
			Return False
		End If
		
		stream = imgStream
		
		Local ok:Int = bmx_freeimage_Save(freeImagePtr, format, flags)
		
		imgStream.close()
		
		Return ok
	
	End Method
	
	Rem
	bbdoc: Converts a High Dynamic Range image (48-bit RGB or 96-bit RGBF) to a 24-bit RGB image, suitable for display.
	about: The meaning of @param1 and @param2 depends on the value of @algorithm. See #tmoDrago03 and #tmoReinhard05
	for details.
	<table>
	<tr><th>Algorithm</th><th>Description</th></tr>
	<tr><td>FITMO_DRAGO03</td><td>Adaptive logarithmic mapping</td></tr>
	<tr><td>FITMO_REINHARD05</td><td>Dynamic range reduction inspired by photoreceptor physiology</td></tr>
	<tr><td>FITMO_FATTAL02</td><td>Gradient domain high dynamic range compression</td></tr>
	</table>
	End Rem
	Method toneMapping:TFreeImage(algorithm:Int, param1:Double, param2:Double)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ToneMapping(freeImagePtr, algorithm, param1, param2))
	End Method
	
	Rem
	bbdoc: Converts a High Dynamic Range image to a 24-bit RGB image using a global operator based on logarithmic compression of luminance values, imitating the human response to light.
	about: A bias power function is introduced to adaptively vary logarithmic bases, resulting in good preservation
	of details and contrast.<br/>
	Upon entry, @gamma (where @gamma > 0) is a gamma correction that is applied after the tone mapping.
	A value of 1 means no correction. The default 2.2 value, used in the original author's paper, is
	recommended as a good starting value.<br/>
	The @exposure parameter is an exposure scale factor allowing users to adjust the brightness
	of the output image to their displaying conditions. The default value (0) means that no correction
	is applied. Higher values will make the image lighter whereas lower values make the image darker.
	End Rem
	Method tmoDrago03:TFreeImage(Gamma:Double = 2.2, exposure:Double = 0)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_TmoDrago03(freeImagePtr, Gamma, exposure))
	End Method

	Rem
	bbdoc: Converts a High Dynamic Range image to a 24-bit RGB image using a global operator inspired by photoreceptor physiology of the human visual system.
	about: Upon entry, the @intensity parameter, in the range [-8, 8], controls the overall image intensity
	The default value 0 means no correction. Higher values will make the image lighter whereas lower values
	make the image darker.<br/>
	The contrast parameter, in the range [0.3, 1.0], controls the overall image contrast. When using the default
	value (0), this parameter is calculated automatically.
	End Rem
	Method tmoReinhard05:TFreeImage(intensity:Double = 0, contrast:Double = 0)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_TmoReinhard05(freeImagePtr, intensity, contrast))
	End Method

	Rem
	bbdoc: Converts a High Dynamic Range image to a 24-bit RGB image using a global / local operator inspired by photoreceptor physiology of the human visual system.
	about: User parameters control intensity, contrast, and level of adaptation.
	Upon entry, the intensity parameter, in the range [-8, 8], controls the overall image intensity. The
	default value 0 means no correction. Higher values will make the image lighter whereas lower values
	make the image darker.<br/>
	The contrast parameter, in the range [0.3, 1.0[, controls the overall image contrast. When using the
	default value (0), this parameter is calculated automatically.<br/>
	The adaptation parameter, in range [0:1], controls the level of light adaptation. When using the default
	value (1), adaptation is local and is based on the pixel intensity. When using a value 0, the adaptation is
	global and is based on the average channel intensity.<br/>
	The color_correction parameter, in range [0:1], controls the level of chromatic adaptation. Using the default value
	(0) means no chromatic adaptation, i.e. the adaptation level is the same for all 3 color channels. Setting this
	value to 1 means that each R, G, B channel is treated independently.
	End Rem
	Method tmoReinhard05Ex:TFreeImage(intensity:Double = 0, contrast:Double = 0, adaptation:Double = 1, colorCorrection:Double = 0)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_TmoReinhard05Ex(freeImagePtr, intensity, contrast, adaptation, colorCorrection))
	End Method

	Rem
	bbdoc: Converts a High Dynamic Range image to a 24-bit RGB image using a local operator that manipulate the gradient field of the luminance image by attenuating the magnitudes of large gradients.
	about: A new, low dynamic range image is then obtained by solving a Poisson equation on the modified gradient field.<br/>
	Upon entry, the color_saturation parameter, in the range [0.4, 0.6], controls color saturation in the resulting image.<br/>
	The attenuation parameter, in the range [0.8, 0.9], controls the amount of attenuation.<br/>
	NOTE : The algorithm works by solving as many Partial Differential Equations as there are pixels in the image, using
	a Poisson solver based on a multigrid algorithm. Thus, the algorithm may take many minutes (up to 5 or more) before to complete.
	End Rem
	Method tmoFattal02:TFreeImage(colorSaturation:Double = 0.5, attenuation:Double = 0.85)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_TmoFattal02(freeImagePtr, colorSaturation, attenuation))
	End Method
	
	Rem
	bbdoc: Retrieves a metadata attached To the image.
	returns: A TFreeImageTag object on success, or Null if the searched tag doesn't exist.
	about: Upon entry, @model is the metadata model to look for and @key is the metadata field name (unique inside a metadata model).
	End Rem
	Method getMetadata:TFreeImageTag(model:Int, key:String)
		Return TFreeImageTag._create(bmx_freeimage_GetMetadata(freeImagePtr, model, key))
	End Method
	
	Rem
	bbdoc: Returns the number of tags contained in the model metadata model attached to the image.
	End Rem
	Method getMetadataCount:Int(model:Int)
		Return bmx_freeimage_GetMetadataCount(freeImagePtr, model)
	End Method
	
	Rem
	bbdoc: Attaches a new FreeImage tag to a dib.
	returns: TRUE on success and returns FALSE otherwise. 
	about: Upon entry, @model is the metadata model used to store the tag, @key is the tag field name and @tag is the FreeImage tag to be 
	attached. 
	<p>
	If tag is NULL then the metadata is deleted.
	</p>
	<p>
	If both key and tag are NULL then the metadata model is deleted.
	</p>
	<p>
	The tag field name (or tag key) used by FreeImage to index a tag is given by the metadata model specification (e.g. EXIF
	specification or Adobe XMP specification).
	</p>
	End Rem
	Method setMetadata:Int(model:Int, key:String, tag:TFreeImageTag)
		If tag Then
			Return bmx_freeimage_SetMetadata(freeImagePtr, model, key, tag.tagPtr)
		Else
			Return bmx_freeimage_SetMetadata(freeImagePtr, model, key, Null)
		End If
	End Method

	Rem
	bbdoc: Returns a list enumerator for the metadata attached to this image.
	about: You can use this returned object in a For ... Eachin loop.
	End Rem
	Method metadata:TTagEnumerator(model:Int)
		Local metaEnumerator:TMetadataEnumerator = New TMetadataEnumerator.Create(Self, model)
		Local enum:TTagEnumerator = New TTagEnumerator
		enum._enumerator = metaEnumerator
		Return enum
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function JPEGCrop:Int(source:String, dest:String, _left:Int, _top:Int, _right:Int, _bottom:Int)
		Return bmx_freeimage_JPEGCrop(source, dest, _left, _top, _right, _bottom)
	End Function
	
	Rem
	bbdoc: Pre-multiplies the 32-bit image's red-, green- and blue channels with its alpha channel for to be used with the Windows GDI function AlphaBlend().
	about: The transformation changes the red-, green- and blue channels according to the following equation:
	<pre>
	channel(x, y) = channel(x, y) * alpha_channel(x, y) / 255
	</pre>
	The method returns True on success, False otherwise (e.g. when the bitdepth of the source dib cannot be handled).
	End Rem
	Method preMultiplyWithAlpha:Int()
		Local res:Int = bmx_freeimage_PreMultiplyWithAlpha(freeImagePtr)
		If res Then
			imageChanged = True
		End If
		Return res
	End Method

	Rem
	bbdoc: Enlarges or shrinks an image selectively per side and fills newly added areas with the specified background color.
	End Rem
	Method enlargeCanvas:TFreeImage(_left:Int, _top:Int, _right:Int, _bottom:Int, color:TRGBQuad, options:Int)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_enlargeCanvas(freeImagePtr, _left, _top, _right, _bottom, color.colorPtr, options))
	End Method
	
	Rem
	bbdoc: Returns TRUE when the image has a file background color, FALSE otherwise.
	End Rem
	Method hasBackgroundColor:Int()
		Return bmx_freeimage_HasBackgroundColor(freeImagePtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getBackgroundColor:TRGBQuad()
		Return TRGBQuad._create(bmx_freeimage_getBackgroundColor(freeImagePtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setBackgroundColor:Int(color:TRGBQuad)
		Return bmx_freeimage_setBackgroundColor(freeImagePtr, color.colorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getPixelColor:TRGBQuad(x:Int, y:Int)
		Return TRGBQuad._create(bmx_freeimage_getPixelColor(freeImagePtr, x, y))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setPixelColor(x:Int, y:Int, color:TRGBQuad)
		bmx_freeimage_setPixelColor(freeImagePtr, x, y, color.colorPtr)
	End Method
	
	Rem
	bbdoc: Returns the data type of a bitmap.
	about: One of the following :
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>FIT_UNKNOWN</td><td> unknown type </td></tr>
	<tr><td> FIT_BITMAP </td><td>standard image : 1-, 4-, 8-, 16-, 24-, 32-bit</td></tr>
	<tr><td> FIT_UINT16 </td><td> array of unsigned short : unsigned 16-bit </td></tr>
	<tr><td> FIT_INT16 </td><td> array of short : signed 16-bit </td></tr>
	<tr><td> FIT_UINT32 </td><td> array of unsigned long : unsigned 32-bit </td></tr>
	<tr><td> FIT_INT32 </td><td> array of long : signed 32-bit </td></tr>
	<tr><td> FIT_FLOAT </td><td> array of float : 32-bit IEEE floating point </td></tr>
	<tr><td> FIT_DOUBLE </td><td> array of double : 64-bit IEEE floating point </td></tr>
	<tr><td> FIT_COMPLEX </td><td> array of FICOMPLEX : 2 x 64-bit IEEE floating point </td></tr>
	<tr><td> FIT_RGB16 </td><td> 48-bit RGB image : 3 x 16-bit </td></tr>
	<tr><td> FIT_RGBA16 </td><td> 64-bit RGBA image : 4 x 16-bit </td></tr>
	<tr><td> FIT_RGBF </td><td> 96-bit RGB float image : 3 x 32-bit IEEE floating point </td></tr>
	<tr><td> FIT_RGBAF </td><td> 128-bit RGBA float image : 4 x 32-bit IEEE floating point </td></tr>
	</table>
	End Rem
	Method getImageType:Int()
		Return bmx_freeimage_getImageType(freeImagePtr)
	End Method
	
	Rem
	bbdoc: Returns False if the bitmap does not contain pixel data (i.e. if it contains only header and possibly some metadata).
	about: Header only bitmap can be loaded using the FIF_LOAD_NOPIXELS load flag.
	This load flag will tell the decoder to read header data and available metadata and skip pixel data decoding.
	The memory size of the dib is thus reduced to the size of its members, excluding the pixel buffer.
	Reading metadata only information is fast since no pixel decoding occurs.
	End Rem
	Method hasPixels:Int()
		Return bmx_freeimage_hasPixels(freeImagePtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TRGBQuad

	Field colorPtr:Byte Ptr
	
	Function _create:TRGBQuad(colorPtr:Byte Ptr)
		If colorPtr Then
			Local this:TRGBQuad = New TRGBQuad
			this.colorPtr = colorPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Create:TRGBQuad(r:Int, g:Int, b:Int, a:Int = 0)
		colorPtr = bmx_rgbquad_create(r, g, b, a)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Red:Int()
		Return bmx_rgbquad_red(colorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Green:Int()
		Return bmx_rgbquad_green(colorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Blue:Int()
		Return bmx_rgbquad_blue(colorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Alpha:Int()
		Return bmx_rgbquad_reserved(colorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetRGBA(r:Int, g:Int, b:Int, a:Int = 0)
		bmx_rgbquad_setrgba(colorPtr, r, g, b, a)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetRed(r:Int)
		bmx_rgbquad_setred(colorPtr, r)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetGreen(g:Int)
		bmx_rgbquad_setgreen(colorPtr, g)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBlue(b:Int)
		bmx_rgbquad_setblue(colorPtr, b)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAlpha(a:Int)
		bmx_rgbquad_setalpha(colorPtr, a)
	End Method
	
	Method Delete()
		If colorPtr Then
			bmx_rgbquad_free(colorPtr)
			colorPtr = Null
		End If
	End Method

End Type


' metadata "eachin" support
Type TMetadataEnumerator

	Field metadataPtr:Byte Ptr

	Method Create:TMetadataEnumerator(image:TFreeImage, model:Int)
		tag = New TFreeImageTag.Create()
		metadataPtr = bmx_freeimagemetadata_FindFirstMetadata(model, image.freeImagePtr, tag.tagPtr)
		If Not metadataPtr Then
			tag = Null
		End If
		Return Self
	End Method

	Method HasNext:Int()
		If tag Then
			Return True
		End If
	End Method
	
	Method NextObject:Object()
		Local nextTag:TFreeImageTag = tag
		
		tag = TFreeImageTag._create(bmx_freeimagemetadata_FindNextMetadata(metadataPtr))
		
		Return nextTag
	End Method
	
	Method Delete()
		If metadataPtr Then
			bmx_freeimagemetadata_free(metadataPtr)
			metadataPtr = Null
		End If
	End Method

	'***** PRIVATE *****
		
	Field tag:TFreeImageTag
End Type

Type TTagEnumerator
	Method ObjectEnumerator:TMetadataEnumerator()
		Return _enumerator
	End Method
	Field _enumerator:TMetadataEnumerator
End Type


Rem
bbdoc: Stores metadata information.
about: The notion of tag originates from the TIFF specification and because of its universality, it is widely used to store 
metadata information in a file.
<p>
FreeImage provides an enhanced version of the standard TIFF or Exif tag structure.
</p>
<p>
Given a metadata model (e.g. Exif, Exif GPS, IPTC/NAA), the tag key (or tag field name) is 
unique inside this data model. This uniqueness allows FreeImage to use this key to index the 
tag inside a hash table, in order to speed up tag access. Whenever you store a tag inside a 
metadata model, you thus need to provide a unique key with the tag to store.
</p>
<p>
A FreeImage tag may be used to store any kind of data (e.g. strings, integers, doubles, 
rational numbers, etc.). The complete list of data type supported by FreeImage is given in 
Table 12. For example, when the tag data type indicates a double and the tag count is 8, then 
the tag value is an array of 8 doubles. Its length should be 64 bytes (8 x sizeof(double)). If the 
tag data type indicates a rational and the length is 48 bytes, then there are (48 bytes / (2 x 4-bytes)) = 6 rational values in the tag. 
As for ASCII strings, the value of the count part of an ASCII tag entry includes the NULL.
</p>
End Rem
Type TFreeImageTag

	Field tagPtr:Byte Ptr
	
	Function _create:TFreeImageTag(tagPtr:Byte Ptr)
		If tagPtr Then
			Local this:TFreeImageTag = New TFreeImageTag
			this.tagPtr = tagptr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Creates a new FreeImage tag.
	about: Tag creation is only needed when you use the FreeImage::setMetadata method.
	End Rem
	Function CreateTag:TFreeImageTag()
		Return New TFreeImageTag.Create()
	End Function
	
	Rem
	bbdoc: Creates a new FreeImage tag.
	about: Tag creation is only needed when you use the FreeImage::setMetadata method.
	End Rem
	Method Create:TFreeImageTag()
		tagPtr = bmx_freeimagetag_create()
		Return Self
	End Method
	
	Rem
	bbdoc: Creates and returns a copy of this tag.
	End Rem
	Method clone:TFreeImageTag()
		Return TFreeImageTag._create(bmx_freeimagetag_clone(tagPtr))
	End Method
	
	Rem
	bbdoc: Returns the tag field name (unique inside a metadata model).
	End Rem
	Method getKey:String()
		Return bmx_freeimagetag_getkey(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the tag description if available, returns NULL otherwise.
	End Rem
	Method getDescription:String()
		Return bmx_freeimagetag_getdescription(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the tag ID if available, returns 0 otherwise.
	End Rem
	Method getID:Int()
		Return bmx_freeimagetag_getid(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the tag data type.
	about: One of :
<table align="center" width="90%">
<tr><th>Constant</th><th>Description</th></tr>
<tr><td>FIDT_NOTYPE</td><td>Placeholder (do not use this type) </td></tr>
<tr><td>FIDT_BYTE</td><td>8-bit unsigned integer </td></tr>
<tr><td>FIDT_ASCII</td><td>8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL (binary zero) </td></tr>
<tr><td>FIDT_SHORT</td><td>16-bit (2-byte) unsigned integer </td></tr>
<tr><td>FIDT_INT</td><td>32-bit (4-byte) unsigned integer </td></tr>
<tr><td>FIDT_RATIONAL</td><td>Two Ints: the first represents the numerator of a fraction; the second, the denominator </td></tr>
<tr><td>FIDT_SBYTE</td><td>An 8-bit signed (twos-complement) integer </td></tr>
<tr><td>FIDT_UNDEFINED</td><td>An 8-bit byte that may contain anything, depending on the definition of the field. </td></tr>
<tr><td>FIDT_SSHORT</td><td>A 16-bit (2-byte) signed (twos-complement) integer </td></tr>
<tr><td>FIDT_SINT</td><td>A 32-bit (4-byte) signed (twos-complement) integer </td></tr>
<tr><td>FIDT_SRATIONAL</td><td>Two SLONG’s: the first represents the numerator of a fraction, the second the denominator </td></tr>
<tr><td>FIDT_FLOAT</td><td>Single precision (4-byte) IEEE format </td></tr>
<tr><td>FIDT_DOUBLE</td><td>Double precision (8-byte) IEEE format </td></tr>
<tr><td>FIDT_IFD FIDT_IFD</td><td>data type is identical to INT, but is only used to store offsets </td></tr>
<tr><td>FIDT_PALETTE</td><td>32-bit (4-byte) RGBQUAD </td></tr>

</table>
	End Rem
	Method getType:Int()
		Return bmx_freeimagetag_gettype(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of components in the tag (in tag type units).
	about: For example, when the tag data type indicates a double (i.e. a FIDT_DOUBLE type) and the tag count is 8, then the tag 
	value is an array of 8 doubles. 
	End Rem
	Method getCount:Int()
		Return bmx_freeimagetag_getcount(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the length of the tag value in bytes.
	End Rem
	Method getLength:Int()
		Return bmx_freeimagetag_getlength(tagPtr)
	End Method
	
	Rem
	bbdoc: Returns the tag value. 
	about: It is up to you to interpret the returned pointer correctly, according to the results of #getType and #getCount.
	End Rem
	Method getValue:Byte Ptr()
		Return bmx_freeimagetag_getvalue(tagPtr)
	End Method
	
	Rem
	bbdoc: Sets the tag field name (always required, must be unique inside a metadata model).
	returns: TRUE if successful and returns FALSE otherwise. 
	End Rem
	Method setKey:Int(key:String)
		Return bmx_freeimagetag_setkey(tagPtr, key)
	End Method
	
	Rem
	bbdoc: Sets the (usually optional) tag description.
	returns: TRUE if successful and returns FALSE otherwise. 
	about: The tag description is never stored in a file. FreeImage maintains an internal table for 
	all known tags, together with their description when available. Whenever you read a 
	known tag, the library is able to give the tag description (provided that the tag is known 
	by the library) using #getDescription. However, provide a tag description when storing a tag. 
	End Rem
	Method setDescription:Int(description:String)
		Return bmx_freeimagetag_setdescription(tagPtr, description)
	End Method
	
	Rem
	bbdoc: Sets the (usually optional) tad ID.
	returns: TRUE if successful and returns FALSE otherwise.
	End Rem
	Method setID:Int(id:Int)
		Return bmx_freeimagetag_setid(tagPtr, id)
	End Method
	
	Rem
	bbdoc: Sets the tag data type (always required).
	returns: TRUE if successful and returns FALSE otherwise.
	End Rem
	Method setType:Int(tagType:Int)
		Return bmx_freeimagetag_settype(tagPtr, tagType)
	End Method
	
	Rem
	bbdoc: Sets the number of data in the tag (always required, expressed in tag type unit).
	returns: TRUE if successful and returns FALSE otherwise.
	End Rem
	Method setCount:Int(count:Int)
		Return bmx_freeimagetag_setcount(tagPtr, count)
	End Method
	
	Rem
	bbdoc: Sets the length of the tag value, in bytes (always required).
	returns: TRUE if successful and returns FALSE otherwise.
	End Rem
	Method setLength:Int(length:Int)
		Return bmx_freeimagetag_setlength(tagPtr, length)
	End Method
	
	Rem
	bbdoc: Sets the tag value (always required).
	returns: TRUE if successful and returns FALSE otherwise.
	about: This must be called after the tag data type, tag count and tag length have been filled. Otherwise, you will
	be unable to successfully call FreeImage::setMetadata. 
	End Rem
	Method setValue:Int(value:Byte Ptr)
		Return bmx_freeimagetag_setvalue(tagPtr, value)
	End Method
	
	Rem
	bbdoc: Converts the tag to a string that represents the interpreted tag value.
	about: The tag value is interpreted according to the metadata model specification. For example, consider 
	a tag extracted from the FIMD_EXIF_EXIF metadata model, whose ID is 0x9209 and whose 
	key is "Flash". Then if the tag value is 0x0005, the function will return "Strobe return light not detected". 
	Upon entry, model is the metadata model from which the tag was extracted, tag is the 
	FreeImage tag to interpret and Make is the camera model. This last parameter is currently not 
	used by the library but will be used in the future to interpret the camera maker notes (FIMD_EXIF_MAKERNOTE metadata model). 
	End Rem
	Method tagToString:String(model:Int, make:String = Null)
		Return bmx_freeimagetag_tagtostring(tagPtr, model, make)
	End Method
	
	Method Delete()
		If tagPtr Then
			bmx_freeimagetag_free(tagPtr)
			tagPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Returns the format type for the given filename
End Rem
Function GetFormatFromFile:Int(filename:String)
?win32
	Local file:Short Ptr = filename.ToWString()
	Local fif:Int = FreeImage_GetFIFFromFilenameU(file)
	MemFree file
	Return fif
?Not win32
	Local file:Byte Ptr = filename.ToCString()
	Local fif:Int = FreeImage_GetFIFFromFilename(file)
	MemFree file
	Return fif
?
End Function

Rem
bbdoc: Returns True if the plugin belonging to the given FREE_IMAGE_FORMAT can load a file using the FIF_LOAD_NOPIXELS load flag.
about: If True, a loader can load header only data and possibly some metadata.
End Rem
Function FormatSupportsNoPixels:Int(format:Int)
	Return FreeImage_FIFSupportsNoPixels(format)
End Function

Rem
bbdoc: Returns a string containing a standard copyright message you can show in your program. 
End Rem
Function GetCopyrightMessage:String()
	Local c:Byte Ptr = FreeImage_GetCopyrightMessage()
	Return String.FromCString(c)
End Function

Rem
bbdoc: Returns a string containing the current version of the library.
End Rem
Function GetFreeImageVersion:String()
	Local c:Byte Ptr = FreeImage_GetVersion()
	Return String.FromCString(c)
End Function

Rem
bbdoc: Returns a FREE_IMAGE_FORMAT identifier from the format string that was used to register the FIF.
End Rem
Function GetFormatFromName:Int(registeredName:String)
	Local name:Byte Ptr = registeredName.ToCString()
	Local fif:Int = FreeImage_GetFIFFromFormat(name)
	MemFree name
	Return fif
End Function

Rem
bbdoc: Returns a String of all loadable formats, in a file selector friendly style.
End Rem
Function GetLoadFormats:String()

	Local all:String = "All formats:"
	Local out:String

	Local fifcount:Int = FreeImage_GetFIFCount()
	For Local fif:Int = 0 Until fifcount
		If FreeImage_FIFSupportsReading(fif)
			Local ext:Byte Ptr = FreeImage_GetFIFExtensionList(fif)
			Local desc:Byte Ptr = FreeImage_GetFIFDescription(fif)
			Local tempext:String = String.FromCString(ext)
			If tempext.find(",")=>0
				tempext = tempext[..tempext.find(",")]
			EndIf
			all :+ String.FromCString(ext)+","
			out :+ String.FromCString(desc)+" (*."+tempext+"):"+String.FromCString(ext)+";"
		EndIf
	Next
	
	Return all+";"+out
		
End Function


Rem
bbdoc: Returns a String of all saveable formats, in a file selector friendly style.
End Rem
Function GetSaveFormats:String()

	Local all:String = "All formats:"
	Local out:String

	Local fifcount:Int = FreeImage_GetFIFCount()
	For Local fif:Int = 0 Until fifcount
		If FreeImage_FIFSupportsWriting(fif)
			Local ext:Byte Ptr = FreeImage_GetFIFExtensionList(fif)
			Local desc:Byte Ptr = FreeImage_GetFIFDescription(fif)
			Local tempext:String = String.FromCString(ext)
			If tempext.find(",")=>0
				tempext = tempext[..tempext.find(",")]
			EndIf
			all :+ String.FromCString(ext)+","
			out :+ String.FromCString(desc)+" (*."+tempext+"):"+String.FromCString(ext)+";"
		EndIf
	Next
	
	Return out
	
End Function



Type TPixmapLoaderFI Extends TPixmapLoader

	Global initialized:Int = False

	Method New()

		If Not initialized Then
		
			'?linux
			FreeImage_Initialise()
			'?macos
			'FreeImage_Initialise()
			'?
			
			initialized = True
		
			' deinit at app end.
			'?linux
			'atexit_ FreeImage_DeInitialise
			'?macos
			atexit_ FreeImage_DeInitialise
			'?
		End If
	
	End Method

	Method LoadPixmap:TPixmap( stream:TStream )

		Local freeImage:TFreeImage = TFreeImage.CreateFromStream(stream)

		If freeImage And freeimage.pixmap Then
			' copy the pixmap, since freeImage is going to be GC'd at some point
			Return CopyPixmap(freeImage.pixmap)
		End If

		Return Null
	End Method
	
End Type


New TPixmapLoaderFI
