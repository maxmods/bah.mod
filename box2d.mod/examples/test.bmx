'
' Part of the examples framework... imported by the examples.
'
SuperStrict

Import BRL.FreeTypeFont
Import "render.bmx"

Global viewZoom:Float = 1
Global tx:Int, ty:Int, tw:Int = 800, th:Int = 600
Global viewCenter:b2Vec2 = New b2Vec2.Create(0.0, 0.0)
Global mouseState:Int = False

Function Run(tst:Test, settings:TSettings)

	detailFont = LoadImageFont(CurrentDir() + "/04B_03__.ttf", 8)

	SetOrigin(400,500)
	
	While Not KeyDown(KEY_ESCAPE)
	
		Cls
		
		Mouse(tst)
		
		tst.Keyboard()
		
		tst.DoStep(settings)
		
		Flip
	
	Wend



End Function

Function Mouse(tst:Test)

	Local x:Int = MouseX()
	Local y:Int = MouseY()

	' Use the mouse to move things around.
	If MouseDown(1) Then

		Local p:b2Vec2 = ConvertScreenToWorld(x, y)

		' button pressed
		If Not mouseState Then
		
			tst.MouseDown(p)
		
		End If

		mouseState = True
		
		tst.MouseMove(p)
	Else
		If mouseState Then

			Local p:b2Vec2 = ConvertScreenToWorld(x, y)

			tst.MouseUp(p)
		End If
		
		mouseState = False
	End If
	
End Function



Type Test
	Const k_maxContactPoints:Int = 2048

	Field m_worldAABB:b2AABB = New b2AABB.Create()
	Field m_world:b2World
	Field m_debugDraw:b2DebugDraw = New debugDraw
	Field m_textLine:Int
	Field m_mouseWorld:b2Vec2
	Field m_mouseJoint:b2MouseJoint
	
	Field m_points:ContactPoint[] = New ContactPoint[k_maxContactPoints]
	Field m_contactListener:ContactListener = New ContactListener
	Field m_destructionListener:DestructionListener = New DestructionListener
	Field m_pointCount:Int
	
	Field _settings:TSettings
	
	Method Init(xs:Float = 0, ys:Float = 0)
		If xs Then
			xScale = xs
		End If
		If ys Then
			yScale = ys
		End If
		m_worldAABB.SetLowerBound(New b2Vec2.Create(-200.0, -100.0))
		m_worldAABB.SetUpperBound(New b2Vec2.Create(200.0, 200.0))
	
		Local gravity:b2Vec2 = New b2Vec2.Create(0.0, -10.0)
		m_world = New b2World.Create(m_worldAABB, gravity, True)

		m_contactListener._test = Self
		m_destructionListener._test = Self

		m_world.SetDebugDraw(m_debugDraw)
		m_world.SetContactListener(m_contactListener)
		m_world.SetDestructionListener(m_destructionListener)
		
	End Method

	Method DoStep(settings:TSettings)
	
		_settings = settings
	
		m_textLine = 10

		Local timeStep:Float
		If settings.hz > 0.0 Then
			timeStep = 1.0 / settings.hz
		Else
			timeStep = 0.0
		End If
	
		If settings.pause Then
			If settings.singleStep Then
				settings.singleStep = 0
			Else
				timeStep = 0.0
			End If
	
			'DrawString(5, m_textLine, "****PAUSED****")
			'm_textLine :+ 15
		End If

		Local flags:Int = 0
		flags :+ settings.drawShapes			* b2DebugDraw.e_shapeBit
		flags :+ settings.drawJoints			* b2DebugDraw.e_jointBit
		flags :+ settings.drawCoreShapes		* b2DebugDraw.e_coreShapeBit
		flags :+ settings.drawControllers	* b2DebugDraw.e_controllerBit
		flags :+ settings.drawAABBs			* b2DebugDraw.e_aabbBit
		flags :+ settings.drawOBBs				* b2DebugDraw.e_obbBit
		flags :+ settings.drawPairs			* b2DebugDraw.e_pairBit
		flags :+ settings.drawCOMs				* b2DebugDraw.e_centerOfMassBit
		m_debugDraw.SetFlags(flags)

		m_world.SetWarmStarting(settings.enableWarmStarting)
