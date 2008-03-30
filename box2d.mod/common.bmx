' Copyright (c) 2008 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import "source.bmx"

Extern

	Function bmx_b2aabb_create:Byte Ptr(lowerBound:Byte Ptr, upperBound:Byte Ptr)
	Function bmx_b2aabb_delete(handle:Byte Ptr)
	Function bmx_b2abb_setlowerbound(handle:Byte Ptr, lowerBound:Byte Ptr)
	Function bmx_b2abb_setupperbound(handle:Byte Ptr, upperBound:Byte Ptr)

	Function bmx_b2vec2_create:Byte Ptr(x:Float, y:Float)
	Function bmx_b2vec2_delete(handle:Byte Ptr)
	Function bmx_b2vec2_getx:Float(handle:Byte Ptr)
	Function bmx_b2vec2_gety:Float(handle:Byte Ptr)

	Function bmx_b2world_createstaticbody:Byte Ptr(handle:Byte Ptr, def:Byte Ptr, body:Object)
	Function bmx_b2world_createdynamicbody:Byte Ptr(handle:Byte Ptr, def:Byte Ptr, body:Object)
	Function bmx_b2world_destroybody(handle:Byte Ptr, body:Byte Ptr)

	Function bmx_b2bodydef_create:Byte Ptr()
	Function bmx_b2bodydef_delete(handle:Byte Ptr)
	'Function bmx_b2bodydef_settype(handle:Byte Ptr, bodyType:Int)
	Function bmx_b2bodydef_setposition(handle:Byte Ptr, position:Byte Ptr)

	Function bmx_b2world_create:Byte Ptr(worldAABB:Byte Ptr, gravity:Byte Ptr, doSleep:Int)
	Function bmx_b2world_dostep(handle:Byte Ptr, timeStep:Float, iterations:Int)

	Function bmx_b2shapedef_setfriction(handle:Byte Ptr, friction:Float)
	Function bmx_b2shapedef_setrestitution(handle:Byte Ptr, restitution:Float)
	Function bmx_b2shapedef_setdensity(handle:Byte Ptr, density:Float)

	Function bmx_b2polygondef_create:Byte Ptr()
	Function bmx_b2polygondef_setasbox(handle:Byte Ptr, hx:Float, hy:Float)

	Function bmx_b2body_createshape:Byte Ptr(handle:Byte Ptr, def:Byte Ptr, shape:Object)
	Function bmx_b2body_destroyshape(handle:Byte Ptr, shape:Byte Ptr)
	Function bmx_b2body_setmassfromshapes(handle:Byte Ptr)
	Function bmx_b2body_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_getangle:Float(handle:Byte Ptr)

	Function bmx_b2debugdraw_create:Byte Ptr(handle:Object)

End Extern
