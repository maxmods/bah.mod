SuperStrict

' Connect to a website via HTTPS, using a certificate bundle.
'
'

Framework BaH.libcurlssl
Import BRL.StandardIO
Import BRL.FileSystem

Local curl:TCurlEasy = TCurlEasy.Create()

curl.setWriteString()
curl.setOptInt(CURLOPT_VERBOSE, 1)
curl.setOptInt(CURLOPT_FOLLOWLOCATION, 1)

curl.setOptString(CURLOPT_CAINFO, "../certificates/ca-bundle.crt") ' the cert bundle

curl.setOptString(CURLOPT_URL, "https://msp.f-secure.com/web-test/")

Local res:Int = curl.perform()

If res Then
	Print CurlError(res)
	End
End If

curl.cleanup()

Print curl.toString()



