' Copyright (c) 2007 Bruce A Henderson
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

ModuleInfo "Version: 1.04"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: FreeImage - FreeImage Public License (FIPL)"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

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
ModuleInfo "CC_OPTS: -DPNG_iTXt_SUPPORTED"
ModuleInfo "CC_OPTS: -DHAVE_PROTOTYPES"
ModuleInfo "CC_OPTS: -DFREEIMAGE_LIB"
ModuleInfo "CC_OPTS: -DOPJ_STATIC"

Import BRL.Pixmap
Import BRL.Max2D
Import BRL.EndianStream
Import BRL.StandardIO

Import "common.bmx"

' NOTES :
'
' Changed PluginMNG.cpp (Enhancement :-p)
'    Added Validate() function to validate file header.
'
' Changed tif_open.c (Bug-fix!)
'    Changed 'tif->tif_mode & O_TRUNC' to read 'm & O_TRUNC'
'
' Changed BitmapAccess.cpp (Win32 build issue)
'    Added '&& !defined(__MINGW32__)'
'
' Changed FreeImage.h (Win32 build issue)
'    Several '__MINGW32__' checks.
'
' Changed openjpeg.h (Win32 build issue)
'    Added '|| defined(__MINGW32__)'
'
' Changed XTIFF.cpp (Win32 Bug-fix??)
'    Added 'value_count' test.
'
' Changed jmorecfg.h (Win32 build issue)
'    Added '__MINGW32__' check.
'

Rem
bbdoc: Loads an image into a #TFreeImage object.
returns: A #TFreeImage object or Null if not found or not a valid image format.
End Rem
Function LoadFreeImage:TFreeImage( url:Object )

	Local imgStream:TStream = ReadStream( url )
	If Not imgStream Then
		Return Null
	End If
	
	Local fi:TFreeImage = TFreeImage.CreateFromStream(imgStream)
	
	imgStream.close()
	
	Return fi

End Function

