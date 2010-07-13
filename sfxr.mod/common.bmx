' Copyright (c) 2010 Bruce A Henderson
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
Import "glue.cpp"

Extern
	
	Function bmx_sfxr_new:Byte Ptr()
	Function bmx_sfxr_resetparams(handle:Byte Ptr)
	Function bmx_sfxr_getsettings:Object(handle:Byte Ptr, settings:Object)
	Function bmx_sfxr_setsettings(handle:Byte Ptr, waveType:Int, soundVol:Float, baseFreq:Float, freqLimit:Float, freqRamp:Float, freqDRamp:Float, ..
			duty:Float, dutyRamp:Float, vibStrength:Float, vibSpeed:Float, vibDelay:Float, envAttack:Float, envSustain:Float, ..
			envDecay:Float, envPunch:Float, filterOn:Int, lpfResonance:Float, lpfFreq:Float, lpfRamp:Float, hpfFreq:Float, ..
			hpfRamp:Float, phaOffset:Float, phaRamp:Float, repeatSpeed:Float, arpSpeed:Float, arpMod:Float)
	
	Function bmx_sfxr_calculatesize:Int(handle:Byte Ptr, freq:Int)
	Function bmx_sfxr_generatesound(handle:Byte Ptr, data:Byte Ptr, length:Int, dataType:Int, freq:Int)
	Function bmx_sfxr_randomize(handle:Byte Ptr)
	Function bmx_sfxr_mutate(handle:Byte Ptr)

End Extern

Rem
bbdoc: 32-bit float sound data.
End Rem
Const SND_FLOAT:Int = 0
Rem
bbdoc: 16-bit sound data.
End Rem
Const SND_SHORT:Int = 1
Rem
bbdoc: 8-bit sound data.
End Rem
Const SND_BYTE:Int = 2

Rem
bbdoc: 44100 hz
End Rem
Const FREQ_44100:Int = 44100
Rem
bbdoc: 22050 hz
End Rem
Const FREQ_22050:Int = 22050
