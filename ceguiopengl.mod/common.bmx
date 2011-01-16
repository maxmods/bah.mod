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

Import BaH.CEGUI
Import BRL.Blitz
Import BRL.Keycodes

Import "../../pub.mod/freetype.mod/include/*.h"
Import "../regex.mod/src/*.h"
Import "../freeimage.mod/src/*.h"

?macos
Import "-framework CoreFoundation"
Import "../cegui.mod/cegui/src/implementations/mac/*.h"
?

Import "../cegui.mod/cegui/include/*.h"

' ogl renderer
Import Pub.Glew
Import "../../pub.mod/glew.mod/GL/*.h"
Import "../cegui.mod/cegui/include/RendererModules/OpenGL/*.h"

?macos
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLApplePBTextureTarget.cpp"
?
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLFBOTextureTarget.cpp"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLGeometryBuffer.cpp"
?linux
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLGLXPBTextureTarget.cpp"
?
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLRenderer.cpp"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLRenderTarget.cpp"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLTexture.cpp"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLViewportTarget.cpp"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLTextureTarget.cpp"
?win32
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/CEGUIOpenGLWGLPBTextureTarget.cpp"
?

Import "glue.cpp"


Extern

	Function bmx_cegui_openglrenderer_new:Byte Ptr()
	Function bmx_cegui_openglrenderer_creategeometrybuffer:Object(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_destroygeometrybuffer(handle:Byte Ptr, buffer:Byte Ptr)
	Function bmx_cegui_openglrenderer_destroyallgeometrybuffers(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_createtexturetarget:Object(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_destroytexturetarget(handle:Byte Ptr, target:Byte Ptr)
	Function bmx_cegui_openglrenderer_destroyalltexturetargets(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_createtexture:Object(handle:Byte Ptr, filename:Byte Ptr, resourceGroup:Byte Ptr)
	Function bmx_cegui_openglrenderer_createtexturewithsize:Object(handle:Byte Ptr, w:Float, h:Float)
	Function bmx_cegui_openglrenderer_destroytexture(handle:Byte Ptr, texture:Byte Ptr)
	Function bmx_cegui_openglrenderer_destroyalltextures(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_getdisplaysize(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_openglrenderer_setdisplaysize(handle:Byte Ptr, width:Float, height:Float)
	Function bmx_cegui_openglrenderer_getdisplaydpi(handle:Byte Ptr, width:Float Ptr, height:Float Ptr)
	Function bmx_cegui_openglrenderer_getmaxtexturesize:Int(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_getidentifierstring:String(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_beginrendering(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_endrendering(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_creategltexture:Object(handle:Byte Ptr, tex:Int, w:Float, h:Float)
	Function bmx_cegui_openglrenderer_enableextrastatesettings(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_openglrenderer_grabtextures(handle:Byte Ptr)
	Function bmx_cegui_openglrenderer_restoretextures(handle:Byte Ptr)

End Extern


