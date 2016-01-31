SuperStrict

Framework BaH.Jansson
Import BRL.StandardIO
Import brl.textstream

Print "* ORIGINAL *"
Print LoadText("menu.json")

Local stream:TStream = ReadStream("menu.json")


Local error:TJSONError

Local json:TJSON = TJSON.Load(stream, 0, error)

stream.Close()

Print "* RE-ENCODED *"
Print json.SaveString(JSON_COMPACT, 0)

