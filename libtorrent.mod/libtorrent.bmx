' Copyright (c) 2010 Bruce A Henderson
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
ModuleInfo "Copyright: Wrapper - 2010 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (libtorrent 0.14.9)"

ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -DTORRENT_USE_OPENSSL -DTORRENT_NO_DEPRECATE"

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
	
	Rem
	bbdoc: 
	End Rem
	Method setUploadRateLimit(bytesPerSecond:Int)
		bmx_torrent_session_set_upload_rate_limit(sessionPtr, bytesPerSecond)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setDownloadRateLimit(bytesPerSecond:Int)
		bmx_torrent_session_set_download_rate_limit(sessionPtr, bytesPerSecond)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method uploadRateLimit:Int()
		Return bmx_torrent_session_upload_rate_limit(sessionPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadRateLimit:Int()
		Return bmx_torrent_session_download_rate_limit(sessionPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method setMaxUploads(limit:Int)
		bmx_torrent_session_set_max_uploads(sessionPtr, limit)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setMaxConnections(limit:Int)
		bmx_torrent_session_set_max_connections(sessionPtr, limit)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method numUploads:Int()
		Return bmx_torrent_session_num_uploads(sessionPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numConnections:Int()
		Return bmx_torrent_session_num_connections(sessionPtr)
	End Method
	
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
	
	Method popAlert:TAlert()
		Return TAlert(bmx_torrent_session_pop_alert(sessionPtr))
	End Method
	
	Method startUpnp:TUpnp()
	End Method
	
	Method stopUpnp()
	End Method
	
	Method startNatpmp:TNatpmp()
	End Method
	
	Method stopNatpmp()
	End Method
	
	Method startLsd()
	End Method
	
	Method stopLsd()
	End Method

	Method setMaxHalfOpenConnections(value:Int)
	End Method
	
	Method maxHalfOpenConnections:Int()
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
	bbdoc: 
	End Rem
	Method uploadRate:Float()
		Return bmx_torrent_sessionstatus_upload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadRate:Float()
		Return bmx_torrent_sessionstatus_download_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method payloadUploadRate:Float()
		Return bmx_torrent_sessionstatus_payload_upload_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method payloadDownloadRate:Float()
		Return bmx_torrent_sessionstatus_payload_download_rate(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalDownload:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_download(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalUpload:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_upload(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method totalRedundantBytes:Long()
		Local v:Long
		bmx_torrent_sessionstatus_total_redundant_bytes(statusPtr, Varptr v)
		Return v
	End Method
	
	Rem
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method numPeers:Int()
		Return bmx_torrent_sessionstatus_num_peers(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method numUnchoked:Int()
		Return bmx_torrent_sessionstatus_num_unchoked(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method allowedUploadSlots:Int()
		Return bmx_torrent_sessionstatus_allowed_upload_slots(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method dhtNodes:Int()
		Return bmx_torrent_sessionstatus_dht_nodes(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method dhtNodeCache:Int()
		Return bmx_torrent_sessionstatus_dht_node_cache(statusPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method dhtTorrents:Int()
		Return bmx_torrent_sessionstatus_dht_torrents(statusPtr)
	End Method
	
	Rem
	bbdoc: 
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
	
	Method setTrackerUrl(url:String)
		If urlPtr Then
			MemFree(urlPtr)
		End If

		urlPtr = url.ToUTF8String()
		bmx_torrent_addtorrentparams_set_tracker_url(paramsPtr, urlPtr)
	End Method
	
	Method trackerUrl:String()
		Return bmx_torrent_addtorrentparams_tracker_url(paramsPtr)
	End Method
	
	Method setName(name:String)
		If namePtr Then
			MemFree(namePtr)
		End If

		namePtr = name.ToUTF8String()
		bmx_torrent_addtorrentparams_set_name(paramsPtr, namePtr)
	End Method
	
	Method name:String()
		Return bmx_torrent_addtorrentparams_name(paramsPtr)
	End Method
	
	Method savePath:String()
		Return bmx_torrent_addtorrentparams_save_path(paramsPtr)
	End Method
	
	Method setStorageMode(_mode:Int)
		bmx_torrent_addtorrentparams_set_storage_mode(paramsPtr, _mode)
	End Method
	
	Method storageMode:Int()
		Return bmx_torrent_addtorrentparams_storage_mode(paramsPtr)
	End Method
	
	Method setPaused(value:Int)
		bmx_torrent_addtorrentparams_set_paused(paramsPtr, value)
	End Method
	
	Method paused:Int()
		Return bmx_torrent_addtorrentparams_paused(paramsPtr)
	End Method
	
	Method setAutoManaged(value:Int)
		bmx_torrent_addtorrentparams_set_auto_managed(paramsPtr, value)
	End Method
	
	Method autoManaged:Int()
		Return bmx_torrent_addtorrentparams_auto_managed(paramsPtr)
	End Method
	
	Method setDuplicateIsError(value:Int)
		bmx_torrent_addtorrentparams_set_duplicate_is_error(paramsPtr, value)
	End Method
	
	Method duplicateIsError:Int()
		Return bmx_torrent_addtorrentparams_duplicate_is_error(paramsPtr)
	End Method
	
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
Warning : All operations on a TTorrentHandle may throw invalid_handle exception, in case the handle is no longer refering to a torrent.
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
	bbdoc: 
	End Rem
	Method isValid:Int()
		Return bmx_torrent_torrenthandle_is_valid(torrentPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method status:TTorrentStatus()
		Return TTorrentStatus._create(bmx_torrent_torrenthandle_status(torrentPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method name:String()
		Return bmx_torrent_torrenthandle_name(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method forceReannounce()
		bmx_torrent_torrenthandle_force_reannounce(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method scrapeTracker()
		bmx_torrent_torrenthandle_scrape_tracker(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setTrackerLogin(username:String, password:String)
		bmx_torrent_torrenthandle_set_tracker_login(torrentPtr, username, password)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method addUrlSeed(url:String)
		bmx_torrent_torrenthandle_add_url_seed(torrentPtr, url)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method removeUrlSeed(url:String)
		bmx_torrent_torrenthandle_remove_url_seed(torrentPtr, url)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method urlSeeds:String[]()
		Return bmx_torrent_torrenthandle_url_seeds(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setRatio(ratio:Float)
		bmx_torrent_torrenthandle_set_ratio(torrentPtr, ratio)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setMaxUploads(maxUploads:Int)
		bmx_torrent_torrenthandle_set_max_uploads(torrentPtr, maxUploads)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setMaxConnections(maxConnections:Int)
		bmx_torrent_torrenthandle_set_max_connections(torrentPtr, maxConnections)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setUploadLimit(limit:Int)
		bmx_torrent_torrenthandle_set_upload_limit(torrentPtr, limit)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method uploadLimit:Int()
		Return bmx_torrent_torrenthandle_upload_limit(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setDownloadLimit(limit:Int)
		bmx_torrent_torrenthandle_set_download_limit(torrentPtr, limit)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method downloadLimit:Int()
		Return bmx_torrent_torrenthandle_download_limit(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setSequentialDownload(value:Int)
		bmx_torrent_torrenthandle_set_sequential_download(torrentPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isSequentialDownload:Int()
		Return bmx_torrent_torrenthandle_is_sequential_download(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method useInterface(netInterface:String)
		bmx_torrent_torrenthandle_use_interface(torrentPtr, netInterface)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method pause()
		bmx_torrent_torrenthandle_pause(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method resume()
		bmx_torrent_torrenthandle_resume(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isPaused:Int()
		Return bmx_torrent_torrenthandle_is_paused(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method isSeed:Int()
		Return bmx_torrent_torrenthandle_is_seed(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method forceRecheck()
		bmx_torrent_torrenthandle_force_recheck(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method clearError()
		bmx_torrent_torrenthandle_clear_error(torrentPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method isAutoManaged:Int()
		Return bmx_torrent_torrenthandle_is_auto_managed(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method autoManaged(value:Int)
		bmx_torrent_torrenthandle_auto_managed(torrentPtr, value)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method hasMetadata:Int()
		Return bmx_torrent_torrenthandle_has_metadata(torrentPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method getPeerInfo:TPeerInfo[]()
		Return bmx_torrent_torrenthandle_get_peer_info(torrentPtr)
	End Method
	
End Type

Extern
	Function bmx_torrent_torrenthandle_get_peer_info:TPeerInfo[](handle:Byte Ptr)
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

	Method pieces:TBitfield()
		Return TBitfield._create(bmx_torrent_torrentstatus_pieces(statusPtr))
	End Method
	
	Method numPieces:Int()
		Return bmx_torrent_torrentstatus_num_pieces(statusPtr)
	End Method
	
	Method Delete()
		If statusPtr Then
			bmx_torrent_torrentstatus_free(statusPtr)
			statusPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TPeerInfo

	Const interesting:Int = $1
	Const choked:Int = $2
	Const remoteInterested:Int = $4
	Const remoteChoked:Int = $8
	Const supportsExtensions:Int = $10
	Const localConnection:Int = $20
	Const handshake:Int = $40
	Const connecting:Int = $80
	Const queued:Int = $100
	Const onParole:Int = $200
	Const seed:Int = $400
	Const optimisticUnchoke:Int = $800
	Const snubbed:Int = $1000
	Const uploadOnly:Int = $2000
	Const rc4Encrypted:Int = $100000
	Const plaintextEncrypted:Int = $200000

	
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

Type TUpnp

	Const PROTOCOL_NONE:Int = 0
	Const PROTOCOL_UDP:Int = 1
	Const PROTOCOL_TCP:Int = 2
	
	Field pnpPtr:Byte Ptr
	
	Method addMapping:Int(protocol:Int, externalPort:Int, localPort:Int)
	End Method
	
	Method deleteMapping(mappingIndex:Int)
	End Method
	
	Method routerModel:String()
	End Method
	
End Type

Type TNatpmp
End Type

