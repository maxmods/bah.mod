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
	End Method

	Rem
	bbdoc: Register a contact filter To provide specific control over collision.
	/// Otherwise the Default filter is used (b2_defaultFilter).
	End Rem
	Method SetFilter(filter:b2ContactFilter)
	End Method

	Rem
	bbdoc: Register a contact event listener
	End Rem
	Method SetContactListener(listener:b2ContactListener)
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
	bbdoc: Create a static rigid body given a definition
	about: No reference to the definition is retained.
	/// @warning This Function is locked during callbacks.
	End Rem
	Method CreateStaticBody:b2Body(def:b2BodyDef)
		Local body:b2Body = New b2Body
		body.userData = def.userData ' copy the userData
		body.b2ObjectPtr = bmx_b2world_createstaticbody(b2ObjectPtr, def.b2ObjectPtr, body)
		Return body
	End Method

	Rem
	bbdoc: Create a dynamic rigid body given a definition.
	about: No reference to the definition is retained.
	/// @warning This Function is locked during callbacks.
	End Rem
	Method CreateDynamicBody:b2Body(def:b2BodyDef)
		Local body:b2Body = New b2Body
		body.userData = def.userData ' copy the userData
		body.b2ObjectPtr = bmx_b2world_createdynamicbody(b2ObjectPtr, def.b2ObjectPtr, body)
		Return body
	End Method

	Rem
	bbdoc: Destroy a rigid body given a definition. No reference To the definition
	/// is retained. This Function is locked during callbacks.
	/// @warning This automatically deletes all associated shapes And joints.
	/// @warning This Function is locked during callbacks.
	End Rem
	Method DestroyBody(body:b2Body)
		bmx_b2world_destroybody(b2ObjectPtr, body.b2ObjectPtr)
	End Method

	Rem
	bbdoc: Create a joint To constrain bodies together. No reference To the definition
	/// is retained. This May cause the connected bodies To cease colliding.
	/// @warning This Function is locked during callbacks.
	End Rem
	Method CreateJoint:b2Joint(def:b2JointDef)
		Local joint:b2Joint = New b2Joint
		joint.userData = def.userData ' copy the userData
		joint.b2ObjectPtr = bmx_b2world_createjoint(b2ObjectPtr, def.b2ObjectPtr, joint)
		Return joint
	End Method

	Rem
	bbdoc: Destroy a joint. This May cause the connected bodies To begin colliding.
	/// @warning This Function is locked during callbacks.
	End Rem
	Method DestroyJoint(joint:b2Joint)
		bmx_b2world_destroyjoint(b2ObjectPtr, joint.b2ObjectPtr)
	End Method

	Rem
	bbdoc: The world provides a single static ground body with no collision shapes.
	/// You can use this To simplify the creation of joints.
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
	Method GetShapeList:b2Shape[]()
	End Method

	Rem
	bbdoc: Get the world shape list. These shapes May Or May Not be attached To bodies.
	/// With the returned shape, use b2Shape::GetWorldNext To get the Next shape in
	/// the world list. A Null shape indicates the End of the list.
	/// @Return the head of the world shape list.
	End Rem
	Method GetBodyList:b2Body[]()
	End Method

	Rem
	bbdoc: Get the world shape list. These shapes May Or May Not be attached To bodies.
	/// With the returned shape, use b2Shape::GetWorldNext To get the Next shape in
	/// the world list. A Null shape indicates the End of the list.
	/// @Return the head of the world shape list.
	End Rem
	Method GetJointList:b2Joint[]()
	End Method

	Rem
	bbdoc: Get the world shape list. These shapes May Or May Not be attached To bodies.
	/// With the returned shape, use b2Shape::GetWorldNext To get the Next shape in
	/// the world list. A Null shape indicates the End of the list.
	/// @Return the head of the world shape list.
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
	Function CreateVec2:b2Vec2(x:Float, y:Float)
		Return New b2Vec2.Create(x, y)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:b2Vec2(x:Float, y:Float)
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
	
End Type

Rem
bbdoc: 
End Rem
Type b2DestructionListener

End Type

Rem
bbdoc: 
End Rem
Type b2BoundaryListener
End Type

