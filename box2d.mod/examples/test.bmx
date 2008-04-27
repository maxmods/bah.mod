SuperStrict

Import "render.bmx"


Function Run(tst:Test, settings:TSettings)

	SetOrigin(400,500)
	'Local ratio:Float = 800 / 600.0
	'Local ex:Float = 25 * ratio
	'Local ey:Float = 25

	'SetScale 10, 2
	
	'gluOrtho2D(0 - ex, 0 + ex,  20 - ey, 20 + ey)
	
	While Not KeyDown(KEY_ESCAPE)
	
		Cls
		
		tst.DoStep(settings)
		
		Flip
	
	Wend



End Function





Type Test

	Field m_worldAABB:b2AABB = New b2AABB.Create()
	Field m_world:b2World
	Field m_debugDraw:b2DebugDraw = New debugDraw

	Method Init()
		m_worldAABB.SetLowerBound(New b2Vec2.Create(-200.0, -100.0))
		m_worldAABB.SetUpperBound(New b2Vec2.Create(200.0, 200.0))
	
		Local gravity:b2Vec2 = New b2Vec2.Create(0.0, -10.0)
		m_world = New b2World.Create(m_worldAABB, gravity, True)

		m_world.SetDebugDraw(m_debugDraw)
		
	End Method

	Method DoStep(settings:TSettings)

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

		'm_world.SetWarmStarting(settings.enableWarmStarting)
		'm_world.SetPositionCorrection(settings.enablePositionCorrection)
		'm_world.SetContinuousPhysics(settings.enableTOI)

		'm_pointCount = 0;

		m_world.DoStep(timeStep, settings.iterationCount)
	
		m_world.Validate()

	End Method
	
End Type


Type TSettings

	Field hz:Float = 60
	Field iterationCount:Int
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