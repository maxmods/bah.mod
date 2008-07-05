SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New SliderCrank.Create(), New TSettings)


Type SliderCrank Extends Test

	Field m_leftWheel:b2Body
	Field m_rightWheel:b2Body
	Field m_vehicle:b2Body
	Field m_leftJoint:b2RevoluteJoint
	Field m_rightJoint:b2RevoluteJoint

	
	Method Create:SliderCrank()
	
		Init(8, 8)
				
		' car body
		Local poly1:b2PolygonDef = New b2PolygonDef
		Local poly2:b2PolygonDef = New b2PolygonDef
		
		' bottom half
		Local vertices:b2Vec2[] = New b2Vec2[5]
		vertices[4] = Vec2(-2.2,-0.74)
		vertices[3] = Vec2(-2.2,0)
		vertices[2] = Vec2(1.0,0)
		vertices[1] = Vec2(2.2,-0.2)
		vertices[0] = Vec2(2.2,-0.74)
		poly1.SetVertices(vertices)
		poly1.GetFilter().SetGroupIndex(-1)
		
		poly1.SetDensity(20.0)
		poly1.SetFriction(0.68)
		poly1.GetFilter().SetGroupIndex(-1)
		
		' top half
		vertices = New b2Vec2[4]
		vertices[3] = Vec2(-1.7,0)
		vertices[2] = Vec2(-1.3,0.7)
		vertices[1] = Vec2(0.5,0.74)
		vertices[0] = Vec2(1.0,0)
		poly2.SetVertices(vertices)
		poly2.GetFilter().SetGroupIndex(-1)
		
		poly2.SetDensity(5.0)
		poly2.SetFriction(0.68)
		poly2.GetFilter().SetGroupIndex(-1)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPositionXY(-35.0, 2.8)
		
		m_vehicle = m_world.CreateBody(bd)
		m_vehicle.CreateShape(poly1)
		m_vehicle.CreateShape(poly2)
		m_vehicle.SetMassFromShapes()
	
		' vehicle wheels
		Local circ:b2CircleDef = New b2CircleDef
		circ.SetDensity(40.0)
		circ.SetRadius(0.38608)
		circ.SetFriction(0.8)
		circ.GetFilter().SetGroupIndex(-1)
		
		bd = New b2BodyDef
		bd.SetAllowSleep(False)
		bd.SetPositionXY(-33.8, 2.0)
		
		m_rightWheel = m_world.CreateBody(bd)
		m_rightWheel.CreateShape(circ)
		m_rightWheel.SetMassFromShapes()
		
		bd.SetPositionXY(-36.2, 2.0)
		m_leftWheel = m_world.CreateBody(bd)
		m_leftWheel.CreateShape(circ)
		m_leftWheel.SetMassFromShapes()
		

		' join wheels to chassis
		Local anchor:b2Vec2 = New b2Vec2.Create()
		Local jd:b2RevoluteJointDef= New b2RevoluteJointDef
		jd.Initialize(m_vehicle, m_leftWheel, m_leftWheel.GetWorldCenter())
		jd.SetCollideConnected(False)
		jd.EnableMotor(True)
		jd.SetMaxMotorTorque(10.0)
		jd.SetMotorSpeed(0.0)
		m_leftJoint = b2RevoluteJoint(m_world.CreateJoint(jd))
		
		jd.Initialize(m_vehicle, m_rightWheel, m_rightWheel.GetWorldCenter())
		jd.SetCollideConnected(False)
		m_rightJoint = b2RevoluteJoint(m_world.CreateJoint(jd))
		
		
		' ground
		Local box:b2PolygonDef = New b2PolygonDef
		box.SetAsBox(19.5, 0.5)
		box.SetFriction(0.62)
		
		bd = New b2BodyDef 
		bd.SetPositionXY(-25.0, 1.0)
		
		Local ground:b2Body = m_world.CreateBody(bd)
		ground.CreateShape(box)
		
		
		' more ground
		box = New b2PolygonDef
		bd = New b2BodyDef 
		
		box.SetAsOrientedBox(9.5, 0.5, b2Vec2.ZERO, 18)
		box.SetFriction(0.62)
		bd.SetPositionXY(27.0 - 30.0, 3.1)
		
		ground = m_world.CreateBody(bd)
		ground.CreateShape(box)
		
		
		' more ground
		box = New b2PolygonDef
		bd = New b2BodyDef 
		
		box.SetAsOrientedBox(9.5, 0.5, b2Vec2.ZERO, -18)
		box.SetFriction(0.62)
		bd.SetPositionXY(55.0 - 30.0, 3.1)
		
		ground = m_world.CreateBody(bd)
		ground.CreateShape(box)
		
		
		' more ground
		box = New b2PolygonDef
		bd = New b2BodyDef 
		
		box.SetAsOrientedBox(9.5, 0.5, b2Vec2.ZERO, 5.4)
		box.SetFriction(0.62)
		bd.SetPositionXY(41.0, 2.0)
		
		ground = m_world.CreateBody(bd)
		ground.CreateShape(box)
		
		
		' more ground
		box = New b2PolygonDef
		bd = New b2BodyDef 
		
		box.SetAsOrientedBox(5.0, 0.5, b2Vec2.ZERO, 27)
		box.SetFriction(0.62)
		bd.SetPositionXY(50.0, 4.0)
		
		ground = m_world.CreateBody(bd)
		ground.CreateShape(box)
		
		
		' more ground
		box = New b2PolygonDef
		bd = New b2BodyDef 
		
		box.SetAsBox(20.0, 0.5)
		box.SetFriction(0.62)
		bd.SetPositionXY(85.0, 2.0)
		
		ground = m_world.CreateBody(bd)
		ground.CreateShape(box)
		
				
		Return Self
	End Method
	
	Method Keyboard()
		Super.Keyboard()
		
		If KeyHit(KEY_A) Then
			m_leftJoint.SetMaxMotorTorque(800.0)
			m_leftJoint.SetMotorSpeed(687.549354)
		End If

		If KeyHit(KEY_S) Then
			m_leftJoint.SetMaxMotorTorque(100.0)
			m_leftJoint.SetMotorSpeed(0.0)
		End If

		If KeyHit(KEY_D) Then
			m_leftJoint.SetMaxMotorTorque(1200.0)
			m_leftJoint.SetMotorSpeed(-2062.64806)
		End If

	End Method

	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		DrawString "Keys: left = a, brake = s, right = d", 5, m_textLine

		m_textLine :+ 12

	End Method


End Type




