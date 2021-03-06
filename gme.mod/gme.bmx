' Copyright (c) 2008-2013 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Rem
bbdoc: Game Music Emulator
about: Plays different classic game audio files.
End Rem
Module BaH.GME

ModuleInfo "Version: 1.01"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: GME - LGPL"
ModuleInfo "Copyright: Wrapper - 2008-2013 Bruce A Henderson"
ModuleInfo "Copyright: GME - Shay Green, http://www.fly.net/~~ant/libs/audio.html"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to GME 0.6.0"
ModuleInfo "History: Added GetTrackInfo() method for track information."
ModuleInfo "History: Removed creation of extra Emu object for incbin'd tracks."
ModuleInfo "History: 1.00 Initial Release"

?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos"
?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?linux
ModuleInfo "LD_OPTS: -L%PWD%/lib/linux"
?

Import "common.bmx"

Private

Global _nullDriver:TGMEDriver = New TGMEDriver

Global _driver:TGMEDriver ,_drivers:TGMEDriver

Public

Rem
bbdoc: Returns the TGMEPlayer instance.
End Rem
Function GetGMEPlayer:TGMEPlayer()
	Return _driver.GetPlayer()
End Function

Rem
bbdoc: The GME engine.
about: This provides low-level access to GME. Usually not required, since you have access to most control via the player.
End Rem
Type TMusicEmu

	Field emuPtr:Byte Ptr

	Rem
	bbdoc: Creates an instance of TMusicEmu for the provided audio file, to playback at the given @sampleRate.
	End Rem
	Function Create:TMusicEmu(file:String, sampleRate:Int)
		Local this:TMusicEmu
		
		Local err:String
		Local i:Int = file.Find( "::",0 )
		' a "normal" filename?
		If i = -1 Then
			this = New TMusicEmu
			this.emuPtr = _driver.GetEmu()

			err = bmx_gme_open_file(this.emuPtr, file, sampleRate)
		Else
			Local proto:String = file[..i].ToLower()
			Local path:String = file[i+2..]
			
			If proto = "incbin" Then
				Local buf:Byte Ptr = IncbinPtr( path )
				If Not buf Then
					Return Null
				End If
				Local size:Int = IncbinLen( path )
				
				Return CreateForData(buf, size, sampleRate)
			Else
				err = "Unsupported protocol : " + proto
			End If
		End If
		
		If err Then
			DebugLog err
			Return Null
		End If
		
		Return this
	End Function
	
	Rem
	bbdoc: Creates an instance of TMusicEmu for the audio file in memory, to playback at the given @sampleRate.
	End Rem
	Function CreateForData:TMusicEmu(data:Byte Ptr, size:Int, sampleRate:Int)
		Local this:TMusicEmu = New TMusicEmu
		this.emuPtr = _driver.GetEmu()
		
		Local err:String = bmx_gme_open_data(this.emuPtr, data, size, sampleRate)
		
		If err Then
			DebugLog err
			Return Null
		End If
		
		Return this
	End Function
	
	Rem
	bbdoc: Number of tracks available
	End Rem
	Method TrackCount:Int()
		Return bmx_gme_track_count(emuPtr)
	End Method
	
	Rem
	bbdoc: Start a track, where 0 is the first track
	End Rem
	Method StartTrack:String(index:Int)
		Return bmx_gme_start_track(emuPtr, index)
	End Method
	
	Rem
	bbdoc: Generate 'count' 16-bit signed samples into 'out'.
	about: Output is in stereo.
	End Rem
	Method Play:String(count:Int, out:Byte Ptr)
		Return bmx_gme_play(emuPtr, count, out)
	End Method
	
	Rem
	bbdoc: Returns True if a track has reached its end.
	End Rem
	Method TrackEnded:Int()
		Return bmx_gme_track_ended(emuPtr)
	End Method
	
	Rem
	bbdoc: Number of milliseconds (1000 = one second) played since beginning of track.
	End Rem
	Method Tell:Int()
		Return bmx_gme_tell(emuPtr)
	End Method
	
	Rem
	bbdoc: Seek to new time in track.
	about: Seeking backwards or far forward can take a while.
	End Rem
	Method Seek:String(msec:Int)
		Return bmx_gme_seek(emuPtr, msec)
	End Method
	
	Rem
	bbdoc: Set time to start fading track out.
	about: Once fade ends track_ended() returns true. Fade time can be changed while track is playing.
	End Rem
	Method SetFade(startMsec:Int)
		bmx_gme_set_fade(emuPtr, startMsec)
	End Method

	Rem
	bbdoc: Adjusts stereo echo depth, where 0.0 = off and 1.0 = maximum.
	about: Has no effect for GYM, SPC, and Sega Genesis VGM music
	End Rem
	Method SetStereoDepth(depth:Double)
		bmx_gme_set_stereo_depth(emuPtr, depth)
	End Method
	
	Rem
	bbdoc: Disables automatic end-of-track detection and skipping of silence at beginning if ignore is true
	End Rem
	Method IgnoreSilence(ignore:Int)
		bmx_gme_ignore_silence(emuPtr, ignore)
	End Method
	
	Rem
	bbdoc: Adjusts song tempo, where 1.0 = normal, 0.5 = half speed, 2.0 = double speed.
	about: Track length as returned by track_info() assumes a tempo of 1.0.
	End Rem
	Method SetTempo(tempo:Double)
		bmx_gme_set_tempo(emuPtr, tempo)
	End Method
	
	Rem
	bbdoc: Mute/unmute voice @index, where voice 0 is first voice.
	End Rem
	Method MuteVoice(index:Int, mute:Int)
		bmx_gme_mute_voice(emuPtr, index, mute)
	End Method
	
	Rem
	bbdoc: Sets muting state of all voices at once.
	about: Uses a bit mask, where -1 mutes all voices, 0 unmutes them all, $1 mutes just the first voice, etc.
	End Rem
	Method MuteVoices(mutingMask:Int)
		bmx_gme_mute_voices(emuPtr, mutingMask)
	End Method
	
	Rem
	bbdoc: Enables/disables most accurate sound emulation options
	End Rem
	Method EnableAccuracy(enabled:Int)
		bmx_gme_enable_accuracy(emuPtr, enabled)
	End Method

	Rem
	bbdoc: Gets information for a particular track (length, name, author, etc.)
	End Rem
	Method GetTrackInfo:TGMETrackInfo(track:Int)
		Local info:TGMETrackInfo = New TGMETrackInfo

		Local err:String = bmx_gme_get_trackinfo(emuPtr, Varptr info.infoPtr, track)
		
		If err Then
			DebugLog err
			Return Null
		End If
		
		Return info
	End Method

	Method Delete()
		If emuPtr Then
			bmx_gme_emu_delete(emuPtr)
			emuPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: The GME Player.
