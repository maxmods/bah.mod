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
' Added windows.h include to tif_win32.c
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

Rem
bbdoc: 
End Rem
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
	
	Field imageChanged:Int = True
	
	Method New()
		If Not _magick_initialized Then
			_init_magick()
		End If
	End Method

	Function _create:TMImage(imagePtr:Byte Ptr)
		If imagePtr Then
			Local this:TMImage = New TMImage
			this.imagePtr = imagePtr
			this._calc()
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Create:TMImage()
		If Not _magick_initialized Then
			_init_magick()
		End If
		Return TMImage._create(bmx_magick_create())
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
		this._calc()
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
	
	Rem
	bbdoc: 
	End Rem
	Function CreateFromPixmap:TMImage(pixmap:TPixmap)
	
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
	Method getPixmap:TPixmap()
	
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
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Add noise to image with specified @noiseType.
	about: One of NOISE_UNIFORMNOISE, NOISE_GAUSSIANNOISE, NOISE_MULTIPLICATIVEGAUSSIANNOISE, NOISE_IMPULSENOISE,
	NOISE_LAPLACIANNOISE or NOISE_POISSONNOISE.
	End Rem
	Method addNoise(noiseType:Int)
		bmx_magick_image_addnoise(imagePtr, noiseType)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Add noise to an image channel with the specified noise type.
	about: The @channel parameter specifies the channel to add noise to.  The @noiseType parameter
	specifies the type of noise. One of, NOISE_UNIFORMNOISE, NOISE_GAUSSIANNOISE, NOISE_MULTIPLICATIVEGAUSSIANNOISE, NOISE_IMPULSENOISE,
	NOISE_LAPLACIANNOISE or NOISE_POISSONNOISE.
	End Rem
	Method addNoiseChannel(channel:Int, noiseType:Int)
		bmx_magick_image_addnoisechannel(imagePtr, channel, noiseType)
		imageChanged = True
	End Method
	
	'method bmx_magick_image_affinetransform(, const DrawableAffine &affine )
	'End Method
	
	Rem
	bbdoc: Annotate using specified @text, and placement @location.
	End Rem
	Method annotate(text:String, location:Object )
		If TMGeometry(location) Then
			bmx_magick_image_annotate(imagePtr, text, TMGeometry(location).geometryPtr)
		Else If String(location) Then
			bmx_magick_image_annotatetxt(imagePtr, text, String(location))
		End If
	End Method
	
	Rem
	bbdoc: Blur image.
	about: The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels.
	End Rem
	Method blur(radius:Double = 1.0, sigma:Double = 0.5)
		bmx_magick_image_blur(imagePtr, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Blur an image channel.
	about: The @channel parameter specifies the channel to blur. The @radius parameter specifies the radius
	of the Gaussian, in pixels, not counting the center pixel.  The @sigma parameter specifies the standard
	deviation of the Laplacian, in pixels.
	End Rem
	Method blurChannel(channel:Int, radius:Double = 0.0, sigma:Double = 1.0 )
		bmx_magick_image_blurchannel(imagePtr, channel, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Border image (add border to image). 
	about: The color of the border is specified by setting borderColor().
	End Rem
	Method border(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_border(imagePtr, TMGeometry(geometry).geometryPtr)
		Else If String(geometry) Then
			bmx_magick_image_bordertxt(imagePtr, String(geometry))
		End If
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
	bbdoc: Channel modulus depth.
	about: The channel modulus depth represents the minimum number of bits required to support the channel
	without loss. Setting the channel's modulus depth modifies the channel (i.e. discards resolution) if the
	requested modulus depth is less than the current modulus depth, otherwise the channel is not altered.
	There is no attribute associated with the modulus depth so the current modulus depth is obtained by
	inspecting the pixels. As a result, the depth returned may be less than the most recently set channel
	depth. Subsequent image processing may result in increasing the channel depth.
	End Rem
	Method channelDepth(channel:Int, depth:Int)
		bmx_magick_image_channeldepth(imagePtr, channel, depth)
	End Method
	
	Rem
	bbdoc: Returns the channel modulus depth.
	End Rem
	Method getChannelDepth:Int(channel:Int)
		Return bmx_magick_image_getchanneldepth(imagePtr, channel)
	End Method
	
	Rem
	bbdoc: Charcoal effect image (looks like charcoal sketch).
	about: The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels.
	End Rem
	Method charcoal(radius:Double = 1.0, sigma:Double = 0.5)
		bmx_magick_image_charcoal(imagePtr, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Chop image (remove vertical or horizontal subregion of image)
	End Rem
	Method chop(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_chop(imagePtr, TMGeometry(geometry).geometryPtr)
			imageChanged = True
		Else If String(geometry) Then
			bmx_magick_image_choptxt(imagePtr, String(geometry))
			imageChanged = True
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method colorize(opacityRed:Int, opacityGreen:Int, opacityBlue:Int, penColor:Object)
		If TMColor(penColor) Then
			bmx_magick_image_colorize(imagePtr, opacityRed, opacityGreen, opacityBlue, TMColor(penColor).colorPtr)
			imageChanged = True
		ElseIf String(penColor)
			bmx_magick_image_colorizetxt(imagePtr, opacityRed, opacityGreen, opacityBlue, String(penColor))
			imageChanged = True
		EndIf
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method colorizeColor(opacity:Int, penColor:Object)
		If TMColor(penColor) Then
			bmx_magick_image_colorizecolor(imagePtr, opacity, TMColor(penColor).colorPtr)
			imageChanged = True
		ElseIf String(penColor)
			bmx_magick_image_colorizecolortxt(imagePtr, opacity, String(penColor))
			imageChanged = True
		End If
	End Method
	
	Rem
	bbdoc: Comment image (add comment string to image).
	about: By default, each image is commented with its file name. Use  this  method to  assign a specific comment
	to the image.  Optionally you can include the image filename, type, width, height, or other
	image  attributes by embedding special format characters.
	End Rem
	Method comment(comment:String)
		bmx_magick_image_comment(imagePtr, comment)
	End Method
	
	Rem
	bbdoc: Returns a copy of the image.
	End Rem
	Method copy:TMImage()
		Return TMImage._create(bmx_magick_image_copy(imagePtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method contrast(sharpen:Int)
	End Method
	
	'Method convolve(order:Int, kernel:Double[])
	'End Method
	
	Rem
	bbdoc: Crop image (subregion of original image)
	End Rem
	Method crop(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_crop(imagePtr, TMGeometry(geometry).geometryPtr)
		ElseIf String(geometry) Then
			bmx_magick_image_croptxt(imagePtr, String(geometry))
		End If
	End Method
	
	Rem
	bbdoc: Cycle image colormap.
	End Rem
	Method cycleColormap(amount:Int)
		bmx_magick_image_cyclecolormap(imagePtr, amount)
	End Method
	
	Rem
	bbdoc: Despeckle image (reduce speckle noise).
	End Rem
	Method despeckle()
		bmx_magick_image_despeckle(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method draw(drawable:TMDrawable)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method drawList(drawables:TList)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method edge(radius:Double = 0.0)
		bmx_magick_image_edge(imagePtr, radius)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Emboss image (hilight edges with 3D effect).
	about: The $radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels.
	End Rem
	Method emboss(radius:Double = 1.0, sigma:Double = 0.5)
		bmx_magick_image_emboss(imagePtr, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Enhance image (minimize noise).
	End Rem
	Method enhance()
		bmx_magick_image_enhance(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Equalize image (histogram equalization).
	End Rem
	Method equalize()
		bmx_magick_image_equalize(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Set all image pixels to the current background color.
	End Rem
	Method erase()
		bmx_magick_image_erase(imagePtr)
		imageChanged = True
	End Method
	
	
	Rem
	bbdoc: Flip image (reflect each scanline in the vertical direction).
	End Rem
	Method Flip()
		bmx_magick_image_flip(imagePtr)
		imageChanged = True
	End Method

	Rem
	bbdoc: Flop image (reflect each scanline in the horizontal direction).
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
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Set or attenuate the opacity channel in the image.
	about: If the image pixels are opaque then they are set to the specified opacity value, otherwise
	they are blended with the supplied opacity value.  The value of @opacity ranges from 0
	(completely opaque) to MaxRGB . The defines OpaqueOpacity and TransparentOpacity are available
	to specify completely opaque or completely transparent, respectively.
	End Rem
	Method opacity(value:Int)
		bmx_magick_image_opacity(imagePtr, value)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method read(imageSpec:String)
		bmx_magick_image_read(imagePtr, imageSpec)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method readGeom(geometry:Object, imageSpec:String)
		If TMGeometry(geometry) Then
			bmx_magick_image_readgeom(imagePtr, TMGeometry(geometry).geometryPtr, imageSpec)
			imageChanged = True
		Else If String(geometry) Then
			bmx_magick_image_readgeomtxt(imagePtr, String(geometry), imageSpec)
			imageChanged = True
		End If
	End Method
	
	
	Rem
	bbdoc: Rotate image counter-clockwise by specified number of degrees.
	End Rem
	Method rotate(degrees:Double)
		bmx_magick_image_rotate(imagePtr, degrees)
		imageChanged = True
	End Method
	
	
	Rem
	bbdoc: 
	End Rem
	Method WriteFile(imageSpec:String)
		bmx_magick_image_writefile(imagePtr, imageSpec)
	End Method

	Rem
	bbdoc: Zoom image to specified size.
	End Rem
	Method zoom(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_zoom(imagePtr, TMGeometry(geometry).geometryPtr)
			imageChanged = True
		ElseIf String(geometry) Then
			bmx_magick_image_zoomtxt(imagePtr, String(geometry))
			imageChanged = True
		End If
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
	
	Rem
	bbdoc: 
	End Rem
	Method backgroundColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_backgroundcolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_backgroundcolortxt(imagePtr, String(color))
		End If
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
	
	Rem
	bbdoc: 
	End Rem
	Method borderColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_bordercolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_bordercolortxt(imagePtr, String(color))
		End If
	End Method
	
	Method getBorderColor:TMColor()
	End Method

	Method getBoundingBox:TMGeometry()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method boxColor(boxColor:Object)
		If TMColor(boxColor) Then
		ElseIf String(boxColor)
		End If
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
	
	Rem
	bbdoc: 
	End Rem
	Method colorMap(index:Int, color:Object)
		If TMColor(color) Then
			bmx_magick_image_colormap(imagePtr, index, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_colormaptxt(imagePtr, index, String(color))
		End If
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
	
	Rem
	bbdoc: 
	End Rem
	Method pixelColor(x:Int, y:Int, color:Object)
		If TMColor(color) Then
			bmx_magick_image_pixelcolor(imagePtr, x, y, TMColor(color).colorPtr)
		ElseIf String(color) Then
			bmx_magick_image_pixelcolortxt(imagePtr, x, y, String(color))
		End If
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
	
	
	
	Method strokeColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_strokecolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color) Then
			bmx_magick_image_strokecolortxt(imagePtr, String(color))
		End If
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
	
	Rem
	bbdoc: Width and height of a raw image (an image which does not support width and height information).
	about: Size may also be used to affect the image size read from a multi-resolution format (e.g. Photo CD, JBIG, or JPEG.)
	End Rem
	Method size(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_size(imagePtr, TMGeometry(geometry).geometryPtr)
		Else If String(geometry) Then
			bmx_magick_image_sizetxt(imagePtr, String(geometry))
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TMGeometry

	Field geometryPtr:Byte Ptr

End Type

Rem
bbdoc: 
End Rem
Type TMColor

	Field colorPtr:Byte Ptr


End Type

Rem
bbdoc: 
End Rem
Type TMDrawable

	Field drawablePtr:Byte Ptr
	
End Type

Rem
bbdoc: 
End Rem
Type TMDrawableAffine Extends TMDrawable

	Method Create:TMDrawableAffine(sx:Double = 1.0, sy:Double = 1.0, rx:Double = 0, ry:Double = 0, tx:Double = 0, ty:Double = 0)
	End Method
	
End Type

Type TMDrawableAngle Extends TMDrawable

	Method Create:TMDrawableAngle(angle:Double)
	End Method
	
End Type

Type TMDrawableArc Extends TMDrawable

	Method Create:TMDrawableArc(startX:Double, startY:Double, endX:Double, endY:Double, startDegrees:Double, endDegrees:Double)
	End Method
	
End Type

Type TMDrawableBezier Extends TMDrawable

	Method Create:TMDrawableBezier()
	End Method
	
End Type

Type TMDrawableClipPath Extends TMDrawable

	Method Create:TMDrawableClipPath(id:String)
	End Method
	
End Type

Type TMDrawableCircle Extends TMDrawable

	Method Create:TMDrawableCircle(originX:Double, originY:Double, perimX:Double, perimY:Double)
	End Method
	
End Type

Rem
bbdoc: Color image according to paintMethod.
about: The point method recolors the target pixel.  The replace method recolors any pixel that matches
the color of the target pixel.  Floodfill recolors any pixel that matches the color of the target pixel
and is a neighbor,  whereas filltoborder recolors any neighbor pixel that is not the border color. Finally,
reset recolors all pixels.
End Rem
Type TMDrawableColor Extends TMDrawable

	Method Create:TMDrawableColor(x:Double, y:Double, paintMethod:Int)
	End Method
	
End Type

Type TMDrawableCompositeImage Extends TMDrawable

	Method Create:TMDrawableCompositeImage()
	End Method
	
End Type

Type TMDrawableDashArray Extends TMDrawable

	Method Create:TMDrawableDashArray()
	End Method
	
End Type

Type TMDrawableDashOffset Extends TMDrawable

	Method Create:TMDrawableDashOffset()
	End Method
	
End Type

Type TMDrawableEllipse Extends TMDrawable

	Method Create:TMDrawableEllipse(originX:Double, originY:Double, radiusX:Double, radiusY:Double, arcStart:Double, arcEnd:Double)
	End Method
	
End Type

Type TMDrawableFillColor Extends TMDrawable

	Method Create:TMDrawableFillColor(color:TMColor)
	End Method
	
End Type

Type TMDrawableFillRule Extends TMDrawable

	Method Create:TMDrawableFillRule(fillRule:Int)
	End Method
	
End Type

Type TMDrawableFillOpacity Extends TMDrawable

	Method Create:TMDrawableFillOpacity(opacity:Double)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TMDrawableFont Extends TMDrawable

	Rem
	bbdoc: 
	End Rem
	Method Create:TMDrawableFont(font:String)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CreateWithAttributes:TMDrawableFont(family:String, style:Int, weight:Int, stretch:Int)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TMDrawableGravity Extends TMDrawable

	Method Create:TMDrawableGravity(gravity:Int)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TMDrawableLine Extends TMDrawable

	Method Create:TMDrawableLine(startX:Double, startY:Double, endX:Double, endY:Double)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TMDrawableMatte Extends TMDrawable

	Method Create:TMDrawableMatte(x:Double, y:Double, paintMethod:Int)
	End Method
	
End Type

Type TMDrawableMiterLimit Extends TMDrawable

	Method Create:TMDrawableMiterLimit(miterLimit:Int)
	End Method
	
End Type

Type TMDrawablePath Extends TMDrawable

	Method Create:TMDrawablePath()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
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

Rem
bbdoc: Obtains information about the image formats supported by GraphicsMagick.
End Rem
Function coderInfoList:TList(isReadable:Int = TMCoderInfo.AnyMatch, isWritable:Int = TMCoderInfo.AnyMatch, isMultiFrame:Int = TMCoderInfo.AnyMatch)
	Local list:TList = New TList
	bmx_magick_coderinfolist(list, isReadable, isWritable, isMultiFrame)
	Return list
End Function

Rem
bbdoc: The TMCoderInfo type provides the means to provide information regarding GraphicsMagick support for an image format (designated by a magick string).
about: It may be used to provide support for a specific named format (provided as an argument to the
constructor), or as an element of a TList when format support is queried using the coderInfoList() function.
End Rem
Type TMCoderInfo

	Rem
	bbdoc: match any coder
	End Rem
	Const AnyMatch:Int = 0
	Rem
	bbdoc: match coder if true
	End Rem
	Const TrueMatch:Int = 1
	Rem
	bbdoc: match coder if false
	End Rem
	Const FalseMatch:Int = 2

	Rem
	bbdoc: Format name (e.g. "GIF").
	End Rem
	Field name:String
	Rem
	bbdoc: Format description (e.g. "CompuServe graphics interchange format").
	End Rem
	Field description:String
	Rem
	bbdoc: Format is readable.
	End Rem
	Field isReadable:Int
	Rem
	bbdoc: Format is writeable.
	End Rem
	Field isWritable:Int
	Rem
	bbdoc: Format supports multiple frames.
	End Rem
	Field isMultiFrame:Int
	
	Function _create:TMCoderInfo(name:String, description:String, isReadable:Int, isWritable:Int, isMultiFrame:Int)
		Local this:TMCoderInfo = New TMCoderInfo
		this.name = name
		this.description = description
		this.isReadable = isReadable
		this.isWritable = isWritable
		this.isMultiFrame = isMultiFrame
		Return this
	End Function
	
	Function _addToList(list:TList, coderInfo:TMCoderInfo)
		list.AddLast(coderInfo)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function info:TMCoderInfo(format:String)
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
