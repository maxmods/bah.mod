SuperStrict

' This is a simple example of building and running a simulation
' using Box2D. Here we create a large ground box and a small dynamic box.

Framework BaH.Box2D
Import BRL.StandardIO

' Define the size of the world. Simulation will still work
' if bodies reach the end of the world, but it will be slower.
Local worldAABB:b2AABB = New b2AABB.Create()
worldAABB.SetLowerBound(New b2Vec2.Create(-100.0, -100.0))
worldAABB.SetUpperBound(New b2Vec2.Create(100.0, 100.0))

' Define the gravity vector.
Local gravity:b2Vec2 = New b2Vec2.Create(0.0, -10.0)

' Do we want to let bodies sleep?
Local doSleep:Int = True

' Construct a world object, which will hold and simulate the rigid bodies.
Local world:b2World = New b2World.Create(worldAABB, gravity, doSleep)

' Define the ground body.
Local groundBodyDef:b2BodyDef = New b2BodyDef
groundBodyDef.SetPosition(New b2Vec2.Create(0.0, -10.0))

' Call the body factory which allocates memory for the ground body
' from a pool and creates the ground box shape (also from a pool).
' The body is also added to the world.
Local groundBody:b2Body = world.CreateBody(groundBodyDef)

' Define the ground box shape.
Local groundShapeDef:b2PolygonDef = New b2PolygonDef

' The extents are the half-widths of the box.
groundShapeDef.SetAsBox(50.0, 10.0)


' Set the density of the ground box to zero. This will
' make the ground body static (fixed).
groundShapeDef.SetDensity(0.0)

' Add the ground shape to the ground body.
groundBody.CreateShape(groundShapeDef)

' Define the dynamic body. We set its position and call the body factory.
Local bodyDef:b2BodyDef = New b2BodyDef
bodyDef.SetPosition(New b2Vec2.Create(0.0, 4.0))
Local body:b2Body = world.CreateBody(bodyDef)

' Define another box shape for our dynamic body.
Local shapeDef:b2PolygonDef = New b2PolygonDef
shapeDef.SetAsBox(1.0, 1.0)


' Set the box density to be non-zero, so it will be dynamic.
shapeDef.SetDensity(1.0)

' Override the default friction.
shapeDef.SetFriction(0.3)

' Add the shape to the body.
body.CreateShape(shapeDef)

' Now tell the dynamic body to compute it's mass properties base
' on its shape.
body.SetMassFromShapes()

' Prepare for simulation. Typically we use a time step of 1/60 of a
' second (60Hz) and 10 iterations. This provides a high quality simulation
' in most game scenarios.
Local timeStep:Float = 1.0 / 60.0
Local iterations:Int = 10


' This is our little game loop.
For Local i:Int = 0 Until 60

	' Instruct the world to perform a single step of simulation. It is
	' generally best to keep the time step and iterations fixed.
	world.DoStep(timeStep, iterations)
	
	' Now print the position and rotation of the body.
	Local position:b2Vec2 = body.GetPosition()
	Local angle:Float = body.GetAngle()

	Print position.GetX() + " " + position.GetY() + " " + angle
Next


