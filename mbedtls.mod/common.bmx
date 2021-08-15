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

?win32
Import "-lws2_32"
?

Import "source.bmx"


Extern

	Function mbedtls_strerror(errornum:Int, buf:Byte Ptr, length:Int)

	Function bmx_mbedtls_net_init:Byte Ptr()
	Function bmx_mbedtls_net_free(handle:Byte Ptr)
	Function bmx_mbedtls_net_delete(handle:Byte Ptr)
	Function bmx_mbedtls_net_bind:Int(handle:Byte Ptr, bindIp:String, port:String, proto:Int)
	Function bmx_mbedtls_net_connect:Int(handle:Byte Ptr, host:String, port:String, proto:Int)
	Function bmx_mbedtls_net_recv:Int(handle:Byte Ptr, buf:Byte Ptr, length:Int)
	Function bmx_mbedtls_net_recv_timeout:Int(handle:Byte Ptr, buf:Byte Ptr, length:Int, timeout:Int)
	Function bmx_mbedtls_net_send:Int(handle:Byte Ptr, buf:Byte Ptr, length:Int)
	Function bmx_mbedtls_net_usleep(usec:Int)
	Function mbedtls_net_accept:Int(handle:Byte Ptr, client:Byte Ptr, buf:Byte Ptr, size:Int, length:Int Ptr)
?bmxng	
	Function bmx_mbedtls_net_cbsend:Int(callback:Byte Ptr, handle:Byte Ptr, buf:Byte Ptr, length:Size_T)
	Function bmx_mbedtls_net_cbrecv:Int(callback:Byte Ptr, handle:Byte Ptr, buf:Byte Ptr, length:Size_T)
	Function bmx_mbedtls_net_cbtimeout:Int(callback:Byte Ptr, handle:Byte Ptr, buf:Byte Ptr, length:Size_T, timeout:Int)
?Not bmxng
	Function bmx_mbedtls_net_cbsend:Int(callback:Byte Ptr, handle:Byte Ptr, buf:Byte Ptr, length:Int)
	Function bmx_mbedtls_net_cbrecv:Int(callback:Byte Ptr, handle:Byte Ptr, buf:Byte Ptr, length:Int)
	Function bmx_mbedtls_net_cbtimeout:Int(callback:Byte Ptr, handle:Byte Ptr, buf:Byte Ptr, length:Int, timeout:Int)
?
?bmxng	
	Function NetSend:Int(ctx:Object, buf:Byte Ptr, length:Size_T) = "mbedtls_net_send"
	Function NetRecv:Int(ctx:Object, buf:Byte Ptr, length:Size_T) = "mbedtls_net_recv"
?Not bmxng
	Function NetSend:Int(ctx:Object, buf:Byte Ptr, length:Int) = "mbedtls_net_send"
	Function NetRecv:Int(ctx:Object, buf:Byte Ptr, length:Int) = "mbedtls_net_recv"
?

	Function mbedtls_net_set_block:Int(handle:Byte Ptr)
	Function mbedtls_net_set_nonblock:Int(handle:Byte Ptr)
	Function mbedtls_debug_set_threshold(threshold:Int)
	
	Function bmx_mbedtls_ssl_init:Byte Ptr()
	Function bmx_mbedtls_ssl_free(handle:Byte Ptr)
	Function bmx_mbedtls_ssl_read:Int(handle:Byte Ptr, buf:Byte Ptr, length:Int)
	Function bmx_mbedtls_ssl_write:Int(handle:Byte Ptr, buf:Byte Ptr, length:Int)
	
	Function mbedtls_ssl_setup:Int(handle:Byte Ptr, config:Byte Ptr)
	Function mbedtls_ssl_session_reset:Int(handle:Byte Ptr)
	Function mbedtls_ssl_set_bio(handle:Byte Ptr, ctx:Object, cbs:Byte Ptr, cbr:Byte Ptr, cbt:Byte Ptr)
	Function mbedtls_ssl_set_timer_cb(handle:Byte Ptr, ctx:Object, cbs:Byte Ptr, cbg:Byte Ptr)
	Function mbedtls_ssl_handshake:Int(handle:Byte Ptr)
	Function mbedtls_ssl_handshake_step:Int(handle:Byte Ptr)
	Function mbedtls_ssl_get_ciphersuite:Byte Ptr(handle:Byte Ptr)
	Function mbedtls_ssl_get_verify_result:Int(handle:Byte Ptr)
