/*
  Copyright (c) 2008-2011 Bruce A Henderson
 
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
#include <CEGUIOpenGLRenderer.h>
#include "../cegui.mod/glue.h"

extern "C" {

#include "blitz.h"

	BBObject * _bah_ceguiopengl_TCEOpenGLGeometryBuffer__create(CEGUI::GeometryBuffer * buffer);
	BBObject * _bah_ceguiopengl_TCEOpenGLTextureTarget__create(CEGUI::TextureTarget * target);
	BBObject * _bah_ceguiopengl_TCEOpenGLTexture__create(CEGUI::Texture * buffer);

	CEGUI::OpenGLRenderer * bmx_cegui_openglrenderer_new();
	BBObject * bmx_cegui_openglrenderer_creategeometrybuffer(CEGUI::OpenGLRenderer * rend);
	void bmx_cegui_openglrenderer_destroygeometrybuffer(CEGUI::OpenGLRenderer * rend, CEGUI::GeometryBuffer * buffer);
	void bmx_cegui_openglrenderer_destroyallgeometrybuffers(CEGUI::OpenGLRenderer * rend);
	BBObject * bmx_cegui_openglrenderer_createtexturetarget(CEGUI::OpenGLRenderer * rend);
	void bmx_cegui_openglrenderer_destroytexturetarget(CEGUI::OpenGLRenderer * rend, CEGUI::TextureTarget * target);
	void bmx_cegui_openglrenderer_destroyalltexturetargets(CEGUI::OpenGLRenderer * rend);
	BBObject * bmx_cegui_openglrenderer_createtexture(CEGUI::OpenGLRenderer * rend, const CEGUI::utf8 * filename, const CEGUI::utf8 * resourceGroup);
	BBObject * bmx_cegui_openglrenderer_createtexturewithsize(CEGUI::OpenGLRenderer * rend, float w, float h);
	void bmx_cegui_openglrenderer_destroytexture(CEGUI::OpenGLRenderer * rend, CEGUI::Texture * texture);
	void bmx_cegui_openglrenderer_destroyalltextures(CEGUI::OpenGLRenderer * rend);
	void bmx_cegui_openglrenderer_getdisplaysize(CEGUI::OpenGLRenderer * rend, float * width, float * height);
	void bmx_cegui_openglrenderer_setdisplaysize(CEGUI::OpenGLRenderer * rend, float width, float height);
	void bmx_cegui_openglrenderer_getdisplaydpi(CEGUI::OpenGLRenderer * rend, float * width, float * height);
	CEGUI::uint bmx_cegui_openglrenderer_getmaxtexturesize(CEGUI::OpenGLRenderer * rend);
	BBString * bmx_cegui_openglrenderer_getidentifierstring(CEGUI::OpenGLRenderer * rend);
	void bmx_cegui_openglrenderer_beginrendering(CEGUI::OpenGLRenderer * rend);
	void bmx_cegui_openglrenderer_endrendering(CEGUI::OpenGLRenderer * rend);
	BBObject * bmx_cegui_openglrenderer_creategltexture(CEGUI::OpenGLRenderer * rend, GLuint tex, float w, float h);
	void bmx_cegui_openglrenderer_enableextrastatesettings(CEGUI::OpenGLRenderer * rend, int setting);
	void bmx_cegui_openglrenderer_grabtextures(CEGUI::OpenGLRenderer * rend);
	void bmx_cegui_openglrenderer_restoretextures(CEGUI::OpenGLRenderer * rend);

}



