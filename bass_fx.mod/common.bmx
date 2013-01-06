' Copyright (c) 2008-2013 Bruce A Henderson
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

?win32
Import "lib/win32/bass_fx.lib"
?macos
Import "-lbass_fx"
?

Import "include/*.h"

Import "glue.cpp"

Extern

	Function bmx_bass_fxecho_new:Byte Ptr()
	Function bmx_bass_fxflanger_new:Byte Ptr()
	Function bmx_bass_fxvolume_new:Byte Ptr()
	Function bmx_bass_fxpeakeq_new:Byte Ptr()
	Function bmx_bass_fxreverb_new:Byte Ptr()
	Function bmx_bass_fxlpf_new:Byte Ptr()
	Function bmx_bass_fxmix_new:Byte Ptr()
	Function bmx_bass_fxdamp_new:Byte Ptr()
	Function bmx_bass_fxautowah_new:Byte Ptr()
	Function bmx_bass_fxecho2_new:Byte Ptr()
	Function bmx_bass_fxphaser_new:Byte Ptr()
	Function bmx_bass_fxecho3_new:Byte Ptr()
	Function bmx_bass_fxchorus_new:Byte Ptr()
	Function bmx_bass_fxapf_new:Byte Ptr()
	Function bmx_bass_fxcompressor_new:Byte Ptr()
	Function bmx_bass_fxdistortion_new:Byte Ptr()

	Function bmx_bassfxecho_getlevel:Float(handle:Byte Ptr)
	Function bmx_bassfxecho_setlevel(handle:Byte Ptr, level:Float)
	Function bmx_bassfxecho_getdelay:Int(handle:Byte Ptr)
	Function bmx_bassfxecho_setdelay(handle:Byte Ptr, dlay:Int)

	Function bmx_bassfxflanger_getwetdry:Float(handle:Byte Ptr)
	Function bmx_bassfxflanger_setwetdry(handle:Byte Ptr, wetdry:Float)
	Function bmx_bassfxflanger_getspeed:Float(handle:Byte Ptr)
	Function bmx_bassfxflanger_setspeed(handle:Byte Ptr, speed:Float)
	Function bmx_bassfxflanger_getchannel:Int(handle:Byte Ptr)
	Function bmx_bassfxflanger_setchannel(handle:Byte Ptr, channel:Int)

End Extern

?win32
Extern "win32"
?macos
Extern
?

	Function BASS_FX_TempoCreate:Int(channel:Int, flags:Int)
	Function BASS_FX_TempoGetRateRatio:Float(handle:Int)
	Function BASS_FX_TempoGetSource:Int(handle:Int)

	Function BASS_FX_ReverseCreate:Int(chan:Int, decBlock:Float, flags:Int)
	
End Extern

' Error codes returned by BASS_ErrorGetCode()
Const BASS_ERROR_FX_NODECODE:Int = 4000	' Not a decoding channel
Const BASS_ERROR_FX_BPMINUSE:Int = 4001	' BPM/Beat detection is in use

' Tempo / Reverse / BPM / Beat flag
Const BASS_FX_FREESOURCE:Int = $10000	' Free the source handle as well?

' DSP channels flags
Const BASS_BFX_CHANALL:Int = -1		' all channels at once (as by default)
Const BASS_BFX_CHANNONE:Int = 0		' disable an effect for all channels
Const BASS_BFX_CHAN1:Int = 1		' left-front channel
Const BASS_BFX_CHAN2:Int = 2		' right-front channel
Const BASS_BFX_CHAN3:Int = 4		' see above info
Const BASS_BFX_CHAN4:Int = 8		' see above info
Const BASS_BFX_CHAN5:Int = 16		' see above info
Const BASS_BFX_CHAN6:Int = 32		' see above info
Const BASS_BFX_CHAN7:Int = 64		' see above info
Const BASS_BFX_CHAN8:Int = 128		' see above info

' if you have more than 8 channels, use one of these
Const BASS_BFX_CHANNEL_N9:Int = 256
Const BASS_BFX_CHANNEL_N10:Int = 512
Const BASS_BFX_CHANNEL_N11:Int = 1024
Const BASS_BFX_CHANNEL_N12:Int = 2048
Const BASS_BFX_CHANNEL_N13:Int = 4096
Const BASS_BFX_CHANNEL_N14:Int = 8192
Const BASS_BFX_CHANNEL_N15:Int = 16384

