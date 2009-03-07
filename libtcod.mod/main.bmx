' Copyright (c) 2008-2009 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the auther nor the names of its contributors may be used to 
'       endorse or promote products derived from this software without specific
'       prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import "color.bmx"
Import "random.bmx"
Import "line.bmx"
Import "map.bmx"
Import "heightmap.bmx"
Import "text.bmx"
Import "common.bmx"

Rem
bbdoc: 
End Rem
Global TCOD_NATIVE_KEY_EVENTS:Int = True

Rem
bbdoc: 
End Rem
Type TCODConsole

	Global root:TCODConsole

	Field consolePtr:Byte Ptr

	Rem
	bbdoc: This will either create a new window in your desktop or switch to full screen.
	about: Once this function has been called, you access the root console with TCODConsole.root.
	End Rem
	Function InitRoot:TCODConsole(w:Int, h:Int, title:String, fullscreen:Int = False)
		' TODO : what to do if we already exist?
		If Not root Then
			Local console:TCODConsole = New TCODConsole
			console.consolePtr = bmx_tcodconsole_initroot(console, w, h, title, fullscreen)
			root = console
			
			TCODKey.Init()
		End If
		
		Return root
	End Function
	
	Rem
	bbdoc: Creates an off-screen console.
	about: You can create as many off-screen consoles as you want. You can draw on them as you would do with the root console, but you
	cannot flush them to the screen. But you can blit them on other consoles, including the root console. See Blit().
	End Rem
	Function CreateConsole:TCODConsole(width:Int, height:Int)
		Return New TCODConsole.Create(width, height)
	End Function
	
	Rem
	bbdoc: Creates an off-screen console.
	about: You can create as many off-screen consoles as you want. You can draw on them as you would do with the root console, but you
	cannot flush them to the screen. But you can blit them on other consoles, including the root console. See Blit().
	End Rem
	Method Create:TCODConsole(width:Int, height:Int)
		consolePtr = bmx_tcodconsole_create(Self, width, height)
		Return Self
	End Method
	
	Rem
	bbdoc: Allows you to use a bitmap font (png, bmp, etc) with custom character size or layout.
	about: It should be called before initializing the root console with initRoot.
	End Rem
	Function SetCustomFont(fontFile:String, flags:Int = TCOD_FONT_LAYOUT_ASCII_INCOL, nbCharHoriz:Int = 0, nbCharVertic:Int = 0)
		bmx_tcodconsole_setcustomfont(fontFile, flags, nbCharHoriz, nbCharVertic)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function IsWindowClosed:Int()
		Return bmx_tcodconsole_iswindowclosed()
	End Function

	Rem
	bbdoc: Applies the console updates to the screen.
	about: This only works for the root console.
	End Rem
	Function Flush()
		bmx_tcodconsole_flush()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function CheckForKeypress:TCODKey()
		PollSystem
		Return TCODKey.PopKey()
	End Function
	
	Rem
	bbdoc: Returns true if the current mode is fullscreen.
	End Rem
	Function IsFullscreen:Int()
		Return bmx_tcodconsole_isfullscreen()
	End Function
	
	Rem
	bbdoc: Destroys the offscreen console and releases any resources allocated.
	about: Don't use it on the root console.
	End Rem
	Method Free()
		If consolePtr Then
			bmx_tcodconsole_free(consolePtr)
			consolePtr = Null
		End If
	End Method
	
	Rem
	bbdoc: Modifies all cells of the console.
	about: <ul>
	<li>Sets the cell's background color to the console default background color</il>
	<li>Sets the cell's foreground color to the console default foreground color</il>
	<li>sets the cell's ASCII code to 32 (space)</il>
	</ul>
	End Rem
	Method Clear()
		bmx_tcodconsole_clear(consolePtr)
	End Method
	
	Rem
	bbdoc: Changes the default foreground color for the console.
	about: The default foreground color is used by several drawing functions like clear, putChar, ...
	End Rem
	Method SetForegroundColor(color:TCODColor)
		bmx_tcodconsole_setforegroundcolor(consolePtr, color.colorPtr)
	End Method

	Rem
	bbdoc: Returns the default foreground color of the console.
	End Rem
	Method GetForegroundColor:TCODColor()
		Return TCODColor._create(bmx_tcodconsole_getforegroundcolor(consolePtr))
	End Method

	Rem
	bbdoc: Changes the default background color for the console.
	about: The default background color is used by several drawing functions like Clear, PutChar, ...
	End Rem
	Method SetBackgroundColor(color:TCODColor)
		bmx_tcodconsole_setbackgroundcolor(consolePtr, color.colorPtr)
	End Method

	Rem
	bbdoc: Returns the default background color of the console.
	End Rem
	Method GetBackgroundColor:TCODColor()
		Return TCODColor._create(bmx_tcodconsole_getbackgroundcolor(consolePtr))
	End Method

	Rem
	bbdoc: Modifies the foreground color of a cell, leaving other properties (background color and ASCII code) unchanged.
	End Rem
	Method SetFore(x:Int, y:Int, col:TCODColor)
		bmx_tcodconsole_setfore(consolePtr, x, y, col.colorPtr)
	End Method

	Rem
	bbdoc: Returns the current foreground color of a cell.
	End Rem
	Method GetFore:TCODColor(x:Int, y:Int)
		Return TCODColor._create(bmx_tcodconsole_getfore(consolePtr, x, y))
	End Method

	Rem
	bbdoc: Modifies the background color of a cell, leaving other properties (foreground color and ASCII code) unchanged.
	End Rem
	Method SetBack(x:Int, y:Int, col:TCODColor, flag:Int = TCOD_BKGND_SET)
		bmx_tcodconsole_setback(consolePtr, x, y, col.colorPtr, flag:Int)
	End Method

	Rem
	bbdoc: Returns the current background color of a cell.
	End Rem
	Method GetBack:TCODColor(x:Int, y:Int)
		Return TCODColor._create(bmx_tcodconsole_getback(consolePtr, x, y))
	End Method

	Rem
	bbdoc: Draws a right aligned string on the console, using default foreground and background colors.
	about: If the string reaches the left border of the console, it is truncated.
	End Rem
	Method PrintRight(x:Int, y:Int, flag:Int, text:String)
		bmx_tcodconsole_printright(consolePtr, x, y, flag, text)
	End Method

	Rem
	bbdoc: Draws a left aligned string on the console, using default foreground and background colors.
	about: If the string reaches the right border of the console, it is truncated.
	End Rem
	Method PrintLeft(x:Int, y:Int, flag:Int, text:String)
		bmx_tcodconsole_printleft(consolePtr, x, y, flag, text)
	End Method

	Rem
	bbdoc: Draws a centered string on the console, using default foreground and background colors.
	about: If the string reaches borders of the console, it is truncated.
	End Rem
	Method PrintCenter(x:Int, y:Int, flag:Int, text:String)
		bmx_tcodconsole_printcenter(consolePtr, x, y,flag, text)
	End Method

	Rem
	bbdoc: Draws a right aligned string in a rectangle inside the console, using default foreground and background colors.
	returns: The height (number of console lines) of the printed string.
	about: If the string reaches the left border of the rectangle, carriage returns are inserted.
	<p>
	If h > 0 and the bottom of the rectangle is reached, the string is truncated. If h = 0, the string is only truncated if it
	reaches the bottom of the console.
	</p>
	End Rem
	Method PrintRightRect:Int(x:Int, y:Int, w:Int, h:Int, flag:Int, text:String)
		Return bmx_tcodconsole_printrightrect(consolePtr, x, y, w, h, flag, text)
	End Method

	Rem
	bbdoc: Gets the expected height without actually printing the string.
	End Rem
	Method GetHeightRightRect:Int(x:Int, y:Int, w:Int, h:Int, text:String)
		Return bmx_tcodconsole_getheightrightrect(consolePtr, x, y, w, h, text)
	End Method

	Rem
	bbdoc: Draws a left aligned string in a rectangle inside the console, using default foreground and background colors.
	returns: The height (number of console lines) of the printed string.
	about: If the string reaches the right border of the rectangle, carriage returns are inserted.
	<p>
	If h > 0 and the bottom of the rectangle is reached, the string is truncated. If h = 0, the string is only truncated if
	it reaches the bottom of the console.
	</p>
	End Rem
	Method PrintLeftRect:Int(x:Int, y:Int, w:Int, h:Int, flag:Int, text:String)
		Return bmx_tcodconsole_printleftrect(consolePtr, x, y, w, h, flag, text)
	End Method

	Rem
	bbdoc: Gets the expected height without actually printing the string.
	End Rem
	Method GetHeightLeftRect:Int(x:Int, y:Int, w:Int, h:Int, text:String)
		Return bmx_tcodconsole_getheightleftrect(consolePtr, x, y, w, h, text)
	End Method
	
	Rem
	bbdoc: Draws a centered string in a rectangle inside the console, using default foreground and background colors.
	returns: the height (number of console lines) of the printed string.
	about: If the string reaches the borders of the rectangle, carriage returns are inserted.
	<p>
	If h > 0 and the bottom of the rectangle is reached, the string is truncated. If h = 0, the string is only truncated if it reaches the bottom of the console.
	</p>
	End Rem
	Method PrintCenterRect:Int(x:Int, y:Int, w:Int, h:Int, flag:Int, text:String)
		Return bmx_tcodconsole_printcenterrect(consolePtr, x, y, w, h, flag, text)
	End Method

	Rem
	bbdoc: Gets the expected height without actually printing the string.
	End Rem
	Method GetHeightCenterRect:Int(x:Int, y:Int, w:Int, h:Int, text:String)
		Return bmx_tcodconsole_getheightcenterrect(consolePtr, x, y, w, h, text)
	End Method

	Rem
	bbdoc: Modifies every property of a cell.
	about: 
	End Rem
	Method PutChar(x:Int, y:Int, c:Int, flag:Int = TCOD_BKGND_SET)
		bmx_tcodconsole_putchar(consolePtr, x, y, c, flag)
	End Method
	
	Rem
	bbdoc: Modifies the ASCII code of a cell, leaving other properties (background and foreground colors) unchanged.
	about: Note that since a clear console has both background and foreground colors set to black for every cell, using setchar will
	produce black characters on black background. Use putchar instead.
	End Rem
	Method SetChar(x:Int, y:Int, c:Int)
		bmx_tcodconsole_setchar(consolePtr, x, y, c)
	End Method
	
	Rem
	bbdoc: Returns the current ASCII code of a cell.
	End Rem
	Method GetChar:Int(x:Int, y:Int)
		Return bmx_tcodconsole_getchar(consolePtr, x, y)
	End Method
	
	Rem
	bbdoc: Allows you to blit a rectangular area of the source console at a specific position on a destination console.
	about: It can also simulate alpha transparency with the fade parameter.
	End Rem
	Function Blit(source:TCODConsole, x:Int, y:Int, width:Int, height:Int, dest:TCODConsole, posX:Int, posY:Int, fade:Int = 255)
		bmx_tcodconsole_blit(source.consolePtr, x, y, width, height, dest.consolePtr, posX, posY, fade)
	End Function
	
	Rem
	bbdoc: Defines a rectangle inside the console.
	about: For each cell in the rectangle :
	<ul>
	<li>Sets the cell's background color to the console default background color</li>
	<li>If clear is true, set the cell's ASCII code to 32 (space)</li>
	</ul>
	End Rem
	Method Rect(x:Int, y:Int, w:Int, h:Int, clear:Int, flag:Int = TCOD_BKGND_SET)
		bmx_tcodconsole_rect(consolePtr, x, y, w, h, clear, flag)
	End Method
	
	Rem
	bbdoc: Draws an horizontal line in the console, using ASCII code TCOD_CHAR_HLINE (196), and the console's default background/foreground colors.
	End Rem
	Method HLine(x:Int, y:Int, length:Int, flag:Int = TCOD_BKGND_SET)
		bmx_tcodconsole_hline(consolePtr, x, y, length, flag)
	End Method
	
	Rem
	bbdoc: Draws a vertical line in the console, using ASCII code TCOD_CHAR_VLINE (179), and the console's default background/foreground colors.
	End Rem
	Method VLine(x:Int, y:Int, length:Int, flag:Int = TCOD_BKGND_SET)
		bmx_tcodconsole_vline(consolePtr, x, y, length, flag)
	End Method
	
	Rem
	bbdoc: Calls Rect using the TCOD_BKGND_SET flag, then draws a rectangle with the console's default foreground color.
	about: If fmt is not NULL, it is printed on the top of the rectangle, using inverted colors.
	End Rem
	Method PrintFrame(x:Int, y:Int, w:Int, h:Int, clear:Int, fmt:String)
		bmx_tcodconsole_printframe(consolePtr, x, y, w, h, clear, fmt)
	End Method
	
	Rem
	bbdoc: You can print a "Powered by libtcod x.y.z" screen during your game startup simply by calling this function after initRoot.
	about: The credits screen can be skipped by pressing any key.
	End Rem
	Function Credits()
		bmx_tcodconsole_credits()
	End Function
	
	Rem
	bbdoc: You can print the credits on one of your game screens (your main menu for example) by calling this function in your main loop.
	about: Returns true when the credits screen is finished, indicating that you no longer need to call it.
	End Rem
	Function RenderCredits:Int(x:Int, y:Int, alpha:Int)
		Return bmx_tcodconsole_rendercredits(x, y, alpha)
	End Function

	Rem
	bbdoc: Switches the root console to fullscreen or windowed mode.
	about: Note that there is no predefined key combination to switch to/from fullscreen. You have to do this in your own code.
	End Rem	
	Function SetFullscreen(fullscreen:Int)
		bmx_tcodconsole_setfullscreen(fullscreen)
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCODSystem

	Global consoleBuffer:Byte Ptr
	Global consoleWidth:Int
	Global consoleHeight:Int

	Global prevConsoleBuffer:Byte Ptr

	
	Global fullscreen_width:Int
	Global fullscreen_height:Int
	
	Global fullscreen_offsetx:Int
	Global fullscreen_offsety:Int
	
	' size of a character in the bitmap font
	Global fontWidth:Int = 8
	Global fontHeight:Int = 8
	' number of characters in the bitmap font
	Global fontNbCharHoriz:Int = 16
	Global fontNbCharVertic:Int = 16
	' font layout (character 0 To 15 on the First row Or the First column)
	Global fontInRow:Int
	' font transparent color
	Global fontKeyCol:TCODColor = TCODColor.Black
	Global font_file:String = "terminal.png"
	
	Global flags:Int

	' minimum length For a frame (when fps are limited)
	Global min_frame_length:Int
	' number of frames in the last Second
	Global fps:Int
	' current number of frames
	Global cur_fps:Int
	' length of the last rendering loop
	Global last_frame_length:Float
	
	Global charmap:TCODFont
	Global charBlank:Int
	
	Global mouseVisible:Int = True
	

	Function _Init:Int(w:Int, h:Int, buf:Byte Ptr, oldbuf:Byte Ptr, fullscreen:Int)
		consoleBuffer = buf
		prevConsoleBuffer = oldbuf
		
		consoleWidth = w
		consoleHeight = h

		Local pix:TPixmap = LoadPixmap(font_file)
		fontWidth = pix.width / fontNbCharHoriz
		fontHeight = pix.height / fontNbCharVertic
		
		createBlankForSpace(pix, fontNbCharHoriz, fontNbCharVertic, fontInRow)

		_dographics(fullscreen)
		Rem
		If fullscreen Then
			Local actual_fullscreen_width:Int, actual_fullscreen_height:Int
			
			_GetRes(consoleWidth, consoleHeight, fontWidth, fontHeight, ..
				fullscreen_width, fullscreen_height, actual_fullscreen_width, actual_fullscreen_height)
				
			Graphics actual_fullscreen_width, actual_fullscreen_height, 32
		Else
			Graphics w * fontWidth, h * fontHeight, 0
		End If
		End Rem

		'setblend alphablend
		SetMaskColor(fontKeyCol.r, fontKeyCol.g, fontKeyCol.b)
		
		charmap = TCODFont.Create(pix, fontNbCharHoriz, fontNbCharVertic, fontInRow, flags)
		
		consoleBuffer = buf
		
		Cls
		Return True
	End Function
	
	Function _dographics(fullscreen:Int)
		If fullscreen Then
			Local actual_fullscreen_width:Int, actual_fullscreen_height:Int
			
			_GetRes(consoleWidth, consoleHeight, fontWidth, fontHeight, ..
				fullscreen_width, fullscreen_height, actual_fullscreen_width, actual_fullscreen_height)
				
			Graphics actual_fullscreen_width, actual_fullscreen_height, 32
			
			fullscreen_offsetx = (actual_fullscreen_width - consoleWidth * fontWidth) / 2
			fullscreen_offsety = (actual_fullscreen_height - consoleHeight * fontHeight) / 2

		Else
			Graphics consoleWidth * fontWidth, consoleHeight * fontHeight, 0
			
			fullscreen_offsetx = 0
			fullscreen_offsety = 0
		End If
		SetBlend alphablend
		TCOD_REFRESH_IMAGE = True
	End Function
	
	Function _SetFullscreen(fullscreen:Int)
		EndGraphics
		_dographics(fullscreen)
	End Function

	Function _SleepMilli(milliseconds:Int)
		Delay milliseconds
	End Function

	Function _SetCustomFont(fontFile:String, _flags:Int, nbch:Int, nbcv:Int)

		font_file = fontFile
		flags = _flags

		If nbch > 0 Then
			fontNbCharHoriz=nbch
			fontNbCharVertic=nbcv
		Else 
			If (flags & TCOD_FONT_LAYOUT_ASCII_INROW ) Or ( flags & TCOD_FONT_LAYOUT_ASCII_INCOL ) Then
				fontNbCharHoriz=16
				fontNbCharVertic=16
		 	Else 
				flags :| TCOD_FONT_LAYOUT_TCOD
				fontNbCharHoriz=32
				fontNbCharVertic=8
			End If
		End If

		If flags & TCOD_FONT_LAYOUT_ASCII_INCOL
			fontInRow = False
		Else
			fontInRow = True
		End If

	End Function

	Function createBlankForSpace(pix:TPixmap, fontNbCharHoriz:Int, fontNbCharVertic:Int, inRow:Int)
		Local fontWidth:Int = pix.width / fontNbCharHoriz
		Local fontHeight:Int = pix.height / fontNbCharVertic

		Local j:Int = 32
		If flags & TCOD_FONT_LAYOUT_TCOD Then
			j = 0
		End If
		charBlank = j
		Local x0:Int, y0:Int

		If inRow Then
			x0 = (j Mod fontNbCharHoriz)*fontWidth
			y0 = (j /fontNbCharHoriz)*fontHeight
		Else
			x0 = (j /fontNbCharVertic)*fontWidth
			y0 = (j Mod fontNbCharVertic)*fontHeight
		End If


		For Local x:Int = 0 Until fontWidth
			For Local y:Int = 0 Until fontHeight
				pix.WritePixel(x + x0, y + y0, $FFFFFFFF)
			Next
		Next
	End Function

	Function _GetRes(cw:Int, ch:Int, fw:Int, fh:Int, fsw:Int, fsh:Int, width:Int Var, height:Int Var)

		Local wantedw:Int
		If fsw > cw * fw Then
			wantedw = fsw
		Else
			wantedw = cw * fw
		End If
		
		Local wantedh:Int
		If fsh > ch * fh Then
			wantedh = fsh
		Else
			wantedh = ch * fh
		End If
		
		width = wantedw
		height = wantedh
		Local modes:TGraphicsMode[] = GraphicsModes()
	
		Local bestw:Int = 99999
		Local besth:Int = 99999
		
		For Local mode:TGraphicsMode = EachIn modes

			If mode.width >= wantedw And mode.width <= bestw ..
					And mode.height >= wantedh And mode.height <= besth ..
					And mode.hertz = 32 Then
				bestw = mode.width
				besth = mode.height
			End If
			
		Next
		
		If bestw <> 99999 Then
			width = bestw
			height = besth
		End If
		'printf ("resolution : %dx%d =>%dx%d\n",wantedw,wantedh,bestw,besth);
	End Function

	Function _Flush(render:Int)
		Global old_time:Int, new_time:Int = _ElapsedMilli(), elapsed:Int
		
		Local frame_time:Int, time_to_wait:Int
		If render Then
			' *** TODO *** : implement rendering the console to the graphics context
			TCOD_sys_console_to_bitmap(Null, TCOD_console_get_width(Null), TCOD_console_get_height(Null), consoleBuffer, prevConsoleBuffer)
			Flip 0
			Cls
		End If
		
		old_time = new_time
		new_time = _ElapsedMilli()
