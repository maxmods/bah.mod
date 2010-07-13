'
' Very basic settings handler.
'
' Writes data to the stream using one line per value.
'
'
SuperStrict

Import BaH.Sfxr

' this registers it with the system. It will be used automagically by LoadSetting and SaveSettings.
New TBasicHandler


' basic implementation of the two methods...
Type TBasicHandler Extends TSfxrSettingsHandler

	Method LoadSettings:TSfxrSettings( stream:TStream )
		Local settings:TSfxrSettings = New TSfxrSettings

		settings.waveType = stream.ReadLine().ToInt()
		settings.soundVol = stream.ReadLine().toFloat()
		settings.baseFreq = stream.ReadLine().toFloat()
		settings.freqLimit = stream.ReadLine().toFloat()
		settings.freqRamp = stream.ReadLine().toFloat()
		settings.freqDRamp = stream.ReadLine().toFloat()
		settings.duty = stream.ReadLine().toFloat()
		settings.dutyRamp = stream.ReadLine().toFloat()
		
		settings.vibStrength = stream.ReadLine().toFloat()
		settings.vibSpeed = stream.ReadLine().toFloat()
		settings.vibDelay = stream.ReadLine().toFloat()
		
		settings.envAttack = stream.ReadLine().toFloat()
		settings.envSustain = stream.ReadLine().toFloat()
		settings.envDecay = stream.ReadLine().toFloat()
		settings.envPunch = stream.ReadLine().toFloat()
		
		settings.filterOn = stream.ReadLine().toInt()
		settings.lpfResonance = stream.ReadLine().toFloat()
		settings.lpfFreq = stream.ReadLine().toFloat()
		settings.lpfRamp = stream.ReadLine().toFloat()
		settings.hpfFreq = stream.ReadLine().toFloat()
		settings.hpfRamp = stream.ReadLine().toFloat()
		
		settings.phaOffset = stream.ReadLine().toFloat()
		settings.phaRamp = stream.ReadLine().toFloat()
		
		settings.repeatSpeed = stream.ReadLine().toFloat()
		
		settings.arpSpeed = stream.ReadLine().toFloat()
		settings.arpMod = stream.ReadLine().toFloat()		
		
		Return settings
	End Method
	
	Method SaveSettings:Int( stream:TStream, settings:TSfxrSettings )

		stream.WriteLine(settings.waveType)
		stream.WriteLine(settings.soundVol)
		stream.WriteLine(settings.baseFreq)
		stream.WriteLine(settings.freqLimit)
		stream.WriteLine(settings.freqRamp)
		stream.WriteLine(settings.freqDRamp)
		stream.WriteLine(settings.duty)
		stream.WriteLine(settings.dutyRamp)
		
		stream.WriteLine(settings.vibStrength)
		stream.WriteLine(settings.vibSpeed)
		stream.WriteLine(settings.vibDelay)
		
		stream.WriteLine(settings.envAttack)
		stream.WriteLine(settings.envSustain)
		stream.WriteLine(settings.envDecay)
		stream.WriteLine(settings.envPunch)
		
		stream.WriteLine(settings.filterOn)
		stream.WriteLine(settings.lpfResonance)
		stream.WriteLine(settings.lpfFreq)
		stream.WriteLine(settings.lpfRamp)
		stream.WriteLine(settings.hpfFreq)
		stream.WriteLine(settings.hpfRamp)
		
		stream.WriteLine(settings.phaOffset)
		stream.WriteLine(settings.phaRamp)
		
		stream.WriteLine(settings.repeatSpeed)
		
		stream.WriteLine(settings.arpSpeed)
		stream.WriteLine(settings.arpMod)	

	End Method

End Type
