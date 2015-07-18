/*
 Copyright (c) 2015 Bruce A Henderson

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/
#include "glue.h"
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

typedef int (*BBAsyncOp)( int asyncInfo );
typedef void (*BBSyncOp)( BBObject *syncInfo,int asyncRet );

typedef struct AsyncOp{
	struct AsyncOp *succ;
	BBSyncOp syncOp;
	BBObject *syncInfo;
	int asyncRet;
	BBAsyncOp asyncOp;
	int asyncInfo;
}AsyncOp;

static AsyncOp *asyncOps;
static pthread_mutex_t asyncMutex=PTHREAD_MUTEX_INITIALIZER;
static int async_pipe[2];

static void postAsyncOp( AsyncOp *p ){
	int ch=0;

	AsyncOp **q=&asyncOps,*t;

	p->succ=0;

	pthread_mutex_lock( &asyncMutex );

	while( *q ) q=&(*q)->succ;
	*q=p;
	
	write( async_pipe[1],&ch,1 );

	pthread_mutex_unlock( &asyncMutex );
}

int bbSystemAsyncFD(){
	return async_pipe[0];
}

void bbSystemFlushAsyncOps(){
	int rd;
	AsyncOp *p;
	
	if( !asyncOps ) return;
	
	pthread_mutex_lock( &asyncMutex );
	
	p=asyncOps;
	asyncOps=0;
	
	if( !ioctl( async_pipe[0],FIONREAD,&rd ) ){
		static char *buf;
		static int buf_sz;
		if( rd>buf_sz ){
			free( buf );
			buf=(char*)malloc( rd );
			buf_sz=rd;
		}
		read( async_pipe[0],buf,rd );
	}
	
	pthread_mutex_unlock( &asyncMutex );

	while( p ){
		AsyncOp *t=p->succ;
		p->syncOp( p->syncInfo,p->asyncRet );
		if( p->asyncOp ){
			BBRELEASE( p->syncInfo );
		}
		free( p );
		p=t;
	}
}

void *asyncOpThread( void *t ){
	AsyncOp *p=(AsyncOp*)t;
	p->asyncRet=p->asyncOp( p->asyncInfo );
	postAsyncOp( p );
	return 0;
}

void bbSystemPostSyncOp( BBSyncOp syncOp,BBObject *syncInfo,int asyncRet ){
	AsyncOp *p=(AsyncOp*)malloc( sizeof( AsyncOp ) );
	p->asyncOp=0;
	p->asyncRet=asyncRet;
	p->syncOp=syncOp;
	p->syncInfo=syncInfo;
	postAsyncOp( p );
}

void bbSystemStartAsyncOp( BBAsyncOp asyncOp,int asyncInfo,BBSyncOp syncOp,BBObject *syncInfo ){
	pthread_t thread;
	AsyncOp *p=(AsyncOp*)malloc( sizeof( AsyncOp ) );
	BBRETAIN( syncInfo );
	p->asyncOp=asyncOp;
	p->asyncInfo=asyncInfo;
	p->syncOp=syncOp;
	p->syncInfo=syncInfo;
	pthread_create( &thread,0,asyncOpThread,p );
	pthread_detach( thread );
}

/* ---------------------------------------------- */

struct winobj_node {
	struct tree_root link;
	xcb_window_t win;
	BBOBJECT obj;
};

static struct tree_root *winobj_root = 0;

#define generic_compare(x, y) (((x) > (y)) - ((x) < (y)))

static int compare_winobj_nodes(const void *x, const void *y) {

        struct winobj_node * node_x = (struct winobj_node *)x;
        struct winobj_node * node_y = (struct winobj_node *)y;

        return generic_compare(node_x->win, node_y->win);
}

void bmx_winobj_bind(xcb_window_t win, BBObject *obj) {
	struct winobj_node * node = (struct winobj_node *)malloc(sizeof(struct winobj_node));
	node->win = win;
	node->obj = obj;
	BBRETAIN(obj);
	
	struct winobj_node * old_node = (struct winobj_node *)tree_map(&node->link, compare_winobj_nodes, &winobj_root);

	if (&node->link != &old_node->link) {
		BBRELEASE(old_node->obj);
		// key already exists. Store the value in this node.
		old_node->obj = obj;
		// delete the new node, since we don't need it
		free(node);
	}
}

