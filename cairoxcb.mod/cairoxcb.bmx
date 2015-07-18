' Copyright (c) 2015 Bruce A Henderson
'
'  The contents of this file are subject to the Mozilla Public License
'  Version 1.1 (the "License"); you may not use this file except in
'  compliance with the License. You may obtain a copy of the License at
'  http://www.mozilla.org/MPL/
'  
'  Software distributed under the License is distributed on an "AS IS"
'  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
'  License for the specific language governing rights and limitations
'  under the License.
'  
'  The Original Code is BaH.CairoXcb.
'  
SuperStrict


Rem
bbdoc: XCB backend for Cairo.
End Rem
Module BaH.CairoXcb

ModuleInfo "Version: 1.00"
ModuleInfo "License: MPL / LGPL"
ModuleInfo "Copyright: 2015 Bruce A Henderson

?linux

Import BaH.Cairo
Import BaH.xcb


Import "common.bmx"


Type TCairoXCBSurface Extends TCairoSurface

	Rem
	bbdoc: Creates an XCB surface that draws to the given drawable. 
	returns: The newly created surface.
	about: The way that colors are represented in the drawable is specified by the provided visual.
	Parameters:
	<ul>
	<li><b>window</b> : </li>
	</ul>
	End Rem
	Function CreateForXCB:TCairoXCBSurface(window:TXCBWindow)
		Local surf:TCairoXCBSurface = New TCairoXCBSurface
		
		surf.surfacePtr = bmx_cairo_xcb_surface_create(TXCBSystemDriver(Driver).xcbPtr, window.winId)
		
		Return surf
	End Function

	Rem
	bbdoc: 
	End Rem
	Method SetSize(width:Int, height:Int)
		bmx_cairo_xcb_surface_set_size(surfacePtr, width, height)
	End Method
	
End Type


?
