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
#ifndef BMX_XCB_GLUE_H
#define BMX_XCB_GLUE_H

#include <stdio.h>
#include <xcb/xcb.h>

#include "brl.mod/blitz.mod/blitz.h"
#include "brl.mod/blitz.mod/tree/tree.h"
#include "brl.mod/event.mod/event.h"

extern void bbSystemEmitEvent( int id,BBObject *source,int data,int mods,int x,int y,BBObject *extra );

extern void brl_event_EmitEvent( BBObject *event );
extern BBObject *brl_event_CreateEvent( int id,BBObject *source,int data,int mods,int x,int y,BBObject *extra );


struct MaxXcb {
	xcb_connection_t * conn;
	xcb_intern_atom_reply_t * protocolAtom;
	xcb_intern_atom_reply_t * deleteAtom;
	int connFD;
};


#endif // BMX_XCB_GLUE_H
