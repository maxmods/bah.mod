' Copyright (c) 2008-2010 Bruce A Henderson
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

?win32
Import "lib/win32/bass.lib"
?macos
Import "-lbass"
?linux
Import "-lbass"
Import "-pthread"
?

Import "include/*.h"

Import "glue.cpp"

Extern
	Function bmx_bassinfo_getflags:Int(handle:Byte Ptr)
	Function bmx_bassinfo_gethwsize:Int(handle:Byte Ptr)
	Function bmx_bassinfo_gethwfree:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getfreesam:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getfree3d:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getminrate:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getmaxrate:Int(handle:Byte Ptr)
	Function bmx_bassinfo_geteax:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getminbuf:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getdsver:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getlatency:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getinitflags:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getspeakers:Int(handle:Byte Ptr)
	Function bmx_bassinfo_getfreq:Int(handle:Byte Ptr)
	Function bmx_bassinfo_delete(handle:Byte Ptr)

	Function bmx_bass_channelgetlevel:Int(handle:Int, Left:Int Ptr, Right:Int Ptr)
	Function bmx_bass_channelgetlength(handle:Int, length:Long Ptr, Mode:Int)
	Function bmx_bass_channelgetposition(handle:Int, pos:Long Ptr, Mode:Int)
	Function bmx_bass_channelgetpositionlowhigh(handle:Int, pos:Long Ptr, Mode:Int, low:Int Ptr, high:Int Ptr)
	Function bmx_bass_channelseconds2bytes(handle:Int, bytes:Long Ptr, pos:Double)
	Function bmx_bass_channelgettags:String[](handle:Int, tags:Int)

	Function bmx_bass_streamgetfileposition(handle:Int, pos:Long Ptr, Mode:Int)
	Function bmx_bass_streamcreatetstream:Int(handle:Object, system:Int, flags:Int)
	Function bmx_bass_streamcreateurlncb:Int(url:Byte Ptr, offset:Int, flags:Int, user:Object)
	Function bmx_bass_streamcreateurl:Int(url:Byte Ptr, offset:Int, flags:Int, user:Object)

	Function bmx_bass_getsampleinfo:Byte Ptr(handle:Int)
	Function bmx_bass_setsampleinfo:Int(handle:Int, info:Byte Ptr)
	
	Function bmx_sampleinfo_getfreq:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getvolume:Float(handle:Byte Ptr)
	Function bmx_sampleinfo_getpan:Float(handle:Byte Ptr)
	Function bmx_sampleinfo_getflags:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getlength:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getmax:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getorigres:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getchans:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getmingap:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getmode3d:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getmindist:Float(handle:Byte Ptr)
	Function bmx_sampleinfo_getmaxdist:Float(handle:Byte Ptr)
	Function bmx_sampleinfo_getiangle:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getoangle:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getoutvol:Float(handle:Byte Ptr)
	Function bmx_sampleinfo_getvam:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_getpriority:Int(handle:Byte Ptr)
	Function bmx_sampleinfo_delete(handle:Byte Ptr)
	Function bmx_sampleinfo_setfreq(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setvolume(handle:Byte Ptr, value:Float)
	Function bmx_sampleinfo_setpan(handle:Byte Ptr, value:Float)
	Function bmx_sampleinfo_setflags(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setmingap(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setmode3d(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setmindist(handle:Byte Ptr, value:Float)
	Function bmx_sampleinfo_setmaxdist(handle:Byte Ptr, value:Float)
	Function bmx_sampleinfo_setiangle(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setoangle(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setoutvol(handle:Byte Ptr, value:Float)
	Function bmx_sampleinfo_setvam(handle:Byte Ptr, value:Int)
	Function bmx_sampleinfo_setpriority(handle:Byte Ptr, value:Int)

	Function bmx_bass_getdeviceinfo:Byte Ptr(device:Int)
	Function bmx_deviceinfo_getname:Byte Ptr(handle:Byte Ptr)
	Function bmx_deviceinfo_getdriver:Byte Ptr(handle:Byte Ptr)
	Function bmx_deviceinfo_getflags:Int(handle:Byte Ptr)
	Function bmx_deviceinfo_delete(handle:Byte Ptr)

	Function bmx_bass_recordgetinfo:Byte Ptr()

	Function bmx_recordinfo_getflags:Int(handle:Byte Ptr)
	Function bmx_recordinfo_getformats:Int(handle:Byte Ptr)
	Function bmx_recordinfo_getinputs:Int(handle:Byte Ptr)
	Function bmx_recordinfo_issinglein:Int(handle:Byte Ptr)
	Function bmx_recordinfo_getfreq:Int(handle:Byte Ptr)
	Function bmx_recordinfo_delete(handle:Byte Ptr)
	
	Function bmx_bass_fxdelete(handle:Byte Ptr)

	Function bmx_bass_3dvector_new:Byte Ptr()
	Function bmx_bass_3dvector_setxyz(handle:Byte Ptr, x:Float, y:Float, z:Float)
	Function bmx_bass_3dvector_delete(handle:Byte Ptr)
	Function bmx_bass_3dvector_getx:Float(handle:Byte Ptr)
	Function bmx_bass_3dvector_gety:Float(handle:Byte Ptr)
	Function bmx_bass_3dvector_getz:Float(handle:Byte Ptr)
	Function bmx_bass_3dvector_setx(handle:Byte Ptr, value:Float)
	Function bmx_bass_3dvector_sety(handle:Byte Ptr, value:Float)
	Function bmx_bass_3dvector_setz(handle:Byte Ptr, value:Float)
	Function bmx_bass_3dvector_getxyz(handle:Byte Ptr, x:Float Ptr, y:Float Ptr, z:Float Ptr)

	Function bmx_bass_getchannelinfo:Byte Ptr(handle:Int)
	Function bmx_channelinfo_getfreq:Int(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_getchannels:Int(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_getflags:Int(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_getctype:Int(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_getorigres:Int(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_getsample:Int(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_getfilename:Byte Ptr(channelinfoPtr:Byte Ptr)
	Function bmx_channelinfo_delete(channelinfoPtr:Byte Ptr)

	Function bmx_bass_getinfo:Byte Ptr()

	Function bmx_bass_setsync(handle:Int, stype:Int, param:Long, syncData:Byte Ptr, channel:Int Ptr, data:Int Ptr, set:Int Ptr)
	Function bmx_bass_syncdata_new:Byte Ptr()
	
End Extern

?win32
Extern "win32"
?Not win32
Extern
?
	Function BASS_Init:Int(device:Int, freq:Int, flags:Int, win:Byte Ptr, clsid:Byte Ptr)
	Function BASS_Free:Int()
	Function BASS_ErrorGetCode:Int()
	Function BASS_GetVolume:Float()
	Function BASS_SetDevice:Int(device:Int)
	Function BASS_GetDevice:Int()
	Function BASS_GetCPU:Float()
	Function BASS_Apply3D()
	Function BASS_Start:Int()
?win32
	Function BASS_GetEAXParameters:Int(env:Int Ptr, vol:Float Ptr, decay:Float Ptr, damp:Float Ptr)
	Function BASS_SetEAXParameters:Int(env:Int, vol:Float, decay:Float, damp:Float)
?
	Function BASS_Get3DFactors:Int(distf:Float Ptr, rollf:Float Ptr, doppf:Float Ptr)
	Function BASS_Set3DFactors:Int(distf:Float, rollf:Float, doppf:Float)

	
	Function BASS_ChannelPlay:Int(handle:Int, restart:Int)
	Function BASS_ChannelPause:Int(handle:Int)
	Function BASS_ChannelStop:Int(handle:Int)
	Function BASS_ChannelIsActive:Int(handle:Int)
	Function BASS_ChannelUpdate:Int(handle:Int, length:Int)
	Function BASS_ChannelSlideAttribute:Int(handle:Int, attrib:Int, value:Float, time:Int)
	Function BASS_ChannelBytes2Seconds:Double(handle:Int, pos:Long)
	Function BASS_ChannelFlags:Int(handle:Int, flags:Int, mask:Int)
	Function BASS_ChannelIsSliding:Int(handle:Int, attrib:Int)
	Function BASS_ChannelSetFX:Int(handle:Int, fxType:Int, priority:Int)
	Function BASS_ChannelGetAttribute:Int(handle:Int, attrib:Int, value:Float Ptr)
	Function BASS_ChannelSetAttribute:Int(handle:Int, attrib:Int, value:Float)
	Function BASS_ChannelSetLink:Int(handle:Int, channel:Int)
	Function BASS_ChannelGet3DPosition:Int(handle:Int, pos:Byte Ptr, orient:Byte Ptr, vel:Byte Ptr)
	Function BASS_ChannelSet3DPosition:Int(handle:Int, pos:Byte Ptr, orient:Byte Ptr, vel:Byte Ptr)
	Function BASS_ChannelSet3DAttributes:Int(handle:Int, Mode:Int, minDist:Float, maxDist:Float, iangle:Int, ..
			oangle:Int, outvol:Float)
	Function BASS_ChannelGet3DAttributes:Int(handle:Int, Mode:Int Ptr, minDist:Float Ptr, maxDist:Float Ptr, ..
		iangle:Int Ptr, oangle:Int Ptr, outvol:Float Ptr)
	Function BASS_ChannelGetData:Int(handle:Int, buffer:Byte Ptr, length:Int)
	Function BASS_ChannelGetDevice:Int(handle:Int)
	Function BASS_ChannelLock:Int(handle:Int, value:Int)
	Function BASS_ChannelSetPosition:Int(handle:Int, pos:Long, Mode:Int)

	Function BASS_StreamCreate:Int(freq:Int, chans:Int, flags:Int, callback:Int(handle:Int, buffer:Byte Ptr, length:Int, data:Object), user:Object)
	Function BASS_StreamCreateFile:Int(mem:Int, filename:Byte Ptr, offset:Long, length:Long, flags:Int)
	Function BASS_StreamFree:Int(handle:Int)
	Function BASS_SetVolume:Int(volume:Float)
	Function BASS_Pause:Int()
	Function BASS_GetVersion:Int()
	Function BASS_Update:Int(length:Int)
	Function BASS_GetConfig:Int(option:Int)
	Function BASS_SetConfig:Int(option:Int, value:Int)

	Function BASS_FXReset:Int(handle:Int)
	Function BASS_FXSetParameters:Int(handle:Int, fx:Byte Ptr)

	Function BASS_MusicFree:Int(handle:Int)
	
	Function BASS_SampleStop:Int(handle:Int)
	Function BASS_SampleFree:Int(handle:Int)
	Function BASS_SampleCreate:Int(length:Int, freq:Int, channels:Int, maxPlaybacks:Int, flags:Int)
	Function BASS_SampleLoad:Int(mem:Int, file:Byte Ptr, offset:Long, length:Int, _max:Int, flags:Int)
	Function BASS_SampleGetChannel:Int(handle:Int, onlyNew:Int)
	Function BASS_SampleGetData:Int(handle:Int, buffer:Byte Ptr)
	Function BASS_SampleSetData:Int(handle:Int, buffer:Byte Ptr)

	Function BASS_MusicLoad:Int(mem:Int, file:Byte Ptr, offset:Long, length:Int, flags:Int, freq:Int)

	Function BASS_RecordFree:Int()
	Function BASS_RecordGetDevice:Int()
	Function BASS_RecordInit:Int(device:Int)
	Function BASS_RecordSetDevice:Int(device:Int)
	Function BASS_RecordSetInput:Int(inp:Int, flags:Int, volume:Float)
	Function BASS_RecordGetInput:Int(inp:Int, volume:Float Ptr)
	Function BASS_RecordGetInputName:Byte Ptr(inp:Int)
	Function BASS_RecordStart:Int(freq:Int, chans:Int, flags:Int, proc:Int(handle:Int, buffer:Byte Ptr, length:Int, user:Object), user:Object)

	Function BASS_PluginLoad:Int(file:Byte Ptr, flags:Int)
	Function BASS_PluginFree:Int(handle:Int)
	
End Extern


' Error codes returned by BASS_ErrorGetCode
Const BASS_OK:Int = 0	' all is OK
Const BASS_ERROR_MEM:Int = 1	' memory error
Const BASS_ERROR_FILEOPEN:Int = 2	' can't open the file
Const BASS_ERROR_DRIVER:Int = 3	' can't find a free/valid driver
Const BASS_ERROR_BUFLOST:Int = 4	' the sample buffer was lost
Const BASS_ERROR_HANDLE:Int = 5	' invalid handle
Const BASS_ERROR_FORMAT:Int = 6	' unsupported sample format
Const BASS_ERROR_POSITION:Int = 7	' invalid position
Const BASS_ERROR_INIT:Int = 8	' BASS_Init has not been successfully called
Const BASS_ERROR_START:Int = 9	' BASS_Start has not been successfully called
Const BASS_ERROR_ALREADY:Int = 14	' already initialized/paused/whatever
Const BASS_ERROR_NOCHAN:Int = 18	' can't get a free channel
Const BASS_ERROR_ILLTYPE:Int = 19	' an illegal type was specified
Const BASS_ERROR_ILLPARAM:Int = 20	' an illegal parameter was specified
Const BASS_ERROR_NO3D:Int = 21	' no 3D support
Const BASS_ERROR_NOEAX:Int = 22	' no EAX support
Const BASS_ERROR_DEVICE:Int = 23	' illegal device number
Const BASS_ERROR_NOPLAY:Int = 24	' not playing
Const BASS_ERROR_FREQ:Int = 25	' illegal sample rate
Const BASS_ERROR_NOTFILE:Int = 27	' the stream is not a file stream
Const BASS_ERROR_NOHW:Int = 29	' no hardware voices available
Const BASS_ERROR_EMPTY:Int = 31	' the MOD music has no sequence data
Const BASS_ERROR_NONET:Int = 32	' no internet connection could be opened
Const BASS_ERROR_CREATE:Int = 33	' couldn't create the file
Const BASS_ERROR_NOFX:Int = 34	' effects are not available
Const BASS_ERROR_NOTAVAIL:Int = 37	' requested data is not available
Const BASS_ERROR_DECODE:Int = 38	' the channel is a "decoding channel"
Const BASS_ERROR_DX:Int = 39	' a sufficient DirectX version is not installed
Const BASS_ERROR_TIMEOUT:Int = 40	' connection timedout
Const BASS_ERROR_FILEFORM:Int = 41	' unsupported file format
Const BASS_ERROR_SPEAKER:Int = 42	' unavailable speaker
Const BASS_ERROR_VERSION:Int = 43	' invalid BASS version (used by add-ons)
Const BASS_ERROR_CODEC:Int = 44	' codec is not available/supported
Const BASS_ERROR_ENDED:Int = 45	' the channel/file has ended
Const BASS_ERROR_UNKNOWN:Int = -1	' some other mystery problem

' BASS_SetConfig options
Const BASS_CONFIG_BUFFER:Int = 0
Const BASS_CONFIG_UPDATEPERIOD:Int = 1
Const BASS_CONFIG_GVOL_SAMPLE:Int = 4
Const BASS_CONFIG_GVOL_STREAM:Int = 5
Const BASS_CONFIG_GVOL_MUSIC:Int = 6
Const BASS_CONFIG_CURVE_VOL:Int = 7
Const BASS_CONFIG_CURVE_PAN:Int = 8
Const BASS_CONFIG_FLOATDSP:Int = 9
Const BASS_CONFIG_3DALGORITHM:Int = 10
Const BASS_CONFIG_NET_TIMEOUT:Int = 11
Const BASS_CONFIG_NET_BUFFER:Int = 12
Const BASS_CONFIG_PAUSE_NOPLAY:Int = 13
Const BASS_CONFIG_NET_PREBUF:Int = 15
Const BASS_CONFIG_NET_PASSIVE:Int = 18
Const BASS_CONFIG_REC_BUFFER:Int = 19
Const BASS_CONFIG_NET_PLAYLIST:Int = 21
Const BASS_CONFIG_MUSIC_VIRTUAL:Int = 22
Const BASS_CONFIG_VERIFY:Int = 23
Const BASS_CONFIG_UPDATETHREADS:Int = 24

' BASS_SetConfigPtr options
Const BASS_CONFIG_NET_AGENT:Int = 16
Const BASS_CONFIG_NET_PROXY:Int = 17

' Initialization flags
Const BASS_DEVICE_8BITS:Int = 1	' use 8 bit resolution, else 16 bit
Const BASS_DEVICE_MONO:Int = 2	' use mono, else stereo
Const BASS_DEVICE_3D:Int = 4	' enable 3D functionality
Const BASS_DEVICE_LATENCY:Int = 256	' calculate device latency (BASS_INFO struct)
Const BASS_DEVICE_CPSPEAKERS:Int = 1024 ' detect speakers via Windows control panel
Const BASS_DEVICE_SPEAKERS:Int = 2048 ' force enabling of speaker assignment
Const BASS_DEVICE_NOSPEAKER:Int = 4096 ' ignore speaker arrangement

' DirectSound interfaces (for use with BASS_GetDSoundObject)
Const BASS_OBJECT_DS:Int = 1	' IDirectSound
Const BASS_OBJECT_DS3DL:Int = 2	' IDirectSound3DListener

' BASS_DEVICEINFO flags
Const BASS_DEVICE_ENABLED:Int = 1
Const BASS_DEVICE_DEFAULT:Int = 2
Const BASS_DEVICE_INIT:Int = 4

' BASS_INFO flags (from DSOUND.H)
Const DSCAPS_CONTINUOUSRATE:Int = $00000010	' supports all sample rates between min/maxrate
Const DSCAPS_EMULDRIVER:Int = $00000020	' device does NOT have hardware DirectSound support
Const DSCAPS_CERTIFIED:Int = $00000040	' device driver has been certified by Microsoft
Const DSCAPS_SECONDARYMONO:Int = $00000100	' mono
Const DSCAPS_SECONDARYSTEREO:Int = $00000200	' stereo
Const DSCAPS_SECONDARY8BIT:Int = $00000400	' 8 bit
Const DSCAPS_SECONDARY16BIT:Int = $00000800	' 16 bit

' BASS_RECORDINFO flags (from DSOUND.H)
Const DSCCAPS_EMULDRIVER:Int = DSCAPS_EMULDRIVER	' device does NOT have hardware DirectSound recording support
Const DSCCAPS_CERTIFIED:Int = DSCAPS_CERTIFIED	' device driver has been certified by Microsoft

' defines for formats field of BASS_RECORDINFO (from MMSYSTEM.H)
Const WAVE_FORMAT_1M08:Int = $00000001       '* 11.025 kHz, Mono,   8-bit  *'
Const WAVE_FORMAT_1S08:Int = $00000002       '* 11.025 kHz, Stereo, 8-bit  *'
Const WAVE_FORMAT_1M16:Int = $00000004       '* 11.025 kHz, Mono,   16-bit *'
Const WAVE_FORMAT_1S16:Int = $00000008       '* 11.025 kHz, Stereo, 16-bit *'
Const WAVE_FORMAT_2M08:Int = $00000010       '* 22.05  kHz, Mono,   8-bit  *'
Const WAVE_FORMAT_2S08:Int = $00000020       '* 22.05  kHz, Stereo, 8-bit  *'
Const WAVE_FORMAT_2M16:Int = $00000040       '* 22.05  kHz, Mono,   16-bit *'
Const WAVE_FORMAT_2S16:Int = $00000080       '* 22.05  kHz, Stereo, 16-bit *'
Const WAVE_FORMAT_4M08:Int = $00000100       '* 44.1   kHz, Mono,   8-bit  *'
Const WAVE_FORMAT_4S08:Int = $00000200       '* 44.1   kHz, Stereo, 8-bit  *'
Const WAVE_FORMAT_4M16:Int = $00000400       '* 44.1   kHz, Mono,   16-bit *'
Const WAVE_FORMAT_4S16:Int = $00000800       '* 44.1   kHz, Stereo, 16-bit *'

Const BASS_SAMPLE_8BITS:Int = 1	' 8 bit
Const BASS_SAMPLE_FLOAT:Int = 256	' 32-bit floating-point
Const BASS_SAMPLE_MONO:Int = 2	' mono
Const BASS_SAMPLE_LOOP:Int = 4	' looped
Const BASS_SAMPLE_3D:Int = 8	' 3D functionality
Const BASS_SAMPLE_SOFTWARE:Int = 16	' not using hardware mixing
Const BASS_SAMPLE_MUTEMAX:Int = 32	' mute at max distance (3D only)
Const BASS_SAMPLE_VAM:Int = 64	' DX7 voice allocation & management
Const BASS_SAMPLE_FX:Int = 128	' old implementation of DX8 effects
Const BASS_SAMPLE_OVER_VOL:Int = $10000	' override lowest volume
Const BASS_SAMPLE_OVER_POS:Int = $20000	' override longest playing
Const BASS_SAMPLE_OVER_DIST:Int = $30000 ' override furthest from listener (3D only)

Const BASS_STREAM_PRESCAN:Int = $20000 ' enable pin-point seeking/length (MP3/MP2/MP1)
Const BASS_MP3_SETPOS:Int = BASS_STREAM_PRESCAN
Const BASS_STREAM_AUTOFREE:Int = $40000	' automatically free the stream when it stop/ends
Const BASS_STREAM_RESTRATE:Int = $80000	' restrict the download rate of internet file streams
Const BASS_STREAM_BLOCK:Int = $100000 ' download/play internet file stream in small blocks
Const BASS_STREAM_DECODE:Int = $200000 ' don't play the stream, only decode (BASS_ChannelGetData)
Const BASS_STREAM_STATUS:Int = $800000 ' give server status info (HTTP/ICY tags) in DOWNLOADPROC

Const BASS_MUSIC_FLOAT:Int = BASS_SAMPLE_FLOAT
Const BASS_MUSIC_MONO:Int = BASS_SAMPLE_MONO
Const BASS_MUSIC_LOOP:Int = BASS_SAMPLE_LOOP
Const BASS_MUSIC_3D:Int = BASS_SAMPLE_3D
Const BASS_MUSIC_FX:Int = BASS_SAMPLE_FX
Const BASS_MUSIC_AUTOFREE:Int = BASS_STREAM_AUTOFREE
Const BASS_MUSIC_DECODE:Int = BASS_STREAM_DECODE
Const BASS_MUSIC_PRESCAN:Int = BASS_STREAM_PRESCAN	' calculate playback length
Const BASS_MUSIC_CALCLEN:Int = BASS_MUSIC_PRESCAN
Const BASS_MUSIC_RAMP:Int = $200	' normal ramping
Const BASS_MUSIC_RAMPS:Int = $400	' sensitive ramping
Const BASS_MUSIC_SURROUND:Int = $800	' surround sound
Const BASS_MUSIC_SURROUND2:Int = $1000	' surround sound (mode 2)
Const BASS_MUSIC_FT2MOD:Int = $2000	' play .MOD as FastTracker 2 does
Const BASS_MUSIC_PT1MOD:Int = $4000	' play .MOD as ProTracker 1 does
Const BASS_MUSIC_NONINTER:Int = $10000	' non-interpolated sample mixing
Const BASS_MUSIC_SINCINTER:Int = $800000 ' sinc interpolated sample mixing
Const BASS_MUSIC_POSRESET:Int = $8000	' stop all notes when moving position
Const BASS_MUSIC_POSRESETEX:Int = $400000 ' stop all notes and reset bmp/etc when moving position
Const BASS_MUSIC_STOPBACK:Int = $80000	' stop the music on a backwards jump effect
Const BASS_MUSIC_NOSAMPLE:Int = $100000 ' don't load the samples

' Speaker assignment flags
Const BASS_SPEAKER_FRONT:Int = $1000000	' front speakers
Const BASS_SPEAKER_REAR:Int = $2000000	' rear/side speakers
Const BASS_SPEAKER_CENLFE:Int = $3000000	' center & LFE speakers (5.1)
Const BASS_SPEAKER_REAR2:Int = $4000000	' rear center speakers (7.1)
Const BASS_SPEAKER_N1:Int = $01000000
Const BASS_SPEAKER_N2:Int = $02000000
Const BASS_SPEAKER_N3:Int = $03000000
Const BASS_SPEAKER_N4:Int = $04000000
Const BASS_SPEAKER_N5:Int = $05000000
Const BASS_SPEAKER_N6:Int = $06000000
Const BASS_SPEAKER_N7:Int = $07000000
Const BASS_SPEAKER_N8:Int = $08000000
Const BASS_SPEAKER_N9:Int = $09000000
Const BASS_SPEAKER_N10:Int = $0A000000
Const BASS_SPEAKER_N11:Int = $0B000000
Const BASS_SPEAKER_N12:Int = $0C000000
Const BASS_SPEAKER_N13:Int = $0D000000
Const BASS_SPEAKER_N14:Int = $0E000000
Const BASS_SPEAKER_N15:Int = $0F000000
Const BASS_SPEAKER_LEFT:Int = $10000000	' modifier: left
Const BASS_SPEAKER_RIGHT:Int = $20000000	' modifier: right
Const BASS_SPEAKER_FRONTLEFT:Int = BASS_SPEAKER_FRONT|BASS_SPEAKER_LEFT
Const BASS_SPEAKER_FRONTRIGHT:Int = BASS_SPEAKER_FRONT|BASS_SPEAKER_RIGHT
Const BASS_SPEAKER_REARLEFT:Int = BASS_SPEAKER_REAR|BASS_SPEAKER_LEFT
Const BASS_SPEAKER_REARRIGHT:Int = BASS_SPEAKER_REAR|BASS_SPEAKER_RIGHT
Const BASS_SPEAKER_CENTER:Int = BASS_SPEAKER_CENLFE|BASS_SPEAKER_LEFT
Const BASS_SPEAKER_LFE:Int = BASS_SPEAKER_CENLFE|BASS_SPEAKER_RIGHT
Const BASS_SPEAKER_REAR2LEFT:Int = BASS_SPEAKER_REAR2|BASS_SPEAKER_LEFT
Const BASS_SPEAKER_REAR2RIGHT:Int = BASS_SPEAKER_REAR2|BASS_SPEAKER_RIGHT

Const BASS_UNICODE:Int = $80000000

Const BASS_RECORD_PAUSE:Int = $8000	' start recording paused

' DX7 voice allocation & management flags
Const BASS_VAM_HARDWARE:Int = 1
Const BASS_VAM_SOFTWARE:Int = 2
Const BASS_VAM_TERM_TIME:Int = 4
Const BASS_VAM_TERM_DIST:Int = 8
Const BASS_VAM_TERM_PRIO:Int = 16

' BASS_CHANNELINFO types
Const BASS_CTYPE_SAMPLE:Int = 1
Const BASS_CTYPE_RECORD:Int = 2
Const BASS_CTYPE_STREAM:Int = $10000
Const BASS_CTYPE_STREAM_OGG:Int = $10002
Const BASS_CTYPE_STREAM_MP1:Int = $10003
Const BASS_CTYPE_STREAM_MP2:Int = $10004
Const BASS_CTYPE_STREAM_MP3:Int = $10005
Const BASS_CTYPE_STREAM_AIFF:Int = $10006
Const BASS_CTYPE_STREAM_WAV:Int = $40000 ' WAVE flag, LOWORD=codec
Const BASS_CTYPE_STREAM_WAV_PCM:Int = $50001
Const BASS_CTYPE_STREAM_WAV_FLOAT:Int = $50003
Const BASS_CTYPE_MUSIC_MOD:Int = $20000
Const BASS_CTYPE_MUSIC_MTM:Int = $20001
Const BASS_CTYPE_MUSIC_S3M:Int = $20002
Const BASS_CTYPE_MUSIC_XM:Int = $20003
Const BASS_CTYPE_MUSIC_IT:Int = $20004
Const BASS_CTYPE_MUSIC_MO3:Int = $00100 ' MO3 flag

' BASS_ChannelGetLength/GetPosition/SetPosition modes
Const BASS_POS_BYTE:Int = 0		' byte position
Const BASS_POS_MUSIC_ORDER:Int = 1		' order.row position, MAKELONG(order,row)
Const BASS_POS_DECODE:Int = $10000000 ' flag: get the decoding (not playing) position

' BASS_ChannelIsActive return values
Const BASS_ACTIVE_STOPPED:Int = 0
Const BASS_ACTIVE_PLAYING:Int = 1
Const BASS_ACTIVE_STALLED:Int = 2
Const BASS_ACTIVE_PAUSED:Int = 3

' BASS_StreamPutFileData options
Const BASS_FILEDATA_END:Int = 0	' end & close the file

' BASS_StreamGetFilePosition modes
Const BASS_FILEPOS_CURRENT:Int = 0
Const BASS_FILEPOS_DECODE:Int = BASS_FILEPOS_CURRENT
Const BASS_FILEPOS_DOWNLOAD:Int = 1
Const BASS_FILEPOS_END:Int = 2
Const BASS_FILEPOS_START:Int = 3
Const BASS_FILEPOS_CONNECTED:Int = 4
Const BASS_FILEPOS_BUFFER:Int = 5
Const BASS_FILEPOS_SOCKET:Int = 6

' BASS_ChannelSetSync types
Const BASS_SYNC_POS:Int = 0
Const BASS_SYNC_END:Int = 2
Const BASS_SYNC_META:Int = 4
Const BASS_SYNC_SLIDE:Int = 5
Const BASS_SYNC_STALL:Int = 6
Const BASS_SYNC_DOWNLOAD:Int = 7
Const BASS_SYNC_FREE:Int = 8
Const BASS_SYNC_SETPOS:Int = 11
Const BASS_SYNC_MUSICPOS:Int = 10
Const BASS_SYNC_MUSICINST:Int = 1
Const BASS_SYNC_MUSICFX:Int = 3
Const BASS_SYNC_OGG_CHANGE:Int = 12
Const BASS_SYNC_MIXTIME:Int = $40000000	' FLAG: sync at mixtime, else at playtime
Const BASS_SYNC_ONETIME:Int = $80000000	' FLAG: sync only once, else continuously

' Channel attributes
Const BASS_ATTRIB_FREQ:Int = 1
Const BASS_ATTRIB_VOL:Int = 2
Const BASS_ATTRIB_PAN:Int = 3
Const BASS_ATTRIB_EAXMIX:Int = 4
Const BASS_ATTRIB_MUSIC_AMPLIFY:Int = $100
Const BASS_ATTRIB_MUSIC_PANSEP:Int = $101
Const BASS_ATTRIB_MUSIC_PSCALER:Int = $102
Const BASS_ATTRIB_MUSIC_BPM:Int = $103
Const BASS_ATTRIB_MUSIC_SPEED:Int = $104
Const BASS_ATTRIB_MUSIC_VOL_GLOBAL:Int = $105
Const BASS_ATTRIB_MUSIC_VOL_CHAN:Int = $200 ' + channel #
Const BASS_ATTRIB_MUSIC_VOL_INST:Int = $300 ' + instrument #

' BASS_ChannelGetData flags
Const BASS_DATA_AVAILABLE:Int = 0			' query how much data is buffered
Const BASS_DATA_FLOAT:Int = $40000000	' flag: return floating-point sample data
Const BASS_DATA_FFT256:Int = $80000000	' 256 sample FFT
Const BASS_DATA_FFT512:Int = $80000001	' 512 FFT
Const BASS_DATA_FFT1024:Int = $80000002	' 1024 FFT
Const BASS_DATA_FFT2048:Int = $80000003	' 2048 FFT
Const BASS_DATA_FFT4096:Int = $80000004	' 4096 FFT
Const BASS_DATA_FFT8192:Int = $80000005	' 8192 FFT
Const BASS_DATA_FFT_INDIVIDUAL:Int = $10	' FFT flag: FFT for each channel, else all combined
Const BASS_DATA_FFT_NOWINDOW:Int = $20	' FFT flag: no Hanning window

' BASS_ChannelGetTags types : what's returned
Const BASS_TAG_ID3:Int = 0	' ID3v1 tags : 128 byte block
Const BASS_TAG_ID3V2:Int = 1	' ID3v2 tags : variable length block
Const BASS_TAG_OGG:Int = 2	' OGG comments : series of null-terminated UTF-8 strings
Const BASS_TAG_HTTP:Int = 3	' HTTP headers : series of null-terminated ANSI strings
Const BASS_TAG_ICY:Int = 4	' ICY headers : series of null-terminated ANSI strings
Const BASS_TAG_META:Int = 5	' ICY metadata : ANSI string
Const BASS_TAG_VENDOR:Int = 9	' OGG encoder : UTF-8 string
Const BASS_TAG_LYRICS3:Int = 10	' Lyric3v2 tag : ASCII string
Const BASS_TAG_CA_CODEC:Int = 11 ' CoreAudio codec info : TAG_CA_CODEC structure
Const BASS_TAG_RIFF_INFO:Int = $100 ' RIFF/WAVE tags : series of null-terminated ANSI strings
Const BASS_TAG_RIFF_BEXT:Int = $101 ' RIFF/BWF "bext" tags : TAG_BEXT structure
Const BASS_TAG_RIFF_CART:Int = $102 ' RIFF/BWF "cart" tags : TAG_CART structure
Const BASS_TAG_MUSIC_NAME:Int = $10000	' MOD music name : ANSI string
Const BASS_TAG_MUSIC_MESSAGE:Int = $10001	' MOD message : ANSI string
Const BASS_TAG_MUSIC_ORDERS:Int = $10002	' MOD order list : BYTE array of pattern numbers
Const BASS_TAG_MUSIC_INST:Int = $10100	' + instrument #, MOD instrument name : ANSI string
Const BASS_TAG_MUSIC_SAMPLE:Int = $10300	' + sample #, MOD sample name : ANSI string

' DX8 effect types, use with BASS_ChannelSetFX
Const BASS_FX_DX8_CHORUS:Int = 0
Const BASS_FX_DX8_COMPRESSOR:Int = 1
Const BASS_FX_DX8_DISTORTION:Int = 2
Const BASS_FX_DX8_ECHO:Int = 3
Const BASS_FX_DX8_FLANGER:Int = 4
Const BASS_FX_DX8_GARGLE:Int = 5
Const BASS_FX_DX8_I3DL2REVERB:Int = 6
Const BASS_FX_DX8_PARAMEQ:Int = 7
Const BASS_FX_DX8_REVERB:Int = 8

