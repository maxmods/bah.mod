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

Import "-lgme"

Import "gme/src/*.h"

Import "*.h"
Import "glue.cpp"

Extern

	Function bmx_gme_emu_new:Byte Ptr()
	Function bmx_gme_emu_delete(handle:Byte Ptr)
	
	Function bmx_gme_open_file:String(handle:Byte Ptr, file:String, sampleRate:Int)
	Function bmx_gme_open_data:String(handle:Byte Ptr, data:Byte Ptr, size:Int, sampleRate:Int)
	Function bmx_gme_start_track:String(handle:Byte Ptr, index:Int)
	Function bmx_gme_play:String(handle:Byte Ptr, count:Int, out:Byte Ptr)
	Function bmx_gme_track_count:Int(handle:Byte Ptr)
	Function bmx_gme_track_ended:Int(handle:Byte Ptr)
	Function bmx_gme_tell:Int(handle:Byte Ptr)
	Function bmx_gme_seek:String(handle:Byte Ptr, msec:Int)
	Function bmx_gme_set_fade(handle:Byte Ptr, startMsec:Int)
	Function bmx_gme_set_stereo_depth(handle:Byte Ptr, depth:Double)
	Function bmx_gme_ignore_silence(handle:Byte Ptr, ignore:Int)
	Function bmx_gme_set_tempo(handle:Byte Ptr, tempo:Double)
	Function bmx_gme_mute_voice(handle:Byte Ptr, index:Int, mute:Int)
	Function bmx_gme_mute_voices(handle:Byte Ptr, mutingMask:Int)

End Extern


