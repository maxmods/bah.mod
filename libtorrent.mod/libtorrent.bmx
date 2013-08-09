' Copyright (c) 2010-2013 Bruce A Henderson
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

Rem
bbdoc: Bit torrent for the masses.
End Rem
Module BaH.Libtorrent

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: libtorrent - 2003-2006 Arvid Norberg"
ModuleInfo "Copyright: Wrapper - 2010-2013 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (libtorrent 0.16.10)"

ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DTORRENT_USE_OPENSSL -DTORRENT_NO_DEPRECATE -DBOOST_FILESYSTEM_VERSION=3 -DBOOST_ASIO_SEPARATE_COMPILATION"
?linux
ModuleInfo "CC_OPTS: -D_FILE_OFFSET_BITS=64"
?win32
' set WINVER to 0x0500 - XP - as minimum, to enable certain functionality
ModuleInfo "CC_OPTS: -DBOOST_THREAD_USE_LIB -DWINVER=1280 -DBOOST_ASIO_ENABLE_CANCELIO"
?


' asio_ssl.cpp
'   Added SSL_CTX_clear_options checks (for boost ssl support)
'
' disk_buffer_pool.hpp
'   Added boost/noncopyable include
'

Import "core.bmx"

Rem
bbdoc: Returns the string representation of the libtorrent version number.
End Rem
Function LibtorrentVersion:String()
	Return bmx_LIBTORRENT_VERSION()
End Function

Rem
bbdoc: Retrieves the libtorrent version number as parts.
End Rem
Function LibtorrentVersionParts(major:Int Var, minor:Int Var, tiny:Int Var)
	bmx_LIBTORRENT_VERSION_parts(Varptr major, Varptr minor, Varptr tiny)
End Function

