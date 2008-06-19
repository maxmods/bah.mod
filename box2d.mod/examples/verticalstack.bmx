SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New VerticalStack.Create(), New TSettings)


Type VerticalStack Extends Test

	Field m_bullet:b2Body

	Method Create:VerticalStack()
	
		Init()
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsOrientedBox(50.0, 10.0, New b2Vec2.Create(0.0, -10.0), 0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPositionXY(0.0, -10.0)
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)

		sd.SetAsOrientedBox(0.1, 10.0, New b2Vec2.Create(20.0, 10.0), 0)
		ground.CreateShape(sd)


		Local xs:Float[] = [0.0, -10.0, -5.0, 5.0, 10.0]

		For Local j:Int = 0 Until 5

			sd = New b2PolygonDef
			sd.SetAsBox(0.5, 0.5)
			sd.SetDensity(1.0)
			sd.SetFriction(0.3)

			For Local i:Int = 0 Until 16
				bd = New b2BodyDef

				bd.SetPositionXY(xs[j], 0.752 + 1.4 * i)
				Local body:b2Body = m_world.CreateBody(bd)

				body.CreateShape(sd)
				body.SetMassFromShapes()
			Next
		Next

		m_bullet = Null

		Return Self
	End Method

	Method Keyboard()
		Super.Keyboard()
		
		If KeyHit(KEY_COMMA) Then

			If m_bullet Then
				m_world.DestroyBody(m_bullet)
				m_bullet = Null
			End If
			
			Local sd:b2CircleDef = New b2CircleDef
			sd.SetDensity(20.0)
			sd.SetRadius(0.25)
			sd.SetRestitution(0.05)

			Local bd:b2BodyDef = New b2BodyDef
			bd.SetIsBullet(True)
			bd.SetPositionXY(-31.0, -5.0)

			m_bullet = m_world.CreateBody(bd)
			m_bullet.CreateShape(sd)
			m_bullet.SetMassFromShapes()

			m_bullet.SetLinearVelocity(New b2Vec2.Create(400.0, 0.0))


		End If

	End Method

	Method DoStep(settings:TSettings)

		Super.DoStep(settings)

		DrawString "Press: (,) to launch a bullet.", 5, m_textLine

		m_textLine :+ 12

	End Method

End Type

