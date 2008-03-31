SuperStrict


Type debugDraw Extends b2DebugDraw

	Method DrawPolygon(vertices:b2Vec2[], color:b2Color)
	End Method

	Rem
	bbdoc: Draw a solid closed polygon provided in CCW order
	End Rem
	Method DrawSolidPolygon(vertices:b2Vec2[], color:b2Color)
	End Method

	Rem
	bbdoc: Draw a circle.
	End Rem
	Method DrawCircle(center:b2Vec2, radius:Float, color:b2Color)
	End Method
	
	Rem
	bbdoc: Draw a solid circle.
	End Rem
	Method DrawSolidCircle(center:b2Vec2, radius:Float, axis:b2Vec2, color:b2Color)
	End Method
	
	Rem
	bbdoc: Draw a line segment.
	End Rem
	Method DrawSegment(p1:b2Vec2, p2:b2Vec2, color:b2Color)
	End Method
	
	Rem
	bbdoc: Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	End Rem
	Method DrawXForm(xf:b2XForm)
	End Method


End Type

