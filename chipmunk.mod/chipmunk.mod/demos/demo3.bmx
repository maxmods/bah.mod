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

Function eachBody(body:Object, data:Object)
	Local pos:CPVect = CPBody(body).GetPosition()
	
	If pos.y > 260 Or Abs(pos.x) > 340 Then
		Local x:Float = Rand(-320, 320)
		CPBody(body).SetPosition(Vec2(x, -260))
	End If

End Function

Function update(ticks:Int)
	Local steps:Int = 1
	Local dt:Float = 1.0/60.0/steps
	
	For Local i:Int = 0 Until steps
		space.DoStep(dt)
		space.EachBody(eachBody)
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
	space.ResizeStaticHash(40.0, 999)
	space.ResizeActiveHash(30.0, 2999)
	space.SetGravity(Vec2(0, 100))


	Local body:CPBody
	Local shape:CPShape

	Local num:Int = 5
	Local verts:CPVect[] = New CPVect[num]
	For Local i:Int = 0 Until num
		Local angle:Float = 360/num * (4 - i)
		verts[i] = Vec2(10 * Cos(angle), 10 * Sin(angle))
	Next
	
	Local tris:CPVect[] = [ ..
		Vec2( 15, 15), ..
		Vec2(  0, -10), ..
		Vec2(-15, 15)]


	For Local i:Int = 0 Until 9
		For Local j:Int = 0 Until 6
			Local stagger:Float = (j Mod 2)*40
			Local offset:CPVect = Vec2(i * 80 - 320 + stagger, 240 - j * 70)

			shape = New CPPolyShape.Create(staticBody, tris, offset)
			shape.SetElasticity(1.0)
			shape.SetFriction(1.0)
			space.AddStaticShape(shape)
		Next
	Next

	For Local i:Int = 0 Until 300
		body = New CPBody.Create(1.0, MomentForPoly(1.0, verts, Vec2(0,0)))
		Local x:Float = Rand(-320, 320)
		body.SetPosition(Vec2(x, Rand(-350, -290)))
		space.AddBody(body)
		shape = New CPPolyShape.Create(body, verts, Vec2(0,0))
		shape.SetElasticity(0)
		shape.SetFriction(0.4)
		space.AddShape(shape)
	Next

End Function



