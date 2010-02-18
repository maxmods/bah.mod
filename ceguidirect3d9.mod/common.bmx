' Copyright (c) 2010 Bruce A Henderson
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

Import BaH.CEGUI
Import BRL.Blitz
Import BRL.Keycodes
Import BRL.D3D9Max2D

Import "-ld3dx9"

Import "../../pub.mod/freetype.mod/include/*.h"
Import "../regex.mod/src/*.h"
Import "../freeimage.mod/src/*.h"

Import "include/*.h"
Import "../cegui.mod/cegui/include/*.h"

' d3d9 renderer
Import "../cegui.mod/cegui/include/RendererModules/Direct3D9/*.h"

Import "../cegui.mod/cegui/src/RendererModules/Direct3D9/CEGUIDirect3D9GeometryBuffer.cpp"
Import "../cegui.mod/cegui/src/RendererModules/Direct3D9/CEGUIDirect3D9Renderer.cpp"
Import "../cegui.mod/cegui/src/RendererModules/Direct3D9/CEGUIDirect3D9RenderTarget.cpp"
Import "../cegui.mod/cegui/src/RendererModules/Direct3D9/CEGUIDirect3D9Texture.cpp"
Import "../cegui.mod/cegui/src/RendererModules/Direct3D9/CEGUIDirect3D9ViewportTarget.cpp"
Import "../cegui.mod/cegui/src/RendererModules/Direct3D9/CEGUIDirect3D9TextureTarget.cpp"

Import "glue.cpp"

Extern

	Function bmx_cegui_direct3d9renderer_new:Byte Ptr(device:IDirect3DDevice9)
	Function bmx_cegui_direct3d9renderer_creategeometrybuffer:Object(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_destroygeometrybuffer(handle:Byte Ptr, buffer:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_destroyallgeometrybuffers(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_createtexturetarget:Object(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_destroytexturetarget(handle:Byte Ptr, target:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_destroyalltexturetargets(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_createtexture:Object(handle:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_createtexturewithsize:Object(handle:Byte Ptr, w:Float, h:Float)
	Function bmx_cegui_direct3d9renderer_destroytexture(handle:Byte Ptr, texture:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_destroyalltextures(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_getdisplaysize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_direct3d9renderer_setdisplaysize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_direct3d9renderer_getdisplaydpi(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_direct3d9renderer_getmaxtexturesize:Int(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_getidentifierstring:String(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_beginrendering(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_endrendering(handle:Byte Ptr)

End Extern

