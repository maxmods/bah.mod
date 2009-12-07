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

Import BRL.Pixmap

Import "source.bmx"


Extern
	' PIX
	Function pixCreate:Byte Ptr(width:Int, height:Int, depth:Int)
	Function pixCreateNoInit:Byte Ptr(width:Int, height:Int, depth:Int)

	Function pixGetData:Byte Ptr(handle:Byte Ptr)
	Function pixGetWidth:Int(handle:Byte Ptr)
	Function pixGetHeight:Int(handle:Byte Ptr)
	Function pixGetWpl:Int(handle:Byte Ptr)
	Function pixGetDepth:Int(handle:Byte Ptr)
	Function pixSetData:Int(handle:Byte Ptr, data:Byte Ptr)
	Function pixCopy:Byte Ptr(dest:Byte Ptr, source:Byte Ptr)
	Function pixClone:Byte Ptr(handle:Byte Ptr)
	Function pixRenderBox:Int(handle:Byte Ptr, box:Byte Ptr, width:Int, op:Int)
	Function pixRenderHashBox:Int(handle:Byte Ptr, box:Byte Ptr, spacing:Int, width:Int, orient:Int, outline:Int, op:Int)
	Function pixRenderLine:Int(handle:Byte Ptr, x1:Int, y1:Int, x2:Int, y2:Int, width:Int, op:Int)
	Function pixSetWidth:Int(handle:Byte Ptr, width:Int)
	Function pixSetHeight:Int(handle:Byte Ptr, height:Int)
	Function pixSetDepth:Int(handle:Byte Ptr, depth:Int)
	Function pixGetDimensions:Int(handle:Byte Ptr, pw:Int Ptr, ph:Int Ptr, pd:Int Ptr)
	Function pixSetDimensions:Int(handle:Byte Ptr, w:Int, h:Int, d:Int)
	Function pixCopyDimensions:Int(handle:Byte Ptr, source:Byte Ptr)
	Function pixSetWpl:Int(handle:Byte Ptr, wpl:Int)
	Function pixGetXRes:Int(handle:Byte Ptr)
	Function pixSetXRes:Int(handle:Byte Ptr, res:Int)
	Function pixGetYRes:Int(handle:Byte Ptr)
	Function pixSetYRes:Int(handle:Byte Ptr, res:Int)
	Function pixGetResolution:Int(handle:Byte Ptr, pxRes:Int Ptr, pyRes:Int Ptr)
	Function pixSetResolution:Int(handle:Byte Ptr, xRes:Int, yRes:Int)
	Function pixCopyResolution:Int(handle:Byte Ptr, source:Byte Ptr)
	Function pixScaleResolution:Int(handle:Byte Ptr, xScale:Float, yScale:Float)
	Function pixGetText:Byte Ptr(handle:Byte Ptr)
	Function pixSetText:Int(handle:Byte Ptr, s:Byte Ptr)
	Function pixAddText:Int(handle:Byte Ptr, s:Byte Ptr)
	Function pixCopyText:Int(handle:Byte Ptr, source:Byte Ptr)
	Function pixAddConstantGray:Int(handle:Byte Ptr, value:Int)
	Function pixMultConstantGray:Int(handle:Byte Ptr, value:Float)
	Function pixThresholdToValue:Byte Ptr(dest:Byte Ptr, source:Byte Ptr, threshVal:Int, setVal:Int)
	Function pixInitAccumulate:Byte Ptr(w:Int, h:Int, offset:Int)
	Function pixFinalAccumulate:Byte Ptr(handle:Byte Ptr, offset:Int, depth:Int)
	Function pixFinalAccumulateThreshold:Byte Ptr(handle:Byte Ptr, offset:Int, threshold:Int)
	Function pixAccumulate(handle:Byte Ptr, source:Byte Ptr, op:Int)
	Function pixMultConstAccumulate:Int(handle:Byte Ptr, factor:Float, offset:Int)
	Function pixAbsDifference:Byte Ptr(handle:Byte Ptr, pix:Byte Ptr)
	Function pixMinOrMax:Byte Ptr(pix:Byte Ptr, pix1:Byte Ptr, pix2:Byte Ptr, minMax:Int)
	Function pixMaxDynamicRange:Byte Ptr(handle:Byte Ptr, scaleType:Int)

	Function pixUnsharpMasking:Byte Ptr(handle:Byte Ptr, halfWidth:Int, fract:Float)
	Function pixUnsharpMaskingGray:Byte Ptr(handle:Byte Ptr, halfWidth:Int, fract:Float)
	Function pixUnsharpMaskingFast:Byte Ptr(handle:Byte Ptr, halfWidth:Int, fract:Float, direction:Int)
	Function pixUnsharpMaskingGrayFast:Byte Ptr(handle:Byte Ptr, halfWidth:Int, fract:Float, direction:Int)
	Function pixUnsharpMaskingGray1D:Byte Ptr(handle:Byte Ptr, halfWidth:Int, fract:Float, direction:Int)
	Function pixUnsharpMaskingGray2D:Byte Ptr(handle:Byte Ptr, halfWidth:Int, fract:Float)
	Function pixModifyHue:Byte Ptr(dest:Byte Ptr, source:Byte Ptr, fract:Float)
	Function pixModifySaturation:Byte Ptr(dest:Byte Ptr, source:Byte Ptr, fract:Float)
	Function pixMeasureSaturation:Int(handle:Byte Ptr, factor:Int, psat:Float Ptr)
	Function pixMultConstantColor:Byte Ptr(handle:Byte Ptr, rFact:Float, gFact:Float, bFact:Float)
	Function pixHalfEdgeByBandpass:Byte Ptr(handle:Byte Ptr, sm1h:Int, sm1v:Int, sm2h:Int, sm2v:Int)
	Function pixBlockconvGray:Byte Ptr(handle:Byte Ptr, accum:Byte Ptr, wc:Int, hc:Int)
	Function pixBlockconvAccum:Byte Ptr(handle:Byte Ptr)
	Function pixBlockconv:Byte Ptr(handle:Byte Ptr, wc:Int, hc:Int)
	Function pixBlockconvGrayUnnormalized:Byte Ptr(handle:Byte Ptr, wc:Int, hc:Int)
	Function pixBlockconvTiled:Byte Ptr(handle:Byte Ptr, wc:Int, hc:Int, nx:Int, ny:Int)
	Function pixBlockconvGrayTile:Byte Ptr(handle:Byte Ptr, accum:Byte Ptr, wc:Int, hc:Int)
	Function pixWindowedMean:Byte Ptr(handle:Byte Ptr, wc:Int, hc:Int, normFlag:Int)
	Function pixWindowedMeanSquare:Byte Ptr(handle:Byte Ptr, size:Int)
	Function pixMeanSquareAccum:Byte Ptr(handle:Byte Ptr)
	Function pixBlockrank:Byte Ptr(handle:Byte Ptr, accum:Byte Ptr, wc:Int, hc:Int, rank:Float)
	Function pixBlocksum:Byte Ptr(handle:Byte Ptr, accum:Byte Ptr, wc:Int, hc:Int)
	Function pixCensusTransform:Byte Ptr(handle:Byte Ptr, halfSize:Int, accum:Byte Ptr)
	Function pixConvolve:Byte Ptr(handle:Byte Ptr, kel:Byte Ptr, outDepth:Int, normFlag:Int)
	Function pixConvolveSep:Byte Ptr(handle:Byte Ptr, kelX:Byte Ptr, kelY:Byte Ptr, outDepth:Int, normFlag:Int)
	Function pixRenderPolyline:Int(handle:Byte Ptr, ptas:Byte Ptr, width:Int, op:Int, closeFlag:Int)

	Function pixRotate:Byte Ptr(handle:Byte Ptr, angle:Float, rotateType:Int, inColor:Int, width:Int, height:Int)
	Function pixEmbedForRotation:Byte Ptr(handle:Byte Ptr, angle:Float, inColor:Int, width:Int, height:Int)
	Function pixRotateBySampling:Byte Ptr(handle:Byte Ptr, xCen:Int, yCen:Int, radians:Float, inColor:Int)

	Function bmx_pixRenderBoxArb:Int(handle:Byte Ptr, box:Byte Ptr, width:Int, rval:Int, gval:Int, bval:Int)
	Function bmx_pixRenderBoxBlend:Int(handle:Byte Ptr, box:Byte Ptr, width:Int, rval:Int, gval:Int, bval:Int, fract:Float)
	Function bmx_pixRenderHashBoxArb:Int(handle:Byte Ptr, box:Byte Ptr, spacing:Int, width:Int, orient:Int, outline:Int, rval:Int, gval:Int, bval:Int)
	Function bmx_pixRenderHashBoxBlend:Int(handle:Byte Ptr, box:Byte Ptr, spacing:Int, width:Int, orient:Int, outline:Int, rval:Int, gval:Int, bval:Int, fract:Float)
	Function bmx_pixRenderLineArb:Int(handle:Byte Ptr, x1:Int, y1:Int, x2:Int, y2:Int, width:Int, rval:Int, gval:Int, bval:Int)
	Function bmx_pixRenderLineBlend:Int(handle:Byte Ptr, x1:Int, y1:Int, x2:Int, y2:Int, width:Int, rval:Int, gval:Int, bval:Int, fract:Float)
	Function bmx_pixRenderPolylineArb:Int(handle:Byte Ptr, ptas:Byte Ptr, width:Int, rval:Int, gval:Int, bval:Int, closeFlag:Int)
	Function bmx_pixRenderPolylineBlend:Int(handle:Byte Ptr, ptas:Byte Ptr, width:Int, rval:Int, gval:Int, bval:Int, fract:Float, closeFlag:Int, removeDups:Int)

	Function bmx_pixFree(handle:Byte Ptr)

	Function boxCreate:Byte Ptr(x:Int, y:Int, w:Int, h:Int)
	Function bmx_boxFree(handle:Byte Ptr)

	Function ptaCreate:Byte Ptr(size:Int)
	Function ptaAddPt(handle:Byte Ptr, x:Float, y:Float)
	Function ptaExtendArrays:Int(handle:Byte Ptr)
	Function ptaJoin:Int(handle:Byte Ptr, source:Byte Ptr, iStart:Int, iEnd:Int)
	Function ptaReverse:Byte Ptr(handle:Byte Ptr, pointType:Int)
	Function ptaCyclicPerm:Byte Ptr(handle:Byte Ptr, xs:Int, ys:Int)
	Function ptaSort:Byte Ptr(handle:Byte Ptr, sortType:Int, sortOrder:Int, arr:Byte Ptr Ptr)
	Function ptaRemoveDuplicates:Byte Ptr(handle:Byte Ptr, factor:Int)
	Function ptaGetCount:Int(handle:Byte Ptr)
	Function ptaGetPt:Int(handle:Byte Ptr, index:Int, px:Float Ptr, py:Float Ptr)
	Function ptaGetIPt:Int(handle:Byte Ptr, index:Int, px:Int Ptr, py:Int Ptr)
	Function ptaGetExtent:Byte Ptr(handle:Byte Ptr)
	Function ptaGetInsideBox:Byte Ptr(handle:Byte Ptr, box:Byte Ptr)
	Function pixFindCornerPixels:Byte Ptr(handle:Byte Ptr)
	Function ptaContainsPt:Int(handle:Byte Ptr, x:Int, y:Int)
	Function ptaTestIntersection:Int(handle:Byte Ptr, pta:Byte Ptr)
	Function ptaTransform:Byte Ptr(handle:Byte Ptr, shiftX:Int, shiftY:Int, scaleX:Float, scaleY:Float)
	Function ptaSubsample:Byte Ptr(handle:Byte Ptr, subFactor:Int)
	
	Function bmx_ptaFree(handle:Byte Ptr)

	Function kernelCreate:Byte Ptr(height:Int, width:Int)
	Function bmx_kernelFree(handle:Byte Ptr)
	Function kernelCopy:Byte Ptr(handle:Byte Ptr)
	Function kernelGetElement:Int(handle:Byte Ptr, row:Int, col:Int, value:Float Ptr)
	Function kernelSetElement:Int(handle:Byte Ptr, row:Int, col:Int, value:Float)
	Function kernelGetParameters:Int(handle:Byte Ptr, psy:Int Ptr, psx:Int Ptr, pcy:Int Ptr, pcx:Int Ptr)
	Function kernelSetOrigin:Int(handle:Byte Ptr, cy:Int, cx:Int)
	Function kernelGetSum:Int(handle:Byte Ptr, sum:Float Ptr)
	Function kernelGetMinMax:Int(handle:Byte Ptr, pMin:Float Ptr, pMax:Float Ptr)
	Function kernelNormalize:Byte Ptr(handle:Byte Ptr, normSum:Float)
	Function kernelInvert:Byte Ptr(handle:Byte Ptr)
	Function kernelCreateFromString:Byte Ptr(h:Int, w:Int, cy:Int, cx:Int, s:Byte Ptr)
	Function kernelCreateFromPix:Byte Ptr(pix:Byte Ptr, cy:Int, cx:Int)
	Function kernelDisplayInPix:Byte Ptr(handle:Byte Ptr, size:Int, thickness:Int)
	Function makeGaussianKernel:Byte Ptr(halfHeight:Int, halfWidth:Int, stdev:Float, _max:Float)
	Function makeDoGKernel:Byte Ptr(halfHeight:Int, halfWidth:Int, stdev:Float, ratio:Float)

