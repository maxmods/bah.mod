SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local s:String = "a b c !£$%^&*(_09<>?"

' print original string
Print s
' print escaped string
Print TxmlURI.URIEscapeString(s, Null)
