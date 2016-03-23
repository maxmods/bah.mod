SuperStrict

Import BaH.Chipmunk
 
Import BRL.GLMax2d
?win32
Import BRL.d3d7max2d
?

Global staticBody:CPBody
Global space:CPSpace
Global ticks:Int = 0
Global toggleMode:Int = 0

Function drawPolyShape(shape:CPPolyShape)

	Local body:CPBody = shape.GetBody()
	
	Local pos:CPVect = body.GetPosition()

	Local verts:CPVect[] = shape.GetVerts()
	Local First:CPVect
	Local last:CPVect
	
	For Local i:Int = 0 Until verts.length
		Local v:CPVect = pos.Add(verts[i].Rotate(body.GetRot()))
		
		If Not First Then
			First = v
		End If
		
		If last Then
			DrawLine Float(last.x), Float(last.y), Float(v.x), Float(v.y)
		End If
		
		last = v
	Next
	
	DrawLine Float(last.x), Float(last.y), Float(First.x), Float(First.y)

	If toggleMode Then
		SetRotation Float(body.GetAngle())
		DrawText Chr(ExtraShape(shape).letter + 64), Float(pos.x), Float(pos.y)
		SetRotation 0
	End If

End Function

Function drawCircleShape(shape:CPCircleShape)

	Local body:CPBody = shape.GetBody()
	
	Local pos:CPVect = body.GetPosition()
	Local center:CPVect = shape.GetTransformedCenter()

	Local radius:Float = shape.GetRadius()

	DrawOval Float(center.x - radius), Float(center.y - radius), radius * 2, radius * 2
End Function

Function drawSegmentShape(shape:CPSegmentShape)

	SetRotation 0

	Local body:CPBody = shape.GetBody()
	
	Local pos:CPVect = body.GetPosition()

	Local a:CPVect = pos.Add(shape.GetEndPointA().Rotate(body.GetRot()))
	Local b:CPVect = pos.Add(shape.GetEndPointB().Rotate(body.GetRot()))
	
	DrawLine Float(a.x), Float(a.y), Float(b.x), Float(b.y)

End Function

Function drawObject(shape:Object, data:Object)

	If CPPolyShape(shape) Then
		drawPolyShape(CPPolyShape(shape))
	ElseIf CPSegmentShape(shape) Then
		drawSegmentShape(CPSegmentShape(shape))
	ElseIf CPCircleShape(shape) Then
		drawCircleShape(CPCircleShape(shape))
	End If
	
End Function


Type ExtraShape Extends CPPolyShape

	Field letter:Int
	
End Type