Rem
bbdoc: 
End Rem
Type b2ContactListener
End Type

Rem
bbdoc: 
End Rem
Type b2Contact
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
			End If
			Return joint
		End If
	End Function

	Rem
	bbdoc: Get the user data pointer that was provided in the joint definition.
	End Rem
	Method GetUserData:Object()
		Return userData
	End Method


End Type

Rem
bbdoc: 
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
	End Method

	Rem
	bbdoc:Get the Local position of the center of mass.
	End Rem
	Method GetLocalCenter:b2Vec2()
	End Method

	Rem
	bbdoc: Set the linear velocity of the center of mass.
	/// @param v the New linear velocity of the center of mass.
	End Rem
	Method SetLinearVelocity(v:b2Vec2)
	End Method

	Rem
	bbdoc: Get the linear velocity of the center of mass.
	/// @Return the linear velocity of the center of mass
	End Rem
	Method GetLinearVelocity:b2Vec2()
	End Method

	Rem
	bbdoc: Set the angular velocity.
	/// @param omega the New angular velocity in radians/Second.
	End Rem
	Method SetAngularVelocity(omega:Float)
	End Method

	Rem
	bbdoc: Get the angular velocity.
	/// @Return the angular velocity in radians/Second.
	End Rem
	Method GetAngularVelocity:Float()
	End Method

	Rem
	bbdoc: Apply a force at a world point. If the force is not
	/// applied at the center of mass, it will generate a torque and
	/// affect the angular velocity. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param point the world position of the point of application.
	End Rem
	Method ApplyForce(force:b2Vec2, point:b2Vec2)
	End Method

	Rem
	bbdoc: Apply a torque. This affects the angular velocity
	/// without affecting the linear velocity of the center of mass.
	/// This wakes up the body.
	/// @param torque about the z-axis (out of the screen), usually in N-m.
	End Rem
	Method ApplyTorque(torque:Float)
	End Method

	Rem
	bbdoc: Apply an impulse at a point. This immediately modifies the velocity.
	/// It also modifies the angular velocity If the point of application
	/// is Not at the center of mass. This wakes up the body.
	/// @param impulse the world impulse vector, usually in N-seconds Or kg-m/s.
	/// @param point the world position of the point of application.
	End Rem
	Method ApplyImpulse(impulse:b2Vec2, point:b2Vec2)
	End Method

	Rem
	bbdoc: Get the total mass of the body.
	/// @Return the mass, usually in kilograms (kg).
	End Rem
	Method GetMass:Float()
	End Method

	Rem
	bbdoc: Get the central rotational inertia of the body.
	/// @return the rotational inertia, usually in kg-m^2.
	End Rem
	Method GetInertia:Float()
	End Method

	Rem
	bbdoc: Get the world coordinates of a point given the local coordinates.
	/// @param localPoint a point on the body measured relative the the body's origin.
	/// @return the same point expressed in world coordinates.
	End Rem
	Method GetWorldPoint:b2Vec2(localPoint:b2Vec2)
	End Method

	Rem
	bbdoc: Get the world coordinates of a vector given the local coordinates.
	/// @param localVector a vector fixed in the body.
	/// @return the same vector expressed in world coordinates.
	End Rem
	Method GetWorldVector:b2Vec2(localVector:b2Vec2)
	End Method

	Rem
	bbdoc: Gets a local point relative to the body's origin given a world point.
	/// @param a point in world coordinates.
	/// @return the corresponding local point relative to the body's origin.
	End Rem
	Method GetLocalPoint:b2Vec2(worldPoint:b2Vec2)
	End Method

	Rem
	bbdoc: Gets a local vector given a world vector.
	/// @param a vector in world coordinates.
	/// @return the corresponding local vector.
	End Rem
	Method GetLocalVector:b2Vec2(worldVector:b2Vec2)
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
	/// This also sets the velocity to zero.
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

