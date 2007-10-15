SuperStrict

Framework BRL.Max2D


Import "main2.bmx"


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

Function init()

	staticBody = New CPBody.Create(INFINITY, INFINITY)
	
	ResetShapeIdCounter()
	
	space = New CPSpace.Create()
	space.SetIterations(20)
	space.ResizeStaticHash(40.0, 1000)
	space.ResizeActiveHash(40.0, 1000)
	space.SetGravity(Vec2(0, 100))

	Local verts:CPVect[] = [ ..
		Vec2(-15,-15), ..
		Vec2(-15, 15), ..
		Vec2( 15, 15), ..
		Vec2( 15,-15)]

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

	For Local i:Int = 0 Until 14
		For Local j:Int = 0 To i
			body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
			'body = New CPBody.Create(1.0, MomentForCircle(10.0, 0.0, 15.0, Vec2(0,0)))
			body.SetPosition(Vec2(j*32 - i*16, 194 - i*30))
			space.AddBody(body)
			shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
			'shape = New CPCircleShape.Create(body, 15.0, Vec2(0,0))
			shape.SetElasticity(0.0)
			shape.SetFriction(0.8)
			space.AddShape(shape)
		Next
	Next
	
	Local radius:Float = 15.0
	body = New CPBody.Create(10.0, MomentForCircle(10.0, 0.0, radius, Vec2(0,0)))
	body.SetPosition(Vec2(0, 240 - radius))
	'body.SetVelocity(Vec2(0, 0))

	space.AddBody(body)
	shape = New CPCircleShape.Create(body, radius, Vec2(0,0))
	shape.SetElasticity(0)
	shape.SetFriction(0.9)
	space.AddShape(shape)
	
End Function