'DebugLog "old_time = " + old_time
'DebugLog "new_time = " + new_time
		If new_time / 1000 <> elapsed Then
			' update fps every Second
			fps = cur_fps
'DebugLog fps
			cur_fps = 0
			elapsed = new_time/1000
		End If
		' If too fast, wait */
		frame_time = (new_time - old_time)
		last_frame_length = frame_time * 0.001
		cur_fps:+ 1
		time_to_wait = min_frame_length - frame_time
		If old_time > 0 And time_to_wait > 0 Then
			SleepMilli(time_to_wait)
			new_time = _ElapsedMilli()
			frame_time = (new_time - old_time)
		End If
		last_frame_length = frame_time * 0.001
	End Function
	
	Function _ConsoleToBitmap(vbitmap:Byte Ptr, console_width:Int, console_height:Int, console_buffer:Byte Ptr)
	
		Global index:Int
		Global br:Int, bg:Int, bb:Int
		Global fr:Int, fg:Int, fb:Int
		Global xc0:Int, yc0:Int

		' rendering direct to the graphics context
		If Not vbitmap Then
		
			charmap.Draw 0, 0, 0, True
		
			For Local y:Int = 0 Until console_height
				yc0 = y*fontHeight
				For Local x:Int = 0 Until console_width
				
					' get char colors, and ascii index.
					bmx_tcodsystem_getcolsandascii(console_buffer, x+y*console_width, Varptr index, ..
						Varptr br, Varptr bg, Varptr bb, Varptr fr, Varptr fg, Varptr fb, flags)

					' draw the background
					SetColor br, bg, bb
					charmap.Draw charBlank, x* fontWidth, yc0

					If index <> charBlank Then

						SetColor fr, fg, fb
						charmap.Draw index, x* fontWidth, yc0
						
					End If

				Next
			Next

		Else ' rendering to a texture...
			
		End If

	End Function

	
	Function _ElapsedMilli:Int()
		Return MilliSecs()
	End Function

	Function _SetFps(val:Int)
		If Not val Then
			min_frame_length = 0
		Else
			min_frame_length = 1000.0 / val
		End If
	End Function
	
	Function _GetFps:Int()
		Return fps
	End Function
	
	Function _LastFrameLength:Float()
