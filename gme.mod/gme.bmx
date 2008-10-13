' Copyright (c) 2008 Bruce A Henderson
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
End Rem
Module BaH.GME

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: GME - LGPL"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"
ModuleInfo "Copyright: GME - Shay Green, http://www.fly.net/~~ant/libs/audio.html"

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

Function GetGMEPlayer:TGMEPlayer()
	Return _driver.GetPlayer()
End Function

Rem
bbdoc: 
End Rem
Type TMusicEmu

	Field emuPtr:Byte Ptr

	Rem
	bbdoc: 
	End Rem
	Function Create:TMusicEmu(file:String, sampleRate:Int)
		Local this:TMusicEmu = New TMusicEmu
		this.emuPtr = _driver.GetEmu()
		
		Local err:String
		Local i:Int = file.Find( "::",0 )
		' a "normal" filename?
		If i = -1 Then
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
	bbdoc: 
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
	bbdoc: 
	End Rem
	Method SetStereoDepth(depth:Double)
		bmx_gme_set_stereo_depth(emuPtr, depth)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method IgnoreSilence(ignore:Int)
		bmx_gme_ignore_silence(emuPtr, ignore)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetTempo(tempo:Double)
		bmx_gme_set_tempo(emuPtr, tempo)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MuteVoice(index:Int, mute:Int)
		bmx_gme_mute_voice(emuPtr, index, mute)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method MuteVoices(mutingMask:Int)
		bmx_gme_mute_voices(emuPtr, mutingMask)
	End Method

	Method Delete()
		If emuPtr Then
			bmx_gme_emu_delete(emuPtr)
			emuPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TGMEPlayer

	Field emu:TMusicEmu

	Rem
	bbdoc: 
	End Rem
	Method LoadFile(path:String, sampleRate:Int)
		emu = TMusicEmu.Create(path, sampleRate)
	End Method
	
	Rem
	bbdoc: 
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
