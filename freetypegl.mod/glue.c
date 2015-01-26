/*
 Copyright (c) 2014-2015 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "freetype-gl.h"
#include "vertex-buffer.h"
#include "font-manager.h"
#include "shader.h"
#include "mat4.h"

#include <brl.mod/blitz.mod/blitz.h>

typedef struct {
    float x, y, z;    // position
    float s, t;       // texture
    float r, g, b, a; // color
	float shift, gamma;
} vertex_t;

struct bmx_font_buffer {

	font_manager_t * manager;
	texture_font_t * font;
	vertex_buffer_t * buffer;
	
	mat4 model, view, projection;
	int height;
	GLuint shader;
	GLuint ufl_texture, ufl_pixel, ufl_model, ufl_view, ufl_projection;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	float gamma;
};

GLuint bmx_freetypegl_shader_load_source( const char * vert_source, const char * frag_source );
texture_font_t * font_manager_get_from_memory( font_manager_t *self, void * mem, int len, const char * filename, const float size );

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

font_manager_t * bmx_freetypegl_font_manager_new(int atlasWidth, int atlasHeight) {
	return font_manager_new(atlasWidth, atlasHeight, 3);
}

void bmx_freetypegl_font_manager_free(font_manager_t * manager) {
	font_manager_delete(manager);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_freetypegl_texture_font_load_glyphs(struct bmx_font_buffer * buf, BBString * text) {
#ifdef WIN32
	return texture_font_load_glyphs(buf->font, text->buf);
#else
	wchar_t s[text->length + 1];
	int n;
	for (n = 0; n < text->length; n++) {
		s[n] = text->buf[n];
	}
	return texture_font_load_glyphs(buf->font, s);
#endif
}

struct bmx_font_buffer * bmx_freetypegl_font_buffer_new(font_manager_t * manager, BBString * fontPath, float size, BBString * vertSource, BBString *fragSource, void * mem, int len) {

	glewInit();

	struct bmx_font_buffer * buf = malloc(sizeof(struct bmx_font_buffer));

	buf->manager = manager;
	
	char * path = bbStringToUTF8String(fontPath);
	if (mem) {
		buf->font = font_manager_get_from_memory(manager, mem, len, path, size);
	} else {
		buf->font = font_manager_get_from_filename(manager, path, size);
	}
	bbMemFree(path);

	buf->buffer = vertex_buffer_new( "vertex:3f,tex_coord:2f,color:4f,ashift:1f,agamma:1f" );

	// shaders
	char * vs = bbStringToCString(vertSource);
	char * fs = bbStringToCString(fragSource);
	buf->shader = bmx_freetypegl_shader_load_source(vs, fs);
	bbMemFree(fs);
	bbMemFree(vs);
	
	buf->ufl_texture = glGetUniformLocation( buf->shader, "texture" );
	buf->ufl_pixel = glGetUniformLocation( buf->shader, "pixel" );

	buf->ufl_model = glGetUniformLocation( buf->shader, "model" );
	buf->ufl_view = glGetUniformLocation( buf->shader, "view" );
	buf->ufl_projection = glGetUniformLocation( buf->shader, "projection" );
	
	buf->gamma = 1;

	mat4_set_identity( &buf->projection );
	mat4_set_identity( &buf->model );
	mat4_set_identity( &buf->view );

	return buf;
}

void bmx_freetypegl_font_buffer_free(struct bmx_font_buffer * buf) {
	vertex_buffer_delete(buf->buffer);
	free(buf);
}

void bmx_freetypegl_font_buffer_draw(struct bmx_font_buffer * buf, float x, float y, BBString * text) {
	size_t i;
	float r0 = buf->r0/255.0, g0 = buf->g0/255.0, b0 = buf->b0/255.0, a0 = buf->a0/255.0;
	float r1 = buf->r1/255.0, g1 = buf->g1/255.0, b1 = buf->b1/255.0, a1 = buf->a1/255.0;

	// top down rendering
	y = buf->height - y - buf->font->size;

	for( i=0; i<text->length; ++i ) {

		texture_glyph_t *glyph = texture_font_get_glyph( buf->font, (wchar_t)text->buf[i] );

		if( glyph != NULL ) {
			float kerning = 0.0f;
			if( i > 0) {
				kerning = texture_glyph_get_kerning( glyph, (wchar_t)text->buf[i-1] );
			}
			x += kerning;
			int x0  = (int)( x + glyph->offset_x );
			int y0  = (int)( y + glyph->offset_y);
			int x1  = (int)( x0 + glyph->width );
			int y1  = (int)( y0 - glyph->height );
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;

			GLuint indices[] = {0, 1, 2, 0, 2, 3};
			vertex_t vertices[] = {
				{ x0,y0,0,  s0,t0,  r0,g0,b0,a0,  0, buf->gamma },
				{ x0,y1,0,  s0,t1,  r1,g1,b1,a1,  0, buf->gamma },
				{ x1,y1,0,  s1,t1,  r1,g1,b1,a1,  0, buf->gamma },
				{ x1,y0,0,  s1,t0,  r0,g0,b0,a0,  0, buf->gamma }
			};

			vertex_buffer_push_back( buf->buffer, vertices, 4, indices, 6 );
			x += glyph->advance_x;
		}
	}
}

void bmx_freetypegl_font_buffer_setcolor(struct bmx_font_buffer * buf, int r, int g, int b, int a) {
	buf->r0 = buf->r1 = r;
	buf->g0 = buf->g1 = g;
	buf->b0 = buf->b1 = b;
	buf->a0 = buf->a1 = a;
}

void bmx_freetypegl_font_buffer_setgradientcolor(struct bmx_font_buffer * buf, int r0, int g0, int b0, int a0, int r1, int g1, int b1, int a1) {
	buf->r0 = r0;
	buf->g0 = g0;
	buf->b0 = b0;
	buf->a0 = a0;

	buf->r1 = r1;
	buf->b1 = b1;
	buf->g1 = g1;
	buf->a1 = a1;
}

void bmx_freetypegl_font_buffer_setviewport(struct bmx_font_buffer * buf, int x, int y, int width, int height) {
	mat4_set_orthographic(&buf->projection, 0, width, 0, height, -1, 1);
	buf->height = height;
}

// clear all text in the buffer
void bmx_freetypegl_font_buffer_clear(struct bmx_font_buffer * buf) {
	vertex_buffer_clear(buf->buffer);
}

// render text in our buffer
void bmx_freetypegl_font_buffer_render(struct bmx_font_buffer * buf) {
	glBindTexture( GL_TEXTURE_2D, buf->manager->atlas->id );

	glUseProgram( buf->shader );
	
	glUniform1i( buf->ufl_texture, 0 );
	glUniformMatrix4fv( buf->ufl_model, 1, 0, buf->model.data);
	glUniformMatrix4fv( buf->ufl_view, 1, 0, buf->view.data);
	glUniformMatrix4fv( buf->ufl_projection, 1, 0, buf->projection.data);
	glUniform3f( buf->ufl_pixel, 1.0/buf->manager->atlas->width, 1.0/buf->manager->atlas->height, buf->manager->atlas->depth );
	vertex_buffer_render( buf->buffer, GL_TRIANGLES );
	
	glUseProgram(0);
}

void bmx_freetypegl_font_buffer_setoutlinetype(struct bmx_font_buffer * buf, int value) {
	buf->font->outline_type = value;
}

void bmx_freetypegl_font_buffer_setoutlinethickness(struct bmx_font_buffer * buf, float value) {
	buf->font->outline_thickness = value;
}

float bmx_freetypegl_texture_font_height(struct bmx_font_buffer * buf) {
	return buf->font->height;
}

float bmx_freetypegl_texture_font_linegap(struct bmx_font_buffer * buf) {
	return buf->font->linegap;
}

float bmx_freetypegl_texture_font_ascender(struct bmx_font_buffer * buf) {
	return buf->font->ascender;
}

float bmx_freetypegl_texture_font_descender(struct bmx_font_buffer * buf) {
	return buf->font->descender;
}

void bmx_freetypegl_font_buffer_setgamma(struct bmx_font_buffer * buf, float gamma) {
	buf->gamma = gamma;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

GLuint bmx_freetypegl_shader_load_source( const char * vert_source, const char * frag_source ) {
    GLuint handle = glCreateProgram( );
    GLint link_status;

    if( vert_source && strlen( vert_source ) )
    {
        GLuint vert_shader = shader_compile( vert_source, GL_VERTEX_SHADER);
        glAttachShader( handle, vert_shader);
        glDeleteShader( vert_shader );
    }
    if( frag_source && strlen( frag_source ) )
    {
        GLuint frag_shader = shader_compile( frag_source, GL_FRAGMENT_SHADER);
        glAttachShader( handle, frag_shader);
        glDeleteShader( frag_shader );
    }

    glLinkProgram( handle );

    glGetProgramiv( handle, GL_LINK_STATUS, &link_status );
    if (link_status == GL_FALSE)
    {
        GLchar messages[256];
        glGetProgramInfoLog( handle, sizeof(messages), 0, &messages[0] );
        fprintf( stderr, "%s\n", messages );
        exit(1);
    }
    return handle;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

texture_font_t * font_manager_get_from_memory( font_manager_t *self, void * mem, int len, const char * filename, const float size ) {
	size_t i;
	texture_font_t *font;
	
	//assert( self );
	for( i=0; i<vector_size(self->fonts); ++i ) {
		font = * (texture_font_t **) vector_get( self->fonts, i );
		if( (strcmp(font->filename, filename) == 0) && ( font->size == size) ) {
			return font;
		}
	}
	font = texture_font_new_from_memory( self->atlas, size, mem, len );
	if( font ) {
		font->filename = strdup(filename);
		vector_push_back( self->fonts, &font );
		texture_font_load_glyphs( font, self->cache );
		return font;
	}
	fprintf( stderr, "Unable to load \"%s\" (size=%.1f) from memory\n", filename, size );
	return 0;
}
