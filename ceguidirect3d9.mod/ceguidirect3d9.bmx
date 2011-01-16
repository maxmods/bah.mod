' Copyright (c) 2010-2011 Bruce A Henderson
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
bbdoc: CEGUI Direct3D 9 Support.
End Rem
Module BaH.CEGUIDirect3D9

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (CEGUI 0.7.x - SVN rev 2357)"

?win32
ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DPCRE_STATIC"
ModuleInfo "CC_OPTS: -DCEGUI_STATIC -DCEGUI_FALAGARD_RENDERER -DCEGUI_WITH_TINYXML -DCEGUI_CODEC_FREEIMAGE"


ModuleInfo "CC_OPTS: -DFREEIMAGE_LIB -DPNG_iTXt_SUPPORTED -DHAVE_PROTOTYPES -DOPJ_STATIC"

ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"


Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TCEDirect3D9Renderer Extends TCERenderer

	Method init:TCERenderer(g:Object = Null)
		If TD3D9Graphics(g) Then
			objectPtr = bmx_cegui_direct3d9renderer_new(TD3D9Graphics(g).GetDirect3DDevice())
		Else If TMax2DGraphics(g) Then
			objectPtr = bmx_cegui_direct3d9renderer_new(TD3D9Graphics(TMax2DGraphics(g)._graphics).GetDirect3DDevice())
		Else
			Return Null
		End If
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method createGeometryBuffer:TCEGeometryBuffer()
		Return TCEDirect3D9GeometryBuffer(bmx_cegui_direct3d9renderer_creategeometrybuffer(objectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyGeometryBuffer(buffer:TCEGeometryBuffer)
		bmx_cegui_direct3d9renderer_destroygeometrybuffer(objectPtr, buffer.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyAllGeometryBuffers()
		bmx_cegui_direct3d9renderer_destroyallgeometrybuffers(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method createTextureTarget:TCETextureTarget()
		Return TCEDirect3D9TextureTarget(bmx_cegui_direct3d9renderer_createtexturetarget(objectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyTextureTarget(target:TCETextureTarget)
		bmx_cegui_direct3d9renderer_destroytexturetarget(objectPtr, target.objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyAllTextureTargets()
		bmx_cegui_direct3d9renderer_destroyalltexturetargets(objectPtr)
	End Method
	
	Rem
	bbdoc: Create a Texture object using the given image file.
	about: Textures are always created with a size that is a power of 2. If the file you specify is
	of a size that is not a power of two, the final size will be rounded up. Additionally, textures are
	always square, so the ultimate size is governed by the larger of the width and height of the
	specified file. You can check the ultimate sizes by querying the texture after creation.
	End Rem
	Method CreateTexture:TCETexture(filename:String, resourceGroup:String)
		Return TCEDirect3D9Texture(bmx_cegui_direct3d9renderer_createtexture(objectPtr, _convertMaxToUTF8(filename), _convertMaxToUTF8(resourceGroup)))
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
		Return TCEDirect3D9Texture(bmx_cegui_direct3d9renderer_createtexturewithsize(objectPtr, w, h))
	End Method
	
	Rem
	bbdoc: Destroys the given Texture.
	End Rem
	Method destroyTexture(texture:TCETexture)
		bmx_cegui_direct3d9renderer_destroytexture(objectPtr, texture.objectPtr)
	End Method
	
	Rem
	bbdoc: Destroys all Texture objects.
	End Rem
	Method destroyAllTextures()
		bmx_cegui_direct3d9renderer_destroyalltextures(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method beginRendering()
		bmx_cegui_direct3d9renderer_beginrendering(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method endRendering()
		bmx_cegui_direct3d9renderer_endrendering(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the display in pixels.
	End Rem
	Method getDisplaySize(width:Float Var, height:Float Var)
		bmx_cegui_direct3d9renderer_getdisplaysize(objectPtr, Varptr width, Varptr height)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setDisplaySize(width:Float, height:Float)
		bmx_cegui_direct3d9renderer_setdisplaysize(objectPtr, width, height)
	End Method
	
	Rem
	bbdoc: Returns the vertical display resolution dpi.
	End Rem
	Method getDisplayDPI(x:Float Var, y:Float Var)
		bmx_cegui_direct3d9renderer_getdisplaydpi(objectPtr, Varptr x, Varptr y)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getMaxTextureSize:Int()
		Return bmx_cegui_direct3d9renderer_getmaxtexturesize(objectPtr)
	End Method
	
	Rem
	bbdoc: Return identification string for the renderer module.
	about: If the internal id string has not been set by the Renderer module creator, a generic string
	of "Unknown renderer" will be returned.
	End Rem
	Method getIdentifierString:String()
		Return bmx_cegui_direct3d9renderer_getidentifierstring(objectPtr)
	End Method
	
End Type


Type TCEDirect3D9GeometryBuffer Extends TCEGeometryBuffer

	Function _create:TCEDirect3D9GeometryBuffer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEDirect3D9GeometryBuffer = New TCEDirect3D9GeometryBuffer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

End Type

Type TCEDirect3D9TextureTarget Extends TCETextureTarget

	Function _create:TCEDirect3D9TextureTarget(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEDirect3D9TextureTarget = New TCEDirect3D9TextureTarget
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
End Type

Type TCEDirect3D9Texture Extends TCETexture

	Function _create:TCEDirect3D9Texture(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TCEDirect3D9Texture = New TCEDirect3D9Texture
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
End Type






?