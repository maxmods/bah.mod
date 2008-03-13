SuperStrict

Framework BaH.Libxml
Import BRL.StandardIO

Local s:String = "http://www.somesite.com:8080/stuff/index.php?count=1#second"

Local uri:TxmlURI = TxmlURI.parseURI(s)

Print "server   = " + uri.getServer()
Print "port     = " + uri.getPort()
Print "path     = " + uri.getPath()
Print "query    = " + uri.getQuery()
Print "fragment = " + uri.getFragment()
