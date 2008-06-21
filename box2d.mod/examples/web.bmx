SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New Web.Create(), New TSettings)


Type Web Extends Test

	Field m_bodies:b2Body[] = New b2Body[4]
	Field m_joints:b2Joint[] = New b2Joint[8]

	
	Method Create:Web()
	
		Init(18, 18)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		
		sd = New b2PolygonDef
		sd.SetAsBox(0.5, 0.5)
		sd.SetDensity(5.0)
		sd.SetFriction(0.2)
		
		bd = New b2BodyDef
		
		bd.SetPositionXY(-5.0, 5.0)
		m_bodies[0] = m_world.CreateBody(bd)
		m_bodies[0].CreateShape(sd)
		m_bodies[0].SetMassFromShapes()
		
		bd.SetPositionXY(5.0, 5.0)
		m_bodies[1] = m_world.CreateBody(bd)
		m_bodies[1].CreateShape(sd)
		m_bodies[1].SetMassFromShapes()
		
		bd.SetPositionXY(5.0, 15.0)
		m_bodies[2] = m_world.CreateBody(bd)
		m_bodies[2].CreateShape(sd)
		m_bodies[2].SetMassFromShapes()
		
		bd.SetPositionXY(-5.0, 15.0)
		m_bodies[3] = m_world.CreateBody(bd)
		m_bodies[3].CreateShape(sd)
		m_bodies[3].SetMassFromShapes()
		
		Local jd:b2DistanceJointDef = New b2DistanceJointDef
		Local p1:b2Vec2 = New b2Vec2.Create()
		Local p2:b2Vec2 = New b2Vec2.Create()
		Local d:b2Vec2 = New b2Vec2.Create()
		
		jd.SetFrequencyHz(4.0)
		jd.SetDampingRatio(0.5)
		
		jd.SetBody1(ground)
		jd.SetBody2(m_bodies[0])
		jd.SetLocalAnchor1(Vec2(-10.0, 10.0))
		jd.SetLocalAnchor2(Vec2(-0.5, -0.5))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[0] = m_world.CreateJoint(jd)
		
		jd.SetBody1(ground)
		jd.SetBody2(m_bodies[1])
		jd.SetLocalAnchor1(Vec2(10.0, 10.0))
		jd.SetLocalAnchor2(Vec2(0.5, -0.5))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[1] = m_world.CreateJoint(jd)
		
		jd.SetBody1(ground)
		jd.SetBody2(m_bodies[2])
		jd.SetLocalAnchor1(Vec2(10.0, 30.0))
		jd.SetLocalAnchor2(Vec2(0.5, 0.5))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[2] = m_world.CreateJoint(jd)
		
		jd.SetBody1(ground)
		jd.SetBody2(m_bodies[3])
		jd.SetLocalAnchor1(Vec2(-10.0, 30.0))
		jd.SetLocalAnchor2(Vec2(-0.5, 0.5))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[3] = m_world.CreateJoint(jd)
		
		jd.SetBody1(m_bodies[0])
		jd.SetBody2(m_bodies[1])
		jd.SetLocalAnchor1(Vec2(0.5, 0.0))
		jd.SetLocalAnchor2(Vec2(-0.5, 0.0))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[4] = m_world.CreateJoint(jd)
		
		jd.SetBody1(m_bodies[1])
		jd.SetBody2(m_bodies[2])
		jd.SetLocalAnchor1(Vec2(0.0, 0.5))
		jd.SetLocalAnchor2(Vec2(0.0, -0.5))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[5] = m_world.CreateJoint(jd)
		
		jd.SetBody1(m_bodies[2])
		jd.SetBody2(m_bodies[3])
		jd.SetLocalAnchor1(Vec2(-0.5, 0.0))
		jd.SetLocalAnchor2(Vec2(0.5, 0.0))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[6] = m_world.CreateJoint(jd)
		
		jd.SetBody1(m_bodies[3])
		jd.SetBody2(m_bodies[0])
		jd.SetLocalAnchor1(Vec2(0.0, -0.5))
		jd.SetLocalAnchor2(Vec2(0.0, 0.5))
		p1 = jd.GetBody1().GetWorldPoint(jd.GetLocalAnchor1())
		p2 = jd.GetBody2().GetWorldPoint(jd.GetLocalAnchor2())
		d = p2.Subtract(p1)
		jd.SetLength(d.Length())
		m_joints[7] = m_world.CreateJoint(jd)
		
	
		Return Self
	End Method

	Method Keyboard()
		Super.Keyboard()
		
		If KeyHit(KEY_B) Then
			For Local i:Int = 0 Until 4
				If m_bodies[i] Then
					m_world.DestroyBody(m_bodies[i])
					m_bodies[i] = Null
					Exit
				End If
			Next
		End If

		If KeyHit(KEY_J) Then
			For Local i:Int = 0 Until 8
				If m_joints[i] Then
					m_world.DestroyJoint(m_joints[i])
					m_joints[i] = Null
					Exit
				End If
			Next
		End If

	End Method

	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		DrawString "This demonstrates a soft distance joint.", 5, m_textLine
		m_textLine :+ 12
		DrawString "Press: (b) to delete a body, (j) to delete a joint", 5, m_textLine
		m_textLine :+ 12

	End Method
	
	Method JointDestroyed(joint:b2Joint)
		For Local i:Int = 0 Until 8
			If m_joints[i] = joint Then
				m_joints[i] = Null
				Exit
			End If
		Next
	End Method

End Type


