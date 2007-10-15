SuperStrict

Framework BRL.Max2D


Import "main.bmx"


InitChipmunk()

init()

Graphics 640, 480, 0

SetColor(255, 255, 255)


Global joint:CPJoint
Global chassis:CPBody, wheel1:CPBody, wheel2:CPBody


While Not KeyDown(KEY_ESCAPE)

	Cls
	
	display()
	
	
	Flip

	GCCollect
Wend

End



Function update(ticks:Int)
	Local steps:Int = 3
	Local dt:Float = 1.0/60.0/steps
	
	For Local i:Int = 0 Until steps
		chassis.ResetForces()
		wheel1.ResetForces()
		wheel2.ResetForces()
		DampedSpring(chassis, wheel1, Vec2(40, 15), Vec2(0,0), 50, 150, 10, dt)
		DampedSpring(chassis, wheel2, Vec2(-40, 15), Vec2(0,0), 50, 150, 10, dt)
		
		space.DoStep(dt)
	Next
	
End Function


Function display()

	SetOrigin 320, 240
	


	space.GetActiveShapes().ForEach(drawObject)
	space.GetStaticShapes().ForEach(drawObject)
	
	ticks:+ 1

	update(ticks)
	
End Function

Function init()

	staticBody = New CPBody.Create(INFINITY, INFINITY)
	
	ResetShapeIdCounter()
	
	space = New CPSpace.Create()
	space.SetIterations(10)
	space.ResizeActiveHash(50.0, 999)
	space.ResizeStaticHash(50.0, 999)
	space.SetGravity(Vec2(0, 300))

	Local body:CPBody
	Local shape:CPShape

	shape = New CPSegmentShape.Create(staticBody, Vec2(-320, 240), Vec2(-320,-240), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, Vec2(320, 240), Vec2(320,-240), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)
	
	shape = New CPSegmentShape.Create(staticBody, Vec2(-320, 240), Vec2(320, 240), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, Vec2(-320,70), Vec2(0,240), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)
	
	shape = New CPSegmentShape.Create(staticBody, Vec2(0, 240), Vec2(320, 200), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)
	
	shape = New CPSegmentShape.Create(staticBody, Vec2(200, 240), Vec2(320, 100), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	Local body1:CPBody, body2:CPBody, body3:CPBody, body4:CPBody, body5:CPBody, body6:CPBody, body7:CPBody
		
	body1 = makeBox(-100, -100);
	body2 = makeBox(body1.GetPosition().x + 40, -100)
	body3 = makeBox(body2.GetPosition().x + 40, -100)
	body4 = makeBox(body3.GetPosition().x + 40, -100)
	body5 = makeBox(body4.GetPosition().x + 40, -100)
	body6 = makeBox(body5.GetPosition().x + 40, -100)
	body7 = makeBox(body6.GetPosition().x + 40, -100)


	joint = New CPPivotJoint.Create(staticBody, body1, Vec2(body1.GetPosition().x - 20, -100))
	space.AddJoint(joint)

	joint = New CPPivotJoint.Create(body1, body2, Vec2(body2.GetPosition().x - 20, -100))
	space.AddJoint(joint)
	
	joint = New CPPivotJoint.Create(body2, body3, Vec2(body3.GetPosition().x - 20, -100))
	space.AddJoint(joint)
	
	joint = New CPPivotJoint.Create(body3, body4, Vec2(body4.GetPosition().x - 20, -100))
	space.AddJoint(joint)
	
	joint = New CPPivotJoint.Create(body4, body5, Vec2(body5.GetPosition().x - 20, -100))
	space.AddJoint(joint)
	
	joint = New CPPivotJoint.Create(body5, body6, Vec2(body6.GetPosition().x - 20, -100))
	space.AddJoint(joint)
	
	joint = New CPPivotJoint.Create(body6, body7, Vec2(body7.GetPosition().x - 20, -100))
	space.AddJoint(joint)
	
	joint = New CPPivotJoint.Create(body7, staticBody, Vec2(body7.GetPosition().x + 20, -100))
	space.AddJoint(joint)
	
	body1 = makeBox(-100, -50);
	body2 = makeBox(body1.GetPosition().x + 40, -50)
	body3 = makeBox(body2.GetPosition().x + 40, -50)
	body4 = makeBox(body3.GetPosition().x + 40, -50)
	body5 = makeBox(body4.GetPosition().x + 40, -50)
	body6 = makeBox(body5.GetPosition().x + 40, -50)
	body7 = makeBox(body6.GetPosition().x + 40, -50)
	
	Local _max:Float = 25
	Local _min:Float = 10

	joint = New CPSlideJoint.Create(staticBody, body1, Vec2(body1.GetPosition().x - 15 - 10, -50), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body1, body2, Vec2(15, 0), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body2, body3, Vec2(15, 0), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body3, body4, Vec2(15, 0), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body4, body5, Vec2(15, 0), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body5, body6, Vec2(15, 0), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body6, body7, Vec2(15, 0), Vec2(-15, 0), _min, _max)
	space.AddJoint(joint)
	
	joint = New CPSlideJoint.Create(body7, staticBody, Vec2(15, 0), Vec2(body7.GetPosition().x + 15 + 10, -50), _min, _max)
	space.AddJoint(joint)
	
	body1 = makeBox(-100, -150)
	body2 = makeBox(body1.GetPosition().x + 40, -150)
	body3 = makeBox(body2.GetPosition().x + 40, -150)
	body4 = makeBox(body3.GetPosition().x + 40, -150)
	body5 = makeBox(body4.GetPosition().x + 40, -150)
	body6 = makeBox(body5.GetPosition().x + 40, -150)
	body7 = makeBox(body6.GetPosition().x + 40, -150)
	
	joint = New CPPinJoint.Create(staticBody, body1, Vec2(body1.GetPosition().x - 15 - 10, -150), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body1, body2, Vec2(15, 0), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body2, body3, Vec2(15, 0), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body3, body4, Vec2(15, 0), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body4, body5, Vec2(15, 0), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body5, body6, Vec2(15, 0), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body6, body7, Vec2(15, 0), Vec2(-15, 0))
	space.AddJoint(joint)
	
	joint = New CPPinJoint.Create(body7, staticBody, Vec2(15, 0), Vec2(body7.GetPosition().x + 15 + 10, -150))
	space.AddJoint(joint)
	
	body1 = makeBox(190, 200)
	joint = New CPGrooveJoint.Create(staticBody, body1, Vec2(0, -195), Vec2(250, -200), Vec2(-15, 0))
	space.AddJoint(joint)



	Local verts:CPVect[] = [..
		Vec2(-20, 15), ..
		Vec2( 20, 15), ..
		Vec2( 20,-15), ..
		Vec2(-20,-15)]
	
	chassis = New CPBody.Create(10.0, MomentForPoly(10.0, verts, Vec2(0,0)))
	chassis.SetPosition(Vec2(-200, -100))
	chassis.SetAngularVelocity(5)
	space.AddBody(chassis)
	shape = New CPPolyShape.Create(chassis, verts, Vec2(0,0))
	shape.SetElasticity(0.0)
	shape.SetFriction(1.0)
	space.AddShape(shape)
	
	Local radius:Float = 15
	Local wheelMass:Float = 0.3
	Local offset:CPVect = Vec2(radius + 30, -25)
	wheel1 = New CPBody.Create(wheelMass, MomentForCircle(wheelMass, 0.0, radius, Vec2(0,0)))
	wheel1.SetPosition(chassis.GetPosition().Add(offset))
	wheel1.SetVelocity(chassis.GetVelocity())
	space.AddBody(wheel1)
	shape = New CPCircleShape.Create(wheel1, radius, Vec2(0,0))
	shape.SetElasticity(0.0)
	shape.SetFriction(2.5)
	space.AddShape(shape)
	
	joint = New CPPinJoint.Create(chassis, wheel1, Vec2(0,0), Vec2(0,0))
	space.AddJoint(joint)
		
	wheel2 = New CPBody.Create(wheelMass, MomentForCircle(wheelMass, 0.0, radius, Vec2(0,0)))
	wheel2.SetPosition(chassis.GetPosition().Add(Vec2(-offset.x, offset.y)))
	wheel2.SetVelocity(chassis.GetVelocity())
	space.AddBody(wheel2)
	shape = New CPCircleShape.Create(wheel2, radius, Vec2(0,0))
	shape.SetElasticity(0.0)
	shape.SetFriction(2.5)
	space.AddShape(shape)
	
	joint = New CPPinJoint.Create(chassis, wheel2, Vec2(0,0), Vec2(0,0))
	space.AddJoint(joint)
	
End Function


Function makeBox:CPBody(x:Float, y:Float)

	Local verts:CPVect[] = [ ..
		Vec2(-15,-7), ..
		Vec2(-15, 7), ..
		Vec2( 15, 7), ..
		Vec2( 15,-7)]
	
	Local body:CPBody = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
	body.SetPosition(Vec2(x, y))
	space.AddBody(body)
	Local shape:CPShape = New CPPolyShape.Create(body, verts, Vec2(0,0))
	shape.SetElasticity(0.0)
	shape.SetFriction(1.0)
	space.AddShape(shape)
	
	Return body

End Function
