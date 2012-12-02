SuperStrict

Framework BaH.Libtorrent
Import BaH.Format
Import BRL.GLMax2D
Import BRL.FreeTypeFont
Import BRL.RAMStream
Import BRL.Map

Incbin "media/Pixel_Berry_08_84_Ltd.Edition.TTF"
Incbin "media/Share-TechMono.ttf"

Const COLOR_NORMAL:Int = 1
Const COLOR_BOLD:Int = 2
Const COLOR_GREEN:Int = 3
Const COLOR_BLUE:Int = 4
Const COLOR_RED:Int = 5
Const COLOR_MAGENTA:Int = 6
Const COLOR_YELLOW:Int = 7


Graphics 1000, 700

SetBlend alphablend

Global smallFont:TImageFont = LoadImageFont("incbin::media/Pixel_Berry_08_84_Ltd.Edition.TTF", 8, 0)
Global mainFont:TImageFont = LoadImageFont("incbin::media/Share-TechMono.ttf", 14)
Global tinyFont:TImageFont = LoadImageFont("incbin::media/Share-TechMono.ttf", 10)

SetImageFont(mainFont)


' search the current directory for .torrent files
Local fileList:TList = findFiles(CurrentDir())



' defaults (lazy lazy globals... :-p )
Global wait_retry:Int = 30
Global bind_port_start:Int = 0
Global bind_port_end:Int = 0
Global listen_port:Int = 6881
Global preferred_ratio:Float = 0
Global download_limit:Int = 0
Global upload_limit:Int = 0
Global torrent_upload_limit:Int = 20
Global torrent_download_limit:Int = 0
Global upload_slots_limit:Int = 5
Global half_open_limit:Int = -1
Global bind_to_interface:String = ""


Local process:TMainProcess = New TMainProcess
process.init(fileList)

process.mainLoop()

End