'DebugLog "last_frame_length = " + last_frame_length
		Return last_frame_length
	End Function
	
	Function _GetCharSize(w:Int Var, h:Int Var)
		w = fontWidth
		h = fontHeight
	End Function
	
	Function _LoadImage:TPixmap(filename:String)
		Return LoadPixmap(filename)
	End Function
	
	Function _ImageAlpha:Int(image:TPixmap, x:Int, y:Int)
		If image.format < PF_BGRA8888 Then
			Return 255
		End If
		Return image.PixelPtr(x,y)[3]
	End Function

	Function _ImageSize(image:TPixmap, w:Int Ptr, h:Int Ptr)
		w[0] = image.width
		h[0] = image.height
	End Function

	Function _ImagePixel(image:TPixmap, x:Int, y:Int, r:Byte Ptr, g:Byte Ptr, b:Byte Ptr)
		If x < 0 Or y < 0 Or x >= image.width Or y >= image.height Then
			Return
		End If
		
		Local col:Int = image.ReadPixel(x, y)
		r[0] = col Shr 16
		g[0] = col Shr 8
		b[0] = col
	End Function
	
	Rem
	bbdoc: Returns the number of milliseconds since the program has started.
	End Rem
	Function GetElapsedMilli:Int()
		Return bmx_tcodsystem_getelapsedmilli()
	End Function
	
	Rem
	bbdoc: Returns the number of seconds since the program has started.
	End Rem
	Function GetElapsedSeconds:Float()
		Return bmx_tcodsystem_getelapsedseconds()
	End Function
	
	Rem
	bbdoc: Allows you to stop the program execution for a specified number of milliseconds.
	End Rem
	Function SleepMilli(value:Int)
		bmx_tcodsystem_sleepmilli(value)
	End Function
	
	Rem
	bbdoc: The setFps function allows you to limit the number of frames per second.
	about: If a frame is rendered faster than expected, the TCOD_console_flush function will wait so that the frame rate never exceed this value.
	<p>
	You can call this function during your game initialization.
	</p>
	<p>
	You can dynamically change the frame rate. Just call this function once again.
	</p>
	End Rem
	Function SetFps(value:Int)
		bmx_tcodsystem_setfps(value)
	End Function
	
	Rem
	bbdoc: The getFps function returns the actual number of frames rendered during the last second.
	about: This value is updated every second.
	End Rem
	Function GetFps:Int()
		Return bmx_tcodsystem_getfps()
	End Function
	
	Rem
	bbdoc: Returns the length in seconds of the last rendered frame.
	about: You can use this value to update every time dependant object in the world.
	End Rem
	Function GetLastFrameLength:Float()
		Return bmx_tcodsystem_getlastframelength()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetCurrentResolution(w:Int Var, h:Int Var)
		bmx_tcodsystem_getcurrentresolution(Varptr w, Varptr h)
	End Function

	Function _ShowCursor(visible:Int)
		mouseVisible = visible
		If visible Then
			ShowMouse()
		Else
			HideMouse()
		End If
	End Function
	

	Function _CursorVisible:Int()
		Return mouseVisible
	End Function

	
	Function _MoveMouse(x:Int, y:Int)
		MoveMouse(x, y)
	End Function
	

