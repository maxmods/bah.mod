' Copyright (c) 2010-2011 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "common.bmx"

Rem
bbdoc: Settings for tracker requests.
about: You create it and fill it with your settings and then use #setSettings() to apply them. You have control over proxy and
authorization settings and also the user-agent that will be sent to the tracker. The userAgent is a good way to identify your client.
End Rem
Type TSessionSettings

	Field settingsPtr:Byte Ptr
	
	Method New()
		settingsPtr = bmx_torrent_sessionsettings_create()
	End Method

	Rem
	bbdoc: This is the client identification to the tracker. 
	about: The recommended format of this string is: "ClientName/ClientVersion libtorrent/libtorrentVersion". This name will not
	only be used when making HTTP requests, but also when sending extended headers to peers that support that extension.
	End Rem
	Method userAgent:String()
		Return bmx_torrent_sessionsettings_user_agent(settingsPtr)
	End Method
	
	Rem
	bbdoc: This is the client identification to the tracker. 
	about: The recommended format of this string is: "ClientName/ClientVersion libtorrent/libtorrentVersion". This name will not
	only be used when making HTTP requests, but also when sending extended headers to peers that support that extension.
	End Rem
	Method setUserAgent(value:String)
		bmx_torrent_sessionsettings_set_user_agent(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of seconds the tracker connection will wait from when it sent the request until it considers the tracker to have timed-out.
	about: Default value is 60 seconds.
	End Rem
	Method trackerCompletionTimeout:Int()
		Return bmx_torrent_sessionsettings_tracker_completion_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of seconds the tracker connection will wait from when it sent the request until it considers the tracker to have timed-out.
	about: Default value is 60 seconds.
	End Rem
	Method setTrackerCompletionTimeout(value:Int)
		bmx_torrent_sessionsettings_set_tracker_completion_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of seconds to wait to receive any data from the tracker.
	about: If no data is received for this number of seconds, the tracker will be considered as having timed out. If
	a tracker is down, this is the kind of timeout that will occur. The default value is 20 seconds.
	End Rem
	Method trackerReceiveTimeout:Int()
		Return bmx_torrent_sessionsettings_tracker_receive_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of seconds to wait to receive any data from the tracker.
	about: If no data is received for this number of seconds, the tracker will be considered as having timed out. If
	a tracker is down, this is the kind of timeout that will occur. The default value is 20 seconds.
	End Rem
	Method setTrackerReceiveTimeout(value:Int)
		bmx_torrent_sessionsettings_set_tracker_receive_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The time to wait for tracker responses when shutting down the session object.
	about: This is given in seconds. Default is 10 seconds.
	End Rem
	Method stopTrackerTimeout:Int()
		Return bmx_torrent_sessionsettings_stop_tracker_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: The time to wait for tracker responses when shutting down the session object.
	about: This is given in seconds. Default is 10 seconds.
	End Rem
	Method setStopTrackerTimeout(value:Int)
		bmx_torrent_sessionsettings_set_stop_tracker_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The maximum number of bytes in a tracker response.
	about: If a response size passes this number it will be rejected and the connection will be closed. On gzipped responses
	this size is measured on the uncompressed data. So, if you get 20 bytes of gzip response that'll expand to 2 megs, it
	will be interrupted before the entire response has been uncompressed (given your limit is lower than 2 megs). Default limit is 1 megabyte.
	End Rem
	Method trackerMaximumResponseLength:Int()
		Return bmx_torrent_sessionsettings_tracker_maximum_response_length(settingsPtr)
	End Method
	
	Rem
	bbdoc: The maximum number of bytes in a tracker response.
	about: If a response size passes this number it will be rejected and the connection will be closed. On gzipped responses
	this size is measured on the uncompressed data. So, if you get 20 bytes of gzip response that'll expand to 2 megs, it
	will be interrupted before the entire response has been uncompressed (given your limit is lower than 2 megs). Default limit is 1 megabyte.
	End Rem
	Method setTrackerMaximumResponseLength(value:Int)
		bmx_torrent_sessionsettings_set_tracker_maximum_response_length(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Controls the number of seconds from a request is sent until it times out if no piece response is returned.
	End Rem
	Method pieceTimeout:Int()
		Return bmx_torrent_sessionsettings_piece_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: Controls the number of seconds from a request is sent until it times out if no piece response is returned.
	End Rem
	Method setPieceTimeout(value:Int)
		bmx_torrent_sessionsettings_set_piece_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The length of the request queue given in the number of seconds it should take for the other end to send all the pieces.
	about: i.e. the actual number of requests depends on the download rate and this number.
	End Rem
	Method requestQueueTime:Float()
		Return bmx_torrent_sessionsettings_request_queue_time(settingsPtr)
	End Method
	
	Rem
	bbdoc: The length of the request queue given in the number of seconds it should take for the other end to send all the pieces.
	about: i.e. the actual number of requests depends on the download rate and this number.
	End Rem
	Method setRequestQueueTime(value:Float)
		bmx_torrent_sessionsettings_set_request_queue_time(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of outstanding block requests a peer is allowed to queue up in the client.
	about: If a peer sends more requests than this (before the first one has been handled) the last request will be dropped.
	The higher this is, the faster upload speeds the client can get to a single peer.
	End Rem
	Method maxAllowedInRequestQueue:Int()
		Return bmx_torrent_sessionsettings_max_allowed_in_request_queue(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of outstanding block requests a peer is allowed to queue up in the client.
	about: If a peer sends more requests than this (before the first one has been handled) the last request will be dropped.
	The higher this is, the faster upload speeds the client can get to a single peer.
	End Rem
	Method setMaxAllowedInRequestQueue(value:Int)
		bmx_torrent_sessionsettings_set_max_allowed_in_request_queue(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The maximum number of outstanding requests to send to a peer.
	about: This limit takes precedence over request_queue_time. i.e. no matter the download speed, the number
	of outstanding requests will never exceed this limit.
	End Rem
	Method maxOutRequestQueue:Int()
		Return bmx_torrent_sessionsettings_max_out_request_queue(settingsPtr)
	End Method
	
	Rem
	bbdoc: The maximum number of outstanding requests to send to a peer.
	about: This limit takes precedence over request_queue_time. i.e. no matter the download speed, the number
	of outstanding requests will never exceed this limit.
	End Rem
	Method setMaxOutRequestQueue(value:Int)
		bmx_torrent_sessionsettings_set_max_out_request_queue(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Pieces download limit, in seconds.
	about: If a whole piece can be downloaded in at least this number of seconds from a specific peer, the peer_connection will prefer
	requesting whole pieces at a time from this peer. The benefit of this is to better utilize disk caches by doing localized accesses
	and also to make it easier to identify bad peers if a piece fails the hash check.
	End Rem
	Method wholePiecesThreshold:Int()
		Return bmx_torrent_sessionsettings_whole_pieces_threshold(settingsPtr)
	End Method
	
	Rem
	bbdoc: Pieces download limit, in seconds.
	about: If a whole piece can be downloaded in at least this number of seconds from a specific peer, the peer_connection will prefer
	requesting whole pieces at a time from this peer. The benefit of this is to better utilize disk caches by doing localized accesses
	and also to make it easier to identify bad peers if a piece fails the hash check.
	End Rem
	Method setWholePiecesThreshold(value:Int)
		bmx_torrent_sessionsettings_set_whole_pieces_threshold(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of seconds the peer connection should wait (for any activity on the peer connection) before closing it due to time out.
	about: This defaults to 120 seconds, since that's what's specified in the protocol specification. After half the time out, a keep
	alive message is sent.
	End Rem
	Method peerTimeout:Int()
		Return bmx_torrent_sessionsettings_peer_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of seconds the peer connection should wait (for any activity on the peer connection) before closing it due to time out.
	about: This defaults to 120 seconds, since that's what's specified in the protocol specification. After half the time out, a keep
	alive message is sent.
	End Rem
	Method setPeerTimeout(value:Int)
		bmx_torrent_sessionsettings_set_peer_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The same as #peerTimeout but applies only to url seeds.
	about: This value defaults to 20 seconds.
	End Rem
	Method urlseedTimeout:Int()
		Return bmx_torrent_sessionsettings_urlseed_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: The same as #setPeerTimeout but applies only to url seeds.
	about: This value defaults to 20 seconds.
	End Rem
	Method setUrlseedTimeout(value:Int)
		bmx_torrent_sessionsettings_set_urlseed_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Controls the pipelining with the web server.
	about: When using persistent connections to HTTP 1.1 servers, the client is allowed to send more requests before the first response
	is received. This number controls the number of outstanding requests to use with url-seeds. Default is 5.
	End Rem
	Method urlseedPipelineSize:Int()
		Return bmx_torrent_sessionsettings_urlseed_pipeline_size(settingsPtr)
	End Method
	
	Rem
	bbdoc: Controls the pipelining with the web server.
	about: When using persistent connections to HTTP 1.1 servers, the client is allowed to send more requests before the first response
	is received. This number controls the number of outstanding requests to use with url-seeds. Default is 5.
	End Rem
	Method setUrlseedPipelineSize(value:Int)
		bmx_torrent_sessionsettings_set_urlseed_pipeline_size(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Time to wait until a new retry takes place.
	End Rem
	Method urlseedWaitRetry:Int()
		Return bmx_torrent_sessionsettings_urlseed_wait_retry(settingsPtr)
	End Method
	
	Rem
	bbdoc: Time to wait until a new retry takes place.
	End Rem
	Method SetUrlseedWaitRetry(value:Int)
		bmx_torrent_sessionsettings_set_urlseed_wait_retry(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The upper limit on the total number of files this session will keep open.
	about: The reason why files are left open at all is that some anti virus software hooks on every file close, and scans the file
	for viruses. Deferring the closing of the files will be the difference between a usable system and a completely hogged down system.
	Most operating systems also has a limit on the total number of file descriptors a process may have open. It is usually a good
	idea to find this limit and set the number of connections and the number of files limits so their sum is slightly below it.
	End Rem
	Method filePoolSize:Int()
		Return bmx_torrent_sessionsettings_file_pool_size(settingsPtr)
	End Method
	
	Rem
	bbdoc: The upper limit on the total number of files this session will keep open.
	about: The reason why files are left open at all is that some anti virus software hooks on every file close, and scans the file
	for viruses. Deferring the closing of the files will be the difference between a usable system and a completely hogged down system.
	Most operating systems also has a limit on the total number of file descriptors a process may have open. It is usually a good
	idea to find this limit and set the number of connections and the number of files limits so their sum is slightly below it.
	End Rem
	Method setFilePoolSize(value:Int)
		bmx_torrent_sessionsettings_set_file_pool_size(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Determines if connections from the same IP address as existing connections should be rejected or not.
	about: Multiple connections from the same IP address is not allowed by default, to prevent abusive behavior by peers. It may
	be useful to allow such connections in cases where simulations are run on the same machie, and all peers in a swarm has the same IP address.
	End Rem
	Method allowMultipleConnectionsPerIp:Int()
		Return bmx_torrent_sessionsettings_allow_multiple_connections_per_ip(settingsPtr)
	End Method
	
	Rem
	bbdoc: Determines if connections from the same IP address as existing connections should be rejected or not.
	about: Multiple connections from the same IP address is not allowed by default, to prevent abusive behavior by peers. It may
	be useful to allow such connections in cases where simulations are run on the same machie, and all peers in a swarm has the same IP address.
	End Rem
	Method setAllowMultipleConnectionsPerIp(value:Int)
		bmx_torrent_sessionsettings_set_allow_multiple_connections_per_ip(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The maximum times we try to connect to a peer before stop connecting again.
	about: If a peer succeeds, the failcounter is reset. If a peer is retrieved from a peer source (other than DHT) the failcount
	is decremented by one, allowing another try.
	End Rem
	Method maxFailcount:Int()
		Return bmx_torrent_sessionsettings_max_failcount(settingsPtr)
	End Method
	
	Rem
	bbdoc: The maximum times we try to connect to a peer before stop connecting again.
	about: If a peer succeeds, the failcounter is reset. If a peer is retrieved from a peer source (other than DHT) the failcount
	is decremented by one, allowing another try.
	End Rem
	Method setMaxFailcount(value:Int)
		bmx_torrent_sessionsettings_set_max_failcount(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The time to wait between connection attempts.
	about: If the peer fails, the time is multiplied by fail counter.
	End Rem
	Method minReconnectTime:Int()
		Return bmx_torrent_sessionsettings_min_reconnect_time(settingsPtr)
	End Method
	
	Rem
	bbdoc: The time to wait between connection attempts.
	about: If the peer fails, the time is multiplied by fail counter.
	End Rem
	Method setMinReconnectTime(value:Int)
		bmx_torrent_sessionsettings_set_min_reconnect_time(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of seconds to wait after a connection attempt is initiated to a peer until it is considered as having timed out.
	about: The default is 10 seconds. This setting is especially important in case the number of half-open connections are limited, since
	stale half-open connection may delay the connection of other peers considerably.
	End Rem
	Method peerConnectTimeout:Int()
		Return bmx_torrent_sessionsettings_peer_connect_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of seconds to wait after a connection attempt is initiated to a peer until it is considered as having timed out.
	about: The default is 10 seconds. This setting is especially important in case the number of half-open connections are limited, since
	stale half-open connection may delay the connection of other peers considerably.
	End Rem
	Method setPeerConnectTimeout(value:Int)
		bmx_torrent_sessionsettings_set_peer_connect_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: If set to true, upload, download and unchoke limits are ignored for peers on the local network.
	End Rem
	Method ignoreLimitsOnLocalNetwork:Int()
		Return bmx_torrent_sessionsettings_ignore_limits_on_local_network(settingsPtr)
	End Method
	
	Rem
	bbdoc: If set to true, upload, download and unchoke limits are ignored for peers on the local network.
	End Rem
	Method setIgnoreLimitsOnLocalNetwork(value:Int)
		bmx_torrent_sessionsettings_set_ignore_limits_on_local_network(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of connection attempts that are made per second.
	about: If a number &lt;= 0 is specified, it will default to 200 connections per second.
	End Rem
	Method connectionSpeed:Int()
		Return bmx_torrent_sessionsettings_connection_speed(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of connection attempts that are made per second.
	about: If a number &lt;= 0 is specified, it will default to 200 connections per second.
	End Rem
	Method setConnectionSpeed(value:Int)
		bmx_torrent_sessionsettings_set_connection_speed(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Controls whether have messages will be sent to peers that already have the piece.
	about: This is typically not necessary, but it might be necessary for collecting statistics in some cases. Default is false.
	End Rem
	Method sendRedundantHave:Int()
		Return bmx_torrent_sessionsettings_send_redundant_have(settingsPtr)
	End Method
	
	Rem
	bbdoc: Controls whether have messages will be sent to peers that already have the piece.
	about: This is typically not necessary, but it might be necessary for collecting statistics in some cases. Default is false.
	End Rem
	Method setSendRedundantHave(value:Int)
		bmx_torrent_sessionsettings_set_send_redundant_have(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Prevents outgoing bitfields from being full.
	about: If the client is seed, a few bits will be set to 0, and later filled in with have-messages. This is to prevent certain ISPs
	from stopping people from seeding.
	End Rem
	Method lazyBitfields:Int()
		Return bmx_torrent_sessionsettings_lazy_bitfields(settingsPtr)
	End Method
	
	Rem
	bbdoc: Prevents outgoing bitfields from being full.
	about: If the client is seed, a few bits will be set to 0, and later filled in with have-messages. This is to prevent certain ISPs
	from stopping people from seeding.
	End Rem
	Method setLazyBitfields(value:Int)
		bmx_torrent_sessionsettings_set_lazy_bitfields(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: If a peer is uninteresting and uninterested for longer than this number of seconds, it will be disconnected.
	about: Default is 10 minutes.
	End Rem
	Method inactivityTimeout:Int()
		Return bmx_torrent_sessionsettings_inactivity_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: If a peer is uninteresting and uninterested for longer than this number of seconds, it will be disconnected.
	about: Default is 10 minutes.
	End Rem
	Method setInactivityTimeout(value:Int)
		bmx_torrent_sessionsettings_set_inactivity_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of seconds between chokes/unchokes.
	about: On this interval, peers are re-evaluated for being choked/unchoked. This is defined as 30 seconds
	in the protocol, and it should be significantly longer than what it takes for TCP to ramp up to it's max rate.
	End Rem
	Method unchokeInterval:Int()
		Return bmx_torrent_sessionsettings_unchoke_interval(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of seconds between chokes/unchokes.
	about: On this interval, peers are re-evaluated for being choked/unchoked. This is defined as 30 seconds
	in the protocol, and it should be significantly longer than what it takes for TCP to ramp up to it's max rate.
	End Rem
	Method setUnchokeInterval(value:Int)
		bmx_torrent_sessionsettings_set_unchoke_interval(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of unchoke intervals between each optimistic unchoke interval.
	about: On this timer, the currently optimistically unchoked peer will change.
	End Rem
	Method optimisticUnchokeInterval:Int()
		Return bmx_torrent_sessionsettings_optimistic_unchoke_interval(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of unchoke intervals between each optimistic unchoke interval.
	about: On this timer, the currently optimistically unchoked peer will change.
	End Rem
	Method setOptimisticUnchokeInterval(value:Int)
		bmx_torrent_sessionsettings_set_optimistic_unchoke_interval(settingsPtr, value)
	End Method
	
	'Method announceIp:address()
	'	Return bmx_torrent_sessionsettings_announce_ip(settingsPtr)
	'End Method
	
	'Method setAnnounceIp(value:address)
	'	bmx_torrent_sessionsettings_set_announce_ip(settingsPtr, value)
	'End Method
	
	Rem
	bbdoc: The number of peers we want from each tracker request.
	about: It defines what is sent as the &amp;num_want= parameter to the tracker.
	End Rem
	Method numWant:Int()
		Return bmx_torrent_sessionsettings_num_want(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of peers we want from each tracker request.
	about: It defines what is sent as the &amp;num_want= parameter to the tracker.
	End Rem
	Method setNumWant(value:Int)
		bmx_torrent_sessionsettings_set_num_want(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: Specifies the number of pieces we need before we switch to rarest first picking.
	about: This defaults to 4, which means the 4 first pieces in any torrent are picked at random, the following pieces are picked
	in rarest first order.
	End Rem
	Method initialPickerThreshold:Int()
		Return bmx_torrent_sessionsettings_initial_picker_threshold(settingsPtr)
	End Method
	
	Rem
	bbdoc: Specifies the number of pieces we need before we switch to rarest first picking.
	about: This defaults to 4, which means the 4 first pieces in any torrent are picked at random, the following pieces are picked
	in rarest first order.
	End Rem
	Method setInitialPickerThreshold(value:Int)
		bmx_torrent_sessionsettings_set_initial_picker_threshold(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of pieces we allow peers to download from us without being unchoked.
	End Rem
	Method allowedFastSetSize:Int()
		Return bmx_torrent_sessionsettings_allowed_fast_set_size(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of pieces we allow peers to download from us without being unchoked.
	End Rem
	Method setAllowedFastSetSize(value:Int)
		bmx_torrent_sessionsettings_set_allowed_fast_set_size(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: The number of bytes each connection is allowed to have waiting in the disk I/O queue before it is throttled back.
	about: This limit is meant to stop fast internet connections to queue up bufferes indefinitely on slow hard-drives or storage.
	End Rem
	Method maxQueuedDiskBytes:Int()
		Return bmx_torrent_sessionsettings_max_queued_disk_bytes(settingsPtr)
	End Method
	
	Rem
	bbdoc: The number of bytes each connection is allowed to have waiting in the disk I/O queue before it is throttled back.
	about: This limit is meant to stop fast internet connections to queue up bufferes indefinitely on slow hard-drives or storage.
	End Rem
	Method setMaxQueuedDiskBytes(value:Int)
		bmx_torrent_sessionsettings_set_max_queued_disk_bytes(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method handshakeTimeout:Int()
		Return bmx_torrent_sessionsettings_handshake_timeout(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setHandshakeTimeout(value:Int)
		bmx_torrent_sessionsettings_set_handshake_timeout(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method useDhtAsFallback:Int()
		Return bmx_torrent_sessionsettings_use_dht_as_fallback(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setUseDhtAsFallback(value:Int)
		bmx_torrent_sessionsettings_set_use_dht_as_fallback(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method freeTorrentHashes:Int()
		Return bmx_torrent_sessionsettings_free_torrent_hashes(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setFreeTorrentHashes(value:Int)
		bmx_torrent_sessionsettings_set_free_torrent_hashes(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method upnpIgnoreNonrouters:Int()
		Return bmx_torrent_sessionsettings_upnp_ignore_nonrouters(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setUpnpIgnoreNonrouters(value:Int)
		bmx_torrent_sessionsettings_set_upnp_ignore_nonrouters(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method sendBufferWatermark:Int()
		Return bmx_torrent_sessionsettings_send_buffer_watermark(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setSendBufferWatermark(value:Int)
		bmx_torrent_sessionsettings_set_send_buffer_watermark(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method autoUploadSlots:Int()
		Return bmx_torrent_sessionsettings_auto_upload_slots(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setAutoUploadSlots(value:Int)
		bmx_torrent_sessionsettings_set_auto_upload_slots(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method useParoleMode:Int()
		Return bmx_torrent_sessionsettings_use_parole_mode(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setUseParoleMode(value:Int)
		bmx_torrent_sessionsettings_set_use_parole_mode(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method cacheSize:Int()
		Return bmx_torrent_sessionsettings_cache_size(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setCacheSize(value:Int)
		bmx_torrent_sessionsettings_set_cache_size(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method cacheExpiry:Int()
		Return bmx_torrent_sessionsettings_cache_expiry(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setCacheExpiry(value:Int)
		bmx_torrent_sessionsettings_set_cache_expiry(settingsPtr, value)
	End Method
	
	Method outgoingPorts(fromPort:Int Var, toPort:Int Var)
		bmx_torrent_sessionsettings_outgoing_ports(settingsPtr, Varptr fromPort, Varptr toPort)
	End Method
	
	Method setOutgoingPorts(fromPort:Int, toPort:Int)
		bmx_torrent_sessionsettings_set_outgoing_ports(settingsPtr, fromPort, toPort)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method peerTos:Int()
		Return bmx_torrent_sessionsettings_peer_tos(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setPeerTos(value:Int)
		bmx_torrent_sessionsettings_set_peer_tos(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method activeDownloads:Int()
		Return bmx_torrent_sessionsettings_active_downloads(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setActiveDownloads(value:Int)
		bmx_torrent_sessionsettings_set_active_downloads(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method activeSeeds:Int()
		Return bmx_torrent_sessionsettings_active_seeds(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setActiveSeeds(value:Int)
		bmx_torrent_sessionsettings_set_active_seeds(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method activeLimit:Int()
		Return bmx_torrent_sessionsettings_active_limit(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setActiveLimit(value:Int)
		bmx_torrent_sessionsettings_set_active_limit(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method dontCountSlowTorrents:Int()
		Return bmx_torrent_sessionsettings_dont_count_slow_torrents(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setDontCountSlowTorrents(value:Int)
		bmx_torrent_sessionsettings_set_dont_count_slow_torrents(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method autoManageInterval:Int()
		Return bmx_torrent_sessionsettings_auto_manage_interval(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setAutoManageInterval(value:Int)
		bmx_torrent_sessionsettings_set_auto_manage_interval(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method shareRatioLimit:Float()
		Return bmx_torrent_sessionsettings_share_ratio_limit(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setShareRatioLimit(value:Float)
		bmx_torrent_sessionsettings_set_share_ratio_limit(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method seedTimeRatioLimit:Float()
		Return bmx_torrent_sessionsettings_seed_time_ratio_limit(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setSeedTimeRatioLimit(value:Float)
		bmx_torrent_sessionsettings_set_seed_time_ratio_limit(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method seedTimeLimit:Int()
		Return bmx_torrent_sessionsettings_seed_time_limit(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setSeedTimeLimit(value:Int)
		bmx_torrent_sessionsettings_set_seed_time_limit(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method closeRedundantConnections:Int()
		Return bmx_torrent_sessionsettings_close_redundant_connections(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setCloseRedundantConnections(value:Int)
		bmx_torrent_sessionsettings_set_close_redundant_connections(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method autoScrapeInterval:Int()
		Return bmx_torrent_sessionsettings_auto_scrape_interval(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setAutoScrapeInterval(value:Int)
		bmx_torrent_sessionsettings_set_auto_scrape_interval(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method autoScrapeMinInterval:Int()
		Return bmx_torrent_sessionsettings_auto_scrape_min_interval(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setAutoScrapeMinInterval(value:Int)
		bmx_torrent_sessionsettings_set_auto_scrape_min_interval(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method maxPeerlistSize:Int()
		Return bmx_torrent_sessionsettings_max_peerlist_size(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setMaxPeerlistSize(value:Int)
		bmx_torrent_sessionsettings_set_max_peerlist_size(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method minAnnounceInterval:Int()
		Return bmx_torrent_sessionsettings_min_announce_interval(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setMinAnnounceInterval(value:Int)
		bmx_torrent_sessionsettings_set_min_announce_interval(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method prioritizePartialPieces:Int()
		Return bmx_torrent_sessionsettings_prioritize_partial_pieces(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setPrioritizePartialPieces(value:Int)
		bmx_torrent_sessionsettings_set_prioritize_partial_pieces(settingsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method autoManageStartup:Int()
		Return bmx_torrent_sessionsettings_auto_manage_startup(settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setAutoManageStartup(value:Int)
		bmx_torrent_sessionsettings_set_auto_manage_startup(settingsPtr, value)
	End Method
	
	Method Delete()
		If settingsPtr Then
			bmx_torrent_sessionsettings_free(settingsPtr)
			settingsPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: The base type that specific messages are derived from.
about: Every alert belongs to one or more category. There is a small cost involved in posting alerts. Only alerts that
belong to an enabled category are posted. Setting the alert bitmask to 0 will disable all alerts.
End Rem
Type TAlert

	Rem
	bbdoc: Alerts that report an error.
	about: This includes:
	<ul>
	<li>tracker errors</li>
	<li>tracker warnings</li>
	<li>file errors</li>
	<li>resume data failures</li>
	<li>web seed errors</li>
	<li>.torrent files errors</li>
	<li>listen socket errors</li>
	<li>port mapping errors</li>
	</ul>
	End Rem
	Const errorNotification:Int = $1
	Rem
	bbdoc: Alerts when peers send invalid requests, get banned or snubbed.
	End Rem
	Const peerNotification:Int = $2
	Rem
	bbdoc: Alerts for port mapping events.
	about: For NAT-PMP and UPnP.
	End Rem
	Const portMappingNotification:Int = $4
	Rem
	bbdoc: Alerts for events related to the storage.
	about: File errors and synchronization events for moving the storage, renaming files etc.
	End Rem
	Const storageNotification:Int = $8
	Rem
	bbdoc: Alerts for all tracker events.
	about: Includes announcing to trackers, receiving responses, warnings and errors
	End Rem
	Const trackerNotification:Int = $10
	Rem
	bbdoc: Alerts for when peers are connected and disconnected.
	End Rem
	Const debugNotification:Int = $20
	Rem
	bbdoc: Alerts for when a torrent or the session changes state.
	End Rem
	Const statusNotification:Int = $40
	Rem
	bbdoc: Alerts for when blocks are requested and completed.
	about: Also when pieces are completed.
	End Rem
	Const progressNotification:Int = $80
	Rem
	bbdoc: Alerts when a peer is blocked by the ip blocker or port blocker.
	End Rem
	Const ipBlockNotification:Int = $100
	Rem
	bbdoc: Alerts when some limit is reached that might limit the download or upload rate.
	End Rem
	Const performanceWarning:Int = $200
	Rem
	bbdoc: The full bitmask, representing all available categories.
	End Rem
	Const allCategories:Int = $ffffffff

	Field alertPtr:Byte Ptr
	
	Rem
	bbdoc: Generates a string describing the alert and the information bundled with it.
	about: This is mainly intended for debug and development use. It is not suitable to use this for applications that may be
	localized. Instead, handle each alert type individually and extract and render the information from the alert depending on the locale.
	End Rem
	Method message:String()
	End Method
	
	Rem
	bbdoc: Returns a bitmask specifying which categories this alert belong to.
	End Rem
	Method category:Int()
	End Method
	
	Rem
	bbdoc: Returns a string literal describing the type of the alert.
	about: It does not include any information that might be bundled with the alert.
	End Rem
	Method what:String()
	End Method
		
End Type

Type TTorrentAlert Extends TAlert

	Function _create:TTorrentAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentAlert = New TTorrentAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerAlert Extends TTorrentAlert

	Function _create:TPeerAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerAlert = New TPeerAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTrackerAlert Extends TTorrentAlert

	Function _create:TTrackerAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTrackerAlert = New TTrackerAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TFileRenamedAlert Extends TTorrentAlert

	Function _create:TFileRenamedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TFileRenamedAlert = New TFileRenamedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TFileRenameFailedAlert Extends TTorrentAlert

	Function _create:TFileRenameFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TFileRenameFailedAlert = New TFileRenameFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPerformanceAlert Extends TTorrentAlert

	Function _create:TPerformanceAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPerformanceAlert = New TPerformanceAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TStateChangedAlert Extends TTorrentAlert

	Function _create:TStateChangedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TStateChangedAlert = New TStateChangedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTrackerErrorAlert Extends TTrackerAlert

	Function _create:TTrackerErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTrackerErrorAlert = New TTrackerErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTrackerWarningAlert Extends TTrackerAlert

	Function _create:TTrackerErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTrackerErrorAlert = New TTrackerErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TScrapeReplyAlert Extends TTrackerAlert

	Function _create:TTrackerErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTrackerErrorAlert = New TTrackerErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TScrapeFailedAlert Extends TTrackerAlert

	Function _create:TScrapeFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TScrapeFailedAlert = New TScrapeFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTrackerReplyAlert Extends TTrackerAlert

	Function _create:TTrackerReplyAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTrackerReplyAlert = New TTrackerReplyAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TDhtReplyAlert Extends TTrackerAlert

	Function _create:TDhtReplyAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TDhtReplyAlert = New TDhtReplyAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTrackerAnnounceAlert Extends TTrackerAlert

	Function _create:TTrackerAnnounceAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTrackerAnnounceAlert = New TTrackerAnnounceAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type THashFailedAlert Extends TTorrentAlert

	Function _create:THashFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:THashFailedAlert = New THashFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerBanAlert Extends TPeerAlert

	Function _create:TPeerBanAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerBanAlert = New TPeerBanAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerUnsnubbedAlert Extends TPeerAlert

	Function _create:TPeerUnsnubbedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerUnsnubbedAlert = New TPeerUnsnubbedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerSnubbedAlert Extends TPeerAlert

	Function _create:TPeerSnubbedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerSnubbedAlert = New TPeerSnubbedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerErrorAlert Extends TPeerAlert

	Function _create:TPeerErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerErrorAlert = New TPeerErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerConnectAlert Extends TPeerAlert

	Function _create:TPeerConnectAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerConnectAlert = New TPeerConnectAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerDisconnectedAlert Extends TPeerAlert

	Function _create:TPeerDisconnectedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerDisconnectedAlert = New TPeerDisconnectedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TInvalidRequestAlert Extends TPeerAlert

	Function _create:TInvalidRequestAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TInvalidRequestAlert = New TInvalidRequestAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTorrentFinishedAlert Extends TTorrentAlert

	Function _create:TTorrentFinishedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentFinishedAlert = New TTorrentFinishedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPieceFinishedAlert Extends TTorrentAlert

	Function _create:TPieceFinishedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPieceFinishedAlert = New TPieceFinishedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TRequestDroppedAlert Extends TPeerAlert

	Function _create:TRequestDroppedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TRequestDroppedAlert = New TRequestDroppedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TBlockTimeoutAlert Extends TPeerAlert

	Function _create:TBlockTimeoutAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TBlockTimeoutAlert = New TBlockTimeoutAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TBlockFinishedAlert Extends TPeerAlert

	Function _create:TBlockFinishedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TBlockFinishedAlert = New TBlockFinishedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TBlockDownloadingAlert Extends TPeerAlert

	Function _create:TBlockDownloadingAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TBlockDownloadingAlert = New TBlockDownloadingAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TUnwantedBlockAlert Extends TPeerAlert

	Function _create:TUnwantedBlockAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TUnwantedBlockAlert = New TUnwantedBlockAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TStorageMovedAlert Extends TTorrentAlert

	Function _create:TStorageMovedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TStorageMovedAlert = New TStorageMovedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TStorageMovedFailedAlert Extends TTorrentAlert

	Function _create:TStorageMovedFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TStorageMovedFailedAlert = New TStorageMovedFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTorrentDeletedAlert Extends TTorrentAlert

	Function _create:TTorrentDeletedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentDeletedAlert = New TTorrentDeletedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTorrentDeleteFailedAlert Extends TTorrentAlert

	Function _create:TTorrentDeleteFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentDeleteFailedAlert = New TTorrentDeleteFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TSaveResumeDataAlert Extends TTorrentAlert

	Function _create:TSaveResumeDataAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TSaveResumeDataAlert = New TSaveResumeDataAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TSaveResumeDataFailedAlert Extends TTorrentAlert

	Function _create:TSaveResumeDataFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TSaveResumeDataFailedAlert = New TSaveResumeDataFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTorrentPausedAlert Extends TTorrentAlert

	Function _create:TTorrentPausedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentPausedAlert = New TTorrentPausedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTorrentResumedAlert Extends TTorrentAlert

	Function _create:TTorrentResumedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentResumedAlert = New TTorrentResumedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TTorrentCheckedAlert Extends TTorrentAlert

	Function _create:TTorrentCheckedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TTorrentCheckedAlert = New TTorrentCheckedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TUrlSeedAlert Extends TTorrentAlert

	Function _create:TUrlSeedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TUrlSeedAlert = New TUrlSeedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TFileErrorAlert Extends TTorrentAlert

	Function _create:TFileErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TFileErrorAlert = New TFileErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TMetadataFailedAlert Extends TTorrentAlert

	Function _create:TMetadataFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TMetadataFailedAlert = New TMetadataFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TMetadataReceivedAlert Extends TTorrentAlert

	Function _create:TMetadataReceivedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TMetadataReceivedAlert = New TMetadataReceivedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TUdpErrorAlert Extends TAlert

	Function _create:TUdpErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TUdpErrorAlert = New TUdpErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TExternalIpAlert Extends TAlert

	Function _create:TExternalIpAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TExternalIpAlert = New TExternalIpAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TListenFailedAlert Extends TAlert

	Function _create:TListenFailedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TListenFailedAlert = New TListenFailedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TListenSucceededAlert Extends TAlert

	Function _create:TListenSucceededAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TListenSucceededAlert = New TListenSucceededAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPortmapErrorAlert Extends TAlert

	Function _create:TPortmapErrorAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPortmapErrorAlert = New TPortmapErrorAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPortmapAlert Extends TAlert

	Function _create:TPortmapAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPortmapAlert = New TPortmapAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TFastresumeRejectedAlert Extends TTorrentAlert

	Function _create:TFastresumeRejectedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TFastresumeRejectedAlert = New TFastresumeRejectedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function

End Type

Type TPeerBlockedAlert Extends TAlert

	Function _create:TPeerBlockedAlert(alertPtr:Byte Ptr)
		If alertPtr Then
			Local this:TPeerBlockedAlert = New TPeerBlockedAlert
			this.alertPtr = alertPtr
			Return this
		End If
	End Function
	
End Type
