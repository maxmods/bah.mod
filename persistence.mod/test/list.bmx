SuperStrict

Import bah.maxunit
Import bah.persistence

Import "types.bmx"

Type ListTest Extends TTest

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
	
		Local list:TList = New TList
		
		list.AddLast(STR_ONE)
		list.AddLast(STR_TWO)
		list.AddLast(STR_THREE)
		list.AddLast(STR_FOUR)

		assertEquals(4, list.Count())

		Local s:String = persist.SerializeToString(list)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_TWO, 1)
		validateRefs(s, STR_THREE, 1)
		validateRefs(s, STR_FOUR, 1)
		
		persist.Free()
		
		Local result:TList = TList(persist.DeserializeObject(s))

		assertEquals(4, result.Count())		
		assertTrue(result.Contains(STR_ONE))
		assertTrue(result.Contains(STR_TWO))
		assertTrue(result.Contains(STR_THREE))
		assertTrue(result.Contains(STR_FOUR))
	End Method

	Method testStringRefs() { test }
	
		Local list:TList = New TList
		
		list.AddLast(STR_ONE)
		list.AddLast(STR_TWO)
		list.AddLast(STR_THREE)
		list.AddLast(STR_FOUR)
		list.AddLast(STR_ONE)
		list.AddLast(STR_TWO)
		list.AddLast(STR_THREE)
		list.AddLast(STR_FOUR)

		assertEquals(8, list.Count())

		Local s:String = persist.SerializeToString(list)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_TWO, 1)
		validateRefs(s, STR_THREE, 1)
		validateRefs(s, STR_FOUR, 1)
		
		persist.Free()

		Local result:TList = TList(persist.DeserializeObject(s))

		assertEquals(8, result.Count())		

	End Method
	
	Method testObjectRef() { test }
	
		Local ids:Int[] = [30, 60, 30, 74, 100]
	
		Local room1:TRoom = New TRoom.Create(30)
		Local room2:TRoom = New TRoom.Create(74)
		Local room3:TRoom = New TRoom.Create(60)
		Local room4:TRoom = New TRoom.Create(100)
		
		Local list:TList = New TList
		
		list.AddLast(room1)
		list.AddLast(room3)
		list.AddLast(room1)
		list.AddLast(room2)
		list.AddLast(room4)
		
		assertEquals(5, list.Count())

		Local s:String = persist.SerializeToString(list)

		persist.Free()

		Local result:TList = TList(persist.DeserializeObject(s))

		assertEquals(5, result.Count())		
		
		Local i:Int
		For Local room:TRoom = EachIn result
			assertEquals(ids[i], room.id)
			i :+ 1
		Next
	
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
