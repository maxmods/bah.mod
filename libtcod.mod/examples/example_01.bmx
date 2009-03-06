SuperStrict


Framework BaH.libtcod
Import BRL.pngloader

' Force the driver to use OpenGL...
' On Windows, comment out this line, and it will probably use DirectX
SetGraphicsDriver(GLMax2DDriver())

' sample screen size
Const SAMPLE_SCREEN_WIDTH:Int = 46
Const SAMPLE_SCREEN_HEIGHT:Int = 20
' sample screen position
Const SAMPLE_SCREEN_X:Int = 20
Const SAMPLE_SCREEN_Y:Int = 10


' the offscreen console in which the samples are rendered
Global sampleConsole:TCODConsole = New TCODConsole.Create(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT)

Local keyColor:TCODColor = TCODColor.Black
Local key:TCODKey

Local font:String = "fonts/terminal8x8_gs_as.png"
Local charWidth:Int = 8
Local charHeight:Int = 8
Local nbCharH:Int =16
Local nbCharV:Int=16
Local inRow:Int = False
Local creditsEnd:Int = False

Local First:Int = True

Local curSample:Int = 0


Local samples:TSample[] = [ ..
		TSample.Create("  True colors        ",render_colors), ..
		TSample.Create("  Offscreen console  ",render_offscreen), ..
		TSample.Create("  Line drawing       ",render_lines), ..
		TSample.Create("  Noise              ",render_noise), ..
		TSample.Create("  Field of view      ",render_fov), ..
		TSample.Create("  Path finding       ",render_path), ..
		TSample.Create("  Bsp toolkit        ",render_bsp), ..
		TSample.Create("  Image toolkit      ",render_image), ..
		TSample.Create("  Mouse support      ",render_mouse) ..
	]

Local nbSamples:Int = samples.length


TCODConsole.SetCustomFont(font, TCOD_FONT_TYPE_GREYSCALE | TCOD_FONT_LAYOUT_ASCII_INCOL)


TCODConsole.InitRoot(80, 50, "Woo!", False)

Repeat
	If KeyDown(key_escape)
		Exit
	End If
	
	If Not creditsEnd Then
		creditsEnd = TCODConsole.RenderCredits(60, 42, False)
	End If


	' print the list of samples
	For Local i:Int = 0 Until nbSamples
		If i = curSample Then
			' set colors For currently selected sample
			TCODConsole.root.SetForegroundColor(TCODColor.white)
			TCODConsole.root.SetBackgroundColor(TCODColor.lightBlue)
		Else
			' set colors For other samples
			TCODConsole.root.SetForegroundColor(TCODColor.grey)
			TCODConsole.root.SetBackgroundColor(TCODColor.black)
		End If
		' Print the sample name
		TCODConsole.root.PrintLeft(2, 46 - (nbSamples - i), TCOD_BKGND_SET, samples[i].name)
	Next

	' print the help message
	TCODConsole.root.SetForegroundColor(TCODColor.Grey)
	TCODConsole.root.PrintRight(79, 46, TCOD_BKGND_NONE, "last frame : " + ..
		PadString(Int(TCODSystem.GetLastFrameLength()*1000), 3) + " ms (" + PadString(TCODSystem.GetFps(), 3) + " fps)")

	TCODConsole.root.PrintRight(79, 47, TCOD_BKGND_NONE, "elapsed : " + PadString(TCODSystem.GetElapsedMilli(), 8) + "ms " + PadString(TCODSystem.GetElapsedSeconds(), 9) + "s")
	TCODConsole.root.PrintLeft(2, 47, TCOD_BKGND_NONE,Chr(TCOD_CHAR_ARROW_N) + Chr(TCOD_CHAR_ARROW_S) + " : Select a sample")
	If TCODConsole.isFullscreen() Then
		TCODConsole.root.PrintLeft(2, 48, TCOD_BKGND_NONE,"ALT-ENTER : switch to windowed mode  ")
	Else
		TCODConsole.root.PrintLeft(2, 48, TCOD_BKGND_NONE,"ALT-ENTER : switch to fullscreen mode")
	End If

	' render current sample
	samples[curSample].render(First, key)
	If First Then
		First = False
	End If

	' blit the sample console on the root console
	TCODConsole.Blit(sampleConsole, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT, TCODConsole.root, SAMPLE_SCREEN_X, SAMPLE_SCREEN_Y, 255)


	'did the user hit a key ?
	key = TCODConsole.CheckForKeypress()
	
	' update the game screen
	TCODConsole.Flush()

	' did the user hit a key ?
	If key.vk = TCODK_DOWN Then
		' down arrow : Next sample
		curSample = (curSample + 1) Mod nbSamples
		First = True
	Else If key.vk = TCODK_UP Then
		' up arrow : previous sample
		curSample :- 1
		If curSample < 0 Then
			curSample = nbSamples - 1
		End If
		First = True
	Else If key.vk = TCODK_ENTER And key.lalt Then
		TCODConsole.SetFullscreen(Not TCODConsole.IsFullscreen())
	End If
	
	'key = TCODConsole.CheckForKeypress()
	
Until TCODConsole.IsWindowClosed()

End

Function PadString:String(text:String, size:Int)
	Local length:Int = text.length
	If length < size Then
		Local s:String = ""
		For Local i:Int = 0 Until size-length
			s:+ " "
		Next
		Return s + text
	Else If length > size Then
		Return text[length - size..]
	End If
	Return text
End Function


