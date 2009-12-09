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

Import BRL.GLMax2D
?win32
Import BRL.d3d7max2d
?

Import "common.bmx"

'
' Based in part on single-surface Fontext code by Robin Beaker.
'
Type TCODFont
	Field image:TImage
	Field fontHeight:Int
	Field fontWidth:Int
	Field char:Int[256]
	
	Field widthMultiplier:Float
	Field heightMultiplier:Float
	
	Field u0:Float[256]
	Field v0:Float[256]
	
	Field u1:Float[256]
	Field v1:Float[256]

	Global isGL:Int = -1
	
	Global GLframe:TGLImageFrame
	?win32
	Global DXframe:TD3D7ImageFrame
	?

	Function Create:TCODFont(file:TPixmap, fontNbCharHoriz:Int, fontNbCharVertic:Int, inRow:Int, Flags:Int = -1)
'DebugStop
		Local img:TImage = LoadImage(file, Flags)
		SetImageHandle(img, 0, 0)
		
		Local f:TCODFont = New TCODFont
		f.Image = img
		
		f.widthMultiplier = 1.0 / Pow2Size(img.width)
		f.heightMultiplier = 1.0 / Pow2Size(img.height)
		
		f.fontWidth = img.width / fontNbCharHoriz
		f.fontHeight = img.height / fontNbCharVertic
		
		Local j:Int, x:Float, y:Float
		Local cur:Int
		
		For Local i:Int = 0 Until 256

			If i < 32 And flags & TCOD_FONT_LAYOUT_TCOD Then
				j:+ 1
				Continue
			End If
			
			cur = i
			f.char[cur] = True
			
			If inRow Then
				x = Float((j Mod fontNbCharHoriz)*f.fontWidth)
				y = Float((j /fontNbCharHoriz)*f.fontHeight)
			Else
				x = Float((j /fontNbCharVertic)*f.fontWidth)
				y = Float((j Mod fontNbCharVertic)*f.fontHeight)
			End If
'DebugLog i + " : " + x + ", " + y

			j:+ 1
		
			f.u0[cur] = x * f.widthMultiplier
			f.v0[cur] = y * f.heightMultiplier
			f.u1[cur] = (x + f.fontWidth) * f.widthMultiplier
			f.v1[cur] = (y + f.fontHeight) * f.heightMultiplier
		Next
		
		Return f
	End Function

	Method Draw(i:Int, X:Float, Y:Float, force:Int = False, cb:Int = -1)
'DebugStop
		'Local frame:Int = 0
		
		If isGL < 0 Or TCOD_REFRESH_IMAGE Then
			If TGLImageFrame(Image.Frame(0))
				isGL = True
				GLframe = TGLImageFrame(Image.Frame(0))
		?win32
			Else
				isGL = False
				DXframe = TD3D7ImageFrame(Image.Frame(0))
		?
			End If
			TCOD_REFRESH_IMAGE = False
		End If
		
		If char[i] Or cb >= 0 Then
			Local curChar:Int = i
			If cb >= 0 Then
				curChar = cb
			End If
			
			If isGL
				GLframe.u0 = u0[curChar]
				GLframe.v0 = v0[curChar]
				GLframe.u1 = u1[curChar]
				GLframe.v1 = v1[curChar]
				
				If force Then
					' just to force the texture bind...
					GLframe.Draw(0, 0, fontWidth, fontHeight, X, Y, 0, 0, 1, 1)
				End If

				glEnable GL_TEXTURE_2D
				glBegin GL_QUADS
				glTexCoord2f GLframe.u0, GLframe.v0
				glVertex2f x,y
				glTexCoord2f GLframe.u1, GLframe.v0
				glVertex2f fontWidth + X, y
				glTexCoord2f GLframe.u1, GLframe.v1
				glVertex2f fontHeight +x, fontWidth+ y
				glTexCoord2f GLframe.u0, GLframe.v1
				glVertex2f x, fontHeight+y
				glEnd

			?win32
			Else
				DXframe.setUV(u0[curchar], v0[curChar], u1[curChar], v1[curChar])
				DXframe.Draw(0, 0, fontWidth, fontHeight, X, Y, 0, 0, 1, 1)
			?
			End If
			
		End If
		
	End Method

Function Pow2Size:Int( n:Int )
	Local t:Int=1
	While t<n
		t:*2
	Wend
	Return t
End Function

End Type


