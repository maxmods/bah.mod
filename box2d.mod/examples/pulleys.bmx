SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New Pulleys.Create(), New TSettings)


Type Pulleys Extends Test

	Field m_joint1:b2PulleyJoint
	
	Method Create:Pulleys()
	
		Init(12, 12)
				
		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		
		Local a:Float = 2.0
		Local b:Float = 4.0
		Local y:Float = 16.0
		Local L:Float = 12.0
		
		sd = New b2PolygonDef
		sd.SetAsBox(a, b)
		sd.SetDensity(5.0)
		
		bd = New b2BodyDef
		
		bd.SetPosition(New b2Vec2.Create(-10.0, y))
		Local body1:b2Body = m_world.CreateBody(bd)
		body1.CreateShape(sd)
		body1.SetMassFromShapes()
		
		bd.SetPosition(New b2Vec2.Create(10.0, y))
		Local body2:b2Body = m_world.CreateBody(bd)
		body2.CreateShape(sd)
		body2.SetMassFromShapes()
		
		Local pulleyDef:b2PulleyJointDef = New b2PulleyJointDef
		Local anchor1:b2Vec2 = New b2Vec2.Create(-10.0, y + b)
		Local anchor2:b2Vec2 = New b2Vec2.Create(10.0, y + b)
		Local groundAnchor1:b2Vec2 = New b2Vec2.Create(-10.0, y + b + L)
		Local groundAnchor2:b2Vec2 = New b2Vec2.Create(10.0, y + b + L)
		pulleyDef.Initialize(body1, body2, groundAnchor1, groundAnchor2, anchor1, anchor2, 2.0)
		
		m_joint1 = b2PulleyJoint(m_world.CreateJoint(pulleyDef))
				
		Return Self
	End Method
	
	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		Local ratio:Float = m_joint1.GetRatio()
		Local L:Float = m_joint1.GetLength1() + ratio * m_joint1.GetLength2()
		
		DrawString "L1 + " + ratio + " * L2 = " + L, 5, m_textLine

		m_textLine :+ 12

	End Method


End Type





