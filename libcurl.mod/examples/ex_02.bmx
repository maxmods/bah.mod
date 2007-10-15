SuperStrict

' Example which includes a progress indicator.
' The content is put into an internal string within the easy handle, and retrieved later with curl.toString()
'

Framework BaH.libcurl
Import BRL.StandardIO

Local curl:TCurlEasy = TCurlEasy.Create()

curl.setOptInt(CURLOPT_FOLLOWLOCATION, 1)
curl.setWriteString()' use the internal string  to store the content

curl.setProgressCallback(progressCallback) ' set the progress callback function

curl.setOptString(CURLOPT_URL, "blitzmax.com")

Local res:Int = curl.perform()

curl.cleanup()

Print curl.toString()

Function progressCallback:Int(data:Object, dltotal:Double, dlnow:Double, ultotal:Double, ulnow:Double)
	Print " ++++ " + dlnow + " bytes"
	Return 0	
End Function