Rem
bbdoc: Loads a multi-page image into a #TMultiFreeImage object.
returns: A #TMultiFreeImage object or Null if not found or not a valid multi-page format.
End Rem
Function LoadMultiFreeImage:TMultiFreeImage( filename:String, readOnly:Int = True )
	
	Return TMultiFreeImage.CreateFromFile(filename, readOnly)

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
		Return image
	End If
	
	Return Null
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
	Function CreateFromFile:TMultiFreeImage(filename:String, readOnly:Int = True)
	
		hasError = False
	
		Local this:TMultiFreeImage = New TMultiFreeImage
		
		this.filename = filename
		this.freeImagePtr = bmx_multifreeimage_new(this, filename, readOnly)
		
		If Not this.isValidImage()
			Return Null
		End If
		
		If Not this.Load() Then
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
		this.freeImagePtr = bmx_multifreeimage_new(this, filename, False)
		
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
	
	Method Load:Int()

		' actually load the image
		bmx_multifreeimage_loadImage(freeImagePtr)
		
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
		
		Print "MultiFreeImage : " + String.fromCString(message)
	
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

		t.SetPixmap 0,pix
		unlockPage(img)
		
		For Local i:Int = 1 Until count
			img = lockPage(i)
			t.setPixmap(i, img.getPixmap())
			unlockPage(img)
		Next
		
		Return t
	End Method

	Method Delete()
		If currentImage Then
			unlockPage(currentImage)
			currentImage = Null
		End If
		
		bmx_multifreeimage_delete(freeImagePtr)
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

	
	Function CreateFromStream:TFreeImage(stream:TStream)
	
		hasError = False
	
		Local this:TFreeImage = New TFreeImage
		
		this.stream = stream
		
		this.freeImagePtr = bmx_freeimage_new(this)
		
		If Not this.isValidImage()
			Return Null
		End If
		
		If Not this.Load() Then
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
	
	' tidy up our memory
	Method Delete()
	
		If displayImagePtr And (displayImagePtr <> freeImagePtr) Then
			bmx_freeimage_delete(displayImagePtr)
		End If

		If freeImagePtr Then
			bmx_freeimage_delete(freeImagePtr)
		End If

	End Method
	
	
	Method isValidImage:Int()
	
		Local format:Int = bmx_FreeImage_GetFileTypeFromHandle(freeImagePtr)

		If format <> FIF_UNKNOWN Then
		
			Return True
		
		End If
		
		Return False
	
	End Method
	
	Method Load:Int()

		' actually load the image
		bmx_freeimage_loadImage(freeImagePtr)
		
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
		
		If displayImagePtr <> freeImagePtr Then
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

			' we don't support 4 bpp images in BlitzMax... convert to something else.
			Case 4
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

			Case 8
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
						displayImagePtr = freeImagePtr
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


		Print "FreeImage: " + t

	End Function
	
	
	Function read:Int(handler:TFreeImage, c:Byte Ptr, n:Int)

		Try
			Return handler.stream.readbytes(c, n)
			
		Catch e:TStreamReadException
			Return False
		End Try
	
	End Function
	
	Function write:Int(handler:TFreeImage, c:Byte Ptr, n:Int)

		Try

			Return handler.stream.writebytes(c, n)
			
		Catch e:TStreamWriteException
			Return False
		End Try
	
	End Function
	
	Function tell:Int(handler:TFreeImage)
		Return handler.stream.pos()
	End Function

	Function seek(handler:TFreeImage, pos:Int)
		handler.stream.seek(pos)
	End Function
	
	Function size:Int(handler:TFreeImage, pos:Int)
		Return handler.stream.size()
	End Function
	
	
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
	bbdoc: Creates a thumbnail so that the output fits inside a square of @maxPixelSize, keeping aspect ratio.
	about: Downsampling is done using a bilinear filter (see #rescale).
	End Rem
	Method makeThumbnail:TFreeImage(maxPixelSize:Int)
	
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_MakeThumbnail(freeImagePtr, maxPixelSize))
	
	End Method

	Rem
	bbdoc: Rotates the image by means of 3 shears.
	about: Rotation occurs around the center of the image area. Rotated image retains size and aspect
	of the source image (destination image size is usually bigger), so that this method should be used
	when rotating an image by 90, 180 or 270 degrees.
	End Rem
	Method rotateClassic:TFreeImage(angle:Double)
	
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_RotateClassic(freeImagePtr, angle))
	
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
	bbdoc: Performs a Gamma correction.
	about: A @gamma value of 1, leaves the image alone, less than 1 darkens it, and greater than 1 lightens it.
	End Rem
	Method adjustGamma(gamma:Double)
		bmx_freeimage_AdjustGamma(freeImagePtr, gamma)
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
	bbdoc: Quantizes a high-color 24-bit bitmap to an 8-bit palette color bitmap.
	returns: The converted image.
	about: @quantize specifies the color reduction algorithm to be used:
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>FIQ_WUQUANT</td><td>Xiaolin Wu color quantization algorithm</td></tr>
	<tr><td>FIQ_NNQUANT</td><td>NeuQuant neural-net quantization algorithm by Anthony Dekker</td></tr>
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
	</table>
	End Rem
	Method toneMapping:TFreeImage(algorithm:Int, param1:Double, param2:Double)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_ToneMapping(freeImagePtr, algorithm, param1, param2))
	End Method
	
	Rem
	bbdoc: Converts a High Dynamic Range image to a 24-bit RGB image using a global operator based on logarithmic compression of luminance values, imitating the human response to light.
	about: A bias power function is introduced to adaptively vary logarithmic bases, resulting in good preservation
	of details and contrast.<br>
	Upon entry, @gamma (where @gamma > 0) is a gamma correction that is applied after the tone mapping.
	A value of 1 means no correction. The default 2.2 value, used in the original author's paper, is
	recommended as a good starting value.<br>
	The @exposure parameter is an exposure scale factor allowing users to adjust the brightness
	of the output image to their displaying conditions. The default value (0) means that no correction
	is applied. Higher values will make the image lighter whereas lower values make the image darker.
	End Rem
	Method tmoDrago03:TFreeImage(gamma:Double = 2.2, exposure:Double = 0)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_TmoDrago03(freeImagePtr, gamma, exposure))
	End Method

	Rem
	bbdoc: Converts a High Dynamic Range image to a 24-bit RGB image using a global operator inspired by photoreceptor physiology of the human visual system.
	about: Upon entry, the @intensity parameter, in the range [-8, 8], controls the overall image intensity
	The default value 0 means no correction. Higher values will make the image lighter whereas lower values
	make the image darker.<br>
	The contrast parameter, in the range [0.3, 1.0], controls the overall image contrast. When using the default
	value (0), this parameter is calculated automatically.
	End Rem
	Method tmoReinhard05:TFreeImage(intensity:Double = 0, contrast:Double = 0)
		Return TFreeImage.CreateFromBitmap(bmx_freeimage_TmoReinhard05(freeImagePtr, intensity, contrast))
	End Method
	
	
	
End Type


Rem
bbdoc: Returns the format type for the given filename
End Rem
Function GetFormatFromFile:Int(filename:String)

	Local file:Short Ptr = filename.ToWString()
	Local fif:Int = FreeImage_GetFIFFromFilenameU(file)
	MemFree file
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
