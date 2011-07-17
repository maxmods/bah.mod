
Strict

Rem
bbdoc: OSMesa OpenGL Graphics
End Rem
Module BaH.OSMesaGraphics

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Mark Sibly, Simon Armstrong and Bruce A Henderson"
ModuleInfo "License: Blitz Shared Source Code"
ModuleInfo "Copyright: Blitz Research Ltd"


ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: OSMesa port from original BRL.GLGraphics module"

?Linux

Import BaH.OsMesa

Import "include/*.h"
Import "osmesagraphics.linux.c"


Import BRL.Graphics
Import BRL.Pixmap
Import Pub.OpenGL

Private

Incbin "gldrawtextfont.bin"

Extern
	Function bbOSMesaGLGraphicsShareContexts()
	Function bbOSMesaGLGraphicsGraphicsModes( buf:Byte Ptr,size )
	Function bbOSMesaGLGraphicsAttachGraphics( widget,flags )
	Function bbOSMesaGLGraphicsCreateGraphics( width,height,depth,hertz,flags )
	Function bbOSMesaGLGraphicsGetSettings( context,width Var,height Var,depth Var,hertz Var,flags Var )
	Function bbOSMesaGLGraphicsClose( context )	
	Function bbOSMesaGLGraphicsSetGraphics( context )
	Function bbOSMesaGLGraphicsFlip( sync )
End Extern

Public

Type TOSMesaGLGraphics Extends TGraphics

	Method Driver:TOSMesaGLGraphicsDriver()
		Assert _context
		Return OSMesaGLGraphicsDriver()
	End Method
	
	Method GetSettings( width Var,height Var,depth Var,hertz Var,flags Var )
		Assert _context
		Local w,h,d,r,f
		bbOSMesaGLGraphicsGetSettings _context,w,h,d,r,f
		width=w
		height=h
		depth=d
		hertz=r
		flags=f
	End Method
	
	Method Close()
		If Not _context Return
		bbOSMesaGLGraphicsClose( _context )
		_context=0
	End Method
	
	Field _context
	
End Type

Type TOSMesaGLGraphicsDriver Extends TGraphicsDriver

	Method GraphicsModes:TGraphicsMode[]()
		Local buf[1024*4]
		Local count=bbOSMesaGLGraphicsGraphicsModes( buf,1024 )
		Local modes:TGraphicsMode[count],p:Int Ptr=buf
		For Local i=0 Until count
			Local t:TGraphicsMode=New TGraphicsMode
			t.width=p[0]
			t.height=p[1]
			t.depth=p[2]
			t.hertz=p[3]
			modes[i]=t
			p:+4
		Next
		Return modes
	End Method
	
	Method AttachGraphics:TOSMesaGLGraphics( widget,flags )
		Local t:TOSMesaGLGraphics=New TOSMesaGLGraphics
		t._context=bbOSMesaGLGraphicsAttachGraphics( widget,flags )
		Return t
	End Method
	
	Method CreateGraphics:TOSMesaGLGraphics( width,height,depth,hertz,flags )
		Local t:TOSMesaGLGraphics=New TOSMesaGLGraphics
		t._context=bbOSMesaGLGraphicsCreateGraphics( width,height,depth,hertz,flags )

		Return t
	End Method
	
	Method SetGraphics( g:TGraphics )
		Local context
		Local t:TOSMesaGLGraphics=TOSMesaGLGraphics( g )
		If t context=t._context
		bbOSMesaGLGraphicsSetGraphics context

	End Method
	
	Method Flip( sync )
		bbOSMesaGLGraphicsFlip sync
	End Method
	
End Type

Rem
bbdoc: Get OpenGL graphics driver
returns: An OpenGL graphics driver
about:
The returned driver can be used with #SetGraphicsDriver
End Rem
Function OSMesaGLGraphicsDriver:TOSMesaGLGraphicsDriver()
	Global _driver:TOSMesaGLGraphicsDriver=New TOSMesaGLGraphicsDriver
	Return _driver
End Function

Rem
bbdoc: Create OpenGL graphics
returns: An OpenGL graphics object
about:
This is a convenience function that allows you to easily create an OpenGL graphics context.
End Rem
Function OsMesaGLGraphics:TGraphics( width,height,depth=0,hertz=60,flags=GRAPHICS_BACKBUFFER|GRAPHICS_DEPTHBUFFER )
	SetGraphicsDriver OSMesaGLGraphicsDriver()
	Return Graphics( width,height,depth,hertz,flags )
End Function
	
SetGraphicsDriver OSMesaGLGraphicsDriver()

?




