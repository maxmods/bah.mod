' Copyright (c) 2008-2009 Bruce A Henderson
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

Import "include/*.h"

Import "glue.cpp"

Import "Source/Dynamics/b2Body.cpp"
Import "Source/Dynamics/b2ContactManager.cpp"
Import "Source/Dynamics/b2Island.cpp"
Import "Source/Dynamics/b2World.cpp"
Import "Source/Dynamics/b2WorldCallbacks.cpp"

Import "Source/Dynamics/Contacts/b2CircleContact.cpp"
Import "Source/Dynamics/Contacts/b2PolyContact.cpp"
Import "Source/Dynamics/Contacts/b2Contact.cpp"
Import "Source/Dynamics/Contacts/b2ContactSolver.cpp"
Import "Source/Dynamics/Contacts/b2PolyAndCircleContact.cpp"
Import "Source/Dynamics/Contacts/b2EdgeAndCircleContact.cpp"
Import "Source/Dynamics/Contacts/b2PolyAndEdgeContact.cpp"

Import "Source/Dynamics/Joints/b2DistanceJoint.cpp"
Import "Source/Dynamics/Joints/b2RevoluteJoint.cpp"
Import "Source/Dynamics/Joints/b2GearJoint.cpp"
Import "Source/Dynamics/Joints/b2Joint.cpp"
Import "Source/Dynamics/Joints/b2MouseJoint.cpp"
Import "Source/Dynamics/Joints/b2PrismaticJoint.cpp"
Import "Source/Dynamics/Joints/b2PulleyJoint.cpp"
Import "Source/Dynamics/Joints/b2LineJoint.cpp"

Import "Source/Dynamics/Controllers/b2BuoyancyController.cpp"
Import "Source/Dynamics/Controllers/b2ConstantAccelController.cpp"
Import "Source/Dynamics/Controllers/b2ConstantForceController.cpp"
Import "Source/Dynamics/Controllers/b2Controller.cpp"
Import "Source/Dynamics/Controllers/b2GravityController.cpp"
Import "Source/Dynamics/Controllers/b2TensorDampingController.cpp"

Import "Source/Collision/b2BroadPhase.cpp"
Import "Source/Collision/b2TimeOfImpact.cpp"
Import "Source/Collision/b2CollideCircle.cpp"
Import "Source/Collision/b2CollidePoly.cpp"
Import "Source/Collision/b2Collision.cpp"
Import "Source/Collision/b2Distance.cpp"
Import "Source/Collision/b2PairManager.cpp"

Import "Source/Collision/Shapes/b2CircleShape.cpp"
Import "Source/Collision/Shapes/b2Shape.cpp"
Import "Source/Collision/Shapes/b2PolygonShape.cpp"
Import "Source/Collision/Shapes/b2EdgeShape.cpp"

Import "Source/Common/b2BlockAllocator.cpp"
Import "Source/Common/b2StackAllocator.cpp"
Import "Source/Common/b2Math.cpp"
Import "Source/Common/b2Settings.cpp"


