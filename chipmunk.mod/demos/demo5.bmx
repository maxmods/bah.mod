SuperStrict

Framework BRL.Max2D
Import BRL.Random

Import "main.bmx"


InitChipmunk()

init()

Graphics 640, 480, 0

SetColor(255, 255, 255)

While Not KeyDown(KEY_ESCAPE)
	
	
	Cls
	
	display()
	
	Flip


Wend

End

Function update(ticks:Int)
	Local steps:Int = 2
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


Function init()

	staticBody = New CPBody.Create(INFINITY, INFINITY)
	
	ResetShapeIdCounter()
	
	space = New CPSpace.Create()
	space.SetIterations(20)
	space.ResizeStaticHash(40.0, 2999)
	space.ResizeActiveHash(40.0, 999)
	space.SetGravity(Vec2(0, 300))

	Local body:CPBody
	Local shape:CPShape
	
	Local verts:CPVect[] = [ ..
		Vec2( 3, 20), ..
		Vec2( 3,-20), ..
		Vec2(-3,-20), ..
		Vec2(-3, 20)]

	shape = New CPSegmentShape.Create(staticBody, Vec2(-600, 240), Vec2(600, 240), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	Local friction:Float = 0.6
	
	Local n:Int = 9
	For Local i:Int = 1 To n
		Local offset:CPVect = Vec2(i*60/2.0, (n - i)*52)
		
		For Local j:Int = 0 Until i
			body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
			body.SetPosition(Vec2(j*60, 220).Sub(offset))
			space.AddBody(body)
			shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
			shape.SetElasticity(0.0)
			shape.SetFriction(friction)
			space.AddShape(shape)

			body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
			body.SetPosition(Vec2(j*60, 197).Sub(offset))
			body.SetAngle(90)
			space.AddBody(body)
			shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
			shape.SetElasticity(0.0)
			shape.SetFriction(friction)
			space.AddShape(shape)
			
			If j = (i - 1) Then
				Continue
			End If
			body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
			body.SetPosition(Vec2(j*60 + 30, 191).Sub(offset))
			body.SetAngle(90)
			space.AddBody(body)
			shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
			shape.SetElasticity(0.0)
			shape.SetFriction(friction)
			space.AddShape(shape)
		Next

		body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
		body.SetPosition(Vec2(-17, 174).Sub(offset))
		space.AddBody(body)
		shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
		shape.SetElasticity(0.0)
		shape.SetFriction(friction)
		space.AddShape(shape)

		body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
		body.SetPosition(Vec2((i - 1)*60 + 17, 174).Sub(offset))
		space.AddBody(body)
		shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
		shape.SetElasticity(0.0)
		shape.SetFriction(friction)
		space.AddShape(shape)
	Next
	
	body.SetAngularVelocity(1)
	
	body.SetVelocity(Vec2(body.GetAngularVelocity() * 20, 0))
	
End Function





