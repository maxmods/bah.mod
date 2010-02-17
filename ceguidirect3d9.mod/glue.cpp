/*
  Copyright (c) 2008-2009 Bruce A Henderson
 
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

#include "glue.h"

/*
CEGUI::Direct3D9Renderer * bmx_cegui_direct3d9renderer_new() {
	return &CEGUI::Direct3D9Renderer::create();
}

BBObject * bmx_cegui_direct3d9renderer_creategeometrybuffer(CEGUI::Direct3D9Renderer * rend) {
	return _bah_ceguidirect3d9_TCEDirect3D9GeometryBuffer__create(&rend->createGeometryBuffer());
}

void bmx_cegui_direct3d9renderer_destroygeometrybuffer(CEGUI::Direct3D9Renderer * rend, CEGUI::GeometryBuffer * buffer) {
	rend->destroyGeometryBuffer(*buffer);
}

void bmx_cegui_direct3d9renderer_destroyallgeometrybuffers(CEGUI::Direct3D9Renderer * rend) {
	rend->destroyAllGeometryBuffers();
}

BBObject * bmx_cegui_direct3d9renderer_createtexturetarget(CEGUI::Direct3D9Renderer * rend) {
	return _bah_ceguidirect3d9_TCEDirect3D9TextureTarget__create(rend->createTextureTarget());
}

void bmx_cegui_direct3d9renderer_destroytexturetarget(CEGUI::Direct3D9Renderer * rend, CEGUI::TextureTarget * target) {
	rend->destroyTextureTarget(target);
}

void bmx_cegui_direct3d9renderer_destroyalltexturetargets(CEGUI::Direct3D9Renderer * rend) {
	rend->destroyAllTextureTargets();
}

BBObject * bmx_cegui_direct3d9renderer_createtexture(CEGUI::Direct3D9Renderer * rend, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	return _bah_ceguidirect3d9_TCEDirect3D9Texture__create(&rend->createTexture(filename, resourceGroup));
}

BBObject * bmx_cegui_direct3d9renderer_createtexturewithsize(CEGUI::Direct3D9Renderer * rend, float w, float h) {
	return _bah_ceguidirect3d9_TCEDirect3D9Texture__create(&rend->createTexture(CEGUI::Size(w, h)));
}

void bmx_cegui_direct3d9renderer_destroytexture(CEGUI::Direct3D9Renderer * rend, CEGUI::Texture * texture) {
	rend->destroyTexture(*texture);
}

void bmx_cegui_direct3d9renderer_destroyalltextures(CEGUI::Direct3D9Renderer * rend) {
	rend->destroyAllTextures();
}

void bmx_cegui_direct3d9renderer_getdisplaysize(CEGUI::Direct3D9Renderer * rend, float * width, float * height) {
	CEGUI::Size s(rend->getDisplaySize());
	*width = s.d_width;
	*height = s.d_height;
}


void bmx_cegui_direct3d9renderer_setdisplaysize(CEGUI::Direct3D9Renderer * rend, float width, float height) {
	rend->setDisplaySize(CEGUI::Size(width, height));
}

void bmx_cegui_direct3d9renderer_getdisplaydpi(CEGUI::Direct3D9Renderer * rend, float * width, float * height) {
	CEGUI::Vector2 v(rend->getDisplayDPI());
	*width = v.d_x;
	*height = v.d_y;
}

CEGUI::uint bmx_cegui_direct3d9renderer_getmaxtexturesize(CEGUI::Direct3D9Renderer * rend) {
	return rend->getMaxTextureSize();
}

BBString * bmx_cegui_direct3d9renderer_getidentifierstring(CEGUI::Direct3D9Renderer * rend) {
	return bah_cegui__convertUTF8ToMax(rend->getIdentifierString().data());
}

void bmx_cegui_direct3d9renderer_beginrendering(CEGUI::Direct3D9Renderer * rend) {
	rend->beginRendering();
}

void bmx_cegui_direct3d9renderer_endrendering(CEGUI::Direct3D9Renderer * rend) {
	rend->endRendering();
}

BBObject * bmx_cegui_direct3d9renderer_creategltexture(CEGUI::Direct3D9Renderer * rend, GLuint tex, float w, float h) {
	// FIXME : linking issue ...
	//return _bah_ceguidirect3d9_TCEOpenGLTexture__create(&rend->createTexture(tex, CEGUI::Size(w, h)));
}

void bmx_cegui_direct3d9renderer_enableextrastatesettings(CEGUI::Direct3D9Renderer * rend, int setting) {
	rend->enableExtraStateSettings(static_cast<bool>(setting));
}

void bmx_cegui_direct3d9renderer_grabtextures(CEGUI::Direct3D9Renderer * rend) {
	rend->grabTextures();
}

void bmx_cegui_direct3d9renderer_restoretextures(CEGUI::Direct3D9Renderer * rend) {
	rend->restoreTextures();
}
*/