'		m_world.SetPositionCorrection(settings.enablePositionCorrection)
		m_world.SetContinuousPhysics(settings.enableTOI)

		m_pointCount = 0

		m_world.DoStep(timeStep, settings.velocityIterations, settings.positionIterations)
	
		m_world.Validate()


		If m_mouseJoint Then

			Local body:b2Body = m_mouseJoint.GetBody2()
			Local p1:b2Vec2 = body.GetWorldPoint(m_mouseJoint.GetLocalAnchor())
			Local p2:b2Vec2 = m_mouseJoint.GetTarget()
			
			debugDraw(m_debugDraw).DrawPoint(p1, 4.0, b2Color.Set(0, 255, 0))
			debugDraw(m_debugDraw).DrawPoint(p2, 4.0, b2Color.Set(0, 255, 0))
		
			m_debugDraw.DrawSegment(p1, p2, b2Color.Set(204, 204, 204))
		End If

		
		If settings.drawContactPoints Then

			Const k_axisScale:Int = 0.3
	
			For Local i:Int = 0 Until m_pointCount

				Local point:ContactPoint = m_points[i]
	
				If point.state = 0 Then
					' Add
					debugDraw(m_debugDraw).DrawPoint(point.position, 5.0, b2Color.Set(76, 242, 76))
				Else If point.state = 1 Then
					' Persist
					debugDraw(m_debugDraw).DrawPoint(point.position, 3.0, b2Color.Set(76, 76, 242))
				Else
					' Remove
					debugDraw(m_debugDraw).DrawPoint(point.position, 5.0, b2Color.Set(242, 76, 76))
				End If
	Rem
				If (settings->drawContactNormals == 1)
				{
					b2Vec2 p1 = point->position;
					b2Vec2 p2 = p1 + k_axisScale * point->normal;
					m_debugDraw.DrawSegment(p1, p2, b2Color(0.4f, 0.9f, 0.4f));
				}
				Else If (settings->drawContactForces == 1)
				{
					//b2Vec2 p1 = point->position;
					//b2Vec2 p2 = p1 + k_forceScale * point->normalForce * point->normal;
					//DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
				}
	
				If (settings->drawFrictionForces == 1)
				{
					//b2Vec2 tangent = b2Cross(point->normal, 1.0f);
					//b2Vec2 p1 = point->position;
					//b2Vec2 p2 = p1 + k_forceScale * point->tangentForce * tangent;
					//DrawSegment(p1, p2, b2Color(0.9f, 0.9f, 0.3f));
				}
				End Rem
			Next
		End If
	
	End Method
	
	Method Keyboard()
		If KeyHit(KEY_F) Then
			debugDraw(m_debugDraw).showDetails = Not debugDraw(m_debugDraw).showDetails
		End If
		
		If KeyHit(KEY_C) Then
			_settings.drawContactPoints = Not _settings.drawContactPoints
		End If
	End Method

	Method DrawString(text:String, x:Int, y:Int)
		SetColor(255, 255, 255)
		
		Local ox:Float, oy:Float
		GetOrigin(ox, oy)
		
		SetOrigin 0,0
		
		DrawText text, x, y
		
		SetOrigin ox, oy
	End Method
	
	Method ShiftMouseDown(p:b2Vec2)
	End Method
	
	Method MouseDown(p:b2Vec2)
		m_mouseWorld  = p

		If m_mouseJoint Then
			Return
		End If
		
		' Make a small box.
		Local aabb:b2AABB = New b2AABB.Create()
		Local d:b2Vec2 = New b2Vec2.Create(0.001, 0.001)
		aabb.SetLowerBound(p.Subtract(d))
		aabb.SetUpperBound(p.Plus(d))
		
		' Query the world for overlapping shapes.
		Local k_maxCount:Int = 10
		Local shapes:b2Shape[] = New b2Shape[k_maxCount]
		Local count:Int = m_world.Query(aabb, shapes)
		Local body:b2Body
		For Local i:Int = 0 Until count
			Local shapeBody:b2Body = shapes[i].GetBody()
			If Not shapeBody.IsStatic() And shapeBody.GetMass() > 0.0 Then
				Local inside:Int = shapes[i].TestPoint(shapeBody.GetXForm(), p)
				If inside Then
					body = shapes[i].GetBody()
					Exit
				End If
			End If
		Next


		If body Then
			Local md:b2MouseJointDef = New b2MouseJointDef
			md.SetBody1(m_world.GetGroundBody())
			md.SetBody2(body)
			md.SetTarget(p)
			md.SetMaxForce(1000.0 * body.GetMass())
		
			m_mouseJoint = b2MouseJoint(m_world.CreateJoint(md))
			body.WakeUp()
		End If

		
	End Method
	
	Method MouseUp(p:b2Vec2)
		If m_mouseJoint Then
			m_world.DestroyJoint(m_mouseJoint)
			m_mouseJoint = Null
		End If
	End Method

	Method MouseMove(p:b2Vec2)
		m_mouseWorld = p

		If debugDraw(m_debugDraw).showDetails Then
			SetImageFont(detailFont)
			SetColor(255, 255, 255)
			DrawText StripPlaces(p.X()) + "," + StripPlaces(p.Y()), MouseX() - 400, MouseY() - 500 - 10
			SetImageFont(Null)
		End If
		
		If m_mouseJoint Then
			m_mouseJoint.SetTarget(p)
		End If
	End Method
	
	Method JointDestroyed(joint:b2Joint)
	End Method
	
