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
bbdoc: AVbin
about: Media decoder.
End Rem
Module BaH.AVbin

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: AVbin - 2007 Alex Holkner"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"


Import "common.bmx"

Rem
bbdoc: AVBin core.
End Rem
Type TAVbin

	Global _callback(_module:String, level:Int, _message:String)

	Rem
	bbdoc: Initialise AVbin.
	about: This must be called before opening a file.  Check the
	return value for success before continuing.
	End Rem
	Function Init:Int()
		Return avbin_init()
	End Function

	Rem
	bbdoc: Get the linked version of AVbin.
	about: Version numbers are always integer, there are no "minor" or "patch"
	revisions.  All AVbin versions are backward and forward compatible, modulo
	the required feature set.
	End Rem
	Function GetVersion:Int()
		Return avbin_get_version()
	End Function
	
	Rem
	bbdoc: Get the SVN revision of FFmpeg.
	about: This is built into AVbin as it is built.
	End Rem
	Function GetFFmpegRevision:Int()
		Return avbin_get_ffmpeg_revision()
	End Function
	
	Rem
	bbdoc: Get the minimum audio buffer size, in bytes.
	End Rem
	Function GetAudioBufferSize:Int()
		Return avbin_get_audio_buffer_size()
	End Function
	
	Rem
	bbdoc: Determine if AVbin includes a requested feature.
	returns: True if AVbin has the feature, False otherwise.
	about: When future versions of AVbin include more functionality, that
	functionality can be tested for by calling this function.  Currently there
	are no features to test for.
	End Rem
	Function HaveFeature:Int(feature:String)
		Return bmx_avbin_havefeature(feature)
	End Function
	
	Rem
	bbdoc: Set the log level verbosity.
	End Rem
	Function SetLogLevel:Int(level:Int)
		Return avbin_set_log_level(level)
	End Function
	
	Rem
	bbdoc: Set a custom log callback.
	about: By default, log messages are printed to
	standard error.  Providing a Null callback restores this default handler.
	End Rem
	Function SetLogCallback:Int(callback(_module:String, level:Int, _message:String))
		_callback = callback
		If callback Then
			Return avbin_set_log_callback(_logcallback)
		Else
			Return avbin_set_log_callback(Null)
		End If
	End Function
	
	Function _logCallback(_module:Byte Ptr, level:Int, _message:Byte Ptr)
		If _callback Then
			_callback(String.FromCString(_module), level, String.FromCString(_message))
		End If
	End Function
	
End Type

