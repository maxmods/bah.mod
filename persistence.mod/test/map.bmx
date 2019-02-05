SuperStrict

Import bah.maxunit
Import bah.persistence

Import "types.bmx"

Type MapsTest Extends TTest

	Const STR_ONE:String = "ABCDEFG"
	Const STR_TWO:String = "HIJKLMNOP"
	Const STR_THREE:String = "QRSTUVWXYZ"
	Const STR_FOUR:String = "1234567890"

	Const OBJ_ONE:Int = 6644
	Const OBJ_TWO:Int = 7755
	Const OBJ_THREE:Int = 8833

	Field persist:TPersist

	Field obj1:TObject
	Field obj2:TObject
	Field obj3:TObject

	Method Setup() { before }
		persist = New TXMLPersistenceBuilder.Build()

		obj1 = New TObject.Create(OBJ_ONE)
		obj2 = New TObject.Create(OBJ_TWO)
		obj3 = New TObject.Create(OBJ_THREE)
	End Method

	Method TearDown() { after }
		persist.Free()
	End Method

	Method testBasic() { test }
	
		Local map:TMap = New TMap
		
		map.Insert(STR_ONE, STR_TWO)
		map.Insert(STR_THREE, STR_FOUR)
			
		Local s:String = persist.SerializeToString(map)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_TWO, 1)
		validateRefs(s, STR_THREE, 1)
		validateRefs(s, STR_FOUR, 1)
		
		persist.Free()
		
		Local result:TMap = TMap(persist.DeserializeObject(s))
		
		assertEquals(STR_TWO, String(result.ValueForKey(STR_ONE)))
		assertEquals(STR_FOUR, String(result.ValueForKey(STR_THREE)))
		
	End Method
	
	' should only store a single copy of the string
	Method testStringRefs() { test }
	
		Local map:TMap = New TMap
		
		map.Insert(STR_ONE, STR_ONE)
		map.Insert(STR_TWO, STR_ONE)
		map.Insert(STR_THREE, STR_TWO)
		map.Insert(STR_FOUR, STR_TWO)

		Local s:String = persist.SerializeToString(map)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_TWO, 1)
		validateRefs(s, STR_THREE, 1)
		validateRefs(s, STR_FOUR, 1)
		
		persist.Free()
		
		Local result:TMap = TMap(persist.DeserializeObject(s))

		assertEquals(STR_ONE, String(result.ValueForKey(STR_ONE)))
		assertEquals(STR_ONE, String(result.ValueForKey(STR_TWO)))
		assertEquals(STR_TWO, String(result.ValueForKey(STR_THREE)))
	
		assertEquals(STR_TWO, String(result.ValueForKey(STR_FOUR)))
	End Method
	
	Method testObjects() { test }
		Local map:TMap = New TMap
		
		map.Insert(STR_ONE, obj1)
		map.Insert(STR_TWO, obj2)
		map.Insert(STR_THREE, obj3)
		map.Insert(obj1, obj1)
		map.Insert(obj2, obj2)
		map.Insert(obj3, obj3)
			
		Local s:String = persist.SerializeToString(map)

		validateRefs(s, OBJ_ONE, 1)
		validateRefs(s, OBJ_TWO, 1)
		validateRefs(s, OBJ_THREE, 1)

		persist.Free()
		
		Local result:TMap = TMap(persist.DeserializeObject(s))
		
		Local obj:TObject = TObject(result.ValueForKey(STR_TWO))
		
		assertEquals(OBJ_ONE, TObject(result.ValueForKey(STR_ONE)).code)
		assertEquals(OBJ_TWO, TObject(result.ValueForKey(STR_TWO)).code)
		assertEquals(OBJ_THREE, TObject(result.ValueForKey(STR_THREE)).code)
		assertEquals(OBJ_TWO, TObject(result.ValueForKey(obj)).code)

	End Method
	
	Method validateRefs(ser:String, txt:String, expected:Int)
		Local count:Int
		Local i:Int = ser.Find(txt, 0)

		While i <> -1
			count :+ 1
			i = ser.Find(txt, i + txt.length)
		Wend
		
		assertEquals(expected, count, "Refs mismatch")
	End Method

End Type
