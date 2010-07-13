'
' SFXR UI
'
' BlitzMax Port, Copyright (c) 2010 Bruce A Henderson
' Original C++ version, Copyright (c) 2007 Tomas Pettersson
'
'
'
'
SuperStrict

Framework BaH.Sfxr
Import BRL.GLMax2D
Import BRL.FreeTypeFont
Import BRL.FileSystem
Import BRL.FreeAudioAudio
Import BRL.RamStream
Import BRL.Random

Import "basic_handler.bmx"

Incbin "fffharmo.ttf"

AppTitle = "sfxr"

' defaults never seem to work here, so set to something that does for me..
' your mileage of course, may vary :-)
?win32
SetAudioDriver("FreeAudio DirectSound")
?

SeedRnd(MilliSecs())

Graphics 640, 480, 0
Local font:TImageFont = LoadImageFont("incbin::fffharmo.ttf", 10)


SetClsColor $C0, $B0, $90


New TGenerator.run()

End

Type TGenerator

	Field mx:Int, px:Int
	Field my:Int, py:Int
	Field mouseLeft:Int, pLeft:Int
	Field mouseRight:Int, pRight:Int
	Field mouseLeftClick:Int
	Field mouseRightClick:Int
	
	Field sfx:TSfxr
	Field settings:TSfxrSettings = New TSfxrSettings
	
	Field vselected:Float Ptr
	Field vcurbutton:Int
	
	Field black:TColor = TColor.Set(0,0,0)
	
	Field playing:Int
	Field bank:TBank
	Field channel:TChannel
	
	Field wavFreq:Int = 44100
	Field wavBits:Int = 16

	Method run()
		sfx = TSfxr.Create()
	
		While Not KeyHit(KEY_ESCAPE)
		
			Cls
		
			Update()
		
			DrawScreen()	
			
			Flip
		
		Wend
	End Method
	
	Method Update()
		px = mx
		py = my
		pLeft = mouseLeft
		pRight = mouseRight
		
		mx = MouseX()
		my = MouseY()
		mouseLeft = MouseDown(1)
		mouseRight = MouseDown(2)
		
		mouseLeftClick = mouseLeft And Not pLeft
		mouseRightClick = mouseRIght And Not pRight

		settings = sfx.GetSettings(settings)
		
		playing = False
		If channel And channel.Playing() Then
			playing = True
		End If
	End Method
	
	Method DrawScreen()
	
		Global categories:String[] = ["PICKUP/COIN", "LASER/SHOOT", "EXPLOSION", "POWERUP", "HIT/HURT", "JUMP", "BLIP/SELECT"]	


		If Not mouseLeft Then
			vselected = Null
		End If

		DrawTextColor 10, 10, TColor.Set($50, $40, $30), "GENERATOR"
		
		For Local i:Int = 0 Until 7
			If Button(5, 35+i*30, False, categories[i], 300+i) Then
			
				Select i
					Case 0 'pickup/coin
						sfx.Reset()
						sfx.GetSettings(settings)
						
						settings.baseFreq = 0.4 + frnd(0.5)
						settings.envAttack = 0.0
						settings.envSustain = frnd(0.1)
						settings.envDecay = 0.1 + frnd(0.4)
						settings.envPunch = 0.3 + frnd(0.3)
						If Rand(0)
							settings.arpSpeed = 0.5 + frnd(0.2)
							settings.arpMod = 0.2 + frnd(0.4)
						End If
						
					Case 1 ' laser/shoot
						sfx.Reset()
						sfx.GetSettings(settings)
						
						settings.waveType = Rand(0, 2)
						If settings.waveType = 2 And Rand(0) Then
							settings.waveType = Rand(0)
						End If
						settings.baseFreq = 0.5 + frnd(0.5)
						settings.freqLimit = settings.baseFreq - 0.2 - frnd(0.6)
						If settings.freqLimit < 0.2 Then
							settings.freqLimit=0.2
						End If
						settings.freqRamp = -0.15 - frnd(0.2)
						If Rand(0, 2)=0 Then
							settings.baseFreq = 0.3 + frnd(0.6)
							settings.freqLimit = frnd(0.1)
							settings.freqRamp = -0.35 - frnd(0.3)
						End If
						
						If Rand(0) Then
							settings.duty = frnd(0.5)
							settings.dutyRamp = frnd(0.2)
						Else
							settings.duty = 0.4 + frnd(0.5)
							settings.dutyRamp = -frnd(0.7)
						End If
						settings.envAttack=0.0
						settings.envSustain = 0.1 + frnd(0.2)	
						settings.envDecay = frnd(0.4)
						If Rand(0) Then
							settings.envPunch = frnd(0.3)
						End If
						If Rand(0, 2) = 0 Then
							settings.phaOffset = frnd(0.2)
							settings.phaRamp = -frnd(0.2)
						End If
						If Rand(0) Then
							settings.hpfFreq = frnd(0.3)
						End If
						
					Case 2 ' explosion
						sfx.Reset()
						sfx.GetSettings(settings)
						
						settings.waveType = 3
						If Rand(0) Then
							settings.baseFreq = 0.1 + frnd(0.4)
							settings.freqRamp = -0.1 + frnd(0.4)
						Else
							settings.baseFreq = 0.2 + frnd(0.7)
							settings.freqRamp = -0.2 - frnd(0.2)
						End If
						settings.baseFreq :* settings.baseFreq
						If Rand(0, 4) = 0 Then
							settings.freqRamp = 0.0
						End If
						If Rand(0, 2) = 0 Then
							settings.repeatSpeed = 0.3+frnd(0.5)
						End If
						settings.envAttack = 0.0
						settings.envSustain = 0.1 + frnd(0.3)
						settings.envDecay = frnd(0.5)
						If Rand(0) = 0 Then
							settings.phaOffset = -0.3 + frnd(0.9)
							settings.phaRamp = -frnd(0.3)
						End If
						settings.envPunch = 0.2 + frnd(0.6)
						If Rand(0) Then
							settings.vibStrength = frnd(0.7)
							settings.vibSpeed = frnd(0.6)
						End If
						If Rand(0, 2) = 0 Then
							settings.arpSpeed = 0.6 + frnd(0.3)
							settings.arpMod = 0.8 - frnd(1.6)
						End If
		
					Case 3 ' powerup
						sfx.Reset()
						sfx.GetSettings(settings)
						
						If Rand(0) Then
							settings.waveType = 1
						Else
							settings.duty = frnd(0.6)
						End If
						If Rand(0) Then
							settings.baseFreq = 0.2 + frnd(0.3)
							settings.freqRamp = 0.1 + frnd(0.4)
							settings.repeatSpeed = 0.4 + frnd(0.4)
						Else
							settings.baseFreq=0.2 + frnd(0.3)
							settings.freqRamp=0.05 + frnd(0.2)
							If Rand(1) Then
								settings.vibStrength = frnd(0.7)
								settings.vibSpeed = frnd(0.6)
							End If
						End If
						settings.envAttack = 0.0
						settings.envSustain = frnd(0.4)
						settings.envDecay = 0.1 + frnd(0.4)

					Case 4 ' hit/hurt
						sfx.Reset()
						sfx.GetSettings(settings)
						
						settings.waveType = Rand(0,2)
						If settings.waveType = 2
							settings.waveType = 3
						End If
						If settings.waveType = 0 Then
							settings.duty = frnd(0.6)
						End If
						settings.baseFreq = 0.2 + frnd(0.6)
						settings.freqRamp = -0.3 - frnd(0.4)
						settings.envAttack = 0.0
						settings.envSustain = frnd(0.1)
						settings.envDecay= 0.1 + frnd(0.2)
						If Rand(0) Then
							settings.hpfFreq = frnd(0.3)
						End If
		
					Case 5 ' jump
						sfx.Reset()
						sfx.GetSettings(settings)

						settings.waveType = 0
						settings.duty = frnd(0.6)
						settings.baseFreq = 0.3 + frnd(0.3)
						settings.freqRamp = 0.1 + frnd(0.2)
						settings.envAttack = 0.0
						settings.envSustain = 0.1 + frnd(0.3)
						settings.envDecay = 0.1 + frnd(0.2)
						If Rand(0) Then
							settings.hpfFreq = frnd(0.3)
						End If
						If Rand(0) Then
							settings.lpfFreq = 1.0 - frnd(0.6)
						End If

					Case 6 ' blip/select
						sfx.Reset()
						sfx.GetSettings(settings)
						
						settings.waveType = Rand(0)
						If settings.waveType = 0 Then
							settings.duty = frnd(0.6)
						End If
						settings.baseFreq = 0.2 + frnd(0.4)
						settings.envAttack = 0.0
						settings.envSustain = 0.1 + frnd(0.1)
						settings.envDecay = frnd(0.2)
						settings.hpfFreq = 0.1
		
				End Select
			
			End If
		
		Next

		DrawBar(110, 0, 2, 480)
		DrawTextColor(120, 10, TColor.Set($50, $40, $30), "MANUAL SETTINGS")


		If Button(130, 30, settings.waveType=0, "SQUAREWAVE", 10) Then
			settings.waveType = 0
		End If
		
		If Button(250, 30, settings.waveType =1, "SAWTOOTH", 11) Then
			settings.waveType = 1
		End If
		
		If Button(370, 30, settings.waveType =2, "SINEWAVE", 12) Then
			settings.waveType = 2
		End If

		
		If Button(490, 30, settings.waveType =3, "NOISE", 13) Then
			settings.waveType = 3
		End If


		DrawBar(5-1-1, 412-1-1, 102+2, 19+2, black)
		If Button(5, 412, False, "RANDOMIZE", 40) Then
			sfx.Randomize()
			sfx.GetSettings(settings)
		End If
		
		If Button(5, 382, False, "MUTATE", 30) Then
			sfx.Mutate()
			sfx.GetSettings(settings)
		End If
		
		DrawTextColor(515, 170, black, "VOLUME")
		DrawBar(490-1-1+60, 180-1+5, 70, 2, black)
		DrawBar(490-1-1+60+68, 180-1+5, 2, 205, black)
		DrawBar(490-1-1+60, 180-1, 42+2, 10+2, TColor.Set($FF, $00, $00))
		Slider(490, 180, settings.soundVol, False, " ")
		If Button(490, 200, False, "PLAY SOUND", 20)
			PlaySample()
		End If
		
		
		If Button(490, 290, False, "LOAD SOUND", 14) Then
			Local filename:String = RequestFile("Load Sound", "txt", False, CurrentDir())

			If filename Then
				sfx.Reset()
				Local stream:TStream = ReadStream(filename)
				settings = sfx.LoadSettings(stream, True)
				stream.close()
			End If
			
		End If
		
		If Button(490, 320, False, "SAVE SOUND", 15) Then
			Local filename:String = RequestFile("Save Sound", "txt", True, CurrentDir())

			If filename Then
				Local stream:TStream = WriteStream(filename)
				sfx.SaveSettings(stream)
				stream.close()
			End If
		End If
	
		DrawBar(490-1-1+60, 380-1+9, 70, 2, black)
		DrawBar(490-1-2, 380-1-2, 102+4, 19+4, black)
		
		If Button(490, 380, False, "EXPORT .WAV", 16) Then
			Local filename:String = RequestFile("Export WAV", "wav", True, CurrentDir())
			If filename Then
				ExportWAV(filename)
			End If
		End If

		If Button(490, 410, False, wavFreq + " HZ", 18) Then
			If wavFreq = 44100 Then
				wavFreq = 22050
			Else
				wavFreq = 44100
			End If
		End If

		If Button(490, 440, False, wavBits + "-BIT", 19) Then
			If wavBits = 16 Then
				wavBits = 8
			Else
				wavBits = 16
			End If
		End If
		
		
		
		Local ypos:Int = 4
	
		Local xpos:Int = 350
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.envAttack, False, "ATTACK TIME")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.envSustain, False, "SUSTAIN TIME")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.envPunch, False, "SUSTAIN PUNCH")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.envDecay, False, "DECAY TIME")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.baseFreq, False, "START FREQUENCY")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.freqLimit, False, "MIN FREQUENCY")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.freqRamp, True, "SLIDE")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.freqDramp, True, "DELTA SLIDE")
		ypos:+ 1
	
		Slider(xpos, ypos*18, settings.vibStrength, False, "VIBRATO DEPTH")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.vibSpeed, False, "VIBRATO SPEED")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.arpMod, True, "CHANGE AMOUNT")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.arpSpeed, False, "CHANGE SPEED")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.duty, False, "SQUARE DUTY")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.dutyRamp, True, "DUTY SWEEP")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.repeatSpeed, False, "REPEAT SPEED")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.phaOffset, True, "PHASER OFFSET")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.phaRamp, True, "PHASER SWEEP")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		Slider(xpos, ypos*18, settings.lpfFreq, False, "LP FILTER CUTOFF")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.lpfRamp, True, "LP FILTER CUTOFF SWEEP")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.lpfResonance, False, "LP FILTER RESONANCE")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.hpfFreq, False, "HP FILTER CUTOFF")
		ypos:+ 1
		Slider(xpos, ypos*18, settings.hpfRamp, True, "HP FILTER CUTOFF SWEEP")
		ypos:+ 1
	
		DrawBar(xpos-190, ypos*18-5, 300, 2, black)
	
		DrawBar(xpos-190, 4*18-5, 1, (ypos-4)*18, black)
		DrawBar(xpos-190+299, 4*18-5, 1, (ypos-4)*18, black)

		' reset the current button so that clicks work nicer
		vcurbutton = -1

		sfx.SetSettings(settings)
	End Method
	
	Method DrawTextColor(x:Int, y:Int, color:TColor, text:String)
		SetColor color.r, color.g, color.b
		DrawText text, x, y-2
	End Method
	
	Method Button:Int(x:Int, y:Int, highlight:Int, text:String, id:Int)
	
		Local color1:TColor = black
		Local color2:TColor = TColor.Set($A0, $90, $88)
		Local color3:TColor = black
	
		
		Local hover:Int = MouseInBox(x, y, 100, 17)

		If hover And mouseLeftClick
			vcurbutton = id
		End If
		
		Local isCurrent:Int = False
		If vcurbutton = id Then
			isCurrent = True
		End If

		If highlight Then
			color1 = black
			color2 = TColor.Set($98, $80, $70)
			color3 = TColor.Set($FF, $F0, $E0)
		End If
		
		If isCurrent And hover Then
			color1 = TColor.Set($A0, $90, $88)
			color2 = TColor.Set($FF, $F0, $E0)
			color3 = TColor.Set($A0, $90, $88)
		End If
		
		DrawBar(x-1, y-1, 102, 19, color1)
		DrawBar(x, y, 100, 17, color2)
		
		DrawTextColor(x+5, y+5, color3, text)
		If isCurrent And hover And mouseLeft Then
			Return True
		End If
		
		Return False
	End Method
	
	
	Method MouseInBox:Int(x:Int, y:Int, w:Int, h:Int)
		If mx>=x And mx<x+w And my>=y And my<y+h Then
			Return True
		End If
		Return False
	End Method

	Method DrawBar(sx:Int, sy:Int, w:Int, h:Int, color:TColor = Null)
		If color Then
			SetColor color.r, color.g, color.b
		Else
			SetColor 0, 0, 0
		End If
		DrawRect sx, sy, w, h
	End Method

	Method Slider(x:Int, y:Int, value:Float Var, bipolar:Int, text:String)
		If MouseInBox(x, y, 100, 10) Then
			If mouseLeftClick Then
				vselected = Varptr value
			End If
			If mouseRightClick Then
				value = 0.0
			End If
		End If
		
		Local mv:Float = mx - px
	
		If vselected <> Varptr value Then
			mv = 0.0
		End If
		
		If bipolar Then
			value :+ mv * 0.005
			If value<-1.0 Then
				value=-1.0
			End If
			If value>1.0 Then
				value=1.0
			End If
		Else
			value :+ mv * 0.0025
			If value<0.0 Then
				value=0.0
			End If
			If value>1.0 Then
				value=1.0
			End If
		End If
		
		DrawBar(x-1, y, 102, 10, black)
		Local ival:Int = value*99
		If bipolar Then
			ival = (value*49.5 + 49.5)
		End If
		
		DrawBar(x, y+1, ival, 8, TColor.Set($F0, $C0, $90))
		DrawBar(x+ival, y+1, 100-ival, 8, TColor.Set($80, $70, $60))
		DrawBar(x+ival, y+1, 1, 8, TColor.Set($FF, $FF, $FF))
		If bipolar Then
			DrawBar(x+50, y-1, 1, 3, black)
			DrawBar(x+50, y+8, 1, 3, black)
		End If
		
		Local tcol:TColor = black
		If settings.waveType <> 0 And (Varptr value = Varptr settings.duty Or  Varptr value = Varptr settings.dutyRamp) Then
			tcol= TColor.Set($80, $80, $80)
		End If
		DrawTextColor(x-4-text.length*8, y+1, tcol, text)
	End Method

	Method PlaySample()
		If Not playing Then

			Local sf:Int
			Local dv:Int
			If wavBits = 16 Then
				sf = SF_MONO16LE
				dv = 2
			Else
				sf = SF_MONO8
				dv = 1
			End If
	
	
			bank = FillBank()
			
			Local sample:TAudioSample = TAudioSample.CreateStatic( bank.Lock(), bank.size() / dv , wavFreq, sf )
			bank.Unlock()
			
			Local sound:TSound=LoadSound( sample, False )
			
			channel = PlaySound(sound)
			playing = True
		End If
	End Method
	
	Method FillBank:TBank()

		Local snd:Int
		If wavBits = 16 Then
			snd = SND_SHORT
		Else
			snd = SND_BYTE
		End If

		Return sfx.GenerateSoundData(snd, wavFreq)
	End Method

	Method frnd:Float(_range:Float)
		Return Rnd(10000)/10000*_range
	End Method
	
	Method ExportWAV(filename:String)
		Local stream:TStream = WriteStream(filename)
		If stream Then
		
			Local audioData:TBank = FillBank()
		
			stream.WriteString("RIFF")
			
			stream.WriteInt(36 + audioData.size())
			
			stream.WriteString("WAVEfmt ")
			stream.WriteInt(16)
			stream.WriteShort(1)
			stream.WriteShort(1)
			stream.WriteInt(wavFreq)
			stream.WriteInt(wavFreq * wavBits / 8)
			stream.WriteShort(wavBits / 8)
			stream.WriteShort(wavBits)
			stream.WriteString("data")
			
			stream.WriteInt(audioData.size())
			
			stream.write(audioData.buf(), audioData.size())
		
			stream.close()
		End If
	End Method
	
End Type


Type TColor
	Field r:Int, g:Int, b:Int
	Function Set:TColor(r:Int, g:Int, b:Int)
		Local this:TColor = New TColor
		this.r = r
		this.g = g
		this.b = b
		Return this
	End Function
End Type
