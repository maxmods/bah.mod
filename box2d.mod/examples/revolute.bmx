SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend

Run(New Revolute.Create(), New TSettings)


Type Revolute Extends Test

	Field m_joint:b2RevoluteJoint
	

	Method Create:Revolute()
	
		Init(14, 14)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		Local sdc:b2CircleDef = New b2CircleDef
		sdc.SetRadius(0.5)
		sdc.SetDensity(5.0)
		
		bd = New b2BodyDef
		
		Local rjd:b2RevoluteJointDef = New b2RevoluteJointDef
		
		bd.SetPositionXY(0.0, 20.0)
		Local body:b2Body = m_world.CreateBody(bd)
		body.CreateShape(sdc)
		body.SetMassFromShapes()
		
		Local w:Float = 5729.57795
		Local wl:Float = 100
		body.SetAngularVelocity(w)
		body.SetLinearVelocity(Vec2(-8.0 * wl, 0.0))
		
		rjd.Initialize(ground, body, Vec2(0.0, 12.0))
		rjd.SetMotorSpeed(180)
		rjd.SetMaxMotorTorque(10000.0)
		rjd.EnableMotor(False)
		rjd.SetLowerAngle(-45)
		rjd.SetUpperAngle(90)
		rjd.EnableLimit(True)
		rjd.SetCollideConnected(True)
		
		m_joint = b2RevoluteJoint(m_world.CreateJoint(rjd))		
		Return Self
		
	End Method

	Method Keyboard()
		Super.Keyboard()
		
		If KeyHit(KEY_L) Then
			m_joint.EnableLimit(m_joint.IsLimitEnabled())
		End If

		If KeyHit(KEY_S) Then
			m_joint.EnableMotor(False)
		End If

	End Method

	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		DrawString "Keys: (l) limits, (a) left, (s) off, (d) right", 5, m_textLine

		m_textLine :+ 12

	End Method

End Type

