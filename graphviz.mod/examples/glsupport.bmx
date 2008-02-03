SuperStrict

Import BaH.Graphviz
Import Pub.OpenGL

Type TOpenGLRenderSupport Extends TGVRenderSupport

	Method Init()
		glEnable GL_LINE_SMOOTH
		glEnable GL_LINE_STIPPLE
	End Method

	Method setDotted()
		glPushAttrib GL_LINE_BIT
  		glLineStipple 1, $1
	End Method
	
	Method setDashed()
		glPushAttrib GL_LINE_BIT
  		glLineStipple 4, $AAAA
	End Method
	
	Method setSolid()
		glPopAttrib
	End Method
End Type
