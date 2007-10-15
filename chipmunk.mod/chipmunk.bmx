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

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.00"
ModuleInfo "History: "

ModuleInfo "CC_OPTS: -std=gnu99"


Import "common.bmx"


Rem
bbdoc: 
End Rem
Function InitChipmunk()
	cpInitChipmunk()
End Function

Rem
bbdoc: 
End Rem
Function ResetShapeIdCounter()
	cpResetShapeIdCounter()
End Function
	
	
Type CPObject

	Field cpObjectPtr:Byte Ptr

End Type

Rem
bbdoc: 
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
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method GetAngle:Float()
		Return bmx_cpbody_getangle(cpObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetAngle(angle:Float)
		bmx_cpbody_setangle(cpObjectPtr, angle)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAngularVelocity:Float()
		Return bmx_cpbody_getangularvelocity(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAngularVelocity(av:Float)
		bmx_cpbody_setangularvelocity(cpObjectPtr, av)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetTorque:Float()
		Return bmx_cpbody_gettorque(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPosition(pos:CPVect)
		bmx_cpbody_setposition(cpObjectPtr, pos.vecPtr)
	End Method
	
	Rem
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method SetVelocity(velocity:CPVect)
		bmx_cpbody_setvelocity(cpObjectPtr, velocity.vecPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method UpdatePosition(dt:Float)
		cpBodyUpdatePosition(cpObjectPtr, dt)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type CPSpace Extends CPObject

	Field activeShapes:CPSpaceHash
	Field staticShapes:CPSpaceHash

	Method Create:CPSpace()
		cpObjectPtr = bmx_cpspace_create(Self)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetGravity(vec:CPVect)
		bmx_cpspace_setgravity(cpObjectPtr, vec.vecPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddStaticShape(shape:CPShape)
		bmx_cpspace_addstaticshape(cpObjectPtr, shape.cpObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddBody(body:CPBody)
		bmx_cpspace_addbody(cpObjectPtr, body.cpObjectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method AddShape(shape:CPShape)
		bmx_cpspace_addshape(cpObjectPtr, shape.cpObjectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddCollisionPairFunc(collTypeA:Int, collTypeB:Int, cpCollFunc(shapeA:cpShape, shapeB:cpShape, ..
			contacts:CPContact[], normalCoeficient:Float, data:Object))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ResizeStaticHash(dim:Float, count:Int)
		cpSpaceResizeStaticHash(cpObjectPtr, dim, count)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ResizeActiveHash(dim:Float, count:Int)
		cpSpaceResizeActiveHash(cpObjectPtr, dim, count)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method DoStep(dt:Float)
		cpSpaceStep(cpObjectPtr, dt)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetIterations(num:Int)
		bmx_cpspace_setiterations(cpObjectPtr, num)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method RehashStatic()
		cpSpaceRehashStatic(cpObjectPtr)
	End Method

	Rem
	bbdoc: 
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
	bbdoc: 
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
	bbdoc: 
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
	
End Type

Rem
bbdoc: 
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

	

End Type

Rem
bbdoc: A 2D vector
End Rem
Type CPVect
	Field vecPtr:Byte Ptr
	
	Field x:Float
	Field y:Float
	
	Rem
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method GetX:Float()
		Return x
	End Method
	
	Rem
	bbdoc: 
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
bbdoc: 
End Rem
Type CPShape Extends CPObject

	Rem
	bbdoc: 
	End Rem
	Method SetElasticity(e:Float)
		bmx_cpshape_setelasticity(cpObjectPtr, e)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetFriction(u:Float)
		bmx_cpshape_setfriction(cpObjectPtr, u)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetCollisionType(kind:Int)
		bmx_cpshape_setcollisiontype(cpObjectPtr, kind)
	End Method
	
	Rem
	bbdoc: 
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
	
End Type

Rem
bbdoc: 
End Rem
Type CPSegmentShape Extends CPShape

	Rem
	bbdoc: 
	End Rem
	Method Create:CPSegmentShape(body:CPBody, a:CPVect, b:CPVect, radius:Float)
		cpObjectPtr = bmx_cpsegmentshape_create(Self, body.cpObjectPtr, a.vecPtr, b.vecPtr, radius)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetEndPointA:CPVect()
		Return CPVect._create(bmx_cpsegmentshape_getendpointa(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetEndPointB:CPVect()
		Return CPVect._create(bmx_cpsegmentshape_getendpointb(cpObjectPtr))
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type CPPolyShape Extends CPShape

	Rem
	bbdoc: 
	End Rem
	Method Create:CPPolyShape(body:CPBody, verts:CPVect[], offset:CPVect)
		cpObjectPtr = bmx_cppolyshape_create(Self, body.cpObjectPtr, verts, verts.length, offset.vecPtr)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetVertsAsCoords:Float[]()
		Return bmx_cppolyshape_getvertsascoords(cpObjectPtr)
	End Method
	
	Rem
	bbdoc: 
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
bbdoc: 
End Rem
Type CPCircleShape Extends CPShape

	Rem
	bbdoc: 
	End Rem
	Method Create:CPCircleShape(body:CPBody, radius:Float, offset:CPVect)
		cpObjectPtr = bmx_cpcircleshape_create(Self, body.cpObjectPtr, radius, offset.vecPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetCenter:CPVect()
		Return CPVect._create(bmx_cpcircleshape_getcenter(cpObjectPtr))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetTransformedCenter:CPVect()
		Return CPVect._create(bmx_cpcircleshape_gettransformedcenter(cpObjectPtr))
	End Method
	
	Rem
	bbdoc: 
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
bbdoc: 
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
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method GetBodyA:CPBody()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetBodyB:CPBody()
	End Method
	
End Type

Rem
bbdoc: Connects two rigid bodies with a solid pin or rod.
about: It keeps the anchor points at a set distance from one another.
End Rem
Type CPPinJoint Extends CPJoint

	Rem
	bbdoc: 
	End Rem
	Method Create:CPPinJoint(bodyA:CPBody, bodyB:CPBody, anchor1:CPVect, anchor2:CPVect)
		cpObjectPtr = bmx_cppinjoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, anchor1.vecPtr, anchor2.vecPtr)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAnchor1:CPVect()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAnchor2:CPVect()
	End Method
	
	
End Type

Rem
bbdoc: Like pin joints, but have a minimum and maximum distance.
about: A chain could be modeled using this joint. It keeps the anchor points from getting to far apart,
but will allow them to get closer together.
End Rem
Type CPSlideJoint Extends CPJoint

	Rem
	bbdoc: 
	End Rem
	Method Create:CPSlideJoint(bodyA:CPBody, bodyB:CPBody, anchor1:CPVect, anchor2:CPVect, minDist:Float, maxDist:Float)
		cpObjectPtr = bmx_cpslidejoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, anchor1.vecPtr, anchor2.vecPtr, minDist, maxDist)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAnchor1:CPVect()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetAnchor2:CPVect()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetMinDist:Float()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMaxDist:Float()
	End Method
	
End Type

Rem
bbdoc: Allows two objects to pivot about a single point.
End Rem
Type CPPivotJoint Extends CPJoint

	Rem
	bbdoc: 
	End Rem
	Method Create:CPPivotJoint(bodyA:CPBody, bodyB:CPBody, pivot:CPVect)
		cpObjectPtr = bmx_cppivotjoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, pivot.vecPtr)
		Return Self
	End Method

End Type

Rem
bbdoc: I'm undocumented...
End Rem
Type CPGrooveJoint Extends CPJoint

	Rem
	bbdoc: 
	End Rem
	Method Create:CPGrooveJoint(bodyA:CPBody, bodyB:CPBody, grooveA:CPVect, grooveB:CPVect, anchor:CPVect)
		cpObjectPtr = bmx_cpgroovejoint_create(Self, bodyA.cpObjectPtr, bodyB.cpObjectPtr, grooveA.vecPtr, grooveB.vecPtr, anchor.vecPtr)
		Return Self
	End Method

End Type

Rem
bbdoc: 
End Rem
Function Vec2:CPVect(x:Float, y:Float)
	Return New CPVect.Create(x, y)
End Function

Extern
	Function bmx_momentforpoly:Float(m:Float, verts:CPVect[], count:Int, offset:Byte Ptr)
	Function bmx_cppolyshape_create:Byte Ptr(handle:Object, body:Byte Ptr, verts:CPVect[], count:Int, offset:Byte Ptr)
	Function bmx_cppolyshape_getverts(handle:Byte Ptr, verts:CPVect[])
End Extern

Rem
bbdoc: 
End Rem
Function MomentForPoly:Float(m:Float, verts:CPVect[], offset:CPVect)
	Return bmx_momentforpoly(m, verts, verts.length, offset.vecPtr)
End Function

Rem
bbdoc: 
End Rem
Function MomentForCircle:Float(m:Float, r1:Float, r2:Float, offset:CPVect)
	Return cpMomentForCircle(m, r1, r2, offset.vecPtr)
End Function
