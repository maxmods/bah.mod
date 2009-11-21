SuperStrict

Framework BaH.ScriptingBridge
Import BRL.GLMax2D
Import BRL.StandardIO
Import BRL.PNGLoader
Import BRL.JPGLoader
Import BRL.RamStream

Local player:iTunes = New iTunes
Local display: DisplayStatus = New DisplayStatus.Create(player)

Graphics 800, 600, 0


While Not KeyDown(KEY_ESCAPE)

	Cls
	
		display.update()
		display.draw()
		
	Flip

Wend

End

Const STATE_STOPPED:Int = Asc("k") Shl 24 | Asc("P") Shl 16 | Asc( "S") Shl 8 | Asc("S")
Const STATE_PLAYING:Int = Asc("k") Shl 24 | Asc("P") Shl 16 | Asc( "S") Shl 8 | Asc("P")
Const STATE_PAUSED:Int = Asc("k") Shl 24 | Asc("P") Shl 16 | Asc( "S") Shl 8 | Asc("p")
Const STATE_FASTFORWARDING:Int = Asc("k") Shl 24 | Asc("P") Shl 16 | Asc( "S") Shl 8 | Asc("F")
Const STATE_REWINDING:Int = Asc("k") Shl 24 | Asc("P") Shl 16 | Asc( "S") Shl 8 | Asc("R")

Type iTunes

	Field app:SBApplication

	Method New()
		app = SBApplication.applicationWithBundleIdentifier("com.apple.iTunes")
	End Method
	
	Method currentTrack:TTrack()
		Return New TTrack.Create(app.propertyAsObject("currentTrack"))
	End Method
	
	Method position:Int()
		Return app.propertyAsInt("playerPosition")
	End Method
	
	' returns one of STATE_STOPPED, STATE_PLAYING, STATE_PAUSED, STATE_FASTFORWARDING or STATE_REWINDING.
	Method state:Int()
		Return app.propertyAsInt("playerState")
	End Method
	
End Type

Type TTrack

	Field track:SBObject

	Field album:String
	Field name:String
	Field length:Int
	Field art:TImage
	
	Method Create:TTrack(track:SBObject)
		Self.track = track
		
		album = track.propertyAsString("album")
		name = track.propertyAsString("name")
		length = track.propertyAsDouble("duration")

		Return Self
	End Method
	
	Method Load()
		' artwork ?
		Local artworks:SBElementArray = track.propertyAsList("artworks")
		For Local artwork:SBObject = EachIn artworks
			Local size:Int
			Local data:Byte Ptr = artwork.propertyAsPtr("rawData", size)
			
			If size > 0 Then
				Local stream:TStream = CreateRamStream(data, size, True, False)
				art = LoadImage(stream)
			End If
			
			Exit ' only one
		Next
	End Method
	
	Method equals:Int(track:TTrack)
		Local isEqual:Int = True
		
		isEqual :& album = track.album
		isEqual :& name = track.name
		isEqual :& length = track.length
		
		Return isEqual
	End Method
	
	Method draw(x:Int, y:Int)
	
		DrawText "Album    : " + album, x + 10, y + 14
		DrawText "Track    : " + name, x + 10, y + 28
		DrawText "Duration : " + secondsToMinSec(length), x + 10, y + 42
	
		If art Then
			Local scale:Float = 256.0 / art.width ' scale to 256 wide
			SetScale scale, scale
			
			DrawImage art, x + 10, y + 70
			
			SetScale 1, 1
		End If
	End Method
	
End Type


Type DisplayStatus

	Field player:iTunes
	
	Field state:Int
	Field currentTrack:TTrack
	
	Field tick:Int
	
	Method Create:DisplayStatus(player:iTunes)
		Self.player = player
		Return Self
	End Method
	
	Method update()
	
		tick:+ 1
		
		' update the state (every 30 ticks)
		If tick = 30 Then
			tick = 0
			
			state = player.state()
			Local track:TTrack = player.currentTrack()
			
			If Not currentTrack Or Not track.equals(currentTrack) Then
				currentTrack = track
				currentTrack.Load()
			End If
			
		End If
	
	End Method
	
	Method stateString:String()
		Select state
			Case STATE_STOPPED
				Return "Stopped"
			Case STATE_PLAYING
				Return "Playing"
			Case STATE_PAUSED
				Return "Paused"
			Case STATE_FASTFORWARDING
				Return "Fast Forwarding"
			Case STATE_REWINDING
				Return "Rewinding"
		End Select
	End Method
	
	Method draw()
		DrawText "iTunes is " + stateString() + ".", 10, 10
		
		drawBar()
		
		If currentTrack Then
			currentTrack.draw(50, 70)
		End If
	End Method
	
	Method drawBar()
	
		DrawRect 40, 60, 400, 5
		
		If currentTrack Then

			Local totaltime:Int = currentTrack.length
			Local currentPos:Int = player.position()		
			Local timeScale:Float = 400.0 / currentTrack.length
			
			SetColor 220, 0, 0
		
			DrawRect 40, 60, currentPos * timeScale, 5
		
			SetColor 255, 255, 255
		
			DrawText secondsToMinSec(currentPos), 12 + currentPos * timeScale, 45
		
		End If
	End Method
	
End Type

Function secondsToMinSec:String(seconds:Int)
	Local minutes:Int = seconds / 60
	Local secs:Int = seconds Mod 60
	
	Local m:String = "   " + minutes
	m = m[m.length - 3..]
	
	Local s:String = "00" + secs
	s = s[s.length - 2..]
	
	Return m + ":" + s
End Function





