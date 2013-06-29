' Copyright (c) 2008-2013 Bruce A Henderson
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
ModuleInfo "Copyright: GraphicsMagick - 2002-2012 GraphicsMagick Group"
ModuleInfo "Copyright: Wrapper - 2008-2013 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CC_OPTS: -fexceptions -DLIBXML_STATIC"
?Not win32
ModuleInfo "CC_OPTS: -std=c99"
?win32
ModuleInfo "CC_OPTS: -DWIN32 -D_MAX_PATH=260"
?

Import BRL.Pixmap
Import BRL.Stream

Import "drawable.bmx"

' NOTES:
'
' Renamed coders/locale.c, map.c and plasma.c with prefix bmx_
' Renamed delegates/bzip2/compress.c with prefix bmx_
' Renamed magick/analyze.c with prefix bmx_
'
' Be careful not to overwrite magick_config.h !!
'
' Added windows.h include to tif_win32.c
'


Global _magick_initialized:Int = False

' set the config path
Function _init_magick()
	If Not _magick_initialized Then
		putenv_("MAGICK_CONFIGURE_PATH=" + MagickConfigPath)
		bmx_magick_InitializeMagick(MagickConfigPath)
		_magick_initialized = True
	End If
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
bbdoc: TMImage is the primary object in Magick and represents a single image frame.
End Rem
Type TMImage

	Field imagePtr:Byte Ptr
	
	' this is PRIVATE.... you should use getPixmap() to retrieve it!!!!
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
		_init_magick()
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
		_init_magick()
		Return TMImage._create(bmx_magick_create())
	End Function

	Rem
	bbdoc: 
	End Rem
	Function CreateFromFile:TMImage(imageSpec:String)
		_init_magick()
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
		_init_magick()
		
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
	' TODO
	End Function

	'internal method
	Method _calc()
		bmx_magick_image_getsize(imagePtr, Varptr width, Varptr height)
		bitsPerPixel = bmx_magick_image_getdepth(imagePtr)
		pitch = width * (bitsPerPixel / 8)
	End Method
	
	'internal method
	Method _invertalpha()
		Local offset:Byte Ptr = pixmap.pixels + 3
		For Local i:Int = 0 Until width * height
			offset[0] = 255 - offset[0]
			offset:+ 4
		Next
	End Method
	
	'internal method
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
		'_invertalpha()
	End Method
	
	Rem
	bbdoc: Returns a pixmap representation of the image.
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
	' TODO
	'End Method
	
	Rem
	bbdoc: Annotates using specified @text, and placement @location.
	End Rem
	Method annotate(text:String, location:Object )
		If TMGeometry(location) Then
			bmx_magick_image_annotate(imagePtr, text, TMGeometry(location).geometryPtr)
			imageChanged = True
		Else If String(location) Then
			bmx_magick_image_annotatetxt(imagePtr, text, String(location))
			imageChanged = True
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
	bbdoc: Extracts channel from image.
	about: Use this option to extract a particular channel from  the image.
	MatteChannel for example, is useful for extracting the opacity values from an image.
	End Rem
	Method channel(channel:Int)
		bmx_magick_image_channel(imagePtr, channel)
	End Method
	
	Rem
	bbdoc: Charcoal effects image (looks like charcoal sketch).
	about: The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels.
	End Rem
	Method charcoal(radius:Double = 1.0, sigma:Double = 0.5)
		bmx_magick_image_charcoal(imagePtr, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Chops image (remove vertical or horizontal subregion of image)
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
	bbdoc: Colorize image with pen color, using specified percent opacity for red, green, and blue quantums.
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
	bbdoc: Colorize image with pen color, using specified percent opacity.
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
	bbdoc: Compare current image with another image.
	about: Sets meanErrorPerPixel , normalizedMaxError , and normalizedMeanError in the current image. False
	is returned if the images are identical. An ErrorOption exception is thrown if the reference image columns, rows,
	colorspace, or matte differ from the current image.
	End Rem
	Method compareImage:Int(image:TMImage)
	' TODO
	End Method
	
	Method composite(compositeImage:TMImage, xOffset:Int, yOffset:Int, operator:Int = COMPOSITE_INCOMPOSITEOP)
	'TODO
	End Method

	Method compositeGeometry(compositeImage:TMImage, geometry:Object, operator:Int = COMPOSITE_INCOMPOSITEOP)
	'TODO
	End Method

	Method compositeGravity(compositeImage:TMImage, gravity:Int, operator:Int = COMPOSITE_INCOMPOSITEOP)
	'TODO
	End Method
	
	Rem
	bbdoc: Returns a copy of the image.
	End Rem
	Method copy:TMImage()
		Return TMImage._create(bmx_magick_image_copy(imagePtr))
	End Method
	
	Rem
	bbdoc: Contrast image (enhance intensity differences in image)
	End Rem
	Method contrast(sharpen:Int)
		bmx_magick_image_contrast(imagePtr, sharpen)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method convolve(order:Int, kernel:Double[])
	'TODO
	End Method
	
	Rem
	bbdoc: Crop image (subregion of original image)
	End Rem
	Method crop(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_crop(imagePtr, TMGeometry(geometry).geometryPtr)
			imageChanged = True
		ElseIf String(geometry) Then
			bmx_magick_image_croptxt(imagePtr, String(geometry))
			imageChanged = True
		End If
	End Method
	
	Rem
	bbdoc: Cycle image colormap.
	End Rem
	Method cycleColormap(amount:Int)
		bmx_magick_image_cyclecolormap(imagePtr, amount)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Despeckle image (reduce speckle noise).
	End Rem
	Method despeckle()
		bmx_magick_image_despeckle(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Draw shape or text on image using a single drawable object.
	End Rem
	Method draw(drawable:TMDrawable)
		bmx_magick_image_draw(imagePtr, drawable.drawablePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method drawList(drawables:TList)
	'TODO
	End Method
	
	Rem
	bbdoc: Edges image (hilight edges in image).
	about: The radius is the radius of the pixel neighborhood.. Specify a radius of zero for automatic radius selection.
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
	bbdoc: Flood-fill color across pixels that match the color of the target pixel and are neighbors of the target pixel and optionally stopping at pixels matching specified border color.
	about: Uses current fuzz setting when determining color match
	End Rem
	Method floodFillColor(x:Int, y:Int, fillColor:Object, borderColor:Object = Null)
		If borderColor Then
			If TMColor(borderColor) Then
				If TMColor(fillColor) Then
					bmx_magick_image_floodfillcolorcc(imagePtr, x, y, TMColor(fillColor).colorPtr, TMColor(borderColor).colorPtr)
					imageChanged = True
				ElseIf String(fillColor) Then
					bmx_magick_image_floodfillcolorsc(imagePtr, x, y, String(fillColor), TMColor(borderColor).colorPtr)
					imageChanged = True
				End If
			ElseIf String(borderColor) Then
				If TMColor(fillColor) Then
					bmx_magick_image_floodfillcolorcs(imagePtr, x, y, TMColor(fillColor).colorPtr, String(borderColor))
					imageChanged = True
				ElseIf String(fillColor) Then
					bmx_magick_image_floodfillcolorss(imagePtr, x, y, String(fillColor), String(borderColor))
					imageChanged = True
				End If
			End If
		Else
			If TMColor(fillColor) Then
				bmx_magick_image_floodfillcolorcc(imagePtr, x, y, TMColor(fillColor).colorPtr, Null)
				imageChanged = True
			ElseIf String(fillColor) Then
				bmx_magick_image_floodfillcolorsc(imagePtr, x, y, String(fillColor), Null)
				imageChanged = True
			End If
		End If
	End Method
	
	Method floodFillColorGeom(point:Object, fillColor:Object, borderColor:Object = Null)
	'TODO
	End Method

	Method floodFillOpacity(x:Int, y:Int, opacity:Int, paintMethod:Int)
	'TODO
	End Method
	
	Method floodFillTexture(x:Int, y:Int, texture:TMImage, borderColor:Object = Null)
	'TODO
	End Method
	
	Method floodFillTextureGeom(point:Object, texture:TMImage, borderColor:Object = Null)
	'TODO
	End Method
	
	Rem
	bbdoc: Flop image (reflect each scanline in the horizontal direction).
	End Rem
	Method flop()
		bmx_magick_image_flop(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Draw a decorative frame around the image.
	End Rem
	Method frame(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_frame(imagePtr, TMGeometry(geometry).geometryPtr)
			imageChanged = True
		ElseIf String(geometry)
			bmx_magick_image_frametxt(imagePtr, String(geometry))
			imageChanged = True
		End If
	End Method
	
	Rem
	bbdoc: Draw a decorative frame with bevel around the image.
	End Rem
	Method frameBevel(width:Int, height:Int, innerBevel:Int = 6, outerBevel:Int = 6)
		bmx_magick_image_framebevel(imagePtr, width, height, innerBevel, outerBevel)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Gamma correct image (uniform red, green, and blue correction).
	End Rem
	Method Gamma(g:Double)
		bmx_magick_image_gamma(imagePtr, g)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Gamma correct red, green, and blue channels of image.
	End Rem
	Method gammaRGB(r:Double, g:Double, b:Double)
		bmx_magick_image_gammargb(imagePtr, r, g, b)
		imageChanged = True
	End Method

	Rem
	bbdoc: Gaussian blur image.
	about: The number of neighbor pixels to be included in the convolution mask is specified by @width.
	For example, a width of one gives a (standard) 3x3 convolution mask. The standard deviation of the gaussian bell curve
	is specified by @sigma.
	End Rem
	Method gaussianBlur(width:Double, sigma:Double)
		bmx_magick_image_gaussianblur(imagePtr, width, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Gaussian blur an image channel.
	about: The @channel parameter specifies the channel to blur. The number of neighbor pixels to be included in the
	convolution mask is specified by @width.  For example, a width of one gives a (standard) 3x3 convolution mask. The
	standard deviation of the gaussian bell curve is specified by @sigma.
	End Rem
	Method gaussianBlurChannel(channel:Int, width:Double = 0.0, sigma:Double = 1.0)
		bmx_magick_image_gaussianblurchannel(imagePtr, channel, width, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Implodes image (special effect).
	End Rem
	Method implode(factor:Double)
		bmx_magick_image_implode(imagePtr, factor)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Assigns a label to an image.
	about: Use this option to  assign  a  specific label to the image. Optionally you can include the image filename, type,
	width, height, or scene number in the label by embedding  special format characters. If the first character of
	string is @, the image label is read from a file titled by the remaining characters in the string. When converting to
	Postscript, use this  option to specify a header string to print above the image.
	End Rem
	Method label(text:String)
		bmx_magick_image_label(imagePtr, text)
	End Method
	
	Rem
	bbdoc: Levels image.
	about: Adjusts the levels of the image by scaling the colors falling between specified white and black points to the full
	available quantum range. The parameters provided represent the black, mid (gamma), and white points.  The black point
	specifies the darkest color in the image. Colors darker than the black point are set to zero. Mid point (gamma) specifies
	a gamma correction to apply to the image. White point specifies the lightest color in the image.  Colors brighter than
	the white point are set to the maximum quantum value. The black and white point have the valid range 0 to MaxRGB while
	mid (gamma) has a useful range of 0 to ten.
	End Rem
	Method level(blackPoint:Double, whitePoint:Double, midPoint:Double = 1.0)
		bmx_magick_image_level(imagePtr, blackPoint, whitePoint, midPoint)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Levels image channel.
	about: Adjust the levels of the image channel by scaling the values falling between specified white and black points
	to the full available quantum range. The parameters provided represent the black, mid (gamma), and white points. The
	black point specifies the darkest color in the image. Colors darker than the black point are set to zero. Mid point
	(gamma) specifies a gamma correction to apply to the image. White point specifies the lightest color in the image.
	Colors brighter than the white point are set to the maximum quantum value. The black and white point have the valid
	range 0 to MaxRGB while mid (gamma) has a useful range of 0 to ten.
	End Rem
	Method levelChannel(channel:Int, blackPoint:Double, whitePoint:Double, midPoint:Double = 1.0)
		bmx_magick_image_levelchannel(imagePtr, channel, blackPoint, whitePoint, midPoint)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Magnifies image by integral size.
	End Rem
	Method magnify()
		bmx_magick_image_magnify(imagePtr)
		imageChanged = True
	End Method
	
	Method map(mapImage:TMImage, dither:Int = False)
	'TODO
	End Method
	
	Method matteFloodfill(target:Object, opacity:Int, x:Int, y:Int, paintMethod:Int)
	'TODO
	End Method
	
	Rem
	bbdoc: Filters image by replacing each pixel component with the median color in a circular neighborhood.
	End Rem
	Method medianFilter(radius:Double = 0.0)
		bmx_magick_image_medianfilter(imagePtr, radius)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Reduces image by integral size.
	End Rem
	Method minify()
		bmx_magick_image_minify(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Prepares to update image.
	about: Ensures that there is only one reference to the underlying image so that the underlying image may be
	safely modified without effecting previous generations of the image. Copies the underlying image to a new image
	if necessary.
	End Rem
	Method modifyImage()
		bmx_magick_image_modifyImage(imagePtr)
	End Method
	
	Rem
	bbdoc: Modulates percent hue, saturation, and brightness of an image.
	about: Modulation of saturation and brightness is as a ratio of the current value (1.0 for no change). Modulation of
	hue is an absolute rotation of -180 degrees to +180 degrees from the current position corresponding to an argument
	range of 0 to 2.0 (1.0 for no change).
	End Rem
	Method modulate(brightness:Double, saturation:Double, hue:Double)
		bmx_magick_image_modulate(imagePtr, brightness, saturation, hue)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Motion blurs image with specified blur factor.
	The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels. The @angle parameter specifies
	the angle the object appears to be comming from (zero degrees is from the right).
	End Rem
	Method motionBlur(radius:Double, sigma:Double, angle:Double)
		bmx_magick_image_motionblur(imagePtr, radius, sigma, angle)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Negates colors in image.
	about: Replaces every pixel with its complementary color (white becomes black, yellow becomes blue, etc.).  Set
	grayscale to only negate grayscale values in image.
	End Rem
	Method negate(grayscale:Int = False)
		bmx_magick_image_negate(imagePtr, grayscale)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Normalizes image (increase contrast by normalizing the pixel values to span the full range of color values).
	End Rem
	Method normalize()
		bmx_magick_image_normalize(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Oilpaints image (image looks like oil painting).
	End Rem
	Method oilPaint(radius:Double = 3.0)
		bmx_magick_image_oilpaint(imagePtr, radius)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Sets or attenuates the opacity channel in the image.
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
	bbdoc: Changes color of specified opaque pixel to specified pen color
	End Rem
	Method opaque(opaqueColor:Object, penColor:Object)
		If TMColor(opaqueColor) Then
			If TMColor(penColor) Then
				bmx_magick_image_opaquecc(imagePtr, TMColor(opaqueColor).colorPtr, TMColor(penColor).colorPtr)
			ElseIf String(penColor) Then
				bmx_magick_image_opaquecs(imagePtr, TMColor(opaqueColor).colorPtr, String(penColor))
			End If
		ElseIf String(opaqueColor)
			If TMColor(penColor) Then
				bmx_magick_image_opaquesc(imagePtr, String(opaqueColor), TMColor(penColor).colorPtr)
			ElseIf String(penColor) Then
				bmx_magick_image_opaquess(imagePtr, String(opaqueColor), String(penColor))
			End If
		End If
	End Method
	
	Rem
	bbdoc: Ping is similar to read except only enough of the image is read to determine the image columns, rows, and filesize.
	about: The columns , rows , and fileSize attributes are valid after invoking ping.  The image data is not valid after
	calling ping.
	End Rem
	Method ping(imageSpec:String)
		bmx_magick_image_ping(imagePtr, imageSpec)
	End Method
	
	Rem
	bbdoc: Quantizes image (reduce number of colors).
	about: Set @measureError to true in order to calculate error attributes.
	End Rem
	Method quantize(measureError:Int = False)
		bmx_magick_image_quantize(imagePtr, measureError)
	End Method
	
	Method raise(geometry:Object)
	'TODO
	End Method
	
	Rem
	bbdoc: Reads single image frame into current object.
	about: Use #ping instead if you want to obtain the basic attributes of the image without reading the whole file.
	End Rem
	Method read(imageSpec:String)
		bmx_magick_image_read(imagePtr, imageSpec)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Reads single image frame of specified size into current object.
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

	Method readBlob(blob:TMBlob)
	' TODO
	End Method
	
	Method readBlobGeom(geometry:Object, blob:TMBlob)
	' TODO
	End Method
	
	Method readPixels(pixels:Byte Ptr, width:Int, height:Int, map:String, storageType:Int = STORAGE_CHARPIXEL)
	' TODO
	End Method
	
	Rem
	bbdoc: Reduces noise in image using a noise peak elimination filter.
	End Rem
	Method reduceNoise(order:Double = 3.0)
		bmx_magick_image_reducenoise(imagePtr, order)
		imageChanged = True
	End Method
	
	Method randomThreshold(thresholds:Object)
	'TODO
	End Method
	
	Method randomThresholdChannel(thresholds:Object, channel:Int)
	'TODO
	End Method
	
	Rem
	bbdoc: Rolls image (rolls image vertically and horizontally by specified number of columnms and rows).
	End Rem
	Method roll(columns:Int, rows:Int)
		bmx_magick_image_roll(imagePtr, columns, rows)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Rotates image counter-clockwise by specified number of degrees.
	End Rem
	Method rotate(degrees:Double)
		bmx_magick_image_rotate(imagePtr, degrees)
		imageChanged = True
	End Method
	
	Method sample(geometry:Object)
	'TODO
	End Method
	
	Method scale(geometry:Object)
	'TODO
	End Method
	
	Method segment(clusterThreshold:Double = 1.0, smoothingThreshold:Double = 1.5)
	'TODO
	End Method
	
	Rem
	bbdoc: Shades image using distant light source.
	about: Specify @azimuth and @elevation as the  position  of  the light source. By default, the shading results as a
	grayscale image. Set @colorShading to true to shade the red, green, and blue components of the image.
	End Rem
	Method shade(azimuth:Double = 30, elevation:Double = 30, colorShading:Int = False)
		bmx_magick_image_shade(imagePtr, azimuth, elevation, colorShading)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Sharpens pixels in image.
	about: The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Laplacian, in pixels.
	End Rem
	Method sharpen(radius:Double = 1.0, sigma:Double = 0.5)
		bmx_magick_image_sharpen(imagePtr, radius, sigma)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Sharpens pixel quantums in an image channel
	about: The @channel parameter specifies the channel to sharpen. The @radius parameter specifies the radius of the
	Gaussian, in pixels, not counting the center pixel. The @sigma parameter specifies the standard deviation of the
	Laplacian, in pixels.
	End Rem
	Method sharpenChannel(channel:Int, radius:Double = 0.0, sigma:Double = 1.0)
		bmx_magick_image_sharpenchannel(imagePtr, channel, radius, sigma)
		imageChanged = True
	End Method
	
	Method shave(geometry:Object)
	'TODO
	End Method
	
	Rem
	bbdoc: Shears image (creates parallelogram by sliding image by X or Y axis). 
	about: Shearing slides one edge of an image along the X  or  Y axis,  creating  a parallelogram.  An
	X direction shear slides an edge along the X axis, while  a  Y  direction shear  slides  an edge along the Y axis.
	The amount of the shear is controlled by a shear angle.  For X direction  shears,  x  degrees is measured relative to
	the Y axis, and similarly, for Y direction shears  y  degrees is measured relative to the X axis. Empty triangles
	left over from shearing the  image  are filled  with  the  color  defined as borderColor. 
	End Rem
	Method shear(xShearAngle:Double, yShearAngle:Double)
		bmx_magick_image_shear(imagePtr, xShearAngle, yShearAngle)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Solarizes image (similar to effect seen when exposing a photographic film to light during the development process).
	End Rem
	Method solarize(factor:Double = 50.0)
		bmx_magick_image_solarize(imagePtr, factor)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Spreads pixels randomly within image by specified amount.
	End Rem
	Method spread(amount:Int = 3)
		bmx_magick_image_spread(imagePtr, amount)
		imageChanged = True
	End Method
	
	Method stegano(watermark:TMImage)
	'TODO
	End Method
	
	Method stereo(rightImage:TMImage)
	'TODO
	End Method
	
	Rem
	bbdoc: Swirls image (image pixels are rotated by degrees).
	End Rem
	Method swirl(degrees:Double)
		bmx_magick_image_swirl(imagePtr, degrees)
		imageChanged = True
	End Method
	
	Method texture(txt:TMImage)
	'TODO
	End Method
	
	Rem
	bbdoc: Threshold image
	End Rem
	Method threshold(value:Double)
		bmx_magick_image_threshold(imagePtr, value)
		imageChanged = True
	End Method
	
	Method transform(imageGeometry:Object, cropGeometry:Object = Null)
	'TODO
	End Method
	
	Rem
	bbdoc: Adds matte channel to image, setting pixels matching @color to transparent
	End Rem
	Method transparent(color:Object)
		If TMColor(color) Then
			bmx_magick_image_transparent(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_transparenttxt(imagePtr, String(color))
		End If
	End Method
	
	Rem
	bbdoc: Trims edges that are the background color from the image.
	End Rem
	Method Trim()
		bmx_magick_image_trim(imagePtr)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Sharpens the image using the unsharp mask algorithm.
	about: The @radius parameter specifies the radius of the Gaussian, in pixels, not counting the center pixel.
	The @sigma parameter specifies the standard deviation of the Gaussian, in pixels. The @amount parameter specifies
	the percentage of the difference between the original and the blur image that is added back into the original. The
	@threshold parameter specifies the threshold in pixels needed to apply the diffence amount.
	End Rem
	Method unsharpmask(radius:Double, sigma:Double, amount:Double, threshold:Double)
		bmx_magick_image_unsharpmask(imagePtr, radius, sigma, amount, threshold)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Sharpens an image channel using the unsharp mask algorithm.
	about: The @channel parameter specifies the channel to sharpen. The @radius parameter specifies the radius
	of the Gaussian, in pixels, not counting the center pixel. The @sigma parameter specifies the standard deviation
	of the Gaussian, in pixels. The @amount parameter specifies the percentage of the difference between the original
	and the blur image that is added back into the original. The @threshold parameter specifies the threshold in pixels
	needed to apply the diffence amount.
	End Rem
	Method unsharpmaskChannel(channel:Int, radius:Double, sigma:Double, amount:Double, threshold:Double)
		bmx_magick_image_unsharpmaskchannel(imagePtr, channel, radius, sigma, amount, threshold)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Alters an image along a sine wave.
	End Rem
	Method wave(amplitude:Double = 25.0, wavelength:Double = 150.0)
		bmx_magick_image_wave(imagePtr, amplitude, wavelength)
		imageChanged = True
	End Method
	
	Rem
	bbdoc: Writes single image frame to a file.
	End Rem
	Method WriteFile(imageSpec:String)
		bmx_magick_image_writefile(imagePtr, imageSpec)
	End Method

	Rem
	bbdoc: Zooms image to specified size.
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
	Rem
	bbdoc: Sets whether to join images into a single multi-image file.
	End Rem
	Method adjoin(flag:Int)
		bmx_magick_image_adjoin(imagePtr, flag)
	End Method
	
	Rem
	bbdoc: Returns whether to join images into a single multi-image file.
	End Rem
	Method getAdjoin:Int()
		Return bmx_magick_image_getadjoin(imagePtr)
	End Method
	
	Rem
	bbdoc: Controls antialiasing of rendered Postscript and Postscript or TrueType fonts.
	about: Enabled by default.
	End Rem
	Method AntiAlias(flag:Int)
		bmx_magick_image_antialias(imagePtr, flag)
	End Method
	
	Rem
	bbdoc: Returns the setting of antialiasing of rendered Postscript and Postscript or TrueType fonts.
	End Rem
	Method getAntiAlias:Int()
		Return bmx_magick_image_getantialias(imagePtr)
	End Method
	
	Rem
	bbdoc: Sets the time in 1/100ths of a second (0 to 65535) which must expire before displaying the next image in an animated sequence.
	about: This option is useful for regulating the animation of a sequence  of GIF images.
	End Rem
	Method animationDelay(_delay:Int)
		bmx_magick_image_animationdelay(imagePtr, _delay)
	End Method
	
	Rem
	bbdoc: Returns the time in 1/100ths of a second (0 to 65535) which must expire before displaying the next image in an animated sequence.
	about: This option is useful for regulating the animation of a sequence of GIF images.
	End Rem
	Method getAnimationDelay:Int() 
		Return bmx_magick_image_getanimationdelay(imagePtr)
	End Method
	
	Rem
	bbdoc: Sets the number of iterations to loop an animation (e.g. Netscape loop extension) for.
	End Rem
	Method animationIterations(iterations:Int)
		bmx_magick_image_animationiterations(imagePtr, iterations)
	End Method
	
	Rem
	bbdoc: Returns the number of iterations to loop an animation (e.g. Netscape loop extension) for.
	End Rem
	Method getAnimationIterations:Int()
		Return bmx_magick_image_getanimationiterations(imagePtr)
	End Method
	
	Rem
	bbdoc: Sets an arbitrary named image attribute.
	about: Any number of named attributes may be attached to the image.
	<p>
	For example, the image comment is a named image attribute with the name "comment". EXIF tags are
	attached to the image as named attributes. Use the syntax "EXIF:<tag>" to request an EXIF tag similar
	to "EXIF:DateTime".
	</p>
	End Rem
	Method attribute(name:String, value:String)
		bmx_magick_image_attribute(imagePtr, name, value)
	End Method
	
	Rem
	bbdoc: Returns an arbitrary named image attribute
	about: Any number of named attributes may be attached to the image.
	<p>
	For example, the image comment is a named image attribute with the name "comment". EXIF tags are
	attached to the image as named attributes. Use the syntax "EXIF:<tag>" to request an EXIF tag similar
	to "EXIF:DateTime".
	</p>
	End Rem
	Method getAttribute:String(name:String)
		Return bmx_magick_image_getattribute(imagePtr, name)
	End Method
	
	Rem
	bbdoc: Sets the image background color.
	End Rem
	Method backgroundColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_backgroundcolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_backgroundcolortxt(imagePtr, String(color))
		End If
	End Method
	
	Method getBackgroundColor:TMColor()
		' TODO
	End Method
	
	Rem
	bbdoc: Sets the image file name to use as the background texture.
	about: Does not modify image pixels.
	End Rem
	Method backgroundTexture(backgroundTexture:String)
		bmx_magick_image_backgroundtexture(imagePtr, backgroundTexture)
	End Method
	
	Rem
	bbdoc: Returns the image file name to use as the background texture.
	End Rem
	Method getBackgroundTexture:String()
		Return bmx_magick_image_getbackgroundtexture(imagePtr)
	End Method
	
	Rem
	bbdoc: Gets the base image width (before transformations).
	End Rem
	Method getBaseColumns:Int()
		Return bmx_magick_image_getbasecolumns(imagePtr)
	End Method
	
	Rem
	bbdoc: Gets the base image filename (before transformations).
	End Rem
	Method getBaseFilename:String()
		Return bmx_magick_image_getbasefilename(imagePtr)
	End Method
	
	Rem
	bbdoc: Gets the base image height (before transformations).
	End Rem
	Method getBaseRows:Int()
		Return bmx_magick_image_getbaserows(imagePtr)
	End Method
	
	Rem
	bbdoc: Image border color.
	End Rem
	Method borderColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_bordercolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_bordercolortxt(imagePtr, String(color))
		End If
	End Method
	
	Method getBorderColor:TMColor()
	' TODO
	End Method

	Rem
	bbdoc: Returns the smallest bounding box enclosing non-border pixels.
	about: The current fuzz value is used when discriminating between pixels. This is the crop bounding box used by crop(Geometry(0,0))
	End Rem
	Method getBoundingBox:TMGeometry()
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method boxColor(boxColor:Object)
		If TMColor(boxColor) Then
	' TODO
		ElseIf String(boxColor)
	' TODO
		End If
	End Method
	
	Method getBoxColor:TMColor()
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function cacheThreshold(threshold:Int)
		bmx_magick_image_cachethreshold(threshold)
	End Function
	
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
	bbdoc: Chromaticity blue primary point (e.g. x=0.15, y=0.06)
	End Rem
	Method chromaBluePrimary(x:Double, y:Double)
		bmx_magick_image_chromablueprimary(imagePtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the chromaticity blue primary point.
	End Rem
	Method getChromaBluePrimary(x:Double Var, y:Double Var)
		bmx_magick_image_getchromablueprimary(imagePtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: Chromaticity green primary point (e.g. x=0.3, y=0.6)
	End Rem
	Method chromaGreenPrimary(x:Double, y:Double)
		bmx_magick_image_chromagreenprimary(imagePtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the chromaticity green primary point.
	End Rem
	Method getChromaGreenPrimary(x:Double Var, y:Double Var)
		bmx_magick_image_getchromagreenprimary(imagePtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: Chromaticity red primary point (e.g. x=0.64, y=0.33)
	End Rem
	Method chromaRedPrimary(x:Double, y:Double)
		bmx_magick_image_chromaredprimary(imagePtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the chromaticity red primary point.
	End Rem
	Method getChromaRedPrimary(x:Double Var, y:Double Var)
		bmx_magick_image_getchromaredprimary(imagePtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: Chromaticity white point (e.g. x=0.3127, y=0.329)
	End Rem
	Method chromaWhitePoint(x:Double, y:Double)
		bmx_magick_image_chromawhitepoint(imagePtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns the chromaticity white point.
	End Rem
	Method getChromaWhitePoint(x:Double Var, y:Double Var)
		bmx_magick_image_getchromawhitepoint(imagePtr, Varptr x, Varptr y)
	End Method

	Rem
	bbdoc: Image class (CLASS_DIRECTCLASS or CLASS_PSEUDOCLASS).
	about: NOTE: setting a DirectClass image to PseudoClass will result in the loss of color information
	if the number of colors in the image is greater than the maximum palette size
	(either 256 or 65536 entries depending on the value of QuantumDepth when ImageMagick was built)
	End Rem
	Method classType(class:Int)
		bmx_magick_image_classtype(imagePtr, class)
	End Method
	
	Method getClassType:Int()
	' TODO
	End Method
	
	Method clipMask(clipMask:TMImage)
	' TODO
	End Method
	
	Method getClipMask:TMImage()
	' TODO
	End Method
	
	Rem
	bbdoc: Colors within this distance are considered equal.
	about: A number of algorithms search for a target color. By default the color must be exact.
	Use this option to match colors that are close to the target color in RGB space
	End Rem
	Method colorFuzz(fuzz:Double)
		bmx_magick_image_colorfuzz(imagePtr, fuzz)
	End Method
	
	Rem
	bbdoc: Returns the distance where colors within this distance are considered equal.
	about: A number of algorithms search for a target color. By default the color must be exact.
	Use this option to match colors that are close to the target color in RGB space
	End Rem
	Method getColorFuzz:Double()
		Return bmx_magick_image_getcolorfuzz(imagePtr)
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
	' TODO
	End Method
	
	Method colorMapSize(entries:Int)
	' TODO
	End Method
	
	Method getColorMapSize:Int()
	' TODO
	End Method
	
	Rem
	bbdoc: The colorspace used to represent the image pixel colors.
	about: One of #COLORSPACE_UNDEFINEDCOLORSPACE, #COLORSPACE_RGBCOLORSPACE, #COLORSPACE_GRAYCOLORSPACE, #COLORSPACE_TRANSPARENTCOLORSPACE, #COLORSPACE_OHTACOLORSPACE, 
	#COLORSPACE_XYZCOLORSPACE, #COLORSPACE_YCCCOLORSPACE, #COLORSPACE_YIQCOLORSPACE, #COLORSPACE_YPBPRCOLORSPACE, #COLORSPACE_YUVCOLORSPACE, 
	#COLORSPACE_CMYKCOLORSPACE, #COLORSPACE_SRGBCOLORSPACE, #COLORSPACE_HSLCOLORSPACE, #COLORSPACE_HWBCOLORSPACE, #COLORSPACE_LABCOLORSPACE, 
	#COLORSPACE_CINEONLOGRGBCOLORSPACE, #COLORSPACE_REC601LUMACOLORSPACE, #COLORSPACE_REC601YCBCRCOLORSPACE, #COLORSPACE_REC709LUMACOLORSPACE or
	#COLORSPACE_REC709YCBCRCOLORSPACE.

	End Rem
	Method colorSpace(colorSpace:Int)
		bmx_magick_image_colorspace(imagePtr, colorSpace)
	End Method
	
	Rem
	bbdoc: Returns the colorspace used to represent the image pixel colors.
	about: One of #COLORSPACE_UNDEFINEDCOLORSPACE, #COLORSPACE_RGBCOLORSPACE, #COLORSPACE_GRAYCOLORSPACE, #COLORSPACE_TRANSPARENTCOLORSPACE, #COLORSPACE_OHTACOLORSPACE, 
	#COLORSPACE_XYZCOLORSPACE, #COLORSPACE_YCCCOLORSPACE, #COLORSPACE_YIQCOLORSPACE, #COLORSPACE_YPBPRCOLORSPACE, #COLORSPACE_YUVCOLORSPACE, 
	#COLORSPACE_CMYKCOLORSPACE, #COLORSPACE_SRGBCOLORSPACE, #COLORSPACE_HSLCOLORSPACE, #COLORSPACE_HWBCOLORSPACE, #COLORSPACE_LABCOLORSPACE, 
	#COLORSPACE_CINEONLOGRGBCOLORSPACE, #COLORSPACE_REC601LUMACOLORSPACE, #COLORSPACE_REC601YCBCRCOLORSPACE, #COLORSPACE_REC709LUMACOLORSPACE or
	#COLORSPACE_REC709YCBCRCOLORSPACE.

	End Rem
	Method getColorSpace:Int()
		Return bmx_magick_image_getcolorspace(imagePtr)
	End Method
	
	Method getColumns:Int()
	' TODO
	End Method
	
	Method getComment:String()
	' TODO
	End Method

	Rem
	bbdoc: Composition operator to be used when composition is implicitly used (such as for image flattening).
	about: One of #COMPOSITE_UNDEFINEDCOMPOSITEOP, #COMPOSITE_OVERCOMPOSITEOP, #COMPOSITE_INCOMPOSITEOP, #COMPOSITE_OUTCOMPOSITEOP,
	#COMPOSITE_ATOPCOMPOSITEOP, #COMPOSITE_XORCOMPOSITEOP, #COMPOSITE_PLUSCOMPOSITEOP, #COMPOSITE_MINUSCOMPOSITEOP,
	#COMPOSITE_ADDCOMPOSITEOP, #COMPOSITE_SUBTRACTCOMPOSITEOP, #COMPOSITE_DIFFERENCECOMPOSITEOP, #COMPOSITE_MULTIPLYCOMPOSITEOP,
	#COMPOSITE_BUMPMAPCOMPOSITEOP, #COMPOSITE_COPYCOMPOSITEOP, #COMPOSITE_COPYREDCOMPOSITEOP, #COMPOSITE_COPYGREENCOMPOSITEOP,
	#COMPOSITE_COPYBLUECOMPOSITEOP, #COMPOSITE_COPYOPACITYCOMPOSITEOP, #COMPOSITE_CLEARCOMPOSITEOP, #COMPOSITE_DISSOLVECOMPOSITEOP,
	#COMPOSITE_DISPLACECOMPOSITEOP, #COMPOSITE_MODULATECOMPOSITEOP, #COMPOSITE_THRESHOLDCOMPOSITEOP, #COMPOSITE_NOCOMPOSITEOP,
	#COMPOSITE_DARKENCOMPOSITEOP, #COMPOSITE_LIGHTENCOMPOSITEOP, #COMPOSITE_HUECOMPOSITEOP, #COMPOSITE_SATURATECOMPOSITEOP,
	#COMPOSITE_COLORIZECOMPOSITEOP, #COMPOSITE_LUMINIZECOMPOSITEOP, #COMPOSITE_SCREENCOMPOSITEOP, #COMPOSITE_OVERLAYCOMPOSITEOP,
	#COMPOSITE_COPYCYANCOMPOSITEOP, #COMPOSITE_COPYMAGENTACOMPOSITEOP, #COMPOSITE_COPYYELLOWCOMPOSITEOP,
	#COMPOSITE_COPYBLACKCOMPOSITEOP or #COMPOSITE_DIVIDECOMPOSITEOP.
	End Rem
	Method compose(compose:Int)
		bmx_magick_image_compose(imagePtr, compose)
	End Method
	
	Rem
	bbdoc: Returns the composition operator to be used when composition is implicitly used (such as for image flattening).
	about: One of #COMPOSITE_UNDEFINEDCOMPOSITEOP, #COMPOSITE_OVERCOMPOSITEOP, #COMPOSITE_INCOMPOSITEOP, #COMPOSITE_OUTCOMPOSITEOP,
	#COMPOSITE_ATOPCOMPOSITEOP, #COMPOSITE_XORCOMPOSITEOP, #COMPOSITE_PLUSCOMPOSITEOP, #COMPOSITE_MINUSCOMPOSITEOP,
	#COMPOSITE_ADDCOMPOSITEOP, #COMPOSITE_SUBTRACTCOMPOSITEOP, #COMPOSITE_DIFFERENCECOMPOSITEOP, #COMPOSITE_MULTIPLYCOMPOSITEOP,
	#COMPOSITE_BUMPMAPCOMPOSITEOP, #COMPOSITE_COPYCOMPOSITEOP, #COMPOSITE_COPYREDCOMPOSITEOP, #COMPOSITE_COPYGREENCOMPOSITEOP,
	#COMPOSITE_COPYBLUECOMPOSITEOP, #COMPOSITE_COPYOPACITYCOMPOSITEOP, #COMPOSITE_CLEARCOMPOSITEOP, #COMPOSITE_DISSOLVECOMPOSITEOP,
	#COMPOSITE_DISPLACECOMPOSITEOP, #COMPOSITE_MODULATECOMPOSITEOP, #COMPOSITE_THRESHOLDCOMPOSITEOP, #COMPOSITE_NOCOMPOSITEOP,
	#COMPOSITE_DARKENCOMPOSITEOP, #COMPOSITE_LIGHTENCOMPOSITEOP, #COMPOSITE_HUECOMPOSITEOP, #COMPOSITE_SATURATECOMPOSITEOP,
	#COMPOSITE_COLORIZECOMPOSITEOP, #COMPOSITE_LUMINIZECOMPOSITEOP, #COMPOSITE_SCREENCOMPOSITEOP, #COMPOSITE_OVERLAYCOMPOSITEOP,
	#COMPOSITE_COPYCYANCOMPOSITEOP, #COMPOSITE_COPYMAGENTACOMPOSITEOP, #COMPOSITE_COPYYELLOWCOMPOSITEOP,
	#COMPOSITE_COPYBLACKCOMPOSITEOP or #COMPOSITE_DIVIDECOMPOSITEOP.
	End Rem
	Method getComposite:Int()
		Return bmx_magick_image_getcomposite(imagePtr)
	End Method
	
	Rem
	bbdoc: Compression type
	about: One of COMPRESSION_UNDEFINEDCOMPRESSION, COMPRESSION_NOCOMPRESSION, COMPRESSION_BZIPCOMPRESSION,
	COMPRESSION_FAXCOMPRESSION, COMPRESSION_GROUP4COMPRESSION, COMPRESSION_JPEGCOMPRESSION,
	COMPRESSION_LOSSLESSJPEGCOMPRESSION, COMPRESSION_LZWCOMPRESSION, COMPRESSION_RLECOMPRESSION or COMPRESSION_ZIPCOMPRESSION.
	End Rem
	Method compressType(_type:Int)
		bmx_magick_image_compresstype(imagePtr, _type)
	End Method
	
	Rem
	bbdoc: Returns the compression type.
	about: One of COMPRESSION_UNDEFINEDCOMPRESSION, COMPRESSION_NOCOMPRESSION, COMPRESSION_BZIPCOMPRESSION,
	COMPRESSION_FAXCOMPRESSION, COMPRESSION_GROUP4COMPRESSION, COMPRESSION_JPEGCOMPRESSION,
	COMPRESSION_LOSSLESSJPEGCOMPRESSION, COMPRESSION_LZWCOMPRESSION, COMPRESSION_RLECOMPRESSION or COMPRESSION_ZIPCOMPRESSION.
	End Rem
	Method getCompressType:Int()
		Return bmx_magick_image_getcompresstype(imagePtr)
	End Method
	
	Rem
	bbdoc: Tagged image format define (set coder-specific option).
	about: The @magick option specifies the coder the define applies to. The @key option provides the key specific to that coder.
	The @value option provides the value to set (if any). See the defineSet() method if the key must be removed entirely.
	End Rem
	Method defineValue(magick:String, key:String, value:String)
		bmx_magick_image_definevalue(imagePtr, magick, key, value)
	End Method
	
	Rem
	bbdoc: Returns the tagged image format define (access coder-specific option).
	about: The @magick option specifies the coder the define applies to. The @key option provides the key specific to that coder.
	End Rem
	Method getDefinedValue:String(magick:String, key:String)
		Return bmx_magick_image_getdefinedvalue(imagePtr, magick, key)
	End Method
	
	Rem
	bbdoc: Tagged image format define.
	about: Similar to the defineValue() method except that passing the @flag value True creates a value-less
	define with that format and key. Passing the @flag value False removes any existing matching definition.
	End Rem
	Method defineSet(magick:String, key:String, flag:Int)
		bmx_magick_image_defineset(imagePtr, magick, key, flag)
	End Method
	
	Rem
	bbdoc: Returns the tagged image format define.
	about: The method returns True if a matching key exists, and False if no matching key exists.
	End Rem
	Method getDefinedSet:Int(magick:String, key:String)
		Return bmx_magick_image_getdefinedset(imagePtr, magick, key)
	End Method
	
	Rem
	bbdoc: Vertical and horizontal resolution in pixels of the image.
	End Rem
	Method density(geometry:Object)
		If TMGeometry(geometry) Then
			bmx_magick_image_density(imagePtr, TMGeometry(geometry).geometryPtr)
		Else If String(geometry) Then
			bmx_magick_image_densitytxt(imagePtr, String(geometry))
		End If
	End Method
	
	Method getDensity:TMGeometry()
	' TODO
	End Method
	
	Rem
	bbdoc: Image depth (bits allocated to red/green/blue components).
	End Rem
	Method depth(depth:Int)
		bmx_magick_image_depth(imagePtr, depth)
	End Method
	
	Rem
	bbdoc: Returns the image depth (bits allocated to red/green/blue components).
	End Rem
	Method getDepth:Int()
		Return bmx_magick_image_getdepth(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the tile names from within an image montage.
	End Rem
	Method directory:String()
		Return bmx_magick_image_directory(imagePtr)
	End Method
	
	Rem
	bbdoc: Endianness (little like Intel or big like SPARC) for image formats which support endian-specific options.
	about: One of ENDIAN_UNDEFINEDENDIAN, ENDIAN_LSBENDIAN, ENDIAN_MSBENDIAN or ENDIAN_NATIVEENDIAN.
	End Rem
	Method endian(endian:Int)
		bmx_magick_image_endian(imagePtr, endian)
	End Method
	
	Rem
	bbdoc: Returns the endianness (little like Intel or big like SPARC) for image formats which support endian-specific options.
	about: One of ENDIAN_UNDEFINEDENDIAN, ENDIAN_LSBENDIAN, ENDIAN_MSBENDIAN or ENDIAN_NATIVEENDIAN.
	End Rem
	Method getEndian:Int()
		Return bmx_magick_image_getendian(imagePtr)
	End Method
	
	Rem
	bbdoc: Image file name.
	End Rem
	Method filename(filename:String)
		bmx_magick_image_filename(imagePtr, filename)
	End Method
	
	Rem
	bbdoc: Returns the image file name.
	End Rem
	Method getFilename:String()
		Return bmx_magick_image_getfilename(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the number of bytes of the image on disk.
	End Rem
	Method FileSize:Int()
		Return bmx_magick_image_filesize(imagePtr)
	End Method
	
	Rem
	bbdoc: Color to use when filling drawn objects.
	End Rem
	Method fillColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_fillcolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color)
			bmx_magick_image_fillcolortxt(imagePtr, String(color))
		EndIf
	End Method
	
	Method getFillColor:TMColor()
	' TODO
	End Method
	
	Rem
	bbdoc: Rule to use when filling drawn objects.
	about: One of FILLRULE_UNDEFINEDRULE, FILLRULE_EVENODDRULE or FILLRULE_NONZERORULE.
	End Rem
	Method fillRule(rule:Int)
		bmx_magick_image_fillrule(imagePtr, rule)
	End Method
	
	Rem
	bbdoc: Returns the rule to use when filling drawn objects.
	about: One of FILLRULE_UNDEFINEDRULE, FILLRULE_EVENODDRULE or FILLRULE_NONZERORULE.
	End Rem
	Method getFillRule:Int()
		Return bmx_magick_image_getfillrule(imagePtr)
	End Method
	
	Method fillPattern(pattern:TMImage)
	' TODO
	End Method
	
	Method getFillPattern:TMImage()
	' TODO
	End Method
	
	Rem
	bbdoc: Filter to use when resizing image.
	about: One of FILTER_UNDEFINEDFILTER, FILTER_POINTFILTER, FILTER_BOXFILTER, FILTER_TRIANGLEFILTER,
	FILTER_HERMITEFILTER, FILTER_HANNINGFILTER, FILTER_HAMMINGFILTER, FILTER_BLACKMANFILTER, FILTER_GAUSSIANFILTER,
	FILTER_QUADRATICFILTER, FILTER_CUBICFILTER, FILTER_CATROMFILTER, FILTER_MITCHELLFILTER, FILTER_LANCZOSFILTER,
	FILTER_BESSELFILTER or FILTER_SINCFILTER
	End Rem
	Method filterType(filterType:Int)
		bmx_magick_image_filtertype(imagePtr, filterType)
	End Method
	
	Rem
	bbdoc: Returns the filter to use when resizing image.
	about: One of FILTER_UNDEFINEDFILTER, FILTER_POINTFILTER, FILTER_BOXFILTER, FILTER_TRIANGLEFILTER,
	FILTER_HERMITEFILTER, FILTER_HANNINGFILTER, FILTER_HAMMINGFILTER, FILTER_BLACKMANFILTER, FILTER_GAUSSIANFILTER,
	FILTER_QUADRATICFILTER, FILTER_CUBICFILTER, FILTER_CATROMFILTER, FILTER_MITCHELLFILTER, FILTER_LANCZOSFILTER,
	FILTER_BESSELFILTER or FILTER_SINCFILTER
	End Rem
	Method getFilterType:Int()
		Return bmx_magick_image_getfiltertype(imagePtr)
	End Method
	
	Rem
	bbdoc: Text rendering font.
	End Rem
	Method font(font:String)
		bmx_magick_image_font(imagePtr, font)
	End Method
	
	Rem
	bbdoc: Returns the text rendering font.
	End Rem
	Method getFont:String()
		Return bmx_magick_image_getfont(imagePtr)
	End Method
	
	Rem
	bbdoc: Font point size.
	End Rem
	Method fontPointSize(pointSize:Double)
		bmx_magick_image_fontpointsize(imagePtr, pointSize)
	End Method
	
	Rem
	bbdoc: Returns the font point size.
	End Rem
	Method getFontPointSize:Double()
		Return bmx_magick_image_getfontpointsize(imagePtr)
	End Method
	
	'Method fontTypeMetrics(text:String, metrics...)
	' TODO
	'End Method
	
	Rem
	bbdoc: Returns the long image format description.
	End Rem
	Method getFormat:String()
		Return bmx_magick_image_getformat(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the gamma level of the image.
	End Rem
	Method getGamma:Double()
		Return bmx_magick_image_getgamma(imagePtr)
	End Method
	
	Method getGeometry:TMGeometry()
	' TODO
	End Method
	
	Rem
	bbdoc: GIF disposal method.
	End Rem
	Method gifDisposeMethod(disposeMethod:Int)
		bmx_magick_image_gifdisposemethod(imagePtr, disposeMethod)
	End Method
	
	Rem
	bbdoc: Returns the GIF disposal method.
	End Rem
	Method getGifDisposeMethod:Int()
		Return bmx_magick_image_getgifdisposemethod(imagePtr)
	End Method
	
	Method iccColorProfile(profile:TMBlob)
	' TODO
	End Method
	
	Method getIccColorProfile:TMBlob()
	' TODO
	End Method
	
	Rem
	bbdoc: Type of interlacing to use.
	about: One of INTERLACE_UNDEFINEDINTERLACE, INTERLACE_NOINTERLACE, INTERLACE_LINEINTERLACE, 
	INTERLACE_PLANEINTERLACE or INTERLACE_PARTITIONINTERLACE.
	End Rem
	Method interlaceType(interlace:Int)
		bmx_magick_image_interlacetype(imagePtr, interlace)
	End Method
	
	Rem
	bbdoc: Return the type of interlacing used.
	about: One of INTERLACE_UNDEFINEDINTERLACE, INTERLACE_NOINTERLACE, INTERLACE_LINEINTERLACE, 
	INTERLACE_PLANEINTERLACE or INTERLACE_PARTITIONINTERLACE.
	End Rem
	Method getInterlaceType:Int()
		Return bmx_magick_image_getinterlacetype(imagePtr)
	End Method
	
	Method iptcProfile(profile:TMBlob)
	' TODO
	End Method
	
	Method getIptcProfile:TMBlob()
	' TODO
	End Method
	
	Rem
	bbdoc: Does object contain valid image?
	End Rem
	Method isValid(isValid:Int)
		bmx_magick_image_isvalid(imagePtr, isValid)
	End Method
	
	Rem
	bbdoc: Returns whether the object contains valid image.
	End Rem
	Method getIsValid:Int()
		Return bmx_magick_image_getisvalid(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the image label.
	End Rem
	Method getLabel:String()
		Return bmx_magick_image_getlabel(imagePtr)
	End Method
	
	Rem
	bbdoc: Stroke width for drawing vector objects.
	End Rem
	Method strokeWidth(width:Double)
		bmx_magick_image_strokewidth(imagePtr, width)
	End Method
	
	Rem
	bbdoc: Returns the stroke width for drawing vector objects
	End Rem
	Method getStrokeWidth:Double()
		Return bmx_magick_image_getstrokewidth(imagePtr)
	End Method
	
	Rem
	bbdoc: File type magick identifier (.e.g "GIF")
	End Rem
	Method magick(magick:String)
		bmx_magick_image_magick(imagePtr, magick)
	End Method
	
	Rem
	bbdoc: Returns the file type magick identifier (.e.g "GIF")
	End Rem
	Method getMagick:String()
		Return bmx_magick_image_getmagick(imagePtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method matte(matteFlag:Int)
		bmx_magick_image_matte(imagePtr, matteFlag)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getMatte:Int()
		Return bmx_magick_image_getmatte(imagePtr)
	End Method
	
	Method matteColor(color:Object)
	' TODO
	End Method
	
	Method getMatteColor:TMColor()
	' TODO
	End Method
	
	Method getMeanErrorPerPixel:Double()
	' TODO
	End Method
	
	Rem
	bbdoc: Image modulus depth (minimum number of bits required to support red/green/blue components without loss of accuracy).
	about: The pixel modulus depth may be decreased by supplying a value which is less than the current value, updating the
	pixels (reducing accuracy) to the new depth. The pixel modulus depth cannot be increased over the current value using this method.
	End Rem
	Method modulusDepth(depth:Int)
		bmx_magick_image_modulusdepth(imagePtr, depth)
	End Method
	
	Rem
	bbdoc: Returns the image modulus depth (minimum number of bits required to support red/green/blue components without loss of accuracy).
	about: The pixel modulus depth may be decreased by supplying a value which is less than the current value, updating the
	pixels (reducing accuracy) to the new depth. The pixel modulus depth cannot be increased over the current value using this method.
	End Rem
	Method getModulusDepth:Int()
		Return bmx_magick_image_getmodulusdepth(imagePtr)
	End Method

	Rem
	bbdoc: Transform image to black and white while color reducing (quantizing).
	End Rem
	Method monochrome(flag:Int)
		bmx_magick_image_monochrome(imagePtr, flag)
	End Method
	
	Rem
	bbdoc: Returns whether to transform image to black and white while color reducing (quantizing).
	End Rem
	Method getMonochrome:Int()
		Return bmx_magick_image_getmonochrome(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the normalized max error per pixel computed when an image is color reduced.
	about: This is only valid if verbose is set to true and the image has just been quantized
	End Rem
	Method getNormalizedMaxError:Double()
		Return bmx_magick_image_normalizedmaxerror(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the normalized mean error per pixel computed when an image is color reduced.
	about: This is only valid if verbose is set to true and the image has just been quantized.
	End Rem
	Method getNormalizedMeanError:Double()
		Return bmx_magick_image_normalizedmeanerror(imagePtr)
	End Method
	
	Rem
	bbdoc: Image orientation.
	about: Supported by some file formats such as DPX and TIFF. Useful for turning the right way up.
	<p>
	One of #ORIENTATION_UNDEFINEDORIENTATION, #ORIENTATION_TOPLEFTORIENTATION, #ORIENTATION_TOPRIGHTORIENTATION, 
	#ORIENTATION_BOTTOMRIGHTORIENTATION, #ORIENTATION_BOTTOMLEFTORIENTATION, #ORIENTATION_LEFTTOPORIENTATION, 
	#ORIENTATION_RIGHTTOPORIENTATION, #ORIENTATION_RIGHTBOTTOMORIENTATION or  #ORIENTATION_LEFTBOTTOMORIENTATION.

	</p>
	End Rem
	Method orientation(orientation:Int)
		bmx_magick_image_orientation(imagePtr, orientation)
	End Method
	
	Rem
	bbdoc: Returns the image orientation.
	about: Supported by some file formats such as DPX and TIFF. Useful for turning the right way up.
	<p>
	One of #ORIENTATION_UNDEFINEDORIENTATION, #ORIENTATION_TOPLEFTORIENTATION, #ORIENTATION_TOPRIGHTORIENTATION, 
	#ORIENTATION_BOTTOMRIGHTORIENTATION, #ORIENTATION_BOTTOMLEFTORIENTATION, #ORIENTATION_LEFTTOPORIENTATION, 
	#ORIENTATION_RIGHTTOPORIENTATION, #ORIENTATION_RIGHTBOTTOMORIENTATION or  #ORIENTATION_LEFTBOTTOMORIENTATION.

	</p>
	End Rem
	Method getOrientation:Int()
		Return bmx_magick_image_getorientation(imagePtr)
	End Method
	
	Method page(pageSize:Object)
	' TODO
	End Method
	
	Method getPage:TMGeometry()
	' TODO
	End Method
	
	Method penColor(color:Object)
	' TODO
	End Method
	
	Method getPenColor:TMColor()
	' TODO
	End Method
	
	Method penTexture(texture:TMImage)
	' TODO
	End Method
	
	Method getPenTexture:TMImage()
	' TODO
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
	' TODO
	End Method
	
	Method profile(name:String, colorProfile:TMBlob)
	' TODO
	End Method
	
	Method getProfile:TMBlob(name:String)
	' TODO
	End Method
	
	Rem
	bbdoc: JPEG/MIFF/PNG compression level (default 75).
	End Rem
	Method quality(value:Int)
		bmx_magick_image_quality(imagePtr, value)
	End Method
	
	Rem
	bbdoc: Returns the JPEG/MIFF/PNG compression level.
	End Rem
	Method getQuality:Int()
		Return bmx_magick_image_getquality(imagePtr)
	End Method
	
	Rem
	bbdoc: Maximum number of colors to quantize to.
	End Rem
	Method quantizeColors(colors:Int)
		bmx_magick_image_quantizecolors(imagePtr, colors)
	End Method

	Rem
	bbdoc: Returns the maximum number of colors to quantize to.
	End Rem
	Method getQuantizeColors:Int()
		Return bmx_magick_image_getquantizecolors(imagePtr)
	End Method
	
	Rem
	bbdoc: Colorspace to quantize in (default RGB).
	about: Empirical evidence suggests that distances in color spaces such as YUV or YIQ correspond
	to perceptual color differences more closely than do distances in RGB space.
	These color spaces may give better results when color reducing an image.
	<p>
	One of #COLORSPACE_UNDEFINEDCOLORSPACE, #COLORSPACE_RGBCOLORSPACE, #COLORSPACE_GRAYCOLORSPACE, #COLORSPACE_TRANSPARENTCOLORSPACE, 
	#COLORSPACE_OHTACOLORSPACE, #COLORSPACE_XYZCOLORSPACE, #COLORSPACE_YCCCOLORSPACE, #COLORSPACE_YIQCOLORSPACE, 
	#COLORSPACE_YPBPRCOLORSPACE, #COLORSPACE_YUVCOLORSPACE, #COLORSPACE_CMYKCOLORSPACE, #COLORSPACE_SRGBCOLORSPACE, 
	#COLORSPACE_HSLCOLORSPACE, #COLORSPACE_HWBCOLORSPACE, #COLORSPACE_LABCOLORSPACE, #COLORSPACE_CINEONLOGRGBCOLORSPACE, 
	#COLORSPACE_REC601LUMACOLORSPACE, #COLORSPACE_REC601YCBCRCOLORSPACE, #COLORSPACE_REC709LUMACOLORSPACE or
	#COLORSPACE_REC709YCBCRCOLORSPACE.
	</p>

	End Rem
	Method quantizeColorSpace(colorSpace:Int)
		bmx_magick_image_quantizecolorspace(imagePtr, colorSpace)
	End Method

	Rem
	bbdoc: Returns the colorspace to quantize in (default RGB).
	about: Empirical evidence suggests that distances in color spaces such as YUV or YIQ correspond
	to perceptual color differences more closely than do distances in RGB space.
	These color spaces may give better results when color reducing an image.
	<p>
	One of #COLORSPACE_UNDEFINEDCOLORSPACE, #COLORSPACE_RGBCOLORSPACE, #COLORSPACE_GRAYCOLORSPACE, #COLORSPACE_TRANSPARENTCOLORSPACE, 
	#COLORSPACE_OHTACOLORSPACE, #COLORSPACE_XYZCOLORSPACE, #COLORSPACE_YCCCOLORSPACE, #COLORSPACE_YIQCOLORSPACE, 
	#COLORSPACE_YPBPRCOLORSPACE, #COLORSPACE_YUVCOLORSPACE, #COLORSPACE_CMYKCOLORSPACE, #COLORSPACE_SRGBCOLORSPACE, 
	#COLORSPACE_HSLCOLORSPACE, #COLORSPACE_HWBCOLORSPACE, #COLORSPACE_LABCOLORSPACE, #COLORSPACE_CINEONLOGRGBCOLORSPACE, 
	#COLORSPACE_REC601LUMACOLORSPACE, #COLORSPACE_REC601YCBCRCOLORSPACE, #COLORSPACE_REC709LUMACOLORSPACE or
	#COLORSPACE_REC709YCBCRCOLORSPACE.
	</p>

	End Rem
	Method getQuantizeColorSpace:Int()
		Return bmx_magick_image_getquantizecolorspace(imagePtr)
	End Method
	
	Rem
	bbdoc: Apply Floyd/Steinberg error diffusion to the image.
	about: The basic strategy of dithering is to trade intensity resolution for spatial resolution by averaging the
	intensities of several neighboring pixels. Images which suffer from severe contouring when reducing colors can
	be improved with this option. The quantizeColors or monochrome option must be set for this option to take effect
	End Rem
	Method quantizeDither(flag:Int)
		bmx_magick_image_quantizedither(imagePtr, flag)
	End Method

	Rem
	bbdoc: Returns wheter to apply Floyd/Steinberg error diffusion to the image.
	about: The basic strategy of dithering is to trade intensity resolution for spatial resolution by averaging the
	intensities of several neighboring pixels. Images which suffer from severe contouring when reducing colors can
	be improved with this option. The quantizeColors or monochrome option must be set for this option to take effect
	End Rem
	Method getQuantizeDither:Int()
		Return bmx_magick_image_getquantizedither(imagePtr)
	End Method
	
	Rem
	bbdoc: Depth of the quantization color classification tree.
	about: Values of 0 or 1 allow selection of the optimal tree depth for the color reduction algorithm.
	Values between 2 and 8 may be used to manually adjust the tree depth
	End Rem
	Method quantizeTreeDepth(treeDepth:Int)
		bmx_magick_image_quantizetreedepth(imagePtr, treeDepth)
	End Method

	Rem
	bbdoc: Returns the depth of the quantization color classification tree.
	about: Values of 0 or 1 allow selection of the optimal tree depth for the color reduction algorithm.
	Values between 2 and 8 may be used to manually adjust the tree depth
	End Rem
	Method getQuantizeTreeDepth:Int()
		Return bmx_magick_image_getquantizetreedepth(imagePtr)
	End Method
	
	Rem
	bbdoc: The type of rendering intent (used when applying an ICC color profile).
	about: One of RENDERING_UNDEFINEDINTENT, RENDERING_SATURATIONINTENT, RENDERING_PERCEPTUALINTENT
	RENDERING_ABSOLUTEINTENT, or RENDERING_RELATIVEINTENT.
	End Rem
	Method renderingIntent(intent:Int)
		bmx_magick_image_renderingintent(imagePtr, intent)
	End Method
	
	Rem
	bbdoc: Returns the type of rendering intent (used when applying an ICC color profile).
	about: One of RENDERING_UNDEFINEDINTENT, RENDERING_SATURATIONINTENT, RENDERING_PERCEPTUALINTENT
	RENDERING_ABSOLUTEINTENT, or RENDERING_RELATIVEINTENT.
	End Rem
	Method getRenderingIntent:Int()
		Return bmx_magick_image_getrenderingintent(imagePtr)
	End Method
	
	Rem
	bbdoc: Units of image resolution.
	End Rem
	Method resolutionUnits(units:Int)
		bmx_magick_image_resolutionunits(imagePtr, units)
	End Method
	
	Rem
	bbdoc: Returns the units of image resolution.
	End Rem
	Method getResolutionUnits:Int()
		Return bmx_magick_image_getresolutionunits(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the number of pixel rows in the image.
	End Rem
	Method getRows:Int()
		Return bmx_magick_image_getrows(imagePtr)
	End Method
	
	Rem
	bbdoc: Image scene number.
	End Rem
	Method scene(scene:Int)
		bmx_magick_image_scene(imagePtr, scene)
	End Method
	
	Rem
	bbdoc: Returns the image scene number.
	End Rem
	Method getScene:Int()
		Return bmx_magick_image_getscene(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the image textual signature.
	about: Set @force to true in order to re-calculate the signature regardless of whether the image data has been modified.
	End Rem
	Method getSignature:String(force:Int = False)
		Return bmx_magick_image_getsignature(imagePtr, force)
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
	
	Rem
	bbdoc: 
	End Rem
	Method strokeAntiAlias(flag:Int)
		bmx_magick_image_strokeantialias(imagePtr, flag)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getStrokeAntiAlias:Int()
		Return bmx_magick_image_getstrokeantialias(imagePtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method strokeColor(color:Object)
		If TMColor(color) Then
			bmx_magick_image_strokecolor(imagePtr, TMColor(color).colorPtr)
		ElseIf String(color) Then
			bmx_magick_image_strokecolortxt(imagePtr, String(color))
		End If
	End Method
	
	Method getStrokeColor:TMColor()
	' TODO
	End Method
	
	Rem
	bbdoc: Specify the pattern of dashes and gaps used to stroke paths.
	about: The strokeDashArray represents an array of numbers that specify the lengths of alternating dashes and gaps in pixels.
	If an odd number of values is provided, then the list of values is repeated to yield an even number
	of values. A typical strokeDashArray array might contain the members 5 3 2.
	End Rem
	Method strokeDashArray(array:Double[])
		bmx_magick_image_strokedasharray(imagePtr, array)
	End Method
	
	Rem
	bbdoc: Returns the pattern of dashes and gaps used to stroke paths.
	about: The strokeDashArray represents an array of numbers that specify the lengths of alternating dashes and gaps in pixels.
	If an odd number of values is provided, then the list of values is repeated to yield an even number
	of values. A typical strokeDashArray array might contain the members 5 3 2.
	End Rem
	Method getStrokeDashArray:Double[]()
		Return bmx_magick_image_getstrokedasharray(imagePtr)
	End Method
	
	Rem
	bbdoc: While drawing using a dash pattern, specify distance into the dash pattern to start the dash (default 0)
	End Rem
	Method strokeDashOffset(offset:Double)
		bmx_magick_image_strokedashoffset(imagePtr, offset)
	End Method
	
	Rem
	bbdoc: Returns the distance into the dash pattern to start the dash of a dash pattern.
	End Rem
	Method getStrokeDashOffset:Double()
		Return bmx_magick_image_getstrokedashoffset(imagePtr)
	End Method
	
	Rem
	bbdoc: Specifies the shape to be used at the end of open subpaths when they are stroked.
	about: Values of LineCap are LINECAP_UNDEFINEDCAP, LINECAP_BUTTCAP, LINECAP_ROUNDCAP and LINECAP_SQUARECAP.
	End Rem
	Method strokeLineCap(lineCap:Int)
		bmx_magick_image_strokelinecap(imagePtr, lineCap)
	End Method
	
	Rem
	bbdoc: Returns the shape to be used at the end of open subpaths when they are stroked.
	about: Values of LineCap are LINECAP_UNDEFINEDCAP, LINECAP_BUTTCAP, LINECAP_ROUNDCAP and LINECAP_SQUARECAP.
	End Rem
	Method getStrokeLineCap:Int()
		Return bmx_magick_image_getstrokelinecap(imagePtr)
	End Method
	
	Rem
	bbdoc: Specifies the shape to be used at the corners of paths (or other vector shapes) when they are stroked.
	about: Values of LineJoin are LINEJOIN_UNDEFINEDJOIN, LINEJOIN_MITERJOIN,  LINEJOIN_ROUNDJOIN and LINEJOIN_BEVELJOIN.
	End Rem
	Method strokeLineJoin(lineJoin:Int)
		bmx_magick_image_strokelinejoin(imagePtr, lineJoin)
	End Method
	
	Rem
	bbdoc: Returns the shape to be used at the corners of paths (or other vector shapes) when they are stroked.
	about: Values of LineJoin are LINEJOIN_UNDEFINEDJOIN, LINEJOIN_MITERJOIN,  LINEJOIN_ROUNDJOIN and LINEJOIN_BEVELJOIN.
	End Rem
	Method getStrokeLineJoin:Int()
		Return bmx_magick_image_getstrokelinejoin(imagePtr)
	End Method
	
	Rem
	bbdoc: Specifies miter limit.
	about: When two line segments meet at a sharp angle and miter joins have been specified for 'lineJoin', it
	is possible for the miter to extend far beyond the thickness of the line stroking the path. The miterLimit imposes a
	limit on the ratio of the miter length to the 'lineWidth'. The default value of this parameter is 4
	End Rem
	Method strokeMiterLimit(miterLimit:Int)
		bmx_magick_image_strokemiterlimit(imagePtr, miterlimit)
	End Method
	
	Rem
	bbdoc: Returns the miter limit.
	about: When two line segments meet at a sharp angle and miter joins have been specified for 'lineJoin', it
	is possible for the miter to extend far beyond the thickness of the line stroking the path. The miterLimit imposes a
	limit on the ratio of the miter length to the 'lineWidth'. The default value of this parameter is 4
	End Rem
	Method getStrokeMiterLimit:Int()
		Return bmx_magick_image_getstrokemiterlimit(imagePtr)
	End Method
	
	Rem
	bbdoc: Pattern image to use while stroking object outlines.
	End Rem
	Method strokePattern(pattern:TMImage)
		bmx_magick_image_strokepattern(imagePtr, pattern.imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the pattern image to use while stroking object outlines.
	End Rem
	Method getStrokePattern:TMImage()
		Return TMImage._create(bmx_magick_image_getstrokepattern(imagePtr))
	End Method
	
	Rem
	bbdoc: Subimage of an image sequence.
	End Rem
	Method subImage(subImage:Int)
		bmx_magick_image_subimage(imagePtr, subImage)
	End Method
	
	Rem
	bbdoc: Returns the subimage of an image sequence.
	End Rem
	Method getSubImage:Int()
		Return bmx_magick_image_getsubimage(imagePtr)
	End Method
	
	Rem
	bbdoc: Number of images relative to the base image.
	End Rem
	Method subRange(subRange:Int)
		bmx_magick_image_subrange(imagePtr, subRange)
	End Method
	
	Rem
	bbdoc: Returns the number of images relative to the base image.
	End Rem
	Method getSubRange:Int()
		Return bmx_magick_image_getsubrange(imagePtr)
	End Method
	
	Rem
	bbdoc: Annotation text encoding (e.g. "UTF-16").
	End Rem
	Method textEncoding(encoding:String)
		bmx_magick_image_textencoding(imagePtr, encoding)
	End Method
	
	Rem
	bbdoc: Returns the annotation text encoding (e.g. "UTF-16").
	End Rem
	Method getTextEncoding:String()
		Return bmx_magick_image_gettextencoding(imagePtr)
	End Method

	Rem
	bbdoc: Tile name.
	End Rem
	Method tileName(name:String)
		bmx_magick_image_tilename(imagePtr, name)
	End Method
	
	Rem
	bbdoc: Returns the tile name.
	End Rem
	Method getTileName:String()
		Return bmx_magick_image_gettilename(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the number of colors in the image.
	End Rem
	Method getTotalColors:Int()
		Return bmx_magick_image_gettotalcolors(imagePtr)
	End Method
	
	Rem
	bbdoc: Origin of coordinate system to use when annotating with text or drawing.
	End Rem
	Method transformOrigin(x:Double, y:Double)
		bmx_magick_image_transformorigin(imagePtr, x, y)
	End Method
	
	Rem
	bbdoc: Rotation to use when annotating with text or drawing.
	End Rem
	Method transformRotation(angle:Double)
		bmx_magick_image_transformrotation(imagePtr, angle)
	End Method
	
	Rem
	bbdoc: Resets transformation parameters to default.
	End Rem
	Method transformReset()
		bmx_magick_image_transformreset(imagePtr)
	End Method
	
	Rem
	bbdoc: Scale to use when annotating with text or drawing.
	End Rem
	Method transformScale(sx:Double, sy:Double)
		bmx_magick_image_transformscale(imagePtr, sx, sy)
	End Method
	
	Rem
	bbdoc: Skew to use in X axis when annotating with text or drawing.
	End Rem
	Method transformSkewX(skew:Double)
		bmx_magick_image_transformskewx(imagePtr, skew)
	End Method
	
	Rem
	bbdoc: Skew to use in Y axis when annotating with text or drawing.
	End Rem
	Method transformSkewY(skew:Double)
		bmx_magick_image_transformskewy(imagePtr, skew)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getType:Int()
		Return bmx_magick_image_gettype(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the x resolution of the image.
	End Rem
	Method getXResolution:Double()
		Return bmx_magick_image_getxresolution(imagePtr)
	End Method
	
	Rem
	bbdoc: Returns the y resolution of the image
	End Rem
	Method getYResolution:Double()
		Return bmx_magick_image_getyresolution(imagePtr)
	End Method
	
End Type

Rem
bbdoc: Geometry provides a convenient means to specify a geometry argument.
about: 
End Rem
Type TMGeometry

	Field geometryPtr:Byte Ptr
	
	Function _create:TMGeometry(geometryPtr:Byte Ptr)
		If geometryPtr Then
			Local this:TMGeometry = New TMGeometry
			this.geometryPtr = geometryPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function CreateGeometry:TMGeometry(geometry:String)
		Return New TMGeometry.Create(geometry)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TMGeometry(geometry:String)
		geometryPtr = bmx_magick_geometry_create(geometry)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function CreateGeometryWithAttributes:TMGeometry(width:Int, height:Int, xOffset:Int = 0, yOffset:Int = 0, xNegative:Int = False, yNegative:Int = False)
		Return New TMGeometry.CreateWithAttributes(width, height, xOffset, yOffset, xNegative, yNegative)
	End Function

	Rem
	bbdoc: 
	End Rem
	Method CreateWithAttributes:TMGeometry(width:Int, height:Int, xOffset:Int = 0, yOffset:Int = 0, xNegative:Int = False, yNegative:Int = False)
		geometryPtr = bmx_magick_geometry_createwithattributes(width, height, xOffset, yOffset, xNegative, yNegative)
		Return Self
	End Method
	
	Rem
	bbdoc: Sets the geometry width.
	End Rem
	Method setWidth(width:Int)
		bmx_magick_geometry_setwidth(geometryPtr, width)
	End Method
	
	Rem
	bbdoc: Returns the geometry width.
	End Rem
	Method getWidth:Int()
		Return bmx_magick_geometry_getwidth(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets the geometry height.
	End Rem
	Method setHeight(height:Int)
		bmx_magick_geometry_setheight(geometryPtr, height)
	End Method
	
	Rem
	bbdoc: Returns the geometry height.
	End Rem
	Method getHeight:Int()
		Return bmx_magick_geometry_getheight(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets the X offset from origin.
	End Rem
	Method setXOffset(offset:Int)
		bmx_magick_geometry_setxoffset(geometryPtr, offset)
	End Method
	
	Rem
	bbdoc: Returns the X offset from origin.
	End Rem
	Method getXOffset:Int()
		Return bmx_magick_geometry_getxoffset(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets the Y offset from origin.
	End Rem
	Method setYOffset(offset:Int)
		bmx_magick_geometry_setyoffset(geometryPtr, offset)
	End Method
	
	Rem
	bbdoc: Returns the Y offset from origin.
	End Rem
	Method getYOffset:Int()
		Return bmx_magick_geometry_getyoffset(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets whether the sign of X offset negative. (X origin at right).
	End Rem
	Method setXNegative(value:Int)
		bmx_magick_geometry_setxnegative(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether the sign of X offset negative. (X origin at right).
	End Rem
	Method isXNegative:Int()
		Return bmx_magick_geometry_isxnegative(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets whether sign of Y offset negative. (Y origin at bottom).
	End Rem
	Method setYNegative(value:Int)
		bmx_magick_geometry_setynegative(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether sign of Y offset negative. (Y origin at bottom).
	End Rem
	Method isYNegative:Int()
		Return bmx_magick_geometry_isynegative(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets whether width and height are expressed as percentages.
	End Rem
	Method setPercent(value:Int)
		bmx_magick_geometry_setpercent(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether width and height are expressed as percentages.
	End Rem
	Method getPercent:Int()
		Return bmx_magick_geometry_getpercent(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets whether resize preserves aspect ratio.
	End Rem
	Method setAspect(value:Int)
		bmx_magick_geometry_setaspect(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether resize preserves aspect ratio.
	End Rem
	Method getAspect:Int()
		Return bmx_magick_geometry_getaspect(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets whether to resize if image is greater than size.
	End Rem
	Method setGreater(value:Int)
		bmx_magick_geometry_setgreater(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether to resize if image is greater than size.
	End Rem
	Method getGreater:Int()
		Return bmx_magick_geometry_getgreater(geometryPtr)
	End Method
	
	Rem
	bbdoc: Sets whether to resize if image is less than size.
	End Rem
	Method setLess(value:Int)
		bmx_magick_geometry_setless(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Returns whether to resize if image is less than size.
	End Rem
	Method getLess:Int()
		Return bmx_magick_geometry_getless(geometryPtr)
	End Method
	
	Rem
	bbdoc: May be set to False in order to invalidate an existing geometry object.
	End Rem
	Method setIsValid(value:Int)
		bmx_magick_geometry_setisvalid(geometryPtr, value)
	End Method
	
	Rem
	bbdoc: Does object contain a valid geometry?
	End Rem
	Method isValid:Int()
		Return bmx_magick_geometry_isvalid(geometryPtr)
	End Method

	Rem
	bbdoc: Returns a String representation of the geometry.
	End Rem
	Method toString:String()
		Return bmx_magick_geometry_tostring(geometryPtr)
	End Method
	
	Method Delete()
		If geometryPtr Then
			bmx_magick_geometry_free(geometryPtr)
			geometryPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TMBlob

	Field blobPtr:Byte Ptr

	Method New()
		_init_magick()
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
	_init_magick()
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
	bbdoc: Format is writable.
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
	bbdoc: Returns a TMCoderInfo corresponding to the named format.
	End Rem
	Function info:TMCoderInfo(format:String)
		_init_magick()
		Return TMCoderInfo(bmx_magick_coderinfo_info(format))
	End Function
	
End Type


' our pixmap loader :-)
Type TPixmapLoaderM Extends TPixmapLoader

	Method New()
	End Method

	Method LoadPixmap:TPixmap( stream:TStream )

		Local image:TMImage = TMImage.CreateFromStream(stream)

		If image Then
			Return image.GetPixmap()
		End If

		Return Null
	End Method
	
End Type


New TPixmapLoaderM