' ***************************
' true colors sample
' ***************************
Function render_colors(_first:Int, key:TCODKey)
	Const TOPLEFT:Int = 0
	Const TOPRIGHT:Int = 1
	Const BOTTOMLEFT:Int = 2
	Const BOTTOMRIGHT:Int = 3
	
	Global cols:TCODColor[] = [New TCODColor.Create(50,40,150), ..
		New TCODColor.Create(240,85,5), New TCODColor.Create(50,35,240), New TCODColor.Create(10,200,130)] ' random corner colors
	Global dirr:Int[] = [1,-1,1,1]
	Global dirg:Int[] = [1,-1,-1,1]
	Global dirb:Int[] = [1,1,1,-1]
	If _first Then
		TCODSystem.SetFps(0) ' unlimited fps
		sampleConsole.Clear()
	End If
	
	' ==== slighty modify the corner colors ====
	For Local c:Int = 0 Until 4
		' move each corner color
		Local component:Int = TCODRandom.GetInstance().GetInt(0,2)
		Select component
		  Case 0
		    cols[c].SetR(cols[c].r + 5 * dirr[c])
		    If cols[c].r = 255 Then
		    	dirr[c] = -1
		    Else If cols[c].r = 0 Then
		    	dirr[c] = 1
	    	End If
		  Case 1
		    cols[c].SetG(cols[c].g + 5*dirg[c])
		    If cols[c].g = 255 Then
		    	dirg[c] = -1
		    Else If cols[c].g = 0 Then
		    	dirg[c] = 1
		    End If
		  Case 2
		    cols[c].SetB(cols[c].b + 5 * dirb[c])
		    If cols[c].b = 255 Then
		    	dirb[c]=-1
		    Else If cols[c].b=0 Then
		    	dirb[c] = 1
		    End If
    	End Select
	Next

	' ==== scan the whole screen, interpolating corner colors ====
	For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
		Local xcoef:Float = x / Float(SAMPLE_SCREEN_WIDTH-1)
		' get the current column top and bottom colors
		Local top:TCODColor = TCODColor.Lerp(cols[TOPLEFT], cols[TOPRIGHT], xcoef)
		Local bottom:TCODColor = TCODColor.Lerp(cols[BOTTOMLEFT], cols[BOTTOMRIGHT], xcoef)
		For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
			Local ycoef:Float = y / Float(SAMPLE_SCREEN_HEIGHT-1)
			' get the current cell color
			Local curColor:TCODColor = TCODColor.Lerp(top, bottom, ycoef)
			sampleConsole.SetBack(x, y, curColor, TCOD_BKGND_SET)
		Next
	Next

	' ==== print the text with a random color ====
	' get the background color at the text position
	Local textColor:TCODColor = sampleConsole.GetBack(SAMPLE_SCREEN_WIDTH / 2, 5)
	' and invert it
	textColor.SetR(255 - textColor.r)
	textColor.SetG(255 - textColor.g)
	textColor.SetB(255 - textColor.b)
		
	' put random text (for performance tests)
	For Local x:Int= 0 Until SAMPLE_SCREEN_WIDTH
		For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
			Local col:TCODColor = sampleConsole.GetBack(x,y)
			col = TCODColor.Lerp(col, TCODColor.black, 0.5)
			Local c:Int = TCODRandom.GetInstance().getInt(97, 122)
			sampleConsole.SetForegroundColor(col)
			sampleConsole.PutChar(x, y, c, TCOD_BKGND_NONE)
		Next
	Next

	sampleConsole.SetForegroundColor(textColor)
	' the background behind the text is slightly darkened using the BKGND_MULTIPLY flag
	sampleConsole.SetBackgroundColor(TCODColor.grey)
	sampleConsole.PrintCenterRect(SAMPLE_SCREEN_WIDTH/2,5,SAMPLE_SCREEN_WIDTH-2,SAMPLE_SCREEN_HEIGHT-1,TCOD_BKGND_MULTIPLY, ..
		"The Doryen library uses 24 bits colors, for both background and foreground.")
End Function

Function render_offscreen(_first:Int, key:TCODKey)
	Global secondary:TCODConsole = New TCODConsole.Create(SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2) ' Second screen
	Global screenshot:TCODConsole = New TCODConsole.Create(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT) ' Second screen
	Global alpha:Int = 0 ' alpha value For the blit operation
	Global init:Int = False ' draw the secondary screen only the First time
	Global counter:Int = 0
	Global x:Int = 0,y:Int = 0 ' secondary screen position
	Global xdir:Int = 1, ydir:Int = 1 ' movement direction
	If Not init Then
		init = True
		secondary.PrintFrame(0, 0, SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2, False, "Offscreen console")
		secondary.PrintCenterRect(SAMPLE_SCREEN_WIDTH / 4, 2, SAMPLE_SCREEN_WIDTH / 2 - 2, SAMPLE_SCREEN_HEIGHT / 2, ..
			TCOD_BKGND_NONE,"You can render to an offscreen console and blit in on another one, simulating alpha transparency.")
	End If
	If _first Then
		TCODSystem.SetFps(30) ' fps limited To 30
		' get a "screenshot" of the current sample screen
		TCODConsole.Blit(sampleConsole, 0, 0,SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT, screenshot, 0, 0, 255)
	End If
	counter:+ 1
	If counter Mod 20 = 0 Then
		' move the secondary screen every 2 seconds
		x:+xdir
		y:+ydir
		If x = SAMPLE_SCREEN_WIDTH/2 Then
			 xdir=-1
		Else If x = 0 Then
			xdir=1
		End If
		If y = SAMPLE_SCREEN_HEIGHT / 2 Then
			ydir=-1
		Else If y = 0 Then
			ydir=1
		End If
	End If
	alpha=(255*(1.0+Cos(TCODSystem.getElapsedSeconds()*2.0 * 57.2957795))/2.0)
	' restore the initial screen
	TCODConsole.Blit(screenshot, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT, sampleConsole, 0, 0, 255)
	' blit the overlapping screen
	TCODConsole.Blit(secondary, 0, 0, SAMPLE_SCREEN_WIDTH / 2, SAMPLE_SCREEN_HEIGHT / 2, sampleConsole, x, y, alpha)
