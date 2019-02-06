SuperStrict

Import bah.maxunit
Import bah.persistencehelper

Import "types.bmx"

Type StringMapTest Extends TTest

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
	
		Local map:TStringMap = New TStringMap

		map[STR_ONE] = obj1
		map[STR_TWO] = obj2
		map[STR_THREE] = obj3
		map[STR_FOUR] = Null
		
		Local s:String = persist.SerializeToString(map)

		validateRefs(s, STR_ONE, 1)
		validateRefs(s, STR_TWO, 1)
		validateRefs(s, STR_THREE, 1)
		validateRefs(s, STR_FOUR, 1)

		persist.Free()
		
		Local result:TStringMap = TStringMap(persist.DeserializeObject(s))

		Local values:String[] = [STR_ONE, STR_TWO, STR_THREE, STR_FOUR]
		Local count:Int
		For Local k:String = EachIn result.Keys()
			assertContains(values, k)
			count :+ 1
			
			Local v:Object = map[k]
			Select k
				Case STR_ONE
					assertEquals(obj1.code, TObject(v).code)
				Case STR_TWO
					assertEquals(obj2.code, TObject(v).code)
				Case STR_THREE
					assertEquals(obj3.code, TObject(v).code)
				Case STR_FOUR
					assertNull(v)
			End Select
		Next
		assertEquals(4, count)

	End Method

	Method assertContains(values:String[], k:String)
		For Local i:String = EachIn values
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
