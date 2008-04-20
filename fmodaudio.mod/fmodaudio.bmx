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
bbdoc: FMOD Audio Driver
about: Provides FMOD driver for use with the BRL.Audio module.
End Rem
Module BaH.FMODAudio

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008 Bruce A Henderson"

Import BaH.FMOD
Import BRL.Audio
Import BRL.Timer

Private

Global _driver:TFMODAudioDriver

Public

New TFMODAudioDriver

Type TFMODAudioDriver Extends TAudioDriver

	Const FMODAUDO_TICK:Int = $10000000

	Field _timer:TTimer
	Field _system:TFMODSystem

	Method Name$()
		Return "FMOD"
	End Method
	
	Method Startup:Int()
		_system = New TFMODSystem.Create()
		_system.Init(32)

		_driver = Self
		
		AddHook EmitEventHook, timerCallback
		Local evt:TEvent = TEvent.Create(FMODAUDO_TICK)
		
		_timer = CreateTimer(2, evt)
		
		Return True
	End Method
	
	Method Shutdown()
		Local sys:TFMODSystem = _system
		_system = Null
		
		sys.Close()
		sys.SystemRelease()
	End Method

	Method CreateSound:TSound( sample:TAudioSample,loop_flag:Int )
		Return New TSound
	End Method

	Method LoadSound:TSound( url:Object, flags:Int = 0)
		Return TFMODSoundSound.Load(url, flags)
	End Method

	Method AllocChannel:TChannel()
		Return New TFMODSoundChannel
	End Method

	Function timerCallback:Object( id:Int, data:Object, context:Object )

		If TEvent(data) And TEvent(data).id = FMODAUDO_TICK Then
			If _driver._system Then
				_driver._system.Update()
			End If
		End If

	End Function
	
End Type

Type TFMODSoundSound Extends TSound

	Field _sound:TFMODSound

	Method Play:TChannel( alloced_channel:TChannel=Null )
		Return StartSound(alloced_channel, False)
	End Method
	
	Method Cue:TChannel( alloced_channel:TChannel=Null )
		Return StartSound(alloced_channel, True)
	End Method
	
	Method StartSound:TChannel(alloced_channel:TChannel=Null, pause:Int = False)
		If Not alloced_channel Then
			Local channel:TFMODChannel = _driver._system.PlaySound(FMOD_CHANNEL_FREE, _sound, pause)
			Return New TFMODSoundChannel.Create(channel)
		Else
			Local chan:TFMODSoundChannel = TFMODSoundChannel(alloced_channel)
			If chan Then
				If Not chan._channel Then
					Local channel:TFMODChannel = _driver._system.PlaySound(FMOD_CHANNEL_FREE, _sound, pause)
					Return chan.Create(channel)
				Else
					Local channel:TFMODChannel = _driver._system.PlaySound(FMOD_CHANNEL_FREE, _sound, pause, chan._channel)
					Return chan.Create(channel)
				End If
			End If
		End If
	End Method
	
	Function Load:TSound( url:Object,loop_flag:Int )

		Local flags:Int
		
		If loop_flag & SOUND_LOOP Then
			flags:| FMOD_LOOP_NORMAL
		End If
		
		If loop_flag & SOUND_HARDWARE
			flags:| FMOD_HARDWARE
		Else
			flags:| FMOD_SOFTWARE
		End If
		
		loop_flag:& ~ 3 ' trim off the above flags
		flags:| loop_flag ' apply any FMOD specific flags

		If String(url) Then
			Local this:TFMODSoundSound = New TFMODSoundSound
			this._sound = _driver._system.CreateSoundURL(String(url), flags)
			Return this
		End If
	End Function
	
	Method Delete()
		_sound.SoundRelease()
	End Method

End Type

Type TFMODSoundChannel Extends TChannel

	Field _channel:TFMODChannel
	Field _freq:Float
	
	Method Create:TFMODSoundChannel(channel:TFMODChannel)
		_channel = channel
		channel.GetFrequency(_freq)
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
		_channel.SetFrequency(_freq * rate)
	End Method

	Method Playing:Int()
		Local p:Int
		_channel.IsPlaying(p)
		Return p
	End Method

End Type
