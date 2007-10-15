SuperStrict

' Use the "multi" interface to retrieve 3 URLs at the same time.
'
'

Framework BaH.libcurlssl
Import BRL.StandardIO

Local urls:String[] = ["http://www.google.com/", "http://www.altavista.com/", "http://www.yahoo.com/"]

'DebugStop
Local multi:TCurlMulti = TCurlMulti.Create()

For Local i:Int = 0 Until urls.length

	Local curl:TCurlEasy = multi.newEasy()
	
	curl.setOptString(CURLOPT_URL, urls[i])
	curl.setWriteString() ' put the data retrieved into a string.
	curl.setOptInt(CURLOPT_FOLLOWLOCATION, 1) ' follow redirects
	curl.setOptInt(CURLOPT_MAXREDIRS, 2) ' maximum redirects
	curl.setOptInt(CURLOPT_CONNECTTIMEOUT, 15) ' timeout

Next

Local running:Int

' start performing the request 
Local status:Int = multi.multiPerform(running)

While status = CURLM_CALL_MULTI_PERFORM 
	status = multi.multiPerform(running)
Wend

While running And status = CURLM_OK
	' wait for network 
	If multi.multiSelect() <> -1 Then
		' pull in any new data, or at least handle timeouts 
		status = multi.multiPerform(running)
		While status = CURLM_CALL_MULTI_PERFORM 
			status = multi.multiPerform(running)
		Wend
	End If
Wend

If status <> CURLM_OK Then
	Print "Curl multi error : " + status
	End
End If

Print "success"


' now, let's see what we got !

Local size:Int
Local msg:TCurlMultiMsg = multi.multiInfoRead(size)

While msg

	Print "*********************************************"

	If msg.result = CURLE_OK Then
		Print msg.easy.toString() ' the data from the internal string
	Else
		Print CurlError(msg.result)
	End If

	' remove the easy handle
	multi.multiRemove(msg.easy)
	
	' cleanup the easy handle
	msg.easy.cleanup()

	msg = multi.multiInfoRead(size)
Wend

' cleanup the multi handle
multi.multiCleanup()



