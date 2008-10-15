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
bbdoc: irrKlang Audio Driver
about: Provides irrKlang driver for use with the BRL.Audio module.
End Rem
Module BaH.irrKlangAudio

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed StartSound not returning a TChannel object."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import BaH.irrKlang
Import BRL.Audio
Import BRL.FileSystem

Private

Global _driver:TirrKlangAudioDriver

Public

New TirrKlangAudioDriver

Type TirrKlangAudioDriver Extends TAudioDriver

	Field _engine:TISoundEngine

	Method Name$()
		Return "irrKlang"
	End Method
	
	Method Startup:Int()
		_engine = CreateIrrKlangDevice()

		_driver = Self
		
		Return True
	End Method
	
	Method Shutdown()
		Local sys:TISoundEngine = _engine
		_engine = Null
		
		sys.Drop()
	End Method

	Method CreateSound:TSound( sample:TAudioSample,loop_flag:Int )
		Return New TSound
	End Method

	Method LoadSound:TSound( url:Object, flags:Int = 0)
		Return TISoundSourceSound.Load(url, flags)
	End Method

	Method AllocChannel:TChannel()
		Return New TTISoundChannel
	End Method

End Type

Type TISoundSourceSound Extends TSound

	Field _sound:TISoundSource
	Field is3D:Int
	Field isLooped:Int

	Method Play:TChannel( alloced_channel:TChannel=Null )
		Return StartSound(alloced_channel, False)
	End Method
	
	Method Cue:TChannel( alloced_channel:TChannel=Null )
		Return StartSound(alloced_channel, True)
	End Method
	
	Method StartSound:TChannel(alloced_channel:TChannel=Null, pause:Int = False)
		If Not alloced_channel Then
			Local channel:TISound
			If is3D Then
				channel = _driver._engine.Play3DSource(_sound, TIVec3D.Zero(), isLooped, pause, True)
			Else
				channel = _driver._engine.Play2DSource(_sound, isLooped, pause, True)
			End If
			Return New TTISoundChannel.Create(channel)
		End If
	End Method
	
	Function Load:TSound( url:Object,loop_flag:Int )

		Local this:TISoundSourceSound = New TISoundSourceSound

		If loop_flag & SOUND_LOOP Then
			this.isLooped = True
		End If
		
		If String(url) Then
			Local s:String = String(url)
			
			Local i:Int = s.Find( "::",0 )
			' a "normal" url?
			If i = -1 Then
				this._sound = _driver._engine.AddSoundSourceFromFile(s)
			Else
				Local proto:String = s[..i].ToLower()
				Local path:String = s[i+2..]
				
				If proto = "incbin" Then
					Local buf:Byte Ptr = IncbinPtr( path )
					If Not buf Then
						Return Null
					End If
					
					' Note: we leave on the file-extension for the name as win32 seems to have a problem
					' playing in-memory sounds if it is not provided.
					this._sound = _driver._engine.AddSoundSourceFromMemory(buf, IncbinLen(path), StripDir(s), False)
				End If
			End If
		
			Return this
		End If
	End Function
	
	Method Delete()
		_sound = Null
	End Method

End Type

Type TTISoundChannel Extends TChannel

	Field _channel:TISound
	
	Method Create:TTISoundChannel(channel:TISound)
		_channel = channel
		Return Self
	End Method

	Method Stop()
		_channel.Stop()
	End Method

	Method SetPaused( paused:Int )
		_channel.SetPaused(paused)
	End Method

	Method SetVolume( volume:Float )
		_channel.SetVolume(volume)
	End Method

	Method SetPan( pan:Float ) 
		_channel.SetPan(pan)
	End Method

	Method SetDepth( depth:Float )
		' TODO ...  3D depth?
	End Method

	Method SetRate( rate:Float )
		_channel.SetPlaybackSpeed(rate)
	End Method

	Method Playing:Int()
		Return Not _channel.IsFinished()
	End Method

End Type
