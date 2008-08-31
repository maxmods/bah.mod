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

Import BRL.Blitz

?macos
Import "-lavbin"
?win32
Import "lib/win32/avbin.a"
?

Import "include/*.h"

Import "glue.cpp"

Extern

	Function avbin_init:Int()
	Function avbin_get_audio_buffer_size:Int()
	Function avbin_get_version:Int()
	Function avbin_set_log_level:Int(level:Int)
	Function avbin_set_log_callback:Int(cb(_mod:Byte Ptr, level:Int, _msg:Byte Ptr))
	Function avbin_get_ffmpeg_revision:Int()

	Function bmx_avbin_havefeature:Int(feature:String)

	Function bmx_avbinfile_openfilename:Byte Ptr(filename:String)
	Function bmx_avbinfile_closefile(handle:Byte Ptr)
	Function bmx_avbinfile_seekfile:Int(handle:Byte Ptr, timestamp:Long)
	Function bmx_avbinfile_fileinfo:Byte Ptr(handle:Byte Ptr)
	Function bmx_avbinfile_read:Int(handle:Byte Ptr, Packet:Byte Ptr)
	Function bmx_avbinfile_openstream:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_avbin_streaminfo:Int(handle:Byte Ptr, index:Int, streamInfo:Byte Ptr)

	Function bmx_avbinfileinfo_getstreamcount:Int(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getstarttime(handle:Byte Ptr, value:Long Ptr)
	Function bmx_avbinfileinfo_getduration(handle:Byte Ptr, value:Long Ptr)
	Function bmx_avbinfileinfo_delete(handle:Byte Ptr)
	Function bmx_avbinfileinfo_gettitle:String(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getauthor:String(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getcopyright:String(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getcomment:String(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getalbum:String(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getyear:Int(handle:Byte Ptr)
	Function bmx_avbinfileinfo_gettrack:Int(handle:Byte Ptr)
	Function bmx_avbinfileinfo_getgenre:String(handle:Byte Ptr)

	Function bmx_avbinstream_decodeaudio:Int(handle:Byte Ptr, dataIn:Byte Ptr, inSize:Int, dataOut:Byte Ptr, outSize:Int Ptr)
	Function bmx_avbinstream_decodevideo:Int(handle:Byte Ptr, dataIn:Byte Ptr, inSize:Int, dataOut:Byte Ptr)
	Function bmx_avbinstream_close(handle:Byte Ptr)

	Function bmx_avbinpacket_create:Byte Ptr()
	Function bmx_avbinpacket_delete(handle:Byte Ptr)
	Function bmx_avbinpacket_gettimestamp(handle:Byte Ptr, value:Long Ptr)
	Function bmx_avbinpacket_getstreamindex:Int(handle:Byte Ptr)
	Function bmx_avbinpacket_getdata:Byte Ptr(handle:Byte Ptr)
	Function bmx_avbinpacket_getsize:Int(handle:Byte Ptr)

	Function bmx_avbinstreaminfo_create:Byte Ptr()
	Function bmx_avbinstreaminfo_gettype:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getwidth:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getheight:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getaspectnum:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getaspectden:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getaudioformat:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getaudiorate:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getaudiobits:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_getaudiochannels:Int(handle:Byte Ptr)
	Function bmx_avbinstreaminfo_delete(handle:Byte Ptr)

End Extern


Const AVBIN_RESULT_ERROR:Int = -1
Const AVBIN_RESULT_OK:Int = 0

Const AVBIN_STREAM_TYPE_UNKNOWN:Int = 0
Const AVBIN_STREAM_TYPE_VIDEO:Int = 1
Const AVBIN_STREAM_TYPE_AUDIO:Int = 2


Const AVBIN_SAMPLE_FORMAT_U8:Int = 0
' Signed 16-bit integer */
Const AVBIN_SAMPLE_FORMAT_S16:Int = 1
' Signed 24-bit integer */
Const AVBIN_SAMPLE_FORMAT_S24:Int = 2
' Signed 32-bit integer */
Const AVBIN_SAMPLE_FORMAT_S32:Int = 3
' 32-bit IEEE floating-point */
Const AVBIN_SAMPLE_FORMAT_FLOAT:Int = 4

Const AVBIN_LOG_QUIET:Int = -8
Const AVBIN_LOG_PANIC:Int = 0
Const AVBIN_LOG_FATAL:Int = 8
Const AVBIN_LOG_ERROR:Int = 16
Const AVBIN_LOG_WARNING:Int = 24
Const AVBIN_LOG_INFO:Int = 32
Const AVBIN_LOG_VERBOSE:Int = 40
Const AVBIN_LOG_DEBUG:Int = 48

