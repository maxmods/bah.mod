' Copyright (c) 2011-2014 Bruce A Henderson
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

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: Wrapper - 2011-2014 Bruce A Henderson"
ModuleInfo "Copyright: theoraplayer - 2008-2013 Kresimir Spes"

ModuleInfo "History: 1.00 Initial Release (SVN rev 186)"



ModuleInfo "CC_OPTS: -fexceptions"
ModuleInfo "CC_OPTS: -D__THEORA"

?win32
ModuleInfo "CC_OPTS: -DTHEORAVIDEO_STATIC -D_LIB"
?

Import "common.bmx"

'
' Code changes :
' 
'  TheoraVideoClip.h - added include for Win32 build.
'  TheoraVideoManager.cpp - added include for Win32 build.
'

Rem
bbdoc: Handles all playback/sync operations.
End Rem
Type TTheoraVideoManager

	Field managerPtr:Byte Ptr
	
	Field audioFactory:TTheoraAudioInterfaceFactory
	
	Global logFunction(text:String) = Null
	
	Rem
	bbdoc: Creates a Video Manager instance, with an optional worker thread count.
	End Rem
	Method Create:TTheoraVideoManager(numWorkerThreads:Int = 1)
		managerPtr = bmx_TheoraVideoManager_new(numWorkerThreads)
		Return Self
	End Method
	
	Rem
	bbdoc: Creates a video clip with the specified parameters.
	about: When you are finished with the clip, it should be freed using the #destroyVideoClip() method.
	End Rem
	Method createVideoClip:TTheoraVideoClip(file:Object, outputMode:Int = TH_RGB, numPrecachedOverride:Int = 0, usePower2Stride:Int = 0)
		'If TTheoraDataSource(file) Then
		'	Return TTheoraVideoClip._create(bmx_TheoraVideoManager_createVideoClipDataSource(managerPtr, TTheoraDataSource(file).dataSourcePtr, outputMode, numPrecachedOverride, usePower2Stride))
		'Else
		 If String(file) Then
			Return TTheoraVideoClip._create(bmx_TheoraVideoManager_createVideoClip(managerPtr, String(file), outputMode, numPrecachedOverride, usePower2Stride))
		End If
	End Method
	
	Rem
	bbdoc: Frees a previously created video clip.
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
	bbdoc: Searches registered clips by name.
	End Rem
	Method getVideoClipByName:TTheoraVideoClip(name:String)
		Return TTheoraVideoClip._create(bmx_TheoraVideoManager_getVideoClipByName(managerPtr, name))
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
	bbdoc: Sets the audio interface.
	about: Call this with an appropriate audio factory to enable sound with the video.
	End Rem
	Method setAudioInterfaceFactory(factory:TTheoraAudioInterfaceFactory)
		audioFactory = factory
		bmx_TheoraVideoManager_setAudioInterfaceFactory(managerPtr, factory.factoryPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of worker threads.
	End Rem
	Method getNumWorkerThreads:Int()
		Return bmx_TheoraVideoManager_getNumWorkerThreads(managerPtr)
	End Method
	
	Rem
	bbdoc: Sets the number of worker threads.
	End Rem
	Method setNumWorkerThreads(numWorkerThreads:Int)
		bmx_TheoraVideoManager_setNumWorkerThreads(managerPtr, numWorkerThreads)
	End Method
	
	Rem
	bbdoc: Sets your own log function to recieve theora's log calls.
	about: Allows the integration of libtheoraplayer's log messages in your own logging system, prefix them, mute them or whatever you want.
	End Rem
	Function setLogFunction(func(text:String))
		logFunction = func
		bmx_TheoraVideoManager_setLogFunction(logFunction)
	End Function
	
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
	bbdoc: Returns the name of the video clip.
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
	
	Rem
	bbdoc: Updates timer to the display time of the next frame.
	returns: Time advanced. 0 if no frames are ready.
	about: Useful if you want to grab frames instead of regular display.
	End Rem
	Method updateToNextFrame:Float()
		Return bmx_TheoraVideoClip_updateToNextFrame(clipPtr)
	End Method
	
	Rem
	bbdoc: Sets the auto restart flag.
	about: If you want the video to automatically and smoothly restart when the last frame is reached.
	End Rem
	Method setAutoRestart(restart:Int)
		bmx_TheoraVideoClip_setAutoRestart(clipPtr, restart)
	End Method
	
	Rem
	bbdoc: Returns the auto restart flag.
	about: If you want the video to automatically and smoothly restart when the last frame is reached.
	bbdoc: 
	End Rem
	Method getAutoRestart:Int()
		Return bmx_TheoraVideoClip_getAutoRestart(clipPtr)
	End Method
	
End Type


Rem
bbdoc: A video frame.
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
	bbdoc: Returns the pixel data buffer for this frame.
	End Rem
	Method getBuffer:Byte Ptr()
		Return bmx_TheoraVideoFrame_getBuffer(framePtr)
	End Method

End Type

'Rem
' FIXME : well, it doesn't seem to work for some reason. Requires threaded + crashes in _read !?
'bbdoc: A streamable datasource, for playback.
'End Rem
Type TTheoraDataSource

	Field dataSourcePtr:Byte Ptr

	Field stream:TStream
	Field name:String

	'Rem
	'bbdoc: Creates a TStream-based datasource.
	'End Rem
	Function Create:TTheoraDataSource(stream:TStream, name:String = Null)
		Local this:TTheoraDataSource = New TTheoraDataSource
		this.dataSourcePtr = bmx_TheoraDataSource_create(this)
		this.stream = stream
		this.name = name
		Return this
	End Function

	Function _tell:Int(source:TTheoraDataSource)
		Return source.stream.Pos()
	End Function

 	Function _size:Int(source:TTheoraDataSource)
		Return source.stream.Size()
	End Function

	Function _seek(source:TTheoraDataSource, pos:Int)
		source.stream.Seek(pos)
	End Function

	Function _read:Int(source:TTheoraDataSource, buf:Byte Ptr, count:Int)
		Return source.stream.Read(buf, count)
	End Function
	
	Function _repr:String(source:TTheoraDataSource)
		Return source.name
	End Function

	Method Free()
		If dataSourcePtr Then
			bmx_TheoraDataSource_free(dataSourcePtr)
			dataSourcePtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
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


