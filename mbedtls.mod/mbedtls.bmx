' 
' Copyright 2018 Bruce A Henderson
' 
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
' 
'     http://www.apache.org/licenses/LICENSE-2.0
' 
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
' 
SuperStrict

Rem
bbdoc: mbed TLS SSL library.
End Rem
Module BaH.mbedtls

ModuleInfo "Version: 1.03"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: Wrapper - 2018 Bruce A Henderson"

ModuleInfo "History: 1.03"
ModuleInfo "History: Update to mbedtls 2.13.0."
ModuleInfo "History: 1.02"
ModuleInfo "History: Update to mbedtls 2.7.1."
ModuleInfo "History: Fixed 64-bit build."
ModuleInfo "History: 1.01"
ModuleInfo "History: Update to mbedtls 2.7.0."
ModuleInfo "History: Applied win32 vsnprintf patch."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import BRL.Stream
Import "common.bmx"

'
' Patch notes :
'    For older versions of win32, a patch was required for 2.7.0 use of vsnprintf
'    see : https://github.com/ARMmbed/mbedtls/issues/355
'

Function MBEDTLSError:String(errornum:Int)
	Local buf:Byte[1024]
	mbedtls_strerror(errornum, buf, 1024)
	Return String.FromUTF8String(buf)
End Function