End Extern


Function _mapDepthToPixFormat:Int(depth:Int)
	Select depth
		Case 8
			Return PF_I8
		Case 24
			Return PF_RGB888
		Case 32
			Return PF_BGRA8888
	End Select
	Return 0	
End Function

Function ConvertPixelsFromStdFormatToARGB( pixmap:TPixmap,out_buf:Byte Ptr )

	Local count:Int = pixmap.width
	Local pitch:Int = pixmap.pitch

	For Local y:Int = 0 Until pixmap.height

		Local out:Byte Ptr = out_buf + y * pitch
		Local in:Byte Ptr = pixmap.pixels + y * pitch
		Local in_end:Byte Ptr = in + count * 4
	
		While in<>in_end
			out[0]=in[2]
			out[1]=in[1]
			out[2]=in[0]
			out[3]=in[3]
			in:+4;out:+4
		Wend

	Next

End Function




Rem
bbdoc: Set all bits in each pixel to 1.
End Rem
Const L_SET_PIXELS:Int = 1
Rem
bbdoc: Set all bits in each pixel to 0.
End Rem
Const L_CLEAR_PIXELS:Int = 2
Rem
bbdoc: Flip all bits in each pixel.
End Rem
Const L_FLIP_PIXELS:Int = 3

Rem
bbdoc: Sort in increasing order.
End Rem
Const L_SORT_INCREASING:Int = 1
Rem
bbdoc: Sort in decreasing order.
End Rem
Const L_SORT_DECREASING:Int = 2
Rem
bbdoc: Sort box or c.c. by horiz location     
End Rem
Const L_SORT_BY_X:Int = 3
Rem
bbdoc: Sort box or c.c. by vert location      
end rem
Const L_SORT_BY_Y:Int = 4
Rem
bbdoc: Sort box or c.c. by width              
end rem
Const L_SORT_BY_WIDTH:Int = 5
Rem
bbdoc: Sort box or c.c. by height             
end rem
Const L_SORT_BY_HEIGHT:Int = 6
Rem
bbdoc: Sort box or c.c. by min dimension      
end rem
Const L_SORT_BY_MIN_DIMENSION:Int = 7
Rem
bbdoc: Sort box or c.c. by max dimension      
end rem
Const L_SORT_BY_MAX_DIMENSION:Int = 8
Rem
bbdoc: Sort box or c.c. by perimeter          
end rem
Const L_SORT_BY_PERIMETER:Int = 9
Rem
bbdoc: Sort box or c.c. by area               
end rem
Const L_SORT_BY_AREA:Int = 10
Rem
bbdoc: Sort box or c.c. by width/height ratio
end rem
Const L_SORT_BY_ASPECT_RATIO:Int = 11


