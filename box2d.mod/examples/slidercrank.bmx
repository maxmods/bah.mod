SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New SliderCrank.Create(), New TSettings)


Type SliderCrank Extends Test

	Field m_joint1:b2RevoluteJoint
	Field m_joint2:b2PrismaticJoint

	Method Create:SliderCrank()
	
		Init(16, 16)
				
		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		' Define crank.
		sd = New b2PolygonDef
		sd.SetAsBox(0.5, 2.0)
		sd.SetDensity(1.0)

		Local rjd:b2RevoluteJointDef = New b2RevoluteJointDef

		Local prevBody:b2Body = ground

		bd = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, 7.0))

		Local body:b2Body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()

		rjd.Initialize(prevBody, body, New b2Vec2.Create(0.0, 5.0))
		rjd.SetMotorSpeed(180)
		rjd.SetMaxMotorTorque(10000.0)
		rjd.EnableMotor(True)
		m_joint1 = b2RevoluteJoint(m_world.CreateJoint(rjd))

		prevBody = body

		' Define follower.
		sd.SetAsBox(0.5, 4.0)
		bd.SetPosition(New b2Vec2.Create(0.0, 13.0))
		body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()

		rjd.Initialize(prevBody, body, New b2Vec2.Create(0.0, 9.0))
		rjd.EnableMotor(False)
		m_world.CreateJoint(rjd)

		prevBody = body

		' Define piston
		sd.SetAsBox(1.5, 1.5)
		bd.SetPosition(New b2Vec2.Create(0.0, 17.0))
		body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()

		rjd.Initialize(prevBody, body, New b2Vec2.Create(0.0, 17.0))
		m_world.CreateJoint(rjd)

		Local pjd:b2PrismaticJointDef = New b2PrismaticJointDef
		pjd.Initialize(ground, body, New b2Vec2.Create(0.0, 17.0), New b2Vec2.Create(0.0, 1.0))

		pjd.SetMaxMotorForce(1000.0)
		pjd.EnableMotor(True)

		m_joint2 = b2PrismaticJoint(m_world.CreateJoint(pjd))

		' Create a payload
		sd.SetDensity(2.0)
		bd.SetPosition(New b2Vec2.Create(0.0, 23.0))
		body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()
		
		Return Self
	End Method
	
	Method Keyboard()
		Super.Keyboard()
		
		If KeyHit(KEY_F) Then
			m_joint2.EnableMotor(Not m_joint2.IsMotorEnabled())
			m_joint2.GetBody2().WakeUp()
		End If

		If KeyHit(KEY_M) Then
			m_joint1.EnableMotor(Not m_joint1.IsMotorEnabled())
			m_joint1.GetBody2().WakeUp()
		End If

	End Method

	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		DrawString "Keys: (f) toggle friction, (m) toggle motor", 5, m_textLine

		m_textLine :+ 12

		Local torque:Float = m_joint1.GetMotorTorque()

		DrawString "Motor Torque = " + torque, 5, m_textLine

		m_textLine :+ 12

	End Method


End Type



