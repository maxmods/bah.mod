' Copyright (c) 2007-2010 Bruce A Henderson
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
'Import "src/cpConstaint.c"
Import "src/cpShape.c"
Import "src/cpSpaceHash.c"
Import "src/constraints/cpConstraint.c"
Import "src/constraints/cpDampedRotarySpring.c"
Import "src/constraints/cpDampedSpring.c"
Import "src/constraints/cpGearJoint.c"
Import "src/constraints/cpGrooveJoint.c"
Import "src/constraints/cpPinJoint.c"
Import "src/constraints/cpPivotJoint.c"
Import "src/constraints/cpRatchetJoint.c"
Import "src/constraints/cpRotaryLimitJoint.c"
Import "src/constraints/cpSimpleMotor.c"
Import "src/constraints/cpSlideJoint.c"

Import "glue.cpp"

Extern

	Function cpfind:Object(obj:Byte Ptr)
	Function cpbind(p:Byte Ptr, obj:Object)

	Function cpInitChipmunk()
	Function cpResetShapeIdCounter()
	Function cpSpaceResizeStaticHash(space:Byte Ptr, dim:Double, count:Int)
	Function cpSpaceResizeActiveHash(space:Byte Ptr, dim:Double, count:Int)
	Function cpSpaceRehashStatic(space:Byte Ptr)
	Function cpSpaceStep(space:Byte Ptr, dt:Double)
	Function cpSpaceFree(space:Byte Ptr)
	
	Function bmx_cpbody_create:Byte Ptr(handle:Object, mass:Double, inertia:Double)
	Function bmx_cpbody_getmass:Double(handle:Byte Ptr)
	Function bmx_cpbody_getinertia:Double(handle:Byte Ptr)
	Function bmx_cpbody_getangle:Double(handle:Byte Ptr)
	Function bmx_cpbody_getangularvelocity:Double(handle:Byte Ptr)
	Function bmx_cpbody_gettorque:Double(handle:Byte Ptr)
	Function bmx_cpbody_setposition(handle:Byte Ptr, pos:Byte Ptr)
	Function bmx_cpbody_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpbody_getrot:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpbody_setvelocity(handle:Byte Ptr, velocity:Byte Ptr)
	Function cpBodyUpdatePosition(handle:Byte Ptr, dt:Double)
	Function bmx_cpbody_setangularvelocity(handle:Byte Ptr, av:Double)
	Function bmx_cpbody_setangle(handle:Byte Ptr, angle:Double)
	Function bmx_cpbody_updatevelocity(handle:Byte Ptr, gravity:Byte Ptr, damping:Double, dt:Double)
	Function cpBodyResetForces(handle:Byte Ptr)
	Function bmx_cpbody_applyforce(handle:Byte Ptr, force:Byte Ptr, offset:Byte Ptr)
	Function bmx_cpbody_local2world:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpbody_world2local:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpbody_getvelocity:Byte Ptr(cpObjectPtr:Byte Ptr)
	Function cpBodySetMass(handle:Byte Ptr, mass:Double)
	Function cpBodySetMoment(handle:Byte Ptr, moment:Double)
	Function bmx_body_applyimpulse(handle:Byte Ptr, impulse:Byte Ptr, offset:Byte Ptr)
	Function bmx_body_slew(handle:Byte Ptr, pos:Byte Ptr, dt:Double)
	Function cpBodyFree(handle:Byte Ptr)
	Function bmx_cpbody_settorque(handle:Byte Ptr, torque:Double)
	Function bmx_cpbody_posfunc(handle:Byte Ptr, func(body:Byte Ptr, dt:Double))
	Function bmx_cpbody_velfunc(handle:Byte Ptr, func(body:Byte Ptr, gravity:Byte Ptr, damping:Double, dt:Double))
	Function bmx_velocity_function(body:Byte Ptr, gravity:Byte Ptr, damping:Double, dt:Double)
	Function bmx_position_function(body:Byte Ptr, dt:Double)
	Function bmx_cpbody_setdata(body:Byte Ptr, data:Object)
	Function bmx_cpbody_getdata:Object(body:Byte Ptr)
	
	Function bmx_cpspace_create:Byte Ptr(handle:Object)
	Function bmx_cpspace_setgravity(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpspace_addstaticshape(handle:Byte Ptr, shape:Byte Ptr)
	Function bmx_cpspace_addbody(handle:Byte Ptr, body:Byte Ptr)
	Function bmx_cpspace_addshape(handle:Byte Ptr, shape:Byte Ptr)
	Function bmx_cpspace_getactiveshapes:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpspace_getstaticshapes:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpspace_setiterations(handle:Byte Ptr, num:Int)
	Function bmx_cpspace_addconstraint(handle:Byte Ptr, constraint:Byte Ptr)
	Function bmx_cpspace_addcollisionpairfunc(handle:Byte Ptr, collTypeA:Int, collTypeB:Int, ..
		func:Int(shapeA:Byte Ptr, shapeB:Byte Ptr, contacts:Byte Ptr, numContacts:Int, normalCoeficient:Double, data:Object), data:Object)
	Function bmx_cpspace_addcollisionpairnullfunc(handle:Byte Ptr, collTypeA:Int, collTypeB:Int)
	Function bmx_cpspace_removecollisionpairfunc(handle:Byte Ptr, collTypeA:Int, collTypeB:Int)
	Function bmx_cpspace_setdefaultcollisionpairfunc(handle:Byte Ptr, ..
		func:Int(shapeA:Byte Ptr, shapeB:Byte Ptr, contacts:Byte Ptr, numContacts:Int, normalCoeficient:Double, data:Object), data:Object)
	'Function bmx_cpspace_setdamping(handle:Byte Ptr, damping:Double)
	
	Function cpSpaceRemoveBody(handle:Byte Ptr, body:Byte Ptr)
	Function cpSpaceRemoveShape(handle:Byte Ptr, shape:Byte Ptr)
	Function cpSpaceRemoveStaticShape(handle:Byte Ptr, shape:Byte Ptr)
	Function cpSpaceRemoveConstraint(handle:Byte Ptr, body:Byte Ptr)

	Function bmx_cpvect_create:Byte Ptr(x:Double, y:Double)
	Function bmx_cpvect_delete(handle:Byte Ptr)
	Function bmx_cpvect_getx:Double(handle:Byte Ptr)
	Function bmx_cpvect_gety:Double(handle:Byte Ptr)
	Function bmx_cpvect_add:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_sub:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_rotate:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_getxy(handle:Byte Ptr, x:Double Ptr, y:Double Ptr)
	Function bmx_cpvect_negate:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpvect_mult:Byte Ptr(handle:Byte Ptr, scalar:Double)
	Function bmx_cpvect_dot:Double(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_cross:Double(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_perp:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpvect_project:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_unrotate:Byte Ptr(handle:Byte Ptr, vec:Byte Ptr)
	Function bmx_cpvect_length:Double(handle:Byte Ptr)
	Function bmx_cpvect_lengthsq:Double(handle:Byte Ptr)
	Function bmx_cpvect_normalize:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpvect_toangle:Double(handle:Byte Ptr)
	Function bmx_cpvect_cpvzero:Byte Ptr()
	Function bmx_cpvect_fromvect:Byte Ptr(vect:Byte Ptr)

	Function bmx_cpsegmentshape_create:Byte Ptr(handle:Object, body:Byte Ptr, a:Byte Ptr, b:Byte Ptr, radius:Double)
	Function bmx_cpsegmentshape_getendpointa:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpsegmentshape_getendpointb:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpsegmentshape_getnormal:Byte Ptr(handle:Byte Ptr)

	Function bmx_cppolyshape_getvertsascoords:Double[](handle:Byte Ptr)
	Function bmx_cppolyshape_numverts:Int(handle:Byte Ptr)

	Function bmx_cpshape_setelasticity(handle:Byte Ptr, e:Double)
	Function bmx_cpshape_setfriction(handle:Byte Ptr, u:Double)
	Function bmx_cpshape_setcollisiontype(handle:Byte Ptr, kind:Int)
	Function bmx_cpshape_getbody:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpshape_setgroup(handle:Byte Ptr, group:Int)
	Function bmx_cpshape_setlayers(handle:Byte Ptr, layers:Int)
	Function bmx_cpshape_setsurfacevelocity(handle:Byte Ptr, velocity:Byte Ptr)
	Function bmx_cpshape_getsurfacevelocity:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpshape_getelasticity:Double(handle:Byte Ptr)
	Function bmx_cpshape_getfriction:Double(handle:Byte Ptr)
	Function cpShapeFree(handle:Byte Ptr)
	Function bmx_cpshape_cachebb:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpshape_setdata(handle:Byte Ptr, data:Object)
	Function bmx_cpshape_getdata:Object(handle:Byte Ptr)

	Function cpMomentForCircle:Double(m:Double, r1:Double, r2:Double, offset:Byte Ptr)
'	Function bmx_cpdampedspring(a:Byte Ptr, b:Byte Ptr, anchor1:Byte Ptr, anchor2:Byte Ptr, rlen:Double, k:Double, dmp:Double, dt:Double)

	Function bmx_cpcircleshape_create:Byte Ptr(handle:Object, body:Byte Ptr, radius:Double, offset:Byte Ptr)
	Function bmx_cpcircleshape_getradius:Double(handle:Byte Ptr)
	Function bmx_cpcircleshape_getcenter:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpcircleshape_gettransformedcenter:Byte Ptr(handle:Byte Ptr)
	
	Function cpSpaceHashEach(handle:Byte Ptr, callback(obj:Byte Ptr, callbackData:Object), data:Object)
	Function cpSpaceEachBody(handle:Byte Ptr, callback(obj:Byte Ptr, callbackData:Object), data:Object)

	Function bmx_cppinjoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, anchor1:Byte Ptr, anchor2:Byte Ptr)
	Function bmx_cpslidejoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, anchor1:Byte Ptr, anchor2:Byte Ptr, minDist:Double, maxDist:Double)
	Function bmx_cppivotjoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, pivot:Byte Ptr)
	Function bmx_cpgroovejoint_create:Byte Ptr(handle:Object, bodyA:Byte Ptr, bodyB:Byte Ptr, grooveA:Byte Ptr, grooveB:Byte Ptr, anchor:Byte Ptr)
	Function cpConstraintFree(handle:Byte Ptr)

	Function bmx_cppinjoint_getanchor1:Byte Ptr(handle:Byte Ptr)
	Function bmx_cppinjoint_getanchor2:Byte Ptr(handle:Byte Ptr)

	Function bmx_cpslidejoint_getanchor1:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpslidejoint_getanchor2:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpslidejoint_getmindist:Double(handle:Byte Ptr)
	Function bmx_cpslidejoint_getmaxdist:Double(handle:Byte Ptr)

	Function bmx_cppivotjoint_getanchor1:Byte Ptr(handle:Byte Ptr)
	Function bmx_cppivotjoint_getanchor2:Byte Ptr(handle:Byte Ptr)

	Function bmx_cpgroovejoint_getgroovea:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpgroovejoint_getgrooveb:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpgroovejoint_getanchor:Byte Ptr(handle:Byte Ptr)

	Function bmx_cpconstraint_getbodya:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpconstraint_getbodyb:Byte Ptr(handle:Byte Ptr)

	Function bmx_CP_HASH_PAIR:Int(collTypeA:Int, collTypeB:Int)
	
	Function bmx_cpcontact_getposition:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpcontact_getnormal:Byte Ptr(handle:Byte Ptr)
	Function bmx_cpcontact_getdistance:Double(handle:Byte Ptr)
	Function bmx_cpcontact_getjnacc:Double(handle:Byte Ptr)
	Function bmx_cpcontact_getjtacc:Double(handle:Byte Ptr)

	Function bmx_cpbb_create:Byte Ptr(l:Double, b:Double, r:Double, t:Double)
	Function bmx_cpbb_intersects:Int(handle:Byte Ptr, other:Byte Ptr)
	Function bmx_cpbb_containsbb:Int(handle:Byte Ptr, other:Byte Ptr)
	Function bmx_cpbb_containsvect:Int(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_cpbb_clampvect:Byte Ptr(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_cpbb_wrapvect:Byte Ptr(handle:Byte Ptr, v:Byte Ptr)
	Function bmx_cpbb_delete(handle:Byte Ptr)

End Extern

Const INFINITY:Double = 1e1000