End Function

Global bkFlag:Int = TCOD_BKGND_SET ' current blending mode
Function line_listener:Int(x:Int, y:Int)
	If x>= 0 And y >= 0 And x < SAMPLE_SCREEN_WIDTH And y < SAMPLE_SCREEN_HEIGHT Then
		sampleConsole.SetBack(x, y, TCODColor.lightBlue, bkFlag)
	End If
	Return True
End Function

Function render_lines(_first:Int, key:TCODKey)
	Global bk:TCODConsole = New TCODConsole.Create(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT) ' colored background
	Global init:Int = False
	Global flagNames:String[] = [..
		"TCOD_BKGND_NONE", ..
		"TCOD_BKGND_SET", ..
		"TCOD_BKGND_MULTIPLY", ..
		"TCOD_BKGND_LIGHTEN", ..
		"TCOD_BKGND_DARKEN", ..
		"TCOD_BKGND_SCREEN", ..
		"TCOD_BKGND_COLOR_DODGE", ..
		"TCOD_BKGND_COLOR_BURN", ..
		"TCOD_BKGND_ADD", ..
		"TCOD_BKGND_ADDALPHA", ..
		"TCOD_BKGND_BURN", ..
		"TCOD_BKGND_OVERLAY", ..
		"TCOD_BKGND_ALPHA" ..
	]
	If key.vk = TCODK_ENTER Or key.vk = TCODK_KPENTER Then
		' switch to the next blending mode
		bkFlag:+ 1
		If bkFlag & $ff > TCOD_BKGND_ALPH Then
			bkFlag = TCOD_BKGND_NONE
		End If
	End If
	If bkFlag & $ff = TCOD_BKGND_ALPH Then
		' for the alpha mode, update alpha every frame
		Local alpha:Float = (1.0 + Cos(TCODSystem.GetElapsedSeconds()*2  * 57.2957795))/2.0
		bkFlag = (TCOD_BKGND_ALPH | (Int(alpha*255) Shl 8))
	Else If bkFlag & $ff = TCOD_BKGND_ADDA Then
		' for the add alpha mode, update alpha every frame
		Local alpha:Float = (1.0 + Cos(TCODSystem.GetElapsedSeconds()*2 * 57.2957795))/2.0
		bkFlag = (TCOD_BKGND_ADDA | (Int(alpha*255) Shl 8))
	End If
	If Not init Then
		' initialize the colored background
		Local col:TCODColor = New TCODColor.Create()
		For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
			For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
				col.SetR(x* 255 / (SAMPLE_SCREEN_WIDTH-1))
				col.SetG((x+y)* 255 / (SAMPLE_SCREEN_WIDTH-1+SAMPLE_SCREEN_HEIGHT-1))
				col.SetB(y* 255 / (SAMPLE_SCREEN_HEIGHT-1))
				bk.SetBack(x, y, col)
			Next
		Next
		init=True
	End If
	If _first Then
		TCODSystem.SetFps(30) ' fps limited to 30
		sampleConsole.SetForegroundColor(TCODColor.white)
	End If
	' blit the background
	TCODConsole.Blit(bk, 0, 0, SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT, sampleConsole, 0, 0)
	' render the gradient
	Local recty:Int = ((SAMPLE_SCREEN_HEIGHT-2)*((1.0 + Cos(TCODSystem.GetElapsedSeconds() * 57.2957795))/2.0))
	Local col:TCODColor = New TCODColor.Create()
	For Local x:Int= 0 Until SAMPLE_SCREEN_WIDTH
		col.SetR(x*255 / SAMPLE_SCREEN_WIDTH)
		col.SetG(x*255 / SAMPLE_SCREEN_WIDTH)
		col.SetB(x*255 / SAMPLE_SCREEN_WIDTH)
		sampleConsole.SetBack(x,recty,col, bkFlag)
		sampleConsole.SetBack(x,recty+1,col, bkFlag)
		sampleConsole.SetBack(x,recty+2,col, bkFlag)
	Next
	' calculate the segment ends
	Local angle:Float = TCODSystem.GetElapsedSeconds() * 2.0 * 57.2957795
	Local cosAngle:Float = Cos(angle)
	Local sinAngle:Float = Sin(angle)
	Local xo:Int = (SAMPLE_SCREEN_WIDTH/2*(1 + cosAngle))
	Local yo:Int = (SAMPLE_SCREEN_HEIGHT/2 + sinAngle * SAMPLE_SCREEN_WIDTH/2)
	Local xd:Int = (SAMPLE_SCREEN_WIDTH/2*(1 - cosAngle))
	Local yd:Int = (SAMPLE_SCREEN_HEIGHT/2 - sinAngle * SAMPLE_SCREEN_WIDTH/2)

	' render the line
	TCODLine.Line(xo, yo, xd, yd, line_listener)
	' print the current flag
	sampleConsole.PrintLeft(2, 2, TCOD_BKGND_NONE,flagNames[bkFlag & $ff] + " (ENTER to change)")
End Function

