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

Rem
bbdoc: Chipmunk 2D Physics
End Rem
Module BaH.Chipmunk

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.01"
ModuleInfo "History: Adds body SetTorque()."
ModuleInfo "History: Adds joint Free()."
ModuleInfo "History: Fixes problem where Free'ing wasn't removing the objects from the space."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial release."

ModuleInfo "CC_OPTS: -std=gnu99"


Import BRL.Map

Import "common.bmx"


Rem
bbdoc: Initializes the physics engine.
End Rem
Function InitChipmunk()
	cpInitChipmunk()
End Function

Rem
bbdoc: Resets the shape id counter.
about: Chipmunk keeps a counter so that every new shape is given a unique hash value to be used in the spatial hash.
Because this affects the order in which the collisions are found and handled, you should reset the shape counter
every time you populate a space with new shapes. If you don't, there might be (very) slight differences in the simulation.
End Rem
Function ResetShapeIdCounter()
	cpResetShapeIdCounter()
End Function
	
	
Type CPObject

	Field cpObjectPtr:Byte Ptr
	Field parent:CPSpace
	
End Type

Rem
bbdoc: A rigid body holds the physical properties of an object. (mass, position, velocity, etc.)
about: It does not have a shape by itself. By attaching shapes to bodies, you can define the a body's shape.
You can attach many shapes to a single body to define a complex shape, or none if it doesn't require a shape.
End Rem
Type CPBody Extends CPObject

	Function _create:CPBody(cpObjectPtr:Byte Ptr)
		If cpObjectPtr Then
			Local this:CPBody = New CPBody
			this.cpObjectPtr = cpObjectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new body
	End Rem
	Method Create:CPBody(mass:Float, inertia:Float)
		cpObjectPtr = bmx_cpbody_create(Self, mass, inertia)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the body mass.
	End Rem
	Method GetMass:Float()
		Return bmx_cpbody_getmass(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Returns the body inertia.
	End Rem
	Method GetInertia:Float()
		Return bmx_cpbody_getinertia(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Gets the current angle for the body.
	End Rem
	Method GetAngle:Float()
		Return bmx_cpbody_getangle(cpObjectPtr)
	End Method

	Rem
	bbdoc: Sets the body angle
	End Rem
	Method SetAngle(angle:Float)
		bmx_cpbody_setangle(cpObjectPtr, angle)
	End Method
	
	Rem
	bbdoc: Returns the body angular velocity.
	End Rem
	Method GetAngularVelocity:Float()
		Return bmx_cpbody_getangularvelocity(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Sets the body angular velocity.
	End Rem
	Method SetAngularVelocity(av:Float)
		bmx_cpbody_setangularvelocity(cpObjectPtr, av)
	End Method
	
	Rem
	bbdoc: Returns the body torque.
	End Rem
	Method GetTorque:Float()
		Return bmx_cpbody_gettorque(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Sets the body torque.
	End Rem
	Method SetTorque(torque:Float)
		bmx_cpbody_settorque(cpObjectPtr, torque)
	End Method
	
	Rem
	bbdoc: Sets the body position.
	End Rem
	Method SetPosition(pos:CPVect)
		bmx_cpbody_setposition(cpObjectPtr, pos.vecPtr)
	End Method
	
	Rem
	bbdoc: Gets the body position
	End Rem
	Method GetPosition:CPVect()
		Return CPVect._create(bmx_cpbody_getposition(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetRot:CPVect()
		Return CPVect._create(bmx_cpbody_getrot(cpObjectPtr))
	End Method

	Rem
	bbdoc: Sets the body velocity.
	End Rem
	Method SetVelocity(velocity:CPVect)
		bmx_cpbody_setvelocity(cpObjectPtr, velocity.vecPtr)
	End Method
	
	Rem
	bbdoc: Returns the body velocity.
	End Rem
	Method GetVelocity:CPVect()
		Return CPVect._create(bmx_cpbody_getvelocity(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Updates the velocity of the body using Euler integration.
	about: You don't need to call this unless you are managing the object manually instead of adding it to a CPSpace.
	End Rem
	Method UpdateVelocity(gravity:CPVect, damping:Float, dt:Float)
		bmx_cpbody_updatevelocity(cpObjectPtr, gravity.vecPtr, damping, dt)
	End Method
	
	Rem
	bbdoc: Updates the position of the body using Euler integration.
	about: Like UpdateVelocity() you shouldn't normally need to call this yourself.
	End Rem
	Method UpdatePosition(dt:Float)
		cpBodyUpdatePosition(cpObjectPtr, dt)
	End Method
	
	Rem
	bbdoc: Zero both the forces and torques accumulated on body.
	End Rem
	Method ResetForces()
		cpBodyResetForces(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Apply (accumulate) the @force on body with @offset.
	about: Both @force and @offset should be in world coordinates.
	End Rem
	Method ApplyForce(force:CPVect, offset:CPVect)
		bmx_cpbody_applyforce(cpObjectPtr, force.vecPtr, offset.vecPtr)
	End Method
	
	Rem
	bbdoc: Convert body local to world coordinates.
	End Rem
	Method Local2World:CPVect(vec:CPVect)
		Return CPVect._create(bmx_cpbody_local2world(cpObjectPtr, vec.vecPtr))
	End Method
	
	Rem
	bbdoc: Convert world to body local coordinates
	End Rem
	Method World2Local:CPVect(vec:CPVect)
		Return CPVect._create(bmx_cpbody_world2local(cpObjectPtr, vec.vecPtr))
	End Method
	
	Rem
	bbdoc: Sets the body mass.
	End Rem
	Method SetMass(mass:Float)
		cpBodySetMass(cpObjectPtr, mass)
	End Method
	
	Rem
	bbdoc: Sets the body moment.
	End Rem
	Method SetMoment(moment:Float)
		cpBodySetMoment(cpObjectPtr, moment)
	End Method
	
	Rem
	bbdoc: Apply the @impulse to the body with @offset.
	about: Both @impulse and @offset should be in world coordinates.
	End Rem
	Method ApplyImpulse(impulse:CPVect, offset:CPVect)
		bmx_body_applyimpulse(cpObjectPtr, impulse.vecPtr, offset.vecPtr)
	End Method

	Rem
	bbdoc: Modify the velocity of an object so that it will slew.
	End Rem
	Method Slew(pos:CPVect, dt:Float)
		bmx_body_slew(cpObjectPtr, pos.vecPtr, dt)
	End Method
	
	Rem
	bbdoc: Frees the body.
	End Rem
	Method Free()
		If cpObjectPtr Then
			cpSpaceRemoveBody(parent.cpObjectPtr, cpObjectPtr)
			cpBodyFree(cpObjectPtr)
			cpObjectPtr = Null
		End If
	End Method
	
End Type


Type _CollisionPair

	Field abHash:Int
	
	Field data:Object
	Field func:Int(shapeA:CPShape, shapeB:CPShape, ..
			contacts:CPContact[], normalCoeficient:Float, data:Object)
	
	Method Compare:Int(obj:Object)
		Return abHash - _CollisionPair(obj).abHash
	End Method
	
End Type

Rem
bbdoc: Spaces are the basic simulation unit in Chipmunk.
about: You add bodies, shapes and joints to a space, and then update the space as a whole.
<p>Notes</p>
<ul>
<li>When removing objects from the space, make sure you remove any other objects that reference it. For instance,
when you remove a body, remove the joints and shapes attached to it.</li>
<li>The number of iterations, and the size of the time step determine the quality of the simulation. More iterations,
or smaller time steps increase the quality.</li>
</li>Because static shapes are only rehashed when you request it, it's possible to use a much higher count argument to
CPHash.ResizeStaticHash() than to cpHashResizeStaticHash(). Doing so will use more memory though.</li>
</ul>
End Rem
Type CPSpace Extends CPObject

	Field activeShapes:CPSpaceHash
	Field staticShapes:CPSpaceHash
	
	Field collisionPairs:TMap = New TMap

	Rem
	bbdoc: Creates a new CPSpace.
	End Rem
	Method Create:CPSpace()
		cpObjectPtr = bmx_cpspace_create(Self)
		Return Self
	End Method

	Rem
	bbdoc: Sets the amount of gravity applied to the system.
	End Rem
	Method SetGravity(vec:CPVect)
		bmx_cpspace_setgravity(cpObjectPtr, vec.vecPtr)
	End Method
	
	Rem
	bbdoc: Sets the amount of viscous damping applied to the system.
	End Rem
	Method SetDamping(damping:Float)
		bmx_cpspace_setdamping(cpObjectPtr, damping)
	End Method

	Rem
	bbdoc: Adds a static shape to the space.
	about: Shapes added as static are assumed not to move. Static shapes should be be attached to a rigid body with an
	infinite mass and moment of inertia. Also, don't add the rigid body used to the space, as that will cause it to
	fall under the effects of gravity.
	End Rem
	Method AddStaticShape(shape:CPShape)
		bmx_cpspace_addstaticshape(cpObjectPtr, shape.cpObjectPtr)
		shape.parent = Self
		shape.static = True
	End Method
	
	Rem
	bbdoc: Adds a body to the space.
	End Rem
	Method AddBody(body:CPBody)
		bmx_cpspace_addbody(cpObjectPtr, body.cpObjectPtr)
		body.parent = Self
	End Method
	
	Rem
	bbdoc: Adds a shape to the space.
	End Rem
	Method AddShape(shape:CPShape)
		bmx_cpspace_addshape(cpObjectPtr, shape.cpObjectPtr)
		shape.parent = Self
	End Method

	Rem
	bbdoc: Register @cpCollFunc to be called when a collision is found between a shapes with collision type fields that match @collTypeA and @collTypeB.
	about: @data is passed to the function as a parameter. The ordering of the collision types will match the ordering
	passed to the callback function.
	<p>
	Collision pair functions allow you to add callbacks for certain collision events. Each CPShape structure has a user
	definable collisionType field that is used to identify its type. For instance, you could define an enumeration
	of collision types such as bullets and players, and then register a collision pair function to reduce the players
	health when a collision is found between the two.
	</p>
	<p>
	Additionally, the return value of a collision pair function determines whether or not a collision will be processed.
	If the function returns False, the collision will be ignored. One use for this functionality is to allow a rock object
	to break a vase object. If the approximated energy of the collision is above a certain level, flag the vase to be
	removed from the space, apply an impulse to the rock to slow it down, and return False. After the DoStep() returns,
	remove the vase from the space.
	</p>
	<p>
	<b>WARNING</b>: It is not safe for collision pair functions to remove or free shapes or bodies from a space. Doing so will
	likely end in a segfault as an earlier collision may already be referencing the shape or body. You must wait until
	after the DoStep() method returns.
	</p>
	End Rem
	Method AddCollisionPairFunc(collTypeA:Int, collTypeB:Int, cpCollFunc:Int(shapeA:cpShape, shapeB:cpShape, ..
			contacts:CPContact[], normalCoeficient:Float, data:Object), data:Object = Null)
		Local collpair:_CollisionPair = New _CollisionPair
		collpair.abHash = bmx_CP_HASH_PAIR(collTypeA, collTypeB)
		
		collpair.data = data
		collpair.func = cpCollFunc
		
		' add it (for reference counting
		collisionPairs.insert(collpair, collpair)
		
		bmx_cpspace_addcollisionpairfunc(cpObjectPtr, collTypeA, collTypeB, _doCollision, collpair)
	End Method
	
	Function _doCollision:Int(shapeA:Byte Ptr, shapeB:Byte Ptr, contacts:Byte Ptr, numContacts:Int, normalCoeficient:Float, data:Object)
		If _CollisionPair(data) Then
			Local conts:CPContact[] = New CPContact[numContacts]
			bmx_cpcontact_fill(conts, contacts, numContacts)
			
			Return _CollisionPair(data).func(CPShape(cpfind(shapeA)), CPShape(cpfind(shapeB)), ..
				conts, normalCoeficient, _CollisionPair(data).data)
		End If
	End Function
	
	Rem
	bbdoc: Remove the function for the given collision type pair.
	about: The order of @collTypeA and @collTypeB must match the original order used with AddCollisionPairFunc().
	End Rem
	Method RemoveCollisionPairFunc(collTypeA:Int, collTypeB:Int)
		Local collpair:_CollisionPair = New _CollisionPair
		collpair.abHash = bmx_CP_HASH_PAIR(collTypeA, collTypeB)

		' remove any existing first..
		collisionPairs.Remove(collpair)
		
		bmx_cpspace_removecollisionpairfunc(cpObjectPtr, collTypeA, collTypeB)
	End Method

	Rem
	bbdoc: The default function is called when no collision pair function is specified.
	about: By default, the default function simply accepts all collisions. Passing Null for @cpCollFunc will reset the 
	default function back to the default. (You know what I mean.)
	<p>
	Passing Null for @cpCollFunc will reject collisions by default.
	</p>
	End Rem
	Method SetDefaultCollisionPairFunc(cpCollFunc(shapeA:cpShape, shapeB:cpShape, ..
			contacts:CPContact[], normalCoeficient:Float, data:Object), data:Object = Null)
			
		Local collpair:_CollisionPair = New _CollisionPair
		collpair.abHash = bmx_CP_HASH_PAIR(-9999, -9999)
		
		If Not cpCollFunc Then
			collisionPairs.remove(collpair)
			bmx_cpspace_setdefaultcollisionpairfunc(cpObjectPtr, Null, Null)
		Else
		
			collpair.data = data
			collpair.func = cpCollFunc
			
			' add it (for reference counting
			collisionPairs.insert(collpair, collpair)
			
			bmx_cpspace_setdefaultcollisionpairfunc(cpObjectPtr, _doCollision, collpair)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddJoint(joint:CPJoint)
		bmx_cpspace_addjoint(cpObjectPtr, joint.cpObjectPtr)
		joint.parent = Self
	End Method

	Rem
	bbdoc: Resizes the static hash table.
	about: The spatial hashes used by Chipmunk's collision detection are fairly size sensitive. @dim is the size of the
	hash cells. Setting @dim to the average objects size is likely to give the best performance.
	<p>
	@count is the suggested minimum number of cells in the hash table. Bigger is better, but only to a point.
	Setting @count to ~10x the number of objects in the hash is probably a good starting point.
	</p>
	<p>
	By default, @dim is 100.0, and @count is 1000.
	</p>
	End Rem
	Method ResizeStaticHash(dim:Float, count:Int)
		cpSpaceResizeStaticHash(cpObjectPtr, dim, count)
	End Method

	Rem
	bbdoc: Resizes the active hash table.
	about: The spatial hashes used by Chipmunk's collision detection are fairly size sensitive. @dim is the size of the
	hash cells. Setting @dim to the average objects size is likely to give the best performance.
	<p>
	@count is the suggested minimum number of cells in the hash table. Bigger is better, but only to a point.
	Setting @count to ~10x the number of objects in the hash is probably a good starting point.
	</p>
	<p>
	By default, @dim is 100.0, and @count is 1000.
	</p>
	End Rem
	Method ResizeActiveHash(dim:Float, count:Int)
		cpSpaceResizeActiveHash(cpObjectPtr, dim, count)
	End Method

	Rem
	bbdoc: Update the space for the given time step.
	about: Using a fixed time step is highly recommended. Doing so will increase the efficiency of the contact
	persistence, requiring an order of magnitude fewer iterations to resolve the collisions in the usual case.
	End Rem
	Method DoStep(dt:Float)
		cpSpaceStep(cpObjectPtr, dt)
	End Method
	
	Rem
	bbdoc: Sets the number of iterations to use when solving constraints (collisions and joints).
	about: Defaults to 10.
	End Rem
	Method SetIterations(num:Int)
		bmx_cpspace_setiterations(cpObjectPtr, num)
	End Method
	
	Rem
	bbdoc: Rehashes the shapes in the static spatial hash.
	about: You only need to call this if you move one of the static shapes.
	End Rem
	Method RehashStatic()
		cpSpaceRehashStatic(cpObjectPtr)
	End Method

	Rem
	bbdoc: Starts an iteration over every body of this space, calling @callback for each.
	End Rem
	Method EachBody(callback(obj:Object, data:Object), data:Object = Null)
		
		Local cb:TFuncCallback = New TFuncCallback
		cb.callback = callback
		cb.data = data
		
		cpSpaceEachBody(cpObjectPtr, hashCallback, cb)
		
	End Method
	
	Function hashCallback(obj:Byte Ptr, callbackData:Object)
		If TFuncCallback(callbackData) Then
			TFuncCallback(callbackData).callback(cpfind(obj), TFuncCallback(callbackData).data)
		End If
	End Function

	Rem
	bbdoc: Returns the hash of active shapes.
	End Rem
	Method GetActiveShapes:CPSpaceHash()

		If Not activeShapes Then
			Local p:Byte Ptr = bmx_cpspace_getactiveshapes(cpObjectPtr)
			If p Then
				activeShapes = CPSpaceHash(cpfind(p))
				If Not activeShapes Then
					activeShapes = New CPSpaceHash.Bind(p)
				End If
			End If
		End If
		
		Return activeShapes
	End Method
	
	Rem
	bbdoc: Returns the hash of static shapes.
	End Rem
	Method GetStaticShapes:CPSpaceHash()

		If Not staticShapes Then
			Local p:Byte Ptr = bmx_cpspace_getstaticshapes(cpObjectPtr)
			If p Then
				staticShapes = CPSpaceHash(cpfind(p))
				If Not staticShapes Then
					staticShapes = New CPSpaceHash.Bind(p)
				End If
			End If
		End If
		
		Return staticShapes
	End Method
	
	Rem
	bbdoc: Frees the CPSpace and all dependencies.
	End Rem
	Method Free()
		If cpObjectPtr Then
			cpSpaceFree(cpObjectPtr)
			cpObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: The contact point of a collision between two shapes.
End Rem
Type CPContact

	Field contactPtr:Byte Ptr
	
	Function _create:CPContact(contactPtr:Byte Ptr)
		If contactPtr Then
			Local this:CPContact = New CPContact
			this.contactPtr = contactPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the position of the collision.
	End Rem
	Method GetPosition:CPVect()
		Return CPVect._create(bmx_cpcontact_getposition(contactPtr))
	End Method
	
	Rem
	bbdoc: Returns the normal of the collision.
	End Rem
	Method GetNormal:CPVect()
		Return CPVect._create(bmx_cpcontact_getnormal(contactPtr))
	End Method
	
	Rem
	bbdoc: Returns the penetration distance of the collision.
	End Rem
	Method GetDistance:Float()
		Return bmx_cpcontact_getdistance(contactPtr)
	End Method
	
	Rem
	bbdoc: Returns the normal component of the accumulated (final) impulse applied to resolve the collision.
	about: This value is not valid until after the call to DoStep() returns.
	End Rem
	Method GetNormalAccumulatedImpulse:Float()
		Return bmx_cpcontact_getjnacc(contactPtr)
	End Method
	
	Rem
	bbdoc: Returns the tangential component of the accumulated (final) impulse applied to resolve the collision.
	about: This value is not valid until after the call to DoStep() returns.
	End Rem
	Method GetTangentAccumulatedImpulse:Float()
		Return bmx_cpcontact_getjtacc(contactPtr)
	End Method
	
	Function _setContact(contacts:CPContact[], index:Int, contactPtr:Byte Ptr)
		contacts[index] = _create(contactPtr)
	End Function
	
End Type

Rem
bbdoc: A 2D vector
End Rem
Type CPVect
	Field vecPtr:Byte Ptr
	
	Field x:Float
	Field y:Float
	
	Rem
	bbdoc: Creates a new CPVect.
	End Rem
	Method Create:CPVect(_x:Float, _y:Float)
		vecPtr = bmx_cpvect_create(_x, _y)
		x = _x
		y = _y
		Return Self
	End Method
	
	Function _create:CPVect(vecPtr:Byte Ptr)
		If vecPtr Then
			Local this:CPVect = New CPVect
			this.vecPtr = vecPtr
			bmx_cpvect_getxy(vecPtr, Varptr this.x, Varptr this.y)
			Return this
		End If
	End Function
	
	Rem
	bbdoc: X
	End Rem
	Method GetX:Float()
		Return x
	End Method
	
	Rem
	bbdoc: Y
	End Rem
	Method GetY:Float()
		Return y
	End Method
	
	Method Delete()
		If vecPtr Then
			bmx_cpvect_delete(vecPtr)
			vecPtr = Null
		End If
	End Method
	
	Rem
	bbdoc: Adds @vec to this vector, returning the new combined vector.
	End Rem
	Method Add:CPVect(vec:CPVect)
		Return _create(bmx_cpvect_add(vecPtr, vec.vecPtr))
	End Method

	Rem
	bbdoc: Subtracts @vec from this vector, returning the new combined vector.
	End Rem
	Method Sub:CPVect(vec:CPVect)
		Return _create(bmx_cpvect_sub(vecPtr, vec.vecPtr))
	End Method
	
	Rem
	bbdoc: Uses complex multiplication to rotate (and scale) this vector by @vec.
	End Rem
	Method Rotate:CPVect(vec:CPVect)
		Return _create(bmx_cpvect_rotate(vecPtr, vec.vecPtr))
	End Method

	Rem
	bbdoc: Negate a vector.
	End Rem
	Method Negate:CPVect()
		Return _create(bmx_cpvect_negate(vecPtr))
	End Method
	
	Rem
	bbdoc: Scalar multiplication.
	End Rem
	Method Mult:CPVect(scalar:Float)
		Return _create(bmx_cpvect_mult(vecPtr, scalar))
	End Method
	
	Rem
	bbdoc: Vector dot product.
	End Rem
	Method Dot:Float(vec:CPVect)
		Return bmx_cpvect_dot(vecPtr, vec.vecPtr)
	End Method
	
	Rem
	bbdoc: 2D vector cross product analog.
	about: The cross product of 2D vectors exists only in the z component, so only that value is returned.
	End Rem
	Method Cross:Float(vec:CPVect)
		Return bmx_cpvect_cross(vecPtr, vec.vecPtr)
	End Method
	
	Rem
	bbdoc: Returns the perpendicular vector. (90 degree rotation)
	End Rem
	Method Perp:CPVect()
		Return _create(bmx_cpvect_perp(vecPtr))
	End Method
	
	Rem
	bbdoc: Returns the vector projection of the vector onto @vec.
	End Rem
	Method Project:CPVect(vec:CPVect)
		Return _create(bmx_cpvect_project(vecPtr, vec.vecPtr))
	End Method
	
	Rem
	bbdoc: Inverse of Rotate().
	End Rem
	Method UnRotate:CPVect(vec:CPVect)
		Return _create(bmx_cpvect_unrotate(vecPtr, vec.vecPtr))
	End Method
	
	Rem
	bbdoc: Returns the length of the vector.
	End Rem
	Method Length:Float()
		Return bmx_cpvect_length(vecPtr)
	End Method
	
	Rem
	bbdoc: Returns the squared length of the vector.
	about: Faster than Length() when you only need to compare lengths.
	End Rem
	Method LengthSq:Float()
		Return bmx_cpvect_lengthsq(vecPtr)
	End Method
	
	Rem
	bbdoc: Returns a normalized copy of the vector.
	End Rem
	Method Normalize:CPVect()
		Return _create(bmx_cpvect_normalize(vecPtr))
	End Method
	
	Rem
	bbdoc: Returns the angular direction the vector is pointing in (in degrees).
	End Rem
	Method ToAngle:Float()
		Return bmx_cpvect_toangle(vecPtr)
	End Method

	Function _getVectForIndex:Byte Ptr(verts:CPVect[], index:Int)
		Return verts[index].vecPtr
	End Function

End Type

Rem
bbdoc: A simple bounding box.
about: Stored as left, bottom, right, top values.
End Rem
Type CPBB

	Field bbPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new bounding box.
	End Rem
	Method Create:CPBB(l:Float, b:Float, r:Float, t:Float)
		bbPtr = bmx_cpbb_create(l, b, r, t)
		Return Self
	End Method
	
	Function _create:CPBB(bbPtr:Byte Ptr)
		If bbPtr Then
			Local this:CPBB = New CPBB
			this.bbPtr = bbPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns True if the bounding boxes intersect.
	End Rem
	Method Intersects:Int(other:CPBB)
		Return bmx_cpbb_intersects(bbPtr, other.bbPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the bounding box completely contains @other.
	End Rem
	Method ContainsBB:Int(other:CPBB)
		Return bmx_cpbb_containsbb(bbPtr, other.bbPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the bounding box contains @v.
	End Rem
	Method ContainsVect:Int(v:CPVect)
		Return bmx_cpbb_containsvect(bbPtr, v.vecPtr)
	End Method
	
	Rem
	bbdoc: Returns a copy of @v clamped to the bounding box.
	End Rem
	Method ClampVect:CPVect(v:CPVect)
		Return CPVect._create(bmx_cpbb_clampvect(bbPtr, v.vecPtr))
	End Method
	
	Rem
	bbdoc: Returns a copy of @v wrapped to the bounding box.
	End Rem
	Method WrapVect:CPVect(v:CPVect)
		Return CPVect._create(bmx_cpbb_wrapvect(bbPtr, v.vecPtr))
	End Method

	Method Delete()
		If bbPtr Then
			bmx_cpbb_delete(bbPtr)
			bbPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A collision shape.
about: By attaching shapes to bodies, you can define the a body's shape. You can attach many shapes to a single body 
to define a complex shape, or none if it doesn't require a shape.
<p>
There is often confusion between rigid bodies and their collision shapes in Chipmunk and how they relate to sprites.
A sprite would be a visual representation of an object, the sprite is drawn at the position of the rigid body.
The collision shape would be the material representation of the object, and how it should collide with other objects.
A sprite and collision shape have little to do with one another other than you probably want the collision shape to
match the sprite's shape.
</p>
End Rem
Type CPShape Extends CPObject

	Field static:Int = False

	Rem
	bbdoc: Sets the elasticity of the shape.
	about: A value of 0.0 gives no bounce, while a value of 1.0 will give a "perfect" bounce. However due to inaccuracies
	in the simulation using 1.0 or greater is not recommended however.
	<p>
	The amount of elasticity applied during a collision is determined by multiplying the elasticity of both shapes together.
	</p>
	End Rem
	Method SetElasticity(e:Float)
		bmx_cpshape_setelasticity(cpObjectPtr, e)
	End Method
	
	Rem
	bbdoc: Returns the shape elasticity.
	End Rem
	Method GetElasticity:Float()
		Return bmx_cpshape_getelasticity(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Sets the friction coefficient for the shape.
	about: Chipmunk uses the Coulomb friction model, a value of 0.0 is frictionless.
	<a href="http://www.roymech.co.uk/Useful_Tables/Tribology/co_of_frict.htm">Tables of friction coefficients.</a>
	<p>
	The amount of friction applied during a collision is determined by multiplying the friction of both shapes together.
	</p>
	End Rem
	Method SetFriction(u:Float)
		bmx_cpshape_setfriction(cpObjectPtr, u)
	End Method
	
	Rem
	bbdoc: Returns the shape friction.
	End Rem
	Method GetFriction:Float()
		Return bmx_cpshape_getfriction(cpObjectPtr)
	End Method

	Rem
	bbdoc: Sets the user-definable type field.
	End Rem
	Method SetCollisionType(kind:Int)
		bmx_cpshape_setcollisiontype(cpObjectPtr, kind)
	End Method

	Rem
	bbdoc: Sets the group to which this shape belongs.
	about: Shapes in the same non-zero group do not generate collisions. Useful when creating an object out of many
	shapes that you don't want to self collide. Defaults to 0.
	End Rem
	Method SetGroup(group:Int)
		bmx_cpshape_setgroup(cpObjectPtr, group)
	End Method

	Rem
	bbdoc: Sets the layers this shape occupies.
	about: Shapes only collide if they are in the same bit-planes. i.e. (a.layers & b.layers) != 0.
	<p>
	By default, a shape occupies all 32 bit-planes.
	</p>
	End Rem
	Method SetLayers(layers:Int)
		bmx_cpshape_setlayers(cpObjectPtr, layers)
	End Method
	
	Rem
	bbdoc: Sets the surface velocity of the object.
	about: Useful for creating conveyor belts or players that move around. This value is only used when calculating
	friction, not the collision.
	End Rem
	Method SetSurfaceVelocity(velocity:CPVect)
		bmx_cpshape_setsurfacevelocity(cpObjectPtr, velocity.vecPtr)
	End Method
	
	Rem
	bbdoc: Returns the surface velocity of the object.
	End Rem
	Method GetSurfaceVelocity:CPVect()
		Return CPVect._create(bmx_cpshape_getsurfacevelocity(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Returns the rigid body the shape is attached to.
	End Rem
	Method GetBody:CPBody()
		Local p:Byte Ptr = bmx_cpshape_getbody(cpObjectPtr)
		If p Then
			Local body:CPBody = CPBody(cpfind(p))
			If Not body Then
				Return CPBody._create(p)
			End If
			Return body
		End If
	End Method
	
	Rem
	bbdoc: Updates and returns the bounding box of the shape.
	End Rem
	Method CacheBB:CPBB()
		Return CPBB._create(bmx_shape_cachebb(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Frees the shape
	End Rem
	Method Free()
		If cpObjectPtr Then
			If static Then
				cpSpaceRemoveStaticShape(parent.cpObjectPtr, cpObjectPtr)
			Else
				cpSpaceRemoveShape(parent.cpObjectPtr, cpObjectPtr)
			End If
			cpShapeFree(cpObjectPtr)
			cpObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A line segment shape.
about: Meant mainly as a static shape. They can be attached to moving bodies, but they don't generate collisions with
other line segments.
End Rem
Type CPSegmentShape Extends CPShape

	Rem
	bbdoc: Creates a new line segment shape.
	about: @body is the body to attach the segment to, @a and @b are the endpoints, and @radius is the thickness of the
	segment.
	End Rem
	Method Create:CPSegmentShape(body:CPBody, a:CPVect, b:CPVect, radius:Float)
		cpObjectPtr = bmx_cpsegmentshape_create(Self, body.cpObjectPtr, a.vecPtr, b.vecPtr, radius)
		Return Self
	End Method

	Rem
	bbdoc: Returns the position of endpoint A.
	End Rem
	Method GetEndPointA:CPVect()
		Return CPVect._create(bmx_cpsegmentshape_getendpointa(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Returns the position of endpoint B.
	End Rem
	Method GetEndPointB:CPVect()
		Return CPVect._create(bmx_cpsegmentshape_getendpointb(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Returns the segment normal.
	End Rem
	Method GetNormal:CPVect()
		Return CPVect._create(bmx_cpsegmentshape_getnormal(cpObjectPtr))
	End Method
	
End Type

Rem
bbdoc: A convex polygon shape.
about: The slowest, but most flexible collision shape.
End Rem
Type CPPolyShape Extends CPShape

	Rem
	bbdoc: Creates a new convex polygon shape.
	about: @body is the body to attach the poly to, @verts is an array of CPVect's defining a convex hull with a
	counterclockwise winding, @offset is the offset from the body's center of gravity in body local coordinates.
	End Rem
	Method Create:CPPolyShape(body:CPBody, verts:CPVect[], offset:CPVect)
		cpObjectPtr = bmx_cppolyshape_create(Self, body.cpObjectPtr, verts, verts.length, offset.vecPtr)
		Return Self
	End Method

	Rem
	bbdoc: Returns the vertices as an array of floats.
	End Rem
	Method GetVertsAsCoords:Float[]()
		Return bmx_cppolyshape_getvertsascoords(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Returns the array of vertices.
	End Rem
	Method GetVerts:CPVect[]()
		Local verts:CPVect[] = New CPVect[bmx_cppolyshape_numverts(cpObjectPtr)]
		bmx_cppolyshape_getverts(cpObjectPtr, verts)
		Return verts
	End Method
	
	Function _setVert(verts:CPVect[], index:Int, vec:Byte Ptr)
		verts[index] = CPVect._create(vec)
	End Function
	
End Type

Rem
bbdoc: A circle shape.
about: The fastest collision shape. They also roll smoothly.
End Rem
Type CPCircleShape Extends CPShape

	Rem
	bbdoc: Creates a new circle shape.
	about: @body is the body attach the circle to, @offset is the offset from the body's center of gravity in body local coordinates.
	End Rem
	Method Create:CPCircleShape(body:CPBody, radius:Float, offset:CPVect)
		cpObjectPtr = bmx_cpcircleshape_create(Self, body.cpObjectPtr, radius, offset.vecPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Returns the shape center, in body space coordinates.
	End Rem
	Method GetCenter:CPVect()
		Return CPVect._create(bmx_cpcircleshape_getcenter(cpObjectPtr))
	End Method

	Rem
	bbdoc: Returns the transformed shape center, in world coordinates.
	End Rem
	Method GetTransformedCenter:CPVect()
		Return CPVect._create(bmx_cpcircleshape_gettransformedcenter(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Returns the shape radius.
	End Rem
	Method GetRadius:Float()
		Return bmx_cpcircleshape_getradius(cpObjectPtr)
	End Method
	
End Type

Type TFuncCallback

	Field callback(obj:Object, data:Object)
	Field data:Object
	
End Type

Rem
bbdoc: The spatial hash is Chipmunk's default (and currently only) spatial index type.
about: Based on a chained hash table.
End Rem
Type CPSpaceHash Extends cpObject

	Function _create:CPSpaceHash(cpObjectPtr:Byte Ptr)
		If cpObjectPtr Then
			Local this:CPSpaceHash = New CPSpaceHash
			this.cpObjectPtr = cpObjectPtr
			Return this
		End If
	End Function

	Method Bind:CPSpaceHash(p:Byte Ptr)
		cpObjectPtr = p
		cpbind(p, Self)
		Return Self
	End Method

	Rem
	bbdoc: Iterate over the objects in the hash, calling @callback for each.
	about: @data is some optional user data that will be passed to @callback.
	End Rem
	Method ForEach(callback(obj:Object, data:Object), data:Object = Null)
		
		Local cb:TFuncCallback = New TFuncCallback
		cb.callback = callback
		cb.data = data
		
		cpSpaceHashEach(cpObjectPtr, hashCallback, cb)
		
	End Method
	
	Function hashCallback(obj:Byte Ptr, callbackData:Object)
		If TFuncCallback(callbackData) Then
			TFuncCallback(callbackData).callback(cpfind(obj), TFuncCallback(callbackData).data)
		End If
	End Function

End Type


Rem
bbdoc: Base type for joints.
End Rem
Type CPJoint Extends CPObject

	Rem
	bbdoc: Body A
	End Rem
	Method GetBodyA:CPBody()
		Return CPBody(cpfind(bmx_cpjoint_getbodya(cpObjectPtr)))
	End Method
	
	Rem
	bbdoc: Body B
	End Rem
	Method GetBodyB:CPBody()
		Return CPBody(cpfind(bmx_cpjoint_getbodyb(cpObjectPtr)))
	End Method
	
	Method Free()
		If cpObjectPtr Then
			cpSpaceRemoveJoint(parent.cpObjectPtr, cpObjectPtr)
			cpJointFree(cpObjectPtr)
			cpObjectPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Connects two rigid bodies with a solid pin or rod.
about: It keeps the anchor points at a set distance from one another.
End Rem
Type CPPinJoint Extends CPJoint

	Rem
	bbdoc: Creates a new pin joint.
	End Rem
	Method Create:CPPinJoint(bodyA:CPBody, bodyB:CPBody, anchor1:CPVect, anchor2:CPVect)
		cpObjectPtr = bmx_cppinjoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, anchor1.vecPtr, anchor2.vecPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: Anchor point 1
	End Rem
	Method GetAnchor1:CPVect()
		Return CPVect._create(bmx_cppinjoint_getanchor1(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Anchor point 2
	End Rem
	Method GetAnchor2:CPVect()
		Return CPVect._create(bmx_cppinjoint_getanchor2(cpObjectPtr))
	End Method	

End Type

Rem
bbdoc: Like pin joints, but have a minimum and maximum distance.
about: A chain could be modeled using this joint. It keeps the anchor points from getting to far apart,
but will allow them to get closer together.
End Rem
Type CPSlideJoint Extends CPJoint

	Rem
	bbdoc: Creates a new slide joint.
	End Rem
	Method Create:CPSlideJoint(bodyA:CPBody, bodyB:CPBody, anchor1:CPVect, anchor2:CPVect, minDist:Float, maxDist:Float)
		cpObjectPtr = bmx_cpslidejoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, anchor1.vecPtr, anchor2.vecPtr, minDist, maxDist)
		Return Self
	End Method
	
	Rem
	bbdoc: Anchor point 1
	End Rem
	Method GetAnchor1:CPVect()
		Return CPVect._create(bmx_cpslidejoint_getanchor1(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Anchor point 2
	End Rem
	Method GetAnchor2:CPVect()
		Return CPVect._create(bmx_cpslidejoint_getanchor2(cpObjectPtr))
	End Method

	Rem
	bbdoc: Minimum allowed distance of the anchor points.
	End Rem
	Method GetMinDist:Float()
		Return bmx_cpslidejoint_getmindist(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: Maximum allowed distance of the anchor points.
	End Rem
	Method GetMaxDist:Float()
		Return bmx_cpslidejoint_getmaxdist(cpObjectPtr)
	End Method
	
End Type

Rem
bbdoc: Allows two objects to pivot about a single point.
End Rem
Type CPPivotJoint Extends CPJoint

	Rem
	bbdoc: Creates a new pivot joint.
	about: @bodyA and @bodyB are the two bodies to connect, and @pivot is the point in world coordinates of the pivot.
	Because the pivot location is given in world coordinates, you must have the bodies moved into the correct positions
	already.
	End Rem
	Method Create:CPPivotJoint(bodyA:CPBody, bodyB:CPBody, pivot:CPVect)
		cpObjectPtr = bmx_cppivotjoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, pivot.vecPtr)
		Return Self
	End Method

	Rem
	bbdoc: Anchor point 1
	End Rem
	Method GetAnchor1:CPVect()
		Return CPVect._create(bmx_cppivotjoint_getanchor1(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Anchor point 2
	End Rem
	Method GetAnchor2:CPVect()
		Return CPVect._create(bmx_cppivotjoint_getanchor2(cpObjectPtr))
	End Method	
	
End Type

Rem
bbdoc: Attaches a point on one body to a groove on the other.
about: Think of it as a sliding pivot joint.
End Rem
Type CPGrooveJoint Extends CPJoint

	Rem
	bbdoc: Creates a new groove joint.
	about: The groove goes from @grooveA to @grooveB on @bodyA, and the pivot is attached to @anchor on @bodyB.
	All coordinates are body local.
	End Rem
	Method Create:CPGrooveJoint(bodyA:CPBody, bodyB:CPBody, grooveA:CPVect, grooveB:CPVect, anchor:CPVect)
		cpObjectPtr = bmx_cpgroovejoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, grooveA.vecPtr, grooveB.vecPtr, anchor.vecPtr)
		Return Self
	End Method

	Rem
	bbdoc: Groove A location.
	End Rem
	Method GetGrooveA:CPVect()
		Return CPVect._create(bmx_cpgroovejoint_getgroovea(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: Groove B location.
	End Rem
	Method GetGrooveB:CPVect()
		Return CPVect._create(bmx_cpgroovejoint_getgrooveb(cpObjectPtr))
	End Method	

	Rem
	bbdoc: Anchor point.
	End Rem
	Method GetAnchor:CPVect()
		Return CPVect._create(bmx_cpgroovejoint_getanchor(cpObjectPtr))
	End Method	

End Type

Rem
bbdoc: Convenience function for the creation of a CPVect of @x, @y.
End Rem
Function Vec2:CPVect(x:Float, y:Float)
	Return New CPVect.Create(x, y)
End Function

Rem
bbdoc: A standard 0,0 vector.
End Rem
Global CPVZero:CPVect = CPVect._create(bmx_cpvect_cpvzero())

Extern
	Function bmx_momentforpoly:Float(m:Float, verts:CPVect[], count:Int, offset:Byte Ptr)
	Function bmx_cppolyshape_create:Byte Ptr(handle:Object, body:Byte Ptr, verts:CPVect[], count:Int, offset:Byte Ptr)
	Function bmx_cppolyshape_getverts(handle:Byte Ptr, verts:CPVect[])
	Function bmx_cpcontact_fill(conts:CPContact[], contacts:Byte Ptr, numContacts:Int)

End Extern

Rem
bbdoc: Calculates the moment of inertia for the polygons of mass @m.
End Rem
Function MomentForPoly:Float(m:Float, verts:CPVect[], offset:CPVect)
	Return bmx_momentforpoly(m, verts, verts.length, offset.vecPtr)
End Function

Rem
bbdoc: Calculates the moment of inertia for the circle of mass @m.
End Rem
Function MomentForCircle:Float(m:Float, r1:Float, r2:Float, offset:CPVect)
	Return cpMomentForCircle(m, r1, r2, offset.vecPtr)
End Function

Rem
bbdoc: Apply a spring force between bodies @a and @b at anchors @anchor1 and @anchor2 respectively.
about: @k is the spring constant (force/distance), @rlen is the rest length of the spring, @dmp is the damping
constant (force/velocity), and @dt is the time step to apply the force over.
End Rem
Function DampedSpring(a:CPBody, b:CPBody, anchor1:CPVect, anchor2:CPVect, rlen:Float, k:Float, dmp:Float, dt:Float)
	bmx_cpdampedspring(a.cpObjectPtr, b.cpObjectPtr, anchor1.vecPtr, anchor2.vecPtr, rlen, k, dmp, dt)
End Function

