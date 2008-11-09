SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New PyramidStaticEdges.Create(), New TSettings)


Type PyramidStaticEdges Extends Test

	Method Create:PyramidStaticEdges()
	
		Init(10, 10)
		
		Local coords:Float[] = [ 50.0, 0.0, -50.0, 0.0 ]
		Local verts:b2Vec2[] = New b2Vec2[2]
		
		For Local i:Int = 0 Until 2
			verts[i] = New b2Vec2.Create(coords[i * 2], coords[i * 2 + 1])
		Next
		
	
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, 0.0))
		Local body:b2Body = m_world.CreateBody(bd)
		
		Local edgeDef:b2EdgeChainDef = New b2EdgeChainDef
		edgeDef.SetVertices(verts)
		edgeDef.SetIsALoop(False)
		body.CreateShape(edgeDef)
		
		
		
		Local sd:b2PolygonDef = New b2PolygonDef
		Local a:Float = 0.5
		sd.SetAsBox(a, a)
		sd.SetDensity(5)

		Local x:b2Vec2 = New b2Vec2.Create(-10.0, 1.0)
		Local y:b2Vec2 = New b2Vec2.Create()

		Local deltaX:b2Vec2 = New b2Vec2.Create(0.5625, 2.0)
		Local deltaY:b2Vec2 = New b2Vec2.Create(1.125, 0.0)

		For Local i:Int = 0 Until 25

			y.Copy(x)

			For Local j:Int = i Until 25
				Local bd:b2BodyDef = New b2BodyDef
				bd.SetPosition(y)
				Local body:b2Body = m_world.CreateBody(bd)
				body.CreateShape(sd)
				body.SetMassFromShapes()

				y.Add(deltaY)
			Next

			x.Add(deltaX)
		Next

		Return Self
	End Method

End Type


