' Copyright (c) 2017 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of the project nor the names of its contributors
'       may be used to endorse or promote products derived from this software
'       without specific prior written permission. 
'
' THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Barcode Generator
End Rem
Module BaH.Zint

ModuleInfo "Version: 1.02"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2017 Bruce A Henderson"
ModuleInfo "Copyright: libzint - 2009-2017 Robin Stuart"

ModuleInfo "History: 1.02"
ModuleInfo "History: Update to libzint 2.6.2."
ModuleInfo "History: 1.01"
ModuleInfo "History: Implemented Zint/Cairo renderer."
ModuleInfo "History: 1.01"
ModuleInfo "History: Implemented Zint/Cairo renderer."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (libzint 2.5.1)"

ModuleInfo "CC_OPTS: -DNO_PNG -DCAIRO_RENDER"

Import "common.bmx"

Import BRL.Pixmap

'
' Notes :
'
'  Cairo renderer changes :
'     zint.h     (modified) - added context to symbol struct
'     library.c  (modified) - added CGR extension to call into cairo_plot()
'     cairo.c    (added)
'

Rem
bbdoc: 
End Rem
Type TZBarcode

	Field zbPtr:Byte Ptr
	Field error:Int
	
	Method New()
		zbPtr = ZBarcode_Create()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetSymbology(symbology:Int = BARCODE_CODE128)
		bmx_zint_setsymbology(zbPtr, symbology)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetScale(scale:Float)
		bmx_zint_setscale(zbPtr, scale)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetWhitespaceWidth(width:Int)
		bmx_zint_setwhitespacewidth(zbPtr, width)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetBorderWidth(width:Int)
		bmx_zint_setborderwidth(zbPtr, width)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFGColour(r:Int, g:Int, b:Int)
		bmx_zint_setfgcolour(zbPtr, r, g, b)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetBGColour(r:Int, g:Int, b:Int)
		bmx_zint_setbgcolour(zbPtr, r, g, b)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Encode:TPixmap(txt:String, rotateAngle:Int = 0)
		error = 0
		Local s:Byte Ptr = txt.ToUTF8String()
		error = ZBarcode_Encode_and_Buffer(zbPtr, s, 0, rotateAngle)
		MemFree(s)
		If Not error Then
			Local w:Int, h:Int
			bmx_zint_bitmapdimensions(zbPtr, Varptr w, Varptr h)
			
			Local data:Byte Ptr = bmx_zint_bitmap(zbPtr)
		
			Local pix:TPixmap = TPixmap.Create(w, h, PF_RGB888, 3)

			MemCopy(pix.pixels, data, Size_T(w * h * 3))
			
			Return pix
		End If
	End Method
	
	Method CairoEncode(ctx:TCairo, txt:String, rotateAngle:Int = 0)
		error = 0
		Local s:Byte Ptr = txt.ToUTF8String()
		error = bmx_zint_cairoencode(zbPtr, ctx.contextPtr, s, 0, rotateAngle)
		MemFree(s)
	End Method

	Method Free()
		If zbPtr Then
			ZBarcode_Delete(zbPtr)
			zbPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method
	
End Type


