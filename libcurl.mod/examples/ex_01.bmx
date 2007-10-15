SuperStrict

' A simple example which outputs the contents of a URL to the console.
' Also outputs debug info.
'

Framework BaH.libcurl
Import BRL.StandardIO

Local curl:TCurlEasy = TCurlEasy.Create()

curl.setOptInt(CURLOPT_VERBOSE, 1)
curl.setOptInt(CURLOPT_FOLLOWLOCATION, 1)

curl.setOptString(CURLOPT_URL, "blitzmax.com")

Local res:Int = curl.perform()

curl.cleanup()

