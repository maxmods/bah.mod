SuperStrict

Framework BaH.Persistence
Import BRL.StandardIO

Type abc
	Field map:TMap = New TMap
End Type


Local map:TMap = New TMap
map.Insert("fish", "chips")
map.insert("abc", New abc)

TPersist.format = True
Local s:String = TPersist.Serialize(map)
Print s + "~n~n~n"

Local obj:Object = TPersist.DeSerialize(s)
Print TPersist.Serialize(obj)

