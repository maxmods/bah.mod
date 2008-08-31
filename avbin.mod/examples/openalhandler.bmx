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

Import BaH.Avbin
Import BRL.OpenALAudio
Import BRL.LinkedList

SetAudioDriver("OpenAL")

Type TOpenALHandler

	' the list of openAL buffer handles
	Field buffers:Int[]
	' the buffer data
	Field bufferData:Byte Ptr[]
	' buffer offsets - for the caching
	Field bufferOffset:Int[]
	
	Global bufferSize:Int = TAVbin.GetAudioBufferSize()
	Const bufferCount:Int = 16

	' the openAL source handle
	Field audioSource:Int
	
	' this is the buffer index we are curently caching
	Field nextBuffer:Int
	
	' some of the audio settings
	Field rate:Int
	Field bits:Int
	Field channels:Int
	Field format:Int

	' true if the audio has started playing	
	Field playing:Int

	Function Create:TOpenALHandler(_rate:Int, _bits:Int, _channels:Int)
		Local this:TOpenALHandler = New TOpenALHandler
		this.Init(_rate, _bits, _channels)
		Return this
	End Function

	Method Init(_rate:Int, _bits:Int, _channels:Int)
		rate = _rate
		bits = _bits
		channels = _channels
		
		' get the format from the bits and channels
		Select bits
			Case 8
				If channels = 1 Then
					format = AL_FORMAT_MONO8
				Else
					format = AL_FORMAT_MONO16
				End If
			Case 16
				If channels = 1 Then
					format = AL_FORMAT_STEREO8
				Else
					format = AL_FORMAT_STEREO16
				End If
		End Select
	
		buffers = New Int[bufferCount]
		bufferOffset = New Int[bufferCount]
		
		bufferData = New Byte Ptr[bufferCount]
		For Local i:Int = 0 Until bufferCount
			bufferData[i] = MemAlloc(bufferSize)
		Next
		
		' set up buffers
		alGenBuffers(bufferCount, buffers)
		
		' create a source
		alGenSources(1, Varptr audioSource)
	End Method
	
	' we pre-cache the first block of audio data, then after that
	' we should be able to keep the cache at a suitable level - not that I've really thrashed it...
	Method ProcessPacket:Long(realTime:Long, pkt:TAVbinPacket, stream:TAVbinStream)

		Local time:Long
	
		Local packetSize:Int = pkt.GetSize()
		Local packetPtr:Byte Ptr = pkt.GetData()
		Local offset:Int = bufferOffset[nextBuffer]

		While packetSize > 0
	
			Local outSize:Int = bufferSize
			Local used:Int = stream.DecodeAudio(packetPtr, packetSize, bufferData[nextBuffer] + offset, outSize)
			
			' offset stores the total size of the data into the buffer
			offset:+ outSize
		
			If used < 0 Then
				Exit
			End If
			
			If outSize <= 0 Then
				Continue
			End If
		
			packetSize :- used
			packetPtr :+ used
		
		Wend
		
		bufferOffset[nextBuffer] = offset
		
		Local buffer:Int
		If offset > bufferSize / 2 Then
		
			buffer = buffers[nextBuffer]
		
			' add a buffer
			alBufferData(buffers[nextBuffer], format, bufferData[nextBuffer], offset, rate)
			alSourceQueueBuffers(audioSource, 1, Varptr buffer)
	
			bufferOffset[nextBuffer] = 0
	
			nextBuffer:+ 1
			If nextBuffer = bufferCount Then
				nextBuffer = 0
			End If
			
			' if we haven't started yet, start now, since our buffer is full :-)
			If Not playing Then
				playing = True
			End If
			
		End If

		' clear processed buffers
		Local processed:Int
		alGetSourcei(audioSource, AL_BUFFERS_PROCESSED, Varptr processed)
		
		buffer = 0
		For Local i:Int = 0 Until processed
			alSourceUnqueueBuffers(audioSource, 1, Varptr buffer)
		Next
	
		offset = 0
		alGetSourcei(audioSource, AL_SOURCE_STATE, Varptr offset)

		If playing And offset <> AL_PLAYING Then
			alSourcePlay audioSource
		End If

		Return time
	End Method

	Method Delete()
		For Local i:Int = 0 Until bufferCount
			MemFree(bufferData[i])
		Next
	End Method

End Type



