SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New CollisionFiltering.Create(), New TSettings)


Type CollisionFiltering Extends Test

	' This is a test of collision filtering.
	' There is a triangle, a box, and a circle.
	' There are 6 shapes. 3 large and 3 small.
	' The 3 small ones always collide.
	' The 3 large ones never collide.
	' The boxes don't collide with triangles (except if both are small).
	Const k_smallGroup:Short = 1
	Const k_largeGroup:Short = -1
	
	Const k_defaultCategory:Short = $0001:Short
	Const k_triangleCategory:Short = $0002:Short
	Const k_boxCategory:Short = $0004:Short
	Const k_circleCategory:Short = $0008:Short

	Const k_triangleMask:Short = $FFFF:Short
	Const k_boxMask:Short = $FFFF:Short ~ k_triangleCategory
	Const k_circleMask:Short = $FFFF:Short

	Method Create:CollisionFiltering()

		Init(20, 20)
		m_world.SetFilter(New myfilter)
	
		Local ground:b2Body
		
		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)

		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)
		
		
		' Small triangle
		Local triangleShapeDef:b2PolygonDef = New b2PolygonDef

		Local vertices:b2Vec2[] = New b2Vec2[3]
		vertices[0] = Vec2(-1.0, 0.0)
		vertices[1] = Vec2(1.0, 0.0)
		vertices[2] = Vec2(0.0, 2.0)

		triangleShapeDef.SetVertices(vertices)
		triangleShapeDef.SetDensity(1.0)

		' modify the def's filter
		triangleShapeDef.GetFilter().SetGroupIndex(k_smallGroup)
		triangleShapeDef.GetFilter().SetCategoryBits(k_triangleCategory)
		triangleShapeDef.GetFilter().SetMaskBits(k_triangleMask)

		Local triangleBodyDef:b2BodyDef = New b2BodyDef
		triangleBodyDef.SetPositionXY(-5.0, 2.0)

		Local body1: b2Body = m_world.CreateBody(triangleBodyDef)
		body1.CreateShape(triangleShapeDef)
		body1.SetMassFromShapes()


		' Large triangle (recycle definitions)
		vertices[0].Multiply(2.0)
		vertices[1].Multiply(2.0)
		vertices[2].Multiply(2.0)
		triangleShapeDef.SetVertices(vertices)
		
		triangleShapeDef.GetFilter().SetGroupIndex(k_largeGroup)
		triangleBodyDef.SetPositionXY(-5, 6.0)
		triangleBodyDef.SetFixedRotation(True) ' look at me!

		Local body2:b2Body = m_world.CreateBody(triangleBodyDef)
		body2.CreateShape(triangleShapeDef)
		body2.SetMassFromShapes()

		
		' Small box
		Local boxShapeDef:b2PolygonDef = New b2PolygonDef
		boxShapeDef.SetAsBox(1.0, 0.5)
		boxShapeDef.SetDensity(1.0)

		' create our own filter object
		Local filter:b2FilterData = New b2FilterData
		filter.SetGroupIndex(k_smallGroup)
		filter.SetCategoryBits(k_boxCategory)
		filter.SetMaskBits(k_boxMask)

		boxShapeDef.SetFilter(filter)
		
		Local boxBodyDef:b2BodyDef = New b2BodyDef
		boxBodyDef.SetPositionXY(0.0, 2.0)

		Local body3:b2Body = m_world.CreateBody(boxBodyDef)
		body3.CreateShape(boxShapeDef)
		body3.SetMassFromShapes()
		
		
		' Large box (recycle definitions)
		boxShapeDef.SetAsBox(2.0, 1.0)
		boxShapeDef.GetFilter().SetGroupIndex(k_largeGroup)
		boxBodyDef.SetPositionXY(0.0, 6.0)

		Local body4:b2Body = m_world.CreateBody(boxBodyDef)
		body4.CreateShape(boxShapeDef)
		body4.SetMassFromShapes()

		
		' Small circle
		Local circleShapeDef:b2CircleDef = New b2CircleDef
		circleShapeDef.SetRadius(1.0)
		circleShapeDef.SetDensity(1.0)

		filter = New b2FilterData
		filter.SetGroupIndex(k_smallGroup)
		filter.SetCategoryBits(k_circleCategory)
		filter.SetMaskBits(k_circleMask)
		
		circleShapeDef.SetFilter(filter)

		Local circleBodyDef:b2BodyDef = New b2BodyDef
		circleBodyDef.SetPositionXY(5.0, 2.0)
		
		Local body5:b2Body = m_world.CreateBody(circleBodyDef)
		body5.CreateShape(circleShapeDef)
		body5.SetMassFromShapes()

		
		' Large circle
		circleShapeDef.SetRadius(circleShapeDef.GetRadius() * 2.0)
		circleShapeDef.GetFilter().SetGroupIndex(k_largeGroup)
		circleBodyDef.SetPositionXY(5.0, 6.0)

		Local body6:b2Body = m_world.CreateBody(circleBodyDef)
		body6.CreateShape(circleShapeDef)
		body6.SetMassFromShapes()
		
		Return Self
	End Method

	Method Keyboard()
		Super.Keyboard()
	
		If KeyHit(KEY_SPACE) Then
			' Small box
			Local boxShapeDef:b2PolygonDef = New b2PolygonDef
			boxShapeDef.SetAsBox(1.0, 0.5)
			boxShapeDef.SetDensity(1.0)
	
			Local boxBodyDef:b2BodyDef = New b2BodyDef
			boxBodyDef.SetPosition(ConvertScreenToWorld(MouseX(), MouseY()))
	
			Local body3:b2Body = m_world.CreateBody(boxBodyDef)
			body3.CreateShape(boxShapeDef)
			body3.SetMassFromShapes()

		End If
		
	End Method

End Type

Type myfilter Extends b2ContactFilter
	Method ShouldCollide:Int(shape1:b2Shape, shape2:b2Shape)

		Local filter1:b2FilterData = shape1.GetFilterData()
		Local filter2:b2FilterData = shape2.GetFilterData()

		If filter1.GetGroupIndex() = filter2.GetGroupIndex() And filter1.GetGroupIndex() <> 0
 			Return filter1.GetGroupIndex() < $7FFF
		End If
	
		Local collide:Int = (filter1.GetMaskBits() & filter2.GetCategoryBits()) <> 0 And (filter1.GetCategoryBits() & filter2.GetMaskBits()) <> 0
		Return collide

	End Method
End Type

