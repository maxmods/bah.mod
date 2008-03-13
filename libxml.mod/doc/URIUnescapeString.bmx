SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local s:String = "a%20b%20c%20!%A3%24%25%5E%26*(_09%3C%3E%3F"

' print original escaped string
Print s
' print unescaped string
Print TxmlURI.URIUnescapeString(s)