Rem
bbdoc: A media file.
End Rem
Type TAVbinFile

	Field avbinFilePtr:Byte Ptr
	
	Function _create:TAVbinFile(avbinFilePtr:Byte Ptr)
		If avbinFilePtr Then
			Local this:TAVbinFile = New TAVbinFile
			this.avbinFilePtr = avbinFilePtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Open a media file given its filename.
	returns: Null if the file could not be opened, or is not of a recognised file format.
	End Rem
	Function OpenFilename:TAVbinFile(filename:String)
		Return TAVbinFile._create(bmx_avbinfile_openfilename(filename))
	End Function
	
	Rem
	bbdoc: Close the media file.
	End Rem
	Method CloseFile()
		bmx_avbinfile_closefile(avbinFilePtr)
	End Method
	
	Rem
	bbdoc: Seek to a timestamp within the file.
	about: For video files, the first keyframe before the requested timestamp will be
	seeked to.  For audio files, the first audio packet before the requested
	timestamp is used.
	End Rem
	Method SeekFile:Int(timestamp:Long)
		Return bmx_avbinfile_seekfile(avbinFilePtr, timestamp)
	End Method
	
	Rem
	bbdoc: Get information about the opened file.
	End Rem
	Method FileInfo:TAVbinFileInfo()
		Return TAVbinFileInfo._create(bmx_avbinfile_fileinfo(avbinFilePtr))
	End Method
	
	Rem
	bbdoc: Get information about a stream within the file.
	about: Ensure that stream @index is less than number of streams given in the file info.
	End Rem
	Method StreamInfo:Int(index:Int, info:TAVbinStreamInfo)
		Return bmx_avbin_streaminfo(avbinFilePtr, index, info.avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Open a stream for decoding.
	about: If you intend to decode audio or video from a file, you must open the
	stream first.  The returned TAVbinStream object should be passed to the relevant
	decode method when a packet for that stream is read.
	End Rem
	Method OpenStream:TAVbinStream(index:Int)
		Return TAVbinStream._create(bmx_avbinfile_openstream(avbinFilePtr, index))
	End Method
	
	Rem
	bbdoc: Read a packet from the file.
	about: Pass in a TAVbinPacket object. On return, the object will be filled with a packet of data.
	The packet data is valid until the next call to Read().
	<p>
	Applications should examine the packet's stream index to match it with
	an appropriate open stream handle, or discard it if none match.  The packet
	data can then be passed to the relevant decode method.
	</p>
	End Rem
	Method Read:Int(Packet:TAVbinPacket)
		Return bmx_avbinfile_read(avbinfileptr, Packet.avbinpacketPtr)
	End Method
	
End Type

Rem
bbdoc: File details.
End Rem
Type TAVbinFileInfo

	Field avbinFileInfoPtr:Byte Ptr
	
	Function _create:TAVbinFileInfo(avbinFileInfoPtr:Byte Ptr)
		If avbinFileInfoPtr Then
			Local this:TAVbinFileInfo = New TAVbinFileInfo
			this.avbinFileInfoPtr = avbinFileInfoPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Number of streams contained in the file.
	End Rem
	Method GetStreamCount:Int()
		Return bmx_avbinfileinfo_getstreamcount(avbinFileInfoPtr)
	End Method

	Rem
	bbdoc: Starting time of all streams.
	End Rem
	Method GetStartTime:Long()
		Local value:Long
		bmx_avbinfileinfo_getstarttime(avbinFileInfoPtr, Varptr value)
		Return value
	End Method
	
	Rem
	bbdoc: Duration of the file. 
	about: Does not include the time given in start_time.
	End Rem
	Method GetDuration:Long()
		Local value:Long
		bmx_avbinfileinfo_getduration(avbinFileInfoPtr, Varptr value)
		Return value
	End Method
	
	Rem
	bbdoc: Returns the title, or Null if not defined.
	End Rem
	Method GetTitle:String()
		Return bmx_avbinfileinfo_gettitle(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the author, or Null if not defined.
	End Rem
	Method GeAuthor:String()
		Return bmx_avbinfileinfo_getauthor(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the copyright information, or Null if not defined.
	End Rem
	Method GetCopyright:String()
		Return bmx_avbinfileinfo_getcopyright(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the comment, or Null if not defined.
	End Rem
	Method GetComment:String()
		Return bmx_avbinfileinfo_getcomment(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the album name, or Null if not defined.
	End Rem
	Method GetAlbum:String()
		Return bmx_avbinfileinfo_getalbum(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the year, or zero if not defined.
	End Rem
	Method GetYear:Int()
		Return bmx_avbinfileinfo_getyear(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the track number, or zero if not defined.
	End Rem
	Method GetTrack:Int()
		Return bmx_avbinfileinfo_gettrack(avbinFileInfoPtr)
	End Method
	
	Rem
	bbdoc: Returns the genre, or Null if not defined.
	End Rem
	Method GetGenre:String()
		Return bmx_avbinfileinfo_getgenre(avbinFileInfoPtr)
	End Method
	
	Method Delete()
		If avbinFileInfoPtr Then
			bmx_avbinfileinfo_delete(avbinFileInfoPtr)
			avbinFileInfoPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: A stream within a file.
End Rem
Type TAVbinStream

	Field avbinstreamPtr:Byte Ptr

	Function _create:TAVbinStream(avbinstreamPtr:Byte Ptr)
		If avbinstreamPtr Then
			Local this:TAVbinStream = New TAVbinStream
			this.avbinstreamPtr = avbinstreamPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Decode some audio data.
	returns: The number of bytes of dataIn actually used. You should call this method repeatedly as long as the return value is greater than 0.
	about: You must ensure that @dataOut is at least as big as the minimum audio
	buffer size (see TAVbin.GetAudioBufferSize()).
	End Rem
	Method DecodeAudio:Int(dataIn:Byte Ptr, inSize:Int, dataOut:Byte Ptr, outSize:Int Var)
		Return bmx_avbinstream_decodeaudio(avbinstreamPtr, dataIn, inSize, dataOut, Varptr outSize)
	End Method
	
	Rem
	bbdoc: Decode a video frame image.
	about: The size of @dataOut must be large enough to hold the entire image.
	This is width * height * 3 (images are always in 8-bit RGB format).
	End Rem
	Method DecodeVideo:Int(dataIn:Byte Ptr, inSize:Int, dataOut:Byte Ptr)
		Return bmx_avbinstream_decodevideo(avbinstreamPtr, dataIn, inSize, dataOut)
	End Method
	
	Rem
	bbdoc: Close the file stream.
	End Rem
	Method Close()
		bmx_avbinstream_close(avbinstreamPtr)
	End Method


End Type

Rem
bbdoc: A single packet of stream data.
End Rem
Type TAVbinPacket

	Field avbinpacketPtr:Byte Ptr

	Function _create:TAVbinPacket(avbinpacketPtr:Byte Ptr)
		If avbinpacketPtr Then
			Local this:TAVbinPacket = New TAVbinPacket
			this.avbinpacketPtr = avbinpacketPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Creates a new TAVbinPacket object, for use with TAVbinFile.Read()
	End Rem
	Function Create:TAVbinPacket()
		Local this:TAVbinPacket = New TAVbinPacket
		this.avbinpacketPtr = bmx_avbinpacket_create()
		Return this
	End Function
	
	Rem
	bbdoc: The time at which this packet is to be played.
	about: This can be used to synchronise audio and video data.
	End Rem
	Method GetTimeStamp:Long()
		Local value:Long
		bmx_avbinpacket_gettimestamp(avbinpacketPtr, Varptr value)
		Return value
	End Method
	
	Rem
	bbdoc: The stream this packet contains data for.
	End Rem
	Method GetStreamIndex:Int()
		Return bmx_avbinpacket_getstreamindex(avbinpacketPtr)
	End Method
	
	Rem
	bbdoc: The data this packet refers to.
	End Rem
	Method GetData:Byte Ptr()
		Return bmx_avbinpacket_getdata(avbinpacketPtr)
	End Method
	
	Rem
	bbdoc: The size of the data.
	End Rem
	Method GetSize:Int()
		Return bmx_avbinpacket_getsize(avbinpacketPtr)
	End Method

	Method Delete()
		If avbinpacketPtr Then
			bmx_avbinpacket_delete(avbinpacketPtr)
			avbinpacketPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: Stream details.
about: A stream is a single audio track or video.  Most audio files contain one
audio stream.  Most video files contain one audio stream and one video
stream.  More than one audio stream may indicate the presence of multiple
languages which can be selected (however at this time AVbin does not
provide language information).
End Rem
Type TAVbinStreamInfo

	Field avbinstreaminfoPtr:Byte Ptr
	
	Function _create:TAVbinStreamInfo(avbinstreaminfoPtr:Byte Ptr)
		If avbinstreaminfoPtr Then
			Local this:TAVbinStreamInfo = New TAVbinStreamInfo
			this.avbinstreaminfoPtr = avbinstreaminfoPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Creates a new TAVbinStreamInfo object for use with TAVbinFile.StreamInfo()
	End Rem
	Function Create:TAVbinStreamInfo()
		Return TAVbinStreamInfo._create(bmx_avbinstreaminfo_create())
	End Function
	
	Rem
	bbdoc: The type of stream; either audio or video.
	End Rem
	Method GetType:Int()
		Return bmx_avbinstreaminfo_gettype(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Width of the video image, in pixels.
	about: This is the width of actual video data, and is not necessarily the size the video
	is to be displayed at (see GetAspectNum).
	End Rem
	Method GetWidth:Int()
		Return bmx_avbinstreaminfo_getwidth(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Height of the video image, in pixels.
	End Rem
	Method GetHeight:Int()
		Return bmx_avbinstreaminfo_getheight(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Aspect-ratio of each pixel.
	about: The aspect is given by dividing AspectNum by AspectDen.
	End Rem
	Method GetAspectNum:Int()
		Return bmx_avbinstreaminfo_getaspectnum(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Aspect-ratio of each pixel.
	about: The aspect is given by dividing AspectNum by AspectDen.
	End Rem
	Method GetAspectDen:Int()
		Return bmx_avbinstreaminfo_getaspectden(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Data type of audio samples.
	End Rem
	Method GetAudioFormat:Int()
		Return bmx_avbinstreaminfo_getaudioformat(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Number of samples per second, in Hz.
	End Rem
	Method GetAudioRate:Int()
		Return bmx_avbinstreaminfo_getaudiorate(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Number of bits per sample; typically 8 or 16.
	End Rem
	Method GetAudioBits:Int()
		Return bmx_avbinstreaminfo_getaudiobits(avbinstreaminfoPtr)
	End Method
	
	Rem
	bbdoc: Number of interleaved audio channels.
	about: Typically 1 for monoaural, 2 for stereo.  Higher channel numbers are used for
     surround sound, however AVbin does not currently provide a way to access the arrangement of these channels.
	End Rem
	Method GetAudioChannels:Int()
		Return bmx_avbinstreaminfo_getaudiochannels(avbinstreaminfoPtr)
	End Method
	
	Method Delete()
		If avbinstreaminfoPtr Then
			bmx_avbinstreaminfo_delete(avbinstreaminfoPtr)
			avbinstreaminfoPtr = Null
		End If
	End Method
	
End Type



