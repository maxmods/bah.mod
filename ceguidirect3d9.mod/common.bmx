Import BaH.CEGUI
Import BRL.Blitz
Import BRL.Keycodes

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

	Function bmx_cegui_direct3d9renderer_new:Byte Ptr()
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
	Function bmx_cegui_direct3d9renderer_creategltexture:Object(handle:Byte Ptr, tex:Int, w:Float, h:Float)
	Function bmx_cegui_direct3d9renderer_enableextrastatesettings(handle:Byte Ptr, setting:Int)
	Function bmx_cegui_direct3d9renderer_grabtextures(handle:Byte Ptr)
	Function bmx_cegui_direct3d9renderer_restoretextures(handle:Byte Ptr)

End Extern

