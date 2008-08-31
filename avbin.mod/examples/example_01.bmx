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

Framework BaH.AVbin
Import BRL.StandardIO
Import BRL.GLMax2D

Import "openalhandler.bmx"

TAVbin.Init()

'TAVbin.SetLogLevel(AVBIN_LOG_DEBUG)
'TAVbin.SetLogCallback(logcallback)

Print "AVbin Version = " + TAVbin.GetVersion()
Print "Avbin Audio buffer size = " + TAVbin.GetAudioBufferSize()

' the file to play...
Local filename:String = "/Volumes/Misc Data/im/stuff/big_buck_bunny_480p_stereo.avi"

Local file:TAVbinFile = TAVbinFile.OpenFilename(filename)

If file Then

	Local videoStream:TAVbinStream
	Local videoStreamIndex:Int
	Local width:Int
	Local height:Int
	
	Local audioStream:TAVbinStream
	Local audioStreamIndex:Int
	Local rate:Int
	Local bits:Int
	Local channels:Int
	Local audioFormat:Int

	Local fileInfo:TAVbinFileInfo = file.FileInfo()

	Print "file = " + filename
	Print "duration = " + fileInfo.GetDuration()


	Local count:Int = fileInfo.GetStreamCount()
	Print "stream count = " + count
	
	Local streamInfo:TAVbinStreamInfo = TAVbinStreamInfo.Create()
	
	For Local i:Int = 0 Until count
	
		Print "Stream " + i + " ..."
		
		file.StreamInfo(i, streamInfo)
		
		If streamInfo.GetType() = AVBIN_STREAM_TYPE_VIDEO And Not videoStream Then
			width = streamInfo.GetWidth()
			height = streamInfo.GetHeight()
			
			Print "VIDEO dimensions = " + width + ", " + height
		
			Local stream:TAVbinStream = file.OpenStream(i)
			If Not stream Then
				' some debug
				Print "** Null Video Stream!"
			End If
		
			videoStream = stream
			videoStreamIndex = i
		Else If streamInfo.GetType() = AVBIN_STREAM_TYPE_AUDIO And Not audioStream Then

			rate = streamInfo.GetAudioRate()
			bits = streamInfo.GetAudioBits()
			channels = streamInfo.GetAudioChannels()
			
			' some debug
			Print "AUDIO rate/bits/ch : " + rate + ", " + bits + ", " + channels
			
			If bits = 8 Then
				If channels = 1 Then
					audioFormat = SF_MONO8
				Else
					audioFormat = SF_STEREO8
				End If
			Else If bits = 16 Then
				If channels = 1 Then
					audioFormat = SF_MONO16LE
				Else
					audioFormat = SF_STEREO16LE
				End If
			End If
		
			Local stream:TAVbinStream = file.OpenStream(i)
			If Not stream Then
			' some debug
				Print "** Null Audio Stream!"
			End If
			audioStream = stream
			audioStreamIndex = i
		
		Else
			
			' some debug
			Select streamInfo.GetType()
				Case AVBIN_STREAM_TYPE_VIDEO
					Print "other video"
				Case AVBIN_STREAM_TYPE_AUDIO
					Print "other audio"
				Default
					Print "other unknown"
			End Select
			
		End If
	
	Next

	If videoStream Then
	


		Graphics 900, 600, 0

	
		Local videoPackets:TList = New TList
		
		Local vidCache:TList = New TList
		Local vidInUse:TList = New TList
		
		Local audio:TOpenALHandler
		
		Local playing:Int
		
		
		For Local i:Int = 0 Until 25
			vidCache.AddLast(TTimedPixmap.Create(width, height)) 
		Next
			
		
		
		If audioStream Then
						
			audio = TOpenALHandler.Create(rate, bits, channels)
		Else
			playing = True
		End If
		
		Local pkt:TAVbinPacket
		
		Local realTime:Int = 0
		Local lastMilli:Long = MilliSecs()



		While Not KeyDown(key_escape)
		
			pkt = Null
			If Not videoPackets.IsEmpty() Then
				pkt = TAVbinPacket(videoPackets.First())
			End If
			If pkt Then

				' ////
				Local tp:TTimedPixmap = TTimedPixmap(vidCache.First())
				tp.timestamp = pkt.GetTimestamp()/1000
				
				vidInUse.AddLast(tp)
				vidCache.RemoveFirst()
				' ////
			
				Local res:Int = videoStream.DecodeVideo(pkt.GetData(), pkt.GetSize(), tp.pixmap.pixels)
			
				videoPackets.RemoveFirst()

			Else
				pkt = TAVbinPacket.Create()

				' keep the image cache quite low.
				If vidInUse.Count() < 15 
					If file.Read(pkt) = AVBIN_RESULT_OK Then

						If pkt.GetStreamIndex() = videoStreamIndex Then

							videoPackets.AddLast(pkt)

						Else If audio And pkt.GetStreamIndex() = audioStreamIndex Then

							audio.ProcessPacket(realTime, pkt, audioStream)
							playing = audio.playing
	
						End If
					End If
			
					Continue
				End If
			End If

			Local tp:TTimedPixmap
			If Not vidInUse.IsEmpty() Then
				tp = TTimedPixmap(vidInUse.First())
			End If

			While tp And realTime >= tp.timestamp

				vidCache.AddLast(tp)
				vidInUse.RemoveFirst()
		
				Cls
	
					DrawText realTime, 10, 20
					
					DrawText videoPackets.Count(), 100, 20
					
	
					
					DrawPixmap2(tp.Pixmap, 10, 50)
			
			
				Flip
			
				If vidInUse.IsEmpty() Then
					Exit
				End If
				tp = TTimedPixmap(vidInUse.First())
				
			Wend

			Local milli:Int = MilliSecs()
			If playing Then
				realTime :+ (milli - lastMilli)
			End If
			lastMilli = milli

		Wend

	End If

End If


End


Type TTimedPixmap

	Field pixmap:TPixmap
	Field timestamp:Long

	Function Create:TTimedPixmap(width:Int, height:Int)
		Local this:TTimedPixmap = New TTimedPixmap
		this.pixmap = CreatePixmap(width, height, PF_RGB888, 3)
		Return this
	End Function

End Type


Function logcallback(_mod:String, level:Int, _msg:String)
	Print "LOG (" + level + ") : ~n.......... : " + _mod + "~n.......... : " + _msg
End Function

Function DrawPixmap2( p:TPixmap,x:Int,y:Int )
	glDisable GL_TEXTURE_2D 'DisableTex

	Local t:TPixmap=YFlipPixmap(p)
	glRasterPos2i 0,0
	glBitmap 0,0,0,0,x,-y-t.height,Null
	glDrawPixels t.width,t.height,GL_RGB,GL_UNSIGNED_BYTE,t.pixels
	
	glEnable GL_TEXTURE_2D 
End Function

