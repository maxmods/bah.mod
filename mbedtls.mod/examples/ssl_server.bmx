SuperStrict

Framework bah.mbedtls
Import brl.standardio

Const HTTP_RESPONSE:String = "HTTP/1.0 200 OK~r~nContent-Type: text/html~r~n~r~n" + ..
    "<h2>mbed TLS Test Server</h2>~r~n" + ..
    "<p>Successful connection using: %s</p>~r~n"

Local listen:TNetContext = New TNetContext.Create()
Local client:TNetContext = New TNetContext.Create()

Local ssl:TSSLContext = New TSSLContext.Create()
Local config:TSSLConfig = New TSSLConfig.Create()

Local cert:TX509Cert = New TX509Cert.Create()
Local pk:TPkContext = New TPkContext.Create()

Local entropy:TEntropyContext = New TEntropyContext.Create()
Local rctx:TRandContext = New TRandContext.Create()

' 1. Load the certificates and private RSA key

Print "  . Loading the server cert. and key..."

Local res:Int = cert.ParseFile("cert.pem")

If res Then
	Print " failed~n  !  TX509Cert.Parse() returned " + res
	Fail(res)
End If

res = pk.ParseKeyFile("privkey.pem")

If res Then
	Print " failed~n  !  TPkContext.ParseKey() returned " + res
	Fail(res)
End If

Print " ok"

' 2. Setup the listening TCP socket

Print "  . Bind on https://localhost:4433/ ..."

res = listen.Bind(Null, "4433", MBEDTLS_NET_PROTO_TCP)

If res Then
	Print " failed~n  !  TNetContext.Bind() returned " + res
	Fail(res)
End If

Print " ok"

' 3. Seed the RNG

Print "  . Seeding the random number generator..."

res = rctx.Seed(EntropyFunc, entropy)

If res Then
	Print " failed~n  !  TRandContext.Seed() returned " + res
	Fail(res)
End If

Print " ok"

' 4. Setup stuff

Print "  . Setting up the SSL data...."

res = config.Defaults(MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)

If res Then
	Print " failed~n  !  TSSLConfig.Defaults() returned " + res
	Fail(res)
End If

config.RNG(RandomFunc, rctx)
config.DBG(myDebug, Null)
config.SetDebugThreshold(0)

config.CaChain(cert, Null)

res = config.OwnCert(cert, pk)

If res Then
	Print " failed~n  !  TSSLConfig.OwnCert() returned " + res
	Fail(res)
End If

res = ssl.Setup(config)

If res Then
	Print " failed~n  !  TSSLContext.Setup() returned " + res
	Fail(res)
End If

Print " ok"

#Reset
While True

	If res <> 0 Then
		Print "Last error was " + res + " - " + MBEDTLSError(res)
	End If

	client.Free()
	ssl.SessionReset()
	
	' 5. Wait until a client connects
	
	Print "  . Waiting for a remote connection ..."
	
	Local clientIp:String
	
	res = listen.Accept(client, clientIp)
	
	If res Then
		Print " failed~n  !  TSSLNetContext.Accept() returned " + res
		Fail(res)
	End If
	
	ssl.SetBio(client, NetSend, NetRecv, Null)
	
	Print " ok"
	
	' 6. Handshake
	
	Print "  . Performing the SSL/TLS handshake..."
	
	res = ssl.Handshake()

	While res
	
		If res <> MBEDTLS_ERR_SSL_WANT_READ And res <> MBEDTLS_ERR_SSL_WANT_WRITE Then
			Print " failed~n  !  TSSLContext.Handshake() returned " + res
			Continue reset
		End If
		
		res = ssl.Handshake()
	Wend


	Print " ok"
	
	' 7. Read the HTTP Request
	
	Print "  < Read from client:"
	
	Local buf:Byte[1024]
	
	While True
	
?bmxng
		Local Length:Size_T = buf.Length - 1
		MemClear(buf, Size_T(buf.Length))
?Not bmxng
		Local Length:Int = buf.Length - 1
		MemClear(buf, buf.Length)
?	
		res = ssl.Read(buf, Length)
		
		If res = MBEDTLS_ERR_SSL_WANT_READ Or res = MBEDTLS_ERR_SSL_WANT_WRITE Then
			Continue
		End If
		
		If res <= 0 Then
			Select res
				Case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY
					Print " connection was closed gracefully"
				Case MBEDTLS_ERR_NET_CONN_RESET
					Print " connection was reset by peer"
				Default
					Print " TSSLContext.Read() returned " + res
			End Select
			
			Exit
		End If
		
		length = res
		
		Print length + " bytes read~n~n" + String.fromUTF8String(buf)
	
		If res > 0 Then
			Exit
		End If
	Wend
	
	' 8. Write the 200 Response
	
	Print "  > Write to client:"
	
	Local response:String = HTTP_RESPONSE.Replace("%s", ssl.GetCipherSuite())
?bmxng
	Local Length:Size_T = response.Length
?Not bmxng
	Local Length:Int = response.Length
?
	
	Local out:Byte Ptr = response.ToUtf8String()
	
	res = ssl.Write(out, Length)
	
	While res <= 0
	
		If res = MBEDTLS_ERR_NET_CONN_RESET Then
			Exit
		End If
		
		If res <> MBEDTLS_ERR_SSL_WANT_READ And res <> MBEDTLS_ERR_SSL_WANT_WRITE Then
			Exit
		End If
	
		res = ssl.Write(out, length)
	Wend
	
	length = res
	
	Print length + " bytes written"
	
	Print "  . Closing the connection..."
	
	res = ssl.CloseNotify()
	
	While res < 0
	
		If res <> MBEDTLS_ERR_SSL_WANT_READ And res <> MBEDTLS_ERR_SSL_WANT_WRITE Then
			Print " failed~n  !  TSSLContext.CloseNotify() returned " + res
			Continue reset
		End If
	
		res = ssl.CloseNotify()
	Wend
	
	Print " ok"
	res = 0
	
Wend

Function Fail(error:Int)
	End
End Function

Function myDebug(ctx:Object, level:Int, file:String, line:Int, str:String)
	Print file + ":" + line + ":  " + str.Replace("~n","")
End Function

