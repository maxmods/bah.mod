SuperStrict

' Sets the User-Agent, and clear the Referer headers.
'
'


Framework BaH.libcurl
Import BRL.StandardIO

Local curl:TCurlEasy = TCurlEasy.Create()

curl.setWriteString()
curl.setOptInt(CURLOPT_FOLLOWLOCATION, 1)
curl.httpHeader(["User-Agent: BlitzMax", "Referer:"])

curl.setOptString(CURLOPT_URL, "google.com")

Local res:Int = curl.perform()

Print curl.toString()

curl.cleanup()
