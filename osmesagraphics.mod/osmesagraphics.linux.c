
#include <stdio.h>
#include "osmesa.h"
#include <assert.h>

/* Added by BaH */
#include <brl.mod/blitz.mod/blitz.h>
#include "GL/glu.h"

extern void bbSystemPoll();

typedef int (* GLXSWAPINTERVALEXT) (int);

GLXSWAPINTERVALEXT glXSwapIntervalEXT;

enum{
	MODE_SHARED=0,
	MODE_WIDGET=1,
	MODE_WINDOW=2,
	MODE_DISPLAY=3
};

enum{
	FLAGS_BACKBUFFER=	0x2,
	FLAGS_ALPHABUFFER=	0x4,
	FLAGS_DEPTHBUFFER=	0x8,
	FLAGS_STENCILBUFFER=0x10,
	FLAGS_ACCUMBUFFER=	0x20,
	FLAGS_FULLSCREEN=0x80000000
};

typedef struct BBGLContext BBGLContext;

struct BBGLContext{
	int mode,width,height,depth,flags,sync;
	void *buffer;
	OSMesaContext glContext;
};

// glgraphics.bmx interface

int bbOSMesaGLGraphicsGraphicsModes( int *imodes,int maxcount );
BBGLContext *bbOSMesaGLGraphicsAttachGraphics( void * buffer,int flags );
BBGLContext *bbOSMesaGLGraphicsCreateGraphics( int width,int height,int depth, void * buffer,int flags );
void bbOSMesaGLGraphicsGetSettings( BBGLContext *context,int *width,int *height,int *depth,int *hz,int *flags );
void bbOSMesaGLGraphicsClose( BBGLContext *context );
void bbOSMesaGLGraphicsSetGraphics( BBGLContext *context );
void bbOSMesaGLGraphicsFlip( int sync );
void bbOSMesaGLExit();

static BBGLContext *_currentContext;
static BBGLContext *_activeContext;
static BBGLContext *_sharedContext;

#define INTERLACE      0x010
#define DBLSCAN        0x020

int _initDisplay(){
	atexit( bbOSMesaGLExit );
	return 0;
}

int bbOSMesaGLGraphicsGraphicsModes( int *imodes,int maxcount ){
	return 0;
}

static void _swapBuffers( BBGLContext *context ){
	if( !context ) return;
}

BBGLContext *bbOSMesaGLGraphicsAttachGraphics( void * buffer,int flags ){
	BBGLContext *context=(BBGLContext*)malloc( sizeof(BBGLContext) );
	memset( context,0,sizeof(BBGLContext) );
	context->mode=MODE_WIDGET;	
	context->flags=flags;
	context->sync=-1;	
	context->buffer=buffer;
	return context;
}

void _makeCurrent(	BBGLContext *context ){

	OSMesaMakeCurrent(context->glContext, context->buffer, GL_UNSIGNED_BYTE, context->width, context->height);

	_currentContext=context;
}

static void _validateContext( BBGLContext *context ){
	OSMesaContext		sharedcontext=0;

	if( !context || context->glContext ) return;

	if (_initDisplay()) return;

	if( _sharedContext ) sharedcontext=_sharedContext->glContext;
	
	context->glContext=OSMesaCreateContextExt(OSMESA_RGBA, 16, 0, 0, sharedcontext);

	OSMesaMakeCurrent(context->glContext, context->buffer, GL_UNSIGNED_BYTE, context->width, context->height);	
}

void bbOSMesaGLGraphicsGetSettings( BBGLContext *context,int *width,int *height,int *depth,int *hertz,int *flags ){
	*width=context->width;
	*height=context->height;
	*depth=context->depth;
	*flags=context->flags;
}

void bbOSMesaGLGraphicsShareContexts(){
	if( _sharedContext ) return;
	_sharedContext=bbOSMesaGLGraphicsCreateGraphics(0,0,0,0,0);
}

BBGLContext *bbOSMesaGLGraphicsCreateGraphics( int width,int height,int depth,void * buffer,int flags ){
	OSMesaContext context = 0;

	int count,i;
	int displaymode;
	int initingShared=0;
	OSMesaContext sharedcontext=0;

	if (_initDisplay()) return 0;
	
	if( width==0 && height==0 )
	{
		width=100;
		height=100;
		initingShared=1;
	}
	else
	{
		if( _sharedContext ) sharedcontext=_sharedContext->glContext;
		
	}

	if (!depth) {
		depth = 32;
	}

	context = OSMesaCreateContextExt(OSMESA_RGBA, 8, 0, 0, NULL);//sharedcontext);


	BBGLContext *bbcontext=(BBGLContext*)malloc( sizeof(BBGLContext) );
	memset( bbcontext,0,sizeof(BBGLContext) );
	//bbcontext->mode=displaymode;	
	bbcontext->width=width;	
	bbcontext->height=height;	
	bbcontext->depth=32;	
	//bbcontext->hertz=hz;
	bbcontext->flags=flags;
	bbcontext->sync=-1;	
	bbcontext->buffer=buffer;
	bbcontext->glContext=context;
	return bbcontext;
}

void bbOSMesaGLGraphicsSetGraphics( BBGLContext *context ){
	if( context ){
		//_validateSize( context );
		//_validateContext( context );
	}

	if( !context || context==_currentContext ) return;

	_makeCurrent(context);
	_activeContext=context;
}

void bbOSMesaGLGraphicsFlip( int sync ){
	if( !_currentContext ) return;

	sync=sync ? 1 : 0;
	if( sync!=_currentContext->sync ){
		_currentContext->sync=sync;
	}

	// ensure all drawing is finished
	glFinish();
	_swapBuffers( _currentContext );
}

void bbOSMesaGLGraphicsClose( BBGLContext *context ){
	if (context){
		if (_currentContext==context) _currentContext=0;

		if (context->glContext) 
		{
			OSMesaDestroyContext(context->glContext);	
		}

		free( context );
	}
}

void bbOSMesaGLExit(){
	bbOSMesaGLGraphicsClose( _currentContext );
	bbOSMesaGLGraphicsClose( _sharedContext );
	_currentContext=0;
	_sharedContext=0;
}
