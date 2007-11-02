SuperStrict

Framework BRL.Max2D


Import "main.bmx"


InitChipmunk()

init()

Graphics 640, 480, 0

SetColor(255, 255, 255)

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	display()
	
	
	Flip

	GCCollect
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
	space.ResizeStaticHash(20.0, 999)
	space.SetGravity(Vec2(0, 100))

	Local verts:CPVect[] = [ ..
		Vec2( 7,-15), ..
		Vec2(-7,-15), ..
		Vec2(-7, 15), ..
		Vec2( 7, 15)]

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
		Local a:CPVect = Vec2(i*10 - 320, 40 + i*10)
		Local b:CPVect = Vec2(j*10 - 320, 40 + i*10)
		Local c:CPVect = Vec2(j*10 - 320, 40 + j*10)
		
		shape = New CPSegmentShape.Create(staticBody, a, b, 0.0)
		shape.SetElasticity(1.0)
		shape.SetFriction(1.0)
		space.AddStaticShape(shape)
		
		shape = New CPSegmentShape.Create(staticBody, b, c, 0.0)
		shape.SetElasticity(1.0)
		shape.SetFriction(1.0)
		space.AddStaticShape(shape)
	Next

	Local moment:Float = MomentForPoly(1.0, verts, Vec2(0,-15))
	moment:+ MomentForCircle(1.0, 0.0, 25.0, Vec2(0,15))
	
	For Local i:Int = 0 Until 2
		body = New CPBody.Create(1.0, moment)
		body.SetPosition(Vec2(-280, -250 - (i * 200)))
		body.SetAngularVelocity(1.2)
		space.AddBody(body)
	
		shape = New CPPolyShape.Create(body, verts, Vec2(0,15))
		shape.SetElasticity(0.0)
		shape.SetFriction(1.5)
		space.AddShape(shape)
	
		shape = New CPCircleShape.Create(body, 25.0, Vec2(0,-15))
		shape.SetElasticity(0.9)
		shape.SetFriction(1.5)
		space.AddShape(shape)
	Next
	
End Function

