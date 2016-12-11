' Copyright (c) 2016 Bruce A Henderson
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
'    1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would be
'    appreciated but is not required.
'
'    2. Altered source versions must be plainly marked as such, and must not be
'    misrepresented as being the original software.
'
'    3. This notice may not be removed or altered from any source
'    distribution.
'
SuperStrict

Rem
bbdoc: Soloud Audio Driver
about: Provides Soloud driver for use with the BRL.Audio module.
End Rem
Module bah.soloudaudio

ModuleInfo "Version: 1.00"
ModuleInfo "License: zlib/libpng"
ModuleInfo "Copyright: 2016 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

Import bah.soloud
Import BRL.Audio
Import brl.filesystem

Private

Global _driver:TSoloudAudioDriver

Public


Const SOLOUD_SOUND_WAV:Int =       $00001000
Const SOLOUD_SOUND_WAVSTREAM:Int = $00002000
Const SOLOUD_SOUND_SFXR:Int =      $00004000
Const SOLOUD_SOUND_MODPLUG:Int =   $00008000
Const SOLOUD_SOUND_MONOTONE:Int =  $00010000
Const SOLOUD_SOUND_TEDSID:Int =    $00020000


New TSoloudAudioDriver

Type TSoloudAudioDriver Extends TAudioDriver

	Field _soloud:TSoloud

	Method Name$()
		Return "SoLoud"
	End Method
	
	Method Startup:Int()
		_soloud = New TSoloud
		_soloud.init()

		_driver = Self
		
		Return True
	End Method
	
	Method Shutdown()
		Local soloud:TSoloud = _soloud
		_soloud = Null
		
		soloud.deinit()
	End Method

	Method CreateSound:TSound( sample:TAudioSample, loopFlag:Int )
		'Return New TSound
	End Method

	Method LoadSound:TSound( url:Object, flags:Int = 0)
		Return TSoloudSound.Load(url, flags)
	End Method

	Method AllocChannel:TChannel()
		Return New TSoloudChannel
	End Method

End Type

Type TSoloudSound Extends TSound

	Field _sound:TSLAudioSource
	Field isLooped:Int

	Method Play:TChannel( allocedChannel:TChannel=Null )
		Return StartSound(allocedChannel, False)
	End Method
	
	Method Cue:TChannel( allocedChannel:TChannel=Null )
		Return StartSound(allocedChannel, True)
	End Method
	
	Method StartSound:TChannel(allocedChannel:TChannel=Null, pause:Int = False)

		Local voiceHandle:Int = _driver._soloud.play(_sound, -1, 0, pause)

		If Not allocedChannel Then
			Return New TSoloudChannel.Create(_driver._soloud, voiceHandle)
		Else
			TSoloudChannel(allocedChannel).Set(_driver._soloud, voiceHandle)
			Return allocedChannel
		End If
	End Method
	
	Function Load:TSound( url:Object, loopFlag:Int )
	
		Local sound:TSLLoadableAudioSource
	
		If loopFlag & SOLOUD_SOUND_WAV Then
			sound = New TSLWav
		Else If loopFlag & SOLOUD_SOUND_WAVSTREAM Then
			sound = New TSLWavStream
		Else If loopFlag & SOLOUD_SOUND_SFXR Then
			sound = New TSLSfxr
		Else If loopFlag & SOLOUD_SOUND_MODPLUG Then
			sound = New TSLModplug
		Else If loopFlag & SOLOUD_SOUND_MONOTONE Then
			sound = New TSLMonotone
		Else If loopFlag & SOLOUD_SOUND_TEDSID Then
			sound = New TSLTedSid
		End If
		
		Local this:TSoloudSound = New TSoloudSound
		If sound Then
			this._sound = sound
		End If

		If loopFlag & SOUND_LOOP Then
			this.isLooped = True
		End If
	
		If String(url) Then
			Local s:String = String(url)
			
			If Not sound Then
				Local suf:String = ExtractExt(s).toLower()
				
				If suf = "wav" Or suf = "ogg" Then
					sound = New TSLWav ' default to memory loaded. For streaming, pass the correct flag
				Else If suf = "mon" Then
					sound = New TSLMonotone
				Else ' list of modplug exts?
					
				End If
				
				If sound Then
					this._sound = sound
				End If
			End If
			
			If sound Then
				Local i:Int = s.Find( "::",0 )
				' a "normal" url?
				If i = -1 Then
					Local res:Int = sound.Load(s)
					If res Then
						' TODO : show error?
						Return Null
					End If
				Else
					Local proto:String = s[..i].ToLower()
					Local path:String = s[i+2..]
					
					If proto = "incbin" Then
						Local buf:Byte Ptr = IncbinPtr( path )
						If Not buf Then
							Return Null
						End If
						
						Local res:Int = sound.loadMem(buf, IncbinLen(path), False, False)
						
						If res Then
							' todo : show error?
							Return Null
						End If
					End If
				End If
			End If
		
		Else If TStream(url) Then
			' TODO : stream api
		End If
		
		Return this

	End Function
	
End Type

Type TSoloudChannel Extends TChannel

	Field _soloud:TSoloud
	Field _voiceHandle:Int

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
	
	Method Create:TSoloudChannel(soloud:TSoloud, voiceHandle:Int)
		_soloud = soloud
		_voiceHandle = voiceHandle
		Return Self
	End Method

	' - usually applied if this object was created with AllocChannel()
	Method Set(soloud:TSoloud, voiceHandle:Int)
		_soloud = soloud
		_voiceHandle = voiceHandle
		
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
		If _voiceHandle Then
			_soloud.stop(_voiceHandle)
		End If
	End Method

	Method SetPaused( paused:Int )
		If _voiceHandle Then
			_soloud.setPause(_voiceHandle, paused)
		Else
			preflags:| APPLY_PAUSED
			prePaused = paused
		End If
	End Method

	Method SetVolume( volume:Float )
		If _voiceHandle Then
			_soloud.setVolume(_voiceHandle, volume)
		Else
			preflags:| APPLY_VOLUME
			preVolume = volume
		End If
	End Method

	Method SetPan( pan:Float )
		If _voiceHandle Then
			_soloud.setPan(_voiceHandle, pan)
		Else
			preflags:| APPLY_PAN
			prePan = pan
		End If
	End Method

	Method SetDepth( depth:Float )
		' TODO ?
	End Method

	Method SetRate( rate:Float )
		If _voiceHandle Then
			_soloud.setSamplerate(_voiceHandle, rate)
		Else
			preflags:| APPLY_RATE
			preRate = rate
		End If
	End Method

	Method Playing:Int()
		If _voiceHandle Then
			Return _soloud.isValidVoiceHandle(_voiceHandle) And Not _soloud.getPause(_voiceHandle)
		Else
			Return False
		End If
	End Method

End Type
