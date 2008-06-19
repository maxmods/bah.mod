SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New CCDTest.Create(), New TSettings)


Type CCDTest Extends Test

	Method Create:CCDTest()
	
		Init(15, 15)

		Const k_restitution:Float = 1.4
		

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, 20.0))
		Local body:b2Body = m_world.CreateBody(bd)

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetDensity(0.0)
		sd.SetRestitution(k_restitution)

		sd.SetAsOrientedBox(0.1, 10.0, New b2Vec2.Create(-10.0, 0.0), 0.0)
		body.CreateShape(sd)

		sd.SetAsOrientedBox(0.1, 10.0, New b2Vec2.Create(10.0, 0.0), 0.0)
		body.CreateShape(sd)

		sd.SetAsOrientedBox(0.1, 10.0, New b2Vec2.Create(0.0, -10.0), 90)
		body.CreateShape(sd)

		sd.SetAsOrientedBox(0.1, 10.0, New b2Vec2.Create(0.0, 10.0), 270)
		body.CreateShape(sd)



		Local sd_bottom:b2PolygonDef = New b2PolygonDef
		sd_bottom.SetAsBox( 1.5, 0.15 )
		sd_bottom.SetDensity(4.0)

		Local sd_left:b2PolygonDef = New b2PolygonDef
		sd_left.SetAsOrientedBox(0.15, 2.7, New b2Vec2.Create(-1.45, 2.35), 11.4591513)
		sd_left.SetDensity(4.0)

		Local sd_right:b2PolygonDef = New b2PolygonDef
		sd_right.SetAsOrientedBox(0.15, 2.7, New b2Vec2.Create(1.45, 2.35), -11.4591513)
		sd_right.SetDensity(4.0)

		bd = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, 15.0))
		body = m_world.CreateBody(bd)
		body.CreateShape(sd_bottom)
		body.CreateShape(sd_left)
		body.CreateShape(sd_right)
		body.SetMassFromShapes()

		Return Self
		
	End Method


End Type

