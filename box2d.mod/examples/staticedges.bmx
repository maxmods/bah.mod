SuperStrict

Framework BaH.Box2d
Import BRL.GLMax2D
Import BRL.Random

Import "test.bmx"


Graphics 800,600, 0
SetBlend alphablend


Run(New StaticEdges.Create(), New TSettings)


Type StaticEdges Extends Test

	Method Create:StaticEdges()
	
		SeedRnd(MilliSecs())
	
		Init(12, 12)

		Local ground:b2Body

		Local sd:b2PolygonDef = New b2PolygonDef
		sd.SetAsBox(50.0, 10.0)
		
		Local bd:b2BodyDef = New b2BodyDef
		bd.SetPosition(New b2Vec2.Create(0.0, -10.0))
		ground = m_world.CreateBody(bd)
		ground.CreateShape(sd)


		'
		Local sd1:b2CircleDef = New b2CircleDef
		sd1.SetRadius(0.5)
		sd1.SetLocalPosition(Vec2(-0.5, 0.5))
		sd1.SetDensity(2.0)
		
		Local sd2:b2CircleDef = New b2CircleDef
		sd2.SetRadius(0.5)
		sd2.SetLocalPosition(Vec2(0.5, 0.5))
		sd2.SetDensity(0.0) ' massless
		
		For Local i:Int = 0 Until 10
			Local x:Float = Rnd(-0.1, 0.1)
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(x + 5.0, 1.05 + 2.5 * i)
			bd.SetAngle(Rand(-180, 180))
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(sd1)
			body.CreateShape(sd2)
			body.SetMassFromShapes()
		Next


		'
		Local pd1:b2PolygonDef = New b2PolygonDef
		pd1.SetAsBox(0.25, 0.5)
		pd1.SetDensity(2.0)
		
		Local pd2:b2PolygonDef = New b2PolygonDef
		pd2.SetAsOrientedBox(0.25, 0.5, Vec2(0.0, -0.5), 90)
		pd2.SetDensity(2.0)
		
		For Local i:Int = 0 Until 10
			Local x:Float = Rnd(-0.1, 0.1)
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(x - 5.0, 1.05 + 2.5 * i)
			bd.SetAngle(Rand(-180, 180))
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(pd1)
			body.CreateShape(pd2)
			body.SetMassFromShapes()
		Next


		'
		Local xf1:b2XForm = New b2XForm.Create()
		xf1.GetR().SetAngle(63.43200)
		xf1.SetPosition(b2Mul(xf1.GetR(), Vec2(1.0, 0.0)))
		
		pd1 = New b2PolygonDef
		Local vertices:b2Vec2[] = New b2Vec2[3]
		vertices[0] = b2MulF(xf1, Vec2(-1.0, 0.0))
		vertices[1] = b2MulF(xf1, Vec2(1.0, 0.0))
		vertices[2] = b2MulF(xf1, Vec2(0.0, 0.5))
		pd1.SetVertices(vertices)
		pd1.SetDensity(2.0)
		
		Local xf2:b2XForm = New b2XForm.Create()
		xf2.GetR().SetAngle(63.43200)
		xf2.SetPosition(b2Mul(xf2.GetR(), Vec2(-1.0, 0.0)))
		
		pd2 = New b2PolygonDef
		vertices[0] = b2MulF(xf2, Vec2(-1.0, 0.0))
		vertices[1] = b2MulF(xf2, Vec2(1.0, 0.0))
		vertices[2] = b2MulF(xf2, Vec2(0.0, 0.5))
		pd2.SetVertices(vertices)
		pd2.SetDensity(2.0)
		
		For Local i:Int = 0 Until 10
			Local x:Float = Rnd(-0.1, 0.1)
			Local bd:b2BodyDef = New b2BodyDef
			bd.SetPositionXY(x, 2.05 + 2.5 * i)
			bd.SetAngle(0.0)
			Local body:b2Body = m_world.CreateBody(bd)
			body.CreateShape(pd1)
			body.CreateShape(pd2)
			body.SetMassFromShapes()
		Next	
		
		'
		Local sd_bottom:b2PolygonDef = New b2PolygonDef
		sd_bottom.SetAsBox( 1.5, 0.15 )
		sd_bottom.SetDensity(4.0)
		
		Local sd_left:b2PolygonDef = New b2PolygonDef
		sd_left.SetAsOrientedBox(0.15, 2.7, Vec2(-1.45, 2.35), 11.4591559)
		sd_left.SetDensity(4.0)
		
		Local sd_right:b2PolygonDef = New b2PolygonDef
		sd_right.SetAsOrientedBox(0.15, 2.7, Vec2(1.45, 2.35), -11.4591559)
		sd_right.SetDensity(4.0)
		
		bd = New b2BodyDef
		bd.SetPositionXY( 0.0, 2.0 )
		Local body:b2Body = m_world.CreateBody(bd)
		body.CreateShape(sd_bottom)
		body.CreateShape(sd_left)
		body.CreateShape(sd_right)
		body.SetMassFromShapes()
		

		Local loop1:Float[] =  [  ..
			0.063134534, 8.3695248,  ..
			0.94701801, 9.3165428,  ..
			0.0, 9.0640047,  ..
			-0.12626907, 10.326695,  ..
			1.4520943, 11.77879,  ..
			2.2728432, 10.137292,  ..
			2.3991123, 11.147444,  ..
			3.5986685, 10.958041,  ..
			3.9143411, 7.3593722,  ..
			4.1668793, 9.4428119,  ..
			5.4295699, 9.3165428,  ..
			6.2503189, 8.3063903,  ..
			6.6922606, 10.137292,  ..
			4.9876282, 9.8216191,  ..
			4.7350901, 10.958041,  ..
			7.2604714, 11.652521,  ..
			10.732871, 11.147444,  ..
			10.480333, 10.642368,  ..
			10.732871, 9.8216191,  ..
			11.55362, 9.4428119,  ..
			12.374369, 9.3796773,  ..
			13.005714, 9.8216191,  ..
			13.195118, 10.38983,  ..
			13.005714, 10.768637,  ..
			12.626907, 10.894906,  ..
			12.753176, 11.526252,  ..
			13.573925, 11.715655,  ..
			14.836616, 11.399982,  ..
			16.351844, 10.768637,  ..
			17.867073, 11.399982,  ..
			17.803939, 10.263561,  ..
			17.361997, 8.3063903,  ..
			17.803939, 8.1801212,  ..
			18.056477, 9.5059464,  ..
			18.182746, 11.336848,  ..
			18.561553, 11.210579,  ..
			18.561553, 9.6322155,  ..
			18.561553, 7.7381795,  ..
			18.687822, 5.5284708,  ..
			19.382302, 5.6547398,  ..
			19.066629, 8.1801212,  ..
			19.003495, 10.263561,  ..
			19.066629, 11.463117,  ..
			19.887378, 11.841924,  ..
			20.708127, 11.273713,  ..
			21.0238, 10.011023,  ..
			20.708127, 7.2962377,  ..
			21.086934, 6.2860852,  ..
			21.150069, 3.7607038,  ..
			20.392455, 2.5611476,  ..
			18.624688, 2.5611476,  ..
			20.771262, 2.1192059,  ..
			20.771262, 0.22516988,  ..
			18.624688, -0.2799064,  ..
			13.826463, 0.16203534,  ..
			14.015867, 1.7403987,  ..
			13.195118, 2.1823404,  ..
			12.626907, 1.5509951,  ..
			12.879445, 0.85651522,  ..
			12.626907, 0.35143895,  ..
			10.543467, 1.298457,  ..
			11.490485, 3.9501074,  ..
			13.889598, 3.6344347,  ..
			13.889598, 2.9399549,  ..
			14.584077, 3.8869729,  ..
			11.932427, 5.2127981,  ..
			9.7227183, 4.0132419,  ..
			10.796005, 3.5081657,  ..
			9.7858528, 3.2556275,  ..
			10.796005, 2.4980131,  ..
			7.9549513, 1.7403987,  ..
			9.6595837, 1.424726,  ..
			9.217642, 0.66711162,  ..
			8.270624, -0.090502792,  ..
			7.0079333, 0.85651522,  ..
			6.1240498, -0.15363733,  ..
			6.1240498, 3.192493,  ..
			5.6821081, 2.4348786,  ..
			4.9876282, 2.1192059,  ..
			4.1037447, 1.8666678,  ..
			3.0304576, 1.8666678,  ..
			2.0834396, 2.245475,  ..
			1.6414979, 2.6242822,  ..
			1.3258252, 3.5081657,  ..
			1.2626907, 0.47770802,  ..
			0.63134534, 0.035766276,  ..
			0.063134534, 0.98278429  ..
		]

		

		Local loop2:Float[] = [ ..
			8.270624, 6.1598161, ..
			8.270624, 5.3390672, ..
			8.7757003, 5.086529, ..
			9.4701801, 5.5284708, ..
			9.217642, 6.033547, ..
			8.7757003, 6.4123542 ..
		]
		
		Local b2Loop1:b2Vec2[] = New b2Vec2[87]
		Local b2Loop2:b2Vec2[] = New b2Vec2[6]
		
		For Local i:Int = 86 To 0 Step - 1
			b2Loop1[86 - i] = Vec2(loop1[i*2] + 10.0,  loop1[i*2 + 1] + 1.0)
		Next
		
		For Local i:Int = 0 Until 6
			b2Loop2[i] = Vec2(loop2[i*2], loop2[i*2 + 1])
		Next


		bd = New b2BodyDef
		bd.SetPositionXY( 0.0, 0.0 );
		body = m_world.CreateBody(bd)
		
		Local weight:b2CircleDef = New b2CircleDef
		weight.GetFilter().SetMaskBits(0)
		weight.SetDensity(4.0)
		weight.SetRadius(0.5)
		weight.SetLocalPosition(Vec2(8.9, 5.75))
		body.CreateShape(weight)
		
		Local edgeDef:b2EdgeChainDef = New b2EdgeChainDef
		edgeDef.SetVertices(b2Loop2)
		body.CreateShape(edgeDef)
		
		body.SetMassFromShapes()
		
		
		body = m_world.CreateBody(bd)
		weight.SetRadius(5.0)
		weight.SetLocalPosition(Vec2(20.5, 7.0))
		body.CreateShape(weight)
		
		edgeDef.SetVertices(b2Loop1)
		body.CreateShape(edgeDef)
		
		Return Self
	End Method

End Type



