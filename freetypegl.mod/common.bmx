' Copyright (c) 2014-2015 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
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

Import BRL.RamStream
Import Pub.FreeType

Import "source.bmx"

Extern

	Function bmx_freetypegl_font_manager_new:Byte Ptr(atlasWidth:Int, atlasHeight:Int)
	Function bmx_freetypegl_font_manager_free(handle:Byte Ptr)

	Function bmx_freetypegl_font_buffer_new:Byte Ptr(manager:Byte Ptr, fontPath:String, size:Float, vertSource:String, fragSource:String, addr:Byte Ptr, length:Int)
	Function bmx_freetypegl_font_buffer_free(handle:Byte Ptr)
	Function bmx_freetypegl_font_buffer_render(handle:Byte Ptr)
	Function bmx_freetypegl_texture_font_load_glyphs:Int(handle:Byte Ptr, text:String)
	Function bmx_freetypegl_font_buffer_clear(handle:Byte Ptr)
	Function bmx_freetypegl_font_buffer_draw(handle:Byte Ptr, x:Float, y:Float, text:String)
	Function bmx_freetypegl_font_buffer_setcolor(handle:Byte Ptr, r:Int, g:Int, b:Int, a:Int)
	Function bmx_freetypegl_font_buffer_setviewport(handle:Byte Ptr, x:Int, y:Int, width:Int, height:Int)
	Function bmx_freetypegl_font_buffer_setoutlinetype(handle:Byte Ptr, value:Int)
	Function bmx_freetypegl_font_buffer_setoutlinethickness(handle:Byte Ptr, value:Float)
	Function bmx_freetypegl_font_buffer_setgradientcolor(handle:Byte Ptr, r0:Int, g0:Int, b0:Int, a0:Int, r1:Int, g1:Int, b1:Int, a1:Int)
	Function bmx_freetypegl_texture_font_height:Float(handle:Byte Ptr)
	Function bmx_freetypegl_texture_font_linegap:Float(handle:Byte Ptr)
	Function bmx_freetypegl_texture_font_ascender:Float(handle:Byte Ptr)
	Function bmx_freetypegl_texture_font_descender:Float(handle:Byte Ptr)
	Function bmx_freetypegl_font_buffer_setgamma(handle:Byte Ptr, g:Float)
	
End Extern

Rem
bbdoc: 
End Rem
Const OUTLINE_NONE:Int = 0
Rem
bbdoc: 
End Rem
Const OUTLINE_LINE:Int = 1
Rem
bbdoc: 
End Rem
Const OUTLINE_INNER:Int = 2
Rem
bbdoc: 
End Rem
Const OUTLINE_OUTER:Int = 3

