' Copyright (c) 2008-2011 Bruce A Henderson
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
bbdoc: CEGUI OpenGL Support.
End Rem
Module BaH.CEGUIOpenGL

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (CEGUI 0.7.x - SVN rev 2772)"

ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DPCRE_STATIC"
ModuleInfo "CC_OPTS: -DCEGUI_STATIC -DCEGUI_FALAGARD_RENDERER -DCEGUI_WITH_TINYXML -DCEGUI_CODEC_FREEIMAGE"

?win32
ModuleInfo "CC_OPTS: -DFREEIMAGE_LIB -DPNG_iTXt_SUPPORTED -DHAVE_PROTOTYPES -DOPJ_STATIC"
ModuleInfo "CC_OPTS: -DGLEW_STATIC"
?

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TCEOpenGLRenderer Extends TCERenderer

	Method New()
		objectPtr = bmx_cegui_openglrenderer_new()
	End Method

	Method init:TCERenderer(g:Object = Null)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method createGeometryBuffer:TCEGeometryBuffer()
		Return TCEOpenGLGeometryBuffer(bmx_cegui_openglrenderer_creategeometrybuffer(objectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyGeometryBuffer(buffer:TCEGeometryBuffer)
		bmx_cegui_openglrenderer_destroygeometrybuffer(objectPtr, buffer.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyAllGeometryBuffers()
		bmx_cegui_openglrenderer_destroyallgeometrybuffers(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method createTextureTarget:TCETextureTarget()
		Return TCEOpenGLTextureTarget(bmx_cegui_openglrenderer_createtexturetarget(objectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyTextureTarget(target:TCETextureTarget)
		bmx_cegui_openglrenderer_destroytexturetarget(objectPtr, target.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyAllTextureTargets()
		bmx_cegui_openglrenderer_destroyalltexturetargets(objectPtr)
	End Method
	
	Rem
	bbdoc: Create a Texture object using the given image file.
	about: Textures are always created with a size that is a power of 2. If the file you specify is
	of a size that is not a power of two, the final size will be rounded up. Additionally, textures are
	always square, so the ultimate size is governed by the larger of the width and height of the
	specified file. You can check the ultimate sizes by querying the texture after creation.
	End Rem
	Method CreateTexture:TCETexture(filename:String, resourceGroup:String)
		Return TCEOpenGLTexture(bmx_cegui_openglrenderer_createtexture(objectPtr, _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
	End Method
	
	Rem
	bbdoc: Create a Texture object with the given pixel dimensions as specified by size. 
	about: Textures are always square.
	<p>
	Textures are always created with a size that is a power of 2. If you specify a size that is not a
	power of two, the final size will be rounded up. So if you specify a size of 1024, the texture will
	be (1024 x 1024), however, if you specify a size of 1025, the texture will be (2048 x 2048). You
	can check the ultimate size by querying the texture after creation.
	</p>
	End Rem
	Method createTextureWithSize:TCETexture(w:Float, h:Float)
		Return TCEopenGLTexture(bmx_cegui_openglrenderer_createtexturewithsize(objectPtr, w, h))
	End Method
	
	Rem
	bbdoc: Destroys the given Texture.
	End Rem
	Method destroyTexture(texture:TCETexture)
		bmx_cegui_openglrenderer_destroytexture(objectPtr, texture.objectPtr)
	End Method
	
	Rem
	bbdoc: Destroys all Texture objects.
	End Rem
	Method destroyAllTextures()
		bmx_cegui_openglrenderer_destroyalltextures(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method beginRendering()
		bmx_cegui_openglrenderer_beginrendering(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method endRendering()
		bmx_cegui_openglrenderer_endrendering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the display in pixels.
	End Rem
	Method getDisplaySize(width:Float Var, height:Float Var)
		bmx_cegui_openglrenderer_getdisplaysize(objectPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setDisplaySize(width:Float, height:Float)
		bmx_cegui_openglrenderer_setdisplaysize(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: Returns the vertical display resolution dpi.
	End Rem
	Method getDisplayDPI(x:Float Var, y:Float Var)
		bmx_cegui_openglrenderer_getdisplaydpi(objectPtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getMaxTextureSize:Int()
		Return bmx_cegui_openglrenderer_getmaxtexturesize(objectPtr)
	End Method
	
	Rem
	bbdoc: Return identification string for the renderer module.
	about: If the internal id string has not been set by the Renderer module creator, a generic string
	of "Unknown renderer" will be returned.
	End Rem
	Method getIdentifierString:String()
		Return bmx_cegui_openglrenderer_getidentifierstring(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method createGLTexture:TCETexture(tex:Int, w:Float, h:Float)
		Return TCEOpenGLTexture(bmx_cegui_openglrenderer_creategltexture(objectPtr, tex, w, h))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method enableExtraStateSettings(setting:Int)
		bmx_cegui_openglrenderer_enableextrastatesettings(objectPtr, setting)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method grabTextures()
		bmx_cegui_openglrenderer_grabtextures(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method restoreTextures()
		bmx_cegui_openglrenderer_restoretextures(objectPtr)
	End Method

End Type


Type TCEOpenGLGeometryBuffer Extends TCEGeometryBuffer

	Function _create:TCEOpenGLGeometryBuffer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEOpenGLGeometryBuffer = New TCEOpenGLGeometryBuffer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Type TCEOpenGLTextureTarget Extends TCETextureTarget

	Function _create:TCEOpenGLTextureTarget(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEOpenGLTextureTarget = New TCEOpenGLTextureTarget
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
End Type

Type TCEOpenGLTexture Extends TCETexture

	Function _create:TCEOpenGLTexture(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEOpenGLTexture = New TCEOpenGLTexture
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
End Type



