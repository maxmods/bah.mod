SuperStrict

Import "render.bmx"

Global viewZoom:Float = 1
Global tx:Int, ty:Int, tw:Int = 800, th:Int = 600
Global viewCenter:b2Vec2 = New b2Vec2.Create(0.0, 20.0)
Global mouseState:Int = False

Function Run(tst:Test, settings:TSettings)

	SetOrigin(400,500)
	'Local ratio:Float = 800 / 600.0
	'Local ex:Float = 25 * ratio
	'Local ey:Float = 25

	'SetScale 10, 2
	
	'gluOrtho2D(0 - ex, 0 + ex,  20 - ey, 20 + ey)
	
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

	Field m_worldAABB:b2AABB = New b2AABB.Create()
	Field m_world:b2World
	Field m_debugDraw:b2DebugDraw = New debugDraw
	Field m_textLine:Int
	Field m_mouseWorld:b2Vec2
	Field m_mouseJoint:b2MouseJoint

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

		m_world.SetDebugDraw(m_debugDraw)
		
	End Method

	Method DoStep(settings:TSettings)
	
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
		flags :+ settings.drawAABBs			* b2DebugDraw.e_aabbBit
		flags :+ settings.drawOBBs				* b2DebugDraw.e_obbBit
		flags :+ settings.drawPairs			* b2DebugDraw.e_pairBit
		flags :+ settings.drawCOMs				* b2DebugDraw.e_centerOfMassBit
		m_debugDraw.SetFlags(flags)

		m_world.SetWarmStarting(settings.enableWarmStarting)
		m_world.SetPositionCorrection(settings.enablePositionCorrection)
		m_world.SetContinuousPhysics(settings.enableTOI)

		'm_pointCount = 0;

		m_world.DoStep(timeStep, settings.iterationCount)
	
		m_world.Validate()

	End Method
	
	Method Keyboard()
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
DebugLog p.X() + ", " + p.Y()
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
	
		If m_mouseJoint Then
			m_mouseJoint.SetTarget(p)
		End If
	End Method
	
End Type


Type TSettings

	Field hz:Float = 60
	Field iterationCount:Int = 10
	Field drawShapes:Int = 1
	Field drawJoints:Int = 1
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
	Field enablePositionCorrection:Int = 1
	Field enableTOI:Int = 1
	Field pause:Int
	Field singleStep:Int


End Type

Function ConvertScreenToWorld:b2Vec2(x:Int, y:Int)
'DebugStop
	Local u:Float = x / Float(tw)
	Local v:Float = (th - y) / Float(th)

	Local ratio:Float = tw / Float(th)
	Local extents:b2Vec2 = New b2Vec2.Create(ratio * 25.0, 25.0)
	extents.Multiply(viewZoom)

	Local _lower:b2Vec2 = viewCenter.Subtract(extents)
	Local _upper:b2Vec2 = viewCenter.Plus(extents)

	Return New b2Vec2.Create((1.0 - u) * _lower.X() + u * _upper.X(), (1.0 - v) * _lower.Y() + v * _upper.Y())
End Function