Rem
bbdoc: The current session.
End Rem
Type TSession

	Field sessionPtr:Byte Ptr
	
	Method New()
		sessionPtr = bmx_torrent_session_create()
	End Method
	
	Rem
	bbdoc: Changes the listen port and/or the listen interface.
	about: If the session is already listening on a port, this socket will be closed and a new socket will be opened with these new settings.
	The port range is the ports it will try to listen on, if the first port fails, it will continue trying the next port within the range
	and so on. The interface parameter can be left as Null, in that case the os will decide which interface to listen on, otherwise it should
	be the ip-address of the interface you want the listener socket bound to. listenOn() returns true if it managed to open the socket, and
	false if it failed. If it fails, it will also generate an appropriate alert (listen_failed_alert). If all ports in the specified range fails
	to be opened for listening, libtorrent will try to use port 0 (which tells the operating system to pick a port that's free). If that still
	fails you may see a listen_failed_alert with port 0 even if you didn't ask to listen on it.
	<p>
	The interface parameter can also be a hostname that will resolve to the device you want to listen on. If you don't specify an interface,
	libtorrent may attempt to listen on multiple interfaces (typically 0.0.0.0 and ::). This means that if your IPv6 interface doesn't work,
	you may still see a listen_failed_alert, even though the IPv4 port succeeded.
	</p>
	<p>
	If you're also starting the DHT, it is a good idea to do that after you've called listenOn(), since the default listen port for the DHT
	is the same as the tcp listen socket. If you start the DHT first, it will assume the tcp port is free and open the udp socket on that port,
	then later, when listenOn() is called, it may turn out that the tcp port is in use. That results in the DHT and the bittorrent socket listening
	on different ports. If the DHT is active when listenOn is called, the udp port will be rebound to the new port, if it was configured to use
	the same port as the tcp socket, and if the listenOn call failed to bind to the same port that the udp uses.
	</p>
	<p>
	The reason why it's a good idea to run the DHT and the bittorrent socket on the same port is because that is an assumption that may be used
	to increase performance. One way to accelerate the connecting of peers on windows may be to first ping all peers with a DHT ping packet, and
	connect to those that responds first. On windows one can only connect to a few peers at a time because of a built in limitation (in XP
	Service pack 2).
	</p>
	End Rem
	Method listenOn:Int(portFrom:Int, portTo:Int, interface:String = Null)
		If interface Then
			Return bmx_torrent_session_listenOn(sessionPtr, portFrom, portTo, interface)
		Else
			Return bmx_torrent_session_listenOn(sessionPtr, portFrom, portTo, Null)
		End If
	End Method
	
	Rem
	bbdoc: Tells you whether or not the session has successfully opened a listening port.
	about: If it hasn't, this method will return False, and then you can use #listenOn() to make another attempt.
	End Rem
	Method isListening:Int()
		Return bmx_torrent_session_is_listening(sessionPtr)
	End Method
	
	Rem
	bbdoc: Pausing the session has the same effect as pausing every torrent in it, except that torrents will not be resumed by the auto-manage mechanism.
	about: A torrent is inactive if it is paused or if the session is paused.
	End Rem
	Method pause()
		bmx_torrent_session_pause(sessionPtr)
	End Method
	
	Rem
	bbdoc: Resuming the sessions will restore the torrents to their previous paused state.
	about: The session pause state is separate from the torrent pause state. A torrent is inactive if it is paused or if the session is paused.
	End Rem
	Method resume()
		bmx_torrent_session_resume(sessionPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the session is paused.
	End Rem
	Method isPaused:Int()
		Return bmx_torrent_session_is_paused(sessionPtr)
	End Method
	
	Rem
	bbdoc: Returns the session wide-statistics and status.
	End Rem
	Method status:TSessionStatus()
		Return TSessionStatus._create(bmx_torrent_session_status(sessionPtr))
	End Method
	
'	Rem
'	bbdoc: 
'	End Rem
'	Method setMaxUploads(limit:Int)
'		bmx_torrent_session_set_max_uploads(sessionPtr, limit)
'	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method addTorrent:TTorrentHandle(params:TAddTorrentParams)
		Return TTorrentHandle._create(bmx_torrent_session_add_torrent(sessionPtr, params.paramsPtr))
	End Method

	Rem
	bbdoc: 
	End Rem	
	Method setSettings(settings:TSessionSettings)
		bmx_torrent_session_set_settings(sessionPtr, settings.settingsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem	
	Method popAlert:TAlert()
		Return TAlert(bmx_torrent_session_pop_alert(sessionPtr))
	End Method
	
	Rem
	bbdoc: Starts the UPnP service.
	about: When started, the listen port and the DHT port are attempted to be forwarded on local UPnP router devices.
	<p>
	The upnp object returned by the method can be used to add and remove arbitrary port mappings. Mapping status is returned
	through the TPortmapAlert and the TPortmapErrorAlert. The object will be valid until stopUpnp() is called.
	</p>
	<p>
	It is off by default.
	</p>
	End Rem	
	Method startUpnp:TUpnp()
		Return TUpnp._create(bmx_torrent_session_start_upnp(sessionPtr))
	End Method
	
	Rem
	bbdoc: Stops the UPnP service.
	End Rem	
	Method stopUpnp()
		bmx_torrent_session_stop_upnp(sessionPtr)
	End Method
	
	Rem
	bbdoc: Starts the NAT-PMP service.
	about: When started, the listen port and the DHT port are attempted to be forwarded on the router through NAT-PMP.
	<p>
	The natpmp object returned by the method can be used to add and remove arbitrary port mappings. Mapping status is returned
	through the TPortmapAlert and the TPortmapErrorAlert. The object will be valid until stopNatpmp() is called.
	</p>
	<p>
	It is off by default.
	</p>
	End Rem	
	Method startNatpmp:TNatpmp()
		Return TNatpmp._create(bmx_torrent_session_start_natpmp(sessionPtr))
	End Method
	
	Rem
	bbdoc: Stops the NAT-PMP service.
	End Rem	
	Method stopNatpmp()
		bmx_torrent_session_stop_natpmp(sessionPtr)
	End Method
	
	Rem
	bbdoc: Starts Local Service Discovery.
	about: This service will broadcast the infohashes of all the non-private torrents on the local network to look for peers on the same
	swarm within multicast reach.
	<p>
	It is turned off by default.
	</p>
	End Rem	
	Method startLsd()
		bmx_torrent_session_start_lsd(sessionPtr)
	End Method
	
	Rem
	bbdoc: Stops Local Service Discovery.
	about: This service will broadcast the infohashes of all the non-private torrents on the local network to look for peers on the same
	swarm within multicast reach.
	<p>
	It is turned off by default.
	</p>
	End Rem	
	Method stopLsd()
		bmx_torrent_session_stop_lsd(sessionPtr)
	End Method

'	Rem
'	bbdoc: Sets the maximum number of half-open connections libtorrent will have when connecting To peers.
'	about: A half-open connection is one where connect() has been called, but the connection still hasn't been established
'	(nor failed). Windows XP Service Pack 2 sets a Default, system wide, limit of the number of half-open connections
'	To 10. So, this limit can be used To work nicer together with other network applications on that system.
'	The Default is To have no limit, And passing -1 as the limit, means To have no limit. When limiting the number of simultaneous
'	connection attempts, peers will be put in a queue waiting For their turn To get connected.
'	End Rem	
'	Method setMaxHalfOpenConnections(limit:Int)
'		bmx_torrent_session_set_max_half_open_connections(sessionPtr, limit)
'	End Method
	
'	Rem
'	bbdoc: Returns the maximum number of half-open connections libtorrent will have when connecting To peers.
'	about: A half-open connection is one where connect() has been called, but the connection still hasn't been established
'	(nor failed). Windows XP Service Pack 2 sets a Default, system wide, limit of the number of half-open connections
'	To 10. So, this limit can be used To work nicer together with other network applications on that system.
'	The Default is To have no limit, And passing -1 as the limit, means To have no limit. When limiting the number of simultaneous
'	connection attempts, peers will be put in a queue waiting For their turn To get connected.
'	End Rem	
'	Method maxHalfOpenConnections:Int()
'		Return bmx_torrent_session_max_half_open_connections(sessionPtr)
'	End Method
	
	Rem
	bbdoc: Returns status of the disk cache for this session.
	End Rem
	Method getCacheStatus:TCacheStatus()
		Return TCacheStatus._create(bmx_torrent_session_get_cache_status(sessionPtr))
	End Method

	Method Delete()
		If sessionPtr Then
			bmx_torrent_session_free(sessionPtr)
			sessionPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Session wide-statistics and status.
End Rem
Type TSessionStatus

	Field statusPtr:Byte Ptr
	
	Function _create:TSessionStatus(statusPtr:Byte Ptr)
		If statusPtr Then
			Local this:TSessionStatus = New TSessionStatus
			this.statusPtr = statusPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: False as long as no incoming connections have been established on the listening socket.
	about: Every time you change the listen port, this will be reset to False.
	End Rem
	Method hasIncomingConnections:Int()
		Return bmx_torrent_sessionstatus_has_incoming_connections(statusPtr)
	End Method
	
	Rem
	bbdoc: The total upload rate accumulated from all torrents.
	End Rem
	Method uploadRate:Float()
		Return bmx_torrent_sessionstatus_upload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total download rate accumulated from all torrents.
	End Rem
	Method downloadRate:Float()
		Return bmx_torrent_sessionstatus_download_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total payload upload rate accumulated from all torrents.
	End Rem
	Method payloadUploadRate:Float()
		Return bmx_torrent_sessionstatus_payload_upload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total payload download rate accumulated from all torrents.
	End Rem
	Method payloadDownloadRate:Float()
		Return bmx_torrent_sessionstatus_payload_download_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total number of bytes downloaded from all torrents.
	End Rem
	Method totalDownload:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_download(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The total number of bytes uploaded to all torrents.
	End Rem
	Method totalUpload:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_upload(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of bytes that have been received more than once.
	about: This can happen if a request from a peer times out and is requested from a different peer, and then received again
	from the first one. To make this lower, increase the requestTimeout and the pieceTimeout in the session settings.
	End Rem
	Method totalRedundantBytes:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_redundant_bytes(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of bytes that was downloaded which later failed the hash-check.
	End Rem
	Method totalFailedBytes:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_failed_bytes(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalPayloadDownload:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_payload_download(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalPayloadUpload:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_payload_upload(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The total number of peer connections this session has.
	about: This includes incoming connections that still hasn't sent their handshake or outgoing connections that still
	hasn't completed the TCP connection. This number may be slightly higher than the sum of all peers of all torrents because
	the incoming connections may not be assigned a torrent yet.
	End Rem
	Method numPeers:Int()
		Return bmx_torrent_sessionstatus_num_peers(statusPtr)
	End Method
	
	Rem
	bbdoc: The current number of unchoked peers.
	End Rem
	Method numUnchoked:Int()
		Return bmx_torrent_sessionstatus_num_unchoked(statusPtr)
	End Method
	
	Rem
	bbdoc: The current allowed number of unchoked peers.
	End Rem
	Method allowedUploadSlots:Int()
		Return bmx_torrent_sessionstatus_allowed_upload_slots(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of nodes in the routing table.
	about: This number only includes active nodes, not cache nodes.
	End Rem
	Method dhtNodes:Int()
		Return bmx_torrent_sessionstatus_dht_nodes(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of nodes in the node cache.
	about: These nodes are used to replace the regular nodes in the routing table in case any of them becomes unresponsive.
	End Rem
	Method dhtNodeCache:Int()
		Return bmx_torrent_sessionstatus_dht_node_cache(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of torrents tracked by the DHT at the moment.
	End Rem
	Method dhtTorrents:Int()
		Return bmx_torrent_sessionstatus_dht_torrents(statusPtr)
	End Method
	
	Rem
	bbdoc: An estimation of the total number of nodes in the DHT network.
	End Rem
	Method dhtGlobalNodes:Int()
		Return bmx_torrent_sessionstatus_dht_global_nodes(statusPtr)
	End Method

	Method Delete()
		If statusPtr Then
			bmx_torrent_sessionstatus_free(statusPtr)
			statusPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TAddTorrentParams

	Const SEED_MODE:Int = $001
	Const OVERRIDE_RESUME_DATA:Int = $002
	Const UPLOAD_MODE:Int = $004
	Const SHARE_MODE:Int = $008
	Const APPLY_IP_FILTER:Int = $010
	Const PAUSED:Int = $020
	Const AUTO_MANAGED:Int = $040
	Const DUPLICATE_IS_ERROR:Int = $080
	Const MERGE_RESUME_TRACKERS:Int = $100
	Const UPDATE_SUBSCRIBE:Int = $200
	Const DEFAULT_FLAGS:Int = UPDATE_SUBSCRIBE | AUTO_MANAGED | PAUSED | APPLY_IP_FILTER
			
	Field paramsPtr:Byte Ptr
	
	Field namePtr:Byte Ptr
	Field urlPtr:Byte Ptr
	
	Method New()
		paramsPtr = bmx_torrent_addtorrentparams_create()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setSavePath(path:String)
		bmx_torrent_addtorrentparams_set_save_path(paramsPtr, path)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTorrentInfo(info:TTorrentInfo)
		bmx_torrent_addtorrentparams_set_torrent_info(paramsPtr, info.infoPtr)
		info.owner = False
	End Method
	
'	Rem
'	bbdoc: 
'	End Rem
'	Method setTrackerUrl(url:String)
'		If urlPtr Then
'			MemFree(urlPtr)
'		End If
'
'		urlPtr = url.ToUTF8String()
'		bmx_torrent_addtorrentparams_set_tracker_url(paramsPtr, urlPtr)
'	End Method
	
'	Rem
'	bbdoc: 
'	End Rem
'	Method trackerUrl:String()
'		Return bmx_torrent_addtorrentparams_tracker_url(paramsPtr)
'	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setName(name:String)
		If namePtr Then
			MemFree(namePtr)
		End If

		namePtr = name.ToUTF8String()
		bmx_torrent_addtorrentparams_set_name(paramsPtr, namePtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method name:String()
		Return bmx_torrent_addtorrentparams_name(paramsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method savePath:String()
		Return bmx_torrent_addtorrentparams_save_path(paramsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setStorageMode(_mode:Int)
		bmx_torrent_addtorrentparams_set_storage_mode(paramsPtr, _mode)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method storageMode:Int()
		Return bmx_torrent_addtorrentparams_storage_mode(paramsPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setFlags(value:Int)
		bmx_torrent_addtorrentparams_set_flags(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method flags:Int()
		Return bmx_torrent_addtorrentparams_flags(paramsPtr)
	End Method
	
'	Rem
'	bbdoc: 
'	End Rem
'	Method setAutoManaged(value:Int)
'		bmx_torrent_addtorrentparams_set_auto_managed(paramsPtr, value)
'	End Method
'	
'	Rem
'	bbdoc: 
'	End Rem
'	Method setDuplicateIsError(value:Int)
'		bmx_torrent_addtorrentparams_set_duplicate_is_error(paramsPtr, value)
'	End Method
'	
'	Rem
'	bbdoc: 
'	End Rem
'	Method duplicateIsError:Int()
'		Return bmx_torrent_addtorrentparams_duplicate_is_error(paramsPtr)
'	End Method
	
	Method Delete()
		If urlPtr Then
			MemFree(urlPtr)
		End If

		If namePtr Then
			MemFree(namePtr)
		End If
	
		If paramsPtr Then
			bmx_torrent_addtorrentparams_free(paramsPtr)
			paramsPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Provides information about the torrent, and aborts the torrent.
about: You will usually have to store your torrent handles somewhere, since it's the object through which you retrieve information it.
<p>
Warning : All operations on a handle may throw TInvalidHandle exception, in case the handle is no longer refering to a torrent.
There is one exception, isValid() will never throw. Since the torrents are processed by a background thread, there is
no guarantee that a handle will remain valid between two calls.
</p>
End Rem
Type TTorrentHandle

	Field torrentPtr:Byte Ptr
	
	Function _create:TTorrentHandle(torrentPtr:Byte Ptr)
		If torrentPtr Then
			Local this:TTorrentHandle = New TTorrentHandle
			this.torrentPtr = torrentPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns True if this handle refers to a valid torrent and false if it hasn't been initialized or if the torrent it refers to has been aborted.
	about: Note that a handle may become invalid after it has been added to the session. Usually this is because the storage for
	the torrent is somehow invalid or if the filenames are not allowed (and hence cannot be opened/created) on your filesystem. If
	such an error occurs, a TFileErrorAlert is generated and all handles that refers to that torrent will become invalid.
	End Rem
	Method isValid:Int()
		Return bmx_torrent_torrenthandle_is_valid(torrentPtr)
	End Method

	Rem
	bbdoc: Returns a TTorrentStatus object with information about the status of this torrent.
	about: If the handle is invalid, it will throw TInvalidHandle exception. 
	End Rem
	Method status:TTorrentStatus()
		Return TTorrentStatus._create(bmx_torrent_torrenthandle_status(torrentPtr))
	End Method
	
	Rem
	bbdoc: Returns the name of the torrent.
	about: i.e. the name from the metadata associated with it. In case the torrent was started without metadata, and
	hasn't completely received it yet, it returns the name given to it when added to the session. See TSession::addTorrent.
	End Rem
	Method name:String()
		Return bmx_torrent_torrenthandle_name(torrentPtr)
	End Method
	
	Rem
	bbdoc: Returns reference to the TTorrentInfo object associated with this torrent.
	about: This reference is valid as long as the TTorrentHandle is valid, no longer. If the TTorrentHandle is invalid or if
	it doesn't have any metadata, a TInvalidHandle exception will be thrown. The torrent may be in a state without metadata
	only if it was started without a .torrent file, i.e. by using the libtorrent extension of just supplying a tracker and info-hash.
	End Rem
	Method getTorrentInfo:TTorrentInfo()
		Return TTorrentInfo._create(bmx_torrent_torrenthandle_get_torrent_info(torrentPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method saveResumeData()
		bmx_torrent_torrenthandle_save_resume_data(torrentPtr)
	End Method
	
	Rem
	bbdoc: Forces this torrent to do another tracker request, to receive new peers.
	End Rem
	Method forceReannounce()
		bmx_torrent_torrenthandle_force_reannounce(torrentPtr)
	End Method
	
	Rem
	bbdoc: Sends a scrape request to the tracker.
	about: A scrape request queries the tracker for statistics such as total number of incomplete peers, complete peers, number of downloads etc.
	<p>
	This request will specifically update the numComplete and numIncomplete fields in the TTorrentStatus object once it completes. When
	it completes, it will generate a TScrapeReplyAlert. If it fails, it will generate a TScrapeFailedAlert.
	</p>
	End Rem
	Method scrapeTracker()
		bmx_torrent_torrenthandle_scrape_tracker(torrentPtr)
	End Method
	
	Rem
	bbdoc: Sets a username and password that will be sent along in the HTTP-request of the tracker announce.
	about: Set this if the tracker requires authorization.
	End Rem
	Method setTrackerLogin(username:String, password:String)
		bmx_torrent_torrenthandle_set_tracker_login(torrentPtr, username, password)
	End Method
	
	Rem
	bbdoc: Adds another url to the torrent's list of url seeds.
	about: If the given url already exists in that list, the call has no effect. The torrent will connect to the server
	and try to download pieces from it, unless it's paused, queued, checking or seeding.
	End Rem
	Method addUrlSeed(url:String)
		bmx_torrent_torrenthandle_add_url_seed(torrentPtr, url)
	End Method
	
	Rem
	bbdoc: Removes the given url if it exists already.
	End Rem
	Method removeUrlSeed(url:String)
		bmx_torrent_torrenthandle_remove_url_seed(torrentPtr, url)
	End Method
	
	Rem
	bbdoc: Returns a set of the url seeds currently in this torrent.
	about: Note that urls that fail may be removed automatically from the list.
	End Rem
	Method urlSeeds:String[]()
		Return bmx_torrent_torrenthandle_url_seeds(torrentPtr)
	End Method
	
'	Rem
'	bbdoc: Sets the desired download / upload ratio.
'	abou: If set To 0, it is considered being infinite. i.e. the client will always upload as much as it can, no
'	matter how much it gets back in Return. With this setting it will work much like the standard clients.
'	<p>
'	Besides 0, the ratio can be set To any number greater than Or equal To 1. It means how much To attempt To upload in Return
'	For each download. e.g. If set To 2, the client will Try To upload 2 bytes For every Byte received. The Default setting
'	For this is 0, which will make it work as a standard client.
'	</p>
'	End Rem
'	Method setRatio(ratio:Float)
'		bmx_torrent_torrenthandle_set_ratio(torrentPtr, ratio)
'	End Method
	
	Rem
	bbdoc: Sets the maximum number of peers that's unchoked at the same time on this torrent.
	about: If you set this to -1, there will be no limit.
	End Rem
	Method setMaxUploads(maxUploads:Int)
		bmx_torrent_torrenthandle_set_max_uploads(torrentPtr, maxUploads)
	End Method
	
	Rem
	bbdoc: Sets the maximum number of connection this torrent will open.
	about: If all connections are used up, incoming connections may be refused or poor connections may be closed.
	This must be at least 2. The default is unlimited number of connections. If -1 is given to the function, it means unlimited.
	End Rem
	Method setMaxConnections(maxConnections:Int)
		bmx_torrent_torrenthandle_set_max_connections(torrentPtr, maxConnections)
	End Method
	
	Rem
	bbdoc: Limits the upload bandwidth used by this particular torrent to the limit you set.
	about: It is given as the number of bytes per second the torrent is allowed to upload.
	End Rem
	Method setUploadLimit(limit:Int)
		bmx_torrent_torrenthandle_set_upload_limit(torrentPtr, limit)
	End Method
	
	Rem
	bbdoc: Returns the current upload limit setting.
	End Rem
	Method uploadLimit:Int()
		Return bmx_torrent_torrenthandle_upload_limit(torrentPtr)
	End Method
	
	Rem
	bbdoc: Limits the download bandwidth used by this particular torrent to the limit you set.
	about: It is given as the number of bytes per second the torrent is allowed to download. 
	End Rem
	Method setDownloadLimit(limit:Int)
		bmx_torrent_torrenthandle_set_download_limit(torrentPtr, limit)
	End Method
	
	Rem
	bbdoc: Returns the current download limit setting.
	End Rem
	Method downloadLimit:Int()
		Return bmx_torrent_torrenthandle_download_limit(torrentPtr)
	End Method
	
	Rem
	bbdoc: Enables or disables sequential download.
	about: When enabled, the piece picker will pick pieces in sequence instead of rarest first.
	<p>
	Enabling sequential download will affect the piece distribution negatively in the swarm. It should be used sparingly.
	</p>
	End Rem
	Method setSequentialDownload(value:Int)
		bmx_torrent_torrenthandle_set_sequential_download(torrentPtr, value)
	End Method
	
	Rem
	bbdoc: Returns the torrent's position in the download queue.
	about: The torrents with the smallest numbers are the ones that are being downloaded. The smaller number, the closer
	the torrent is to the front of the line to be started.
	End Rem
	Method queuePosition:Int()
		Return bmx_torrent_torrenthandle_queue_position(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method queuePositionUp()
		bmx_torrent_torrenthandle_queue_position_up(torrentPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method queuePositionDown()
		bmx_torrent_torrenthandle_queue_position_down(torrentPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method queuePositionTop()
		bmx_torrent_torrenthandle_queue_position_top(torrentPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method queuePositionBottom()
		bmx_torrent_torrenthandle_queue_position_bottom(torrentPtr)
	End Method
	
	Rem
	bbdoc: Sets the network interface this torrent will use when it opens outgoing connections.
	about: By default, it uses the same interface as the session uses to listen on. The parameter must be a string
	containing an ip-address (either an IPv4 or IPv6 address). If the string does not conform to this format and exception is thrown.
	End Rem
	Method useInterface(netInterface:String)
		bmx_torrent_torrenthandle_use_interface(torrentPtr, netInterface)
	End Method
	
	Rem
	bbdoc: Will disconnect all peers.
	about: When a torrent is paused, it will however remember all share ratios to all peers and remember all potential
	(not connected) peers. You can use isPaused() to determine if a torrent is currently paused. Torrents may be paused
	automatically if there is a file error (e.g. disk full) or something similar. See TFileErrorAlert.
	<p>
	Torrents that are auto-managed may be automatically resumed again. It does not make sense to pause an auto-managed torrent without
	making it not automanaged first. Torrents are auto-managed by default when added to the session.
	</p>
	End Rem
	Method pause()
		bmx_torrent_torrenthandle_pause(torrentPtr)
	End Method
	
	Rem
	bbdoc: Reconnects all peers.
	about: When a torrent is paused, it will however remember all share ratios to all peers and remember all potential
	(not connected) peers. You can use isPaused() to determine if a torrent is currently paused. Torrents may be paused
	automatically if there is a file error (e.g. disk full) or something similar. See TFileErrorAlert.
	<p>
	Torrents that are auto-managed may be automatically resumed again. It does not make sense to pause an auto-managed torrent without
	making it not automanaged first. Torrents are auto-managed by default when added to the session.
	</p>
	End Rem
	Method resume()
		bmx_torrent_torrenthandle_resume(torrentPtr)
	End Method
	
'	Rem
'	bbdoc: Only returns True If the torrent itself is paused.
'	about: If the torrent is Not running because the session is paused, this still returns False. To know If a torrent is active Or Not,
'	you need To inspect both TTorrentHandle::isPaused() And TSession::isPaused().
'	End Rem
'	Method isPaused:Int()
'		Return bmx_torrent_torrenthandle_is_paused(torrentPtr)
'	End Method
	
'	Rem
'	bbdoc: Returns True If the torrent is in seed Mode (i.e. If it has finished downloading).
'	End Rem
'	Method isSeed:Int()
'		Return bmx_torrent_torrenthandle_is_seed(torrentPtr)
'	End Method
	
	Rem
	bbdoc: Puts the torrent back in a state where it assumes to have no resume data.
	about: All peers will be disconnected and the torrent will stop announcing to the tracker. The torrent will be added to the
	checking queue, and will be checked (all the files will be read and compared to the piece hashes). Once the check
	is complete, the torrent will start connecting to peers again, as normal.
	End Rem
	Method forceRecheck()
		bmx_torrent_torrenthandle_force_recheck(torrentPtr)
	End Method
	
	Rem
	bbdoc: If the torrent is in an error state (i.e. TTorrentStatus::error is non-empty), this will clear the error and start the torrent again.
	End Rem
	Method clearError()
		bmx_torrent_torrenthandle_clear_error(torrentPtr)
	End Method

'	Rem
'	bbdoc: Returns True If this torrent is currently auto managed.
'	End Rem
'	Method isAutoManaged:Int()
'		Return bmx_torrent_torrenthandle_is_auto_managed(torrentPtr)
'	End Method
	
	Rem
	bbdoc: Changes whether the torrent is auto managed or not.
	End Rem
	Method autoManaged(value:Int)
		bmx_torrent_torrenthandle_auto_managed(torrentPtr, value)
	End Method
	
'	Rem
'	bbdoc: Returns True If this torrent has metadata (either it was started from a .torrent file Or the metadata has been downloaded).
'	about: The only scenario where this can Return False is when the torrent was started torrent-less (i.e. with just an info-hash
'	And tracker ip). Note that If the torrent doesn't have metadata, the member getTorrentInfo() will throw.
'	End Rem
'	Method hasMetadata:Int()
'		Return bmx_torrent_torrenthandle_has_metadata(torrentPtr)
'	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getPeerInfo:TPeerInfo[]()
		Return bmx_torrent_torrenthandle_get_peer_info(torrentPtr)
	End Method
	
	Rem
	bbdoc: Returns the availability for each piece in this torrent.
	about: libtorrent does not keep track of availability for seeds, so if the torrent is seeding the availability for all pieces is reported as 0.
	<p>
	The piece availability is the number of peers that we are connected that has advertized having a particular piece. This is the
	information that libtorrent uses in order to prefer picking rare pieces.
	</p>
	End Rem
	Method pieceAvailablity:Int[]()
		Return bmx_torrent_torrenthandle_piece_availability(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setPiecePriority(index:Int, priority:Int)
		bmx_torrent_torrenthandle_set_piece_priority(torrentPtr, index, priority)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method piecePriority:Int(index:Int)
		Return bmx_torrent_torrenthandle_piece_priority(torrentPtr, index)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method prioritizePieces(pieces:Int[])
		bmx_torrent_torrenthandle_prioritize_pieces(torrentPtr, pieces)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method piecePriorities:Int[]()
		Return bmx_torrent_torrenthandle_piece_priorities(torrentPtr)
	End Method
	
	Rem
	bbdoc: Returns a list about pieces that are partially downloaded or not downloaded at all but partially requested.
	End Rem
	Method getDownloadQueue:TPartialPieceInfo[]()
		Return bmx_torrent_torrenthandle_get_download_queue(torrentPtr)
	End Method

End Type

Extern
	Function bmx_torrent_torrenthandle_get_peer_info:TPeerInfo[](handle:Byte Ptr)
	Function bmx_torrent_partialpieceinfo_blocks:TBlockInfo[](handle:Byte Ptr)
	Function bmx_torrent_torrenthandle_get_download_queue:TPartialPieceInfo[](handle:Byte Ptr)
End Extern

Rem
bbdoc: 
End Rem
Type TTorrentInfo

	Field infoPtr:Byte Ptr
	Field owner:Int = True
	
	Function _create:TTorrentInfo(infoPtr:Byte Ptr)
		If infoPtr Then
			Local this:TTorrentInfo = New TTorrentInfo
			this.infoPtr = infoPtr
			this.owner = False
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function Load:TTorrentInfo(path:String)
		Local this:TTorrentInfo = New TTorrentInfo
		this.infoPtr = bmx_torrent_torrentinfo_load(path)
		If this.infoPtr Then
			Return this
		End If
	End Function
	
	
	Method Delete()
		If infoPtr And owner Then
			bmx_torrent_torrentinfo_free(infoPtr)
		End If
		infoPtr = Null
	End Method

End Type

Rem
bbdoc: Torrent status information.
End Rem
Type TTorrentStatus

	Field statusPtr:Byte Ptr
	
	Rem
	bbdoc: The torrent is in the queue for being checked.
	about: But there currently is another torrent that are being checked. This torrent will wait for its turn.
	End Rem
	Const STATE_QUEUED_FOR_CHECKING:Int = 0
	Rem
	bbdoc: The torrent has not started its download yet, and is currently checking existing files.
	End Rem
	Const STATE_CHECKING_FILES:Int = 1
	Rem
	bbdoc: The torrent is trying to download metadata from peers.
	about: This assumes the metadata_transfer extension is in use.
	End Rem
	Const STATE_DOWNLOADING_METADATA:Int = 2
	Rem
	bbdoc: The torrent is being downloaded.
	about: This is the state most torrents will be in most of the time. The progress meter will tell how much of the files that has been downloaded.
	End Rem
	Const STATE_DOWNLOADING:Int = 3
	Rem
	bbdoc: In this state the torrent has finished downloading but still doesn't have the entire torrent.
	about: i.e. some pieces are filtered and won't get downloaded.
	End Rem
	Const STATE_FINISHED:Int = 4
	Rem
	bbdoc: In this state the torrent has finished downloading and is a pure seeder.
	End Rem
	Const STATE_SEEDING:Int = 5
	Rem
	bbdoc: If the torrent was started in full allocation mode, this indicates that the (disk) storage for the torrent is allocated.
	End Rem
	Const STATE_ALLOCATING:Int = 6

	Function _create:TTorrentStatus(statusPtr:Byte Ptr)
		If statusPtr Then
			Local this:TTorrentStatus = New TTorrentStatus
			this.statusPtr = statusPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method state:Int()
		Return bmx_torrent_torrentstatus_state(statusPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the torrent is paused and False otherwise.
	End Rem
	Method paused:Int()
		Return bmx_torrent_torrentstatus_paused(statusPtr)
	End Method
	
	Rem
	bbdoc: A value in the range [0, 1], that represents the progress of the torrent's current task.
	about: It may be checking files or downloading.
	End Rem
	Method progress:Float()
		Return bmx_torrent_torrentstatus_progress(statusPtr)
	End Method
	
	Rem
	bbdoc: May be set to an error message describing why the torrent was paused, in case it was paused by an error.
	about: If the torrent is not paused or if it's paused but not because of an error, this string is empty.
	End Rem
	Method error:String()
		Return bmx_torrent_torrentstatus_error(statusPtr)
	End Method
	
	Rem
	bbdoc: The URL of the last working tracker.
	about: If no tracker request has been successful yet, it's set to an empty string.
	End Rem
	Method currentTracker:String()
		Return bmx_torrent_torrentstatus_current_tracker(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of bytes uploaded to all peers, accumulated, this session only.
	about: The session is considered to restart when a torrent is paused and restarted again. When a torrent is paused,
	these counters are reset to 0. If you want complete, persistent, stats, see #allTimeUpload.
	End Rem
	Method totalUpload:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_upload(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of bytes downloaded to all peers, accumulated, this session only.
	about: The session is considered to restart when a torrent is paused and restarted again. When a torrent is paused,
	these counters are reset to 0. If you want complete, persistent, stats, see #allTimeDownload.
	End Rem
	Method totalDownload:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_download(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: Counts the amount of bytes received this session, but only the actual payload data (i.e the interesting data).
	about: This counter ignore any protocol overhead.
	End Rem
	Method totalPayloadDownload:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_payload_download(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: Counts the amount of bytes sent this session, but only the actual payload data (i.e the interesting data).
	about: This counter ignore any protocol overhead.
	End Rem
	Method totalPayloadUpload:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_payload_upload(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of bytes that has been downloaded and that has failed the piece hash test.
	about: In other words, this is just how much crap that has been downloaded.
	End Rem
	Method totalFailedBytes:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_failed_bytes(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of bytes that has been downloaded even though that data already was downloaded.
	about: The reason for this is that in some situations the same data can be downloaded by mistake. When libtorrent
	sends requests to a peer, and the peer doesn't send a response within a certain timeout, libtorrent
	will re-request that block. Another situation when libtorrent may re-request blocks is when the requests it sends out are
	not replied in FIFO-order (it will re-request blocks that are skipped by an out of order block). This is supposed to be as low as possible.
	End Rem
	Method totalRedundantBytes:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_redundant_bytes(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The total download rate for all peers for this torrent.
	about: This will usually have better precision than summing the rates from all peers. The rate is given as the number of bytes per second. 
	End Rem
	Method downloadRate:Float()
		Return bmx_torrent_torrentstatus_download_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total upload rate for all peers for this torrent.
	about: This will usually have better precision than summing the rates from all peers. The rate is given as the number of bytes per second. 
	End Rem
	Method uploadRate:Float()
		Return bmx_torrent_torrentstatus_upload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total download transfer rate of payload only, not counting protocol chatter.
	about: This might be slightly smaller than the other rates, but if projected over a long time (e.g. when calculating ETA:s) the
	difference may be noticeable.
	End Rem
	Method downloadPayloadRate:Float()
		Return bmx_torrent_torrentstatus_download_payload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The total upload transfer rate of payload only, not counting protocol chatter.
	about: This might be slightly smaller than the other rates, but if projected over a long time (e.g. when calculating ETA:s) the
	difference may be noticeable.
	End Rem
	Method uploadPayloadRate:Float()
		Return bmx_torrent_torrentstatus_upload_payload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of peers this torrent currently is connected to.
	about: Peer connections that are in the half-open state (is attempting to connect) or are queued for later connection
	attempt do not count. Although they are visible in the peer list when you call #getPeerInfo().
	End Rem
	Method numPeers:Int()
		Return bmx_torrent_torrentstatus_num_peers(statusPtr)
	End Method
	
	Rem
	bbdoc: Set to -1 if the tracker did not send any scrape data in its announce reply.
	about: This data is optional and may not be available from all trackers. If this is not -1, it is the total number of
	peers that are seeding (complete).
	End Rem
	Method numComplete:Int()
		Return bmx_torrent_torrentstatus_num_complete(statusPtr)
	End Method
	
	Rem
	bbdoc: Set to -1 if the tracker did not send any scrape data in its announce reply.
	about: This data is optional and may not be available from all trackers. If this is not -1, it is the total number of
	peers that are still downloading (incomplete) this torrent.
	End Rem
	Method numIncomplete:Int()
		Return bmx_torrent_torrentstatus_num_incomplete(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of seeds in our peer list.
	about: We are not necessarily connected to all the peers in our peer list.
	End Rem
	Method listSeeds:Int()
		Return bmx_torrent_torrentstatus_list_seeds(statusPtr)
	End Method
	
	Rem
	bbdoc: The total number of peers (including seeds).
	about: We are not necessarily connected to all the peers in our peer list. This is the number of peers we know of in
	total, including banned peers and peers that we have failed to connect to.
	End Rem
	Method listPeers:Int()
		Return bmx_torrent_torrentstatus_list_peers(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of peers in this torrent's peer list that is a candidate to be connected to.
	about: It has fewer connect attempts than the max fail count, it is not a seed if we are a seed, it is not banned etc.
	If this is 0, it means we don't know of any more peers that we can try.
	End Rem
	Method connectCandidates:Int()
		Return bmx_torrent_torrentstatus_connect_candidates(statusPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method totalDone:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_done(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalWantedDone:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_wanted_done(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalWanted:Long()
		Local v:Long
		bmx_torrent_torrentstatus_total_wanted(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numSeeds:Int()
		Return bmx_torrent_torrentstatus_num_seeds(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method distributedCopies:Float()
		Return bmx_torrent_torrentstatus_distributed_copies(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method blockSize:Int()
		Return bmx_torrent_torrentstatus_block_size(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numUploads:Int()
		Return bmx_torrent_torrentstatus_num_uploads(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numConnections:Int()
		Return bmx_torrent_torrentstatus_num_connections(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method uploadsLimit:Int()
		Return bmx_torrent_torrentstatus_uploads_limit(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method connectionsLimit:Int()
		Return bmx_torrent_torrentstatus_connections_limit(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method upBandwidthQueue:Int()
		Return bmx_torrent_torrentstatus_up_bandwidth_queue(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downBandwidthQueue:Int()
		Return bmx_torrent_torrentstatus_down_bandwidth_queue(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method allTimeUpload:Long()
		Local v:Long
		bmx_torrent_torrentstatus_all_time_upload(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method allTimeDownload:Long()
		Local v:Long
		bmx_torrent_torrentstatus_all_time_download(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method activeTime:Int()
		Return bmx_torrent_torrentstatus_active_time(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method seedingTime:Int()
		Return bmx_torrent_torrentstatus_seeding_time(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method seedRank:Int()
		Return bmx_torrent_torrentstatus_seed_rank(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method lastScrape:Int()
		Return bmx_torrent_torrentstatus_last_scrape(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method hasIncoming:Int()
		Return bmx_torrent_torrentstatus_has_incoming(statusPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method pieces:TBitfield()
		Return TBitfield._create(bmx_torrent_torrentstatus_pieces(statusPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numPieces:Int()
		Return bmx_torrent_torrentstatus_num_pieces(statusPtr)
	End Method
	
	Rem
	bbdoc: Returns True if this torrent is downloading in sequence, And False otherwise.
	End Rem
	Method sequentialDownload:Int()
		Return bmx_torrent_torrentstatus_sequential_download(statusPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method autoManaged:Int()
		Return bmx_torrent_torrentstatus_auto_managed(statusPtr)
	End Method

	Method Delete()
		If statusPtr Then
			bmx_torrent_torrentstatus_free(statusPtr)
			statusPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Peer information.
End Rem
Type TPeerInfo

	Rem
	bbdoc: We are interested in pieces from this peer.
	End Rem
	Const interesting:Int = $1
	Rem
	bbdoc: We have choked this peer.
	End Rem
	Const choked:Int = $2
	Rem
	bbdoc: The peer is interested in us.
	End Rem
	Const remoteInterested:Int = $4
	Rem
	bbdoc: Tthe peer has choked us.
	End Rem
	Const remoteChoked:Int = $8
	Rem
	bbdoc: Means that this peer supports the extension protocol.
	End Rem
	Const supportsExtensions:Int = $10
	Rem
	bbdoc: The connection was initiated by us, the peer has a listen port open, and that port is the same as in the address of this peer.
	about: If this flag is not set, this peer connection was opened by this peer connecting to us.
	End Rem
	Const localConnection:Int = $20
	Rem
	bbdoc: The connection is opened, and waiting for the handshake.
	about: Until the handshake is done, the peer cannot be identified.
	End Rem
	Const handshake:Int = $40
	Rem
	bbdoc: The connection is in a half-open state (i.e. it is being connected).
	End Rem
	Const connecting:Int = $80
	Rem
	bbdoc: The connection is currently queued for a connection attempt.
	about: This may happen if there is a limit set on the number of half-open TCP connections.
	End Rem
	Const queued:Int = $100
	Rem
	bbdoc: The peer has participated in a piece that failed the hash check, and is now "on parole", which means we're only requesting whole pieces from this peer until it either fails that piece or proves that it doesn't send bad data.
	End Rem
	Const onParole:Int = $200
	Rem
	bbdoc: This peer is a seed (it has all the pieces).
	End Rem
	Const seed:Int = $400
	Rem
	bbdoc: This peer is subject to an optimistic unchoke.
	about: It has been unchoked for a while to see if it might unchoke us in return an earn an upload/unchoke slot. If it doesn't within
	some period of time, it will be choked and another peer will be optimistically unchoked.
	End Rem
	Const optimisticUnchoke:Int = $800
	Rem
	bbdoc: This peer has recently failed to send a block within the request timeout from when the request was sent.
	about: We're currently picking one block at a time from this peer.
	End Rem
	Const snubbed:Int = $1000
	Rem
	bbdoc: This peer has either explicitly (with an extension) or implicitly (by becoming a seed) told us that it will not downloading anything more, regardless of which pieces we have.
	End Rem
	Const uploadOnly:Int = $2000
	Rem
	bbdoc: This peer uses rc4 encryption.
	End Rem
	Const rc4Encrypted:Int = $100000
	Rem
	bbdoc: This peer uses plaintext encryption.
	End Rem
	Const plaintextEncrypted:Int = $200000

	Rem
	bbdoc: The peer was received from the tracker.
	End Rem
	Const tracker:Int = $1
	Rem
	bbdoc: The peer was received from the kademlia DHT.
	End Rem
	Const dht:Int = $2
	Rem
	bbdoc: The peer was received from the peer exchange extension.
	End Rem
	Const pex:Int = $4
	Rem
	bbdoc: The peer was received from the local service discovery (The peer is on the local network).
	End Rem
	Const lsd:Int = $8
	Rem
	bbdoc: The peer was added from the fast resume data.
	End Rem
	Const resumeData:Int = $10
	Rem
	bbdoc: 
	End Rem
	Const incoming:Int = $20
	
	Const bwIdle:Int = 0
	Const bwTorrent:Int = 1
	Const bwGlobal:Int = 2
	Const bwNetwork:Int = 3

	Field infoPtr:Byte Ptr
	
	Function _create:TPeerInfo(infoPtr:Byte Ptr)
		If infoPtr Then
			Local this:TPeerInfo = New TPeerInfo
			this.infoPtr = infoPtr
			Return this
		End If
	End Function
	
	Function _newArray:TPeerInfo[](size:Int)
		Return New TPeerInfo[size]
	End Function
	
	Function _newEntry(array:TPeerInfo[], index:Int, infoPtr:Byte Ptr)
		array[index] = TPeerInfo._create(infoPtr)
	End Function
	
	Rem
	bbdoc: Describes in which state the peer is.
	End Rem
	Method flags:Int()
		Return bmx_torrent_peerinfo_flags(infoPtr)
	End Method
	
	Rem
	bbdoc: A combination of flags describing from which sources this peer was received.
	End Rem
	Method source:Int()
		Return bmx_torrent_peerinfo_source(infoPtr)
	End Method
	
	Rem
	bbdoc: The IP-address to this peer.
	End Rem
	Method ip:String()
		Return bmx_torrent_peerinfo_ip(infoPtr)
	End Method
	
	Rem
	bbdoc: The current upload speed we have to this peer (including any protocol messages). 
	End Rem
	Method upSpeed:Float()
		Return bmx_torrent_peerinfo_up_speed(infoPtr)
	End Method
	
	Rem
	bbdoc: The current download speed we have from this peer (including any protocol messages). 
	End Rem
	Method downSpeed:Float()
		Return bmx_torrent_peerinfo_down_speed(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method payloadUpSpeed:Float()
		Return bmx_torrent_peerinfo_payload_up_speed(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method payloadDownSpeed:Float()
		Return bmx_torrent_peerinfo_payload_down_speed(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalDownload:Long()
		Local v:Long
		bmx_torrent_peerinfo_total_download(infoPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalUpload:Long()
		Local v:Long
		bmx_torrent_peerinfo_total_upload(infoPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method pieces:TBitfield()
		Return TBitfield._create(bmx_torrent_peerinfo_pieces(infoPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method uploadLimit:Int()
		Return bmx_torrent_peerinfo_upload_limit(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadLimit:Int()
		Return bmx_torrent_peerinfo_download_limit(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method lastRequest:Int()
		Return bmx_torrent_peerinfo_last_request(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method lastActive:Int()
		Return bmx_torrent_peerinfo_last_active(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method requestTimeout:Int()
		Return bmx_torrent_peerinfo_request_timeout(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method sendBufferSize:Int()
		Return bmx_torrent_peerinfo_send_buffer_size(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method usedSendBuffer:Int()
		Return bmx_torrent_peerinfo_used_send_buffer(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method receiveBufferSize:Int()
		Return bmx_torrent_peerinfo_receive_buffer_size(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method usedReceiveBuffer:Int()
		Return bmx_torrent_peerinfo_used_receive_buffer(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numHashfails:Int()
		Return bmx_torrent_peerinfo_num_hashfails(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method inetAsName:String()
		Return bmx_torrent_peerinfo_inet_as_name(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method inetAs:Int()
		Return bmx_torrent_peerinfo_inet_as(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method loadBalancing:Long()
		Local v:Long
		bmx_torrent_peerinfo_load_balancing(infoPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of request messages waiting to be sent inside the send buffer.
	End Rem
	Method requestsInBuffer:Int()
		Return bmx_torrent_peerinfo_requests_in_buffer(infoPtr)
	End Method
	
	Rem
	bbdoc: The number of requests that is tried to be maintained (this is typically a function of download speed)
	End Rem
	Method targetDlQueueLength:Int()
		Return bmx_torrent_peerinfo_target_dl_queue_length(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadQueueLength:Int()
		Return bmx_torrent_peerinfo_download_queue_length(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method uploadQueueLength:Int()
		Return bmx_torrent_peerinfo_upload_queue_length(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method failcount:Int()
		Return bmx_torrent_peerinfo_failcount(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadingPieceIndex:Int()
		Return bmx_torrent_peerinfo_downloading_piece_index(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadingBlockIndex:Int()
		Return bmx_torrent_peerinfo_downloading_block_index(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadingProgress:Int()
		Return bmx_torrent_peerinfo_downloading_progress(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadingTotal:Int()
		Return bmx_torrent_peerinfo_downloading_total(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method client:String()
		Return bmx_torrent_peerinfo_client(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method connectionType:Int()
		Return bmx_torrent_peerinfo_connection_type(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method remoteDlRate:Int()
		Return bmx_torrent_peerinfo_remote_dl_rate(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method pendingDiskBytes:Int()
		Return bmx_torrent_peerinfo_pending_disk_bytes(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method sendQuota:Int()
		Return bmx_torrent_peerinfo_send_quota(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method receiveQuota:Int()
		Return bmx_torrent_peerinfo_receive_quota(infoPtr)
	End Method
	
	Rem
	bbdoc: An estimated round trip time to this peer, in milliseconds.
	about: It is estimated by timing the the tcp connect(). It may be 0 for incoming connections.
	End Rem
	Method rtt:Int()
		Return bmx_torrent_peerinfo_rtt(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadRatePeak:Int()
		Return bmx_torrent_peerinfo_download_rate_peak(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method uploadRatePeak:Int()
		Return bmx_torrent_peerinfo_upload_rate_peak(infoPtr)
	End Method
	
	Rem
	bbdoc: The progress of the peer.
	End Rem
	Method progress:Float()
		Return bmx_torrent_peerinfo_progress(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method readState:Int()
		Return bmx_torrent_peerinfo_read_state(infoPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method writeState:Int()
		Return bmx_torrent_peerinfo_write_state(infoPtr)
	End Method
	
	Method Delete()
		If infoPtr Then
			bmx_torrent_peerinfo_free(infoPtr)
			infoPtr = Null
		End If
	End Method
	
End Type

Type TPESettings

End Type


Type TProxySettings

End Type

Rem
bbdoc: Stores any number of bits as a bitfield in an array.
End Rem
Type TBitfield

	Field bitPtr:Byte Ptr
	
	Function _create:TBitfield(bitPtr:Byte Ptr)
		If bitPtr Then
			Local this:TBitfield = New TBitfield
			this.bitPtr = bitPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the bit at the selected index.
	End Rem
	Method getBit:Int(index:Int)
		Return bmx_torrent_bitfield_get_bit(bitPtr, index)
	End Method
	
	Rem
	bbdoc: The number of bits.
	End Rem
	Method size:Int()
		Return bmx_torrent_bitfield_size(bitPtr)
	End Method

	Method Delete()
		If bitPtr Then
			bmx_torrent_bitfield_free(bitPtr)
			bitPtr = Null
		End If
	End Method
	
End Type

Type TFingerprint
End Type

Rem
bbdoc: Contains the state for all UPnP mappings.
End Rem
Type TUpnp

	Const PROTOCOL_NONE:Int = 0
	Const PROTOCOL_UDP:Int = 1
	Const PROTOCOL_TCP:Int = 2
	
	Field pnpPtr:Byte Ptr
	
	Function _create:TUpnp(pnpPtr:Byte Ptr)
		If pnpPtr Then
			Local this:TUpnp = New TUpnp
			this.pnpPtr = pnpPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Attempts to add a port mapping for the specified protocol.
	End Rem
	Method addMapping:Int(protocol:Int, externalPort:Int, localPort:Int)
		Return bmx_torrent_upnp_add_mapping(pnpPtr, protocol, externalPort, localPort)
	End Method
	
	Rem
	bbdoc: Removes a port mapping.
	End Rem
	Method deleteMapping(mappingIndex:Int)
		bmx_torrent_upnp_delete_mapping(pnpPtr, mappingIndex)
	End Method
	
	Rem
	bbdoc: If the model is advertized by the router, it can be queried through this method.
	End Rem
	Method routerModel:String()
		Return bmx_torrent_upnp_router_model(pnpPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TNatpmp

	Field pmpPtr:Byte Ptr

	Function _create:TNatpmp(pmpPtr:Byte Ptr)
		If pmpPtr Then
			Local this:TNatpmp = New TNatpmp
			this.pmpPtr = pmpPtr
			Return this
		End If
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TFileStorage

	Field storagePtr:Byte Ptr

	Method Create:TFileStorage(files:String = Null)
		If files Then
			'storagePtr = bmx_torrent_file_storage_create(files)
		Else
			'storagePtr = bmx_torrent_file_storage_create(Null)
		End If
		
		Return Self
	End Method

	Method isValid:Int()
		'Return bmx_torrent_file_storage_is_valid(storagePtr)
	End Method
	
	
	
	Method Delete()
		If storagePtr Then
			'bmx_torrent_file_storage_free(storagePtr)
			storagePtr = Null
		End If
	End Method
	
End Type

Type TCreateTorrent

End Type

Rem
bbdoc: The disk cache status.
End Rem
Type TCacheStatus

	Field statusPtr:Byte Ptr
	
	Function _create:TCacheStatus(statusPtr:Byte Ptr)
		If statusPtr Then
			Local this:TCacheStatus = New TCacheStatus
			this.statusPtr = statusPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: The total number of 16 KiB blocks written to disk since this session was started.
	End Rem
	Method blocks_written:Long()
		Local v:Long
		bmx_torrent_cachestatus_blocks_written(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The total number of write operations performed since this session was started.
	End Rem
	Method writes:Long()
		Local v:Long
		bmx_torrent_cachestatus_writes(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of blocks that were requested from the bittorrent engine (from peers), that were served from disk or cache.
	End Rem
	Method blocks_read:Long()
		Local v:Long
		bmx_torrent_cachestatus_blocks_read(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of blocks that were served from cache.
	End Rem
	Method blocks_read_hit:Long()
		Local v:Long
		bmx_torrent_cachestatus_blocks_read_hit(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The total number of read operations performed since this session was started.
	End Rem
	Method reads:Long()
		Local v:Long
		bmx_torrent_cachestatus_reads(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: The number of 16 KiB blocks currently in the disk cache.
	about: This includes both read and write cache.
	End Rem
	Method cache_size:Int()
		Return bmx_torrent_cachestatus_cache_size(statusPtr)
	End Method
	
	Rem
	bbdoc: The number of 16KiB blocks in the read cache.
	End Rem
	Method read_cache_size:Int()
		Return bmx_torrent_cachestatus_read_cache_size(statusPtr)
	End Method

	Method Delete()
		If statusPtr Then
			bmx_torrent_cachestatus_free(statusPtr)
			statusPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TPartialPieceInfo

	Field infoPtr:Byte Ptr

	Rem
	bbdoc: No peer is currently downloading any part of the piece.
	about: Peers prefer picking pieces from the same category as themselves. The reason for this is to keep the number of partially
	downloaded pieces down.
	End Rem
	Const none:Int = 0
	Rem
	bbdoc: The slow download rate category.
	End Rem
	Const slow:Int = 1
	Rem
	bbdoc: The medium download rate category.
	End Rem
	Const medium:Int = 2
	Rem
	bbdoc: The fast download rate category.
	End Rem
	Const fast:Int = 3
	
	Function _create:TPartialPieceInfo(infoPtr:Byte Ptr)
		If infoPtr Then
			Local this:TPartialPieceInfo = New TPartialPieceInfo
			this.infoPtr = infoPtr
			Return this
		End If
	End Function

	Function _newArray:TPartialPieceInfo[](size:Int)
		Return New TPartialPieceInfo[size]
	End Function
	
	Function _newEntry(array:TPartialPieceInfo[], index:Int, infoPtr:Byte Ptr)
		array[index] = TPartialPieceInfo._create(infoPtr)
	End Function
	
	Rem
	bbdoc: The index of the piece in question.
	End Rem
	Method pieceIndex:Int()
		Return bmx_torrent_partialpieceinfo_piece_index(infoPtr)
	End Method
	
	Rem
	bbdoc: The number of blocks in this particular piece.
	about: This number will be the same for most pieces, but the last piece may have fewer blocks than the standard pieces.
	End Rem
	Method blocksInPiece:Int()
		Return bmx_torrent_partialpieceinfo_blocks_in_piece(infoPtr)
	End Method
	
	Rem
	bbdoc: Returns an array of data for each individual block in the piece.
	End Rem
	Method blocks:TBlockInfo[]()
		Return bmx_torrent_partialpieceinfo_blocks(infoPtr)
	End Method
	
	Rem
	bbdoc: One of fast, medium, slow or none.
	about: It tells which download rate category the peers downloading this piece falls into. Pieces set to none can be converted into
	any of fast, medium or slow as soon as a peer want to download from it.
	End Rem
	Method pieceState:Int()
		Return bmx_torrent_partialpieceinfo_piece_state(infoPtr)
	End Method

	Method Delete()
		If infoPtr Then
			bmx_torrent_partialpieceinfo_free(infoPtr)
			infoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Block information for a single piece.
End Rem
Type TBlockInfo

	Field infoPtr:Byte Ptr

	Rem
	bbdoc: This block has not been downloaded or requested form any peer.
	End Rem
	Const none:Int = 0
	Rem
	bbdoc: The block has been requested, but not completely downloaded yet.
	End Rem
	Const requested:Int = 1
	Rem
	bbdoc: The block has been downloaded and is currently queued for being written to disk.
	End Rem
	Const writing:Int = 2
	Rem
	bbdoc: The block has been written to disk.
	End Rem
	Const finished:Int = 3
	
	Function _create:TBlockInfo(infoPtr:Byte Ptr)
		If infoPtr Then
			Local this:TBlockInfo = New TBlockInfo
			this.infoPtr = infoPtr
			Return this
		End If
	End Function

	Function _newArray:TBlockInfo[](size:Int)
		Return New TBlockInfo[size]
	End Function
	
	Function _newEntry(array:TBlockInfo[], index:Int, infoPtr:Byte Ptr)
		array[index] = TBlockInfo._create(infoPtr)
	End Function
	
	Rem
	bbdoc: The ip address of the peer this block was downloaded from.
	End Rem
	Method peer:String()
		Return bmx_torrent_blockinfo_peer(infoPtr)
	End Method
	
	Rem
	bbdoc: The block state.
	about: One of, none, requested, writing or finished.
	End Rem
	Method state:Int()
		Return bmx_torrent_blockinfo_state(infoPtr)
	End Method
	
	Rem
	bbdoc: The number of peers that is currently requesting this block.
	about: Typically this is 0 or 1, but at the end of the torrent blocks may be requested by more peers in parallel to speed things up.
	End Rem
	Method numPeers:Int()
		Return bmx_torrent_blockinfo_num_peers(infoPtr)
	End Method

	Method Delete()
		If infoPtr Then
			bmx_torrent_blockinfo_free(infoPtr)
			infoPtr = Null
		End If
	End Method
	
End Type
