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

Rem
bbdoc: Sound FX Generator
End Rem
Module BaH.sfxr

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: sfxr - 2007 Tomas Pettersson (http://www.drpetter.se)"
ModuleInfo "Copyright: wrapper - 2010 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"


Import BRL.Bank
Import BRL.Stream

Import "common.bmx"

Rem
bbdoc: Sound FX Generator
about: Generates TBank audio data from TSfxrSettings.
End Rem
Type TSfxr

	Field sfxrPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new Sfxr instance.
	End Rem
	Function Create:TSfxr()
		Local this:TSfxr = New TSfxr
		this.sfxrPtr = bmx_sfxr_new()
		Return this
	End Function
	
	Rem
	bbdoc: Resets the settings to their default values.
	End Rem
	Method Reset()
		bmx_sfxr_resetparams(sfxrPtr)
	End Method

	Rem
	bbdoc: Retrieves the current sfx settings.
	about: Pass in an existing TSfxrSettings object to have it populated instead of creating a new one.
	End Rem
	Method GetSettings:TSfxrSettings(settings:TSfxrSettings = Null)
		If settings Then
			Return TSfxrSettings(bmx_sfxr_getsettings(sfxrPtr, settings))
		Else
			Return TSfxrSettings(bmx_sfxr_getsettings(sfxrPtr, Null))
		End If
	End Method
	
	Rem
	bbdoc: Sets the sfx settings to @settings.
	End Rem
	Method SetSettings(settings:TSfxrSettings)
	
		bmx_sfxr_setsettings(sfxrPtr, settings.waveType, settings.soundVol, settings.baseFreq, settings.freqLimit, settings.freqRamp, settings.freqDRamp, ..
			settings.duty, settings.dutyRamp, settings.vibStrength, settings.vibSpeed, settings.vibDelay, settings.envAttack, settings.envSustain, ..
			settings.envDecay, settings.envPunch, settings.filterOn, settings.lpfResonance, settings.lpfFreq, settings.lpfRamp, settings.hpfFreq, ..
			settings.hpfRamp, settings.phaOffset, settings.phaRamp, settings.repeatSpeed, settings.arpSpeed, settings.arpMod)
			
	End Method
	
	Rem
	bbdoc: Saves @settings to @stream.
	about: Note : There is no default TSfxrSettingsHandler. See the basic_handler example for ideas.
	End Rem
	Method SaveSettings(stream:TStream, settings:TSfxrSettings = Null)
	
		If Not settings Then
			settings = GetSettings()
		End If
		
		Local saver:TSfxrSettingsHandler = sfxr_handlers
	
		Local result:Int
		While saver
			Try
				result = saver.SaveSettings( stream, settings )
			Catch ex:TStreamException
			End Try
			If result Exit
			saver=saver._succ
		Wend
		
	End Method

	Rem
	bbdoc: Loads settings from @stream.
	about: If @setAsCurrent is True, also override the current sfx settings with the new data.
	Note : There is no default TSfxrSettingsHandler. See the basic_handler example for ideas.
	End Rem
	Method LoadSettings:TSfxrSettings(stream:TStream, setAsCurrent:Int = False)	
		Local settings:TSfxrSettings
		Local loader:TSfxrSettingsHandler = sfxr_handlers
	
		While loader
			Try
				settings = loader.LoadSettings( stream )
			Catch ex:TStreamException
			End Try
			If settings Exit
			loader=loader._succ
		Wend
		
		If setAsCurrent Then
			SetSettings(settings)
		End If
		
		Return settings
	End Method
	
	Rem
	bbdoc: Generate the PCM data.
	about: @soundType should be one of SND_FLOAT (32-bit), SND_SHORT (16-bit) or SND_BYTE (8-bit).
	@freq should be one of FREQ_44100 or FREQ_22050.
	End Rem
	Method GenerateSoundData:TBank(soundType:Int = 0, freq:Int = FREQ_44100)
		Local size:Int = bmx_sfxr_calculatesize(sfxrPtr, freq)
		
		Local sof:Int = 1
		Select soundType
			Case 0 ' 32 bit float
				sof = 4
			Case 1 ' 16 bit int
				sof = 2
			Case 2 ' 8 bit int
				sof = 1
		End Select
		
		Local bsize:Int = size
		
		' shrink for lower frequency?
		If freq = FREQ_22050 Then
			bsize :/ 2
		End If
		
		Local bank:TBank = TBank.Create(bsize * sof)
		
		bmx_sfxr_generatesound(sfxrPtr, bank.Lock(), size, soundType, freq)
		bank.Unlock()

		Return bank
	End Method
	
	Rem
	bbdoc: Randomize the sfx settings.
	End Rem
	Method Randomize()
		bmx_sfxr_randomize(sfxrPtr)
	End Method
	
	Rem
	bbdoc: Mutate the current sfx settings.
	End Rem
	Method Mutate()
		bmx_sfxr_mutate(sfxrPtr)
	End Method
	
End Type

Rem
bbdoc: Sound FX Settings
End Rem
Type TSfxrSettings

	Rem
	bbdoc: SQUAREWAVE (0), SAWTOOTH (1), SINEWAVE (2), or NOISE (3)
	End Rem
	Field waveType:Int
	Rem
	bbdoc: VOLUME
	End Rem
	Field soundVol:Float
	Rem
	bbdoc: START FREQUENCY
	End Rem
	Field baseFreq:Float
	Rem
	bbdoc: MIN FREQUENCY
	End Rem
	Field freqLimit:Float
	Rem
	bbdoc: SLIDE
	End Rem
	Field freqRamp:Float
	Rem
	bbdoc: DELTA SLIDE
	End Rem
	Field freqDRamp:Float
	Rem
	bbdoc: SQUARE DUTY
	End Rem
	Field duty:Float
	Rem
	bbdoc: DUTY SWEEP
	End Rem
	Field dutyRamp:Float
	
	Rem
	bbdoc: VIBRATO DEPTH
	End Rem
	Field vibStrength:Float
	Rem
	bbdoc: VIBRATO SPEED
	End Rem
	Field vibSpeed:Float
	Rem
	bbdoc: 
	End Rem
	Field vibDelay:Float
	
	Rem
	bbdoc: ATTACK TIME
	End Rem
	Field envAttack:Float
	Rem
	bbdoc: SUSTAIN TIME
	End Rem
	Field envSustain:Float
	Rem
	bbdoc: DECAY TIME
	End Rem
	Field envDecay:Float
	Rem
	bbdoc: SUSTAIN PUNCH
	End Rem
	Field envPunch:Float
	
	Rem
	bbdoc: 
	End Rem
	Field filterOn:Int
	Rem
	bbdoc: LP FILTER RESONANCE
	End Rem
	Field lpfResonance:Float
	Rem
	bbdoc: LP FILTER CUTOFF
	End Rem
	Field lpfFreq:Float
	Rem
	bbdoc: LP FILTER CUTOFF SWEEP
	End Rem
	Field lpfRamp:Float
	Rem
	bbdoc: HP FILTER CUTOFF
	End Rem
	Field hpfFreq:Float
	Rem
	bbdoc: HP FILTER CUTOFF SWEEP
	End Rem
	Field hpfRamp:Float
	
	Rem
	bbdoc: PHASER OFFSET
	End Rem
	Field phaOffset:Float
	Rem
	bbdoc: PHASER SWEEP
	End Rem
	Field phaRamp:Float
	
	Rem
	bbdoc: REPEAT SPEED
	End Rem
	Field repeatSpeed:Float
	
	Rem
	bbdoc: CHANGE SPEED
	End Rem
	Field arpSpeed:Float
	Rem
	bbdoc: CHANGE AMOUNT
	End Rem
	Field arpMod:Float
	
	Function _create:TSfxrSettings(settings:TSfxrSettings, waveType:Int, soundVol:Float, baseFreq:Float, freqLimit:Float, freqRamp:Float, freqDRamp:Float, ..
			duty:Float, dutyRamp:Float, vibStrength:Float, vibSpeed:Float, vibDelay:Float, envAttack:Float, envSustain:Float, ..
			envDecay:Float, envPunch:Float, filterOn:Int, lpfResonance:Float, lpfFreq:Float, lpfRamp:Float, hpfFreq:Float, ..
			hpfRamp:Float, phaOffset:Float, phaRamp:Float, repeatSpeed:Float, arpSpeed:Float, arpMod:Float)
		
		Local this:TSfxrSettings
		
		If settings Then
			this = settings
		Else
			this = New TSfxrSettings
		End If

		this.waveType = waveType
		this.soundVol = soundVol
		this.baseFreq = baseFreq
		this.freqLimit = freqLimit
		this.freqRamp = freqRamp
		this.freqDRamp = freqDRamp
		this.duty = duty
		this.dutyRamp = dutyRamp
		
		this.vibStrength = vibStrength
		this.vibSpeed = vibSpeed
		this.vibDelay = vibDelay
		
		this.envAttack = envAttack
		this.envSustain = envSustain
		this.envDecay = envDecay
		this.envPunch = envPunch
		
		this.filterOn = filterOn
		this.lpfResonance = lpfResonance
		this.lpfFreq = lpfFreq
		this.lpfRamp = lpfRamp
		this.hpfFreq = hpfFreq
		this.hpfRamp = hpfRamp
		
		this.phaOffset = phaOffset
		this.phaRamp = phaRamp
		
		this.repeatSpeed = repeatSpeed
		
		this.arpSpeed = arpSpeed
		this.arpMod = arpMod		
	
		Return this
	End Function

End Type




Private

Global sfxr_handlers:TSfxrSettingsHandler

Public

Rem
bbdoc: Handler for loading and saving settings via streams.
End Rem
Type TSfxrSettingsHandler
	Field _succ:TSfxrSettingsHandler
	
	Method New()
		_succ = sfxr_handlers
		sfxr_handlers = Self
	End Method
	
	Rem
	bbdoc: Loads settings data from @stream.
	End Rem
	Method LoadSettings:TSfxrSettings( stream:TStream ) Abstract
	
	Rem
	bbdoc: Saves @settings to @stream.
	End Rem
	Method SaveSettings:Int( stream:TStream, settings:TSfxrSettings ) Abstract
	
End Type