about: Loads and plays GME supported audio.
End Rem
Type TGMEPlayer

	Field emu:TMusicEmu

	Rem
	bbdoc: Loads the specific audio file, to play back at the given @sampleRate.
	End Rem
	Method LoadFile(path:String, sampleRate:Int)
		emu = TMusicEmu.Create(path, sampleRate)
	End Method
	
	Rem
	bbdoc: Loads the specific audio data from memory, to play back at the given @sampleRate.
	End Rem
	Method LoadData(data:Byte Ptr, size:Int, sampleRate:Int)
		emu = TMusicEmu.CreateForData(data, size, sampleRate)
	End Method
	
	Rem
	bbdoc: Returns the number of tracks in the file.
	End Rem
	Method TrackCount:Int()
		Return emu.TrackCount()
	End Method
	
	Rem
	bbdoc: Plays @track.
	End Rem
	Method Play(track:Int)
		' Override !
	End Method
	
	Rem
	bbdoc: Sets the pause
	End Rem
	Method Pause(paused:Int = True)
		' Override !
	End Method

	Rem
	bbdoc: Resumes the current track.
	End Rem
	Method Resume()
		' Override !
	End Method

	Rem
	bbdoc: Returns True if a track has reached its end.
	End Rem
	Method TrackEnded:Int()
		Return emu.TrackEnded()
	End Method
	
	Rem
	bbdoc: Number of milliseconds (1000 = one second) played since beginning of track.
	End Rem
	Method Tell:Int()
		Return emu.Tell()
	End Method
	
	Rem
	bbdoc: Seek to new time in track.
	about: Seeking backwards or far forward can take a while.
	End Rem
	Method Seek(msec:Int)
		emu.Seek(msec)
	End Method

	Rem
	bbdoc: Returns True if the player state is okay.
	End Rem
	Method IsOk:Int()
		If emu Then
			Return True
		End If
		Return False
	End Method

	Rem
	bbdoc: Set time to start fading track out.
	about: Once fade ends TrackEnded() returns True.
	End Rem
	Method SetFade(startMsec:Int)
		emu.SetFade(startMsec)
	End Method
	
	Rem
	bbdoc: Adjust stereo echo depth.
	about: 0.0 = off and 1.0 = maximum. Has no effect for GYM, SPC, and Sega Genesis VGM music
	End Rem
	Method SetStereoDepth(depth:Double)
		' Override !
	End Method
	
	Rem
	bbdoc: Disable automatic end-of-track detection and skipping of silence at beginning if ignore is true.
	End Rem
	Method IgnoreSilence(ignore:Int)
		' Override !
	End Method
	
	Rem
	bbdoc: Adjust song tempo.
	about: 1.0 = normal, 0.5 = half speed, 2.0 = double speed.
	Track length as returned by track_info() assumes a tempo of 1.0.
	End Rem
	Method SetTempo(tempo:Double)
		' Override !
	End Method
	
	Rem
	bbdoc: Number of voices used by currently loaded file.
	End Rem
	Method VoiceCount:Int()
	End Method
	
	Rem
	bbdoc: Mute/unmute voice i, where voice 0 is first voice.
	End Rem
	Method MuteVoice(index:Int, mute:Int)
		' Override !
	End Method
	
	Rem
	bbdoc: Set muting state of all voices at once using a bit mask.
	about: -1 mutes all voices, 0 unmutes them all, $01 mutes just the first voice, etc.
	End Rem
	Method MuteVoices(mutingMask:Int)
		' Override !
	End Method

	Rem
	bbdoc: Sets the volume.
	about: 0 off, 1 full.
	End Rem
	Method SetVolume(volume:Float)
		' Override !
	End Method
	
	Rem
	bbdoc: Gets the current volume.
	about: 0 off, 1 full.
	End Rem
	Method GetVolume:Float()
		' Override !
	End Method
	
	Rem
	bbdoc: Update the current track.
	about: This is required for GME drivers that don't run their own update loop.
	<p>
	Call this often.
	</p>
	End Rem
	Method Update()
	End Method
	
	Rem
	bbdoc: Gets information for a particular track (length, name, author, etc.)
	End Rem
	Method GetTrackInfo:TGMETrackInfo(track:Int)
		Return emu.GetTrackInfo(track)
	End Method

	Rem
	bbdoc: Enables/disables most accurate sound emulation options
	End Rem
	Method EnableAccuracy(enabled:Int)
		emu.EnableAccuracy(enabled)
	End Method

