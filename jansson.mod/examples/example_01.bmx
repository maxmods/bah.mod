SuperStrict

Framework BaH.Jansson
Import BRL.StandardIO
Import BRL.TextStream

Local text:String = LoadText("menu.json")

Print "* ORIGINAL *"
Print text

Local error:TJSONError

Local json:TJSON = TJSON.Load(text, 0, error)

Print "* RE-ENCODED *"
Print json.SaveString(0, 2)