End Type

Rem
bbdoc: 
End Rem
Type TCODKey

	Const maxqueue:Int = 256
	Global queue:TCODKey[] = New TCODKey[maxqueue]
	Global currentKey:Int = 0
	Global queueTail:Int = 0
	Global lastTail:Int = 0
	
	Global noKey:TCODKey = New TCODKey

	Rem
	bbdoc: 
	End Rem
	Field vk:Int
	Rem
	bbdoc: 
	End Rem
	Field c:Int
	Rem
	bbdoc: 
	End Rem
	Field pressed:Int
	Rem
	bbdoc: 
	End Rem
	Field lalt:Int
	Rem
	bbdoc: 
	End Rem
	Field lctrl:Int
	Rem
	bbdoc: 
	End Rem
	Field ralt:Int
	Rem
	bbdoc: 
	End Rem
	Field rctrl:Int
	Rem
	bbdoc: 
	End Rem
	Field shift:Int
	
	Function Init()
		For Local i:Int = 0 Until maxqueue
			queue[i] = New TCODKey
		Next
		
		If TCOD_NATIVE_KEY_EVENTS Then
			AddHook EmitEventHook, Hook, Null, 0
		End If
		
		TCODMouse.Init()
	End Function
	
	' adds a key onto the stack
	Function PushKey(vk:Int, c:Int, pressed:Int, lalt:Int, lctrl:Int, ralt:Int, rctrl:Int, shift:Int)
		Local key:TCODKey = queue[queueTail]
		
		key.vk = vk
		key.c = c
		key.pressed = pressed
		key.lalt = lalt
		key.lctrl = lctrl
		key.ralt = ralt
		key.rctrl = rctrl
		key.shift = shift
		
		lastTail = queueTail
		queueTail = (queueTail + 1) Mod maxqueue
		
	End Function
	
	' pops a key from the stack
	Function PopKey:TCODKey()
		If currentKey = queueTail Then
			Return noKey
		End If
		
		Local key:TCODKey = queue[currentKey]
		
		currentKey = (currentKey + 1) Mod maxqueue
		
		Return key
		
	End Function
	
	' flushes all keys
	Function Flush()
		queueTail = (currentKey + 1) Mod maxqueue
		currentKey = queueTail
	End Function

	Function Hook:Object( id:Int, data:Object, context:Object )
	
		Local ev:TEvent=TEvent(data)
		If Not ev Return data
		
		Select ev.id
		Case EVENT_KEYDOWN, EVENT_KEYREPEAT
			Local mods:Int = ev.mods
			PushKey(MapKeyToVK(ev.data), 0, 1, mods & MODIFIER_OPTION, mods & MODIFIER_CONTROL, mods & MODIFIER_OPTION, ..
				mods & MODIFIER_CONTROL, mods & MODIFIER_SHIFT)
		Case EVENT_KEYUP
