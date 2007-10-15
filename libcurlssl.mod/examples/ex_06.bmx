SuperStrict

' Lists the cookies returned from a website
'

Framework BaH.libcurlssl
Import BRL.StandardIO

Local curl:TCurlEasy = TCurlEasy.Create()

curl.setOptInt(CURLOPT_FOLLOWLOCATION, 1)
curl.setOptString(CURLOPT_COOKIEFILE, "cookie.txt") ' enables cookie engine ! (the file doesn't have to exist)
curl.setWriteString()
curl.setOptString(CURLOPT_URL, "google.com")

Local res:Int = curl.perform()

Local info:TCurlInfo = curl.getInfo()

' retrieve the cookies, if there were any
Local cookies:String[] = info.cookieList()

If cookies Then
	Print "cookie count = " + cookies.length
	
	For Local i:Int = 0 Until cookies.length
		Print cookies[i]
	Next
Else
	Print "No cookies!"
End If

curl.cleanup()

