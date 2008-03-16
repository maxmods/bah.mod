SuperStrict

Framework BaH.Persistence
Import BRL.StandardIO

Type TRect
	Field x:Int
	Field y:Int
	Field w:Int
	Field h:Int
End Type

Type TObj
	Field text:String
	Field numbers:Int[]
	Field circularRef:TTest
	Field refNull:TTest
	Field emptyList:TList = New TList
	Field list:TList = New TList
	Field rect:TRect = New TRect
		
	Function Set:TObj()
		Local this:TObj = New TObj
		
		this.text = "woot"
		this.numbers = [ 1, 2, 3, 4, 5, 6 ]
		this.list.AddLast("Item 1")
		this.rect.x = 100
		this.rect.y = 200
		this.rect.w = 300
		this.rect.h = 400
		
		Return this
	End Function
	
End Type

Type TTest
	Field one:String
	Field two:Int
	Field three:Float
	Field four:Double
	Field five:Long
	
	Field obj:TObj
	
	Field rects:TRect[]
	
	Function Set:TTest()
		Local this:TTest = New TTest
		
		this.one = "Hello World"
		this.two = 155
		this.three = 3.33
		this.four = 2.95
		this.five = 222
		
		this.obj = TObj.Set()
		this.obj.circularRef = this
		
		this.rects = New TRect[2]
		'rects[0] = New TRect ' <- null!
		this.rects[1] = New TRect
		this.rects[1].y = 125
		
		Return this
	End Function
	
End Type


Local test:TTest = TTest.Set()

Local s:String = TPersist.Serialize(test)
Print s
Local obj:Object = TPersist.DeSerialize(s)
Print TPersist.Serialize(obj)


