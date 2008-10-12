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
bbdoc: BASS driver for BaH.GME
End Rem
Module BaH.GMEBass

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008 Bruce A Henderson"

?Not linux

Import "common.bmx"

Private

Global _driver:TGMEBASSDriver

Public

New TGMEBASSDriver
SetGMEDriver("BASS")

Type TGMEBASSDriver Extends TGMEDriver

	Method New()
		
	End Method
	
	Method Name$()
		Return "BASS"
	End Method
	
	Method Startup:Int()

		_driver = Self
		
		Return True
	End Method

	Method GetEmu:Byte Ptr()
		Return bmx_gmebass_emu_new()
	End Method

	Method GetPlayer:TGMEPlayer()
		Return New TBASSGMEPlayer
	End Method

End Type


Type TBASSGMEPlayer Extends TGMEPlayer

	Field channel:TBassChannel
	Field playing:Int = False

	Method LoadFile(path:String, sampleRate:Int)
		Super.LoadFile(path, sampleRate)
		If Super.IsOk() Then
			channel = TBassChannel._create(bmx_gmebass_channel_create(sampleRate, 2, 0, emu.emuPtr))
		End If
	End Method
	
	Method LoadData(data:Byte Ptr, size:Int, sampleRate:Int)
		Super.LoadData(data, size, sampleRate)
		If Super.IsOk() Then
			channel = TBassChannel._create(bmx_gmebass_channel_create(sampleRate, 2, 0, emu.emuPtr))
		End If
	End Method
	
	Method Play(track:Int)
		channel.Pause()
		emu.StartTrack(track)
		channel.Play()
		playing = True
	End Method
	
	Method Pause(paused:Int)
		If paused Then
			channel.Pause()
		Else
			channel.Play()
		End If
		playing = Not playing
	End Method

	Method Resume()
		channel.Play()
		playing = True
	End Method

	Method SetStereoDepth(depth:Double)
		channel.Pause()
		emu.SetStereoDepth(depth)
		If playing Then
			channel.Play()
		End If
	End Method
	
	Method IgnoreSilence(ignore:Int)
		channel.Pause()
		emu.IgnoreSilence(ignore)
		If playing Then
			channel.Play()
		End If
	End Method
	
	Method SetTempo(tempo:Double)
		channel.Pause()
		emu.SetTempo(tempo)
		If playing Then
			channel.Play()
		End If
	End Method
	
	Method MuteVoice(index:Int, mute:Int)
		channel.Pause()
		emu.MuteVoice(index, mute)
		If playing Then
			channel.Play()
		End If
	End Method
	
	Method MuteVoices(mutingMask:Int)
		channel.Pause()
		emu.MuteVoices(mutingMask)
		If playing Then
			channel.Play()
		End If
	End Method

	Method SetVolume(volume:Float)
		channel.SetAttribute(BASS_ATTRIB_VOL, volume)
	End Method

	Method GetVolume:Float()
		Local value:Float
		channel.GetAttribute(BASS_ATTRIB_VOL, value)
		Return value
	End Method

	Method IsOk:Int()
		If Super.IsOk() Then
			If channel Then
				Return True
			End If
		End If
		Return False
	End Method

End Type

?