End Type

Rem
bbdoc: Track information
End Rem
Type TGMETrackInfo

	Field infoPtr:Byte Ptr

	Rem
	bbdoc: Game system.
	End Rem
	Method System:String()
		Return bmx_gme_trackinfo_system(infoPtr)
	End Method
	
	Rem
	bbdoc: The game the track belongs to.
	End Rem
	Method Game:String()
		Return bmx_gme_trackinfo_game(infoPtr)
	End Method
	
	Rem
	bbdoc: Name of the track.
	End Rem
	Method Song:String()
		Return bmx_gme_trackinfo_song(infoPtr)
	End Method
	
	Rem
	bbdoc: Author of the track
	End Rem
	Method Author:String()
		Return bmx_gme_trackinfo_author(infoPtr)
	End Method
	
	Rem
	bbdoc: Track copyright.
	End Rem
	Method Copyright:String()
		Return bmx_gme_trackinfo_copyright(infoPtr)
	End Method
	
	Rem
	bbdoc: Comment
	End Rem
	Method Comment:String()
		Return bmx_gme_trackinfo_comment(infoPtr)
	End Method
	
	Rem
	bbdoc: The name of the dumper who extracted the track data from its source.
	End Rem
	Method Dumper:String()
		Return bmx_gme_trackinfo_dumper(infoPtr)
	End Method
	
	Method Delete()
		If infoPtr Then
			bmx_gme_trackinfo_free(infoPtr)
			infoPtr = Null
		End If
	End Method

End Type

Type TGMEDriver

	Field _succ:TGMEDriver

	Method New()
		_succ=_drivers
		_drivers=Self
	End Method
	
	Method Name:String()
		Return "Null"
	End Method
	
	Method Startup:Int()
		Return True
	End Method
	
	Method GetEmu:Byte Ptr()
		Return bmx_gme_emu_new()
	End Method

	Method GetPlayer:TGMEPlayer()
		Return New TGMEPlayer
	End Method
	
End Type

Function SetGMEDriver:Int( name$ )
	name=name.ToLower()
	_driver=_nullDriver
	Local t:TGMEDriver=_drivers
	While t
		If t.Name().ToLower()=name
			If t.Startup()
				_driver=t
				Return True
			EndIf
			Return False
		EndIf
		t=t._succ
	Wend
End Function
