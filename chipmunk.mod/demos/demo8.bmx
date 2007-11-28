SuperStrict

Framework BRL.Max2D


Import "main.bmx"

Const WIDTH:Int = 200
Const HEIGHT:Int = 40
Global boxCount:Int

InitChipmunk()

init()

Graphics 640, 480, 0

SetColor(255, 255, 255)


While Not KeyDown(KEY_ESCAPE)

	Cls
	
	display()
	
	drawFPS()
	DrawText boxCount, 300, -230
	
	If KeyHit(key_b) Then
		makeBox(Vec2(0, -250), Vec2(0, 0), 0.0, 10.0)
	End If
	
	Flip

	GCCollect
Wend

End

Function update(ticks:Int)
	Local steps:Int = 1
	Local dt:Float = 1.0/60.0/steps
	
	For Local i:Int = 0 Until steps
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

Function applyBuoyancy(body:CPBody, gravity:CPVect, damping:Float, dt:Float)

	Const numx:Int = 20
	Const numy:Int = 4
	
	Const stepx:Float = WIDTH/Float(numx)
	Const stepy:Float = HEIGHT/Float(numy)
	
	body.ResetForces()

	For Local x:Int = 0 Until numx
		For Local y:Int = 0 Until numy
			Local pSample:CPVect = Vec2((x + 0.5)*stepx - WIDTH/2, (y + 0.5)*stepy - HEIGHT/2)
			Local p:CPVect = body.Local2World(pSample)
			Local r:CPVect = p.Sub(body.GetPosition())
		
			If p.y > 0 Then
				Local v:CPVect = body.GetVelocity().Add(r.Perp().Mult(body.GetAngularVelocity()))
				Local fDamp:CPVect = v.Mult(-0.0003 * v.Length())
				Local f:CPVect = Vec2(0, -2.0).Add(fDamp)
				body.ApplyForce(f, r)
			End If
		Next
	Next
	
	body.UpdateVelocity(gravity, damping, dt)

End Function

Function makeBox(p:CPVect, v:CPVect, a:Float, w:Float)

	Global verts:CPVect[] = [ ..
		Vec2(-WIDTH/3.5,-HEIGHT/2.0), ..
		Vec2(-WIDTH/3.5, HEIGHT/2.0), ..
		Vec2( WIDTH/3.5, HEIGHT/2.0), ..
		Vec2( WIDTH/3.5,-HEIGHT/2.0) ]
	

	Local body:CPBody = New CPBody.Create(1.0, MomentForPoly(1.0, verts, CPVZero))
	body.SetPosition(p)
	body.SetVelocity(v)
	body.SetAngle(a)
	body.SetAngularVelocity(w)
	body.SetVelocityFunction(applyBuoyancy)
	space.AddBody(body)
	
	Local shape:CPShape = New CPPolyShape.Create(body, verts, CPVZero)
	shape.SetElasticity(0.0)
	shape.SetFriction(0.7)
	space.AddShape(shape)
	
	boxCount:+1
End Function

Function addBall(vel:Int, pos:Int)

	Local radius:Float = 15.0
	Local body:CPBody = New CPBody.Create(2, MomentForCircle(10.0, 0.0, radius, CPVZero))
	body.SetPosition(Vec2(0, pos))
	body.SetVelocity(Vec2(vel, 0))

	body.SetVelocityFunction(applyBuoyancy)

	space.AddBody(body)
	Local shape:CPShape = New CPCircleShape.Create(body, radius, CPVZero)
	shape.SetElasticity(0.4)
	shape.SetFriction(0.6)
	space.AddShape(shape)

End Function

Function init()

	staticBody = New CPBody.Create(INFINITY, INFINITY)
	
	ResetShapeIdCounter()
	
	space = New CPSpace.Create()
	space.SetIterations(5)
	space.ResizeActiveHash(30.0, 2999)
	space.ResizeStaticHash(40.0, 999)
	space.SetGravity(Vec2(0, 100))

	Local shape:CPShape
	
	' Screen border
	shape = New CPSegmentShape.Create(staticBody, Vec2(-320, 240), Vec2(-320, -240), 0.0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, Vec2(320, 240), Vec2(320, -240), 0.0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, Vec2(-320, 240), Vec2(320, 240), 0.0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)
	
	' Reference line
	shape = New CPSegmentShape.Create(staticBody, Vec2(-320,0), Vec2(320,0), 0.0)
	shape.SetCollisionType(1)
	space.AddStaticShape(shape)
	
	space.AddCollisionPairFunc(0, 1, Null, Null)
	
	' Create boxes
	makeBox(Vec2(-150, -150), Vec2(200, 100), 0, 0.0)
	makeBox(Vec2(150, -150), Vec2(0, 300), 5, 0.0)
	makeBox(Vec2(0, -150), Vec2(0, -200), 0.0, 0.0)
	makeBox(Vec2(0, -250), Vec2(50, -100), 0.0, 2.0)

End Function