'			Local mods:Int = ev.mods
'			PushKey(MapKeyToVK(ev.data), 0, 0, mods & MODIFIER_OPTION, mods & MODIFIER_CONTROL, mods & MODIFIER_OPTION, ..
'				mods & MODIFIER_CONTROL, mods & MODIFIER_SHIFT)
		Case EVENT_KEYCHAR
			queue[lastTail].c = ev.data
		End Select
	
		Return TCODMouse.Hook(id, data, context)
	
	End Function
	
	Function MapKeyToVK:Int(key:Int)
		Select key
			Case KEY_ESCAPE
				Return TCODK_ESCAPE
			Case KEY_BACKSPACE
				Return TCODK_BACKSPACE
			Case KEY_TAB
				Return TCODK_TAB
			Case KEY_ENTER
				Return TCODK_ENTER
			Case KEY_LSHIFT, KEY_RSHIFT
				Return TCODK_SHIFT
			Case KEY_LCONTROL, KEY_RCONTROL
				Return TCODK_CONTROL
			Case KEY_LALT, KEY_RALT
				Return TCODK_ALT
			Case 19 ' KEY_PAUSE
				Return TCODK_PAUSE
			Case 20 ' KEY_CAPSLOCK
				Return TCODK_CAPSLOCK
			Case KEY_PAGEUP
				Return TCODK_PAGEUP
			Case KEY_PAGEDOWN
				Return TCODK_PAGEDOWN
			Case KEY_END
				Return TCODK_END
			Case KEY_HOME
				Return TCODK_HOME
			Case KEY_UP
				Return TCODK_UP
			Case KEY_LEFT
				Return TCODK_LEFT
			Case KEY_RIGHT
				Return TCODK_RIGHT
			Case KEY_DOWN
				Return TCODK_DOWN
			Case KEY_PRINT
				Return TCODK_PRINTSCREEN
			Case KEY_INSERT
				Return TCODK_INSERT
			Case KEY_DELETE
				Return TCODK_DELETE
			Case KEY_LSYS
				Return TCODK_LWIN
			Case KEY_RSYS
				Return TCODK_RWIN
			Case 182 ' KEY_APPS ?
				Return TCODK_APPS
			Case KEY_0
				Return TCODK_0
			Case KEY_1
				Return TCODK_1
			Case KEY_2
				Return TCODK_2
			Case KEY_3
				Return TCODK_3
			Case KEY_4
				Return TCODK_4
			Case KEY_5
				Return TCODK_5
			Case KEY_6
				Return TCODK_6
			Case KEY_7
				Return TCODK_7
			Case KEY_8
				Return TCODK_8
			Case KEY_9
				Return TCODK_9
			Case KEY_NUM0
				Return TCODK_KP0
			Case KEY_NUM1
				Return TCODK_KP1
			Case KEY_NUM2
				Return TCODK_KP2
			Case KEY_NUM3
				Return TCODK_KP3
			Case KEY_NUM4
				Return TCODK_KP4
			Case KEY_NUM5
				Return TCODK_KP5
			Case KEY_NUM6
				Return TCODK_KP6
			Case KEY_NUM7
				Return TCODK_KP7
			Case KEY_NUM8
				Return TCODK_KP8
			Case KEY_NUM9
				Return TCODK_KP9
			Case KEY_NUMADD
				Return TCODK_KPADD
			Case KEY_NUMSUBTRACT
				Return TCODK_KPSUB
			Case KEY_NUMDIVIDE
				Return TCODK_KPDIV
			Case KEY_NUMMULTIPLY
				Return TCODK_KPMUL
			Case KEY_NUMDECIMAL
				Return TCODK_KPDEC
			'Case KEY_KPENTER
			'	Return TCODK_KPENTER
			Case KEY_F1
				Return TCODK_F1
			Case KEY_F2
				Return TCODK_F2
			Case KEY_F3
				Return TCODK_F3
			Case KEY_F4
				Return TCODK_F4
			Case KEY_F5
				Return TCODK_F5
			Case KEY_F6
				Return TCODK_F6
			Case KEY_F7
				Return TCODK_F7
			Case KEY_F8
				Return TCODK_F8
			Case KEY_F9
				Return TCODK_F9
			Case KEY_F10
				Return TCODK_F10
			Case KEY_F11
				Return TCODK_F11
			Case KEY_F12
				Return TCODK_F12
			Case 144 ' KEY_NUMLOCK
				Return TCODK_NUMLOCK
			Case 145 ' KEY_SCROLL
				Return TCODK_SCROLLLOCK
			Case KEY_SPACE
				Return TCODK_SPACE
		End Select

		Return TCODK_CHAR
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TCODMouse

	Rem
	bbdoc: 
	End Rem
	Field x:Int
	Rem
	bbdoc: 
	End Rem
	Field y:Int
	Rem
	bbdoc: 
	End Rem
	Field dx:Int
	Rem
	bbdoc: 
	End Rem
	Field dy:Int
	Rem
	bbdoc: 
	End Rem
	Field cx:Int
	Rem
	bbdoc: 
	End Rem
	Field cy:Int
	Rem
	bbdoc: 
	End Rem
	Field dcx:Int
	Rem
	bbdoc: 
	End Rem
	Field dcy:Int
	Rem
	bbdoc: 
	End Rem
	Field lbutton:Int
	Rem
	bbdoc: 
	End Rem
	Field rbutton:Int
	Rem
	bbdoc: 
	End Rem
	Field mbutton:Int
	Rem
	bbdoc: 
	End Rem
	Field lbuttonPressed:Int
	Rem
	bbdoc: 
	End Rem
	Field rbuttonPressed:Int
	Rem
	bbdoc: 
	End Rem
	Field mbuttonPressed:Int
	Rem
	bbdoc: 
	End Rem
	Field wheelUp:Int
	Rem
	bbdoc: 
	End Rem
	Field wheelDown:Int

	Global mx:Int, my:Int, mwheel:Int
	Global mb:Int[3], ox:Int, oy:Int
	Global _dx:Int, _dy:Int
	
	Global mouse:TCODMouse
	
	Function Init()
		mouse = New TCODMouse
	
		For Local i:Int = 0 Until 3
			If MouseDown(i + 1) Then
				mb[i] = True
			End If
		Next
		mx = MouseX()
		my = MouseY()
	End Function

	Function Hook:Object( id:Int, data:Object, context:Object )

		Local ev:TEvent=TEvent(data)
		If Not ev Return data
		
		Select ev.id
		Case EVENT_MOUSEDOWN
			mb[ev.data - 1] = True
		Case EVENT_MOUSEUP
			mb[ev.data - 1] = False
		Case EVENT_MOUSEMOVE
			mx = ev.x
			my = ev.y
		Case EVENT_MOUSEWHEEL
			mwheel = ev.data
		End Select
	
		Return data
	End Function
	
	Function _Status(x:Int Var, y:Int Var, dxx:Int Var, dyy:Int Var, mb1:Int Var, mb2:Int Var, mb3:Int Var, wheel:Int Var, fsx:Int Var, fsy:Int Var)
		_dx = mx - ox
		_dy = my - oy
		ox = mx
		oy = my
		
		x = mx
		y = my
		dxx = _dx
		dyy = _dy
		mb1 = mb[0]
		mb2 = mb[1]
		mb3 = mb[2]
		wheel = mwheel
		fsx = TCODSystem.fullscreen_offsetx
		fsy = TCODSystem.fullscreen_offsety
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetStatus:TCODMouse()
		Return TCODMouse(bmx_tcodmouse_getstatus())
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function Move(x:Int, y:Int)
		bmx_tcodmouse_move(x, y)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function ShowCursor(visible:Int)
		bmx_tcodmouse_showcursor(visible)
	End Function
	
	Function _create:TCODMouse(	x:Int, y:Int, dx:Int, dy:Int, cx:Int, cy:Int, dcx:Int, dcy:Int, lbutton:Int, rbutton:Int, mbutton:Int, ..
			lbuttonPressed:Int, rbuttonPressed:Int, mbuttonPressed:Int, wheelUp:Int, wheelDown:Int)
		Local this:TCODMouse = New TCODMouse
		this.x = x
		this.y = y
		this.dx = dx
		this.dy = dy
		this.cx = cx
		this.cy = cy
		this.dcx = dcx
		this.dcy = dcy
		this.lbutton = lbutton
		this.rbutton = rbutton
		this.mbutton = mbutton
		this.lbuttonPressed = lbuttonPressed
		this.rbuttonPressed = rbuttonPressed
		this.mbuttonPressed = mbuttonPressed
		this.wheelUp = wheelUp
		this.wheelDown = wheelDown
		Return this
	End Function