' DSP effects
Const BASS_FX_BFX_ROTATE:Int = $10000				' A channels volume ping-pong	/ stereo
Const BASS_FX_BFX_ECHO:Int = $10001				' Echo							/ 2 channels max
Const BASS_FX_BFX_FLANGER:Int = $10002			' Flanger						/ multi channel
Const BASS_FX_BFX_VOLUME:Int = $10003				' Volume						/ multi channel
Const BASS_FX_BFX_PEAKEQ:Int = $10004				' Peaking Equalizer			/ multi channel
Const BASS_FX_BFX_REVERB:Int = $10005				' Reverb						/ 2 channels max
Const BASS_FX_BFX_LPF:Int = $10006				' Low Pass Filter				/ multi channel
Const BASS_FX_BFX_MIX:Int = $10007				' Swap, remap and mix channels	/ multi channel
Const BASS_FX_BFX_DAMP:Int = $10008				' Dynamic Amplification		/ multi channel
Const BASS_FX_BFX_AUTOWAH:Int = $10009			' Auto WAH						/ multi channel
Const BASS_FX_BFX_ECHO2:Int = $1000A				' Echo 2						/ multi channel
Const BASS_FX_BFX_PHASER:Int = $1000B				' Phaser						/ multi channel
Const BASS_FX_BFX_ECHO3:Int = $1000C				' Echo 3						/ multi channel
Const BASS_FX_BFX_CHORUS:Int = $1000D				' Chorus						/ multi channel
Const BASS_FX_BFX_APF:Int = $1000E				' All Pass Filter				/ multi channel
Const BASS_FX_BFX_COMPRESSOR:Int = $1000F			' Compressor					/ multi channel
Const BASS_FX_BFX_DISTORTION:Int = $10010			' Distortion					/ multi channel

' tempo attributes (BASS_ChannelSet/GetAttribute)
Const BASS_ATTRIB_TEMPO:Int = $10000
Const BASS_ATTRIB_TEMPO_PITCH:Int = $10001
Const BASS_ATTRIB_TEMPO_FREQ:Int = $10002

' tempo attributes options
'		[option]											[value]
Const BASS_ATTRIB_TEMPO_OPTION_USE_AA_FILTER:Int = $10011	' TRUE / FALSE
Const BASS_ATTRIB_TEMPO_OPTION_AA_FILTER_LENGTH:Int = $10012			' 32 default (8 .. 128 taps)
Const BASS_ATTRIB_TEMPO_OPTION_USE_QUICKALGO:Int = $10013				' TRUE / FALSE
Const BASS_ATTRIB_TEMPO_OPTION_SEQUENCE_MS:Int = $10014				' 82 default
Const BASS_ATTRIB_TEMPO_OPTION_SEEKWINDOW_MS:Int = $10015				' 14 default
Const BASS_ATTRIB_TEMPO_OPTION_OVERLAP_MS:Int = $10016					' 12 default

' reverse attribute (BASS_ChannelSet/GetAttribute)
Const BASS_ATTRIB_REVERSE_DIR:Int = $11000

' playback directions
Const BASS_FX_RVS_REVERSE:Int = -1
Const BASS_FX_RVS_FORWARD:Int = 1

' bpm flags
Const BASS_FX_BPM_BKGRND:Int = 1	' if in use, then you can do other processing while detection's in progress. Not available in MacOSX yet. (BPM/Beat)
Const BASS_FX_BPM_MULT2:Int = 2	' if in use, then will auto multiply bpm by 2 (if BPM < minBPM*2)

' translation options
Const BASS_FX_BPM_TRAN_X2:Int = 0		' multiply the original BPM value by 2 (may be called only once & will change the original BPM as well!)
Const BASS_FX_BPM_TRAN_2FREQ:Int = 1		' BPM value to Frequency
Const BASS_FX_BPM_TRAN_FREQ2:Int = 2		' Frequency to BPM value
Const BASS_FX_BPM_TRAN_2PERCENT:Int = 3	' BPM value to Percents
Const BASS_FX_BPM_TRAN_PERCENT2:Int = 4	' Percents to BPM value

