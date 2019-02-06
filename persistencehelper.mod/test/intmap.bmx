SuperStrict

Import bah.maxunit
Import bah.persistencehelper

Import "types.bmx"

Type IntMapTest Extends TTest

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
	
		Local map:TIntMap = New TIntMap
		map.Insert(OBJ_ONE, STR_ONE)
		map.Insert(OBJ_TWO, STR_TWO)
		map.Insert(OBJ_THREE, STR_THREE)
		map.Insert(0, Null)

		Local s:String = persist.SerializeToString(map)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_TWO, 1)
		validateRefs(s, STR_THREE, 1)

		persist.Free()
		
		Local result:TIntMap = TIntMap(persist.DeserializeObject(s))
	
		Local values:Int[] = [OBJ_ONE, OBJ_TWO, OBJ_THREE, 0]
		Local count:Int
		For Local k:TIntKey = EachIn result.Keys()
			assertContains(values, k.value)
			count :+ 1
			
			Local v:Object = map[k.value]
			Select k.value
				Case OBJ_ONE
					assertEquals(STR_ONE, v)
				Case OBJ_TWO
					assertEquals(STR_TWO, v)
				Case OBJ_THREE
					assertEquals(STR_THREE, v)
				Case 0
					assertNull(v)
			End Select
		Next
		assertEquals(4, count)
	
	End Method

	Method testRefs() { test }
	
		Local map:TIntMap = New TIntMap
		map.Insert(OBJ_ONE, STR_ONE)
		map.Insert(OBJ_TWO, STR_THREE)
		map.Insert(OBJ_THREE, STR_ONE)
		map.Insert(0, STR_THREE)

		Local s:String = persist.SerializeToString(map)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_THREE, 1)

		persist.Free()
		
		Local result:TIntMap = TIntMap(persist.DeserializeObject(s))

		Local values:Int[] = [OBJ_ONE, OBJ_TWO, OBJ_THREE, 0]
		Local count:Int
		For Local k:TIntKey = EachIn result.Keys()
			assertContains(values, k.value)
			count :+ 1
			
			Local v:Object = map[k.value]
			Select k.value
				Case OBJ_ONE
					assertEquals(STR_ONE, v)
				Case OBJ_TWO
					assertEquals(STR_THREE, v)
				Case OBJ_THREE
					assertEquals(STR_ONE, v)
				Case 0
					assertEquals(STR_THREE, v)
			End Select
		Next
		assertEquals(4, count)
	End Method
	
	Method assertContains(values:Int[], k:Int)
		For Local i:Int = EachIn values
			If i = k Then
				Return
			End If
		Next
		fail("not contains")
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
