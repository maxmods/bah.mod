' Copyright (c) 2007 Bruce A Henderson
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

Import BRL.Blitz

Import "src/*.h"
Import "src/chipmunk.c"
Import "src/cpArray.c"
Import "src/cpBody.c"
Import "src/cpHashSet.c"
Import "src/cpPolyShape.c"
Import "src/cpSpace.c"
Import "src/cpVect.c"
Import "src/cpArbiter.c"
Import "src/cpBB.c"
Import "src/cpCollision.c"
Import "src/cpJoint.c"
Import "src/cpShape.c"
Import "src/cpSpaceHash.c"


Import "glue.cpp"

Extern

	Function cpfind:Object(obj:Byte Ptr)
	Function cpbind(p:Byte Ptr, obj:Object)

	Function cpInitChipmunk()
	Function cpResetShapeIdCounter()
	Function cpSpaceResizeStaticHash(space:Byte Ptr, dim:Float, count:Int)
	Function cpSpaceResizeActiveHash(space:Byte Ptr, dim:Float, count:Int)
	Function cpSpaceRehashStatic(space:Byte Ptr)
	Function cpSpaceStep(space:Byte Ptr, dt:Float)
	Function cpSpaceFree(space:Byte Ptr)
	
	Function bmx_cpbody_create:Byte Ptr(handle:Object, mass:Float, inertia:Float)
	Function bmx_cpbody_getmass:Float(handle:Byte Ptr)
	Function bmx_cpbody_getinertia:Float(handle:Byte Ptr)
	Function bmx_cpbody_getangle:Float(handle:Byte Ptr)
	Function bmx_cpbody_getangularvelocity:Float(handle:Byte Ptr)
	Function bmx_cpbody_gettorque:Float(handle:Byte Ptr)
	Function bmx_cpbody_setposition(handle:Byte Ptr, pos:Byte Ptr)
	Function bmx_cpbody_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpbody_getrot:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpbody_setvelocity(handle:Byte Ptr, velocity:Byte Ptr)
	Function cpBodyUpdatePosition(handle:Byte Ptr, dt:Float)
	Function bmx_cpbody_setangularvelocity(handle:Byte Ptr, av:Float)
	Function bmx_cpbody_setangle(handle:Byte Ptr, angle:Float)
	Function bmx_cpbody_updatevelocity(handle:Byte Ptr, gravity:Byte Ptr, damping:Float, dt:Float)
	Function cpBodyResetForces(handle:Byte Ptr)
	Function bmx_cpbody_applyforce(handle:Byte Ptr, force:Byte Ptr, offset:Byte Ptr)
	Function bmx_cpbody_local2world:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpbody_world2local:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpbody_getvelocity:Byte Ptr(cpObjectPtr:Byte Ptr)
	Function cpBodySetMass(handle:Byte Ptr, mass:Float)
	Function cpBodySetMoment(handle:Byte Ptr, moment:Float)
	Function bmx_body_applyimpulse(handle:Byte Ptr, impulse:Byte Ptr, offset:Byte Ptr)
	Function bmx_body_slew(handle:Byte Ptr, pos:Byte Ptr, dt:Float)
	Function cpBodyFree(handle:Byte Ptr)
	Function bmx_cpbody_settorque(handle:Byte Ptr, torque:Float)

	Function bmx_cpspace_create:Byte Ptr(handle:Object)
	Function bmx_cpspace_setgravity(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpspace_addstaticshape(handle:Byte Ptr, shape:Byte Ptr)
	Function bmx_cpspace_addbody(handle:Byte Ptr, body:Byte Ptr)
	Function bmx_cpspace_addshape(handle:Byte Ptr, shape:Byte Ptr)
	Function bmx_cpspace_getactiveshapes:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpspace_getstaticshapes:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpspace_setiterations(handle:Byte Ptr, num:Int)
	Function bmx_cpspace_addjoint(handle:Byte Ptr, joint:Byte Ptr)
	Function bmx_cpspace_addcollisionpairfunc(handle:Byte Ptr, collTypeA:Int, collTypeB:Int, ..
		func:Int(shapeA:Byte Ptr, shapeB:Byte Ptr, contacts:Byte Ptr, numContacts:Int, normalCoeficient:Float, data:Object), data:Object)
	Function bmx_cpspace_removecollisionpairfunc(handle:Byte Ptr, collTypeA:Int, collTypeB:Int)
	Function bmx_cpspace_setdefaultcollisionpairfunc(handle:Byte Ptr, ..
		func:Int(shapeA:Byte Ptr, shapeB:Byte Ptr, contacts:Byte Ptr, numContacts:Int, normalCoeficient:Float, data:Object), data:Object)
	Function bmx_cpspace_setdamping(handle:Byte Ptr, damping:Float)
	
	Function cpSpaceRemoveBody(handle:Byte Ptr, body:Byte Ptr)
	Function cpSpaceRemoveShape(handle:Byte Ptr, shape:Byte Ptr)
	Function cpSpaceRemoveStaticShape(handle:Byte Ptr, shape:Byte Ptr)
	Function cpSpaceRemoveJoint(handle:Byte Ptr, body:Byte Ptr)

	Function bmx_cpvect_create:Byte Ptr(x:Float, y:Float)
	Function bmx_cpvect_delete(handle:Byte Ptr)
	Function bmx_cpvect_getx:Float(handle:Byte Ptr)
	Function bmx_cpvect_gety:Float(handle:Byte Ptr)
	Function bmx_cpvect_add:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_sub:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_rotate:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_getxy(handle:Byte Ptr, x:Float Ptr, y:Float Ptr)
	Function bmx_cpvect_negate:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpvect_mult:Byte Ptr(handle:Byte Ptr, scalar:Float)
	Function bmx_cpvect_dot:Float(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_cross:Float(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_perp:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpvect_project:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_unrotate:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_length:Float(handle:Byte Ptr)
	Function bmx_cpvect_lengthsq:Float(handle:Byte Ptr)
	Function bmx_cpvect_normalize:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpvect_toangle:Float(handle:Byte Ptr)
	Function bmx_cpvect_cpvzero:Byte Ptr()

	Function bmx_cpsegmentshape_create:Byte Ptr(handle:Object, body:Byte Ptr, a:Byte Ptr, b:Byte Ptr, radius:Float)
	Function bmx_cpsegmentshape_getendpointa:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpsegmentshape_getendpointb:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpsegmentshape_getnormal:Byte Ptr(handle:Byte Ptr)

	Function bmx_cppolyshape_getvertsascoords:Float[](handle:Byte Ptr)
	Function bmx_cppolyshape_numverts:Int(handle:Byte Ptr)

	Function bmx_cpshape_setelasticity(handle:Byte Ptr, e:Float)
	Function bmx_cpshape_setfriction(handle:Byte Ptr, u:Float)
	Function bmx_cpshape_setcollisiontype(handle:Byte Ptr, kind:Int)
	Function bmx_cpshape_getbody:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpshape_setgroup(handle:Byte Ptr, group:Int)
	Function bmx_cpshape_setlayers(handle:Byte Ptr, layers:Int)
	Function bmx_cpshape_setsurfacevelocity(handle:Byte Ptr, velocity:Byte Ptr)
	Function bmx_cpshape_getsurfacevelocity:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpshape_getelasticity:Float(handle:Byte Ptr)
	Function bmx_cpshape_getfriction:Float(handle:Byte Ptr)
	Function cpShapeFree(handle:Byte Ptr)
	Function bmx_shape_cachebb:Byte Ptr(handle:Byte Ptr)

	Function cpMomentForCircle:Float(m:Float, r1:Float, r2:Float, offset:Byte Ptr)
	Function bmx_cpdampedspring(a:Byte Ptr, b:Byte Ptr, anchor1:Byte Ptr, anchor2:Byte Ptr, rlen:Float, k:Float, dmp:Float, dt:Float)

	Function bmx_cpcircleshape_create:Byte Ptr(handle:Object, body:Byte Ptr, radius:Float, offset:Byte Ptr)
	Function bmx_cpcircleshape_getradius:Float(handle:Byte Ptr)
	Function bmx_cpcircleshape_getcenter:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpcircleshape_gettransformedcenter:Byte Ptr(handle:Byte Ptr)
	
	Function cpSpaceHashEach(handle:Byte Ptr, callback(obj:Byte Ptr, callbackData:Object), data:Object)
	Function cpSpaceEachBody(handle:Byte Ptr, callback(obj:Byte Ptr, callbackData:Object), data:Object)

	Function bmx_cppinjoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, anchor1:Byte Ptr, anchor2:Byte Ptr)
	Function bmx_cpslidejoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, anchor1:Byte Ptr, anchor2:Byte Ptr, minDist:Float, maxDist:Float)
	Function bmx_cppivotjoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, pivot:Byte Ptr)
	Function bmx_cpgroovejoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, grooveA:Byte Ptr, grooveB:Byte Ptr, anchor:Byte Ptr)
	Function cpJointFree(handle:Byte Ptr)

	Function bmx_cppinjoint_getanchor1:Byte Ptr(handle:Byte Ptr)
	Function bmx_cppinjoint_getanchor2:Byte Ptr(handle:Byte Ptr)

	Function bmx_cpslidejoint_getanchor1:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpslidejoint_getanchor2:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpslidejoint_getmindist:Float(handle:Byte Ptr)
	Function bmx_cpslidejoint_getmaxdist:Float(handle:Byte Ptr)

	Function bmx_cppivotjoint_getanchor1:Byte Ptr(handle:Byte Ptr)
	Function bmx_cppivotjoint_getanchor2:Byte Ptr(handle:Byte Ptr)

	Function bmx_cpgroovejoint_getgroovea:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpgroovejoint_getgrooveb:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpgroovejoint_getanchor:Byte Ptr(handle:Byte Ptr)

	Function bmx_cpjoint_getbodya:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpjoint_getbodyb:Byte Ptr(handle:Byte Ptr)

	Function bmx_CP_HASH_PAIR:Int(collTypeA:Int, collTypeB:Int)
	
	Function bmx_cpcontact_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpcontact_getnormal:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpcontact_getdistance:Float(handle:Byte Ptr)
	Function bmx_cpcontact_getjnacc:Float(handle:Byte Ptr)
	Function bmx_cpcontact_getjtacc:Float(handle:Byte Ptr)

	Function bmx_cpbb_create:Byte Ptr(l:Float, b:Float, r:Float, t:Float)
	Function bmx_cpbb_intersects:Int(handle:Byte Ptr, other:Byte Ptr)
	Function bmx_cpbb_containsbb:Int(handle:Byte Ptr, other:Byte Ptr)
	Function bmx_cpbb_containsvect:Int(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_cpbb_clampvect:Byte Ptr(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_cpbb_wrapvect:Byte Ptr(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_cpbb_delete(handle:Byte Ptr)

End Extern

Const INFINITY:Float = 1e1000

