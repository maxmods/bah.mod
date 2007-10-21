SuperStrict

Framework BRL.Max2D
Import BRL.StandardIO

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

Function collFunc:Int(shapeA:cpShape, shapeB:cpShape, contacts:CPContact[], normalCoeficient:Float, data:Object)
	
	Local pos:CPVect
	
	For Local i:Int = 0 Until contacts.length
		pos = contacts[i].GetPosition()
		DrawText "Collision at " + pos.x + ", " + pos.y, 10, -220 + i * 14
	Next

	Return True
End Function


Function init()

	staticBody = New CPBody.Create(INFINITY, INFINITY)
	
	ResetShapeIdCounter()
	
	space = New CPSpace.Create()

	space.ResizeStaticHash(20.0, 999)
	space.SetGravity(Vec2(0, 100))


	Local body:CPBody
	Local shape:CPShape

	shape = New CPSegmentShape.Create(staticBody, Vec2(-320,-240), Vec2(-320,239), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, Vec2(319,-240), Vec2(319,239), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	shape = New CPSegmentShape.Create(staticBody, Vec2(-320, 239), Vec2(319, 239), 0)
	shape.SetElasticity(1.0)
	shape.SetFriction(1.0)
	space.AddStaticShape(shape)

	For Local i:Int = 0 Until 50
		Local j:Int = i + 1
		Local a:CPVect = Vec2(i*10 - 320, i*10 - 240)
		Local b:CPVect = Vec2(j*10 - 320, i*10 - 240)
		Local c:CPVect = Vec2(j*10 - 320, j*10 - 240)
		
		shape = New CPSegmentShape.Create(staticBody, a, b, 0)
		shape.SetElasticity(1.0)
		shape.SetFriction(1.0)
		space.AddStaticShape(shape)
		
		shape = New CPSegmentShape.Create(staticBody, b, c, 0)
		shape.SetElasticity(1.0)
		shape.SetFriction(1.0)
		space.AddStaticShape(shape)
	Next
	
	Local verts:CPVect[] = [ ..
		Vec2(-15,-15), ..
		Vec2(-15, 15), ..
		Vec2( 15, 15), ..
		Vec2( 15,-15)]


	body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
	body.SetPosition(Vec2(-280, -240))
	space.AddBody(body)
	
	shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
	shape.SetElasticity(0)
	shape.SetFriction(1.5)
	shape.SetCollisionType(1)
	space.AddShape(shape)
	
	space.AddCollisionPairFunc(1, 0, collFunc)

End Function


