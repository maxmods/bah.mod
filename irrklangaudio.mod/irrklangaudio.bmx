' Copyright (c) 2008-2009 Bruce A Henderson
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
ModuleInfo "Copyright: 2008-2009 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed StartSound not handling TChannel properly."
ModuleInfo "History: Fixed issue where pre-Alloc'd channel could not be modified (volume, rate, etc)."
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
		Local channel:TISound
		If is3D Then
			channel = _driver._engine.Play3DSource(_sound, TIVec3D.Zero(), isLooped, pause, True)
		Else
			channel = _driver._engine.Play2DSource(_sound, isLooped, pause, True)
		End If
		
		If Not alloced_channel Then
			Return New TTISoundChannel.Create(channel)
		Else
			TTISoundChannel(alloced_channel).Set(channel)
			Return alloced_channel
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
	
	' since we can "alloc" a channel, it won't yet have a _channel object.. which will be added later.
	' so, any settings changed need to be cached and applied when initialized
	Field prePaused:Int
	Field preVolume:Float
	Field prePan:Float
	Field preRate:Float
	Field preflags:Int
	
	Field APPLY_PAUSED:Int = 1
	Field APPLY_VOLUME:Int = 2
	Field APPLY_PAN:Int = 4
	Field APPLY_RATE:Int = 8
	
	Method Create:TTISoundChannel(channel:TISound)
		_channel = channel
		Return Self
	End Method

	' sets _channel with channel - usually applied if this object was created with AllocChannel()
	Method Set(channel:TISound)
		_channel = channel
		
		If preflags & APPLY_PAUSED Then
			SetPaused(prePaused)
		End If

		If preflags & APPLY_VOLUME Then
			SetVolume(preVolume)
		End If

		If preflags & APPLY_PAN Then
			SetPan(prePan)
		End If

		If preflags & APPLY_RATE Then
			SetRate(preRate)
		End If
	End Method

	Method Stop()
		_channel.Stop()
	End Method

	Method SetPaused( paused:Int )
		If _channel Then
			_channel.SetPaused(paused)
		Else
			preflags:| APPLY_PAUSED
			prePaused = paused
		End If
	End Method

	Method SetVolume( volume:Float )
		If _channel Then
			_channel.SetVolume(volume)
		Else
			preflags:| APPLY_VOLUME
			preVolume = volume
		End If
	End Method

	Method SetPan( pan:Float ) 
		If _channel Then
			_channel.SetPan(pan)
		Else
			preflags:| APPLY_PAN
			prePan = pan
		End If
	End Method

	Method SetDepth( depth:Float )
		' TODO ...  3D depth?
	End Method

	Method SetRate( rate:Float )
		If _channel Then
			_channel.SetPlaybackSpeed(rate)
		Else
			preflags:| APPLY_RATE
			preRate = rate
		End If
	End Method

	Method Playing:Int()
		Return Not _channel.IsFinished()
	End Method

End Type
