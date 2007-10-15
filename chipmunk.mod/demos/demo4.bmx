SuperStrict

Framework BRL.Max2D
Import BRL.Random

Import "main4.bmx"


InitChipmunk()

init()

Graphics 640, 480, 0

SetColor(255, 255, 255)

While Not KeyDown(KEY_ESCAPE)
	
	
	Cls
	
	display()
	
	DrawText "'T' to toggle text - " + toggleMode, -310, 220
	
	Flip

	If KeyHit(KEY_T) Then
		toggleMode = Not toggleMode
	End If

Wend

End

Function update(ticks:Int)
	Local steps:Int = 3
	Local dt:Float = 1.0/60.0/steps
	
	For Local i:Int = 0 Until steps
		space.DoStep(dt)
		staticBody.UpdatePosition(dt)
		space.RehashStatic()
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
	space.SetIterations(5)
	space.ResizeStaticHash(30.0, 999)
	space.ResizeActiveHash(200.0, 99)
	space.SetGravity(Vec2(0, 600))

	Local body:CPBody
	Local shape:CPShape
	
	Local verts:CPVect[] = [ ..
		Vec2(-30,-15), ..
		Vec2(-30, 15), ..
		Vec2( 30, 15), ..
		Vec2( 30,-15)]

	Local a:CPVect = Vec2(-200,  200)
	Local b:CPVect = Vec2(-200, -200)
	Local c:CPVect = Vec2( 200, -200)
	Local d:CPVect = Vec2( 200,  200)
	
	shape = New CPSegmentShape.Create(staticBody, a, b, 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, b, c, 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, c, d, 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, d, a, 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)
	
	staticBody.SetAngularVelocity(0.4)

	Local count:Int = 1
	For Local i:Int = 0 Until 3
		For Local j:Int = 0 Until 7
			body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
			body.SetPosition(Vec2(i*60 - 150, j*30 - 150))
			space.AddBody(body)
			shape = New ExtraShape.Create(body, verts, Vec2(0,0))
			ExtraShape(shape).letter = count
			shape.SetElasticity(0.0)
			shape.SetFriction(0.7)
			space.AddShape(shape)
			
			count:+ 1
		Next
	Next
	
End Function

