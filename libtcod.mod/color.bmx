' Copyright (c) 2008-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "common.bmx"


Rem
bbdoc: A color is defined by its red, green and blue component between 0 and 255.
End Rem
Type TCODColor

	Field colorPtr:Byte Ptr
	
	Field r:Int
	Field g:Int
	Field b:Int
	
	Function _create:TCODColor(colorPtr:Byte Ptr)
		If colorPtr Then
			Local this:TCODColor = New TCODColor
			this.colorPtr = colorPtr
			this.UpdateColors()
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Creates a new color with the given rgb values.
	End Rem
	Function CreateColor:TCODColor(r:Int = 0, g:Int = 0, b:Int = 0)
		Return New TCODColor.Create(r, g, b)
	End Function
	
	Rem
	bbdoc: Creates a new color with the given rgb values.
	End Rem
	Method Create:TCODColor(_r:Int = 0, _g:Int = 0, _b:Int = 0)
		colorPtr = bmx_tcodcolor_create(_r, _g, _b)
		UpdateColors()
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Copy:TCODColor()
		Return New TCODColor.Create(r, g, b)
	End Method
	
	Rem
	bbdoc: Returns true if the two colors are equal.
	End Rem
	Method Equals:Int(color:TCODColor)
		Return bmx_tcodcolor_equals(colorPtr, color.colorPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetR(value:Int)
		bmx_tcodcolor_setr(colorPtr, value)
		r = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetG(value:Int)
		bmx_tcodcolor_setg(colorPtr, value)
		g = value
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetB(value:Int)
		bmx_tcodcolor_setb(colorPtr, value)
		b = value
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UpdateColors()
		bmx_tcodcolor_updatecolors(colorPtr, Varptr r, Varptr g, Varptr b)
	End Method

	Rem
	bbdoc: 
	End Rem
	Function Lerp:TCODColor(a:TCODColor, b:TCODColor, coef:Float)
		Return _create(bmx_tcodcolor_lerp(a.colorPtr, b.colorPtr, coef))
	End Function
	
	Method Delete()
		If colorPtr Then
			bmx_tcodcolor_delete(colorPtr)
			colorPtr = Null
		End If
	End Method
	
	' grey levels
	Global Black:TCODColor = TCODColor._create(bmx_tcodcolor_black())
	Global darkGrey:TCODColor = TCODColor._create(bmx_tcodcolor_darkgrey())
	Global grey:TCODColor = TCODColor._create(bmx_tcodcolor_grey())
	Global lightGrey:TCODColor = TCODColor._create(bmx_tcodcolor_lightgrey())
	Global white:TCODColor = TCODColor._create(bmx_tcodcolor_white())
	
	' standard
	Global red:TCODColor = TCODColor._create(bmx_tcodcolor_red())
	Global orange:TCODColor = TCODColor._create(bmx_tcodcolor_orange())
	Global chartreuse:TCODColor = TCODColor._create(bmx_tcodcolor_chartreuse())
	Global yellow:TCODColor = TCODColor._create(bmx_tcodcolor_yellow())
	Global green:TCODColor = TCODColor._create(bmx_tcodcolor_green())
	Global sea:TCODColor = TCODColor._create(bmx_tcodcolor_sea())
	Global cyan:TCODColor = TCODColor._create(bmx_tcodcolor_cyan())
	Global sky:TCODColor = TCODColor._create(bmx_tcodcolor_sky())
	Global blue:TCODColor = TCODColor._create(bmx_tcodcolor_blue())
	Global violet:TCODColor = TCODColor._create(bmx_tcodcolor_violet())
	Global magenta:TCODColor = TCODColor._create(bmx_tcodcolor_magenta())
	Global pink:TCODColor = TCODColor._create(bmx_tcodcolor_pink())

	' dark
	Global darkRed:TCODColor = TCODColor._create(bmx_tcodcolor_darkred())
	Global darkOrange:TCODColor = TCODColor._create(bmx_tcodcolor_darkorange())
	Global darkYellow:TCODColor = TCODColor._create(bmx_tcodcolor_darkyellow())
	Global darkChartreuse:TCODColor = TCODColor._create(bmx_tcodcolor_darkchartreuse())
	Global darkGreen:TCODColor = TCODColor._create(bmx_tcodcolor_darkgreen())
	Global darkSea:TCODColor = TCODColor._create(bmx_tcodcolor_darksea())
	Global darkCyan:TCODColor = TCODColor._create(bmx_tcodcolor_darkcyan())
	Global darkSky:TCODColor = TCODColor._create(bmx_tcodcolor_darksky())
	Global darkBlue:TCODColor = TCODColor._create(bmx_tcodcolor_darkblue())
	Global darkViolet:TCODColor = TCODColor._create(bmx_tcodcolor_darkviolet())
	Global darkMagenta:TCODColor = TCODColor._create(bmx_tcodcolor_darkmagenta())
	Global darkPink:TCODColor = TCODColor._create(bmx_tcodcolor_darkpink())

	' darker colors
	Global darkerRed:TCODColor = TCODColor._create(bmx_tcodcolor_darkerred())
	Global darkerOrange:TCODColor = TCODColor._create(bmx_tcodcolor_darkerorange())
	Global darkerYellow:TCODColor = TCODColor._create(bmx_tcodcolor_darkeryellow())
	Global darkerChartreuse:TCODColor = TCODColor._create(bmx_tcodcolor_darkerchartreuse())
	Global darkerGreen:TCODColor = TCODColor._create(bmx_tcodcolor_darkergreen())
	Global darkerSea:TCODColor = TCODColor._create(bmx_tcodcolor_darkersea())
	Global darkerCyan:TCODColor = TCODColor._create(bmx_tcodcolor_darkercyan())
	Global darkerSky:TCODColor = TCODColor._create(bmx_tcodcolor_darkersky())
	Global darkerBlue:TCODColor = TCODColor._create(bmx_tcodcolor_darkerblue())
	Global darkerViolet:TCODColor = TCODColor._create(bmx_tcodcolor_darkerviolet())
	Global darkerMagenta:TCODColor = TCODColor._create(bmx_tcodcolor_darkermagenta())
	Global darkerPink:TCODColor = TCODColor._create(bmx_tcodcolor_darkerpink())


	' light colors
	Global lightRed:TCODColor = TCODColor._create(bmx_tcodcolor_lightred())
	Global lightOrange:TCODColor = TCODColor._create(bmx_tcodcolor_lightorange())
	Global lightYellow:TCODColor = TCODColor._create(bmx_tcodcolor_lightyellow())
	Global lightChartreuse:TCODColor = TCODColor._create(bmx_tcodcolor_lightchartreuse())
	Global lightGreen:TCODColor = TCODColor._create(bmx_tcodcolor_lightgreen())
	Global lightSea:TCODColor = TCODColor._create(bmx_tcodcolor_lightsea())
	Global lightCyan:TCODColor = TCODColor._create(bmx_tcodcolor_lightcyan())
	Global lightSky:TCODColor = TCODColor._create(bmx_tcodcolor_lightsky())
	Global lightBlue:TCODColor = TCODColor._create(bmx_tcodcolor_lightblue())
	Global lightViolet:TCODColor = TCODColor._create(bmx_tcodcolor_lightviolet())
	Global lightMagenta:TCODColor = TCODColor._create(bmx_tcodcolor_lightmagenta())
	Global lightPink:TCODColor = TCODColor._create(bmx_tcodcolor_lightpink())


	' desaturated colors
	Global desaturatedRed:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedred())
	Global desaturatedOrange:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedorange())
	Global desaturatedYellow:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedyellow())
	Global desaturatedChartreuse:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedchartreuse())
	Global desaturatedGreen:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedgreen())
	Global desaturatedSea:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedsea())
	Global desaturatedCyan:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedcyan())
	Global desaturatedSky:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedsky())
	Global desaturatedBlue:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedblue())
	Global desaturatedViolet:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedviolet())
	Global desaturatedMagenta:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedmagenta())
	Global desaturatedPink:TCODColor = TCODColor._create(bmx_tcodcolor_desaturatedpink())


	' special
	Global silver:TCODColor = TCODColor._create(bmx_tcodcolor_silver())
	Global gold:TCODColor = TCODColor._create(bmx_tcodcolor_gold())

End Type

