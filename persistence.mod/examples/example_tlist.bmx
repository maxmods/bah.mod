SuperStrict

Framework BRL.Standardio
Import BaH.Persistence

Local list:TList = New TList
list.AddLast("Hello")
list.AddLast("World")


Local persist:TPersist = New TXMLPersistenceBuilder.Build()

Local s:String = persist.SerializeToString(list)

Print s

persist.Free()

list = TList(persist.DeserializeObject(s))

For Local s:String = EachIn list
	Print s
Next
