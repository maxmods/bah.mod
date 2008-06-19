SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New ApplyForce.Create(), New TSettings)


Type ApplyForce Extends Test

	Field m_body:b2Body


	Method Create:ApplyForce()
	
		Init(12, 12)
						
		m_world.SetGravity(New b2Vec2.Create(0.0, 0.0))

		Const k_restitution:Float = 0.4

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, 20.0))
		Local ground:b2Body = m_world.CreateBody(bd)

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetDensity(0.0)
		sd.SetRestitution(k_restitution)

		sd.SetAsOrientedBox(0.2, 20.0, New b2Vec2.Create(-20.0, 0.0), 0.0)
		ground.CreateShape(sd)

		sd.SetAsOrientedBox(0.2, 20.0, New b2Vec2.Create(20.0, 0.0), 0.0)
		ground.CreateShape(sd)

		sd.SetAsOrientedBox(0.2, 20.0, New b2Vec2.Create(0.0, -20.0), 90)
		ground.CreateShape(sd)

		sd.SetAsOrientedBox(0.2, 20.0, New b2Vec2.Create(0.0, 20.0), 90)
		ground.CreateShape(sd)
		

		Local xf1:b2XForm = New b2XForm.Create()

		xf1.GetR().SetAngle(63.432)
		xf1.SetPosition(b2Mul(xf1.GetR(), New b2Vec2.Create(1.0, 0.0)))

		Local sd1:b2PolygonDef = New b2PolygonDef
		Local vertices:b2Vec2[] = New b2Vec2[3]
		vertices[0] = b2MulF(xf1, New b2Vec2.Create(-1.0, 0.0))
		vertices[1] = b2MulF(xf1, New b2Vec2.Create(1.0, 0.0))
		vertices[2] = b2MulF(xf1, New b2Vec2.Create(0.0, 0.5))
		sd1.SetVertices(vertices)
		sd1.SetDensity(2.0)

		Local xf2:b2XForm = New b2XForm.Create()
		xf2.GetR().SetAngle(-63.432)
		xf2.SetPosition(b2Mul(xf2.GetR(), New b2Vec2.Create(-1.0, 0.0)))

		Local sd2:b2PolygonDef = New b2PolygonDef
		vertices = New b2Vec2[3]
		vertices[0] = b2MulF(xf2, New b2Vec2.Create(-1.0, 0.0))
		vertices[1] = b2MulF(xf2, New b2Vec2.Create(1.0, 0.0))
		vertices[2] = b2MulF(xf2, New b2Vec2.Create(0.0, 0.5))
		sd2.SetVertices(vertices)
		sd2.SetDensity(2.0)

		bd = New b2BodyDef
		bd.SetAngularDamping(2.0)
		bd.SetLinearDamping(0.1)

		bd.SetPosition(New b2Vec2.Create(0.0, 1.05))
		bd.SetAngle(180)
		m_body = m_world.CreateBody(bd)
		m_body.CreateShape(sd1)
		m_body.CreateShape(sd2)
		m_body.SetMassFromShapes()
		
		Return Self
	End Method
	
	Method Keyboard()
		Super.Keyboard()
		
		If KeyDown(KEY_W) Then
			Local f:b2Vec2 = m_body.GetWorldVector(New b2Vec2.Create(0.0, -50.0))
			Local p:b2Vec2 = m_body.GetWorldPoint(New b2Vec2.Create(0.0, 1.0))
			m_body.ApplyForce(f, p)
		End If

		If KeyDown(KEY_A)
			m_body.ApplyTorque(10.0)
		End If

		If KeyDown(KEY_D)
			m_body.ApplyTorque(-10.0)
		End If

	End Method

End Type

