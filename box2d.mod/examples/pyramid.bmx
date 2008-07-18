SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New Pyramid.Create(), New TSettings)


Type Pyramid Extends Test

	Method Create:Pyramid()
	
		Init(10, 10)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		sd = New b2PolygonDef
		Local a:Float = 0.5
		sd.SetAsBox(a, a)
		sd.SetDensity(5)

		Local x:b2Vec2 = New b2Vec2.Create(-10.0, 1.0)
		Local y:b2Vec2 = New b2Vec2.Create()

		Local deltaX:b2Vec2 = New b2Vec2.Create(0.5625, 2.0)
		Local deltaY:b2Vec2 = New b2Vec2.Create(1.125, 0.0)

		For Local i:Int = 0 Until 25

			y.Copy(x)

			For Local j:Int = i Until 25
				Local bd:b2BodyDef = New b2BodyDef
				bd.SetPosition(y)
				Local body:b2Body = m_world.CreateBody(bd)
				body.CreateShape(sd)
				body.SetMassFromShapes()

				y.Add(deltaY)
			Next

			x.Add(deltaX)
		Next

		Return Self
	End Method

End Type