Function render_noise(_first:Int, key:TCODKey)
	Const PERLIN:Int = 0
	Const SIMPLEX:Int = 1
	Const WAVELET:Int = 2
	Const FBM_PERLIN:Int = 3
	Const TURBULENCE_PERLIN:Int = 4
	Const FBM_SIMPLEX:Int = 5
	Const TURBULENCE_SIMPLEX:Int = 6
	Const FBM_WAVELET:Int = 7
	Const TURBULENCE_WAVELET:Int = 8
	
	Global funcName:String[] = [ ..
		"1 : perlin noise       ", ..
		"2 : simplex noise      ", ..
		"3 : wavelet noise      ", ..
		"4 : perlin fbm         ", ..
		"5 : perlin turbulence  ", ..
		"6 : simplex fbm        ", ..
		"7 : simplex turbulence ", ..
		"8 : wavelet fbm        ", ..
		"9 : wavelet turbulence " ]
	
	Global noise:TCODNoise = Null
	Global func:Int = PERLIN
	Global dx:Float = 0.0, dy:Float = 0.0
	Global octaves:Float = 4.0
	Global hurst:Float = TCOD_NOISE_DEFAULT_HURST
	Global lacunarity:Float = TCOD_NOISE_DEFAULT_LACUNARITY
	Global zoom:Float = 3.0
	
	If Not noise Then
		noise = New TCODNoise.Create(2, hurst, lacunarity)
	End If
	If _first Then
		TCODSystem.SetFps(30) ' fps limited To 30
	End If
	
	sampleConsole.Clear()

	' texture animation
	dx :+ 0.01
	dy :+ 0.01
	' render the 2d noise function
	For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
		For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
			Local f:Float[2]
			f[0] = zoom * x / SAMPLE_SCREEN_WIDTH + dx
			f[1] = zoom * y / SAMPLE_SCREEN_HEIGHT + dy
			Local value:Float = 0.0
			Select func 
				Case PERLIN
					value = noise.GetPerlin(f)
					
				Case SIMPLEX
					value = noise.GetSimplex(f)
				Case WAVELET
					value = noise.GetWavelet(f)
				Case FBM_PERLIN
					value = noise.GetFbmPerlin(f, octaves)
				Case TURBULENCE_PERLIN
					value = noise.GetTurbulencePerlin(f, octaves)
				Case FBM_SIMPLEX
					value = noise.GetFbmSimplex(f, octaves)
				Case TURBULENCE_SIMPLEX
					value = noise.GetTurbulenceSimplex(f, octaves)
				Case FBM_WAVELET
					value = noise.GetFbmWavelet(f, octaves)
				Case TURBULENCE_WAVELET
					value = noise.GetTurbulenceWavelet(f, octaves)
			End Select
			Local c:Int=((value+1.0)/2.0*255)
			' use a bluish color
			Local col:TCODColor = New TCODColor.Create((c/2),(c/2),c)
			sampleConsole.setBack(x, y, col, TCOD_BKGND_SET)
		Next
	Next
	' draw a transparent rectangle
	sampleConsole.SetBackgroundColor(TCODColor.grey)

	sampleConsole.Rect(2, 2, 24 , 9, False, TCOD_BKGND_MULTIPLY)
	' draw the text
	For Local curfunc:Int = PERLIN To TURBULENCE_WAVELET
		If curfunc = func Then
				sampleConsole.setForegroundColor(TCODColor.white)
				sampleConsole.setBackgroundColor(TCODColor.lightBlue)
				sampleConsole.printLeft(2,2+curfunc,TCOD_BKGND_SET,funcName[curfunc])
		Else
				sampleConsole.setForegroundColor(TCODColor.grey)
				sampleConsole.printLeft(2,2+curfunc,TCOD_BKGND_NONE,funcName[curfunc])
		End If
	Next
	' draw parameters
	sampleConsole.SetForegroundColor(TCODColor.white)
	sampleConsole.PrintLeft(2, 11, TCOD_BKGND_NONE, "Y/H : zoom " + zoom)
	If func > WAVELET Then
		sampleConsole.printLeft(2,12,TCOD_BKGND_NONE,"E/D : hurst (" + hurst + ")")
		sampleConsole.printLeft(2,13,TCOD_BKGND_NONE,"R/F : lacunarity (" + lacunarity + ")")
		sampleConsole.printLeft(2,14,TCOD_BKGND_NONE,"T/G : octaves (" + octaves + ")")
	End If
	' handle keypress
	If key.vk = TCODK_NONE Then
		Return
	End If
	If key.c >= Asc("1") And key.c <= Asc("9") Then
		' change the noise function
		func = key.c - Asc("1")
	Else If key.c = Asc("E") Or key.c = Asc("e")
		' increase hurst
		hurst :+ 0.1
		noise.Free()
		noise = New TCODNoise.Create(2,hurst,lacunarity)
	Else If key.c = Asc("D") Or key.c = Asc("d")
		' decrease hurst
		hurst :- 0.1
		noise.Free()
		noise = New TCODNoise.Create(2,hurst,lacunarity)
	Else If key.c = Asc("R") Or key.c = Asc("r")
		' increase lacunarity
		lacunarity :+ 0.5
		noise.Free()
		noise = New TCODNoise.Create(2,hurst,lacunarity)
	Else If key.c = Asc("F") Or key.c = Asc("f")
		' decrease lacunarity
		lacunarity :- 0.5
		noise.Free()
		noise = New TCODNoise.Create(2,hurst,lacunarity)
	Else If key.c = Asc("T") Or key.c = Asc("t")
		' increase octaves
		octaves :+ 0.5
	Else If key.c = Asc("G") Or key.c = Asc("g")
		' decrease octaves
		octaves :- 0.5
	Else If key.c = Asc("Y") Or key.c = Asc("y")
		' increase zoom
		zoom :+ 0.2
	Else If key.c = Asc("H") Or key.c = Asc("h")
		' decrease zoom
		zoom :- 0.2
	End If


