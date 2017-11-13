SuperStrict

Framework BaH.Libxml
Import brl.standardio

Local s:String = "<?xml version=~q1.0~q?>~n" + ..
	"<!--This is a comment--> ~n" + ..
	"<Game>~n" + ..
	"	<test>~n" + ..
	"		<Element name=#testing#>~n" + ..
	"	</test>~n" + ..
	"</Game>~n"

xmlSetErrorFunction(errorCallback)

Local xmldoc:TxmlDoc = TxmlDoc.parseDoc(s)

If xmldoc Then 

	Local root:TxmlNode = xmldoc.getRootElement()
	Print root.getName()

	Local children:TList = root.getChildren()
	For Local node:TxmlNode = EachIn children
		Print " =>" + node.getname()					
	Next
Else
	Print "** Invalid XML Syntax **"
End If

Print "DONE"

End

Function errorCallback(data:Object, error:TxmlError)
	Print "+++++++++  Callback :-) ++++++++"
	Print "message: " + error.getErrorMessage()
	Print "level  : " + error.getErrorLevel()
	Print "file   : " + error.getFilename()
	Print "line   : " + error.getLine()
	Local s:String[] = error.getExtraText()
	If s Then
		For Local i:Int = 0 Until s.length
			Print "xtra   : " + s[i]
		Next
	End If
	Print "column : " + error.getColumn()
	Local node:TxmlNode = error.getErrorNode()
	If node Then
		Print "node   : " + node.getName()
	End If
End Function