Rem
bbdoc: Add some of src inverse to itself.
end rem
Const L_BLEND_WITH_INVERSE:Int = 1
Rem
bbdoc: Shift src colors towards white.
end rem
Const L_BLEND_TO_WHITE:Int = 2
Rem
bbdoc: Shift src colors towards black.
end rem
Const L_BLEND_TO_BLACK:Int = 3
Rem
bbdoc: Blend src directly with blender.
end rem
Const L_BLEND_GRAY:Int = 4
Rem
bbdoc: Add amount of src inverse to itself, based on blender pix value.
end rem
Const L_BLEND_GRAY_WITH_INVERSE:Int = 5

Rem
bbdoc: Use area map rotation, if possible
end rem
Const L_ROTATE_AREA_MAP:Int = 1
Rem
bbdoc: Use shear rotation
end rem
Const L_ROTATE_SHEAR:Int = 2
Rem
bbdoc: Use sampling
end rem
Const L_ROTATE_SAMPLING:Int = 3


Rem
bbdoc: Bring in white pixels from the outside.
end rem
Const L_BRING_IN_WHITE:Int = 1
Rem
bbdoc: Bring in black pixels from the outside.
end rem
Const L_BRING_IN_BLACK:Int = 2


Rem
bbdoc: Shear image about UL corner.
End Rem
Const L_SHEAR_ABOUT_CORNER:Int = 1
Rem
bbdoc: Shear image about center.
End Rem
Const L_SHEAR_ABOUT_CENTER:Int = 2