BBObject * bmx_winobj_find(xcb_window_t win) {
	struct winobj_node node;
	node.win = win;
	
	struct winobj_node * found = (struct winobj_node *)tree_search(&node, compare_winobj_nodes, winobj_root);
	
	if (found) {
		return found->obj;
	}
	
	return &bbNullObject;
}

int bmx_winobj_unbind(xcb_window_t win) {
	struct winobj_node node;
	node.win = win;
	
	struct winobj_node * found = (struct winobj_node *)tree_search(&node, compare_winobj_nodes, winobj_root);
	
	if (found) {
		BBRELEASE(found->obj);
		tree_del(&found->link, &winobj_root);
		free(found);
		return 1;
	} else {
		return 0;
	}
}

/* ---------------------------------------------- */


void bbXCBSystemEmitEvent( int id,BBObject *source,int data,int mods,int x,int y,BBObject *extra ){
	BBObject *event=brl_event_CreateEvent( id,source,data,mods,x,y,extra );
	brl_event_EmitEvent( event );
}

struct MaxXcb * bmx_xcb_system_startup(int * screenCount) {

	struct MaxXcb * xcb = malloc(sizeof(struct MaxXcb));

	xcb->conn = xcb_connect(NULL, NULL);

	xcb_intern_atom_cookie_t pcookie = xcb_intern_atom(xcb->conn, 1, 12, "WM_PROTOCOLS");
	xcb->protocolAtom = xcb_intern_atom_reply(xcb->conn, pcookie, 0);

	xcb_intern_atom_cookie_t cookie = xcb_intern_atom(xcb->conn, 0, 16, "WM_DELETE_WINDOW");
	xcb->deleteAtom = xcb_intern_atom_reply(xcb->conn, cookie, 0);

	xcb->connFD = xcb_get_file_descriptor(xcb->conn);
	
	pipe( async_pipe );

	return xcb;
}


int bmx_xcb_system_desktopwidth(struct MaxXcb * xcb) {
	xcb_screen_t * screen = xcb_setup_roots_iterator(xcb_get_setup(xcb->conn)).data;
	return screen->width_in_pixels;
}

int bmx_xcb_system_desktopheight(struct MaxXcb * xcb) {
	xcb_screen_t * screen = xcb_setup_roots_iterator(xcb_get_setup(xcb->conn)).data;
	return screen->height_in_pixels;
}

int bmx_xcb_system_desktopdepth(struct MaxXcb * xcb) {
	xcb_screen_t * screen = xcb_setup_roots_iterator(xcb_get_setup(xcb->conn)).data;
	return screen->root_depth;
}


void bmx_xcb_system_emit_event(struct MaxXcb * xcb, xcb_generic_event_t * event, BBObject *source) {
	switch (event->response_type & ~0x80) {
		case XCB_BUTTON_PRESS:
		{
			xcb_button_press_event_t * ev = (xcb_button_press_event_t *)event;
			
			return;
		}
		case XCB_BUTTON_RELEASE:
		{
			xcb_button_release_event_t *ev = (xcb_button_release_event_t *)event;
			return;
		}
		case XCB_MOTION_NOTIFY:
		{
			xcb_motion_notify_event_t *ev = (xcb_motion_notify_event_t *)event;
			return;
		}
		case XCB_ENTER_NOTIFY:
		{
			xcb_enter_notify_event_t *ev = (xcb_enter_notify_event_t *)event;
			return;
		}
		case XCB_LEAVE_NOTIFY:
		{
			xcb_leave_notify_event_t *ev = (xcb_leave_notify_event_t *)event;
			return;
		}
		case XCB_KEY_PRESS:
		{
			xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
			return;
		}
		case XCB_KEY_RELEASE:
		{
			xcb_key_release_event_t *ev = (xcb_key_release_event_t *)event;
			return;
		}
		case XCB_FOCUS_IN:
		{
			xcb_focus_in_event_t *ev = (xcb_focus_in_event_t *)event;
			bbXCBSystemEmitEvent(BBEVENT_APPRESUME, &bbNullObject, 0, 0, 0, 0, &bbNullObject);
			return;
		}
		case XCB_FOCUS_OUT:
		{
			xcb_focus_out_event_t *ev = (xcb_focus_out_event_t *)event;
			bbXCBSystemEmitEvent(BBEVENT_APPSUSPEND, &bbNullObject, 0, 0, 0, 0, &bbNullObject);
			return;
		}
		case XCB_CLIENT_MESSAGE:
		{
			if ((*(xcb_client_message_event_t*)event).data.data32[0] == xcb->deleteAtom->atom) {
				bbXCBSystemEmitEvent(BBEVENT_APPTERMINATE, &bbNullObject, 0, 0, 0, 0, &bbNullObject);
			}
			return;
		}
		case XCB_EXPOSE:
		{
			xcb_expose_event_t *ev = (xcb_expose_event_t *)event;
			BBObject * obj = bmx_winobj_find(ev->window);
			if (obj != &bbNullObject) {
				bbXCBSystemEmitEvent(BBEVENT_GADGETPAINT, obj, 0, 0, 0, 0, &bbNullObject);
			}
			return;
		}
		case XCB_CONFIGURE_NOTIFY:
		{
			xcb_configure_notify_event_t *ev = (xcb_configure_notify_event_t *)event;
			BBObject * obj = bmx_winobj_find(ev->window);
			if (obj != &bbNullObject) {
				bbXCBSystemEmitEvent(BBEVENT_WINDOWSIZE, obj, 0, 0, ev->width, ev->height, &bbNullObject);
			}
			return;
		}
		default:
			return;
	}
	
}

