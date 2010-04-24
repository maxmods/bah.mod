#include "cairoint.h"
#include "cairo-gl-private.h"


#include <AppKit/AppKit.h>
#include <Carbon/Carbon.h>
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>


typedef struct BBGLContext BBGLContext;

struct BBGLContext{
	int mode,width,height,depth,hertz,flags,sync;

	NSView *view;
	void *window;
	NSOpenGLContext *glContext;
};

typedef struct _cairo_maxgl_context {
    cairo_gl_context_t base;


	BBGLContext * context;
} cairo_maxgl_context_t;




// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

static void _bmx_cairo_gl_make_current(void *abstract_ctx, cairo_gl_surface_t *abstract_surface) {
printf("_bmx_cairo_gl_make_current\n");fflush(stdout);

}

static void _bmx_cairo_gl_swap_buffers(void *abstract_ctx, cairo_gl_surface_t *abstract_surface) {

printf("_bmx_cairo_gl_swap_buffers\n");fflush(stdout);

}

static void _bmx_cairo_gl_destroy(void *abstract_ctx) {
printf("_bmx_cairo_gl_destroy\n");fflush(stdout);

}


cairo_gl_context_t * bmx_cairo_glcontext_create(BBGLContext * context) {
	cairo_maxgl_context_t * ctx;
	cairo_status_t status;
	
	ctx = calloc (1, sizeof (cairo_maxgl_context_t));

	if (ctx == NULL) {
		return _cairo_gl_context_create_in_error (CAIRO_STATUS_NO_MEMORY);
	}
	
	ctx->context = context;
	ctx->base.make_current = _bmx_cairo_gl_make_current;
	ctx->base.swap_buffers = _bmx_cairo_gl_swap_buffers;
	ctx->base.destroy = _bmx_cairo_gl_destroy;


	status = _cairo_gl_context_init(&ctx->base);
	if (status) {
		free (ctx);
		return _cairo_gl_context_create_in_error(status);
	}
	
	return &ctx->base;

}



