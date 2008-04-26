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
	Function bmx_b2world_getgroundbody:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2world_setwarmstarting(handle:Byte Ptr, flag:Int)
	Function bmx_b2world_setpositioncorrection(handle:Byte Ptr, flag:Int)
	Function bmx_b2world_setcontinuousphysics(handle:Byte Ptr, flag:Int)
	Function bmx_b2world_validate(handle:Byte Ptr)
	Function bmx_b2world_setdebugDraw(handle:Byte Ptr, debugDraw:Byte Ptr)
	Function bmx_b2world_createjoint:Byte Ptr(handle:Byte Ptr, def:Byte Ptr)
	Function bmx_b2world_destroyjoint(handle:Byte Ptr, joint:Byte Ptr)
	Function bmx_b2world_getbodylist:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2world_getjointlist:Byte Ptr(handle:Byte Ptr)

	Function bmx_b2bodydef_create:Byte Ptr()
	Function bmx_b2bodydef_delete(handle:Byte Ptr)
	'Function bmx_b2bodydef_settype(handle:Byte Ptr, bodyType:Int)
	Function bmx_b2bodydef_setposition(handle:Byte Ptr, position:Byte Ptr)
	Function bmx_b2bodydef_setangle(handle:Byte Ptr, angle:Float)
	Function bmx_b2bodydef_setmassdata(handle:Byte Ptr, data:Byte Ptr)
	Function bmx_b2bodydef_issleeping:Int(handle:Byte Ptr)
	Function bmx_b2bodydef_setissleeping(handle:Byte Ptr, sleeping:Int)
	Function bmx_b2bodydef_setfixedrotation(handle:Byte Ptr, fixed:Int)
	Function bmx_b2bodydef_getfixedrotation:Int(handle:Byte Ptr)
	Function bmx_b2bodydef_setisbullet(handle:Byte Ptr, bullet:Int)
	Function bmx_b2bodydef_setlineardamping(handle:Byte Ptr, damping:Float)
	Function bmx_b2bodydef_getlineardamping:Float(handle:Byte Ptr)
	Function bmx_b2bodydef_setangulardamping(handle:Byte Ptr, damping:Float)
	Function bmx_b2bodydef_getangulardamping:Float(handle:Byte Ptr)
	Function bmx_b2bodydef_setallowsleep(handle:Byte Ptr, allow:Int)
	Function bmx_b2bodydef_getallowsleep:Int(handle:Byte Ptr)

	Function bmx_b2world_create:Byte Ptr(worldAABB:Byte Ptr, gravity:Byte Ptr, doSleep:Int)
	Function bmx_b2world_dostep(handle:Byte Ptr, timeStep:Float, iterations:Int)

	Function bmx_b2shapedef_setfriction(handle:Byte Ptr, friction:Float)
	Function bmx_b2shapedef_setrestitution(handle:Byte Ptr, restitution:Float)
	Function bmx_b2shapedef_setdensity(handle:Byte Ptr, density:Float)

	Function bmx_b2polygondef_create:Byte Ptr()
	Function bmx_b2polygondef_setasbox(handle:Byte Ptr, hx:Float, hy:Float)
	Function bmx_b2polygondef_delete(handle:Byte Ptr)

	Function bmx_b2body_createshape:Byte Ptr(handle:Byte Ptr, def:Byte Ptr, shape:Object)
	Function bmx_b2body_destroyshape(handle:Byte Ptr, shape:Byte Ptr)
	Function bmx_b2body_setmassfromshapes(handle:Byte Ptr)
	Function bmx_b2body_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_getangle:Float(handle:Byte Ptr)
	Function bmx_b2body_getmaxbody:Object(handle:Byte Ptr)
	Function bmx_b2body_getnext:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_getshapelist:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_isstatic:Int(handle:Byte Ptr)
	Function bmx_b2body_isdynamic:Int(handle:Byte Ptr)
	Function bmx_b2body_isfrozen:Int(handle:Byte Ptr)
	Function bmx_b2body_issleeping:Int(handle:Byte Ptr)
	Function bmx_b2body_allowsleeping(handle:Byte Ptr, flag:Int)
	Function bmx_b2body_wakeup(handle:Byte Ptr)
	Function bmx_b2body_puttosleep(handle:Byte Ptr)
	Function bmx_b2body_isbullet:Int(handle:Byte Ptr)
	Function bmx_b2body_setbullet(handle:Byte Ptr, flag:Int)
	Function bmx_b2body_getworldcenter:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_getlocalcenter:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_setlinearvelocity(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_b2body_getlinearvelocity:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2body_setangularvelocity(handle:Byte Ptr, omega:Float)
	Function bmx_b2body_getangularvelocity:Float(handle:Byte Ptr)
	Function bmx_b2body_applyforce(handle:Byte Ptr, force:Byte Ptr, point:Byte Ptr)
	Function bmx_b2body_applytorque(handle:Byte Ptr, torque:Float)
	Function bmx_b2body_applyimpulse(handle:Byte Ptr, impulse:Byte Ptr, point:Byte Ptr)
	Function bmx_b2body_getmass:Float(handle:Byte Ptr)
	Function bmx_b2body_getinertia:Float(handle:Byte Ptr)
	Function bmx_b2body_getworldpoint:Byte Ptr(handle:Byte Ptr, localPoint:Byte Ptr)
	Function bmx_b2body_getworldvector:Byte Ptr(handle:Byte Ptr, localVector:Byte Ptr)
	Function bmx_b2body_getlocalpoint:Byte Ptr(handle:Byte Ptr, worldPoint:Byte Ptr)
	Function bmx_b2body_getlocalvector:Byte Ptr(handle:Byte Ptr, worldVector:Byte Ptr)
	Function bmx_b2body_getjointlist:Byte Ptr(handle:Byte Ptr)

	Function bmx_b2debugdraw_create:Byte Ptr(handle:Object)
	Function bmx_b2debugdraw_setflags(handle:Byte Ptr, flags:Int)
	Function bmx_b2debugdraw_getflags:Int(handle:Byte Ptr)
	Function bmx_b2debugdraw_appendflags(handle:Byte Ptr, flags:Int)
	Function bmx_b2debugdraw_clearflags(handle:Byte Ptr, flags:Int)

	Function bmx_b2circledef_create:Byte Ptr()
	Function bmx_b2circledef_setradius(handle:Byte Ptr, radius:Float)
	Function bmx_b2circledef_setlocalposition(handle:Byte Ptr, pos:Byte Ptr)
	Function bmx_b2circledef_delete(handle:Byte Ptr)

	Function bmx_b2shape_issensor:Int(handle:Byte Ptr)
	Function bmx_b2shape_getbody:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2shape_getmaxshape:Object(handle:Byte Ptr)

	Function bmx_b2revolutejointdef_create:Byte Ptr()
	Function bmx_b2revolutejointdef_initialize(handle:Byte Ptr, body1:Byte Ptr, body2:Byte Ptr, anchor:Byte Ptr)

	Function bmx_b2joint_getmaxjoint:Object(handle:Byte Ptr)

	Function bmx_b2massdata_new:Byte Ptr()
	Function bmx_b2massdata_delete(handle:Byte Ptr)
	Function bmx_b2massdata_setmass(handle:Byte Ptr, mass:Float)
	Function bmx_b2massdata_setcenter(handle:Byte Ptr, center:Byte Ptr)
	Function bmx_b2massdata_seti(handle:Byte Ptr, i:Float)

	Function bmx_b2jointedge_getother:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2jointedge_getjoint:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2jointedge_getprev:Byte Ptr(handle:Byte Ptr)
	Function bmx_b2jointedge_getnext:Byte Ptr(handle:Byte Ptr)

End Extern

Const e_unknownJoint:Int = 0
Const e_revoluteJoint:Int = 1
Const e_prismaticJoint:Int = 2
Const e_distanceJoint:Int = 3
Const e_pulleyJoint:Int = 4
Const e_mouseJoint:Int = 5
Const e_gearJoint:Int = 6

