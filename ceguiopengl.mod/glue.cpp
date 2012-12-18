/*
  Copyright (c) 2008-2012 Bruce A Henderson
 
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


CEGUI::OpenGLRenderer * bmx_cegui_openglrenderer_new() {
	return &CEGUI::OpenGLRenderer::create();
}

BBObject * bmx_cegui_openglrenderer_creategeometrybuffer(CEGUI::OpenGLRenderer * rend) {
	return _bah_ceguiopengl_TCEOpenGLGeometryBuffer__create(&rend->createGeometryBuffer());
}

void bmx_cegui_openglrenderer_destroygeometrybuffer(CEGUI::OpenGLRenderer * rend, CEGUI::GeometryBuffer * buffer) {
	rend->destroyGeometryBuffer(*buffer);
}

void bmx_cegui_openglrenderer_destroyallgeometrybuffers(CEGUI::OpenGLRenderer * rend) {
	rend->destroyAllGeometryBuffers();
}

BBObject * bmx_cegui_openglrenderer_createtexturetarget(CEGUI::OpenGLRenderer * rend) {
	return _bah_ceguiopengl_TCEOpenGLTextureTarget__create(rend->createTextureTarget());
}

void bmx_cegui_openglrenderer_destroytexturetarget(CEGUI::OpenGLRenderer * rend, CEGUI::TextureTarget * target) {
	rend->destroyTextureTarget(target);
}

void bmx_cegui_openglrenderer_destroyalltexturetargets(CEGUI::OpenGLRenderer * rend) {
	rend->destroyAllTextureTargets();
}

BBObject * bmx_cegui_openglrenderer_createtexture(CEGUI::OpenGLRenderer * rend, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup) {
	return _bah_ceguiopengl_TCEOpenGLTexture__create(&rend->createTexture(filename, resourceGroup));
}

BBObject * bmx_cegui_openglrenderer_createtexturewithsize(CEGUI::OpenGLRenderer * rend, float w, float h) {
	return _bah_ceguiopengl_TCEOpenGLTexture__create(&rend->createTexture(CEGUI::Size(w, h)));
}

void bmx_cegui_openglrenderer_destroytexture(CEGUI::OpenGLRenderer * rend, CEGUI::Texture * texture) {
	rend->destroyTexture(*texture);
}

void bmx_cegui_openglrenderer_destroyalltextures(CEGUI::OpenGLRenderer * rend) {
	rend->destroyAllTextures();
}

void bmx_cegui_openglrenderer_getdisplaysize(CEGUI::OpenGLRenderer * rend, float * width, float * height) {
	CEGUI::Size s(rend->getDisplaySize());
	*width = s.d_width;
	*height = s.d_height;
}


void bmx_cegui_openglrenderer_setdisplaysize(CEGUI::OpenGLRenderer * rend, float width, float height) {
	rend->setDisplaySize(CEGUI::Size(width, height));
}

void bmx_cegui_openglrenderer_getdisplaydpi(CEGUI::OpenGLRenderer * rend, float * width, float * height) {
	CEGUI::Vector2 v(rend->getDisplayDPI());
	*width = v.d_x;
	*height = v.d_y;
}

CEGUI::uint bmx_cegui_openglrenderer_getmaxtexturesize(CEGUI::OpenGLRenderer * rend) {
	return rend->getMaxTextureSize();
}

BBString * bmx_cegui_openglrenderer_getidentifierstring(CEGUI::OpenGLRenderer * rend) {
	return bah_cegui__convertUTF8ToMax(rend->getIdentifierString().data());
}

void bmx_cegui_openglrenderer_beginrendering(CEGUI::OpenGLRenderer * rend) {
	rend->beginRendering();
}

void bmx_cegui_openglrenderer_endrendering(CEGUI::OpenGLRenderer * rend) {
	rend->endRendering();
}

BBObject * bmx_cegui_openglrenderer_creategltexture(CEGUI::OpenGLRenderer * rend, GLuint tex, float w, float h) {
	// FIXME : linking issue ...
	//return _bah_ceguiopengl_TCEOpenGLTexture__create(&rend->createTexture(tex, CEGUI::Size(w, h)));
}

void bmx_cegui_openglrenderer_enableextrastatesettings(CEGUI::OpenGLRenderer * rend, int setting) {
	rend->enableExtraStateSettings(static_cast<bool>(setting));
}

void bmx_cegui_openglrenderer_grabtextures(CEGUI::OpenGLRenderer * rend) {
	rend->grabTextures();
}

void bmx_cegui_openglrenderer_restoretextures(CEGUI::OpenGLRenderer * rend) {
	rend->restoreTextures();
}


