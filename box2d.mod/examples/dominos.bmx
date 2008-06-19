SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend

Run(New Dominos.Create(), New TSettings)


Type Dominos Extends Test

	Method Create:Dominos()
	
		Init(22, 22)
	
		Local b1:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		 Local bd:b2BodyDef = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		 b1 = m_world.CreateBody(bd)
		 b1.CreateShape(sd)
		
		 sd = New b2PolygonDef
		 sd.SetAsBox(6.0, 0.25)
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(-1.5, 10.0))
		 Local ground:b2Body = m_world.CreateBody(bd)
		 ground.CreateShape(sd)
		
		 sd = New b2PolygonDef
		 sd.SetAsBox(0.1, 1.0)
		 sd.SetDensity(20.0)
		 sd.SetFriction(0.1)
		
		
		 For Local i:Int = 0 Until 10
		     bd = New b2BodyDef
		     bd.SetPosition(New b2Vec2.Create(-6.0 + 1.0 * i, 11.25))
		     Local body:b2Body = m_world.CreateBody(bd)
		     body.CreateShape(sd)
		     body.SetMassFromShapes()
		Next
		
		
		 sd = New b2PolygonDef
		 sd.SetAsOrientedBox(7.0, 0.25, b2Vec2.ZERO, 17.2)
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(1.0, 6.0))
		 ground = m_world.CreateBody(bd)
		 ground.CreateShape(sd)
		
		Local b2:b2Body
		
		 sd = New b2PolygonDef
		 sd.SetAsBox(0.25, 1.5)
		
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(-7.0, 4.0))
		 b2 = m_world.CreateBody(bd)
		 b2.CreateShape(sd)
		
		
		Local b3:b2Body

		 sd = New b2PolygonDef
		 sd.SetAsBox(6.0, 0.125)
		 sd.SetDensity(10.0)
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(-0.9, 1.0))
		 bd.SetAngle(-8.59)
		
		 b3 = m_world.CreateBody(bd)
		 b3.CreateShape(sd)
		 b3.SetMassFromShapes()
		
		Local jd:b2RevoluteJointDef = New b2RevoluteJointDef
		Local anchor:b2Vec2 = New b2Vec2.Create()
		
		anchor.Set(-2.0, 1.0)
		jd.Initialize(b1, b3, anchor)
		jd.SetCollideConnected(True)
		m_world.CreateJoint(jd)
		
		Local b4:b2Body
		
		 sd = New b2PolygonDef
		 sd.SetAsBox(0.25, 0.25)
		 sd.SetDensity(10.0)
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(-10.0, 15.0))
		 b4 = m_world.CreateBody(bd)
		 b4.CreateShape(sd)
		 b4.SetMassFromShapes()
		
		anchor.Set(-7.0, 15.0)
		jd.Initialize(b2, b4, anchor)
		m_world.CreateJoint(jd)
		
		Local b5:b2Body
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(6.5, 3.0))
		 b5 = m_world.CreateBody(bd)
		
		 sd = New b2PolygonDef
		 sd.SetDensity(10.0)
		 sd.SetFriction(0.1)
		
		 sd.SetAsOrientedBox(1.0, 0.1, New b2Vec2.Create(0.0, -0.9), 0.0)
		 b5.CreateShape(sd)
		
		 sd.SetAsOrientedBox(0.1, 1.0, New b2Vec2.Create(-0.9, 0.0), 0.0)
		 b5.CreateShape(sd)
		
		 sd.SetAsOrientedBox(0.1, 1.0, New b2Vec2.Create(0.9, 0.0), 0.0)
		 b5.CreateShape(sd)
		
		 b5.SetMassFromShapes()
		
		
		anchor.Set(6.0, 2.0)
		jd.Initialize(b1, b5, anchor)
		m_world.CreateJoint(jd)
		
		Local b6:b2Body
		
		 sd = New b2PolygonDef
		 sd.SetAsBox(1.0, 0.1)
		 sd.SetDensity(30.0)
		 sd.SetFriction(0.2)
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(6.5, 4.1))
		 b6 = m_world.CreateBody(bd)
		 b6.CreateShape(sd)
		 b6.SetMassFromShapes()
		
		
		anchor.Set(7.5, 4.0)
		jd.Initialize(b5, b6, anchor)
		m_world.CreateJoint(jd)
		
		Local b7:b2Body
		
		 sd = New b2PolygonDef
		 sd.SetAsBox(0.1, 1.0)
		 sd.SetDensity(10.0)
		
		 bd = New b2BodyDef
		 bd.SetPosition(New b2Vec2.Create(7.4, 1.0))
		
		 b7 = m_world.CreateBody(bd)
		 b7.CreateShape(sd)
		 b7.SetMassFromShapes()
		
		Local djd:b2DistanceJointDef = New b2DistanceJointDef
		djd.SetBody1(b3)
		djd.SetBody2(b7)
		djd.SetLocalAnchor1(New b2Vec2.Create(6.0, 0.0))
		djd.SetLocalAnchor2(New b2Vec2.Create(0.0, -1.0))
		Local d:b2Vec2 = djd.GetBody2().GetWorldPoint(djd.GetLocalAnchor2()).Subtract(djd.GetBody1().GetWorldPoint(djd.GetLocalAnchor1()))
		djd.SetLength(d.Length())
		m_world.CreateJoint(djd)
		

		 Local cd:b2CircleDef = New b2CircleDef
		 cd.SetRadius(0.2)
		 cd.SetDensity(10.0)
		
		 For Local i:Int = 0 Until 4
		     bd = New b2BodyDef
		     bd.SetPosition(New b2Vec2.Create(5.9 + 2.0 * cd.GetRadius() * i, 2.4))
		     Local body:b2Body = m_world.CreateBody(bd)
		     body.CreateShape(cd)
		     body.SetMassFromShapes()
		 Next

		Return Self
	End Method

End Type



