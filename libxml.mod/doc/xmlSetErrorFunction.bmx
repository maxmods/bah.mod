SuperStrict

Framework BaH.Libxml


Local s:String = "<?xml version=~q1.0~q?>~n" + ..
	"<!--This is a comment--> ~n" + ..
	"<Game>~n" + ..
	"	<test>~n" + ..
	"		<Element name=#testing#>~n" + ..
	"	</test>~n" + ..
	"</Game>~n"

xmlSetErrorFunction(errorCallback)

Local xmldoc:TxmlDoc = TxmlDoc.parseDoc(s)
DebugLog "Loaded"
If xmldoc Then 
	DebugLog "XML Valid"
	Local root:TxmlNode = xmldoc.getRootElement()
	DebugLog root.getName()

	Local children:TList = root.getChildren()
	For Local node:TxmlNode = EachIn children
		DebugLog " =>" + node.getname()					
	Next
Else
	DebugLog "** Invalid XML Syntax **"
End If

DebugLog "DONE"

End

Function errorCallback(data:Object, error:TxmlError)
	DebugLog "+++++++++  Callback :-) ++++++++"
	DebugLog "message: " + error.getErrorMessage()
	DebugLog "level  : " + error.getErrorLevel()
	DebugLog "file   : " + error.getFilename()
	DebugLog "line   : " + error.getLine()
	Local s:String[] = error.getExtraText()
	If s Then
		For Local i:Int = 0 Until s.length
			DebugLog "xtra   : " + s[i]
		Next
	End If
	DebugLog "column : " + error.getColumn()
	Local node:TxmlNode = error.getErrorNode()
	If node Then
		DebugLog "node   : " + node.getName()
	End If
End Function