End Type

Rem
bbdoc: 
End Rem
Type TCODImage

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function CreateImage:TCODImage(width:Int, height:Int)
		Return New TCODImage.Create(width, height)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TCODImage(width:Int, height:Int)
		objectPtr = bmx_tcodimage_create(width, height)
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function CreateFromFile:TCODImage(filename:String)
		Local this:TCODImage = New TCODImage
		this.objectPtr = bmx_tcodimage_createfromfile(filename)
		If this.objectPtr Then
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Defines a key color that will be ignored by the blitting function.
	about: This makes it possible to blit non rectangular images or images with transparent pixels
	End Rem
	Method SetKeyColor(keyColor:TCODColor)
		bmx_tcodimage_setkeycolor(objectPtr, keyColor.colorPtr)
	End Method

	Rem
	bbdoc: Allows you to specify the floating point coordinates of the center of the image, and its scale and rotation angle.
	about: The image will be rendered with sub-cell accuracy (no to be confused with sub-cell resolution, which is not yet implemented).
	For example, if you increase x by 0.01 per frame, you will achieve a smooth scrolling effect.
	End Rem
	Method Blit(console:TCODConsole, x:Float, y:Float, backgroundFlag:Int = TCOD_BKGND_SET, scalex:Float = 1.0, scaley:Float = 1.0, angle:Float = 0.0)
		bmx_tcodimage_blit(objectPtr, console.consolePtr, x, y, backgroundFlag, scalex, scaley, angle)
	End Method
	
	Rem
	bbdoc: Allows you to easily map an image to a specific part of a console, by specifying a rectangular part of the console (upper-left corner and size).
	End Rem
	Method BlitRect(console:TCODConsole, x:Int, y:Int, w:Int = -1, h:Int = -1, backgroundFlag:Int = TCOD_BKGND_SET)
		bmx_tcodimage_blitrect(objectPtr, console.consolePtr, x, y, w, h, backgroundFlag)
	End Method
	
	Rem
	bbdoc: Fills the whole image with the specified color.
	End Rem
	Method Clear(color:TCODColor)
		bmx_tcodimage_clear(objectPtr, color.colorPtr)
	End Method
	
	Rem
	bbdoc: Returns the size of the image in pixels.
	End Rem
	Method GetSize(w:Int Var, h:Int Var)
		bmx_tcodimage_getsize(objectPtr, Varptr w, Varptr h)
	End Method
	
	Rem
	bbdoc: Returns the color of the pixel at the specified location.
	End Rem
	Method GetPixel:TCODColor(x:Int, y:Int)
		Return TCODColor._create(bmx_tcodimage_getpixel(objectPtr, x, y))
	End Method
	
	Rem
	bbdoc: Returns the transparency level of the pixel at the specified location.
	End Rem
	Method GetAlpha:Int(x:Int, y:Int)
		Return bmx_tcodimage_getalpha(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Returns true if this pixel is completely transparent.
	End Rem
	Method IsPixelTransparent:Int(x:Int, y:Int)
		Return bmx_tcodimage_ispixeltransparent(objectPtr, x, y)
	End Method
	
	Rem
	bbdoc: Uses mipmaps to get the average color of an arbitrary rectangular region of the image.
	about: It can be used to draw a scaled-down version of the image. It's used by libtcod's blitting functions.
	End Rem
	Method GetMipmapPixel:TCODColor(x0:Float, y0:Float, x1:Float, y1:Float)
		Return TCODColor._create(bmx_tcodimage_getmipmappixel(objectPtr, x0, y0, x1, y1))
	End Method
	
	Rem
	bbdoc: Modifies the color of a pixel.
	End Rem
	Method PutPixel(x:Int, y:Int, col:TCODColor)
		bmx_tcodimage_putpixel(objectPtr, x, y, col.colorPtr)
	End Method
	
	Rem
	bbdoc: Resizes the image, scaling its content.
	about: If neww < oldw or newh < oldh, supersampling is used to scale down the image, otherwise the image is scaled up using nearest neightbor.
	End Rem
	Method Scale(newW:Int, newH:Int)
		bmx_tcodimage_scale(objectPtr, newW, newH)
	End Method
	
	Rem
	bbdoc: Flips the image horizontally.
	End Rem
	Method HFlip()
		bmx_tcodimage_hflip(objectPtr)
	End Method
	
	Rem
	bbdoc: Flips the image vertically.
	End Rem
	Method VFlip()
		bmx_tcodimage_vflip(objectPtr)
	End Method
	
	Rem
	bbdoc: Inverts the colors of the image.
	End Rem
	Method Invert()
		bmx_tcodimage_invert(objectPtr)
	End Method
	
	Rem
	bbdoc: Destroys an image and release used resources.
	End Rem
	Method Free()
		If objectPtr Then
			bmx_tcodimage_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