End Function

Function render_fov(_first:Int, key:TCODKey)
	Global smap:String[] = [ ..
		"##############################################", ..
		"#######################      #################", ..
		"#####################    #     ###############", ..
		"######################  ###        ###########", ..
		"##################      #####             ####", ..
		"################       ########    ###### ####", ..
		"###############      #################### ####", ..
		"################    ######                  ##", ..
		"########   #######  ######   #     #     #  ##", ..
		"########   ######      ###                  ##", ..
		"########                                    ##", ..
		"####       ######      ###   #     #     #  ##", ..
		"#### ###   ########## ####                  ##", ..
		"#### ###   ##########   ###########=##########", ..
		"#### ##################   #####          #####", ..
		"#### ###             #### #####          #####", ..
		"####           #     ####                #####", ..
		"########       #     #### #####          #####", ..
		"########       #####      ####################", ..
		"##############################################" ..
	]

	Const TORCH_RADIUS:Int = 10.0
	Const SQUARED_TORCH_RADIUS:Int = TORCH_RADIUS * TORCH_RADIUS
	Global px:Int = 20, py:Int = 10 ' player position
	'Global dx:Int = 24, dy:Int = 1 ' destination
	Global recomputeFov:Int = True ' the player moved. must recompute fov
	Global torch:Int = False ' torch fx on ?
	Global map:TCODMap
	Global darkWall:TCODColor = New TCODColor.Create(0,0,100)
	Global lightWall:TCODColor = New TCODColor.Create(130,110,50)
	Global darkGround:TCODColor = New TCODColor.Create(50,50,150)
	Global lightGround:TCODColor = New TCODColor.Create(200,180,50)
	Global noise:TCODNoise
	Global lightWalls:Int = True
	Global algonum:Int = 0
	Global algoNames:String[] = ["BASIC      ", "DIAMOND    ", "SHADOW     ", ..
		"PERMISSIVE0","PERMISSIVE1","PERMISSIVE2","PERMISSIVE3","PERMISSIVE4", ..
		"PERMISSIVE5","PERMISSIVE6","PERMISSIVE7","PERMISSIVE8"]
	Global torchx:Float = 0.0 ' torch light position in the perlin noise
	If Not map
		' initialize the map for the fov toolkit
		map = New TCODMap.Create(SAMPLE_SCREEN_WIDTH,SAMPLE_SCREEN_HEIGHT)
		For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
			For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
				If smap[y][x] = 32 Then
					map.setProperties(x,y,True,True)' ground
				Else If smap[y][x] = Asc("=") Then
					map.setProperties(x,y,True,False) ' window
				End If
			Next
		Next
		' 1d noise used for the torch flickering
		noise = New TCODNoise.Create(1)
	End If
	If _first Then
		TCODSystem.SetFps(30) ' fps limited to 30
		' we draw the foreground only the first time.
		' during the player movement, only the @ is redrawn.
		' the rest impacts only the background color
		' draw the help text & player @
		sampleConsole.Clear()
		sampleConsole.SetForegroundColor(TCODColor.white)
		sampleConsole.PrintLeft(1,0,TCOD_BKGND_NONE, fovdets(torch, lightWalls) + algoNames[algonum])
		sampleConsole.SetForegroundColor(TCODColor.black)
		sampleConsole.PutChar(px,py, Asc("@"),TCOD_BKGND_NONE)
		' draw windows
		For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
			For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
				If smap[y][x] = Asc("=") Then
					sampleConsole.PutChar(x, y, TCOD_CHAR_DHLINE, TCOD_BKGND_NONE)
				End If
			Next
		Next
	End If
	If recomputeFov Then
		' calculate the field of view from the player position
		recomputeFov = False
		If torch Then
			map.ComputeFov(px, py, TORCH_RADIUS, lightWalls, algonum)
		Else
			map.ComputeFov(px, py, 0, lightWalls, algonum)
		End If
	End If
	' torch position & intensity variation
	Local dx:Float = 0.0, dy:Float = 0.0, di:Float = 0.0
	If torch Then
		' slightly change the perlin noise parameter
		torchx :+ 0.2
		' randomize the light position between -1.5 and 1.5
		Local tdx:Float = torchx + 20.0
		dx = noise.getSimplex([tdx]) * 1.5
		tdx :+ 30.0
		dy = noise.GetSimplex([tdx]) * 1.5
		' randomize the light intensity between -0.2 and 0.2
		di = 0.2 * noise.GetSimplex([torchx])
	End If
	' draw the dungeon
	For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
		For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
			Local visible:Int = map.IsInFov(x,y)
			Local wall:Int = False
			If smap[y][x] = Asc("#") Then
				wall = True
			End If
			If Not visible Then
				If wall Then
					sampleConsole.SetBack(x, y, darkWall, TCOD_BKGND_SET)
				Else
					sampleConsole.SetBack(x, y, darkGround, TCOD_BKGND_SET)
				End If
			Else
				Local light:TCODColor
				If Not torch Then
					If wall Then
						light = lightWall
					Else
						light = lightGround
					End If
				Else
					' torch flickering fx
					Local base:TCODColor
					If wall Then 
						base = darkWall.Copy()
						light = lightWall.Copy()
					Else
						base = darkGround.Copy()
						light = lightGround.Copy()
					End If

					' cell distance to torch (squared)
					Local r:Float = ((x-px+dx)*(x-px+dx)+(y-py+dy)*(y-py+dy))
					If r < SQUARED_TORCH_RADIUS Then
						' l = 1.0 at player position, 0.0 at a radius of 10 cells
						Local l:Float = (SQUARED_TORCH_RADIUS - r) / SQUARED_TORCH_RADIUS + di
						' clamp between 0 and 1
						If l < 0.0 Then
							l = 0.0
						Else If l > 1.0 Then
							l = 1.0
						End If
						' interpolate the color
						base.SetR(base.r + (light.r - base.r)*l)
						base.SetG(base.g + (light.g - base.g)*l)
						base.SetB(base.b + (light.b - base.b)*l)
					End If
					light = base
				End If
				sampleConsole.setBack(x,y,light, TCOD_BKGND_SET )
			End If
		Next
	Next
	If key.c = Asc("I") Or key.c = Asc("i") Then
		' player move north
		If smap[py-1][px] = 32 Then
			sampleConsole.putChar(px,py, 32, TCOD_BKGND_NONE)
			py:- 1
			sampleConsole.putChar(px,py, Asc("@"),TCOD_BKGND_NONE)
			recomputeFov=True
		End If
	Else If key.c = Asc("K") Or key.c = Asc("k") Then
		' player move south
		If smap[py+1][px] = 32 Then
			sampleConsole.putChar(px,py, 32,TCOD_BKGND_NONE)
			py:+ 1
			sampleConsole.putChar(px,py, Asc("@"),TCOD_BKGND_NONE)
			recomputeFov=True
		End If
	Else If key.c = Asc("J") Or key.c = Asc("j") Then
		' player move west
		If smap[py][px-1] = 32 Then
			sampleConsole.putChar(px,py, 32,TCOD_BKGND_NONE)
			px:- 1
			sampleConsole.putChar(px,py, Asc("@"),TCOD_BKGND_NONE)
			recomputeFov=True
		End If
	Else If key.c = Asc("L") Or key.c = Asc("l") Then
		' player move east
		If smap[py][px+1] = 32 Then
			sampleConsole.putChar(px,py, 32,TCOD_BKGND_NONE)
			px:+ 1
			sampleConsole.putChar(px,py, Asc("@"),TCOD_BKGND_NONE)
			recomputeFov=True
		End If
	Else If key.c = Asc("T") Or key.c = Asc("t") Then
		' enable/disable the torch fx
		torch = Not torch
		sampleConsole.SetForegroundColor(TCODColor.white)
		sampleConsole.PrintLeft(1,0,TCOD_BKGND_NONE, fovdets(torch, lightWalls) + algoNames[algonum])
		sampleConsole.SetForegroundColor(TCODColor.black)
	Else If key.c = Asc("W") Or key.c = Asc("w") Then
		lightWalls = Not lightWalls
		sampleConsole.SetForegroundColor(TCODColor.white)
		sampleConsole.PrintLeft(1,0,TCOD_BKGND_NONE, fovdets(torch, lightWalls) + algoNames[algonum])
		sampleConsole.SetForegroundColor(TCODColor.black)
		recomputeFov = True
	Else If key.c = Asc("+") Or key.c = Asc("-") Then
		If key.c = Asc("+") Then
			algonum :+ 1
		Else
			algonum :- 1
		End If
		algonum = Max(0, Min(NB_FOV_ALGORITHMS, algonum))
		sampleConsole.SetForegroundColor(TCODColor.white)
		sampleConsole.PrintLeft(1,0,TCOD_BKGND_NONE, fovdets(torch, lightWalls) + algoNames[algonum])
		sampleConsole.SetForegroundColor(TCODColor.black)
		recomputeFov = True
	End If