Rem
bbdoc: Wrapper type for sockets.
about: Currently backed by just a file descriptor, but might be more in the future
(eg two file descriptors for combined IPv4 + IPv6 support, or additional structures
for hand-made UDP demultiplexing).
End Rem
Type TNetContext

	Field contextPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Method Create:TNetContext()
		contextPtr = bmx_mbedtls_net_init()
		Return Self
	End Method

	Rem
	bbdoc: Creates a receiving socket on bindIp:port in the chosen protocol (#MBEDTLS_NET_PROTO_TCP or #MBEDTLS_NET_PROTO_UDP).
	returns: 0 if successful, or one of: #MBEDTLS_ERR_NET_SOCKET_FAILED, #MBEDTLS_ERR_NET_BIND_FAILED, #MBEDTLS_ERR_NET_LISTEN_FAILED.
	about: If bindIp is Null, all interfaces are bound
	Regardless of the protocol, opens the sockets and binds it. In addition, make the socket listening if protocol is TCP.
	End Rem
	Method Bind:Int(bindIp:String, port:String, proto:Int)
		Return bmx_mbedtls_net_bind(contextPtr, bindIp, port, proto)
	End Method

	Rem
	bbdoc: Initiates a connection with host:port in the given protocol  (#MBEDTLS_NET_PROTO_TCP or #MBEDTLS_NET_PROTO_UDP)..
	returns: 0 if successful, or one of: #MBEDTLS_ERR_NET_SOCKET_FAILED, #MBEDTLS_ERR_NET_UNKNOWN_HOST, #MBEDTLS_ERR_NET_CONNECT_FAILED.
	about: Sets the socket in connected mode even with UDP.
	End Rem
	Method Connect:Int(host:String, port:String, proto:Int)
		Return bmx_mbedtls_net_connect(contextPtr, host, port, proto)
	End Method
	
	Rem
	bbdoc: Reads at most @length characters.
	returns: the number of bytes received, or a non-zero error code; with a non-blocking socket, #MBEDTLS_ERR_SSL_WANT_READ indicates read() would block.
	about: If no error occurs, the actual amount read is returned.
	End Rem
	Method Recv:Int(buf:Byte Ptr, length:Int)
		Return bmx_mbedtls_net_recv(contextPtr, buf, length)
	End Method
	
	Rem
	bbdoc: Read at most @length characters, blocking for at most @timeout milliseconds.
	returns: the number of bytes received, or a non-zero error code: #MBEDTLS_ERR_SSL_TIMEOUT if the operation timed out, #MBEDTLS_ERR_SSL_WANT_READ if interrupted by a signal.
	about: If no error occurs, the actual amount read is returned.
	This will block (until data becomes available or timeout is reached) even if the socket is
	set to non-blocking. Handling timeouts with non-blocking reads requires a different strategy.
	End Rem
	Method RecvTimeout:Int(buf:Byte Ptr, length:Int, timeout:Int)
		Return bmx_mbedtls_net_recv_timeout(contextPtr, buf, length, timeout)
	End Method
	
	Rem
	bbdoc: Writes at most @length characters.
	returns: the number of bytes sent, or a non-zero error code; with a non-blocking socket, #MBEDTLS_ERR_SSL_WANT_WRITE indicates write() would block.
	about: If no error occurs, the actual amount read is returned.
	End Rem
	Method Send:Int(buf:Byte Ptr, length:Int)
		Return bmx_mbedtls_net_send(contextPtr, buf, length)
	End Method
	
	Rem
	bbdoc: Sets the socket blocking.
	returns: 0 if successful, or a non-zero error code
	End Rem
	Method SetBlock:Int()
		Return mbedtls_net_set_block(contextPtr)
	End Method
	
	Rem
	bbdoc: Sets the socket non-blocking.
	returns: 0 if successful, or a non-zero error code
	End Rem
	Method SetNonBlock:Int()
		Return mbedtls_net_set_nonblock(contextPtr)
	End Method
	
	Rem
	bbdoc: Portable usleep helper.
	about: Real amount of time slept will not be less than select()'s timeout granularity (typically, 10ms).
	End Rem
	Function USleep(usec:Int)
		bmx_mbedtls_net_usleep(usec)
	End Function
	
	Rem
	bbdoc: Check if data is available on the socket.
	End Rem
	Method Poll:Int(rw:Int, timeout:Int)
		Return bmx_mbedtls_net_poll(contextPtr, rw, timeout)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Accept:Int(client:TNetContext, ip:String Var)
		Local buf:Byte[256]
		Local length:Int
		Local res:Int = mbedtls_net_accept(contextPtr, client.contextPtr, buf, 256, Varptr length)
		
		If length = 4
			ip = StrIPv4(buf)
		ElseIf length = 16
			' IPv6 works, I tested
			' You have to bind to [::1] and you can then `curl -g -6 --insecure "https://[::1]"`
			ip = "IPv6"
		Else
			ip = "UNKNOWN"
		End If
		
		Return res
	End Method
	
	Function StrIPv4:String(buf:Byte Ptr)
		Return buf[0] + "." + buf[1] + "." + buf[2] + "." + buf[3]		
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		bmx_mbedtls_net_free(contextPtr)
	End Method
	
	Method Delete()
		If contextPtr Then
			Free()
			bmx_mbedtls_net_delete(contextPtr)
			contextPtr = Null
		End If
	End Method
	
End Type

Rem
bdoc: 
End Rem
Type TSSLContext

	Field contextPtr:Byte Ptr
	
	Field _bioCtx:Object
?bmxng
	Field _cbSend:Int(ctx:Object, buf:Byte Ptr, length:Size_T)
	Field _cbRecv:Int(ctx:Object, buf:Byte Ptr, length:Size_T)
	Field _cbTimeout:Int(ctx:Object, buf:Byte Ptr, length:Size_T, timeout:Int)
?Not bmxng
	Field _cbSend:Int(ctx:Object, buf:Byte Ptr, length:Int)
	Field _cbRecv:Int(ctx:Object, buf:Byte Ptr, length:Int)
	Field _cbTimeout:Int(ctx:Object, buf:Byte Ptr, length:Int, timeout:Int)
?
	Field _timerCtx:Object
	Field _cbTimerSet(ctx:Object, intMs:Int, finMs:Int)
	Field _cbTimerGet:Int(ctx:Object)
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TSSLContext()
		contextPtr = bmx_mbedtls_ssl_init()
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Setup:Int(config:TSSLConfig)
		Return mbedtls_ssl_setup(contextPtr, config.configPtr)
	End Method
	
	Rem
	bbdoc: Resets an already initialized SSL context for re-use while retaining application-set variables, function pointers and data.
	returns: 0 if successful, or #MBEDTLS_ERR_SSL_ALLOC_FAILED, #MBEDTLS_ERR_SSL_HW_ACCEL_FAILED or #MBEDTLS_ERR_SSL_COMPRESSION_FAILED
	End Rem
	Method SessionReset:Int()
		Return mbedtls_ssl_session_reset(contextPtr)
	End Method
	
	Rem
	bbdoc: Sets the underlying BIO callbacks for write, read and read-with-timeout.
	End Rem
?bmxng
	Method SetBio(ctx:Object, cbSend:Int(ctx:Object, buf:Byte Ptr, length:Size_T), ..
			cbRecv:Int(ctx:Object, buf:Byte Ptr, length:Size_T), ..
			cbTimeout:Int(ctx:Object, buf:Byte Ptr, length:Size_T, timeout:Int))
?Not bmxng
	Method SetBio(ctx:Object, cbSend:Int(ctx:Object, buf:Byte Ptr, length:Int), ..
			cbRecv:Int(ctx:Object, buf:Byte Ptr, length:Int), ..
			cbTimeout:Int(ctx:Object, buf:Byte Ptr, length:Int, timeout:Int))
?
		_bioCtx = ctx
		_cbSend = cbSend
		_cbRecv = cbRecv
		_cbTimeout = cbTimeout
		
		Local cbs:Byte Ptr
		Local cbr:Byte Ptr
		Local cbt:Byte Ptr
		
		If cbSend Then
			cbs = _sendCallback
		End If
		
		If cbRecv Then
			cbr = _recvCallback
		End If
		
		If cbTimeout Then
			cbt = _timeoutCallback
		End If
		
		mbedtls_ssl_set_bio(contextPtr, Self, cbs, cbr, cbt)
	End Method
	
	Rem
	bbdoc: Sets the timer callbacks (Mandatory for DTLS.)
	End Rem
	Method SetTimerCallbacks(ctx:Object, set(ctx:Object, intMs:Int, finMs:Int), get:Int(ctx:Object))
		_timerCtx = ctx
		_cbTimerSet = set
		_cbTimerGet = get
		
		mbedtls_ssl_set_timer_cb(contextPtr, Self, _timerSetCallback, _timerGetCallback)
	End Method

	Rem
	bbdoc: Performs the SSL handshake.
	returns: 0 if successful, or #MBEDTLS_ERR_SSL_WANT_READ or #MBEDTLS_ERR_SSL_WANT_WRITE, or #MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED (see below), or a specific SSL error code.
	about: If this method returns something other than 0 or #MBEDTLS_ERR_SSL_WANT_READ/WRITE, then
	the ssl context becomes unusable, and you should either free it or call SessionReset() on it
	before re-using it for a new connection; the current connection must be closed.
	
	If DTLS is in use, then you may choose to handle #MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED specially
	for logging purposes, as it is an expected return value rather than an actual error, but you still
	need to reset/free the context.
	End Rem
	Method Handshake:Int()
		Return mbedtls_ssl_handshake(contextPtr)
	End Method
	
	Rem
	bbdoc: Performs a single step of the SSL handshake.
	returns: 0 if successful, or #MBEDTLS_ERR_SSL_WANT_READ or #MBEDTLS_ERR_SSL_WANT_WRITE, or a specific SSL error code.
	End Rem
	Method HandshakeStep:Int()
		Return mbedtls_ssl_handshake_step(contextPtr)
	End Method
	
	Rem
	bbdoc: Initiates an SSL renegotiation on the running connection.
	about: Client: perform the renegotiation right now.
	Server: request renegotiation, which will be performed during the next call to
	#Read() if honored by client.
	End Rem
	Method Renegotiate:Int()
		Return mbedtls_ssl_renegotiate(contextPtr)
	End Method
	
	Rem
	bbdoc: Reads at most @length application data bytes.
	returns: the number of bytes read, or 0 for EOF, or #MBEDTLS_ERR_SSL_WANT_READ or #MBEDTLS_ERR_SSL_WANT_WRITE, or #MBEDTLS_ERR_SSL_CLIENT_RECONNECT (see below), or another negative error code.
	about: If this method returns something other than a positive value or 
	#MBEDTLS_ERR_SSL_WANT_READ/WRITE or #MBEDTLS_ERR_SSL_CLIENT_RECONNECT, then the ssl
	context becomes unusable, and you should either free it or call #SessonReset() on it 
	before re-using it for a new connection; the current connection must be closed.
	End Rem
	Method Read:Int(buf:Byte Ptr, length:Int)
		Return bmx_mbedtls_ssl_read(contextPtr, buf, length)
	End Method

	Method Write:Int(buf:Byte Ptr, length:Int)
		Return bmx_mbedtls_ssl_write(contextPtr, buf, length)
	End Method
	
	Rem
	bbdoc: Returns the number of data bytes available to read.
	returns: How many bytes are available in the read buffer.
	End Rem
	Method GetBytesAvail:Int()
		Return mbedtls_ssl_get_bytes_avail(contextPtr)
	End Method
	
	Rem
	bbdoc: Returns the result of the certificate verification.
	returns: 0 if successful, -1 if result is not available (eg because the handshake was aborted too early), or a combination of BADCERT_xxx and BADCRL_xxx flags, see x509.h
	End Rem
	Method GetVerifyResult:Int()
		Return mbedtls_ssl_get_verify_result(contextPtr)
	End Method
	
	Rem
	bbdoc: Returns the name of the current ciphersuite.
	End Rem
	Method GetCipherSuite:String()
		Return String.FromUTF8String(mbedtls_ssl_get_ciphersuite(contextPtr))
	End Method
	
	Rem
	bbdoc: Returns the current SSL version (SSLv3/TLSv1/etc).
	End Rem
	Method GetVersion:String()
		Return String.FromUTF8String(mbedtls_ssl_get_version(contextPtr))
	End Method
	
	Rem
	bbdoc: Returns the (maximum) number of bytes added by the record layer: header + encryption/MAC overhead (inc. padding)
	returns: Current maximum record expansion in bytes, or #MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE if compression is enabled, which makes expansion much less predictable
	End Rem
	Method GetRecordExpansion:Int()
		Return mbedtls_ssl_get_record_expansion(contextPtr)
	End Method

	Rem
	bbdoc: Notifies the peer that the connection is being closed.
	returns: 0 if successful, or a specific SSL error code.
	End Rem
	Method CloseNotify:Int()
		Return mbedtls_ssl_close_notify(contextPtr)
	End Method

?bmxng
	Function _sendCallback:Int(ctx:TSSLContext, buf:Byte Ptr, length:Size_T)
?Not bmxng
	Function _sendCallback:Int(ctx:TSSLContext, buf:Byte Ptr, length:Int)
?
		If TNetContext(ctx._bioCtx) Then
			Return bmx_mbedtls_net_cbsend(ctx._cbSend, TNetContext(ctx._bioCtx).contextPtr, buf, length)
		Else
			Return ctx._cbSend(ctx._bioCtx, buf, length)
		End If
	End Function

?bmxng
	Function _recvCallback:Int(ctx:TSSLContext, buf:Byte Ptr, length:Size_T)
?Not bmxng
	Function _recvCallback:Int(ctx:TSSLContext, buf:Byte Ptr, length:Int)
?
		If TNetContext(ctx._bioCtx) Then
			Return bmx_mbedtls_net_cbrecv(ctx._cbRecv, TNetContext(ctx._bioCtx).contextPtr, buf, length)
		Else
			Return ctx._cbRecv(ctx._bioCtx, buf, length)
		End If
	End Function

?bmxng
	Function _timeoutCallback:Int(ctx:TSSLContext, buf:Byte Ptr, length:Size_T, timeout:Int)
?Not bmxng
	Function _timeoutCallback:Int(ctx:TSSLContext, buf:Byte Ptr, length:Int, timeout:Int)
?
		If TNetContext(ctx._bioCtx) Then
			Return bmx_mbedtls_net_cbtimeout(ctx._cbTimeout, TNetContext(ctx._bioCtx).contextPtr, buf, length, timeout)
		Else
			Return ctx._cbTimeout(ctx._bioCtx, buf, length, timeout)
		End If
	End Function

	Function _timerSetCallback(ctx:TSSLContext, intMs:Int, finMs:Int)
		ctx._cbTimerSet(ctx._timerCtx, intMs, finMs)
	End Function
	
	Function _timerGetCallback:Int(ctx:TSSLContext)
		Return ctx._cbTimerGet(ctx._timerCtx)
	End Function
	
	Method Delete()
		If contextPtr Then
			bmx_mbedtls_ssl_free(contextPtr)
			contextPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TSSLConfig

	Field configPtr:Byte Ptr
	
	Field _cbDbg(ctx:Object, level:Int, file:String, line:Int, str:String)
	Field _dbgCtx:Object
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TSSLConfig()
		configPtr = bmx_mbedtls_ssl_config_init()
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Defaults:Int(endpoint:Int = MBEDTLS_SSL_IS_SERVER, transport:Int = MBEDTLS_SSL_TRANSPORT_STREAM, preset:Int = MBEDTLS_SSL_PRESET_DEFAULT)
		Return mbedtls_ssl_config_defaults(configPtr, endpoint, transport, preset)
	End Method
	
	Rem
	bbdoc: Sets the current endpoint type.
	about: One of #MBEDTLS_SSL_IS_CLIENT or #MBEDTLS_SSL_IS_SERVER.
	End Rem
	Method SetEndpoint(endpoint:Int)
		mbedtls_ssl_conf_endpoint(configPtr, endpoint)
	End Method
	
	Rem
	bbdoc: Set the transport type (TLS or DTLS). Default: TLS
	about: #MBEDTLS_SSL_TRANSPORT_STREAM for TLS, #MBEDTLS_SSL_TRANSPORT_DATAGRAM for DTLS.
	For DTLS, you must either provide a recv callback that doesn't block, or one that handles
	timeouts, see TSSLContext.SetBio(). You also need to provide timer callbacks with 
	TSSLContext.SetTimerCallbacks().
	End Rem
	Method SetTransport(transport:Int)
		mbedtls_ssl_conf_transport(configPtr, transport)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAuthMode(authMode:Int)
	End Method
	
	Rem
	bbdoc:
	End Rem
?bmxng
	Method RNG(cbRandom:Int(rng:Object, output:Byte Ptr, length:Size_T), rng:Object)
?Not bmxng
	Method RNG(cbRandom:Int(rng:Object, output:Byte Ptr, length:Int), rng:Object)
?
		If TRandContext(rng) Then
			mbedtls_ssl_conf_rng(configPtr, cbRandom, TRandContext(rng).contextPtr)
		Else
			bmx_mbedtls_ssl_conf_rng(configPtr, cbRandom, rng)
		End If
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method DBG(cbDebug(ctx:Object, level:Int, file:String, line:Int, str:String), ctx:Object)
		_cbDbg = cbDebug
		_dbgCtx = ctx
		mbedtls_ssl_conf_dbg(configPtr, _callbackDebug, Self)
	End Method
	
	Rem
	bbdoc: Set the threshold error level to handle globally all debug output.
	about: Debug messages that have a level over the threshold value are discarded.
	(Default value: 0 = No debug )
	End Rem
	Function SetDebugThreshold(threshold:Int)
		mbedtls_debug_set_threshold(threshold)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method CaChain(chain:TX509Cert, crl:TX509CertRevList = Null)
		If crl Then
			mbedtls_ssl_conf_ca_chain(configPtr, chain.certPtr, crl.certPtr)
		Else
			mbedtls_ssl_conf_ca_chain(configPtr, chain.certPtr, Null)
		End If
	End Method
	
	Function _callbackDebug(ctx:Object, level:Int, file:Byte Ptr, line:Int, str:Byte Ptr)
		Local f:String = String.FromUTF8String(file)
		Local s:String = String.FromUTF8String(str)
		Local config:TSSLConfig = TSSLConfig(ctx)
		config._cbDbg(config._dbgCtx, level, f, line, s)
	End Function

	Rem
	bbdoc: 
	End Rem
	Method OwnCert:Int(cert:TX509Cert, pkContext:TPkContext)
		Return mbedtls_ssl_conf_own_cert(configPtr, cert.certPtr, pkContext.contextPtr)
	End Method
	
	Method Delete()
		If configPtr Then
			bmx_mbedtls_ssl_config_free(configPtr)
			configPtr = Null
		End If
	End Method

End Type

Type TSSLSession

	Field sessionPtr:Byte Ptr
	
End Type

Rem
bbdoc: Container for an X.509 certificate.
about: Ther certificate may be chained.
End Rem
Type TX509Cert

	Field certPtr:Byte Ptr
	Field _owned:Int
	
	Function _create:TX509Cert(certPtr:Byte Ptr)
		If certPtr Then
			Local this:TX509Cert = New TX509Cert
			this.certPtr = certPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method Create:TX509Cert()
		certPtr = bmx_mbedtls_x509_crt_init()
		_owned = True
		Return Self
	End Method
	
	Rem
	bbdoc: Parses one or more certificates and add them to the chained list.
	about: Parses permissively. If some certificates can be parsed, the result is the number
	of failed certificates it encountered. If none complete correctly, the first error is returned.
	End Rem
	Method Parse:Int(buf:Byte Ptr, buflen:Int)
		Return bmx_mbedtls_x509_crt_parse(certPtr, buf, buflen)
	End Method

	Rem
	bbdoc: Parses a file with one or more certificates (usually .pem extension)
	End Rem
	Method ParseFile:Int(path:String)
		Local Status:Int
		Local buf:Byte[] = LoadByteArray(path)

		buf = buf[..buf.length + 1]
		buf[buf.length - 1] = 0

		Status = Parse(buf, buf.length)

		Return Status
	End Method

	Rem
	bbbdoc: 
	End Rem
	Method GetNext:TX509Cert()
		Return TX509Cert._create(bmx_mbedtls_x509_crt_next(certPtr))
	End Method
	
	Method Delete()
		If certPtr And _owned Then
			bmx_mbedtls_x509_crt_free(certPtr)
			certPtr = Null
		End If
	End Method

End Type

Type TX509CertRevList

	Field certPtr:Byte Ptr

End Type

Rem
bbdoc: 
End Rem
Type TPkContext

	Field contextPtr:Byte Ptr
	
	Method Create:TPkContext()
		contextPtr = bmx_mbedtls_pk_init()
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ParseKey:Int(key:Byte Ptr, keylen:Int, pwd:Byte Ptr = Null, pwdlen:Int = 0)
		Return bmx_mbedtls_pk_parse_key(contextPtr, key, keylen, pwd, pwdlen)
	End Method
	
	Rem
	bbdoc: Load a private key from file (usually .pem extension)
	End Rem
	Method ParseKeyFile:Int(Path:String)
		Local Status:Int
		Local buf:Byte[] = LoadByteArray(path)

		buf = buf[..buf.length + 1]
		buf[buf.length - 1] = 0

		' Warning: password argument hardcoded to NULL
		Status = ParseKey(buf, buf.length)

		Return Status
	End Method

	Rem
	bbdoc: 
	End Rem
	Method ParseKeyString:Int(key:String, pwd:String = Null)
		Return bmx_mbedtls_pk_parse_key_string(contextPtr, key, pwd)
	End Method
	
	Method Delete()
		If contextPtr Then
			bmx_mbedtls_pk_free(contextPtr)
			contextPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TSSLCacheContext

	Field contextPtr:Byte Ptr
	
	
	
	Function Get:Int()
	End Function
End Type

Type TEntropyContext

	Field contextPtr:Byte Ptr
	
	Method Create:TEntropyContext()
		contextPtr = bmx_mbedtls_entropy_init()
		Return Self
	End Method
	
End Type

Rem
bbdoc: Counter mode Deterministic Random Byte Generator
End Rem
Type TRandContext

	Field contextPtr:Byte Ptr
	
	Method Create:TRandContext()
		contextPtr = bmx_mbedtls_ctr_drbg_init()
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
?bmxng
	Method Seed:Int(cbEntropy:Int(entropy:Object, buf:Byte Ptr, length:Size_T), entropy:Object, custom:Byte Ptr = Null, length:Size_T = 0)
?Not bmxng
	Method Seed:Int(cbEntropy:Int(entropy:Object, buf:Byte Ptr, length:Int), entropy:Object, custom:Byte Ptr = Null, length:Int = 0)
?
		If TEntropyContext(entropy) Then
			Return mbedtls_ctr_drbg_seed(contextPtr, cbEntropy, TEntropyContext(entropy).contextPtr, custom, length)
		Else
			Return bmx_mbedtls_ctr_drbg_seed(contextPtr, cbEntropy, entropy, custom, length)
		End If
	End Method

	Method Delete()
		If contextPtr Then
			bmx_mbedtls_ctr_drbg_free(contextPtr)
			contextPtr = Null
		End If
	End Method

End Type

