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
bbdoc: BASS FX - Real-time DSP
End Rem
Module BaH.Bass_FX

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"
ModuleInfo "Copyright: BASS FX - 2002-2008 JOBnik!"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?macos
ModuleInfo "LD_OPTS: -L%PWD%/lib/macos"
?

?Not Linux
Import BaH.Bass

Import "common.bmx"


Rem
bbdoc: A resampling stream.
End Rem
Type TBassTempo Extends TBassStream

	Field source:TBassChannel

	Rem
	bbdoc: 
	End Rem
	Function BassTempoCreate:TBassTempo(channel:TBassChannel, flags:Int)
		Return New TBassTempo.TempoCreate(channel, flags)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method TempoCreate:TBassTempo(channel:TBassChannel, flags:Int)
		handle = BASS_FX_TempoCreate(channel.handle, flags)
		If Not handle Then
			Return Null
		End If
		source = channel
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetRateRatio:Float()
		Return BASS_FX_TempoGetRateRatio(handle)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetSource:TBassChannel()
		Local sourceHandle:Int = BASS_FX_TempoGetSource(handle)
		If source.handle = sourceHandle Then
			Return source
		End If
		Return TBassChannel._create(sourceHandle)
	End Method

End Type

Rem
bbdoc: This is an echo effect that replays what you have played one or more times after a period of time.
about: It's something like the echoes you might hear shouting against a canyon wall.
End Rem
Type TBassFXEcho Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxecho_new()
	End Method
	
	Rem
	bbdoc: Returns the volume of the signal.
	End Rem
	Method GetLevel:Float()
		Return bmx_bassfxecho_getlevel(fxPtr)
	End Method
	
	Rem
	bbdoc: Sets the volume of the signal.
	about: Echo level [0....1....n] linear
	End Rem
	Method SetLevel(level:Float)
		bmx_bassfxecho_setlevel(fxPtr, level)
	End Method
	
	Rem
	bbdoc: Returns the delay time in ms.
	End Rem
	Method GetDelay:Int()
		Return bmx_bassfxecho_getdelay(fxPtr)
	End Method
	
	Rem
	bbdoc: Sets the delay time in ms.
	about: Delay in ms [1200..30000] ms 
	End Rem
	Method SetDelay(dlay:Int)
		bmx_bassfxecho_setdelay(fxPtr, dlay)
	End Method
	
End Type

Rem
bbdoc: Flangers mix a varying delayed signal (usually about 5mS to 15mS) with the original to produce a series of notches in the frequency response.
about: The important difference between flanging and phasing is that a flanger produces a large number of
notches that are harmonically (musically) related, while a phaser produces a small number of notches that
are evenly spread across the frequency spectrum. With high resonance, you get the "jet plane" effect.
End Rem
Type TBassFXFlanger Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxflanger_new()
	End Method
	
	Rem
	bbdoc: Returns the ratio of wet signal to dry.
	End Rem
	Method GetWetDry:Float()
		Return bmx_bassfxflanger_getwetdry(fxPtr)
	End Method
	
	Rem
	bbdoc: Sets the ratio of wet signal to dry.
	about: Ratio of wet (processed) signal to dry (unprocessed) signal [0..1..n] linear
	End Rem
	Method SetWetDry(wetDry:Float)
		bmx_bassfxflanger_setwetdry(fxPtr, wetdry)
	End Method
	
	Rem
	bbdoc: Returns the flanger speed in ms.
	End Rem
	Method GetSpeed:Float()
		Return bmx_bassfxflanger_getspeed(fxPtr)
	End Method
	
	Rem
	bbdoc: Sets the flanger speed in ms.
	about: Flanger speed in ms [0..0.09] ms
	End Rem
	Method SetSpeed(speed:Float)
		bmx_bassfxflanger_setspeed(fxPtr, speed)
	End Method
	
	Rem
	bbdoc: Returns the affected channels.
	End Rem
	Method GetChannel:Int()
		Return bmx_bassfxflanger_getchannel(fxPtr)
	End Method
	
	Rem
	bbdoc: Sets the affected channels.
	End Rem
	Method SetChannel(channel:Int)
		bmx_bassfxflanger_setchannel(fxPtr, channel)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TBassFXVolume Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxvolume_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXPeakEQ Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxpeakeq_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXReverb Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxreverb_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXLPF Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxlpf_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXMix Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxmix_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXDamp Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxdamp_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXAutoWah Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxautowah_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXEcho2 Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxecho2_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXPhaser Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxphaser_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXEcho3 Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxecho3_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXChorus Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxchorus_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXAPF Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxapf_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXCompressor Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxcompressor_new()
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TBassFXDistortion Extends TBassFXType
	Method New()
		fxPtr = bmx_bass_fxdistortion_new()
	End Method
End Type

New TBassFXFXFactory

Type TBassFXFXFactory Extends TBassFXFactory

	Method FXForType:TBassFXType(fxType:Int)
		Select fxType
			Case BASS_FX_BFX_ROTATE
			Case BASS_FX_BFX_ECHO
				Return New TBassFXEcho
			Case BASS_FX_BFX_FLANGER
				Return New TBassFXFlanger
			Case BASS_FX_BFX_VOLUME
				Return New TBassFXVolume
			Case BASS_FX_BFX_PEAKEQ
				Return New TBassFXPeakEQ
			Case BASS_FX_BFX_REVERB
				Return New TBassFXReverb
			Case BASS_FX_BFX_LPF
				Return New TBassFXLPF
			Case BASS_FX_BFX_MIX
				Return New TBassFXMix
			Case BASS_FX_BFX_DAMP
				Return New TBassFXDamp
			Case BASS_FX_BFX_AUTOWAH
				Return New TBassFXAutoWah
			Case BASS_FX_BFX_ECHO2
				Return New TBassFXEcho2
			Case BASS_FX_BFX_PHASER
				Return New TBassFXPhaser
			Case BASS_FX_BFX_ECHO3
				Return New TBassFXEcho3
			Case BASS_FX_BFX_CHORUS
				Return New TBassFXChorus
			Case BASS_FX_BFX_APF
				Return New TBassFXAPF
			Case BASS_FX_BFX_COMPRESSOR
				Return New TBassFXCompressor
			Case BASS_FX_BFX_DISTORTION
				Return New TBassFXDistortion
		End Select
	End Method	

End Type

?