End Function

Function fovdets:String(torch:Int, lightWalls:Int)
	Local s:String = "IJKL : move around~nT : torch fx "
	If torch Then
		s:+ "on ~nW : light walls "
	Else
		s:+ "off~nW : light walls "
	End If
	If lightWalls Then
		s:+ "on ~n+-: algo "
	Else
		s:+ "off~n+-: algo "
	End If
	Return s
End Function

Function render_path(_first:Int, key:TCODKey)
	Global smap:String[] = [ ..
		"##############################################", ..
		"#######################      #################", ..
		"#####################    #     ###############", ..
		"######################  ###        ###########", ..
		"##################      #####             ####", ..
		"################       ########    ###### ####", ..
		"###############      #################### ####", ..
		"################    ######                  ##", ..
		"########   #######  ######   #     #     #  ##", ..
		"########   ######      ###                  ##", ..
		"########                                    ##", ..
		"####       ######      ###   #     #     #  ##", ..
		"#### ###   ########## ####                  ##", ..
		"#### ###   ##########   ###########=##########", ..
		"#### ##################   #####          #####", ..
		"#### ###             #### #####          #####", ..
		"####           #     ####                #####", ..
		"########       #     #### #####          #####", ..
		"########       #####      ####################", ..
		"##############################################" ..
	]

	Const TORCH_RADIUS:Int = 10.0
	Const SQUARED_TORCH_RADIUS:Int = TORCH_RADIUS * TORCH_RADIUS
	Global px:Int = 20, py:Int = 10 ' player position
	Global dx:Int = 24, dy:Int = 1 ' destination
	Global map:TCODMap
	Global darkWall:TCODColor = New TCODColor.Create(0,0,100)
	Global darkGround:TCODColor = New TCODColor.Create(50,50,150)
	Global lightGround:TCODColor = New TCODColor.Create(200,180,50)
	Global path:TCODPath
	Global recalculatePath:Int = False
	Global busy:Float
	Global oldChar:Int = 32
	
	Local mouse:TCODMouse
	Local mx:Int, my:Int

	If Not map Then
		' initialize the map for the fov toolkit
		map = New TCODMap.Create(SAMPLE_SCREEN_WIDTH, SAMPLE_SCREEN_HEIGHT)
		For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
			For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
				If smap[y][x] = 32 Then
					map.SetProperties(x, y, True, True)' ground
				Else If smap[y][x] = Asc("=") Then
					map.SetProperties(x, y, True, False) ' window
				End If
			Next
		Next
		path = New TCODPath.Create(map)
	End If
	If _first Then
		TCODSystem.SetFps(30) ' fps limited to 30
		' we draw the foreground only the first time.
		' during the player movement, only the @ is redrawn.
		' the rest impacts only the background color
		' draw the help text & player @
		sampleConsole.Clear()
		sampleConsole.SetForegroundColor(TCODColor.white)
		sampleConsole.PutChar(dx, dy, Asc("+"), TCOD_BKGND_NONE)
		sampleConsole.PutChar(px, py, Asc("@"), TCOD_BKGND_NONE)
		sampleConsole.PrintLeft(1,1,TCOD_BKGND_NONE,"IJKL / mouse :~nmove destination")
		' draw windows
		For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
			For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
				If smap[y][x] = Asc("=") Then
					sampleConsole.PutChar(x, y, TCOD_CHAR_DHLINE, TCOD_BKGND_NONE)
				End If
			Next
		Next
		recalculatePath=True
	End If
	If recalculatePath Then
		path.Compute(px, py, dx, dy)
		recalculatePath = False
		busy = 1.0
	End If
	' draw the dungeon
	For Local y:Int = 0 Until SAMPLE_SCREEN_HEIGHT
		For Local x:Int = 0 Until SAMPLE_SCREEN_WIDTH
			Local wall:Int = False
			If smap[y][x]= Asc("#") Then
				wall = True
			End If
			If wall Then
				sampleConsole.SetBack(x,y, darkWall, TCOD_BKGND_SET )
			Else
				sampleConsole.SetBack(x,y, darkGround, TCOD_BKGND_SET )
			End If
		Next
	Next
	' draw the path
	For Local i:Int = 0 Until path.Size()
		Local x:Int, y:Int
		path.Get(i, x, y)
		sampleConsole.SetBack(x, y, lightGround, TCOD_BKGND_SET)
	Next
	' move the creature
	busy :- TCODSystem.GetLastFrameLength()
	If busy <= 0.0 Then
		busy :+ 0.2
		If Not path.IsEmpty() Then
			sampleConsole.PutChar(px,py, 32, TCOD_BKGND_NONE)
			path.Walk(px, py, True)
			sampleConsole.PutChar(px,py, Asc("@"), TCOD_BKGND_NONE)
		End If
	End If
	If (key.c = Asc("I") Or key.c = Asc("i")) And dy > 0 Then
		' destination move north
		sampleConsole.putChar(dx,dy,oldChar,TCOD_BKGND_NONE)
		dy:- 1
		oldChar = sampleConsole.GetChar(dx,dy)
		sampleConsole.PutChar(dx, dy, Asc("+"), TCOD_BKGND_NONE)
		If smap[dy][dx] = 32 Then
			recalculatePath = True
		End If
	Else If (key.c = Asc("K") Or key.c = Asc("k")) And dy < SAMPLE_SCREEN_HEIGHT-1 Then
		' destination move south
		sampleConsole.PutChar(dx,dy,oldChar,TCOD_BKGND_NONE)
		dy:+ 1
		oldChar = sampleConsole.GetChar(dx,dy)
		sampleConsole.PutChar(dx,dy, Asc("+"), TCOD_BKGND_NONE)
		If smap[dy][dx] = 32 Then
			recalculatePath = True
		End If
	Else If (key.c = Asc("J") Or key.c = Asc("j")) And dx > 0 Then
		' destination move west
		sampleConsole.PutChar(dx,dy,oldChar,TCOD_BKGND_NONE)
		dx:- 1
		oldChar = sampleConsole.GetChar(dx,dy)
		sampleConsole.PutChar(dx,dy, Asc("+"), TCOD_BKGND_NONE)
		If smap[dy][dx] = 32 Then
			recalculatePath = True
		End If
	Else If (key.c = Asc("L") Or key.c = Asc("l")) And dx < SAMPLE_SCREEN_WIDTH -1 Then
		' destination move east
		sampleConsole.PutChar(dx,dy,oldChar,TCOD_BKGND_NONE)
		dx:+ 1
		oldChar = sampleConsole.GetChar(dx,dy)
		sampleConsole.PutChar(dx,dy, Asc("+"), TCOD_BKGND_NONE)
		If smap[dy][dx] = 32 Then
			recalculatePath = True
		End If
	End If
	mouse = TCODMouse.GetStatus()
	mx = mouse.cx - SAMPLE_SCREEN_X
	my = mouse.cy - SAMPLE_SCREEN_Y
	If mx >= 0 And mx < SAMPLE_SCREEN_WIDTH And my >= 0 And my < SAMPLE_SCREEN_HEIGHT And ( dx <> mx Or dy <> my ) Then
		sampleConsole.PutChar(dx, dy, oldChar, TCOD_BKGND_NONE)
		dx = mx
		dy = my
		oldChar = sampleConsole.GetChar(dx,dy)
		sampleConsole.PutChar(dx,dy, Asc("+"), TCOD_BKGND_NONE)
		If smap[dy][dx] = 32 Then
			recalculatePath = True
		End If
	End If
