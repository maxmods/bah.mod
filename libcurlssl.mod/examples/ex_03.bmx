SuperStrict

' Example showing FTP access using a pair of "performs", a custom Stream, and a debug callback.
'
' The first perform connects to a valid directory, the second re-uses the connection and attempts to get
' a non-existent dir.
' FTP'ing to a directory rather than a specific file results in a directory listing.
'

Framework BaH.libcurlssl
Import BRL.StandardIO

Local curl:TCurlEasy = TCurlEasy.Create()

Local stream:TStringStream = New TStringStream

' enable verbosity
curl.setOptInt(CURLOPT_VERBOSE, 1)

' redirect messages via callback
curl.setDebugCallback(debugFunction)

' redirect data to stream
curl.setWriteStream(stream)

' ftp connect and initial directory
curl.setOptString(CURLOPT_URL, "ftp.sunsite.dk/pub/")

' go ! !
Local res:Int = curl.perform()

If res Then
	Print "***** " + CurlError(res) + " *****"
	End
End If

' print the result :-)
Print stream.text
stream.text = "" ' reset our stream text...

' directory change should fail...
curl.setOptString(CURLOPT_URL, "ftp.sunsite.dk/pub/gnu/")
res = curl.perform()

If res Then
	Print "***** " + CurlError(res) + " *****"
End If

curl.cleanup()

Print stream.text

End

Function debugFunction:Int(data:Object, msgType:Int, message:String)

	' we only care about server info
	If msgType = CURLINFO_HEADER_IN Then
		Print message
	End If

End Function

' a simple string stream to collect the text content
Type TStringStream Extends TStream
	
	Method Write:Int( buf:Byte Ptr, count:Int )
		
		text:+ String.FromBytes( buf, count )
		
		Return count
	End Method

	Field text:String = ""
	
End Type
