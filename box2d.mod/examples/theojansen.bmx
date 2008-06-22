SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New TheoJansen.Create(), New TSettings)


Type TheoJansen Extends Test

	Field m_offset:b2Vec2 = Vec2(0, 8)
	Field m_chassis:b2Body
	Field m_wheel:b2Body
	Field m_motorJoint:b2RevoluteJoint
	Field m_motorOn:Int = True
	Field m_motorSpeed:Float = 115
	
	Method Create:TheoJansen()

		Init(10, 10)
				
		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		Local pivot:b2Vec2 = Vec2(0.0, 0.8)
	
		
		sd.SetAsOrientedBox(0.5, 5.0, Vec2(-50.0, 15.0), 0.0)
		ground.CreateShape(sd)

		sd.SetAsOrientedBox(0.5, 5.0, Vec2(50.0, 15.0), 0.0)
		ground.CreateShape(sd)
		
		
		For Local i:Int = 0 Until 40
			Local sd:b2CircleDef = New b2CircleDef
			sd.SetDensity(1.0)
			sd.SetRadius(0.25)
		
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(-40.0 + 2.0 * i, 0.5)
		
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(sd)
			body.SetMassFromShapes()
		Next



		sd = New b2PolygonDef
		sd.SetDensity(1.0)
		sd.SetAsBox(2.5, 1.0)
		sd.GetFilter().SetGroupIndex(-1)
		bd = New b2BodyDef
		bd.SetPosition(pivot.Plus(m_offset))
		m_chassis = m_world.CreateBody(bd)
		m_chassis.CreateShape(sd)
		m_chassis.SetMassFromShapes()
		
		
		Local sd1:b2CircleDef = New b2CircleDef
		sd1.SetDensity(1.0)
		sd1.SetRadius(1.6)
		sd1.GetFilter().SetGroupIndex(-1)
		bd = New b2BodyDef
		bd.SetPosition(pivot.Plus(m_offset))
		m_wheel = m_world.CreateBody(bd)
		m_wheel.CreateShape(sd1)
		m_wheel.SetMassFromShapes()

		
		Local jd:b2RevoluteJointDef = New b2RevoluteJointDef
		jd.Initialize(m_wheel, m_chassis, pivot.Plus(m_offset))
		jd.SetCollideConnected(False)
		jd.SetMotorSpeed(m_motorSpeed)
		jd.SetMaxMotorTorque(400.0)
		jd.EnableMotor(m_motorOn)
		m_motorJoint = b2RevoluteJoint(m_world.CreateJoint(jd))

		Local wheelAnchor:b2Vec2
		
		wheelAnchor = pivot.Plus(Vec2(0.0, -0.8))

		CreateLeg(-1.0, wheelAnchor)
		CreateLeg(1.0, wheelAnchor)

		m_wheel.SetXForm(m_wheel.GetPosition(), 120.0)
		CreateLeg(-1.0, wheelAnchor)
		CreateLeg(1.0, wheelAnchor)

		m_wheel.SetXForm(m_wheel.GetPosition(), -120.0)
		CreateLeg(-1.0, wheelAnchor)
		CreateLeg(1.0, wheelAnchor)

						
		Return Self
	End Method
	
	Method CreateLeg(s:Float, wheelAnchor:b2Vec2)

		Local p1:b2Vec2 = Vec2(5.4 * s, -6.1)
		Local p2:b2Vec2 = Vec2(7.2 * s, -1.2)
		Local p3:b2Vec2 = Vec2(4.3 * s, -1.9)
		Local p4:b2Vec2 = Vec2(3.1 * s, 0.8)
		Local p5:b2Vec2 = Vec2(6.0 * s, 1.5)
		Local p6:b2Vec2 = Vec2(2.5 * s, 3.7)

		Local sd1:b2PolygonDef = New b2PolygonDef
		Local sd2:b2PolygonDef = New b2PolygonDef
		Local sd1Vertices:b2Vec2[] = New b2Vec2[3]
		Local sd2Vertices:b2Vec2[] = New b2Vec2[3]

		sd1.GetFilter().SetGroupIndex(-1)
		sd2.GetFilter().SetGroupIndex(-1)
		sd1.SetDensity(1.0)
		sd2.SetDensity(1.0)

		If s > 0.0 Then
			sd1Vertices[0] = p1
			sd1Vertices[1] = p2
			sd1Vertices[2] = p3

			sd2Vertices[0] = b2Vec2.ZERO
			sd2Vertices[1] = p5.Subtract(p4)
			sd2Vertices[2] = p6.Subtract(p4)
		Else
			sd1Vertices[0] = p1
			sd1Vertices[1] = p3
			sd1Vertices[2] = p2

			sd2Vertices[0] = b2Vec2.ZERO
			sd2Vertices[1] = p6.Subtract(p4)
			sd2Vertices[2] = p5.Subtract(p4)
		End If
		
		sd1.SetVertices(sd1Vertices)
		sd2.SetVertices(sd2Vertices)

		Local bd1:b2BodyDef = New b2BodyDef
		Local bd2:b2BodyDef = New b2BodyDef
		bd1.SetPosition(m_offset)
		bd2.SetPosition(p4.Plus(m_offset))

		bd1.SetAngularDamping(10.0)
		bd2.SetAngularDamping(10.0)

		Local body1:b2Body = m_world.CreateBody(bd1)
		Local body2:b2Body = m_world.CreateBody(bd2)

		body1.CreateShape(sd1)
		body2.CreateShape(sd2)

		body1.SetMassFromShapes()
		body2.SetMassFromShapes()

		Local djd:b2DistanceJointDef = New b2DistanceJointDef

		' Using a soft distance constraint can reduce some jitter.
		' It also makes the structure seem a bit more fluid by
		' acting like a suspension system.
		djd.SetDampingRatio(0.5)
		djd.SetFrequencyHz(10.0)

		djd.Initialize(body1, body2, p2.Plus(m_offset), p5.Plus(m_offset))
		m_world.CreateJoint(djd)

		djd.Initialize(body1, body2, p3.Plus(m_offset), p4.Plus(m_offset))
		m_world.CreateJoint(djd)

		djd.Initialize(body1, m_wheel, p3.Plus(m_offset), wheelAnchor.Plus(m_offset))
		m_world.CreateJoint(djd)

		djd.Initialize(body2, m_wheel, p6.Plus(m_offset), wheelAnchor.Plus(m_offset))
		m_world.CreateJoint(djd)

		Local rjd:b2RevoluteJointDef = New b2RevoluteJointDef

		rjd.Initialize(body2, m_chassis, p4.Plus(m_offset))
		m_world.CreateJoint(rjd)
	End Method

	Method Keyboard()
		Super.Keyboard()
	
		If KeyHit(KEY_A) Then
			m_chassis.WakeUp()
			m_motorJoint.SetMotorSpeed(-m_motorSpeed)
		End If

		If KeyHit(KEY_S) Then
			m_chassis.WakeUp()
			m_motorJoint.SetMotorSpeed(0.0)
		End If

		If KeyHit(KEY_D) Then
			m_chassis.WakeUp()
			m_motorJoint.SetMotorSpeed(m_motorSpeed)
		End If

		If KeyHit(KEY_M) Then
			m_chassis.WakeUp()
			m_motorJoint.EnableMotor(Not m_motorJoint.IsMotorEnabled())
		End If
		
	End Method
	
	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		DrawString "Keys: left = a, brake = s, right = d, toggle motor = m", 5, m_textLine

		m_textLine :+ 12

	End Method


End Type





