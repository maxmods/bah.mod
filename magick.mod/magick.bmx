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

Rem
bbdoc: GraphicsMagick
about: The swiss army knife of image processing.
End Rem
Module BaH.Magick

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: GraphicsMagick - 2002-2008 GraphicsMagick Group"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CC_OPTS: -fexceptions -DLIBXML_STATIC"

Import BRL.Pixmap
Import BRL.Stream

Import "common.bmx"

' NOTES:
'
' Renamed coders/locale.c and map.c with prefix bmx_
' Renamed delegates/bzip2/compress.c with prefix bmx_
'
' Be careful not to overwrite magick_config.h !!
'


Global _magick_initialized:Int = False

' set the config path
Function _init_magick()
	putenv_("MAGICK_CONFIGURE_PATH=" + MagickConfigPath)
	_magick_initialized = True
End Function

Rem
bbdoc: The path to magick.mgk
about: Can be set at runtime before calling any Magick functions.
End Rem
Global MagickConfigPath:String = "config"

Rem
bbdoc: 
End Rem
Function LoadMagickImage:TMImage( url:Object )

	Local imgStream:TStream = TStream(url)
	
	If Not imgStream
		imgStream = ReadStream( url )
	End If
	
	If Not imgStream Then
		Return Null
	End If
	
	Local fi:TMImage = TMImage.CreateFromStream(imgStream)
	
	imgStream.close()
	
	Return fi

End Function

