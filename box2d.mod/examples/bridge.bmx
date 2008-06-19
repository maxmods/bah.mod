SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend

Run(New Bridge.Create(), New TSettings)


Type Bridge Extends Test

	Method Create:Bridge()
	
		Init(22, 22)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		sd = New b2PolygonDef
		sd.SetAsBox(0.5, 0.125)
		sd.SetDensity(20.0)
		sd.SetFriction(0.2)
		
		Local jd:b2RevoluteJointDef = New b2RevoluteJointDef
		Local numPlanks:Int = 30
		
		Local prevBody:b2Body = ground
		For Local i:Int = 0 Until numPlanks
		
			bd = New b2BodyDef
			bd.SetPosition(New b2Vec2.Create(-14.5 + 1.0 * i, 5.0))
			Local body:b2Body = m_world.CreateBody(bd)
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

		For Local i:Int = 0 Until 2
	
			Local sd:b2PolygonDef = New b2PolygonDef
			Local vertices:b2Vec2[] = New b2Vec2[3]
			vertices[0] = Vec2(-0.5, 0.0)
			vertices[1] = Vec2(0.5, 0.0)
			vertices[2] = Vec2(0.0, 1.5)
			sd.SetVertices(vertices)
			sd.SetDensity(1.0)
			
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(-8.0 + 8.0 * i, 12.0)
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(sd)
			body.SetMassFromShapes()
		Next
		
		For Local i:Int = 0 Until 3
			Local sd1:b2CircleDef = New b2CircleDef
			sd1.SetRadius(0.5)
			sd1.SetDensity(1.0)
			
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(-6.0 + 6.0 * i, 10.0)
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(sd1)
			body.SetMassFromShapes()
		Next
		
		Return Self
		
	End Method


End Type
