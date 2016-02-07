SuperStrict

Framework BaH.PersistenceJSON
Import BRL.StandardIO
Import BRL.Random

Type abc
	Const bits:String = "<>`~~~q~r~n!@£$%^&*()|}{:?abcdefghijklmnopqrstuvwxyz¡€#¢∞§¶•ªº–≠"

	Field Text:String
	Method Create:abc()
		For Local i:Int = 0 Until 128
			Local index:Int = Rand(0, bits.length-1)
			Text:+ bits[index..index+1]
		Next
		
		Return Self
	End Method
End Type

SeedRnd(MilliSecs())


Local obj:abc = New abc.Create()
Print obj.Text

'TPersistJSON.format = True
Local s:String = TPersistJSON.Serialize(obj)
Print s + "~n~n~n"

obj = abc(TPersistJSON.DeSerialize(s))
Print TPersistJSON.Serialize(obj)
Print obj.Text

