' Copyright (c) 2009-2011 Bruce A Henderson
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
bbdoc: BASS audio driver for BaH.SID
End Rem
Module BaH.SidBASS

Import BaH.Sid

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TSIDBASS Extends TSID

	Field channel:TBassStream

	Method New()
		'Init()
	End Method
	
	Method Free()
		Stop()
		
		If sidPtr Then
			bmx_sidbass_free(sidPtr)
			sidPtr = Null
		End If
	End Method
	
	Method doInit()
		If Not sidPtr Then
			sidPtr = bmx_sidbass_new()
		End If
	End Method

	Method doLoad()
		If channel Then
			channel.Free()
		End If
		
		channel = TBassStream._create(bmx_sidbass_channel_create(44100, 2, 0, sidPtr))
	End Method
	
	Method Stop()
		If playing Then
			playing = False
			channel.stop()
			bmx_sid_stop(sidPtr)
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Play(track:Int = 0)
		channel.Pause()

		bmx_sid_play(sidPtr, track, True)

		channel.Play()
		playing = True
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Pause()
		channel.Pause()
		playing = False
	End Method

	Method SetVolume(volume:Float)
		channel.SetAttribute(BASS_ATTRIB_VOL, volume)
	End Method

End Type


TSID.sid = New TSIDBASS
