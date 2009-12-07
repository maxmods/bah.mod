' Copyright (c) 2009 Bruce A Henderson
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
bbdoc: Image processing and analysis.
End Rem
Module BaH.Leptonica

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial release. (leptonica 1.63)"


Import "common.bmx"


Rem
bbdoc: 
End Rem
Type TLPIX

	Field pixPtr:Byte Ptr
	
	Function _create:TLPIX(pixPtr:Byte Ptr)
		If pixPtr Then
			Local this:TLPIX = New TLPIX
			this.pixPtr = pixPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Create:TLPIX(width:Int, height:Int, depth:Int, init:Int = True)
		Local this:TLPIX = New TLPIX
		
		If init Then
			this.pixPtr = pixCreate(width, height, depth)
		Else
			this.pixPtr = pixCreateNoInit(width, height, depth)
		End If
		
		If this.pixPtr Then
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function CreateWithPixmap:TLPIX(pixmap:TPixmap)
		Local this:TLPIX = New TLPIX

		Local depth:Int = BitsPerPixel[pixmap.format]
		If depth = 24 Then
			' upgrade to 32 (ARGB)
			pixmap = ConvertPixmap(pixmap, PF_RGBA8888)
		End If

		this.pixPtr = pixCreateNoInit(pixmap.width, pixmap.height, BitsPerPixel[pixmap.format])

		ConvertPixelsFromStdFormatToARGB(pixmap, pixGetData(this.pixPtr))

		Return this
	End Function

	Rem
	bbdoc: Creates a clone of the image.
	about: Because images can be large and hence expensive to copy, and extra handles to a data
         structure need to be made with a simple policy to avoid
         both double frees and memory leaks.  Pix are reference
         counted.  The side effect of Clone() is an increase
         by 1 in the ref count.
	End Rem
	Method Clone:TLPIX()
		Return TLPIX._create(pixClone(pixPtr))
	End Method

	Rem
	bbdoc: Creates a new copy of the image.
	End Rem
	Method Copy:TLPIX()
		Return TLPIX._create(pixCopy(Null, pixPtr))
	End Method

	Rem
	bbdoc: Returns a TPixmap representation of the image. 
	End Rem
	Method GetPixmap:TPixmap(static:Int = True)
		If static Then
			Return CreateStaticPixmap(pixGetData(pixPtr), pixGetWidth(pixPtr), pixGetHeight(pixPtr), pixGetWpl(pixPtr) * 4, _mapDepthToPixFormat(pixGetDepth(pixPtr)))
		End If
	End Method

	Rem
	bbdoc: 
	End Rem
	Method RenderLine:Int(x1:Int, y1:Int, x2:Int, y2:Int, width:Int, op:Int)
		Return pixRenderLine(pixPtr, x1, y1, x2, y2, width, op)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method RenderLineArb:Int(x1:Int, y1:Int, x2:Int, y2:Int, width:Int, rval:Int, gval:Int, bval:Int)
		Return bmx_pixRenderLineArb(pixPtr, x1, y1, x2, y2, width, rval, gval, bval)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method RenderLineBlend:Int(x1:Int, y1:Int, x2:Int, y2:Int, width:Int, rval:Int, gval:Int, bval:Int, fract:Float)
		Return bmx_pixRenderLineBlend(pixPtr, x1, y1, x2, y2, width, rval, gval, bval, fract)
	End Method

	Rem
	bbdoc: 
	about: @op should be one of #L_SET_PIXELS, #L_CLEAR_PIXELS or #L_FLIP_PIXELS.
	End Rem
	Method RenderBox:Int(box:TLBOX, width:Int, op:Int)
		Return pixRenderBox(pixPtr, box.boxPtr, width, op)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method RenderBoxArb:Int(box:TLBOX, width:Int, rval:Int, gval:Int, bval:Int)
		Return bmx_pixRenderBoxArb(pixPtr, box.boxPtr, width, rval, gval, bval)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method RenderBoxBlend:Int(box:TLBOX, width:Int, rval:Int, gval:Int, bval:Int, fract:Float)
		Return bmx_pixRenderBoxBlend(pixPtr, box.boxPtr, width, rval, gval, bval, fract)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method RenderHashBox:Int(box:TLBOX, spacing:Int, width:Int, orient:Int, outline:Int, op:Int)
		Return pixRenderHashBox(pixPtr, box.boxPtr, spacing, width, orient, outline, op)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method RenderHashBoxArb:Int(box:TLBOX, spacing:Int, width:Int, orient:Int, outline:Int, rval:Int, gval:Int, bval:Int)
		Return bmx_pixRenderHashBoxArb(pixPtr, box.boxPtr, spacing, width, orient, outline, rval, gval, bval)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method RenderHashBoxBlend:Int(box:TLBOX, spacing:Int, width:Int, orient:Int, outline:Int, rval:Int, gval:Int, bval:Int, fract:Float)
		Return bmx_pixRenderHashBoxBlend(pixPtr, box.boxPtr, spacing, width, orient, outline, rval, gval, bval, fract)
	End Method
	
	Rem
	bbdoc: Renders a closed contour.
	returns: 0 if OK, 1 on error
	about: Parameters:
	<ul>
	<li><b>ptas</b> : list of points.</li>
	<li><b>width</b> : thickness of the line.</li>
	<li><b>op</b> : one of L_SET_PIXELS, L_CLEAR_PIXELS or L_FLIP_PIXELS.</li>
	<li><b>closeFlag</b> : 1 to close the contour; 0 otherwise.</li>
	</ul>
	End Rem
	Method RenderPolyline:Int(ptas:TLPTA, width:Int, op:Int, closeFlag:Int)
		Return pixRenderPolyline(pixPtr, ptas.ptaPtr, width, op, closeFlag)
	End Method
	
	Rem
	bbdoc: 
	about: Parameters:
	<ul>
	<li><b>ptas</b> : list of points.</li>
	<li><b>width</b> : thickness of the line.</li>
	<li><b>rval</b> : red value.</li>
	<li><b>gval</b> : green value.</li>
	<li><b>bval</b> : blue value.</li>
	<li><b>closeFlag</b> : 1 to close the contour; 0 otherwise.</li>
	</ul>
	End Rem
	Method RenderPolylineArb:Int(ptas:TLPTA, width:Int, rval:Int, gval:Int, bval:Int, closeFlag:Int)
		Return bmx_pixRenderPolylineArb(pixPtr, ptas.ptaPtr, width, rval, gval, bval, closeFlag)
	End Method
	
	Rem
	bbdoc: 
	about: Parameters:
	<ul>
	<li><b>ptas</b> : list of points.</li>
	<li><b>width</b> : thickness of the line.</li>
	<li><b>rval</b> : red value.</li>
	<li><b>gval</b> : green value.</li>
	<li><b>bval</b> : blue value.</li>
	<li><b>fract</b> : in [0.0 - 1.0]; complete transparency (no effect) if 0.0; no transparency if 1.0.</li>
	<li><b>closeFlag</b> : 1 to close the contour; 0 otherwise.</li>
	<li><b>removeDups</b> : xxxxxxxxxxxxxx.</li>
	</ul>
	End Rem
	Method RenderPolylineBlend:Int(ptas:TLPTA, width:Int, rval:Int, gval:Int, bval:Int, fract:Float, closeFlag:Int, removeDups:Int)
		Return bmx_pixRenderPolylineBlend(pixPtr, ptas.ptaPtr, width, rval, gval, bval, fract, closeFlag, removeDups)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetData:Int(data:Byte Ptr)
		Return pixSetData(pixPtr, data)
	End Method
	
	Rem
	bbdoc: Returns the pix width.
	End Rem
	Method GetWidth:Int()
		Return pixGetWidth(pixPtr)
	End Method
	
	Rem
	bbdoc: Sets the pix width.
	End Rem
	Method SetWidth:Int(width:Int)
		Return pixSetWidth(pixPtr, width)
	End Method
	
	Rem
	bbdoc: Returns the pix height.
	End Rem
	Method GetHeight:Int()
		Return pixGetHeight(pixPtr)
	End Method
	
	Rem
	bbdoc: Sets the pix height.
	End Rem
	Method SetHeight:Int(height:Int)
		Return pixSetHeight(pixPtr, height)
	End Method
	
	Rem
	bbdoc: Returns the pix depth.
	End Rem
	Method GetDepth:Int()
		Return pixGetDepth(pixPtr)
	End Method
	
	Rem
	bbdoc: Sets the pix depth.
	End Rem
	Method SetDepth:Int(depth:Int)
		Return pixSetDepth(pixPtr, depth)
	End Method
	
	Rem
	bbdoc: Returns the pix dimensions (width, height and depth).
	End Rem
	Method GetDimensions:Int(pw:Int Var, ph:Int Var, pd:Int Var)
		Return pixGetDimensions(pixPtr, Varptr pw, Varptr ph, Varptr pd)
	End Method
	
	Rem
	bbdoc: Sets the pix dimensions.
	End Rem
	Method SetDimensions:Int(w:Int, h:Int, d:Int)
		Return pixSetDimensions(pixPtr, w, h, d)
	End Method
	
	Rem
	bbdoc: Copies the @source dimensions into the current pix.
	End Rem
	Method CopyDimensions:Int(source:TLPIX)
		Return pixCopyDimensions(pixPtr, source.pixPtr)
	End Method
	
	Rem
	bbdoc: Returns the "words per line".
	End Rem
	Method GetWpl:Int()
		Return pixGetWpl(pixPtr)
	End Method
	
	Rem
	bbdoc: Sets the "words per line".
	End Rem
	Method SetWpl:Int(wpl:Int)
		Return pixSetWpl(pixPtr, wpl)
	End Method
	
	Rem
	bbdoc: Returns the X resolution of the pix.
	End Rem
	Method GetXRes:Int()
		Return pixGetXRes(pixPtr)
	End Method
	
	Rem
	bbdoc: Sets the X resolution of the pix.
	End Rem
	Method SetXRes:Int(res:Int)
		Return pixSetXRes(pixPtr, res)
	End Method
	
	Rem
	bbdoc: Returns the Y resolution of the pix.
	End Rem
	Method GetYRes:Int()
		Return pixGetYRes(pixPtr)
	End Method
	
	Rem
	bbdoc: Sets the Y resolution of the pix.
	End Rem
	Method SetYRes:Int(res:Int)
		Return pixSetYRes(pixPtr, res)
	End Method
	
	Rem
	bbdoc: Returns the resolution of the pix.
	End Rem
	Method GetResolution:Int(pxRes:Int Var, pyRes:Int Var)
		Return pixGetResolution(pixPtr, Varptr pxRes, Varptr pyRes)
	End Method
	
	Rem
	bbdoc: Sets the resolution of the pix.
	End Rem
	Method SetResolution:Int(xRes:Int, yRes:Int)
		Return pixSetResolution(pixPtr, xRes, yRes)
	End Method
	
	Rem
	bbdoc: Copies the resolution from @source to the current pix.
	End Rem
	Method CopyResolution:Int(source:TLPIX)
		Return pixCopyResolution(pixPtr, source.pixPtr)
	End Method
	
	Rem
	bbdoc: Scales the resolution by @xScale, @yScale.
	End Rem
	Method ScaleResolution:Int(xScale:Float, yScale:Float)
		Return pixScaleResolution(pixPtr, xScale, yScale)
	End Method
	
	Rem
	bbdoc: Returns the text string belongs to the pix.
	End Rem
	Method GetText:String()
		Return String.fromCString(pixGetText(pixPtr))
	End Method
	
	Rem
	bbdoc: Sets the pix text.
	about: This removes any existing text.
	End Rem
	Method SetText:Int(text:String)
		Local s:Byte Ptr = text.toCString()
		Local ret:Int = pixSetText(pixPtr, s)
		MemFree s
		Return ret
	End Method
	
	Rem
	bbdoc: Adds @text to any existing text.
	End Rem
	Method AddText:Int(text:String)
		Local s:Byte Ptr = text.toCString()
		Local ret:Int = pixAddText(pixPtr, s)
		MemFree s
		Return ret
	End Method
	
	Rem
	bbdoc: Copies text from @source to the current pix.
	End Rem
	Method CopyText:Int(source:TLPIX)
		Return pixCopyText(pixPtr, source.pixPtr)
	End Method
	
	Method GetColorMap:TLPIXCMAP()
	' TODO
	End Method
	
	Method SetColorMap:Int(colormap:TLPIXCMAP)
	' TODO
	End Method
	
	Method DestroyColormap:Int()
	' TODO
	End Method
	
	Rem
	bbdoc: Adds an amount of grey to each pixel.
	returns: 0 if OK, 1 on error.
	about: 
	End Rem
	Method AddConstantGray:Int(value:Int)
		Return pixAddConstantGray(pixPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MultConstantGray:Int(value:Float)
		Return pixMultConstantGray(pixPtr, value)
	End Method
	
	Rem
	bbdoc: Grayscale threshold operation
	about: Notes:
	<ul>
	<li>if @setVal &gt; @threshVal, sets pixels with a value &gt;= @threshVal to @setVal</li>
	<li>if @setVal &lt; @threshVal, sets pixels with a value &lt;= @threshVal to @setVal</li>
	<li>if @setVal == @threshVal, no-op</li>
	</ul>
	End Rem
	Method ThresholdToValue(threshVal:Int, setVal:Int)
		pixThresholdToValue(pixPtr, pixPtr, threshVal, setVal)
	End Method
	
	Rem
	bbdoc: Image accumulator arithmetic operations
	returns: A 32 bpp TLPIX, or Null on error.
	about: Notes:
	<ul>
	<li>The offset must be &gt;= 0.</li>
	<li>The offset is used so that we can do arithmetic with negative number results on Int data; it prevents the Int data from going negative.</li>
	<li>Because we use Int intermediate data results, these should never exceed the max of Int ($7fffffff). We do not permit the offset to be
	above $40000000, which is half way between 0 and the max of Int.</li>
	<li>The same offset should be used for initialization, multiplication by a constant, and final extraction!</li>
	<li>If you're only adding positive values, offset can be 0.</li>
	</ul>

	End Rem
	Function InitAccumulate:TLPIX(w:Int, h:Int, offset:Int)
		Return TLPIX._create(pixInitAccumulate(w, h, offset))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method FinalAccumulate:TLPIX(offset:Int, depth:Int)
		Return TLPIX._create(pixFinalAccumulate(pixPtr, offset, depth))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method FinalAccumulateThreshold:TLPIX(offset:Int, threshold:Int)
		Return TLPIX._create(pixFinalAccumulateThreshold(pixPtr, offset, threshold))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Accumulate:Int(source:TLPIX, op:Int)
		Return pixAccumulate(pixPtr, source.pixPtr, op)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MultConstAccumulate:Int(factor:Float, offset:Int)
		Return pixMultConstAccumulate(pixPtr, factor, offset)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AbsDifference:TLPIX(pix:TLPIX)
		Return TLPIX._create(pixAbsDifference(pixPtr, pix.pixPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MinOrMax:TLPIX(pix:TLPIX, minMax:Int)
		Return TLPIX._create(pixMinOrMax(Null, pixPtr, pix.pixPtr, minMax))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MaxDynamicRange:TLPIX(scaleType:Int)
		Return TLPIX._create(pixMaxDynamicRange(pixPtr, scaleType))
	End Method
	
	Rem
	bbdoc: CC-preserving thinning.
	End Rem
	Method Thin:TLPIX(thinType:Int, connectivity:Int, maxIters:Int)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UnsharpMasking:TLPIX(halfWidth:Int, fract:Float)
		Return TLPIX._create(pixUnsharpMasking(pixPtr, halfWidth, fract))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UnsharpMaskingGray:TLPIX(halfWidth:Int, fract:Float)
		Return TLPIX._create(pixUnsharpMaskingGray(pixPtr, halfWidth, fract))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UnsharpMaskingFast:TLPIX(halfWidth:Int, fract:Float, direction:Int)
		Return TLPIX._create(pixUnsharpMaskingFast(pixPtr, halfWidth, fract, direction))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UnsharpMaskingGrayFast:TLPIX(halfWidth:Int, fract:Float, direction:Int)
		Return TLPIX._create(pixUnsharpMaskingGrayFast(pixPtr, halfWidth, fract, direction))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UnsharpMaskingGray1D:TLPIX(halfWidth:Int, fract:Float, direction:Int)
		Return TLPIX._create(pixUnsharpMaskingGray1D(pixPtr, halfWidth, fract, direction))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UnsharpMaskingGray2D:TLPIX(halfWidth:Int, fract:Float)
		Return TLPIX._create(pixUnsharpMaskingGray2D(pixPtr, halfWidth, fract))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ModifyHue(fract:Float)
		pixModifyHue(pixPtr, pixPtr, fract)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ModifySaturation(fract:Float)
		pixModifySaturation(pixPtr, pixPtr, fract)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MeasureSaturation:Int(factor:Int, psat:Float Var)
		Return pixMeasureSaturation(pixPtr, factor, Varptr psat)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MultConstantColor:TLPIX(rFact:Float, gFact:Float, bFact:Float)
		Return TLPIX._create(pixMultConstantColor(pixPtr, rFact, gFact, bFact))
	End Method
	
	Method MultMatrixColor:TLPIX(kel:TLKERNEL)
	' TODO
'		Return TLPIX._create(pixMultMatrixColor(
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method HalfEdgeByBandpass:TLPIX(sm1h:Int, sm1v:Int, sm2h:Int, sm2v:Int)
		Return TLPIX._create(pixHalfEdgeByBandpass(pixPtr, sm1h, sm1v, sm2h, sm2v))
	End Method
	
	Rem
	bbdoc: Top-level grayscale or color block convolution
	End Rem
	Method Blockconv:TLPIX(wc:Int, hc:Int)
		Return TLPIX._create(pixBlockconv(pixPtr, wc, hc))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method BlockconvGray:TLPIX(accum:TLPIX, wc:Int, hc:Int)
		If accum Then
			Return TLPIX._create(pixBlockconvGray(pixPtr, accum.pixPtr, wc, hc))
		Else
			Return TLPIX._create(pixBlockconvGray(pixPtr, Null, wc, hc))
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method BlockconvAccum:TLPIX()
		Return TLPIX._create(pixBlockconvAccum(pixPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method BlockconvGrayUnnormalized:TLPIX(wc:Int, hc:Int)
		Return TLPIX._create(pixBlockconvGrayUnnormalized(pixPtr, wc, hc))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method BlockconvTiled:TLPIX(wc:Int, hc:Int, nx:Int, ny:Int)
		Return TLPIX._create(pixBlockconvTiled(pixPTr, wc, hc, nx, ny))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method BlockconvGrayTile:TLPIX(accum:TLPIX, wc:Int, hc:Int)
		Return TLPIX._create(pixBlockconvGrayTile(pixPtr, accum.pixPtr, wc, hc))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method WindowedMean:TLPIX(wc:Int, hc:Int, normFlag:Int)
		Return TLPIX._create(pixWindowedMean(pixPtr, wc, hc, normFlag))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method WindowedMeanSquare:TLPIX(size:Int)
		Return TLPIX._create(pixWindowedMeanSquare(pixPtr, size))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MeanSquareAccum:TLPIX()
		Return TLPIX._create(pixMeanSquareAccum(pixPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Blockrank:TLPIX(accum:TLPIX, wc:Int, hc:Int, rank:Float)
		If accum Then
			Return TLPIX._create(pixBlockrank(pixPtr, accum.pixPtr, wc, hc, rank))
		Else
			Return TLPIX._create(pixBlockrank(pixPtr, Null, wc, hc, rank))
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Blocksum:TLPIX(accum:TLPIX, wc:Int, hc:Int)
		If accum Then
			Return TLPIX._create(pixBlocksum(pixPtr, accum.pixPtr, wc, hc))
		Else
			Return TLPIX._create(pixBlocksum(pixPtr, Null, wc, hc))
		End If
	End Method
	
	Rem
	bbdoc: Census transform.
	End Rem
	Method CensusTransform:TLPIX(halfSize:Int, accum:TLPIX = Null)
		If accum Then
			Return TLPIX._create(pixCensusTransform(pixPtr, halfSize, accum.pixPtr))
		Else
			Return TLPIX._create(pixCensusTransform(pixPtr, halfSize, Null))
		End If
	End Method
	
	Rem
	bbdoc: Convolution with an arbitrary kernel.
	returns: A new TLPIX (8, 16 or 32 bpp).
	about: Notes:
	<ul>
	<li>This gives a convolution with an arbitrary kernel.</li>
	<li>The parameter @outdepth determines the depth of the result.</li>
	<li>If normflag == 1, the result is normalized by scaling all kernel values for a unit sum.  Do not normalize if
          the kernel has null sum, such as a DoG.</li>
	<li>If the kernel is normalized to unit sum, the output values can never exceed 255, so an output depth of 8 bpp is sufficient.
          If the kernel is not normalized, it may be necessary to use 16 or 32 bpp output to avoid overflow.</li>
	<li>The kernel values can be positive or negative, but the result for the convolution can only be stored as a positive
          number.  Consequently, if it goes negative, the choices are to clip to 0 or take the absolute value.  We're choosing
          the former for now.  Another possibility would be to output a second unsigned image for the negative values.</li>
	<li>This uses a mirrored border to avoid special casing on the boundaries.</li>
	<li>The function is slow, running at about 12 machine cycles for each pixel-op in the convolution.  For example, with a 3 GHz
          cpu, a 1 Mpixel grayscale image, and a kernel with (sx * sy) = 25 elements, the convolution takes about 100 msec.
	End Rem
	Method Convolve:TLPIX(kel:TLKERNEL, outDepth:Int, normFlag:Int)
		Return TLPIX._create(pixConvolve(pixPtr, kel.kernelPtr, outDepth, normFlag))
	End Method
	
	Rem
	bbdoc: Convolution with a separable kernel
	returns: A new TLPIX (8, 16 or 32 bpp).
	about: Notes:
	<ul>
	<li>This does a convolution with a separable kernel that is is a sequence of convolutions in x and y.  The two
		one-dimensional kernel components must be input separately; the full kernel is the product of these components.
          The support for the full kernel is thus a rectangular region.</li>
	<li>The @outDepth and @normFlag parameters are used as in #Convolve().</li>
	<li>If the kernel is normalized to unit sum, the output values can never exceed 255, so an output depth of 8 bpp is sufficient.
          If the kernel is not normalized, it may be necessary to use 16 or 32 bpp output to avoid overflow.</li>
	<li>The kernel values can be positive or negative, but the result for the convolution can only be stored as a positive
          number.  Consequently, if it goes negative, the choices are to clip to 0 or take the absolute value.  We're choosing
          the former for now.  Another possibility would be to output a second unsigned image for the negative values.</li>
	<li>This uses mirrored borders to avoid special casing on the boundaries.</li>
	</ul>
	End Rem
	Method ConvolveSep:TLPIX(kelX:TLKERNEL, kelY:TLKERNEL, outDepth:Int, normFlag:Int)
		Return TLPIX._create(pixConvolveSep(pixPtr, kelX.kernelPtr, kelY.kernelPtr, outDepth, normFlag))
	End Method
	
	Rem
	bbdoc: General rotation about the center.
	returns: The rotated pix, or Null on error.
	about: Notes:
	<ul>
	<li>Rotation is about the center of the image.</li>
	<li>For very small rotations, just return a clone.</li>
	<li>Rotation brings either white or black pixels in from outside the image.</li>
	<li>Above 20 degrees, if rotation by shear is requested, we rotate by sampling.</li>
	<li>Colormaps are removed for rotation by area map and shear.</li>
	<li>The dest can be expanded so that no image pixels are lost.  To invoke expansion, input the original
          width and height.  For repeated rotation, use of the original width and height allows the expansion to
          stop at the maximum required size, which is a square with side = sqrt(w*w + h*h).</li>
	</ul>
	<p>Parameters:
	<ul>
	<li><b>angle</b> : degrees; clockwise is positive.</li>
	<li><b>rotateType </b> : one of L_ROTATE_AREA_MAP, L_ROTATE_SHEAR or L_ROTATE_SAMPLING.</li>
	<li><b>inColor</b> : L_BRING_IN_WHITE or L_BRING_IN_BLACK.</li>
	<li><b>width</b> : original width; use 0 to avoid embedding.</li>
	<li><b>height</b> : original height; use 0 to avoid embedding.</li>
	</ul>
	</p>

	End Rem
	Method Rotate:TLPIX(angle:Float, rotateType:Int, inColor:Int, width:Int, height:Int)
		Local radians:Float = angle * 0.0174532925
		Return TLPIX._create(pixRotate(pixPtr, radians, rotateType, inColor, width, height))
	End Method
	
	Rem
	bbdoc: General rotation about the center.
	about: Notes:
	<ul>
	<li>For very small rotations, just return a clone.</li>
	<li>Generate larger image to embed pixs if necessary, and place in the center.</li>
	<li>Rotation brings either white or black pixels in from outside the image.  For colormapped images where
          there is no white or black, a new color is added if possible for these pixels; otherwise, either the
          lightest or darkest color is used.  In most cases, the colormap will be removed prior to rotation.</li>
	<li>The dest is to be expanded so that no image pixels are lost after rotation.  Input of the original width
          and height allows the expansion to stop at the maximum required size, which is a square with side equal to sqrt(w*w + h*h).</li>
	<li>Let theta be atan(w/h).  Then the height after rotation  cannot increase by a factor more than
	<pre>cos(theta - |angle|)</pre>
          whereas the width after rotation cannot increase by a factor more than 
     <pre>sin(theta + |angle|)</pre>
          These must be clipped to the maximal side, and additionally, we don't allow either the width or height to decrease.</li>
	</ul>
	<p>Parameters:
	<ul>
	<li><b>angle</b> : degrees; clockwise is positive.</li>
	<li><b>inColor</b> : L_BRING_IN_WHITE or L_BRING_IN_BLACK.</li>
	<li><b>width</b> : original width; use 0 to avoid embedding.</li>
	<li><b>height</b> : original height; use 0 to avoid embedding.</li>
	</ul>
	</p>
	End Rem
	Method EmbedForRotation:TLPIX(angle:Float, inColor:Int, width:Int, height:Int)
		Local radians:Float = angle * 0.0174532925
		Return TLPIX._create(pixEmbedForRotation(pixPtr, radians, inColor, width, height))
	End Method
	
	Rem
	bbdoc: General rotation by sampling.
	about: Notes:
	<ul>
	<li>For very small rotations, just return a clone.</li>
	<li>Rotation brings either white or black pixels in from outside the image.</li>
	<li>Colormaps are retained.</li>
	</ul>
	<p>Parameters:
	<ul>
	<li><b>xCen</b> : x value of center of rotation.</li>
	<li><b>yCen</b> : y value of center of rotation.</li>
	<li><b>angle</b> : degrees; clockwise is positive.</li>
	<li><b>inColor</b> : L_BRING_IN_WHITE or L_BRING_IN_BLACK.</li>
	</ul>
	</p>	
	End Rem
	Method RotateBySampling:TLPIX(xCen:Int, yCen:Int, angle:Float, inColor:Int)
		Local radians:Float = angle * 0.0174532925
		Return TLPIX._create(pixRotateBySampling(pixPtr, xCen, yCen, radians, inColor))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If pixPtr Then
			bmx_pixFree(pixPtr)
			pixPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type


Type TLBOX

	Field boxPtr:Byte Ptr
	
	Function _create:TLBOX(boxPtr:Byte Ptr)
		If boxPtr Then
			Local this:TLBOX = New TLBOX
			this.boxPtr = boxPtr
			Return this
		End If
	End Function

	Function Create:TLBOX(x:Int, y:Int, w:Int, h:Int)
		Local this:TLBOX = New TLBOX
		this.boxPtr = boxCreate(x, y, w, h)
		Return this
	End Function

	Method GetGeometry:Int(x:Int Var, y:Int Var, w:Int Var, h:Int Var)
	End Method
	
	Method SetGeometry:Int(x:Int, y:Int, w:Int, h:Int)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If boxPtr Then
			bmx_boxFree(boxPtr)
			boxPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type


Rem
bbdoc: An array of points.
End Rem
Type TLPTA

	Field ptaPtr:Byte Ptr
	
	Function _create:TLPTA(ptaPtr:Byte Ptr)
		If ptaPtr Then
			Local this:TLPTA = New TLPTA
			this.ptaPtr = ptaPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Create:TLPTA(size:Int)
		Local this:TLPTA = New TLPTA
		this.ptaPtr = ptaCreate(size)
		Return this
	End Function

	Rem
	bbdoc: 
	End Rem
	Method AddPt:Int(x:Float, y:Float)
		Return ptaAddPt(ptaPtr, x, y)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ExtendArrays:Int()
		Return ptaExtendArrays(ptaPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Join:Int(source:TLPTA, iStart:Int, iEnd:Int)
		Return ptaJoin(ptaPtr, source.ptaPtr, iStart, iEnd)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Reverse:TLPTA(pointType:Int)
		Return TLPTA._create(ptaReverse(ptaPtr, pointType))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CyclicPerm:TLPTA(xs:Int, ys:Int)
		Return TLPTA._create(ptaCyclicPerm(ptaPtr, xs, ys))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Sort:TLPTA(sortType:Int, sortOrder:Int)
		Return TLPTA._create(ptaSort(ptaPtr, sortType, sortOrder, Null))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method RemoveDuplicates:TLPTA(factor:Int)
		Return TLPTA._create(ptaRemoveDuplicates(ptaPtr, factor))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetCount:Int()
		Return ptaGetCount(ptaPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPt:Int(index:Int, px:Float Var, py:Float Var)
		Return ptaGetPt(ptaPtr, index, Varptr px, Varptr py)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetIPt:Int(index:Int, px:Int Var, py:Int Var)
		Return ptaGetIPt(ptaPtr, index, Varptr px, Varptr py)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetExtent:TLBOX()
		Return TLBOX._create(ptaGetExtent(ptaPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetInsideBox:TLPTA(box:TLBOX)
		Return TLPTA._create(ptaGetInsideBox(ptaPtr, box.boxPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method FindCornerPixels:TLPTA()
		Return TLPTA._create(pixFindCornerPixels(ptaPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ContainsPt:Int(x:Int, y:Int)
		Return ptaContainsPt(ptaPtr, x, y)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TestIntersection:Int(pta:TLPTA)
		Return ptaTestIntersection(ptaPtr, pta.ptaPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Transform:TLPTA(shiftX:Int, shiftY:Int, scaleX:Float, scaleY:Float)
		Return TLPTA._create(ptaTransform(ptaPtr, shiftX, shiftY, scaleX, scaleY))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Subsample:TLPTA(subFactor:Int)
		Return TLPTA._create(ptaSubsample(ptaPtr, subFactor))
	End Method
	
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If ptaPtr Then
			bmx_ptaFree(ptaPtr)
			ptaPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Type TLPIXCMAP

	Field cmapPtr:Byte Ptr
	

End Type

Rem
bbdoc: Basic operations on kernels for image convolution.
End Rem
Type TLKERNEL

	Field kernelPtr:Byte Ptr

	Function _create:TLKERNEL(kernelPtr:Byte Ptr)
		If kernelPtr Then
			Local this:TLKERNEL = New TLKERNEL
			this.kernelPtr = kernelPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Create:TLKERNEL(height:Int, width:Int)
		Local this:TLKERNEL = New TLKERNEL
		this.kernelPtr = kernelCreate(height, width)
		Return this
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Copy:TLKERNEL()
		Return TLKERNEL._create(kernelCopy(kernelPtr))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetElement:Int(row:Int, col:Int, value:Float Var)
		Return kernelGetElement(kernelPtr, row, col, Varptr value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetElement:Int(row:Int, col:Int, value:Float)
		Return kernelSetElement(kernelPtr, row, col, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetParameters:Int(psy:Int Var, psx:Int Var, pcy:Int Var, pcx:Int Var)
		Return kernelGetParameters(kernelPtr, Varptr psy, Varptr psx, Varptr pcy, Varptr pcx)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetOrigin:Int(cy:Int, cx:Int)
		Return kernelSetOrigin(kernelPtr, cy, cx)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetSum:Int(sum:Float Var)
		Return kernelGetSum(kernelPtr, Varptr sum)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMinMax:Int(pMin:Float Var, pMax:Float Var)
		Return kernelGetMinMax(kernelPtr, Varptr pMin, Varptr pMax)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Normalize:TLKERNEL(normSum:Float)
		Return TLKERNEL._create(kernelNormalize(kernelPtr, normSum))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Invert:TLKERNEL()
		Return TLKERNEL._create(kernelInvert(kernelPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CreateFromString:TLKERNEL(h:Int, w:Int, cy:Int, cx:Int, data:String)
		Local s:Byte Ptr = data.ToCString()
		Local kernel:TLKERNEL = TLKERNEL._create(kernelCreateFromString(h, w, cy, cx, s))
		MemFree s
		Return kernel
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function CreateFromPix:TLKERNEL(pix:TLPIX, cy:Int, cx:Int)
		Return TLKERNEL._create(kernelCreateFromPix(pix.pixPtr, cy, cx))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method DisplayInPix:TLPIX(size:Int, thickness:Int)
		Return TLPIX._create(kernelDisplayInPix(kernelPtr, size, thickness))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function MakeGaussianKernel:TLKERNEL(halfHeight:Int, halfWidth:Int, stdev:Float, _max:Float)
		Return TLKERNEL._create(makeGaussianKernel(halfHeight, halfWidth, stdev, _max))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function MakeDoKernel:TLKERNEL(halfHeight:Int, halfWidth:Int, stdev:Float, ratio:Float)
		Return TLKERNEL._create(makeDoGKernel(halfHeight, halfWidth, stdev, ratio))
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If kernelPtr Then
			bmx_kernelFree(kernelPtr)
			kernelPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