End Function

Function render_bsp(_first:Int, key:TCODKey)
End Function

Function render_image(_first:Int, key:TCODKey)
	Global img:TCODImage = Null, circle:TCODImage = Null
	Global blue:TCODColor = New TCODColor.Create(0,0,255)
	Global green:TCODColor = New TCODColor.Create(0,255,0)
	If img = Null Then
		img = TCODImage.CreateFromFile("skull.png")
		img.SetKeyColor(TCODColor.black)
		circle = TCODImage.CreateFromFile("circle.png")
	End If
	If _first Then
		TCODSystem.SetFps(30) ' fps limited to 30
	End If
	sampleConsole.SetBackgroundColor(TCODColor.black)
	sampleConsole.Clear()
	Local x:Float= SAMPLE_SCREEN_WIDTH/2 + Cos(TCODSystem.GetElapsedSeconds() * 57.2957795)*10.0
	Local y:Float=(SAMPLE_SCREEN_HEIGHT/2)
	Local scalex:Float = 0.2 + 1.8 * (1.0 + Cos((TCODSystem.GetElapsedSeconds()/2) * 57.2957795))/2.0
	Local scaley:Float = scalex
	Local angle:Float = TCODSystem.GetElapsedSeconds()  * 57.2957795
	Local elapsed:Int = TCODSystem.GetElapsedMilli() / 2000
	If elapsed & 1 Then
		' split the color channels of circle.png
		' the red channel
		sampleConsole.setBackgroundColor(TCODColor.red)
		sampleConsole.rect(0, 3, 15, 15, False, TCOD_BKGND_SET)
		circle.BlitRect(sampleConsole, 0, 3, -1, -1, TCOD_BKGND_MULTIPLY)
		' the green channel
		sampleConsole.setBackgroundColor(green)
		sampleConsole.rect(15, 3, 15, 15, False, TCOD_BKGND_SET)
		circle.BlitRect(sampleConsole, 15, 3, -1, -1, TCOD_BKGND_MULTIPLY)
		' the blue channel
		sampleConsole.setBackgroundColor(blue)
		sampleConsole.rect(30, 3, 15, 15, False, TCOD_BKGND_SET)
		circle.BlitRect(sampleConsole, 30, 3, -1, -1, TCOD_BKGND_MULTIPLY)
	Else
		' render circle.png with normal blitting
		circle.BlitRect(sampleConsole, 0, 3, -1, -1, TCOD_BKGND_SET)
		circle.BlitRect(sampleConsole, 15, 3, -1, -1, TCOD_BKGND_SET)
		circle.BlitRect(sampleConsole, 30, 3, -1, -1, TCOD_BKGND_SET)
	End If
	img.blit(sampleConsole, x, y, TCOD_BKGND_SET, scalex, scaley, angle)
