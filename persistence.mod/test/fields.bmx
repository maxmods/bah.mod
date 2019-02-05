SuperStrict

Import bah.maxunit
Import bah.persistence

Import "types.bmx"

Type FieldsTest Extends TTest

	Const NUM_INT:Int = 12345
	Const NUM_LONG:Long = 84343238901:Long
	Const NUM_FLOAT:Float = 10.688:Float
	Const NUM_DOUBLE:Double = 9420.0394:Double
	Const NUM_BYTE:Byte = 129
	Const NUM_SHORT:Short = 40000
	
	Const STR_ONE:String = "ABCDEFG"
	Const STR_TWO:String = "HIJKLMNOP"
	Const STR_THREE:String = "QRSTUVWXYZ"
	
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

	Method testNumbers() { test }

		Local numbers:TNumbers = New TNumbers.Create(NUM_INT, NUM_LONG, NUM_FLOAT, NUM_DOUBLE, NUM_BYTE, NUM_SHORT)
		
		Local s:String = persist.SerializeToString(numbers)

		persist.Free()
		
		Local result:TNumbers = TNumbers(persist.DeserializeObject(s))
	
		assertEquals(NUM_INT, result.a)
		assertEquals(NUM_LONG, result.b)
		assertEquals(NUM_FLOAT, result.c)
		assertEquals(NUM_DOUBLE, result.d)
		assertEquals(NUM_BYTE, result.e)
		assertEquals(NUM_SHORT, result.f)
	
	End Method
	
	Method testStrings() { test }
	
		Local strings:TStrings = New TStrings.Create(STR_ONE, STR_TWO, STR_THREE)
		
		Local s:String = persist.SerializeToString(strings)
		
		validateStringRefs(s, STR_ONE, 1)
		validateStringRefs(s, STR_TWO, 1)
		validateStringRefs(s, STR_THREE, 1)
		
		persist.Free()
		
		Local result:TStrings = TStrings(persist.DeserializeObject(s))
	
		assertEquals(STR_ONE, result.a)
		assertEquals(STR_TWO, result.b)
		assertEquals(STR_THREE, result.c)
	
	End Method

	' should only store a single copy of the string
	Method testStringRefs() { test }
	
		Local strings:TStrings = New TStrings.Create(STR_ONE, STR_ONE, STR_ONE)
		
		Local s:String = persist.SerializeToString(strings)
		
		validateStringRefs(s, STR_ONE, 1)
				
		persist.Free()
		
		Local result:TStrings = TStrings(persist.DeserializeObject(s))
	
		assertEquals(STR_ONE, result.a)
		assertEquals(STR_ONE, result.b)
		assertEquals(STR_ONE, result.c)
	
	End Method
	
	Method testObjects() { test }
		
		Local container:TObjectContainer = New TObjectContainer.Create(obj1, obj2, obj3)

		Local s:String = persist.SerializeToString(container)

		validateStringRefs(s, OBJ_ONE, 1)
		validateStringRefs(s, OBJ_TWO, 1)
		validateStringRefs(s, OBJ_THREE, 1)

		persist.Free()

		Local result:TObjectContainer = TObjectContainer(persist.DeserializeObject(s))

		assertEquals(OBJ_ONE, result.object1.code)
		assertEquals(OBJ_TWO, result.object2.code)
		assertEquals(OBJ_THREE, result.object3.code)
	End Method

	Method testObjectRefs() { test }
		
		Local container:TObjectContainer = New TObjectContainer.Create(obj1, obj2, obj1)

		Local s:String = persist.SerializeToString(container)

		validateStringRefs(s, OBJ_ONE, 1)
		validateStringRefs(s, OBJ_TWO, 1)

		persist.Free()

		Local result:TObjectContainer = TObjectContainer(persist.DeserializeObject(s))

		assertEquals(OBJ_ONE, result.object1.code)
		assertEquals(OBJ_TWO, result.object2.code)
		assertEquals(OBJ_ONE, result.object3.code)
	End Method

	Method testEmptyStrings() { test }
	
		Local strings:TStrings = New TStrings.Create(Null, STR_ONE, Null)
		
		assertNull(strings.a)
		assertEquals(STR_ONE, strings.b)
		assertNull(strings.c)

		Local s:String = persist.SerializeToString(strings)

		validateStringRefs(s, STR_ONE, 1)

		persist.Free()

		Local result:TStrings = TStrings(persist.DeserializeObject(s))
	
		assertNull(result.a)
		assertEquals(STR_ONE, result.b)
		assertNull(result.c)
		
	End Method
	
	Method validateStringRefs(ser:String, txt:String, expected:Int)
		Local count:Int
		Local i:Int = ser.Find(txt, 0)

		While i <> -1
			count :+ 1
			i = ser.Find(txt, i + txt.length)
		Wend
		
		assertEquals(expected, count, "Refs mismatch")
	End Method

End Type
