/*
  Copyright (c) 2010 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/ 

#include "CEGUI.h"
#include <CEGUIDirect3D9Renderer.h>
#include "../cegui.mod/glue.h"

extern "C" {

#include "blitz.h"


	BBObject * _bah_ceguidirect3d9_TCEDirect3D9GeometryBuffer__create(CEGUI::GeometryBuffer * buffer);
	BBObject * _bah_ceguidirect3d9_TCEDirect3D9TextureTarget__create(CEGUI::TextureTarget * target);
	BBObject * _bah_ceguidirect3d9_TCEDirect3D9Texture__create(CEGUI::Texture * buffer);

	CEGUI::Direct3D9Renderer * bmx_cegui_direct3d9renderer_new(LPDIRECT3DDEVICE9 device);
	BBObject * bmx_cegui_direct3d9renderer_creategeometrybuffer(CEGUI::Direct3D9Renderer * rend);
	void bmx_cegui_direct3d9renderer_destroygeometrybuffer(CEGUI::Direct3D9Renderer * rend, CEGUI::GeometryBuffer * buffer);
	void bmx_cegui_direct3d9renderer_destroyallgeometrybuffers(CEGUI::Direct3D9Renderer * rend);
	BBObject * bmx_cegui_direct3d9renderer_createtexturetarget(CEGUI::Direct3D9Renderer * rend);
	void bmx_cegui_direct3d9renderer_destroytexturetarget(CEGUI::Direct3D9Renderer * rend, CEGUI::TextureTarget * target);
	void bmx_cegui_direct3d9renderer_destroyalltexturetargets(CEGUI::Direct3D9Renderer * rend);
	BBObject * bmx_cegui_direct3d9renderer_createtexture(CEGUI::Direct3D9Renderer * rend, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	BBObject * bmx_cegui_direct3d9renderer_createtexturewithsize(CEGUI::Direct3D9Renderer * rend, float w, float h);
	void bmx_cegui_direct3d9renderer_destroytexture(CEGUI::Direct3D9Renderer * rend, CEGUI::Texture * texture);
	void bmx_cegui_direct3d9renderer_destroyalltextures(CEGUI::Direct3D9Renderer * rend);
	void bmx_cegui_direct3d9renderer_getdisplaysize(CEGUI::Direct3D9Renderer * rend, float * width, float * height);
	void bmx_cegui_direct3d9renderer_setdisplaysize(CEGUI::Direct3D9Renderer * rend, float width, float height);
	void bmx_cegui_direct3d9renderer_getdisplaydpi(CEGUI::Direct3D9Renderer * rend, float * width, float * height);
	CEGUI::uint bmx_cegui_direct3d9renderer_getmaxtexturesize(CEGUI::Direct3D9Renderer * rend);
	BBString * bmx_cegui_direct3d9renderer_getidentifierstring(CEGUI::Direct3D9Renderer * rend);
	void bmx_cegui_direct3d9renderer_beginrendering(CEGUI::Direct3D9Renderer * rend);
	void bmx_cegui_direct3d9renderer_endrendering(CEGUI::Direct3D9Renderer * rend);
//	BBObject * bmx_cegui_direct3d9renderer_creategltexture(CEGUI::Direct3D9Renderer * rend, GLuint tex, float w, float h);

}