End Function

Function render_mouse(_first:Int, key:TCODKey)
 	Local mouse:TCODMouse
	Global lbut:Int = False, rbut:Int = False, mbut:Int = False
	
	If _first Then
		TCODSystem.SetFps(30) ' fps limited to 30
		sampleConsole.SetBackgroundColor(TCODColor.grey)
		sampleConsole.SetForegroundColor(TCODColor.lightYellow)
		TCODMouse.Move(320,200)
		TCODMouse.ShowCursor(True)
	End If
	
	sampleConsole.Clear()
	mouse = TCODMouse.GetStatus()
	If mouse.lbuttonPressed Then
		lbut = Not lbut
	End If
	If mouse.rbuttonPressed Then
		rbut = Not rbut
	End If
	If mouse.mbuttonPressed Then
		mbut = Not mbut
	End If
	
	Local s:String = "Mouse position : " + PadString(mouse.x, 4) + "x" + PadString(mouse.y, 4) + "~n" + ..
		"Mouse cell     : " + PadString(mouse.cx, 4) + "x" + PadString(mouse.cy, 4) + "~n" + ..
		"Mouse movement : " + PadString(mouse.dx, 4) + "x" + PadString(mouse.dy, 4) + "~n" + ..
		"Left button    : " + mbutton(mouse.lbutton, lbut) + ..
		"Right button   : " + mbutton(mouse.rbutton, lbut) + ..
		"Middle button  : " + mbutton(mouse.mbutton, lbut)
	
	sampleConsole.PrintLeft(1, 1, TCOD_BKGND_NONE, s)

	sampleConsole.printLeft(1, 10, TCOD_BKGND_NONE, "1 : Hide cursor~n2 : Show cursor")
	If key.c = Asc("1") Then
		TCODMouse.ShowCursor(False)
	Else If key.c = Asc("2") Then
		TCODMouse.ShowCursor(True)
	End If
	
End Function

Function mbutton:String(button:Int, but:Int)
	Local s:String
	If button Then
		s:+ " ON (toggle "
	Else
		s:+ "OFF (toggle "
	End If
	If but Then
		s:+ " ON)~n"
	Else
		s:+ "OFF)~n"
	End If
	Return s
End Function

Type TSample

	Field name:String
	Field render(_first:Int, key:TCODKey)

	Function Create:TSample(name:String, func(_first:Int, key:TCODKey))
		Local this:TSample = New TSample
		this.name = name
		this.render = func
		Return this
	End Function
	
End Type

