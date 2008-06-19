SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New MotorsAndLimits.Create(), New TSettings)


Type MotorsAndLimits Extends Test

	Field m_joint1:b2RevoluteJoint
	Field m_joint2:b2RevoluteJoint
	Field m_joint3:b2PrismaticJoint

	Method Create:MotorsAndLimits()
	
		Init(20, 20)

		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)

		sd = New b2PolygonDef
		sd.SetAsBox(2.0, 0.5)
		sd.SetDensity(5.0)
		sd.SetFriction(0.05)

		bd = New b2BodyDef
		Local rjd:b2RevoluteJointDef = New b2RevoluteJointDef
		
		Local body:b2Body

		Local prevBody:b2Body = ground
		Const y:Float = 8.0

		bd.SetPosition(New b2Vec2.Create(3.0, y))
		body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()

		rjd.Initialize(prevBody, body, New b2Vec2.Create(0.0, y))
		rjd.SetMotorSpeed(180)
		rjd.SetMaxMotorTorque(10000.0)
		rjd.EnableMotor(True)

		m_joint1 = b2RevoluteJoint(m_world.CreateJoint(rjd))

		prevBody = body

		bd.SetPosition(New b2Vec2.Create(9.0, y))
		body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()

		rjd.Initialize(prevBody, body, New b2Vec2.Create(6.0, y))
		rjd.SetMotorSpeed(90)
		rjd.SetMaxMotorTorque(2000.0)
		rjd.EnableMotor(True)
		rjd.SetLowerAngle(-90)
		rjd.SetUpperAngle(90)
		rjd.EnableLimit(True)

		m_joint2 = b2RevoluteJoint(m_world.CreateJoint(rjd))

		bd.SetPosition(New b2Vec2.Create(-10.0, 10.0))
		bd.SetAngle(90)
		body = m_world.CreateBody(bd)
		body.CreateShape(sd)
		body.SetMassFromShapes()

		Local pjd:b2PrismaticJointDef = New b2PrismaticJointDef
		pjd.Initialize(ground, body, New b2Vec2.Create(-10.0, 10.0), New b2Vec2.Create(1.0, 0.0))
		pjd.SetMotorSpeed(572.957825)
		pjd.SetMaxMotorForce(1000.0)
		pjd.EnableMotor(True)
		pjd.SetLowerTranslation(0.0)
		pjd.SetUpperTranslation(20.0)
		pjd.EnableLimit(True)

		m_joint3 = b2PrismaticJoint(m_world.CreateJoint(pjd))

		Return Self
	End Method

	Method Keyboard()
		Super.Keyboard()
	
		If KeyHit(KEY_L) Then
			m_joint2.EnableLimit(Not m_joint2.IsLimitEnabled())
			m_joint3.EnableLimit(Not m_joint3.IsLimitEnabled())
			m_joint2.GetBody1().WakeUp()
			m_joint3.GetBody2().WakeUp()
		End If

		If KeyHit(KEY_M) Then
			m_joint1.EnableMotor(Not m_joint1.IsMotorEnabled())
			m_joint2.EnableMotor(Not m_joint2.IsMotorEnabled())
			m_joint3.EnableMotor(Not m_joint3.IsMotorEnabled())
			m_joint2.GetBody1().WakeUp()
			m_joint3.GetBody2().WakeUp()
		End If

		If KeyHit(KEY_P) Then
			m_joint3.GetBody2().WakeUp()
			m_joint3.SetMotorSpeed(-m_joint3.GetMotorSpeed())
		End If
		
	End Method

	Method DoStep(settings:TSettings)
		Super.DoStep(settings)

		DrawString("Keys: (L) limits, (M) motors, (P) prismatic speed", 5, m_textLine)

		m_textLine :+ 12

		Local torque1:Float = m_joint1.GetMotorTorque()

		Local torque2:Float = m_joint2.GetMotorTorque()

		Local force3:Float = m_joint3.GetMotorForce()

		DrawString("Motor Torque = " + torque1 + ", " + torque2 + " : Motor Force = " + force3, 5, m_textLine)

		m_textLine :+ 12

	End Method
	
End Type



