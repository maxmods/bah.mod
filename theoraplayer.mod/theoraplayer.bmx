' Copyright (c) 2011, Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Rem
bbdoc: Theora Video Player
End Rem
Module BaH.TheoraPlayer

ModuleInfo "CC_OPTS: -fexceptions"
?win32
ModuleInfo "CC_OPTS: -DTHEORAVIDEO_STATIC"
?

Import "common.bmx"


Rem
bbdoc: Handles all playback/sync operations.
End Rem
Type TTheoraVideoManager

	Field managerPtr:Byte Ptr
	
	Field audioFactory:TTheoraAudioInterfaceFactory
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TTheoraVideoManager()
		managerPtr = bmx_TheoraVideoManager_new()
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method createVideoClip:TTheoraVideoClip(filename:String, outputMode:Int = TH_RGB, numPrecachedOverride:Int = 0, usePower2Stride:Int = 0)
		Return TTheoraVideoClip._create(bmx_TheoraVideoManager_createVideoClip(managerPtr, filename, outputMode, numPrecachedOverride, usePower2Stride))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method destroyVideoClip(clip:TTheoraVideoClip)
		bmx_TheoraVideoManager_destroyVideoClip(managerPtr, clip.clipPtr)
	End Method
	
	Rem
	bbdoc: Advance time.
	End Rem
	Method update(timeIncrease:Float)
		bmx_TheoraVideoManager_update(managerPtr, timeIncrease)
	End Method
	
	Rem
	bbdoc: Get nicely formated version string.
	End Rem
	Method getVersionString:String()
		Return bmx_TheoraVideoManager_getVersionString(managerPtr)
	End Method
	
	Rem
	bbdoc: Get version numbers.
	about: If c is negative, it means it's a release candidate -c
	End Rem
	Method getVersion(a:Int Var, b:Int Var, c:Int Var)
		bmx_TheoraVideoManager_getVersion(managerPtr, Varptr a, Varptr b, Varptr c)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method setAudioInterfaceFactory(factory:TTheoraAudioInterfaceFactory)
		audioFactory = factory
		bmx_TheoraVideoManager_setAudioInterfaceFactory(managerPtr, factory.factoryPtr)
	End Method
	
	Method Free()
		If managerPtr Then
			bmx_TheoraVideoManager_free(managerPtr)
			managerPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type


Rem
bbdoc: Contains all data related to video playback, eg. the open source file, the frame queue etc.
End Rem
Type TTheoraVideoClip

	Field clipPtr:Byte Ptr
	
	Function _create:TTheoraVideoClip(clipPtr:Byte Ptr)
		If clipPtr Then
			Local this:TTheoraVideoClip = New TTheoraVideoClip
			this.clipPtr = clipPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the first available frame in the queue or Null if no frames are available.
	End Rem
	Method getNextFrame:TTheoraVideoFrame()
		Return TTheoraVideoFrame._create(bmx_TTheoraVideoClip_getNextFrame(clipPtr))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method getName:String()
		Return bmx_TheoraVideoClip_getName(clipPtr)
	End Method
	
	Rem
	bbdoc: Returns the width in pixels of the video clip.
	End Rem
	Method getWidth:Int()
		Return bmx_TheoraVideoClip_getWidth(clipPtr)
	End Method
	
	Rem
	bbdoc: Returns the height in pixels of the video clip.
	End Rem
	Method getHeight:Int()
		Return bmx_TheoraVideoClip_getHeight(clipPtr)
	End Method
	
	Rem
	bbdoc: Returns the stride in pixels.
	about: If you've specified usePower2Stride when creating the TheoraVideoClip object then this value will be the
	next power of two size compared to width, eg: w=376, stride=512. Otherwise, stride will be equal to width
	End Rem
	Method getStride:Int()
		Return bmx_TheoraVideoClip_getStride(clipPtr)
	End Method
	
	Rem
	bbdoc: Pop the frame from the front of the FrameQueue.
	End Rem
	Method popFrame()
		bmx_TheoraVideoClip_popFrame(clipPtr)
	End Method
	
	Rem
	bbdoc: Gets the current time index from the timer object.
	End Rem
	Method getTimePosition:Float()
		Return bmx_TheoraVideoClip_getTimePosition(clipPtr)
	End Method
	
	Rem
	bbdoc: Gets the duration of the movie in seconds.
	End Rem
	Method getDuration:Float()
		Return bmx_TheoraVideoClip_getDuration(clipPtr)
	End Method
	
	Rem
	bbdoc: Returns the clips' frame rate.
	End Rem
	Method getFPS:Int()
		Return bmx_TheoraVideoClip_getFPS(clipPtr)
	End Method
	
	Rem
	bbdoc: Gets the number of frames in this movie.
	End Rem
	Method getNumFrames:Int()
		Return bmx_TheoraVideoClip_getNumFrames(clipPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the clip has finished playing.
	End Rem
	Method isDone:Int()
		Return bmx_TheoraVideoClip_isDone(clipPtr)
	End Method
	
	Rem
	bbdoc: Play the clip from the current position.
	End Rem
	Method play()
		bmx_TheoraVideoClip_play(clipPtr)
	End Method
	
	Rem
	bbdoc: Pause the clip.
	End Rem
	Method pause()
		bmx_TheoraVideoClip_pause(clipPtr)
	End Method
	
	Rem
	bbdoc: Restart from the beginning.
	End Rem
	Method restart()
		bmx_TheoraVideoClip_restart(clipPtr)
	End Method
	
	Rem
	bbdoc: Returns True if the clip is currently paused.
	End Rem
	Method isPaused:Int()
		Return bmx_TheoraVideoClip_isPaused(clipPtr)
	End Method
	
	Rem
	bbdoc: Stop the clip.
	End Rem
	Method stop()
		bmx_TheoraVideoClip_stop(clipPtr)
	End Method
	
	Rem
	bbdoc: Sets the playback speed.
	End Rem
	Method setPlaybackSpeed(speed:Float)
		bmx_TheoraVideoClip_setPlaybackSpeed(clipPtr, speed)
	End Method
	
	Rem
	bbdoc: Returns the playback speed.
	End Rem
	Method getPlaybackSpeed:Float()
		Return bmx_TheoraVideoClip_getPlaybackSpeed(clipPtr)
	End Method
	
	Rem
	bbdoc: Seeks to the time specified.
	End Rem
	Method seek(time:Float)
		bmx_TheoraVideoClip_seek(clipPtr, time)
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type TTheoraVideoFrame

	Field framePtr:Byte Ptr

	Function _create:TTheoraVideoFrame(framePtr:Byte Ptr)
		If framePtr Then
			Local this:TTheoraVideoFrame = New TTheoraVideoFrame
			this.framePtr = framePtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method getBuffer:Byte Ptr()
		Return bmx_TheoraVideoFrame_getBuffer(framePtr)
	End Method

End Type

Type TTheoraAudioInterface

	Field audioPtr:Byte Ptr

End Type

Rem
bbdoc: 
End Rem
Type TTheoraAudioInterfaceFactory

	Field factoryPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TTheoraAudioInterfaceFactory() Abstract

End Type


