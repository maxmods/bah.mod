SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend

Run(New Raycast.Create(), New TSettings)


Type Raycast Extends Test

	Field laserBody:b2Body

	Method Create:Raycast()
	
		Init(15, 15)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		bd = New b2BodyDef
		bd.SetPositionXY(0.0, 1.0)
		laserBody = m_world.CreateBody(bd)
		
		b2PolygonDef sd
		sd.SetAsBox(5.0, 1.0)
		sd.SetDensity(4.0)
		laserBody.CreateShape(sd)
		laserBody.SetMassFromShapes()
		
		Local body:b2Body
		' Create a few shapes
		bd.SetPositionXY(-5.0, 10.0)
		body = m_world.CreateBody(bd)
		
		Local cd:b2CircleDef = New b2CircleDef
		cd.SetRadius(3)
		body.CreateShape(cd)
		
		bd.SetPositionXY(5.0, 10.0)
		body = m_world.CreateBody(bd)
		
		body.CreateShape(cd)


		Return Self
		
	End Method

	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		Local segmentLength:Float = 40
		
		Local segment:b2Segment = New b2Segment.Create()
		Local laserStart:b2Vec2 = Vec2(5.0 - 0.1, 0.0)
		Local laserDir:b2Vec2 = Vec2(segmentLength, 0.0)
		segment.SetStartPoint(laserBody.GetWorldPoint(laserStart))
		segment.SetEndPoint(laserBody.GetWorldVector(laserDir))
		segment.SetEndPoint(segment.GetEndPoint().Plus(segment.GetStartPoint()))
		
		For Local rebounds:Int = 0 Until 10
		
			Local lambda:Float = 1
			Local normal:b2Vec2 = New b2Vec2.Create()
			Local shape:b2Shape = m_world.RaycastOne(segment, lambda, normal, False)
		
			Local laserColor:b2Color = New b2Color.Set(255,0,0)
		
			If shape Then
				m_debugDraw.DrawSegment(segment.GetStartPoint(), ..
					segment.GetStartPoint().Multiply(1-lambda).Plus(segment.GetEndPoint().Multiply(lambda)),laserColor)
			Else
				m_debugDraw.DrawSegment(segment.GetStartPoint(), segment.GetEndPoint(), laserColor)
				Exit
			End If
			
			' Bounce
			segmentLength :* (1 - lambda)
			If segmentLength <= 0.00007 Then
				Exit
			End If
			laserStart = segment.GetStartPoint().Multiply(1 - lambda).Plus(segment.GetEndPoint().Multiply(lambda))
			laserDir = segment.GetEndPoint().Subtract(segment.GetStartPoint())
			laserDir.Normalize()
			laserDir = laserDir.Subtract(normal.Multiply(2 * b2Dot(laserDir, normal)))
			segment.SetStartPoint(laserStart.Subtract(laserDir.Multiply(0.1)))
			segment.SetEndPoint(laserStart.Plus(laserDir.Multiply(segmentLength)))
		Next

		
	End Method

End Type

