SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New Bridge.Create(), New TSettings)



Type Bridge Extends Test

	Method Create:Bridge()
	
		Init()
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef.Create()
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef.Create()
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateStaticBody(bd)
		ground.CreateShape(sd)
		
		
		sd = New b2PolygonDef.Create()
		sd.SetAsBox(0.5, 0.125)
		sd.SetDensity(20.0)
		sd.SetFriction(0.2)
		
		Local jd:b2RevoluteJointDef = New b2RevoluteJointDef.Create()
		Local numPlanks:Int = 30
		
		Local prevBody:b2Body = ground
		For Local i:Int = 0 Until numPlanks
		
			bd = New b2BodyDef.Create()
			bd.SetPosition(New b2Vec2.Create(-14.5 + 1.0 * i, 5.0))
			Local body:b2Body = m_world.CreateDynamicBody(bd)
			body.CreateShape(sd)
			body.SetMassFromShapes()
		
			Local anchor:b2Vec2 = New b2Vec2.Create(-15.0 + 1.0 * i, 5.0)
			jd.Initialize(prevBody, body, anchor)
			m_world.CreateJoint(jd)

			prevBody = body
		Next
		
		Local anchor:b2Vec2 = New b2Vec2.Create(-15.0 + 1.0 * numPlanks, 5.0)
		jd.Initialize(prevBody, ground, anchor)
		m_world.CreateJoint(jd)
		
		Return Self
		
	End Method


End Type
