SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New Chain.Create(), New TSettings)


Type Chain Extends Test

	Method Create:Chain()
	
		Init(15, 15)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		sd = New b2PolygonDef
		sd.SetAsBox(0.6, 0.125)
		sd.SetDensity(20.0)
		sd.SetFriction(0.2)

		Local jd:b2RevoluteJointDef = New b2RevoluteJointDef
		jd.SetCollideConnected(False)

		Const y:Float = 25.0
		Local prevBody:b2Body = ground
		For Local i:Int = 0 Until 30
			bd = New b2BodyDef
			bd.SetPosition(New b2Vec2.Create(0.5 + i, y))
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(sd)
			body.SetMassFromShapes()

			Local anchor:b2Vec2 = New b2Vec2.Create(i, y)
			jd.Initialize(prevBody, body, anchor)
			m_world.CreateJoint(jd)

			prevBody = body
		Next

		Return Self
	End Method

End Type