Type TMImage

	Field imagePtr:Byte Ptr
	
	Field pixmap:TPixmap

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
	bbdoc: The image pitch
	End Rem
	Field pitch:Int
	
	Field imageChanged:Int
	
	Method New()
		If Not _magick_initialized Then
			_init_magick()
		End If
	End Method

	Function _create:TMImage(imagePtr:Byte Ptr)
		If imagePtr Then
			Local this:TMImage = New TMImage
			this.imagePtr = imagePtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function CreateFromFile:TMImage(imageSpec:String)
		If Not _magick_initialized Then
			_init_magick()
		End If
		Return TMImage._create(bmx_magick_createfromspec(imageSpec))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function CreateFromBlob:TMImage(blob:TMBlob)
		Local this:TMImage = New TMImage
		this.imagePtr = bmx_magick_image_createfromblob(blob.blobPtr)
		this._updatePixmap()
		Return this
	End Function

	Rem
	bbdoc: 
	End Rem
	Function CreateFromStream:TMImage(stream:TStream)
		
		Local size:Int = stream.Size()
		Local buffer:Byte Ptr = MemAlloc(size)
		
		stream.ReadBytes(buffer, size)
		Local blob:TMBlob = TMBlob.CreateFromData(buffer, size)
		
		Local image:TMImage = TMImage.CreateFromBlob(blob)
		
		MemFree(buffer)
		
		Return image
	End Function

	Method _calc()
		bmx_magick_image_getsize(imagePtr, Varptr width, Varptr height)
		bitsPerPixel = bmx_magick_image_getdepth(imagePtr)
		pitch = width * (bitsPerPixel / 8)
	End Method
	
	Method _invertalpha()
		Local offset:Byte Ptr = pixmap.pixels + 3
		For Local i:Int = 0 Until width * height
			offset[0] = 255 - offset[0]
			offset:+ 4
		Next
	End Method
	
	Method _updatePixmap()
		' (re)calculate image size
		_calc()
		
		' do we need a brand new pixmap?
		If Not pixmap Or pixmap.width <> width Or pixmap.height <> height Then
			pixmap = CreatePixmap(width, height, PF_RGBA8888)
		End If
		
		' copy pixel data to pixmap
		bmx_magick_image_writedata(imagePtr, 0, 0, width, height, "RGBA", 0, pixmap.pixels)
		' invert the alpha - magick uses opposite, where 0 is opaque, 1 is transparent
		_invertalpha()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPixmap:TPixmap()
	
		If imageChanged Then
			_updatePixmap()
		End If
		
		Return pixmap
	End Method

	Rem
	bbdoc: Apply adaptive thresholding to the image.
	about: Adaptive thresholding is useful if the ideal threshold level is not known in advance, or if the illumination
	gradient is not constant across the image. Adaptive thresholding works by evaulating the mean (average) of a pixel
	region (size specified by @width and @height) and using the mean as the thresholding value. In order to remove residual
	noise from the background, the threshold may be adjusted by subtracting a constant @offset (default zero) from the mean
	to compute the threshold.
	End Rem
	Method adaptiveThreshold(width:Int, height:Int, offset:Int = 0)
		bmx_magick_image_adaptivethreshold(imagePtr, width, height, offset)
	End Method
	
	Rem
	bbdoc: Add noise to image with specified @noiseType.
	about: One of NOISE_UNIFORMNOISE, NOISE_GAUSSIANNOISE, NOISE_MULTIPLICATIVEGAUSSIANNOISE, NOISE_IMPULSENOISE,
	NOISE_LAPLACIANNOISE or NOISE_POISSONNOISE.
	End Rem
	Method addNoise(noiseType:Int)
		bmx_magick_image_addnoise(imagePtr, noiseType)
	End Method
	
	Rem
	bbdoc: Add noise to an image channel with the specified noise type.
	about: The @channel parameter specifies the channel to add noise to.  The @noiseType parameter
	specifies the type of noise. One of, NOISE_UNIFORMNOISE, NOISE_GAUSSIANNOISE, NOISE_MULTIPLICATIVEGAUSSIANNOISE, NOISE_IMPULSENOISE,
	NOISE_LAPLACIANNOISE or NOISE_POISSONNOISE.
	End Rem
	Method addNoiseChannel(channel:Int, noiseType:Int)
		bmx_magick_image_addnoisechannel(imagePtr, channel, noiseType)
	End Method
	
	'method bmx_magick_image_affinetransform(, const DrawableAffine &affine )
	'End Method
	
	Rem
	bbdoc: Annotate using specified @text, and placement @location.
	End Rem
	Method annotate(text:String, location:TMGeometry )
		bmx_magick_image_annotate(imagePtr, text, location.geometryPtr)
	End Method
	
	Rem
	bbdoc: Blur image.
	about: The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels.
	End Rem
	Method blur(radius:Double = 1.0, sigma:Double = 0.5)
		bmx_magick_image_blur(imagePtr, radius, sigma)
	End Method
	
	Rem
	bbdoc: Blur an image channel.
	about: The @channel parameter specifies the channel to blur. The @radius parameter specifies the radius
	of the Gaussian, in pixels, not counting the center pixel.  The @sigma parameter specifies the standard
	deviation of the Laplacian, in pixels.
	End Rem
	Method blurChannel(channel:Int, radius:Double = 0.0, sigma:Double = 1.0 )
		bmx_magick_image_blurchannel(imagePtr, channel, radius, sigma)
	End Method
	
	Rem
	bbdoc: Border image (add border to image). 
	about: The color of the border is specified by setting borderColor().
	End Rem
	Method border(geometry:TMGeometry)
		bmx_magick_image_border(imagePtr, geometry.geometryPtr)
	End Method
	
	Rem
	bbdoc: Extract channel from image.
	about: Use this option to extract a particular channel from  the image.
	MatteChannel for example, is useful for extracting the opacity values from an image.
	End Rem
	Method channel(channel:Int)
		bmx_magick_image_channel(imagePtr, channel)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method channelDepth(channel:Int, depth:Int)
		bmx_magick_image_channeldepth(imagePtr, channel, depth)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getChannelDepth:Int(channel:Int)
		Return bmx_magick_image_getchanneldepth(imagePtr, channel)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method charcoal(radius:Double, sigma:Double)
		bmx_magick_image_charcoal(imagePtr, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method chop(geometry:TMGeometry)
		bmx_magick_image_chop(imagePtr, geometry.geometryPtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method colorize(opacityRed:Int, opacityGreen:Int, opacityBlue:Int, penColor:TMColor)
		bmx_magick_image_colorize(imagePtr, opacityRed, opacityGreen, opacityBlue, penColor.colorPtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method colorizeColor(opacity:Int, penColor:TMColor)
		bmx_magick_image_colorizecolor(imagePtr, opacity, penColor.colorPtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method comment(comment:String)
		bmx_magick_image_comment(imagePtr, comment)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Flip()
		bmx_magick_image_flip(imagePtr)
		imageChanged = True
	End Method

	Rem
	bbdoc: 
	End Rem
	Method flop()
		bmx_magick_image_flop(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Oilpaint image (image looks like oil painting).
	End Rem
	Method oilPaint(radius:Double = 3.0)
		bmx_magick_image_oilpaint(imagePtr, radius)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method opacity(value:Int)
		bmx_magick_image_opacity(imagePtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method rotate(degrees:Double)
		bmx_magick_image_rotate(imagePtr, degrees)
	End Method
	
	
	Rem
	bbdoc: 
	End Rem
	Method WriteFile(imageSpec:String)
		bmx_magick_image_writefile(imagePtr, imageSpec)
	End Method

' attributes
	Method adjoin(flag:Int)
	End Method
	
	Method getAdjoin:Int()
	End Method
	
	Method AntiAlias(flag:Int)
	End Method
	
	Method getAntiAlias:Int()
	End Method
	
	Method animationDelay(Delay:Int)
	End Method
	
	Method getAnimationDelay:Int() 
	End Method
	
	Method animationIterations(iterations:Int)
	End Method
	
	Method getAnimationIterations()
	End Method
	
	Method attribute(name:String, value:String)
	End Method
	
	Method getAttribute:String(name:String)
	End Method
	
	Method backgroundColor(color:TMColor )
	End Method
	
	Method getBackgroundColor:TMColor()
	End Method
	
	Method backgroundTexture(backgroundTexture:String)
	End Method
	
	Method getBackgroundTexture:String()
	End Method
	
	Method getBaseColumns:Int()
	End Method
	
	Method getBaseFilename:String()
	End Method
	
	Method getBaseRows:Int()
	End Method
	
	Method borderColor(color:TMColor)
	End Method
	
	Method getBorderColor:TMColor()
	End Method

	Method getBoundingBox:TMGeometry()
	End Method
	
	Method boxColor(boxColor:TMColor)
	End Method
	
	Method getBoxColor:TMColor()
	End Method
	
	Function cacheThreshold(threshold:Int)
	End Function
	
	Method chromaBluePrimary(x:Double, y:Double)
	End Method
	
	Method getChromaBluePrimary(x:Double Var, y:Double Var)
	End Method
	
	Method chromaGreenPrimary(x:Double, y:Double)
	End Method
	
	Method getChromaGreenPrimary(x:Double Var, y:Double Var)
	End Method
	
	Method chromaRedPrimary(x:Double, y:Double)
	End Method
	
	Method getChromaRedPrimary(x:Double Var, y:Double Var)
	End Method
	
	Method chromaWhitePoint(x:Double, y:Double)
	End Method
	
	Method getChromaWhitePoint(x:Double Var, y:Double Var)
	End Method


	Method classType(class:Int)
	End Method
	
	Method getClassType:Int()
	End Method
	
	Method clipMask(clipMask:TMImage)
	End Method
	
	Method getClipMask:TMImage()
	End Method
	
	Method colorFuzz(fuzz:Double)
	End Method
	
	Method getColorFuzz:Double()
	End Method
	
	Method colorMap(index:Int, color:TMColor)
	End Method
	
	Method getColorMap:TMColor(index:Int)
	End Method
	
	Method colorMapSize(entries:Int)
	End Method
	
	Method getColorMapSize:Int()
	End Method
	
	Method colorSpace(colorSpace:Int)
	End Method
	
	Method getColorSpace:Int()
	End Method
	
	Method getColumns:Int()
	End Method
	
	Method getComment:String()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method compose(compose:Int)
		bmx_magick_image_compose(imagePtr, compose)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getComposite:Int()
		Return bmx_magick_image_getcomposite(imagePtr)
	End Method
	
	Method pixelColor(x:Int, y:Int, color:TMColor)
	End Method
	
	Method getPixelColor:TMColor(x:Int, y:Int)
	End Method
	
	Method quality(value:Int)
	End Method
	
	Method getQuality:Int()
	End Method
	
	Method quantizeColors(colors:Int)
	End Method
	
	Method quantizeColorSpace(colorSpace:Int)
	End Method
	
	Method quantizeDither(flag:Int)
	End Method
	
	Method quantizeTreeDepth(treeDepth:Int)
	End Method
	
	
	
	Method strokeColor(color:TMColor)
	End Method
	
	Method getStrokeColor:TMColor()
	End Method
	
	Method strokeDashOffset(offset:Double)
	End Method
	
	Method getStrokeDashOffset:Double()
	End Method
	
	'Method strokeDashArray()
	'End Method
	
	Method strokeLineCap(lineCap:Int)
	End Method
	
	Method getStrokeLineCap:Int()
	End Method
	
	Method strokeLineJoin(lineJoin:Int)
	End Method
	
	Method getStrokeLineJoin:Int()
	End Method
	
	Method strokeMiterLimit(miterLimit:Int)
	End Method
	
	Method getStrokeMiterLimit:Int()
	End Method
	
	Method strokeWidth(width:Double)
	End Method
	
	Method getStrokeWidth:Double()
	End Method
	
	Method strokePattern(pattern:TMImage)
	End Method
	
	Method getStrokePattern:TMImage()
	End Method
	
	
	
End Type

Type TMGeometry

	Field geometryPtr:Byte Ptr

End Type

Type TMColor

	Field colorPtr:Byte Ptr


End Type

Type TMDrawable

	Field drawablePtr:Byte Ptr
	
End Type

Type TMDrawableAffine Extends TMDrawable

	Method Create:TMDrawableAffine(sx:Double = 1.0, sy:Double = 1.0, rx:Double = 0, ry:Double = 0, tx:Double = 0, ty:Double = 0)
	End Method
	
		

End Type


Type TMBlob

	Field blobPtr:Byte Ptr

	Method New()
		If Not _magick_initialized Then
			_init_magick()
		End If
	End Method
	
	Function CreateFromData:TMBlob(data:Byte Ptr, size:Int)
		Local this:TMBlob = New TMBlob
		this.blobPtr = bmx_magick_blob_createfromdata(data, size)
		Return this
	End Function

End Type



Type TPixmapLoaderM Extends TPixmapLoader

	Method New()
	End Method

	Method LoadPixmap:TPixmap( stream:TStream )

		Local image:TMImage = TMImage.CreateFromStream(stream)

		If image And image.pixmap Then
			' copy the pixmap, since image is going to be GC'd at some point
			Return image.pixmap
		End If

		Return Null
	End Method
	
End Type


New TPixmapLoaderM
