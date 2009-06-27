' Copyright (c) 2008,2009 Bruce A Henderson
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

Import "core.bmx"


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

Function DrawableFont:TMDrawableFont(font:String)
	Return New TMDrawableFont.Create(font)
End Function

Rem
bbdoc: Specify font name to use when drawing text.
End Rem
Type TMDrawableFont Extends TMDrawable

	Rem
	bbdoc: 
	End Rem
	Method Create:TMDrawableFont(font:String)
		font = TCustomMagickFont.getFont(font)
		drawablePtr = bmx_magick_drawable_drawablefont_create("@" + font)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CreateWithAttributes:TMDrawableFont(family:String, style:Int, weight:Int, stretch:Int)
	End Method

	Method Delete()
		If drawablePtr Then
			bmx_magick_drawable_drawablefont_delete(drawablePtr)
			drawablePtr = Null
		End If
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

Type TMDrawablePoint Extends TMDrawable

	Method Create:TMDrawablePoint()
	End Method
	
End Type

Type TMDrawablePointSize Extends TMDrawable

	Method Create:TMDrawablePointSize()
	End Method
	
End Type

Type TMDrawablePolygon Extends TMDrawable

	Method Create:TMDrawablePolygon()
	End Method
	
End Type

Type TMDrawablePolyline Extends TMDrawable

	Method Create:TMDrawablePolyline()
	End Method
	
End Type

Type TMDrawablePopClipPath Extends TMDrawable

	Method Create:TMDrawablePopClipPath()
	End Method
	
End Type

Type TMDrawablePopGraphicContext Extends TMDrawable

	Method Create:TMDrawablePopGraphicContext()
	End Method
	
End Type

Type TMDrawablePushClipPath Extends TMDrawable

	Method Create:TMDrawablePushClipPath()
	End Method
	
End Type

Type TMDrawablePushGraphicContext Extends TMDrawable

	Method Create:TMDrawablePushGraphicContext()
	End Method
	
End Type

Type TMDrawablePushPattern Extends TMDrawable

	Method Create:TMDrawablePushPattern()
	End Method
	
End Type

Type TMDrawablePopPattern Extends TMDrawable

	Method Create:TMDrawablePopPattern()
	End Method
	
End Type

Type TMDrawableRectangle Extends TMDrawable

	Method Create:TMDrawableRectangle()
	End Method
	
End Type

Type TMDrawableRotation Extends TMDrawable

	Method Create:TMDrawableRotation()
	End Method
	
End Type

Type TMDrawableRoundRectangle Extends TMDrawable

	Method Create:TMDrawableRoundRectangle()
	End Method
	
End Type

Type TMDrawableScaling Extends TMDrawable

	Method Create:TMDrawableScaling()
	End Method
	
End Type

Type TMDrawableSkewX Extends TMDrawable

	Method Create:TMDrawableSkewX()
	End Method
	
End Type

Type TMDrawableSkewY Extends TMDrawable

	Method Create:TMDrawableSkewY()
	End Method
	
End Type

Type TMDrawableStrokeAntialias Extends TMDrawable

	Method Create:TMDrawableStrokeAntialias()
	End Method
	
End Type

Type TMDrawableStrokeColor Extends TMDrawable

	Method Create:TMDrawableStrokeColor()
	End Method
	
End Type

Type TMDrawableStrokeLineCap Extends TMDrawable

	Method Create:TMDrawableStrokeLineCap()
	End Method
	
End Type

Type TMDrawableStrokeLineJoin Extends TMDrawable

	Method Create:TMDrawableStrokeLineJoin()
	End Method
	
End Type

Type TMDrawableStrokeOpacity Extends TMDrawable

	Method Create:TMDrawableStrokeOpacity()
	End Method
	
End Type

Type TMDrawableStrokeWidth Extends TMDrawable

	Method Create:TMDrawableStrokeWidth()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Function DrawableText:TMDrawableText(x:Double, y:Double, text:String)
	Return New TMDrawableText.Create(x, y, text)
End Function

Rem
bbdoc: Annotate image with text represented with text encoding, using current stroke color, font, font pointsize, and box color (text background color), at specified coordinates.
about: If text contains special format characters the image filename, type, width, height, or other image attributes may be incorporated
in the text (see label()).
End Rem
Type TMDrawableText Extends TMDrawable

	Method Create:TMDrawableText(x:Double, y:Double, text:String)
		drawablePtr = bmx_magick_drawable_drawabletext_create(x, y, text)
		Return Self
	End Method
	
	Method getX:Double()
	End Method
	
	Method setX(x:Double)
	End Method
	
	Method getY:Double()
	End Method
	
	Method setY(y:Double)
	End Method
	
	Method getText:String()
	End Method
	
	Method setText(text:String)
	End Method
	
	Method Delete()
		If drawablePtr Then
			bmx_magick_drawable_drawabletext_delete(drawablePtr)
			drawablePtr = Null
		End If
	End Method
	
End Type

Rem
bbbdoc: Antialias while drawing text (default true).
about: The main reason to disable text antialiasing is to avoid adding new colors to the image.
End Rem
Function DrawableTextAntialias:TMDrawableTextAntialias(flag:Int)
	Return New TMDrawableTextAntialias.Create(flag)
End Function

Rem
bbbdoc: Antialias while drawing text (default true).
about: The main reason to disable text antialiasing is to avoid adding new colors to the image.
End Rem
Type TMDrawableTextAntialias Extends TMDrawable

	Method Create:TMDrawableTextAntialias(flag:Int)
		drawablePtr = bmx_magick_drawable_drawabletextantialias_create(flag)
		Return Self
	End Method
	
	Method getFlag:Int()
	End Method
	
	Method setFlag(flag:Int)
	End Method
	
	Method Delete()
		If drawablePtr Then
			bmx_magick_drawable_drawabletextantialias_delete(drawablePtr)
			drawablePtr = Null
		End If
	End Method

End Type

Type TMDrawableTextDecoration Extends TMDrawable

	Method Create:TMDrawableTextDecoration()
	End Method
	
End Type

Type TMDrawableTextUnderColor Extends TMDrawable

	Method Create:TMDrawableTextUnderColor()
	End Method
	
End Type

Type TMDrawableTranslation Extends TMDrawable

	Method Create:TMDrawableTranslation()
	End Method
	
End Type

Type TMDrawableViewbox Extends TMDrawable

	Method Create:TMDrawableViewbox()
	End Method
	
End Type


