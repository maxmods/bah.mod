SuperStrict


Type TNumbers

	Field a:Int
	Field b:Long
	Field c:Float
	Field d:Double
	Field e:Byte
	Field f:Short
	
	Method Create:TNumbers(a:Int, b:Long, c:Float, d:Double, e:Byte, f:Short)
		Self.a = a
		Self.b = b
		Self.c = c
		Self.d = d
		Self.e = e
		Self.f = f
		Return Self
	End Method

End Type

Type TStrings

	Field a:String
	Field b:String
	Field c:String

	Method Create:TStrings(a:String, b:String, c:String)
		Self.a = a
		Self.b = b
		Self.c = c
		Return Self
	End Method

End Type

Type TObject

	Field code:Int

	Method Create:TObject(code:Int)
		Self.code = code
		Return Self
	End Method

End Type

Type TObjectContainer

	Field object1:TObject
	Field object2:TObject
	Field object3:TObject

	Method Create:TObjectContainer(object1:TObject, object2:TObject, object3:TObject)
		Self.object1 = object1
		Self.object2 = object2
		Self.object3 = object3
		Return Self
	End Method

End Type

Type TRoom
	Field id:Int
	
	Method Create:TRoom(id:Int)
		Self.Id = id
		Return Self
	End Method
End Type