Type TMainProcess

	Field session:TSession
	Field handles:TList = New TList
	Field activeHandle:TTorrentHandle
	Field torrentIndex:Int = 0

	Field print_peers:Int = False
	Field print_log:Int = False
	Field print_downloads:Int = False
	Field print_piece_bar:Int = False
	Field print_file_progress:Int = False
	Field print_ip:Int = True
	Field print_as:Int = False
	Field print_timers:Int = False
	Field print_block:Int = False
	Field print_peer_rate:Int = False
	Field print_fails:Int = False
	Field print_send_bufs:Int = True


	Global states:String[] = ["checking (q)", "checking", "dl metadata" , "downloading", "finished", "seeding", "allocating"]
	
	Global formatBuilder:TFormatter = TFormatter.Create("%d.%df")
	
	Method init(files:TList)
	
		Local settings:TSessionSettings = New TSessionSettings
		settings.setUserAgent("client_test/" + LibtorrentVersion())
		settings.setUrlseedWaitRetry(wait_retry)
		settings.setOutgoingPorts(bind_port_start, bind_port_end)
		

		session = New TSession
		' UPnP port mapping
		session.startUpnp()
		' NAT-PMP port mapping
		session.startNatpmp()
		' Local service discovery (finds peers on the local network)
		session.startLsd()
		
		'settings.setMaxUploads(upload_slots_limit)
		'settings.setMaxHalfOpenConnections(half_open_limit)
		settings.setDownloadRateLimit(download_limit)
		settings.setUploadRateLimit(upload_limit)
		session.listenOn(listen_port, listen_port + 10, bind_to_interface)
		session.setSettings(settings)

		For Local file:String = EachIn files
			addTorrent(file)
		Next
		
	End Method
	
	Method addTorrent(file:String)
	
		Local info:TTorrentInfo = TTorrentInfo.Load(file)
		
		Local params:TAddTorrentParams = New TAddTorrentParams
		params.setSavePath("./") ' current dir...
		params.setTorrentInfo(info)

		' setup default torrent flags
		Local flags:Int = params.flags()
		flags :| TAddTorrentParams.PAUSED
		flags :~ TAddTorrentParams.DUPLICATE_IS_ERROR
		flags :| TAddTorrentParams.AUTO_MANAGED

		params.setFlags(flags)
		
		Local handle:TTorrentHandle = session.addTorrent(params)
		
		handles.AddLast(handle)
		If Not activeHandle Then
			activeHandle = handle
		End If
		
		handle.setMaxConnections(50)
		handle.setMaxUploads(-1)
		'handle.setRatio(preferred_ratio)
		handle.setUploadLimit(torrent_upload_limit)
		handle.setDownloadLimit(torrent_download_limit)
		
	End Method

	Method mainLoop()
	
		While Not KeyDown(KEY_ESCAPE)
			
			update()
			
			Cls
			
			display()
			
			Flip
			
			' take a small nap
			Delay 100
		Wend
	
	End Method

	Method update()
	
		' tidy up the handles list
		Local removeList:TList = New TList
		For Local handle:TTorrentHandle = EachIn handles
			If Not handle.isValid() Then
				If activeHandle = handle Then
					activeHandle = Null
				End If
				removeList.AddLast(handle)
			End If
		Next
		For Local handle:TTorrentHandle = EachIn removeList
			handles.remove(handle)
		Next
		
		' reset the activeHandle if required
		If Not activeHandle And handles.count() > 0 Then
			activeHandle = TTorrentHandle(handles.First())
		End If
		
		
		' check for user input
		If KeyHit(KEY_SPACE) Then
			If session.isPaused() Then
				session.resume()
			Else
				session.pause()
			End If
		End If
		
		' toggle displays
		If KeyHit(KEY_I) print_peers = Not print_peers
		If KeyHit(KEY_L) print_log = Not print_log
		If KeyHit(KEY_D) print_downloads = Not print_downloads
		If KeyHit(KEY_F) print_file_progress = Not print_file_progress
		If KeyHit(KEY_A) print_piece_bar = Not print_piece_bar
		' toggle columns
		If KeyHit(KEY_1) print_ip = Not print_ip
		If KeyHit(KEY_2) print_as = Not print_as
		If KeyHit(KEY_3) print_timers = Not print_timers
		If KeyHit(KEY_4) print_block = Not print_block
		If KeyHit(KEY_5) print_peer_rate = Not print_peer_rate
		If KeyHit(KEY_6) print_fails = Not print_fails
		If KeyHit(KEY_7) print_send_bufs = Not print_send_bufs
		
		If KeyHit(KEY_J) Then
			If activeHandle And activeHandle.isValid() Then
				activeHandle.forceRecheck()
			End If
		End If

		If KeyHit(KEY_R) Then
			If activeHandle And activeHandle.isValid() Then
				activeHandle.forceReannounce()
			End If
		End If

		If KeyHit(KEY_S) Then
			If activeHandle And activeHandle.isValid() Then
				Local status:TTorrentStatus = activeHandle.status()
				activeHandle.setSequentialDownload(Not status.sequentialDownload())
			End If
		End If

		If KeyHit(KEY_V) Then
			If activeHandle And activeHandle.isValid() Then
				activeHandle.scrapeTracker()
			End If
		End If
	
		If KeyHit(KEY_C) Then
			If activeHandle And activeHandle.isValid() Then
				activeHandle.clearError()
			End If
		End If
	
		Local newHandle:Int = False
		If KeyHit(KEY_UP) Then
			torrentIndex :- 1
			If torrentIndex < 0 Then
				torrentIndex = handles.count() - 1
			End If
			
			newHandle = True
		End If
		
		If KeyHit(KEY_DOWN) Then
			torrentIndex :+ 1
			If torrentIndex >= handles.count() Then
				torrentIndex = 0
			End If

			newHandle = True
		End If
		
		If newHandle Then
			Local i:Int = 0
			For Local handle:TTorrentHandle = EachIn handles
				If i = torrentIndex Then
					activeHandle = handle
					Exit
				End If
				
				i:+ 1
			Next	
		End If
	
	End Method

	Method display()
		color()
	
		drawHelp()
		drawTorrentsList()

		color()
		SetImageFont(mainFont)
		
		Local stat:TSessionStatus = session.status()
		
		' display the active handle
		If activeHandle Then
		
			Local status:TTorrentStatus = activeHandle.status()
			Local paused:Int = status.paused()
			Local autoManaged:Int = status.autoManaged()
			Local sequentialDownload:Int = status.sequentialDownload()
			
			
			DrawText "name:", 10, 70
			DrawText "download:", 10, 90
			DrawText "upload:", 170, 90
			DrawText "swarm:", 390, 90
			DrawText "bw queue:", 550, 90
			DrawText "all-time (Rx:", 10, 110
			DrawText "Tx:        )", 190, 110
			
			
			color(COLOR_BOLD)
			
			Local name:String = activeHandle.name()
			If name.length > 50 Then
				name = name[..50]
			End If
			DrawText name, 60, 70
			
			If Not status.error() Then
			
				
				Local state:String
				If paused And Not autoManaged Then
					color(COLOR_RED)
					state = "paused"
				Else If paused And autoManaged Then
					color(COLOR_RED)
					state = "queued"
				Else
					color(COLOR_BLUE)
					state = states[status.state()]
				End If
			
				DrawText "( " + state + " )", 480, 70
			
			
				Local seeds:Int
				If status.numComplete() >= 0 Then
					seeds = status.numComplete()
				Else
					seeds = status.listSeeds()
				End If
				
				Local downloaders:Int
				If status.numIncomplete() >= 0 Then
					downloaders = status.numIncomplete()
				Else
					downloaders = status.listPeers() - status.listSeeds()
				End If
				
				color(COLOR_GREEN)
				DrawText "(" + addSuffix(status.totalDownload()) + ")", 90, 90
				color(COLOR_RED)
				If status.uploadRate() > 0 Then
					DrawText addSuffix(status.uploadRate()) + "/s (" + addSuffix(status.totalUpload()) + ")", 235, 90
				Else
					DrawText "         (" + addSuffix(status.totalUpload()) + ")", 235, 90
				End If

				color(COLOR_BOLD)
				DrawText fixedString(downloaders, 4) + ":" + fixedString(seeds, 4), 440, 90
				DrawText "(" + status.upBandwidthQueue() + " | " + status.downBandwidthQueue() + ")", 630, 90
				color(COLOR_GREEN)
				DrawText addSuffix(status.allTimeDownload()), 125, 110
				color(COLOR_RED)
				DrawText addSuffix(status.allTimeUpload()), 220, 110
				color(COLOR_BOLD)
				DrawText hexString(status.seedRank()) + " " + status.lastScrape(), 300, 110
				
				
				If status.state() <> TTorrentStatus.STATE_SEEDING Then
				
					Local progressBarColor:Int = COLOR_YELLOW
					
					If status.state() = TTorrentStatus.STATE_CHECKING_FILES Or ..
							status.state() = TTorrentStatus.STATE_CHECKING_FILES Then
						
						progressBarColor = COLOR_MAGENTA
						
					Else If Not status.currentTracker() Then
					
						progressBarColor = COLOR_RED
					
					Else If stat.hasIncomingConnections() Then
						
						progressBarColor = COLOR_GREEN
						
					End If
					
					color()
					Local s:String
					If sequentialDownload Then
						s = "sequential:"
					Else
						s = "  progress:"
					End If
					DrawText s , 10, 150
					
					color(COLOR_GREEN)
					DrawText status.totalDone() + " Bytes", 110, 150
					DrawText getString(status.progress() * 100, 5, 4) + "%", 260, 150
					
					drawProgressBar(status.progress(), 330, 150, 50, progressBarColor)
					
					If print_piece_bar And status.progress() < 1 Then
						color()
						drawPieceBar(status.pieces(), 25, 170, 88)
					End If
					
					color()
					DrawText "  peers:", 10, 190
					DrawText "seeds:", 120, 190
					DrawText "distributed copies:", 220, 190
					DrawText "download:", 440, 190
					
					color(COLOR_BOLD)
					DrawText status.numPeers(), 85, 190
					DrawText status.numSeeds(), 180, 190
					DrawText getString(status.distributedCopies(), 4), 385, 190
					If status.downloadRate() > 0 Then
						DrawText addSuffix(status.downloadRate()) + "/s", 525, 190
					End If
				End If
				
				Local yOffset:Int = 230
				
				If activeHandle.isValid() Then
				
					Local peers:TPeerInfo[]
				
					If (print_downloads And status.state() <> TTorrentStatus.STATE_SEEDING) Or print_peers Then
						peers = activeHandle.getPeerInfo()
					End If
					
					If print_peers And peers Then
						yOffset = drawPeerInfo(peers, yOffset)
					End If
				
				End If

			Else
			
				color()
				DrawText "error: ", 10, 100
				color(COLOR_BOLD)
				DrawText status.error(), 50, 100
				
			End If
		
		End If
		
	End Method
	
	Method drawHelp()
		SetImageFont(smallFont)

		DrawText "[ESC] quit [i] toggle peers [d] toggle downloading pieces [p] toggle paused [a] toggle piece bar [s] toggle download sequential [f] toggle files", 120, 5
		DrawText "[j] force recheck [SPACE] toggle session pause [c] clear error [v] scrape [UP] previous torrent [DOWN] next torrent", 120, 20
		DrawText "[1] toggle IP [2] toggle AS [3] toggle timers [4] toggle block progress [5] toggle peer rate [6] toggle failures [7] toggle send buffers", 120, 35

		SetImageFont(mainFont)
	End Method
	
	Method drawProgressBar(progress:Float, x:Int, y:Int, width:Int, pbColor:Int)
		Local count:Int = progress * width + 0.5
		
		Local s:String = "["
		Local p:String
		For Local i:Int = 0 Until width
			If i < count Then
				s:+ " "
				p:+ "#"
			Else
				s:+ "-"
			End If
		Next
		s:+ "]"
		
		color()
		DrawText s, x, y
		
		color(pbColor)
		DrawText p, x + 8, y
		
	End Method
	
	Method drawPieceBar(bitfield:TBitfield, x:Int, y:Int, width:Int)
		Global lookup:String[] = [" ", ".", ":", "-", "+", "*", "#"]
		
		Local piecesPerChar:Double = bitfield.size() / Double(width)
		Local bar:String = "["
		If bitfield.size() = 0 Then
			For Local i:Int = 0 Until width
				bar:+ " "
			Next
			DrawText bar + "]", x, y
			Return
		End If
		
		Local piece:Double
		For Local i:Int = 0 Until width
			Local numPieces:Int
			Local numHave:Int
			
			Local last:Int = Max(Int(piece + piecesPerChar), Int(piece) + 1)
			For Local k:Int = piece Until last
				numPieces:+ 1
				If bitfield.getBit(k) Then
					numHave:+ 1
				End If
			Next
			Local col:Int = Ceil(numHave / Float(numPieces * 6))
			bar:+ lookup[col]
			
			piece:+ piecesPerChar
		Next
		DrawText bar + "]", x, y
		
	End Method
	
	Method drawTorrentsList()
		SetImageFont(mainFont)

		color(COLOR_BOLD)
		DrawText "Torrents", 800, 60
		color()

		SetImageFont(smallFont)
		Local i:Int
		
		
		For Local handle:TTorrentHandle = EachIn handles
		
			Local offset:Int = 80 + i * 15
		
			If handle.isValid() Then
				
				If activeHandle = handle Then
					color(COLOR_GREEN)
				End If

				DrawText handle.name(), 800, offset
				
				i:+ 1
			End If
			
			color()
		Next
	End Method
	
	Method drawPeerInfo:Int(peers:TPeerInfo[], y:Int)
		SetImageFont(tinyFont)
	
		Local s:String
		
		If print_ip Then s:+ "IP                     "
		s:+ "down     (total | peak   ) up       (total | peak   ) sent-req recv flags          source "
		If print_fails s:+ "fail hshf "
		If print_send_bufs s:+ "rq sndb            quota rcvb            "
		If print_timers s:+ "inactive wait timeout "

		
		s:+ "disk    rtt "
		If print_block s:+ "block-progress "
		If print_peer_rate s:+ "peer-rate "
		s:+ "client"
		
		DrawText s, 10, y
		
		Const rowHeight:Int = 11
		Local yOffset:Int = y + rowHeight
		
		For Local info:TPeerInfo = EachIn peers
		
			Local xOffset:Int = 10

			If info.flags() & TPeerInfo.handshake Then
				Continue
			End If
				
			color()
			
			If print_ip Then
				DrawText info.ip(), xOffset, yOffset
				xOffset:+ 115
			End If

			' Downloads
			color(COLOR_GREEN)
			If info.downSpeed() > 0 Then
				DrawText addSuffix(info.downSpeed()) + "/s", xOffset, yOffset
			End If

			s = "("
			If info.totalDownload() > 0 Then
				s:+ addSuffix(info.totalDownload())
			Else
				s:+ "      "
			End If
			
			s:+ "|"
			
			If info.downloadRatePeak() > 0 Then
				s:+ addSuffix(info.downloadRatePeak()) + "/s"
			Else
				s:+ "        "
			End If
			s:+ ")"
			
			xOffset:+ 45
			DrawText s, xOffset, yOffset
			xOffset:+ 90
		

			' Uploads
			color(COLOR_RED)
			If info.upSpeed() > 0 Then
				DrawText addSuffix(info.upSpeed()) + "/s", xOffset, yOffset
			End If

			s = "("
			If info.totalUpload() > 0 Then
				s:+ addSuffix(info.totalUpload())
			Else
				s:+ "      "
			End If
			
			s:+ "|"
			
			If info.uploadRatePeak() > 0 Then
				s:+ addSuffix(info.uploadRatePeak()) + "/s"
			Else
				s:+ "        "
			End If
			s:+ ")"
			
			xOffset:+ 45
			DrawText s, xOffset, yOffset
			xOffset:+ 90
	
			' 
			color()
			s = fixedString(info.downloadQueueLength(), 3) + " ("
			s:+ fixedString(info.targetDlQueueLength(), 3) + ") "
			s:+ fixedString(info.uploadQueueLength(), 3)
			DrawText s, xOffset, yOffset
			xOffset:+ 70

			' flags
			s = ""
			Local flags:Int = info.flags()
			
			s:+ checkFlag(flags, TPeerInfo.interesting, "I")
			s:+ checkFlag(flags, TPeerInfo.choked, "C")
			s:+ checkFlag(flags, TPeerInfo.remoteInterested, "i")
			s:+ checkFlag(flags, TPeerInfo.remoteChoked, "c")
			s:+ checkFlag(flags, TPeerInfo.supportsExtensions, "e")
			s:+ checkFlag(flags, TPeerInfo.localConnection, "l", "r")
			s:+ checkFlag(flags, TPeerInfo.seed, "s")
			s:+ checkFlag(flags, TPeerInfo.onParole, "p")
			s:+ checkFlag(flags, TPeerInfo.optimisticUnchoke, "O")
			
			Select info.readState()
				Case TPeerInfo.bwTorrent
					s:+ "t"
				Case TPeerInfo.bwGlobal
					s:+ "r"
				Case TPeerInfo.bwNetwork
					s:+ "R"
				Default
					s:+ "."
			End Select

			Select info.writeState()
				Case TPeerInfo.bwTorrent
					s:+ "t"
				Case TPeerInfo.bwGlobal
					s:+ "w"
				Case TPeerInfo.bwNetwork
					s:+ "W"
				Default
					s:+ "."
			End Select
			
			s:+ checkFlag(flags, TPeerInfo.snubbed, "S")
			s:+ checkFlag(flags, TPeerInfo.uploadOnly, "U", "D")

			If flags & TPeerInfo.rc4Encrypted Then
				s:+ "E"
			Else If flags & TPeerInfo.plaintextEncrypted Then
				s:+ "e"
			Else
				s:+ "."
			End If
			
			s:+ " "
			
			s:+ checkFlag(info.source(), TPeerInfo.tracker, "T", "_")
			s:+ checkFlag(info.source(), TPeerInfo.pex, "P", "_")
			s:+ checkFlag(info.source(), TPeerInfo.dht, "D", "_")
			s:+ checkFlag(info.source(), TPeerInfo.lsd, "L", "_")
			s:+ checkFlag(info.source(), TPeerInfo.resumeData, "R", "_")
			
			DrawText s, xOffset, yOffset
			

			xOffset:+ 110
			
			If print_fails Then
				xOffset:+ 5
				DrawText fixedString(info.failCount(), 3) + " " + fixedString(info.numHashFails(), 3), xOffset, yOffset
				xOffset:+ 45
			End If
			
			If print_send_bufs Then
				s = fixedString(info.requestsInBuffer(), 2) + " "
				s:+ fixedString(info.usedSendBuffer(), 6) + " ("
				s:+ addSuffix(info.sendBufferSize()) + ") "
				s:+ fixedString(info.sendQuota(), 5) + " "
				s:+ fixedString(info.usedReceiveBuffer(), 6) + " ("
				s:+ addSuffix(info.receiveBufferSize()) + ")"
				
				DrawText s, xOffset, yOffset
				xOffset:+ 205
			End If
			
			If print_timers Then
				s = fixedString(info.lastActive(), 8) + " "
				s:+ fixedString(info.lastRequest(), 4) + " "
				s:+ fixedString(info.requestTimeout(), 7)

				DrawText s, xOffset, yOffset
				xOffset:+ 100
			End If
			
			' pending disk bytes
			DrawText addSuffix(info.pendingDiskBytes()) + " " + fixedString(info.rtt(), 4), xOffset, yOffset
			xOffset :+ 60
			
			If print_block Then
				xOffset:+ 50
			End If
			
			If print_peer_rate Then
				xOffset:+ 80
			End If
			
			
			If info.flags() & TPeerInfo.handshake Then
				color(COLOR_RED)
				DrawText "waiting for handshake", xOffset, yOffset
			Else If info.flags() & TPeerInfo.connecting Then
				color(COLOR_RED)
				DrawText "connecting to peer", xOffset, yOffset
			Else If info.flags() & TPeerInfo.queued Then
				color(COLOR_YELLOW)
				DrawText "queued", xOffset, yOffset
			Else
				DrawText info.client(), xOffset, yOffset
			End If
	
			yOffSet:+ rowHeight
		Next
		
		
		SetImageFont(mainFont)
		Return y
	End Method
	
	Function checkFlag:String(flag:Int, value:Int, character:String, def:String = ".")
		If flag & value Then
			Return character
		Else
			Return def
		End If
	End Function
	
	Function addSuffix:String(value:Float)
		Global prefix:String[] = ["kB", "MB", "GB", "TB"]

		For Local i:Int = 0 Until prefix.length
			value :/ 1000.0
			If Abs(value) < 1000 Then
				Return getString(value, 4) + prefix[i]
			End If
		Next

		Return getString(value, 4) + "PB"		
	End Function
	
	Function getString:String(value:Float, width:Int, precision:Int = 3)
		Global formats:TMap = New TMap
	
		Local format:String = "%" + formatBuilder.IntArg(width).IntArg(precision).Format()
		formatBuilder.Clear()

		Local formatter:TFormatter = TFormatter(formats.ValueForKey(format))
		
		If Not formatter Then
			formatter = TFormatter.Create(format)
			formats.Insert(format, formatter)
		End If
		
		formatter.Clear()
		Local s:String = formatter.FloatArg(value).Format()
		If s.length > width Then
			s = s[..width]
		End If
		
		' strip dot
		If s.EndsWith(".") Then
			s = " " + s[..s.length -1]
		End If
		
		Return s
	
	End Function
	
	Function fixedString:String(value:Int, width:Int)
		Global formats:TMap = New TMap
		
		Local format:String = "%" + width + "d"
		Local formatter:TFormatter = TFormatter(formats.ValueForKey(format))
		
		If Not formatter Then
			formatter = TFormatter.Create(format)
			formats.Insert(format, formatter)
		End If
		
		formatter.Clear()
		Return formatter.IntArg(value).Format()
		
	End Function
	
	Function hexString:String(value:Int)
		Global formatter:TFormatter = TFormatter.Create("%0X")
		
		formatter.Clear()
		Return formatter.IntArg(value).format()
	End Function
	
End Type

Function findFiles:TList(path:String)
	Local files:TList = New TList

	Local dir:String[] = LoadDir(path)

	For Local s:String = EachIn dir
		If s.EndsWith(".torrent") Then
			files.AddLast(s)
		End If
	Next

	Return files
End Function

Function color(c:Int = COLOR_NORMAL)
	Global currentColor:Int = COLOR_NORMAL
	
	If currentColor = c Then
		Return
	End If
	
	Select c
		Case COLOR_NORMAL
			SetColor 200, 200, 200
		Case COLOR_BOLD
			SetColor 255, 255, 255
		Case COLOR_GREEN
			SetColor 0, 255, 0
		Case COLOR_BLUE
			SetColor 110, 110, 255
		Case COLOR_RED
			SetColor 255, 110, 110
		Case COLOR_MAGENTA
			SetColor 221, 160, 221
		Case COLOR_YELLOW
			SetColor 255, 255, 100
	End Select
	
	currentColor = c
End Function
