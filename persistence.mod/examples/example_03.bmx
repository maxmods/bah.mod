SuperStrict

Framework BaH.Persistence
Import BRL.StandardIO

Type abc
	Field map:TMap = New TMap
End Type

Local persist:TPersist = New TXMLPersistenceBuilder.Build()

Local map:TMap = New TMap
map.Insert("fish", "chips")
map.insert("abc", New abc)

TPersist.format = True
Local s:String = persist.SerializeToString(map)
Print s + "~n~n~n"

persist.Free()

Local obj:Object = persist.DeSerializeObject(s)
persist.Free()

Print persist.SerializeToString(obj)

