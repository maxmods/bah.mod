SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New Gears.Create(), New TSettings)


Type Gears Extends Test

	Field m_joint1:b2RevoluteJoint
	Field m_joint2:b2RevoluteJoint
	Field m_joint3:b2PrismaticJoint
	Field m_joint4:b2GearJoint
	Field m_joint5:b2GearJoint
	
	Method Create:Gears()
	
		Init(20, 20)
				
		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		
		Local circle1:b2CircleDef = New b2CircleDef
		circle1.SetRadius(1)
		circle1.SetDensity(5)

		Local circle2:b2CircleDef = New b2CircleDef
		circle2.SetRadius(2)
		circle2.SetDensity(5)
		
		Local box:b2PolygonDef = New b2PolygonDef
		box.SetAsBox(0.5, 5.0)
		box.SetDensity(5)
		
		Local bd1:b2BodyDef = New b2BodyDef
		bd1.SetPosition(New b2Vec2.Create(-3.0, 12.0))
		Local body1:b2Body = m_world.CreateBody(bd1)
		body1.CreateShape(circle1)
		body1.SetMassFromShapes()
		
		
		Local jd1:b2RevoluteJointDef = New b2RevoluteJointDef
		jd1.SetBody1(ground)
		jd1.SetBody2(body1)
		jd1.SetLocalAnchor1(ground.GetLocalPoint(bd1.GetPosition()))
		jd1.SetLocalAnchor2(body1.GetLocalPoint(bd1.GetPosition()))
		jd1.SetReferenceAngle(body1.GetAngle() - ground.GetAngle())
		m_joint1 = b2RevoluteJoint(m_world.CreateJoint(jd1))
		
		Local bd2:b2BodyDef = New b2BodyDef
		bd2.SetPosition(New b2Vec2.Create(0.0, 12.0))
		Local body2:b2Body = m_world.CreateBody(bd2)
		body2.CreateShape(circle2)
		body2.SetMassFromShapes()
		
		Local jd2:b2RevoluteJointDef = New b2RevoluteJointDef
		jd2.Initialize(ground, body2, bd2.GetPosition())
		m_joint2 = b2RevoluteJoint(m_world.CreateJoint(jd2))
		
		Local bd3:b2BodyDef = New b2BodyDef
		bd3.SetPosition(New b2Vec2.Create(2.5, 12.0))
		Local body3:b2Body = m_world.CreateBody(bd3)
		body3.CreateShape(box)
		body3.SetMassFromShapes()
		
		Local jd3:b2PrismaticJointDef = New b2PrismaticJointDef
		jd3.Initialize(ground, body3, bd3.GetPosition(), New b2Vec2.Create(0.0, 1.0))
		jd3.SetLowerTranslation(-5.0)
		jd3.SetUpperTranslation(5.0)
		jd3.EnableLimit(True)
		
		m_joint3 = b2PrismaticJoint(m_world.CreateJoint(jd3))
		
		Local jd4:b2GearJointDef = New b2GearJointDef
		jd4.SetBody1(body1)
		jd4.SetBody2(body2)
		jd4.SetJoint1(m_joint1)
		jd4.SetJoint2(m_joint2)
		jd4.SetRatio(circle2.GetRadius() / circle1.GetRadius())
		m_joint4 = b2GearJoint(m_world.CreateJoint(jd4))
		
		Local jd5:b2GearJointDef = New b2GearJointDef
		jd5.SetBody1(body2)
		jd5.SetBody2(body3)
		jd5.SetJoint1(m_joint2)
		jd5.SetJoint2(m_joint3)
		jd5.SetRatio(-1.0 / circle2.GetRadius())
		m_joint5 = b2GearJoint(m_world.CreateJoint(jd5))		
				
		Return Self
	End Method
	
	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		Local ratio:Float = m_joint4.GetRatio()
		Local value:Float = m_joint1.GetJointAngle() + ratio * m_joint2.GetJointAngle()
		
		DrawString "theta1 + " + ratio + " * theta2 = " + value, 5, m_textLine

		m_textLine :+ 12

		ratio = m_joint5.GetRatio()
		value = m_joint2.GetJointAngle() + ratio * m_joint3.GetJointTranslation()

		DrawString "theta2 + " + ratio + " * delta = " + value, 5, m_textLine

		m_textLine :+ 12

	End Method


End Type