Rem
bbdoc: 
End Rem
Type b2ContactFilter

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
Type b2XForm
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
	
	Rem
	bbdoc: 
	End Rem
	Function CreateBodyDef:b2BodyDef()
		Return New b2BodyDef.Create()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:b2BodyDef()
		b2ObjectPtr = bmx_b2bodydef_create()
		Return Self
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
	bbdoc: Linear damping is use To reduce the linear velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive To the
	/// time Step when the damping parameter is large.
	End Rem
	Method SetLinearDamping(damping:Float)
	End Method
	
	'Method GetLinearDamping:Float()
	'End Method

	Rem
	bbdoc: Angular damping is use To reduce the angular velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive To the
	/// time Step when the damping parameter is large.
	End Rem
	Method SetAngularDamping(damping:Float)
	End Method
	
	'Method GetAngularDamping:Float()
	'End Method

	Rem
	bbdoc: Set this flag To False If this body should never fall asleep. Note that
	/// this increases CPU usage.
	End Rem
	Method SetAllowSleep(allow:Int)
	End Method
	
	'Method GetAllowSleep:Int()
	'End Method

	Rem
	bbdoc: Is this body initially sleeping?
	End Rem
	Method isSleeping:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetIsSleeping(sleeping:Int)
	End Method

	Rem
	bbdoc: Should this body be prevented from rotating? Useful For characters.
	End Rem
	Method SetFixedRotation(fixed:Int)
	End Method
	
	Method GetFixedRotation:Int()
	End Method

	Rem
	bbdoc: Is this a fast moving body that should be prevented from tunneling through
	/// other moving bodies? Note that all bodies are prevented from tunneling through
	/// static bodies.
	/// @warning You should use this flag sparingly since it increases processing time.
	End Rem
	Method SetIsBullet(bullet:Int)
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

End Type

Rem
bbdoc: 
End Rem
Type b2JointEdge
End Type

Rem
bbdoc: 
End Rem
Type b2MassData
End Type

Rem
bbdoc: 
End Rem
Type b2ShapeDef

	Field b2ObjectPtr:Byte Ptr
	Field userData:Object

	Method SetFriction(friction:Float)
		bmx_b2shapedef_setfriction(b2ObjectPtr, friction)
	End Method
	
	Method SetRestitution(restitution:Float)
		bmx_b2shapedef_setrestitution(b2ObjectPtr, restitution)
	End Method
	
	Method SetDensity(density:Float)
		bmx_b2shapedef_setdensity(b2ObjectPtr, density)
	End Method
	
	
End Type

Rem
bbdoc: Convex polygon.
about: Vertices must be in CCW order.
End Rem
Type b2PolygonDef Extends b2ShapeDef

	Rem
	bbdoc: 
	End Rem
	Function CreatePolygonDef:b2PolygonDef()
		Return New b2PolygonDef.Create()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:b2PolygonDef()
		b2ObjectPtr = bmx_b2polygondef_create()
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetAsBox(hx:Float, hy:Float)
		bmx_b2polygondef_setasbox(b2ObjectPtr, hx, hy)
	End Method

	Method Delete()
		If b2ObjectPtr Then
			bmx_b2polygondef_delete(b2ObjectPtr)
			b2ObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type b2CircleDef Extends b2ShapeDef

	Rem
	bbdoc: 
	End Rem
	Function CreateCircleDef:b2CircleDef()
		Return New b2CircleDef.Create()
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Create:b2CircleDef()
		b2ObjectPtr = bmx_b2circledef_create()
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetRadius(radius:Float)
		bmx_b2circledef_setradius(b2ObjectPtr, radius)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLocalPosition(pos:b2Vec2)
		bmx_b2circledef_setlocalposition(b2ObjectPtr, pos.b2ObjectPtr)
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

	Function CreateRevoluteJointDef:b2RevoluteJointDef()
		Return New b2RevoluteJointDef.Create()
	End Function
	
	Method Create:b2RevoluteJointDef()
		b2ObjectPtr = bmx_b2revolutejointdef_create()
		Return Self
	End Method
	
	Method Initialize(body1:b2Body, body2:b2Body, anchor:b2Vec2)
		bmx_b2revolutejointdef_initialize(b2ObjectPtr, body1.b2ObjectPtr, body2.b2ObjectPtr, anchor.b2ObjectPtr)
	End Method
	
	

End Type