?bmxng
	Function mbedtls_ssl_get_bytes_avail:Size_T(handle:Byte Ptr)
?Not bmxng
	Function mbedtls_ssl_get_bytes_avail:Int(handle:Byte Ptr)
?
	Function mbedtls_ssl_get_version:Byte Ptr(handle:Byte Ptr)
	Function mbedtls_ssl_get_record_expansion:Int(handle:Byte Ptr)
	Function mbedtls_ssl_renegotiate:Int(handle:Byte Ptr)
	Function mbedtls_ssl_close_notify:Int(handle:Byte Ptr)

	Function bmx_mbedtls_ssl_config_init:Byte Ptr()
	Function bmx_mbedtls_ssl_config_free(handle:Byte Ptr)

	Function mbedtls_ssl_config_defaults:Int(handle:Byte Ptr, endpoint:Int, transport:Int, preset:Int)
	Function mbedtls_ssl_conf_endpoint(handle:Byte Ptr, endpoint:Int)
	Function mbedtls_ssl_conf_transport(handle:Byte Ptr, transport:Int)
	Function mbedtls_ssl_conf_own_cert:Int(handle:Byte Ptr, cert:Byte Ptr, pkContext:Byte Ptr)

	Function bmx_mbedtls_x509_crt_init:Byte Ptr()
	Function bmx_mbedtls_x509_crt_free(handle:Byte Ptr)
	Function bmx_mbedtls_x509_crt_parse:Int(handle:Byte Ptr, buf:Byte Ptr, buflen:Int)
	
	Function bmx_mbedtls_pk_init:Byte Ptr()
	Function bmx_mbedtls_pk_free(handle:Byte Ptr)
	Function bmx_mbedtls_pk_parse_key:Int(handle:Byte Ptr, key:Byte Ptr, keylen:Int, pwd:Byte Ptr, pwdlen:Int)
	Function bmx_mbedtls_pk_parse_key_string:Int(handle:Byte Ptr, key:String, pwd:String)
	
	Function bmx_mbedtls_ctr_drbg_init:Byte Ptr()
	Function bmx_mbedtls_ctr_drbg_free(handle:Byte Ptr)
	
	Function bmx_mbedtls_entropy_init:Byte Ptr()
	Function bmx_mbedtls_entropy_free(handle:Byte Ptr)
	
	Rem
	bbdoc: Retrieves entropy from the accumulator.
	returns: 0 if successful, or #MBEDTLS_ERR_ENTROPY_SOURCE_FAILED
	about: (Maximum length: #MBEDTLS_ENTROPY_BLOCK_SIZE
	End Rem
?bmxng
	Function EntropyFunc:Int(entropy:Object, output:Byte Ptr, length:Size_T) = "mbedtls_entropy_func"
	Function mbedtls_ctr_drbg_seed:Int(handle:Byte Ptr, cb:Byte Ptr, entropy:Byte Ptr, custom:Byte Ptr, length:Size_T)
	Function bmx_mbedtls_ctr_drbg_seed:Int(handle:Byte Ptr, cb:Byte Ptr, entropy:Object, custom:Byte Ptr, length:Size_T)
?Not bmxng
	Function EntropyFunc:Int(entropy:Object, output:Byte Ptr, length:Int) = "mbedtls_entropy_func"
	Function mbedtls_ctr_drbg_seed:Int(handle:Byte Ptr, cb:Byte Ptr, entropy:Byte Ptr, custom:Byte Ptr, length:Int)
	Function bmx_mbedtls_ctr_drbg_seed:Int(handle:Byte Ptr, cb:Byte Ptr, entropy:Object, custom:Byte Ptr, length:Int)
?
	
?bmxng
	Function RandomFunc:Int(rng:Object, output:Byte Ptr, length:Size_T) = "mbedtls_ctr_drbg_random"
?Not bmxng
	Function RandomFunc:Int(rng:Object, output:Byte Ptr, length:Int) = "mbedtls_ctr_drbg_random"
?	
	Function mbedtls_ssl_conf_rng(handle:Byte Ptr, cb:Byte Ptr, rng:Byte Ptr)
	Function bmx_mbedtls_ssl_conf_rng(handle:Byte Ptr, cb:Byte Ptr, rng:Object)
	Function mbedtls_ssl_conf_dbg(handle:Byte Ptr, cb:Byte Ptr, dbg:Object)
	Function mbedtls_ssl_conf_ca_chain(handle:Byte Ptr, chain:Byte Ptr, crl:Byte Ptr)

End Extern

Rem
bbdoc: Failed to open a socket.
End Rem
Const MBEDTLS_ERR_NET_SOCKET_FAILED:Int = -$0042 
Rem
bbdoc: The connection to the given server / port failed.
end rem
Const MBEDTLS_ERR_NET_CONNECT_FAILED:Int = -$0044 
Rem
bbdoc: Binding of the socket failed.
end rem
Const MBEDTLS_ERR_NET_BIND_FAILED:Int = -$0046 
Rem
bbdoc: Could not listen on the socket.
end rem
Const MBEDTLS_ERR_NET_LISTEN_FAILED:Int = -$0048 
Rem
bbdoc: Could not accept the incoming connection.
end rem
Const MBEDTLS_ERR_NET_ACCEPT_FAILED:Int = -$004A 
Rem
bbdoc: Reading information from the socket failed.
end rem
Const MBEDTLS_ERR_NET_RECV_FAILED:Int = -$004C 
Rem
bbdoc: Sending information through the socket failed.
end rem
Const MBEDTLS_ERR_NET_SEND_FAILED:Int = -$004E 
Rem
bbdoc: Connection was reset by peer.
end rem
Const MBEDTLS_ERR_NET_CONN_RESET:Int = -$0050 
Rem
bbdoc: Failed to get an IP address for the given hostname.
end rem
Const MBEDTLS_ERR_NET_UNKNOWN_HOST:Int = -$0052 
Rem
bbdoc: Buffer is too small to hold the data.
end rem
Const MBEDTLS_ERR_NET_BUFFER_TOO_SMALL:Int = -$0043 
Rem
bbdoc: The context is invalid, eg because it was free()ed.
end rem
Const MBEDTLS_ERR_NET_INVALID_CONTEXT:Int = -$0045 

Rem
bbdoc: The backlog that listen() should use.
end rem
Const MBEDTLS_NET_LISTEN_BACKLOG:Int = 10

Rem
bbdoc: The TCP transport protocol
end rem
Const MBEDTLS_NET_PROTO_TCP:Int = 0
Rem
bbdoc: The UDP transport protocol
end rem
Const MBEDTLS_NET_PROTO_UDP:Int = 1


Rem
bbdoc: The requested feature is not available.
end rem
Const MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE:Int = -$7080 
Rem
bbdoc: Bad input parameters to function.
end rem
Const MBEDTLS_ERR_SSL_BAD_INPUT_DATA:Int = -$7100 
Rem
bbdoc: Verification of the message MAC failed.
end rem
Const MBEDTLS_ERR_SSL_INVALID_MAC:Int = -$7180 
Rem
bbdoc: An invalid SSL record was received.
end rem
Const MBEDTLS_ERR_SSL_INVALID_RECORD:Int = -$7200 
Rem
bbdoc: The connection indicated an EOF.
end rem
Const MBEDTLS_ERR_SSL_CONN_EOF:Int = -$7280 
Rem
bbdoc: An unknown cipher was received.
end rem
Const MBEDTLS_ERR_SSL_UNKNOWN_CIPHER:Int = -$7300 
Rem
bbdoc: The server has no ciphersuites in common with the client.
end rem
Const MBEDTLS_ERR_SSL_NO_CIPHER_CHOSEN:Int = -$7380 
Rem
bbdoc: No RNG was provided to the SSL module.
end rem
Const MBEDTLS_ERR_SSL_NO_RNG:Int = -$7400 
Rem
bbdoc: No client certification received from the client, but required by the authentication mode.
end rem
Const MBEDTLS_ERR_SSL_NO_CLIENT_CERTIFICATE:Int = -$7480 
Rem
bbdoc: Our own certificate(s) is/are too large to send in an SSL message.
end rem
Const MBEDTLS_ERR_SSL_CERTIFICATE_TOO_LARGE:Int = -$7500 
Rem
bbdoc: The own certificate is not set, but needed by the server.
end rem
Const MBEDTLS_ERR_SSL_CERTIFICATE_REQUIRED:Int = -$7580 
Rem
bbdoc: The own private key or pre-shared key is not set, but needed.
end rem
Const MBEDTLS_ERR_SSL_PRIVATE_KEY_REQUIRED:Int = -$7600 
Rem
bbdoc: No CA Chain is set, but required to operate.
end rem
Const MBEDTLS_ERR_SSL_CA_CHAIN_REQUIRED:Int = -$7680 
Rem
bbdoc: An unexpected message was received from our peer.
end rem
Const MBEDTLS_ERR_SSL_UNEXPECTED_MESSAGE:Int = -$7700 
Rem
bbdoc: A fatal alert message was received from our peer.
end rem
Const MBEDTLS_ERR_SSL_FATAL_ALERT_MESSAGE:Int = -$7780 
Rem
bbdoc: Verification of our peer failed.
end rem
Const MBEDTLS_ERR_SSL_PEER_VERIFY_FAILED:Int = -$7800 
Rem
bbdoc: The peer notified us that the connection is going to be closed.
end rem
Const MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:Int = -$7880 
Rem
bbdoc: Processing of the ClientHello handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CLIENT_HELLO:Int = -$7900 
Rem
bbdoc: Processing of the ServerHello handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO:Int = -$7980 
Rem
bbdoc: Processing of the Certificate handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CERTIFICATE:Int = -$7A00 
Rem
bbdoc: Processing of the CertificateRequest handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CERTIFICATE_REQUEST:Int = -$7A80 
Rem
bbdoc: Processing of the ServerKeyExchange handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_SERVER_KEY_EXCHANGE:Int = -$7B00 
Rem
bbdoc: Processing of the ServerHelloDone handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_SERVER_HELLO_DONE:Int = -$7B80 
Rem
bbdoc: Processing of the ClientKeyExchange handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE:Int = -$7C00 
Rem
bbdoc: Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Read Public.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_RP:Int = -$7C80 
Rem
bbdoc: Processing of the ClientKeyExchange handshake message failed in DHM / ECDH Calculate Secret.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CLIENT_KEY_EXCHANGE_CS:Int = -$7D00 
Rem
bbdoc: Processing of the CertificateVerify handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CERTIFICATE_VERIFY:Int = -$7D80 
Rem
bbdoc: Processing of the ChangeCipherSpec handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_CHANGE_CIPHER_SPEC:Int = -$7E00 
Rem
bbdoc: Processing of the Finished handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_FINISHED:Int = -$7E80 
Rem
bbdoc: Memory allocation failed
end rem
Const MBEDTLS_ERR_SSL_ALLOC_FAILED:Int = -$7F00 
Rem
bbdoc: Hardware acceleration function returned with error
end rem
Const MBEDTLS_ERR_SSL_HW_ACCEL_FAILED:Int = -$7F80 
Rem
bbdoc: Hardware acceleration function skipped / left alone data
end rem
Const MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH:Int = -$6F80 
Rem
bbdoc: Processing of the compression / decompression failed
end rem
Const MBEDTLS_ERR_SSL_COMPRESSION_FAILED:Int = -$6F00 
Rem
bbdoc: Handshake protocol not within min/max boundaries
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_PROTOCOL_VERSION:Int = -$6E80 
Rem
bbdoc: Processing of the NewSessionTicket handshake message failed.
end rem
Const MBEDTLS_ERR_SSL_BAD_HS_NEW_SESSION_TICKET:Int = -$6E00 
Rem
bbdoc: Session ticket has expired.
end rem
Const MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED:Int = -$6D80 
Rem
bbdoc: Public key type mismatch (eg, asked for RSA key exchange and presented EC key)
end rem
Const MBEDTLS_ERR_SSL_PK_TYPE_MISMATCH:Int = -$6D00 
Rem
bbdoc: Unknown identity received (eg, PSK identity)
end rem
Const MBEDTLS_ERR_SSL_UNKNOWN_IDENTITY:Int = -$6C80 
Rem
bbdoc: Internal error (eg, unexpected failure in lower-level module)
end rem
Const MBEDTLS_ERR_SSL_INTERNAL_ERROR:Int = -$6C00 
Rem
bbdoc: A counter would wrap (eg, too many messages exchanged).
end rem
Const MBEDTLS_ERR_SSL_COUNTER_WRAPPING:Int = -$6B80 
Rem
bbdoc: Unexpected message at ServerHello in renegotiation.
end rem
Const MBEDTLS_ERR_SSL_WAITING_SERVER_HELLO_RENEGO:Int = -$6B00 
Rem
bbdoc: DTLS client must retry for hello verification
end rem
Const MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED:Int = -$6A80 
Rem
bbdoc: A buffer is too small to receive or write a message
end rem
Const MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL:Int = -$6A00 
Rem
bbdoc: None of the common ciphersuites is usable (eg, no suitable certificate, see debug messages).
end rem
Const MBEDTLS_ERR_SSL_NO_USABLE_CIPHERSUITE:Int = -$6980 
Rem
bbdoc: Connection requires a read call.
end rem
Const MBEDTLS_ERR_SSL_WANT_READ:Int = -$6900 
Rem
bbdoc: Connection requires a write call.
end rem
Const MBEDTLS_ERR_SSL_WANT_WRITE:Int = -$6880 
Rem
bbdoc: The operation timed out.
end rem
Const MBEDTLS_ERR_SSL_TIMEOUT:Int = -$6800 
Rem
bbdoc: The client initiated a reconnect from the same port.
end rem
Const MBEDTLS_ERR_SSL_CLIENT_RECONNECT:Int = -$6780 
Rem
bbdoc: Record header looks valid but is not expected.
end rem
Const MBEDTLS_ERR_SSL_UNEXPECTED_RECORD:Int = -$6700 
Rem
bbdoc: The alert message received indicates a non-fatal error.
end rem
Const MBEDTLS_ERR_SSL_NON_FATAL:Int = -$6680 
Rem
bbdoc: Couldn't set the hash for verifying CertificateVerify
end rem
Const MBEDTLS_ERR_SSL_INVALID_VERIFY_HASH:Int = -$6600 

Const MBEDTLS_SSL_IS_CLIENT:Int = 0
Const MBEDTLS_SSL_IS_SERVER:Int = 1

Const MBEDTLS_SSL_IS_NOT_FALLBACK:Int = 0
Const MBEDTLS_SSL_IS_FALLBACK:Int = 1

Const MBEDTLS_SSL_EXTENDED_MS_DISABLED:Int = 0
Const MBEDTLS_SSL_EXTENDED_MS_ENABLED:Int = 1

Const MBEDTLS_SSL_ETM_DISABLED:Int = 0
Const MBEDTLS_SSL_ETM_ENABLED:Int = 1

Const MBEDTLS_SSL_COMPRESS_NULL:Int = 0
Const MBEDTLS_SSL_COMPRESS_DEFLATE:Int = 1

Const MBEDTLS_SSL_VERIFY_NONE:Int = 0
Const MBEDTLS_SSL_VERIFY_OPTIONAL:Int = 1
Const MBEDTLS_SSL_VERIFY_REQUIRED:Int = 2
Const MBEDTLS_SSL_VERIFY_UNSET:Int = 3

Const MBEDTLS_SSL_LEGACY_RENEGOTIATION:Int = 0
Const MBEDTLS_SSL_SECURE_RENEGOTIATION:Int = 1

Const MBEDTLS_SSL_RENEGOTIATION_DISABLED:Int = 0
Const MBEDTLS_SSL_RENEGOTIATION_ENABLED:Int = 1

Const MBEDTLS_SSL_ANTI_REPLAY_DISABLED:Int = 0
Const MBEDTLS_SSL_ANTI_REPLAY_ENABLED:Int = 1

Const MBEDTLS_SSL_RENEGOTIATION_NOT_ENFORCED:Int = -1
Const MBEDTLS_SSL_RENEGO_MAX_RECORDS_DEFAULT:Int = 16

Const MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION:Int = 0
Const MBEDTLS_SSL_LEGACY_ALLOW_RENEGOTIATION:Int = 1
Const MBEDTLS_SSL_LEGACY_BREAK_HANDSHAKE:Int = 2

Const MBEDTLS_SSL_TRUNC_HMAC_DISABLED:Int = 0
Const MBEDTLS_SSL_TRUNC_HMAC_ENABLED:Int = 1
Const MBEDTLS_SSL_TRUNCATED_HMAC_LEN:Int = 10

Const MBEDTLS_SSL_SESSION_TICKETS_DISABLED:Int = 0
Const MBEDTLS_SSL_SESSION_TICKETS_ENABLED:Int = 1

Const MBEDTLS_SSL_CBC_RECORD_SPLITTING_DISABLED:Int = 0
Const MBEDTLS_SSL_CBC_RECORD_SPLITTING_ENABLED:Int = 1

Const MBEDTLS_SSL_ARC4_ENABLED:Int = 0
Const MBEDTLS_SSL_ARC4_DISABLED:Int = 1

Const MBEDTLS_SSL_PRESET_DEFAULT:Int = 0
Const MBEDTLS_SSL_PRESET_SUITEB:Int = 2

Const MBEDTLS_SSL_CERT_REQ_CA_LIST_ENABLED:Int = 1
Const MBEDTLS_SSL_CERT_REQ_CA_LIST_DISABLED:Int = 0

Const MBEDTLS_SSL_MAJOR_VERSION_3:Int = 3
Rem
bbdoc: SSL v3.0
end rem
Const MBEDTLS_SSL_MINOR_VERSION_0:Int = 0  
Rem
bbdoc: TLS v1.0
end rem
Const MBEDTLS_SSL_MINOR_VERSION_1:Int = 1  
Rem
bbdoc: TLS v1.1
end rem
Const MBEDTLS_SSL_MINOR_VERSION_2:Int = 2  
Rem
bbdoc: TLS v1.2
end rem
Const MBEDTLS_SSL_MINOR_VERSION_3:Int = 3  

Rem
bbdoc: TLS     
end rem
Const MBEDTLS_SSL_TRANSPORT_STREAM:Int = 0  
Rem
bbdoc: DTLS    
end rem
Const MBEDTLS_SSL_TRANSPORT_DATAGRAM:Int = 1  

Rem
bbdoc: Maximum host name defined in RFC 1035
end rem
Const MBEDTLS_SSL_MAX_HOST_NAME_LEN:Int = 255
