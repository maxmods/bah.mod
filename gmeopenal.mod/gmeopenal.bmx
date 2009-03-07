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
bbdoc: OpenAL driver for BaH.GME
End Rem
Module BaH.GMEOpenAL

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2008-2009 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"


Import BaH.GME
Import BRL.OpenALAudio

Private

Global _driver:TGMEOpenALDriver

Public

New TGMEOpenALDriver
SetGMEDriver("OpenAL")

Type TGMEOpenALDriver Extends TGMEDriver

	Method New()
		
	End Method
	
	Method Name$()
		Return "OpenAL"
	End Method
	
	Method Startup:Int()
	
		SetAudioDriver("OpenAL")
		
		_driver = Self
		
		Return True
	End Method

	Method GetEmu:Byte Ptr()
		Return Super.GetEmu()
		'Return bmx_gmeopen_emu_new()
	End Method

	Method GetPlayer:TGMEPlayer()
		Return New TOpenALGMEPlayer
	End Method

End Type


Type TOpenALGMEPlayer Extends TGMEPlayer

	'Field channel:TBassChannel
	Field playing:Int = False
	
	' the list of openAL buffer handles
	Field buffers:Int[]
	' current buffer state - used (1) / unused (0)
	Field bufferState:Int[]
	Field usedCount:Int = 0
	Field bufferOffset:Int
	
	Field procBuffer:Int[]
	
	' the buffer data
	Field bufferData:Byte Ptr[]

	Const bufferSize:Int = 44100 * 4
	Const bufferCount:Int = 2

	' the openAL source handle
	Field audioSource:Int
	
	Field rate:Int

	Method New()
		buffers = New Int[bufferCount]
		bufferState = New Int[bufferCount]
		procBuffer = New Int[bufferCount]
		
		bufferData = New Byte Ptr[bufferCount]
		For Local i:Int = 0 Until bufferCount
			bufferData[i] = MemAlloc(bufferSize)
		Next

		' set up buffers
		alGenBuffers(bufferCount, buffers)
		
		' create a source
		alGenSources(1, Varptr audioSource)
	End Method
	
	Method Delete()
		For Local i:Int = 0 Until bufferCount
			MemFree(bufferData[i])
		Next
	End Method

	Method LoadFile(path:String, sampleRate:Int)
		Super.LoadFile(path, sampleRate)
		rate = sampleRate
		If Super.IsOk() Then
			'channel = TBassChannel._create(bmx_gmebass_channel_create(sampleRate, 2, 0, emu.emuPtr))
		End If
	End Method
	
	Method LoadData(data:Byte Ptr, size:Int, sampleRate:Int)
		Super.LoadData(data, size, sampleRate)
		rate = sampleRate
		If Super.IsOk() Then
			'channel = TBassChannel._create(bmx_gmebass_channel_create(sampleRate, 2, 0, emu.emuPtr))
		End If
	End Method
	
	Method Update()
		' clear used buffers, fill new

		' clear processed buffers
		Local processed:Int
		alGetSourcei(audioSource, AL_BUFFERS_PROCESSED, Varptr processed)

		If processed Then

			alSourceUnqueueBuffers(audioSource, processed, procBuffer)

			For Local n:Int = 0 Until processed
				For Local i:Int = 0 Until bufferCount
					If procBuffer[n] = buffers[i] Then
						bufferState[i] = 0
						Exit
					End If
				Next
			Next
		End If
				
		usedCount:- processed
		Local count:Int = bufferCount - usedCount
		
		' fill new buffers
		If count And playing Then
		
			If Not bufferState[bufferOffset] Then
				emu.Play(bufferSize / 2, bufferData[bufferOffset])

				Local buffer:Int = buffers[bufferOffset]
				
				alBufferData(buffers[bufferOffset], AL_FORMAT_STEREO16, bufferData[bufferOffset], bufferSize, rate)
				alSourceQueueBuffers(audioSource, 1, Varptr buffer)

				count:- 1
				usedCount:+ 1
				bufferState[bufferOffset] = 1
				
				bufferOffset:+ 1
				If bufferOffset >= bufferCount Then
					bufferOffset = 0
				End If
			
			End If
		End If

	End Method
	
	Method Play(track:Int)
		' stop source and flush buffers
		alSourceStop audioSource
	
		emu.StartTrack(track)

		playing = True
		Update()
		Resume()
	End Method
	
	Method Pause(paused:Int)
		If paused Then
			alSourcePause audioSource
		Else
			alSourcePlay audioSource
		End If
		playing = Not playing
	End Method

	Method Resume()
		alSourcePlay audioSource
		playing = True
	End Method

	Method SetStereoDepth(depth:Double)
		emu.SetStereoDepth(depth)
	End Method
	
	Method IgnoreSilence(ignore:Int)
		emu.IgnoreSilence(ignore)
	End Method
	
	Method SetTempo(tempo:Double)
		emu.SetTempo(tempo)
	End Method
	
	Method MuteVoice(index:Int, mute:Int)
		emu.MuteVoice(index, mute)
	End Method
	
	Method MuteVoices(mutingMask:Int)
		emu.MuteVoices(mutingMask)
	End Method

	Method SetVolume(volume:Float)
		alSourcef audioSource, AL_GAIN, volume
	End Method

	Method GetVolume:Float()
		Local value:Float
		alGetSourcef audioSource, AL_GAIN, Varptr value
		Return value
	End Method

	Method IsOk:Int()
		If Super.IsOk() Then
			'If channel Then
				Return True
			'End If
		End If
		Return False
	End Method

End Type
