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
Import "../cegui.mod/cegui/include/RendererModules/OpenGL/*.h"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/GLEW/*.h"
Import "../cegui.mod/cegui/src/RendererModules/OpenGL/GLEW/src/glew.c"

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


