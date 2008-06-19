SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New VaryingRestitution.Create(), New TSettings)


Type VaryingRestitution Extends Test

	Method Create:VaryingRestitution()
	
		Init(20, 20)
	
		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)


		Local cd:b2CircleDef = New b2CircleDef
		cd.SetRadius(1.0)
		cd.SetDensity(1.0)

		Local restitution:Float[] = [0.0, 0.1, 0.3, 0.5, 0.75, 0.9, 1.0]

		For Local i:Int = 0 Until 7
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(-10.0 + 3.0 * i, 20.0)

			Local body:b2Body = m_world.CreateBody(bd)

			cd.SetRestitution(restitution[i])
			body.CreateShape(cd)
			body.SetMassFromShapes()
		Next

		Return Self
	End Method

End Type


