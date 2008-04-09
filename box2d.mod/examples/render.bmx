SuperStrict

Import BRL.GLMax2D

Type debugDraw Extends b2DebugDraw

	Method DrawPolygon(vertices:b2Vec2[], color:b2Color)
	End Method

	Rem
	bbdoc: Draw a solid closed polygon provided in CCW order
	End Rem
	Method DrawSolidPolygon(vertices:b2Vec2[], color:b2Color)
DebugLog "DrawSolidPolygon"
		SetAlpha 0.5
		SetColor(color.red / 2, color.green / 2, color.blue / 2)
		
		Local poly:Float[] = New Float[vertices.length * 2]
		For Local i:Int = 0 Until vertices.length
			poly[i * 2] = vertices[i].X()
			poly[i * 2 + 1] = vertices[i].Y()
DebugLog "x = " + poly[i * 2] + ", y = " + poly[i * 2 + 1]
		Next
		DrawPoly(poly)
		
		SetAlpha 1
		SetColor(color.red / 2, color.green / 2, color.blue / 2)
		
		For Local i:Int = 0 Until vertices.length - 1
			DrawLine poly[i * 2], poly[i * 2 + 1], poly[i * 2 + 2], poly[i * + 3]
		Next
		DrawLine poly[0], poly[1], poly[poly.length - 2], poly[poly.length - 1]
		
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
		SetColor(color.red, color.green, color.blue)
		DrawLine p1.x(), p1.y(), p2.x(), p2.y()
	End Method
	
	Rem
	bbdoc: Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	End Rem
	Method DrawXForm(xf:b2XForm)
	End Method


End Type