End Type


Type TSettings

	Field hz:Float = 60
	Field velocityIterations:Int = 10
	Field positionIterations:Int = 8
	Field drawShapes:Int = 1
	Field drawJoints:Int = 1
	Field drawControllers:Int = 1
	Field drawCoreShapes:Int
	Field drawAABBs:Int
	Field drawOBBs:Int
	Field drawPairs:Int
	Field drawContactPoints:Int
	Field drawContactNormals:Int
	Field drawContactForces:Int
	Field drawFrictionForces:Int
	Field drawCOMs:Int
	Field drawStats:Int
	Field enableWarmStarting:Int = 1
	Field enableTOI:Int = 1
	Field pause:Int
	Field singleStep:Int


End Type

Type ContactPoint

	Const e_contactAdded:Int = 0
	Const e_contactPersisted:Int = 1
	Const e_contactRemoved:Int = 2

	Field shape1:b2Shape
	Field shape2:b2Shape
	Field normal:b2Vec2 
	Field position:b2Vec2 
	Field velocity:b2Vec2 
	Field id:Int
	Field state:Int
End Type

Type ContactListener Extends b2ContactListener

	Field _test:Test
	
	Method Add(point:b2ContactPoint)
		If _test.m_pointCount = Test.k_maxContactPoints Then
			Return
		End If
	
		Local cp:ContactPoint = _test.m_points[_test.m_pointCount]
		If Not cp Then
			cp = New ContactPoint
			_test.m_points[_test.m_pointCount] = cp
		End If
		
		cp.shape1 = point.GetShape1()
		cp.shape2 = point.GetShape2()
		cp.position = point.GetPosition()
		cp.normal = point.GetNormal()
'		cp.id = point->id
		cp.state = ContactPoint.e_contactAdded
	
		_test.m_pointCount :+ 1
	End Method
	
	Method Persist(point:b2ContactPoint)
		If _test.m_pointCount = Test.k_maxContactPoints Then
			Return
		End If
	
		Local cp:ContactPoint = _test.m_points[_test.m_pointCount]
		If Not cp Then
			cp = New ContactPoint
			_test.m_points[_test.m_pointCount] = cp
		End If
		cp.shape1 = point.GetShape1()
		cp.shape2 = point.GetShape2()
		cp.position = point.GetPosition()
		cp.normal = point.GetNormal()
'		cp.id = point->id
		cp.state = ContactPoint.e_contactPersisted
	
		_test.m_pointCount :+ 1
	End Method

	Method Remove(point:b2ContactPoint)
		If _test.m_pointCount = Test.k_maxContactPoints Then
			Return
		End If
	
		Local cp:ContactPoint = _test.m_points[_test.m_pointCount]
		If Not cp Then
			cp = New ContactPoint
			_test.m_points[_test.m_pointCount] = cp
		End If
		cp.shape1 = point.GetShape1()
		cp.shape2 = point.GetShape2()
		cp.position = point.GetPosition()
		cp.normal = point.GetNormal()
'		cp.id = point->id
		cp.state = ContactPoint.e_contactRemoved
	
		_test.m_pointCount :+ 1
	End Method

End Type

Type DestructionListener Extends b2DestructionListener


	Field _test:Test

	Method SayGoodbyeJoint(joint:b2Joint)
		If _test.m_mouseJoint = joint
			_test.m_mouseJoint = Null
		Else
			_test.JointDestroyed(joint)
		End If
	End Method

End Type

Function ConvertScreenToWorld:b2Vec2(x:Int, y:Int)
	Local ox:Float, oy:Float
	GetOrigin(ox, oy)
	
	x :- ox
	y :- oy

	Local wx:Float = x / xScale
	Local wy:Float = y / yScale
	
	Return Vec2(wx, -wy)
	'Local u:Float = x / Float(tw)
	'Local v:Float = (th - y) / Float(th)

	'Local ratio:Float = tw / Float(th)
	'Local extents:b2Vec2 = New b2Vec2.Create(ratio * 38, 50.0)
	'extents.Multiply(viewZoom)

	'Local _lower:b2Vec2 = viewCenter.Subtract(extents)
	'Local _upper:b2Vec2 = viewCenter.Plus(extents)

	'Return New b2Vec2.Create((1.0 - u) * _lower.X() + u * _upper.X(), (1.0 - v) * _lower.Y() + v * _upper.Y())
End Function