void bmx_xcb_system_poll(struct MaxXcb * xcb) {
	xcb_generic_event_t * event;
	
	while (event = xcb_poll_for_event(xcb->conn)) {
		bmx_xcb_system_emit_event(xcb, event, &bbNullObject);
	}

	free(event);
	
	bbSystemFlushAsyncOps();
}

void bmx_xcb_system_wait(struct MaxXcb * xcb) {
	fd_set in_fds;
	struct timeval tv;
	
	FD_ZERO( &in_fds );
	FD_SET( xcb->connFD,&in_fds );
	FD_SET( async_pipe[0],&in_fds );
	
  	tv.tv_sec=10;
    tv.tv_usec=0;

	select( (xcb->connFD>async_pipe[0] ? xcb->connFD : async_pipe[0]) + 1,&in_fds,0,0,&tv );

	bmx_xcb_system_poll(xcb);
}

void bmx_xcb_window_set_title(struct MaxXcb * xcb, xcb_window_t winId, BBString * title) {
	char * t = bbStringToUTF8String(title);

	xcb_change_property (xcb->conn, XCB_PROP_MODE_REPLACE, winId, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(t), t);
	
	bbMemFree(t);
}

xcb_window_t bmx_xcb_window_create(BBObject * obj, struct MaxXcb * xcb, BBString * title, xcb_window_t parent, int x, int y, int w, int h, int flags) {

	xcb_window_t winId = xcb_generate_id(xcb->conn);
	xcb_screen_t * screen = 0;

	if (!parent) {
		screen = xcb_setup_roots_iterator(xcb_get_setup(xcb->conn)).data;
	}

	uint32_t mask = XCB_CW_EVENT_MASK;
    uint32_t valwin[] = {XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | 
			XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
			XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};

	xcb_create_window(xcb->conn, XCB_COPY_FROM_PARENT, winId, (parent) ? parent : screen->root, x, y, w, h, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, valwin);

	bmx_winobj_bind(winId, obj);

	// use our atom for window close event
	xcb_change_property(xcb->conn, XCB_PROP_MODE_REPLACE, winId, xcb->protocolAtom->atom, 4, 32, 1, &xcb->deleteAtom->atom);

	bmx_xcb_window_set_title(xcb, winId, title);

	return winId;
}

void bmx_xcb_window_show(struct MaxXcb * xcb, xcb_window_t winId) {
	xcb_map_window(xcb->conn, winId);
	xcb_flush(xcb->conn);
}

void bmx_xcb_window_hide(struct MaxXcb * xcb, xcb_window_t winId) {
	xcb_unmap_window(xcb->conn, winId);
	xcb_flush(xcb->conn);
}

void bmx_xcb_connection_flush(struct MaxXcb * xcb) {
	xcb_flush(xcb->conn);
}

