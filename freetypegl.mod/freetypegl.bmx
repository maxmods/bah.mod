' Copyright (c) 2014 Bruce A Henderson
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

Rem
bbdoc: FreeType-GL
End Rem
Module BaH.FreeTypeGL

?linux
ModuleInfo "CC_OPTS: `freetype-config --cflags`"
?

Import "common.bmx"

'
' Notes :
'  texture-font customized to allow for filename AND memory - so that Incbin can work with font manager.
'  Implemented a font_manager_get_from_memory() in glue.c, using filename and mem.
'

' vertex shader
Incbin "freetype-gl/shaders/text.vert"
' frag shader
Incbin "freetype-gl/shaders/text.frag"

Rem
bbdoc: Font Manager
about: In charge of caching fonts.
End Rem
Type TFontManager

	Field fmPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Method Create:TFontManager(atlasWidth:Int = 512, atlasHeight:Int = 512)
		fmPtr = bmx_freetypegl_font_manager_new(atlasWidth, atlasHeight)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method FontBuffer:TFontBuffer(fontPath:String, size:Int)
		Return TFontBuffer._create(fmPtr, fontPath, size)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If fmPtr Then
			bmx_freetypegl_font_manager_free(fmPtr)
			fmPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type TFontBuffer

	Field bufferPtr:Byte Ptr
	
	Function _create:TFontBuffer(fmPtr:Byte Ptr, fontPath:String, size:Int)
		Local vertSource:String = LoadString("incbin::freetype-gl/shaders/text.vert")
		Local fragSource:String = LoadString("incbin::freetype-gl/shaders/text.frag")

		Local bufferPtr:Byte Ptr

		If fontpath.Find("incbin::") >= 0 Then
			Local path:String = fontPath[fontpath.Find("incbin::") + 8..]
			bufferPtr = bmx_freetypegl_font_buffer_new(fmPtr, path, size, vertSource, fragSource, IncbinPtr(path), IncbinLen(path))
		Else
			bufferPtr = bmx_freetypegl_font_buffer_new(fmPtr, fontPath, size, vertSource, fragSource, 0, 0)
		End If

		If bufferPtr Then
			Local buffer:TFontBuffer = New TFontBuffer
			buffer.bufferPtr = bufferPtr
			Return buffer
		End If
		
		Return Null
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method SetViewport(x:Int, y:Int, width:Int, height:Int)
		bmx_freetypegl_font_buffer_setviewport(bufferPtr, x, y, width, height)
	End Method

	Rem
	bbdoc: Renders the text.
	End Rem
	Method Render()
		bmx_freetypegl_font_buffer_render(bufferPtr)
	End Method

	Rem
	bbdoc: Clears the text buffer of all text.
	about: Use Draw() to re-fill the buffer.
	End Rem
	Method Clear()
		bmx_freetypegl_font_buffer_clear(bufferPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Draw(text:String, x:Float, y:Float)
		bmx_freetypegl_font_buffer_draw(bufferPtr, x, y, text)
	End Method
	
	Rem
	bbdoc: Sets the font color for the next Draw.
	End Rem
	Method SetColor(r:Int, g:Int, b:Int, a:Int = 255)
		bmx_freetypegl_font_buffer_setcolor(bufferPtr, r, g, b, a)
	End Method

	Rem
	bbdoc: Sets the vertical gradient colors for the next Draw.
	End Rem
	Method SetGradientColor(r0:Int, g0:Int, b0:Int, a0:Int, r1:Int, g1:Int, b1:Int, a1:Int)
		bmx_freetypegl_font_buffer_setgradientcolor(bufferPtr, r0, g0, b0, a0, r1, g1, b1, a1)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetGamma(g:Float)
		bmx_freetypegl_font_buffer_setgamma(bufferPtr, g)
	End Method
	
	Rem
	bbdoc: Sets glyph outline type.
	about: One of, #OUTLINE_NONE, #OUTLINE_LINE, #OUTLINE_INNER or #OUTLINE_OUTER
	End Rem
	Method SetOutlineType(value:Int)
		bmx_freetypegl_font_buffer_setoutlinetype(bufferPtr, value)
	End Method
	
	Rem
	bbdoc: Sets glyph outline thickness.
	End Rem
	Method SetOutlineThickness(value:Float)
		bmx_freetypegl_font_buffer_setoutlinethickness(bufferPtr, value)
	End Method

	Rem
	bbdoc: Requests the loading of several glyphs at once.
	returns: Number of missed glyphs if the texture is not big enough to hold every glyph.
	End Rem
	Method LoadGlyphs:Int(text:String)
		Return bmx_freetypegl_texture_font_load_glyphs(bufferPtr, text)
	End Method
	
	Rem
	bbdoc: This is simply used to compute a default line spacing (i.e., the baseline-to-baseline distance) when writing text with this font.
	about: Note that it usually is larger than the sum of the ascender and descender
	taken as absolute values. There is also no guarantee that no glyphs extend above or below subsequent baselines when using this distance.
	End Rem
	Method Height:Float()
		Return bmx_freetypegl_texture_font_height(bufferPtr)
	End Method

	Rem
	bbdoc: This is the distance that must be placed between two lines of text.
	about: The baseline-to-baseline distance should be computed as: sascender - descender + linegap
	End Rem
	Method LineGap:Float()
		Return bmx_freetypegl_texture_font_linegap(bufferPtr)
	End Method

	Rem
	bbdoc: The ascender is the vertical distance from the horizontal baseline to the highest 'character' coordinate in a font face.
	about: Unfortunately, font formats define the ascender differently. For some, it represents the
	ascent of all capital latin characters (without accents), for others it is the ascent of the highest accented character, and finally, other
	formats define it as being equal to bbox.yMax.
	End Rem
	Method Ascender:Float()
		Return bmx_freetypegl_texture_font_ascender(bufferPtr)
	End Method

	Rem
	bbdoc: The descender is the vertical distance from the horizontal baseline to the lowest 'character' coordinate in a font face.
	about: Unfortunately, font formats define the descender differently. For some, it represents the
	descent of all capital latin characters (without accents), for others it is the ascent of the lowest accented character, and finally, other
	formats define it as being equal to bbox.yMin. This field is negative for values below the baseline.
	End Rem
	Method Descender:Float()
		Return bmx_freetypegl_texture_font_descender(bufferPtr)
	End Method

	Method Free()
		If bufferPtr Then
			bmx_freetypegl_font_buffer_free(bufferPtr)
			bufferPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method

End Type
