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

Rem
bbdoc: Box2D
End Rem
Module BaH.Box2D

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Box2D (c) 2006-2008 Erin Catto http://www.gphysics.com"
ModuleInfo "Copyright: BlitzMax port - 2008 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"


Import "common.bmx"

Rem
bbdoc: 
End Rem
Type b2World

	Field b2ObjectPtr:Byte Ptr
	
	Field filter:b2ContactFilter
	Field contactListener:b2ContactListener
	Field boundaryListener:b2BoundaryListener

	Rem
	bbdoc: 
	End Rem
	Function CreateWorld:b2World(worldAABB:b2AABB, gravity:b2Vec2, doSleep:Int)
		Return New b2World.Create(worldAABB, gravity, doSleep)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:b2World(worldAABB:b2AABB, gravity:b2Vec2, doSleep:Int)
		b2ObjectPtr = bmx_b2world_create(worldAABB.b2ObjectPtr, gravity.b2ObjectPtr, doSleep)
		Return Self
	End Method

	Method Free()
	End Method
	
	Method Delete()
		Free()
	End Method
	
	Rem
	bbdoc: Register a destruction listener.
	End Rem
	Method SetDestructListener(listener:b2DestructionListener)
	End Method

	Rem
	bbdoc: Register a broad-phase boundary listener.
	End Rem
	Method SetBoundaryListener(listener:b2BoundaryListener)
		boundaryListener = listener
		bmx_b2world_setboundarylistener(b2ObjectPtr, listener.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Register a contact filter to provide specific control over collision.
	about: Otherwise the default filter is used.
	End Rem
	Method SetFilter(_filter:b2ContactFilter)
		filter = _filter
		bmx_b2world_setfilter(b2ObjectPtr, filter.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Register a contact event listener
	End Rem
	Method SetContactListener(listener:b2ContactListener)
		contactListener = listener
		bmx_b2world_setcontactlistener(b2ObjectPtr, listener.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Register a routine for debug drawing.
	about: The debug draw functions are called inside the b2World::DoStep method, so make sure your renderer is ready to
	consume draw commands when you call DoStep().
	End Rem
	Method SetDebugDraw(debugDraw:b2DebugDraw)
		bmx_b2world_setdebugDraw(b2ObjectPtr, debugDraw.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Create a rigid body given a definition
	about: No reference to the definition is retained.
	<p>
	Warning: This method is locked during callbacks.
	</p>
	End Rem
	Method CreateBody:b2Body(def:b2BodyDef)
		Local body:b2Body = New b2Body
		body.userData = def.userData ' copy the userData
		body.b2ObjectPtr = bmx_b2world_createbody(b2ObjectPtr, def.b2ObjectPtr, body)
		Return body
	End Method

'	Rem
'	bbdoc: Create a dynamic rigid body given a definition.
'	about: No reference To the definition is retained.
'	<p>
'	Warning: This Method is locked during callbacks.
'	</p>
'	End Rem
'	Method CreateDynamicBody:b2Body(def:b2BodyDef)
'		Local body:b2Body = New b2Body
'		body.userData = def.userData ' copy the userData
'		body.b2ObjectPtr = bmx_b2world_createdynamicbody(b2ObjectPtr, def.b2ObjectPtr, body)
'		Return body
'	End Method

	Rem
	bbdoc: Destroy a rigid body given a definition.
	about: No reference to the definition is retained.
	<p>
	Warning: This automatically deletes all associated shapes and joints.
	</p>
	<p>
	Warning: This method is locked during callbacks.
	</p>
	End Rem
	Method DestroyBody(body:b2Body)
		bmx_b2world_destroybody(b2ObjectPtr, body.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Create a joint to constrain bodies together.
	about: No reference to the definition is retained. This may cause the connected bodies to cease
	colliding.
	<p>
	Warning: This method is locked during callbacks.
	</p>
	End Rem
	Method CreateJoint:b2Joint(def:b2JointDef)
		Local jointType:Int
		Local joint:b2Joint = b2Joint._create(bmx_b2world_createjoint(b2ObjectPtr, def.b2ObjectPtr))
		joint.userData = def.userData ' copy the userData
		Return joint
	End Method
	
	' 
	Function _createJoint:b2Joint(jointType:Int)
		Local joint:b2Joint
		Select jointType
			Case e_unknownJoint
				joint = New b2Joint
			Case e_revoluteJoint
				joint = New b2RevoluteJoint
			Case e_prismaticJoint
				joint = New b2PrismaticJoint
			Case e_distanceJoint
				joint = New b2DistanceJoint
			Case e_pulleyJoint
				joint = New b2PulleyJoint
			Case e_mouseJoint
				joint = New b2MouseJoint
			Case e_gearJoint
				joint = New b2GearJoint
			Default
				DebugLog "Warning, joint type '" + jointType + "' is not defined in module."
				joint = New b2Joint
		End Select
		Return joint
	End Function

	Rem
	bbdoc: Destroy a joint.
	about: This may cause the connected bodies to begin colliding.
	<p>
	Warning: This method is locked during callbacks.
	</p>
	End Rem
	Method DestroyJoint(joint:b2Joint)
		bmx_b2world_destroyjoint(b2ObjectPtr, joint.b2ObjectPtr)
	End Method

	Rem
	bbdoc: The world provides a single static ground body with no collision shapes.
	about: You can use this to simplify the creation of joints.
	End Rem
	Method GetGroundBody:b2Body()
		Return b2Body._create(bmx_b2world_getgroundbody(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Take a time Step.
	about: This performs collision detection, integration, and constraint solution.
	/// @param timeStep the amount of time To simulate, this should Not vary.
	/// @param iterations the number of iterations To be used by the constraint solver.
	End Rem
	Method DoStep(timeStep:Float, iterations:Int)
		bmx_b2world_dostep(b2ObjectPtr, timeStep, iterations)
	End Method

	Rem
	bbdoc:  Query the world For all shapes that potentially overlap the
	/// provided AABB. You provide a shape pointer buffer of specified
	/// size. The number of shapes found is returned.
	/// @param aabb the query box.
	/// @param shapes a user allocated shape pointer array of size maxCount (Or greater).
	/// @param maxCount the capacity of the shapes array.
	/// @Return the number of shapes found in aabb.
	End Rem
	'int32 Query(Const b2AABB& aabb, b2Shape** shapes, int32 maxCount);

	Rem
	bbdoc: Get the world shape list. These shapes May Or May Not be attached To bodies.
	/// With the returned shape, use b2Shape::GetWorldNext To get the Next shape in
	/// the world list. A Null shape indicates the End of the list.
	/// @Return the head of the world shape list.
	End Rem
	Method GetBodyList:b2Body()
		Return b2Body._create(bmx_b2world_getbodylist(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the world shape list.
	returns: The head of the world shape list.
	about: These shapes may or may not be attached to bodies. With the returned shape, use 
	b2Shape::GetWorldNext To get the next shape in the world list. A Null shape indicates the end of the
	list.
	End Rem
	Method GetJointList:b2Joint()
		Return b2Joint._create(bmx_b2world_getjointlist(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the world shape list.
	returns: The head of the world shape list.
	about: These shapes may or may not be attached to bodies. With the returned shape, use
	b2Shape::GetWorldNext To get the Next shape in the world list. A Null shape indicates the end of the
	list.
	End Rem
	Method GetContactList:b2Contact[]()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetWarmStarting(flag:Int)
		bmx_b2world_setwarmstarting(b2ObjectPtr, flag)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetPositionCorrection(flag:Int)
		bmx_b2world_setpositioncorrection(b2ObjectPtr, flag)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetContinuousPhysics(flag:Int)
		bmx_b2world_setcontinuousphysics(b2ObjectPtr, flag)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Validate()
		bmx_b2world_validate(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc:  Change the global gravity vector.
	End Rem
	Method SetGravity(gravity:b2Vec2)
		bmx_b2world_setgravity(b2ObjectPtr, gravity.b2ObjectPtr)
	End Method
	
End Type

Rem
bbdoc: An axis aligned bounding box.
End Rem
Type b2AABB

	Field b2ObjectPtr:Byte Ptr 

	Rem
	bbdoc: 
	End Rem
	Function CreateAABB:b2AABB(lowerBound:b2Vec2 = Null, upperBound:b2Vec2 = Null)
		Return New b2AABB.Create(lowerBound, upperBound)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:b2AABB(lowerBound:b2Vec2 = Null, upperBound:b2Vec2 = Null)
		If lowerBound Then
			If upperBound Then
				b2ObjectPtr = bmx_b2aabb_create(lowerBound.b2ObjectPtr, upperBound.b2ObjectPtr)
			Else
				b2ObjectPtr = bmx_b2aabb_create(lowerBound.b2ObjectPtr, Null)
			End If
		Else
			If upperBound Then
				b2ObjectPtr = bmx_b2aabb_create(Null, upperBound.b2ObjectPtr)
			Else
				b2ObjectPtr = bmx_b2aabb_create(Null, Null)
			End If
		End If
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLowerBound(lowerBound:b2Vec2)
		bmx_b2abb_setlowerbound(b2ObjectPtr, lowerBound.b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetUpperBound(upperBound:b2Vec2)
		bmx_b2abb_setupperbound(b2ObjectPtr, upperBound.b2ObjectPtr)
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2aabb_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type b2Vec2

	Field b2ObjectPtr:Byte Ptr
	
	Function _create:b2Vec2(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local this:b2Vec2 = New b2Vec2
			this.b2ObjectPtr = b2ObjectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function CreateVec2:b2Vec2(x:Float = 0, y:Float = 0)
		Return New b2Vec2.Create(x, y)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:b2Vec2(x:Float = 0, y:Float = 0)
		b2ObjectPtr = bmx_b2vec2_create(x, y)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetX:Float()
		Return bmx_b2vec2_getx(b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method X:Float()
		Return bmx_b2vec2_getx(b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetY:Float()
		Return bmx_b2vec2_gety(b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Y:Float()
		Return bmx_b2vec2_gety(b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem	
	Method Add(vec:b2Vec2)
		bmx_b2vec2_add(b2ObjectPtr, vec.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Copy(vec:b2Vec2)
		bmx_b2vec2_copy(b2ObjectPtr, vec.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Subtract:b2Vec2(vec:b2Vec2)
		Return _create(bmx_b2vec2_subtract(b2ObjectPtr, vec.b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Set(x:Float, y:Float)
		bmx_b2vec2_set(b2ObjectPtr, x, y)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method Length:Float()
		Return bmx_b2vec2_length(b2ObjectPtr)
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2vec2_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
	Function _newVecArray:b2Vec2[](count:Int)
		Return New b2Vec2[count]
	End Function
	
	Function _setVec(array:b2Vec2[], index:Int, vec:Byte Ptr)
		array[index] = _create(vec)
	End Function

	Function _getVec:Byte Ptr(array:b2Vec2[], index:Int)
		Return array[index].b2ObjectPtr
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Global ZERO:b2Vec2 = New b2Vec2.Create()
	
End Type

Rem
bbdoc: 
End Rem
Type b2DestructionListener

End Type

Rem
bbdoc: Use this type for when a body's shape passes outside of the world boundary.
about: Override Violation().
End Rem
Type b2BoundaryListener

	Field b2ObjectPtr:Byte Ptr

	Method New()
		b2ObjectPtr = bmx_b2boundarylistener_new(Self)
	End Method
	
	Rem
	bbdoc: This is called for each body that leaves the world boundary.
	about: Warning: you can't modify the world inside this callback.
	End Rem
	Method Violation(body:b2Body)
	End Method

	Function _Violation(listener:b2BoundaryListener, body:Byte Ptr)
		listener.Violation(b2Body._create(body))
	End Function

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2boundarylistener_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2ContactListener

	Field b2ObjectPtr:Byte Ptr

	Method New()
		b2ObjectPtr = bmx_b2contactlistener_new(Self)
	End Method

	Rem
	bbdoc: Called when a contact point is added.
	about: This includes the geometry and the forces.
	End Rem
	Method Add(point:b2ContactPoint)
	End Method
	
	Function _Add(listener:b2ContactListener, point:Byte Ptr)
		listener.Add(b2ContactPoint._create(point))
	End Function

	Rem
	bbdoc: Called when a contact point persists.
	about: This includes the geometry and the forces.
	End Rem
	Method Persist(point:b2ContactPoint)
	End Method
	
	Function _Persist(listener:b2ContactListener, point:Byte Ptr)
		listener.Persist(b2ContactPoint._create(point))
	End Function

	Rem
	bbdoc: Called when a contact point is removed.
	about: This includes the last computed geometry and forces.
	End Rem
	Method Removed(point:b2ContactPoint)
	End Method

	Function _Removed(listener:b2ContactListener, point:Byte Ptr)
		listener.Removed(b2ContactPoint._create(point))
	End Function

	Rem
	bbdoc: Called after a contact point is solved.
	End Rem
	Method Result(result:b2ContactResult)
	End Method
	
	Function _Result(listener:b2ContactListener, result:Byte Ptr)
		listener.Result(b2ContactResult._create(result))
	End Function

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2contactlistener_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Implement this type and override ShouldCollide() to provide collision filtering.
about: In other words, you can implement this type if you want finer control over contact creation.
End Rem
Type b2ContactFilter

	Field b2ObjectPtr:Byte Ptr

	Method New()
		b2ObjectPtr = bmx_b2contactfilter_new(Self)
	End Method

	Rem
	bbdoc: Return True if contact calculations should be performed between these two shapes.
	about: Warning:  for performance reasons this is only called when the AABBs begin to overlap.
	End Rem
	Method ShouldCollide:Int(shape1:b2Shape, shape2:b2Shape)
		Return True
	End Method

	Function _ShouldCollide:Int(filter:b2ContactFilter, shape1:Byte Ptr, shape2:Byte Ptr)
		Return filter.ShouldCollide(b2Shape._create(shape1), b2Shape._create(shape2))
	End Function
	
	Method Delete()
		If b2ObjectPtr Then
			bmx_b2contactfilter_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type b2Contact
End Type

Rem
bbdoc: 
End Rem
Type b2ContactPoint

	Field b2ObjectPtr:Byte Ptr

	Function _create:b2ContactPoint(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local contactPoint:b2ContactPoint = New b2ContactPoint
			contactPoint.b2ObjectPtr = b2ObjectPtr
			Return contactPoint
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type b2ContactResult

	Field b2ObjectPtr:Byte Ptr

	Function _create:b2ContactResult(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local result:b2ContactResult = New b2ContactResult
			result.b2ObjectPtr = b2ObjectPtr
			Return result
		End If
	End Function

End Type

Rem
bbdoc: 
End Rem
Type b2Joint

	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	Function _create:b2Joint(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local joint:b2Joint = b2Joint(bmx_b2joint_getmaxjoint(b2ObjectPtr))
			If Not joint Then
				joint = New b2Joint
				joint.b2ObjectPtr = b2ObjectPtr
			Else
				If Not joint.b2ObjectPtr Then
					joint.b2ObjectPtr = b2ObjectPtr
				EndIf
			End If
			Return joint
		End If
	End Function
	
	Rem
	bbdoc: Get the first body attached to this joint.
	end rem
	Method GetBody1:b2Body()
		Return b2Body._create(bmx_b2joint_getbody1(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: Get the second body attached to this joint.
	end rem
	Method GetBody2:b2Body()
		Return b2Body._create(bmx_b2joint_getbody2(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: Get the next joint the world joint list.
	end rem
	Method GetNext:b2Joint()
		Return b2Joint._create(bmx_b2joint_getnext(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the user data pointer that was provided in the joint definition.
	End Rem
	Method GetUserData:Object()
		Return userData
	End Method


End Type

Rem
bbdoc: Bodies are the backbone for shapes.
about: Bodies carry shapes and move them around in the world. Bodies are always rigid bodies in Box2D. That
means that two shapes attached to the same rigid body never move relative to each other.
<p>
Bodies have position and velocity. You can apply forces, torques, and impulses to bodies. Bodies can be
static or dynamic. Static bodies never move and don't collide with other static bodies.
</p>
End Rem
Type b2Body
	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	Function _create:b2Body(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local body:b2Body = b2Body(bmx_b2body_getmaxbody(b2ObjectPtr))
			If Not body Then
				body = New b2Body
				body.b2ObjectPtr = b2ObjectPtr
			End If
			Return body
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method CreateShape:b2Shape(def:b2ShapeDef)
		Local shape:b2Shape = New b2Shape
		shape.userData = def.userData ' copy the userData
		shape.b2ObjectPtr = bmx_b2body_createshape(b2ObjectPtr, def.b2ObjectPtr, shape)
		Return shape
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DestroyShape(shape:b2Shape)
		bmx_b2body_destroyshape(b2ObjectPtr, shape.b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetMassFromShapes()
		bmx_b2body_setmassfromshapes(b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetPosition:b2Vec2()
		Return b2Vec2._create(bmx_b2body_getposition(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the angle in degrees.
	returns: The current world rotation angle in degrees.
	End Rem
	Method GetAngle:Float()
		Return bmx_b2body_getangle(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Get the world position of the center of mass.
	End Rem
	Method GetWorldCenter:b2Vec2()
		Return b2Vec2._create(bmx_b2body_getworldcenter(b2ObjectPtr))
	End Method

	Rem
	bbdoc:Get the Local position of the center of mass.
	End Rem
	Method GetLocalCenter:b2Vec2()
		Return b2Vec2._create(bmx_b2body_getlocalcenter(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Set the linear velocity of the center of mass.
	/// @param v the New linear velocity of the center of mass.
	End Rem
	Method SetLinearVelocity(v:b2Vec2)
		bmx_b2body_setlinearvelocity(b2ObjectPtr, v.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Get the linear velocity of the center of mass.
	returns: The linear velocity of the center of mass.
	End Rem
	Method GetLinearVelocity:b2Vec2()
		Return b2Vec2._create(bmx_b2body_getlinearvelocity(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: Set the angular velocity.
	/// @param omega the New angular velocity in degrees/Second.
	End Rem
	Method SetAngularVelocity(omega:Float)
		bmx_b2body_setangularvelocity(b2ObjectPtr, omega)
	End Method

	Rem
	bbdoc: Get the angular velocity.
	returns: The angular velocity in degrees/Second.
	End Rem
	Method GetAngularVelocity:Float()
		Return bmx_b2body_getangularvelocity(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Apply a force at a world point. If the force is not
	/// applied at the center of mass, it will generate a torque and
	/// affect the angular velocity. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param point the world position of the point of application.
	End Rem
	Method ApplyForce(force:b2Vec2, point:b2Vec2)
		bmx_b2body_applyforce(b2ObjectPtr, force.b2ObjectPtr, point.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Apply a torque.
	about: This affects the angular velocity without affecting the linear velocity of the center of mass.
	/// This wakes up the body.
	/// @param torque about the z-axis (out of the screen), usually in N-m.
	End Rem
	Method ApplyTorque(torque:Float)
		bmx_b2body_applytorque(b2ObjectPtr, torque)
	End Method

	Rem
	bbdoc: Apply an impulse at a point.
	about: This immediately modifies the velocity.
	/// It also modifies the angular velocity If the point of application
	/// is not at the center of mass. This wakes up the body.
	/// @param impulse the world impulse vector, usually in N-seconds Or kg-m/s.
	/// @param point the world position of the point of application.
	End Rem
	Method ApplyImpulse(impulse:b2Vec2, point:b2Vec2)
		bmx_b2body_applyimpulse(b2ObjectPtr, impulse.b2ObjectPtr, point.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Get the total mass of the body.
	/// @Return the mass, usually in kilograms (kg).
	End Rem
	Method GetMass:Float()
		Return bmx_b2body_getmass(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Get the central rotational inertia of the body.
	returns: The rotational inertia, usually in kg-m^2.
	End Rem
	Method GetInertia:Float()
		Return bmx_b2body_getinertia(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Get the world coordinates of a point given the local coordinates.
	returns: The same point expressed in world coordinates.
	/// @param localPoint a point on the body measured relative the the body's origin.
	End Rem
	Method GetWorldPoint:b2Vec2(localPoint:b2Vec2)
		Return b2Vec2._create(bmx_b2body_getworldpoint(b2ObjectPtr, localPoint.b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the world coordinates of a vector given the local coordinates.
	returns: The same vector expressed in world coordinates.
	/// @param localVector a vector fixed in the body.
	End Rem
	Method GetWorldVector:b2Vec2(localVector:b2Vec2)
		Return b2Vec2._create(bmx_b2body_getworldvector(b2ObjectPtr, localVector.b2ObjectPtr))
	End Method

	Rem
	bbdoc: Gets a local point relative to the body's origin given a world point.
	returns: The corresponding local point relative to the body's origin.
	/// @param a point in world coordinates.
	End Rem
	Method GetLocalPoint:b2Vec2(worldPoint:b2Vec2)
		Return b2Vec2._create(bmx_b2body_getlocalpoint(b2ObjectPtr, worldPoint.b2ObjectPtr))
	End Method

	Rem
	bbdoc: Gets a local vector given a world vector.
	returns: The corresponding local vector.
	/// @param a vector in world coordinates.
	End Rem
	Method GetLocalVector:b2Vec2(worldVector:b2Vec2)
		Return b2Vec2._create(bmx_b2body_getlocalvector(b2ObjectPtr, worldVector.b2ObjectPtr))
	End Method

	Rem
	bbdoc: Is this body treated like a bullet for continuous collision detection?
	End Rem
	Method IsBullet:Int()
		Return bmx_b2body_isbullet(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Should this body be treated like a bullet for continuous collision detection?
	End Rem
	Method SetBullet(flag:Int)
		bmx_b2body_setbullet(b2ObjectPtr, flag)
	End Method

	Rem
	bbdoc: Is this body static (immovable)?
	End Rem
	Method IsStatic:Int()
		Return bmx_b2body_isstatic(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Is this body dynamic (movable)?
	End Rem
	Method IsDynamic:Int()
		Return bmx_b2body_isdynamic(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Is this body frozen?
	End Rem
	Method IsFrozen:Int()
		Return bmx_b2body_isfrozen(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Is this body sleeping (not simulating).
	End Rem
	Method IsSleeping:Int()
		Return bmx_b2body_issleeping(b2ObjectPtr)
	End Method

	Rem
	bbdoc: You can disable sleeping on this body.
	End Rem
	Method AllowSleeping(flag:Int)
		bmx_b2body_allowsleeping(b2ObjectPtr, flag)
	End Method

	Rem
	bbdoc: Wake up this body so it will begin simulating.
	End Rem
	Method WakeUp()
		bmx_b2body_wakeup(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Put this body to sleep so it will stop simulating.
	about: This also sets the velocity to zero.
	End Rem
	Method PutToSleep()
		bmx_b2body_puttosleep(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Get the list of all shapes attached to this body.
	End Rem
	Method GetShapeList:b2Shape()
		Return b2Shape._create(bmx_b2body_getshapelist(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the list of all joints attached to this body.
	End Rem
	Method GetJointList:b2JointEdge()
		Return b2JointEdge._create(bmx_b2body_getjointlist(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the next body in the world's body list.
	End Rem
	Method GetNext:b2Body()
		Return _create(bmx_b2body_getnext(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Get the user data pointer that was provided in the body definition.
	End Rem
	Method GetUserData:Object()
		Return userData
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2Shape

	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	Function _create:b2Shape(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local shape:b2Shape = b2Shape(bmx_b2shape_getmaxshape(b2ObjectPtr))
			If Not shape Then
				shape = New b2Shape
				shape.b2ObjectPtr = b2ObjectPtr
			End If
			Return shape
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method IsSensor:Int()
		Return bmx_b2shape_issensor(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetBody:b2Body()
		Return b2Body._create(bmx_b2shape_getbody(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetUserData:Object()
		Return userData
	End Method
	
End Type

Type b2DebugDraw

	Field b2ObjectPtr:Byte Ptr

	Const e_shapeBit:Int = $0001        ' draw shapes
	Const e_jointBit:Int = $0002        ' draw joint connections
	Const e_coreShapeBit:Int = $0004    ' draw core (TOI) shapes
	Const e_aabbBit:Int = $0008         ' draw axis aligned bounding boxes
	Const e_obbBit:Int = $0010          ' draw oriented bounding boxes
	Const e_pairBit:Int = $0020         ' draw broad-phase pairs
	Const e_centerOfMassBit:Int = $0040 ' draw center of mass frame

	Method New()
		b2ObjectPtr = bmx_b2debugdraw_create(Self)
	End Method
	
	Rem
	bbdoc: Set the drawing flags.
	End Rem
	Method SetFlags(flags:Int)
		bmx_b2debugdraw_setflags(b2ObjectPtr, flags)
	End Method
	
	Rem
	bbdoc: Get the drawing flags.
	End Rem
	Method GetFlags:Int()
		Return bmx_b2debugdraw_getflags(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Append flags to the current flags.
	End Rem
	Method AppendFlags(flags:Int)
		bmx_b2debugdraw_appendflags(b2ObjectPtr, flags)
	End Method
	
	Rem
	bbdoc: Clear flags from the current flags.
	End Rem
	Method ClearFlags(flags:Int)
		bmx_b2debugdraw_clearflags(b2ObjectPtr, flags)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method DrawPolygon(vertices:b2Vec2[], color:b2Color) Abstract


	Function _DrawPolygon(obj:b2DebugDraw , vertices:b2Vec2[], r:Int, g:Int, b:Int)
		obj.DrawPolygon(vertices, b2Color.Set(r, g, b))
	End Function
	
	Rem
	bbdoc: Draw a solid closed polygon provided in CCW order
	End Rem
	Method DrawSolidPolygon(vertices:b2Vec2[], color:b2Color) Abstract

	Function _DrawSolidPolygon(obj:b2DebugDraw , vertices:b2Vec2[], r:Int, g:Int, b:Int)
		obj.DrawSolidPolygon(vertices, b2Color.Set(r, g, b))
	End Function

	Rem
	bbdoc: Draw a circle.
	End Rem
	Method DrawCircle(center:b2Vec2, radius:Float, color:b2Color) Abstract
	
	Rem
	bbdoc: Draw a solid circle.
	End Rem
	Method DrawSolidCircle(center:b2Vec2, radius:Float, axis:b2Vec2, color:b2Color) Abstract
	
	Function _DrawSolidCircle(obj:b2DebugDraw, center:Byte Ptr, radius:Float, axis:Byte Ptr, r:Int, g:Int, b:Int)
		obj.DrawSolidCircle(b2Vec2._create(center), radius, b2Vec2._create(axis), b2Color.Set(r, g, b))
	End Function
	
	Rem
	bbdoc: Draw a line segment.
	End Rem
	Method DrawSegment(p1:b2Vec2, p2:b2Vec2, color:b2Color) Abstract

	Function _DrawSegment(obj:b2DebugDraw, p1:Byte Ptr, p2:Byte Ptr, r:Int, g:Int, b:Int)
		obj.DrawSegment(b2Vec2._create(p1), b2Vec2._create(p2), b2Color.Set(r, g, b))
	End Function
	
	Rem
	bbdoc: Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	End Rem
	Method DrawXForm(xf:b2XForm) Abstract


End Type

Rem
bbdoc: 
End Rem
Type b2Color
	
	Field red:Int, green:Int, blue:Int

	Function Set:b2Color(r:Int, g:Int, b:Int)
		Local this:b2Color = New b2Color
		this.red = r
		this.green = g
		this.blue = b
		Return this
	End Function

End Type

Rem
bbdoc: 
End Rem
Type b2BodyDef

	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	'Rem
	'bbdoc: A static body should Not move And has infinite mass.
	'End Rem
	'Const e_staticBody:Int = 0
	'Rem
	'bbdoc: A regular moving body.
	'End Rem
	'Const e_dynamicBody:Int = 1
	
	Method New()
		b2ObjectPtr = bmx_b2bodydef_create()
	End Method
	
	'Rem
	'bbdoc: We need the body Type To setup collision filtering correctly, so
	'/// that static bodies don't collide with each other. You can't change
	''/// this once a body is created.
	'End Rem
	'Method SetType(bodyType:Int)
	'	bmx_b2bodydef_settype(b2ObjectPtr, bodyType)
	'End Method
	
	'Method GetType:Int()
	'End Method

	Rem
	bbdoc: You can use this To initialized the mass properties of the body.
	/// If you prefer, you can set the mass properties after the shapes
	/// have been added using b2Body::SetMassFromShapes.
	End Rem
	Method SetMassData(data:b2MassData)
		bmx_b2bodydef_setmassdata(b2ObjectPtr, data.b2ObjectPtr)
	End Method
	
	'Method GetMassData:b2MassData()
	'End Method

	Rem
	bbdoc: Use this To store application specific body data.
	End Rem
	Method SetUserData(data:Object)
		userData = data
	End Method
	
	Rem
	bbdoc: The world position of the body. Avoid creating bodies at the origin
	/// since this can lead To many overlapping shapes.
	End Rem
	Method SetPosition(position:b2Vec2)
		bmx_b2bodydef_setposition(b2ObjectPtr, position.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The world angle of the body in degrees.
	End Rem
	Method SetAngle(angle:Float)
		bmx_b2bodydef_setangle(b2ObjectPtr, angle)
	End Method
	
	Rem
	bbdoc: Linear damping is used to reduce the linear velocity.
	about: The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive To the
	/// time Step when the damping parameter is large.
	End Rem
	Method SetLinearDamping(damping:Float)
		bmx_b2bodydef_setlineardamping(b2ObjectPtr, damping)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLinearDamping:Float()
		Return bmx_b2bodydef_getlineardamping(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Angular damping is use To reduce the angular velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive To the
	/// time Step when the damping parameter is large.
	End Rem
	Method SetAngularDamping(damping:Float)
		bmx_b2bodydef_setangulardamping(b2ObjectPtr, damping)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAngularDamping:Float()
		Return bmx_b2bodydef_getangulardamping(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Set this flag to False if this body should never fall asleep.
	about: Note that this increases CPU usage.
	End Rem
	Method SetAllowSleep(allow:Int)
		bmx_b2bodydef_setallowsleep(b2ObjectPtr, allow)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAllowSleep:Int()
		Return bmx_b2bodydef_getallowsleep(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Is this body initially sleeping?
	End Rem
	Method isSleeping:Int()
		Return bmx_b2bodydef_issleeping(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetIsSleeping(sleeping:Int)
		bmx_b2bodydef_setissleeping(b2ObjectPtr, sleeping)
	End Method

	Rem
	bbdoc: Should this body be prevented from rotating? Useful For characters.
	End Rem
	Method SetFixedRotation(fixed:Int)
		bmx_b2bodydef_setfixedrotation(b2ObjectPtr, fixed)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetFixedRotation:Int()
		Return bmx_b2bodydef_getfixedrotation(b2ObjectPtr)
	End Method

	Rem
	bbdoc: Is this a fast moving body that should be prevented from tunneling through other moving bodies?
	about: Note that all bodies are prevented from tunneling through static bodies.
	<p>
	Warning: You should use this flag sparingly since it increases processing time.
	</p>
	End Rem
	Method SetIsBullet(bullet:Int)
		bmx_b2bodydef_setisbullet(b2ObjectPtr, bullet)
	End Method
	
	Method Delete()
		If b2ObjectPtr Then
			bmx_b2bodydef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type b2JointDef

	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	Rem
	bbdoc: The First attached body.
	End Rem
	Method SetBody1(body:b2Body)
		bmx_b2jointdef_setbody1(b2ObjectPtr, body.b2ObjectPtr)
	End Method
	
	Method GetBody1:b2Body()
		Return b2Body._create(bmx_b2jointdef_getbody1(b2ObjectPtr))
	End Method

	Rem
	bbdoc: The Second attached body.
	End Rem
	Method SetBody2(body:b2Body)
		bmx_b2jointdef_setbody2(b2ObjectPtr, body.b2ObjectPtr)
	End Method
	
	Method GetBody2:b2Body()
		Return b2Body._create(bmx_b2jointdef_getbody2(b2ObjectPtr))
	End Method

	Rem
	bbdoc: Set this flag To True If the attached bodies should collide.
	End Rem
	Method SetCollideConnected(collideConnected:Int)
		bmx_b2jointdef_setcollideconnected(b2ObjectPtr, collideConnected)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetCollideConnected:Int()
		Return bmx_b2jointdef_getcollideconnected(b2ObjectPtr)
	End Method

End Type

Rem
bbdoc: A joint edge is used to connect bodies and joints together in a joint graph where each body is a node and each joint is an edge.
about: A joint edge belongs to a doubly linked list maintained in each attached body. Each joint has two
joint nodes, one for each attached body.
End Rem
Type b2JointEdge

	Field b2ObjectPtr:Byte Ptr

	Function _create:b2JointEdge(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local this:b2JointEdge = New b2JointEdge
			this.b2ObjectPtr = b2ObjectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetOther:b2Body()
		Return b2Body._create(bmx_b2jointedge_getother(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetJoint:b2Joint()
		Return b2Joint._create(bmx_b2jointedge_getjoint(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPrev:b2JointEdge()
		Return b2JointEdge._create(bmx_b2jointedge_getprev(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNext:b2JointEdge()
		Return b2JointEdge._create(bmx_b2jointedge_getnext(b2ObjectPtr))
	End Method
	
End Type

Rem
bbdoc: Holds the mass data computed for a shape.
End Rem
Type b2MassData

	Field b2ObjectPtr:Byte Ptr

	Method New()
		b2ObjectPtr = bmx_b2massdata_new()
	End Method
	
	Method Delete()
		If b2ObjectPtr Then
			bmx_b2massdata_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method

	Rem
	bbdoc: Sets the mass of the shape, usually in kilograms.
	End Rem	
	Method SetMass(mass:Float)	
		bmx_b2massdata_setmass(b2ObjectPtr, mass)
	End Method
	
	Rem
	bbdoc: Sets the position of the shape's centroid relative to the shape's origin.
	End Rem
	Method SetCenter(center:b2Vec2)
		bmx_b2massdata_setcenter(b2ObjectPtr, center.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Sets the rotational inertia of the shape.
	End Rem
	Method SetRotationalInertia(i:Float)
		bmx_b2massdata_seti(b2ObjectPtr, i)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type b2ShapeDef

	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	Rem
	bbdoc: 
	End Rem
	Method SetFriction(friction:Float)
		bmx_b2shapedef_setfriction(b2ObjectPtr, friction)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetRestitution(restitution:Float)
		bmx_b2shapedef_setrestitution(b2ObjectPtr, restitution)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetDensity(density:Float)
		bmx_b2shapedef_setdensity(b2ObjectPtr, density)
	End Method
	
	
End Type

Rem
bbdoc: Convex polygon.
about: Vertices must be in CCW order.
End Rem
Type b2PolygonDef Extends b2ShapeDef

	Method New()
		b2ObjectPtr = bmx_b2polygondef_create()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAsBox(hx:Float, hy:Float)
		bmx_b2polygondef_setasbox(b2ObjectPtr, hx, hy)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetAsOrientedBox(hx:Float, hy:Float, center:b2Vec2, angle:Float)
		bmx_b2polygondef_setasorientedbox(b2ObjectPtr, hx, hy, center.b2ObjectPtr, angle)
	End Method
	
	Method SetVertices(vertices:b2Vec2[])
		bmx_b2polygondef_setvertices(b2ObjectPtr, vertices)
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2polygondef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Extern
	Function bmx_b2polygondef_setvertices(handle:Byte Ptr, vertices:b2Vec2[])
End Extern

Rem
bbdoc: Used to build circle shapes.
End Rem
Type b2CircleDef Extends b2ShapeDef

	Method New()
		b2ObjectPtr = bmx_b2circledef_create()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetRadius(radius:Float)
		bmx_b2circledef_setradius(b2ObjectPtr, radius)
	End Method
	
	Method GetRadius:Float()
		Return bmx_b2circledef_getradius(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLocalPosition(pos:b2Vec2)
		bmx_b2circledef_setlocalposition(b2ObjectPtr, pos.b2ObjectPtr)
	End Method
	
	Method GetLocalPosition:b2Vec2()
		Return b2Vec2._create(bmx_b2circledef_getlocalposition(b2ObjectPtr))
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2circledef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2RevoluteJointDef Extends b2JointDef

	Method New()
		b2ObjectPtr = bmx_b2revolutejointdef_create()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Initialize(body1:b2Body, body2:b2Body, anchor:b2Vec2)
		bmx_b2revolutejointdef_initialize(b2ObjectPtr, body1.b2ObjectPtr, body2.b2ObjectPtr, anchor.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The local anchor point relative to body1's origin.
	end rem
	Method GetLocalAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetLocalAnchor1(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc: The local anchor point relative to body2's origin.
	end rem
	Method GetLocalAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetLocalAnchor2(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc: The body2 angle minus body1 angle in the reference state (radians).
	end rem
	Method GetReferenceAngle:Float()
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetReferenceAngle(angle:Float)
	End Method
	
	Rem
	bbdoc: A flag to enable joint limits.
	end rem
	Method IsLimitEnabled:Int()
		Return bmx_b2revolutejointdef_islimitenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method EnableLimit(limit:Int)
		bmx_b2revolutejointdef_enablelimit(b2ObjectPtr, limit)
	End Method
	
	Rem
	bbdoc: The lower angle for the joint limit (radians).
	end rem
	Method GetLowerAngle:Float()
		Return bmx_b2revolutejointdef_getlowerangle(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetLowerAngle(angle:Float)
		bmx_b2revolutejointdef_setlowerangle(b2ObjectPtr, angle)
	End Method
	
	Rem
	bbdoc: The upper angle for the joint limit (radians).
	end rem
	Method GetUpperAngle:Float()
		Return bmx_b2revolutejointdef_getupperangle(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetUpperAngle(angle:Float)
		bmx_b2revolutejointdef_setupperangle(b2ObjectPtr, angle)
	End Method
	
	Rem
	bbdoc: A flag to enable the joint motor.
	end rem
	Method IsMotorEnabled:Int()
		Return bmx_b2revolutejointdef_ismotorenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method EnableMotor(value:Int)
		bmx_b2revolutejointdef_enablemotor(b2ObjectPtr, value)
	End Method
	
	Rem
	bbdoc: The desired motor speed. Usually in radians per second.
	end rem
	Method GetMotorSpeed:Float()
		Return bmx_b2revolutejointdef_getmotorspeed(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetMotorSpeed(speed:Float)
		bmx_b2revolutejointdef_setmotorspeed(b2ObjectPtr, speed)
	End Method
	
	Rem
	bbdoc: The maximum motor torque used to achieve the desired motor speed.
	/// Usually in N-m.
	end rem
	Method GetMaxMotorTorque:Float()
		Return bmx_b2revolutejointdef_getmaxmotortorque(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method SetMaxMotorTorque(torque:Float)
		bmx_b2revolutejointdef_setmaxmotortorque(b2ObjectPtr, torque)
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2revolutejointdef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc:
End Rem
Type b2PulleyJointDef Extends b2JointDef
	
	Rem
	bbdoc: Initialize the bodies, anchors, lengths, max lengths, and ratio using the world anchors.
	End Rem
	Method Initialize(body1:b2Body, body2:b2Body, groundAnchor1:b2Vec2, groundAnchor2:b2Vec2, ..
	                anchor1:b2Vec2, anchor2:b2Vec2, ratio:Float)
	End Method
	
	Rem
	bbdoc: The first ground anchor in world coordinates. This point never moves.
	end rem
	Method SetGroundAnchor1(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetGroundAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: The second ground anchor in world coordinates. This point never moves.
	end rem
	Method SetGroundAnchor2(anchod:b2Vec2)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetGroundAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: The local anchor point relative to body1's origin.
	end rem
	Method SetLocalAnchor1(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetLocalAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: The local anchor point relative to body2's origin.
	end rem
	Method SetLocalAnchor2(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetLocalAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: The a reference length for the segment attached to body1.
	end rem
	Method SetLength1(length:Float)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetLength1:Float()
	End Method
	
	Rem
	bbdoc: The maximum length of the segment attached to body1.
	end rem
	Method SetMaxLength1(maxLength:Float)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetMaxLength1:Float()
	End Method
	
	Rem
	bbdoc: The a reference length for the segment attached to body2.
	end rem
	Method SetLength2(length:Float)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetLength2:Float()
	End Method
	
	Rem
	bbdoc: The maximum length of the segment attached to body2.
	end rem
	Method SetMaxLength2(maxLength:Float)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetMaxLength2:Float()
	End Method
	
	Rem
	bbdoc: The pulley ratio, used to simulate a block-and-tackle.
	end rem
	Method SetRatio(ratio:Float)
	End Method
	
	Rem
	bbdoc:
	end rem
	Method GetRatio:Float()
	End Method

End Type

Type b2PrismaticJointDef Extends b2JointDef

	Method New()
		b2ObjectPtr = bmx_b2prismaticjointdef_create()
	End Method
	
	Rem
	bbdoc: Initialize the bodies, anchors, axis, and reference angle using the world
	/// anchor and world axis.
	end rem
	Method Initialize(body1:b2Body, body2:b2Body, anchor:b2Vec2, axis:b2Vec2)
		bmx_b2prismaticjointdef_initialize(b2ObjectPtr, body1.b2ObjectPtr, body2.b2ObjectPtr, ..
				anchor.b2ObjectPtr, axis.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The local anchor point relative to body1's origin.
	end rem
	Method SetLocalAnchor1(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetLocalAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: The local anchor point relative to body2's origin.
	end rem
	Method SetLocalAnchor2(anchor:b2Vec2)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetLocalAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: The local translation axis in body1.
	end rem
	Method SetLocalAxis1(axis:b2Vec2)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetLocalAxis1:b2Vec2()
	End Method
	
	Rem
	bbdoc: The constrained angle between the bodies: body2_angle - body1_angle.
	end rem
	Method SetReferenceAngle(angle:Float)
	End Method
	
	Method GetReferenceAngle:Float()
	End Method
	
	Rem
	bbdoc: Enable/disable the joint limit.
	end rem
	Method EnableLimit(value:Int)
		bmx_b2prismaticjointdef_enablelimit(b2ObjectPtr, value)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method IsLimitEnabled:Int()
		Return bmx_b2prismaticjointdef_islimitenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The lower translation limit, usually in meters.
	end rem
	Method SetLowerTranslation(translation:Float)
		bmx_b2prismaticjointdef_setlowertranslation(b2ObjectPtr, translation)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetLowerTranslation:Float()
		Return bmx_b2prismaticjointdef_getlowertranslation(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The upper translation limit, usually in meters.
	end rem
	Method SetUpperTranslation(translation:Float)
		bmx_b2prismaticjointdef_setuppertranslation(b2ObjectPtr, translation)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetUpperTranslation:Float()
		Return bmx_b2prismaticjointdef_getuppertranslation(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Enable/disable the joint motor.
	end rem
	Method EnableMotor(value:Int)
		bmx_b2prismaticjointdef_enablemotor(b2ObjectPtr, value)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method IsMotorEnabled:Int()
		Return bmx_b2prismaticjointdef_ismotorenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The maximum motor torque, usually in N-m.
	end rem
	Method SetMaxMotorForce(force:Float)
		bmx_b2prismaticjointdef_setmaxmotorforce(b2ObjectPtr, force)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetMaxMotorForce:Float()
		Return bmx_b2prismaticjointdef_getmaxmotorforce(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: The desired motor speed in degrees per second.
	End Rem
	Method SetMotorSpeed(speed:Float)
		bmx_b2prismaticjointdef_setmotorspeed(b2ObjectPtr, speed)
	End Method
	
	Rem
	bbdoc: The motorspeed, in degrees per second.
	End Rem
	Method GetMotorSpeed:Float()
		Return bmx_b2prismaticjointdef_getmotorspeed(b2ObjectPtr)
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2prismaticjointdef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method

End Type

Type b2MouseJointDef Extends b2JointDef
	
	Rem
	bbdoc: The initial world target point.
	about: This is assumed to coincide with the body anchor initially.
	End Rem
	Method SetTarget(target:b2Vec2)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetTarget:b2Vec2()
	End Method
	
	Rem
	bbdoc: The maximum constraint force that can be exerted to move the candidate body.
	about: Usually you will express as some multiple of the weight (multiplier * mass * gravity).
	End Rem
	Method SetMaxForce(maxForce:Float)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetMaxForce:Float()
	End Method
	
	Rem
	bbdoc: The response speed.
	end rem
	Method SetFrequencyHz(frequency:Float)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetFrequencyHz:Float()
	End Method
	
	Rem
	bbdoc: The damping ratio.
	about: 0 = no damping, 1 = critical damping.
	End Rem
	Method SetDampingRatio(ratio:Float)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetDampingRatio:Float()
	End Method
	
	Rem
	bbdoc: The time step used in the simulation.
	end rem
	Method SetTimeStep(timeStep:Float)
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetTimeStep:Float()
	End Method

End Type

Type b2GearJointDef Extends b2JointDef
End Type

Rem
bbdoc: Distance joint definition.
about: This requires defining an anchor point on both bodies and the non-zero length of the
distance joint. The definition uses local anchor points so that the initial configuration can violate the
constraint slightly. This helps when saving and loading a game.
<p>
Warning: Do not use a zero or short length.
</p>
End Rem
Type b2DistanceJointDef Extends b2JointDef

	Method New()
		b2ObjectPtr = bmx_b2distancejointdef_new()
	End Method
	
	Method Initialize(body1:b2Body, body2:b2Body, anchor1:b2Vec2, anchor2:b2Vec2)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLocalAnchor1(anchor:b2Vec2)
		bmx_b2distancejointdef_setlocalanchor1(b2ObjectPtr, anchor.b2ObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetLocalAnchor1:b2Vec2()
		Return b2Vec2._create(bmx_b2distancejointdef_getlocalanchor1(b2ObjectPtr))
	End Method

	Rem
	 The Local anchor point relative To body2's origin.
	End Rem
	Method SetLocalAnchor2(anchor:b2Vec2)
		bmx_b2distancejointdef_setlocalanchor2(b2ObjectPtr, anchor.b2ObjectPtr)
	End Method

	Rem
	 The Local anchor point relative To body2's origin.
	End Rem
	Method GetLocalAnchor2:b2Vec2()
		Return b2Vec2._create(bmx_b2distancejointdef_getlocalanchor2(b2ObjectPtr))
	End Method

	Rem
	 The equilibrium length between the anchor points.
	End Rem
	Method SetLength(length:Float)
		bmx_b2distancejointdef_setlength(b2ObjectPtr, length)
	End Method

	Rem
	 The equilibrium length between the anchor points.
	End Rem
	Method GetLength:Float()
		Return bmx_b2distancejointdef_getlength(b2ObjectPtr)
	End Method

	Rem
	 The response speed.
	End Rem
	Method SetFrequencyHz(freq:Float)
	End Method

	Rem
	 The damping ratio. 0 = no damping, 1 = critical damping.
	End Rem
	Method SetDampingRatio(ratio:Float)
	End Method

	
	Method Delete()
		If b2ObjectPtr Then
			bmx_b2distancejointdef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2DistanceJoint Extends b2Joint

	Rem
	bbdoc: 
	End Rem
	Method GetAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetReactionForce:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetReactionTorque:Float()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2RevoluteJoint Extends b2Joint
	
	Rem
	bbdoc: 
	End Rem
	Method GetAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetReactionForce:b2Vec2()
	End Method
	
	Rem
	bbdoc: 
	end rem
	Method GetReactionTorque:Float()
	End Method
	
	Rem
	bbdoc: Get the current joint angle in radians.
	end rem
	Method GetJointAngle:Float()
	End Method
	
	Rem
	bbdoc: Get the current joint angle speed in radians per second.
	end rem
	Method GetJointSpeed:Float()
	End Method
	
	Rem
	bbdoc: Is the joint limit enabled?
	end rem
	Method IsLimitEnabled:Float()
		Return bmx_b2revolutejoint_islimitenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Enable/disable the joint limit.
	end rem
	Method EnableLimit(flag:Int)
		bmx_b2revolutejoint_enablelimit(b2ObjectPtr, flag)
	End Method
	
	Rem
	bbdoc: Get the lower joint limit in degrees.
	End Rem
	Method GetLowerLimit:Float()
		Return bmx_b2revolutejoint_getlowerlimit(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Get the upper joint limit in degrees.
	End Rem
	Method GetUpperLimit:Float()
		Return bmx_b2revolutejoint_getupperlimit(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Set the joint limits in degrees.
	End Rem
	Method SetLimits(lowerLimit:Float, upperLimit:Float)
		bmx_b2revolutejoint_setlimits(b2ObjectPtr, lowerLimit, upperLimit)
	End Method
	
	Rem
	bbdoc: Is the joint motor enabled?
	end rem
	Method IsMotorEnabled:Int()
		Return bmx_b2revolutejoint_ismotorenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Enable/disable the joint motor.
	end rem
	Method EnableMotor(flag:Int)
		bmx_b2revolutejoint_enablemotor(b2ObjectPtr, flag)
	End Method
	
	Rem
	bbdoc: Set the motor speed in radians per second.
	end rem
	Method SetMotorSpeed(speed:Float)
		bmx_b2revolutejoint_setmotorspeed(b2ObjectPtr, speed)
	End Method
	
	Rem
	bbdoc: Get the motor speed in radians per second.
	end rem
	Method GetMotorSpeed:Float()
		Return bmx_b2revolutejoint_getmotorspeed(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Set the maximum motor torque, usually in N-m.
	end rem
	Method SetMaxMotorTorque(torque:Float)
		bmx_b2revolutejoint_setmaxmotortorque(b2ObjectPtr, torque)
	End Method
	
	Rem
	bbdoc: Get the current motor torque, usually in N-m.
	end rem
	Method GetMotorTorque:Float()
		Return bmx_b2revolutejoint_getmotortorque(b2ObjectPtr)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2PrismaticJoint Extends b2Joint
	
	Method GetAnchor1:b2Vec2()
	End Method
	
	Method GetAnchor2:b2Vec2()
	End Method
	
	Method GetReactionForce:b2Vec2()
	End Method
	
	Method GetReactionTorque:Float()
	End Method
	
	Rem
	bbdoc: Get the current joint translation, usually in meters.
	End Rem
	Method GetJointTranslation:Float()
	End Method
	
	Rem
	bbdoc: Get the current joint translation speed, usually in meters per second.
	end rem
	Method GetJointSpeed:Float()
		Return bmx_b2prismaticjoint_getjointspeed(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Is the joint limit enabled?
	end rem
	Method IsLimitEnabled:Int()
		Return bmx_b2prismaticjoint_islimitenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Enable/disable the joint limit.
	end rem
	Method EnableLimit(flag:Int)
		bmx_b2prismaticjoint_enablelimit(b2ObjectPtr, flag)
	End Method
	
	Rem
	bbdoc: Get the lower joint limit, usually in meters.
	end rem
	Method GetLowerLimit:Float()
		Return bmx_b2prismaticjoint_getlowerlimit(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Get the upper joint limit, usually in meters.
	end rem
	Method GetUpperLimit:Float()
		Return bmx_b2prismaticjoint_getupperlimit(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Set the joint limits, usually in meters.
	end rem
	Method SetLimits(lowerLimit:Float, upperLimit:Float)
		bmx_b2prismaticjoint_setlimits(b2ObjectPtr, lowerLimit, upperLimit)
	End Method
	
	Rem
	bbdoc: Is the joint motor enabled?
	end rem
	Method IsMotorEnabled:Int()
		Return bmx_b2prismaticjoint_ismotorenabled(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Enable/disable the joint motor.
	end rem
	Method EnableMotor(flag:Int)
		bmx_b2prismaticjoint_enablemotor(b2ObjectPtr, flag)
	End Method
	
	Rem
	bbdoc: Set the motor speed, usually in meters per second.
	end rem
	Method SetMotorSpeed(speed:Float)
		bmx_b2prismaticjoint_setmotorspeed(b2ObjectPtr, speed)
	End Method
	
	Rem
	bbdoc: Get the motor speed, usually in meters per second.
	end rem
	Method GetMotorSpeed:Float()
		Return bmx_b2prismaticjoint_getmotorspeed(b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: Set the maximum motor force, usually in N.
	end rem
	Method SetMaxMotorForce(force:Float)
		bmx_b2prismaticjoint_setmaxmotorforce(b2ObjectPtr, force)
	End Method
	
	Rem
	bbdoc: Get the current motor force, usually in N.
	end rem
	Method GetMotorForce:Float()
		Return bmx_b2prismaticjoint_getmotorforce(b2ObjectPtr)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2PulleyJoint Extends b2Joint
	
	Method GetAnchor1:b2Vec2()
	End Method
	
	Method GetAnchor2:b2Vec2()
	End Method
	
	Method GetReactionForce:b2Vec2()
	End Method
	
	Method GetReactionTorque:Float()
	End Method
	
	Rem
	bbdoc: Get the first ground anchor.
	End Rem
	Method GetGroundAnchor1:b2Vec2()
	End Method
	
	Rem
	bbdoc: Get the second ground anchor.
	end rem
	Method GetGroundAnchor2:b2Vec2()
	End Method
	
	Rem
	bbdoc: Get the current length of the segment attached to body1.
	end rem
	Method GetLength1:Float()
	End Method
	
	Rem
	bbdoc: Get the current length of the segment attached to body2.
	end rem
	Method GetLength2:Float()
	End Method
	
	Rem
	bbdoc: Get the pulley ratio.
	end rem
	Method GetRatio:Float()
	End Method

End Type

Type b2MouseJoint Extends b2Joint

	Method GetAnchor1:b2Vec2()
	End Method
	
	Method GetAnchor2:b2Vec2()
	End Method
	
	Method GetReactionForce:b2Vec2()
	End Method
	
	Method GetReactionTorque:Float()
	End Method
	
	Rem
	bbdoc: Use this to update the target point.
	end rem
	Method SetTarget(target:b2Vec2)
	End Method

End Type

Rem
bbdoc: A gear joint is used to connect two joints together.
about: Either joint can be a revolute or prismatic joint. You specify a gear ratio to bind the motions
together:
<pre>
	coordinate1 + ratio * coordinate2 = constant
</pre>
The ratio can be negative or positive. If one joint is a revolute joint and the other joint is a prismatic
joint, then the ratio will have units of length or units of 1/length.
<p>
Warning: The revolute and prismatic joints must be attached to fixed bodies (which must be body1 on those
joints).
</p>
End Rem
Type b2GearJoint Extends b2Joint

	Method GetAnchor1:b2Vec2()
	End Method
	
	Method GetAnchor2:b2Vec2()
	End Method
	
	Method GetReactionForce:b2Vec2()
	End Method
	
	Method GetReactionTorque:Float()
	End Method
	
	Rem
	bbdoc: Get the gear ratio.
	End Rem
	Method GetRatio:Float()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type b2XForm
	
	Field b2ObjectPtr:Byte Ptr
	Field owner:Int
	
	Function _create:b2XForm(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local this:b2XForm = New b2XForm
			this.b2ObjectPtr = b2ObjectPtr
			this.owner = False
			Return this
		End If
	End Function
	
	Method Create:b2XForm()
		b2ObjectPtr = bmx_b2xform_create()
		owner = True
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPosition:b2Vec2()
		Return b2Vec2._create(bmx_b2xform_getposition(b2ObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPosition(pos:b2Vec2)
		bmx_b2xform_setposition(b2ObjectPtr, pos.b2ObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetR:b2Mat22()
		Return b2Mat22._create(bmx_b2xform_getr(b2ObjectPtr))	
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetR(r:b2Mat22)
		bmx_b2xform_setr(b2ObjectPtr, r.b2ObjectPtr)
	End Method
	
	Method Delete()
		If b2ObjectPtr And owner Then
			bmx_b2xform_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type b2Mat22

	Field b2ObjectPtr:Byte Ptr
	
	Function _create:b2Mat22(b2ObjectPtr:Byte Ptr)
		If b2ObjectPtr Then
			Local this:b2Mat22 = New b2Mat22
			this.b2ObjectPtr = b2ObjectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Initialize this matrix using an angle. This matrix becomes
	/// an orthonormal rotation matrix.
	End Rem
	Method SetAngle(angle:Float)
		bmx_b2mat22_setangle(b2ObjectPtr, angle)
	End Method
	
End Type

Rem
bbdoc: Perform the cross product on a vector and a scalar.
about: In 2D this produces a vector.
End Rem
Function b2Cross:b2Vec2(a:b2Vec2, s:Float)
	Return b2Vec2._create(bmx_b2cross(a.b2ObjectPtr, s))
End Function

Rem
bbdoc: Perform the cross product on a scalar and a vector.
about: In 2D this produces a vector.
End Rem
Function b2CrossF:b2Vec2(s:Float, a:b2Vec2)
	Return b2Vec2._create(bmx_b2crossf(s, a.b2ObjectPtr))
End Function

Rem
bbdoc: Multiply a matrix times a vector.
about: If a rotation matrix is provided, then this transforms the vector from one frame to another.
End Rem
Function b2Mul:b2Vec2(A:b2Mat22, v:b2Vec2)
	Return b2Vec2._create(bmx_b2mul(A.b2ObjectPtr, v.b2ObjectPtr))
End Function

Rem
bbdoc: Multiply a matrix transpose times a vector.
about: If a rotation matrix is provided, then this transforms the vector from one frame to another
(inverse transform).
End Rem
Function b2MulT:b2Vec2(A:b2Mat22, v:b2Vec2)
	Return b2Vec2._create(bmx_b2mult(A.b2ObjectPtr, v.b2ObjectPtr))
End Function

Rem
bbdoc: 
end rem
Function b2MulF:b2Vec2(T:b2XForm, v:b2Vec2)
	Return b2Vec2._create(bmx_b2mulf(T.b2ObjectPtr, v.b2ObjectPtr))
End Function

Rem
bbdoc: 
end rem
Function b2MulTF:b2Vec2(T:b2XForm, v:b2Vec2)
	Return b2Vec2._create(bmx_b2multf(T.b2ObjectPtr, v.b2ObjectPtr))
End Function

