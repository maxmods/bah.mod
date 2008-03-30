SuperStrict


Type debugDraw Extends b2DebugDraw

	Function DrawPolygon(vertices:b2Vec2[], color:b2Color)
	End Function

	Rem
	bbdoc: Draw a solid closed polygon provided in CCW order
	End Rem
	Function DrawSolidPolygon(vertices:b2Vec2[], color:b2Color)
	End Function

	Rem
	bbdoc: Draw a circle.
	End Rem
	Function DrawCircle(center:b2Vec2, radius:Float, color:b2Color)
	End Function
	
	Rem
	bbdoc: Draw a solid circle.
	End Rem
	Function DrawSolidCircle(center:b2Vec2, radius:Float, axis:b2Vec2, color:b2Color)
	End Function
	
	Rem
	bbdoc: Draw a line segment.
	End Rem
	Function DrawSegment(p1:b2Vec2, p2:b2Vec2, color:b2Color)
	End Function
	
	Rem
	bbdoc: Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	End Rem
	Function DrawXForm(xf:b2XForm)
	End Function


End Type

