' Copyright 2008 Bruce A Henderson
'
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
'
'     http://www.apache.org/licenses/LICENSE-2.0
'
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
'
SuperStrict


Rem
bbdoc: Tesseract OCR Engine
End Rem
Module BaH.tesseract


ModuleInfo "Version: 1.0"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release (Tesseract-2.03 SVNr201)"


ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H"
ModuleInfo "CC_OPTS: -I../ccutil -I../cutil"
?win32
ModuleInfo "CC_OPTS: -D__MSW32__ -D__BLOB_T_DEFINED"
?

Import BRL.Pixmap

Import "common.bmx"


'
' NOTES:
'
' For all changes, seek BaH in code
'
' platform.h   - Win32 __GNUC__ changes
' dawg.h       - Win32 __GNUC__ changes
' dawg.cpp     - Win32 __GNUC__ changes
' makedawg.cpp - Win32 __GNUC__ changes
' ocrclass.h   - Win32 __GNUC__ changes
' svutil.h     - Win32 __GNUC__ changes
'

Rem
bbdoc: 
End Rem
Type Tess

	Rem
	bbdoc: Initialises tesseract.
	about: Should be called before processing.
	<p>Parameters:
	<ul>
	<li><b>datapath</b> : The name of the data directory.</li>
	<li><b>lang</b> : The code of the language for which the data will be loaded. (Codes follow ISO 639-3). Default is 'eng'.</li>
	<li><b>numericMode</b> : If True, only digits and roman numbers are returned.</li>
	</ul>
	</p>
	End Rem
	Function Init(datapath:String, lang:String = "eng", numericMode:Int = False)
		bmx_tess_init(datapath, lang, numericMode)
	End Function

	Rem
	bbdoc: Sets the value of an internal "variable" (of either old or new types).
	returns: False if the name lookup failed.
	about: Supply the name of the variable and the value as a string.
	<p>
	For most variables, it is wise to set them before calling Init().
	</p>
	<p>
	eg. Tess.SetVariable("tessedit_char_blacklist", "xyz")
	</p>
	End Rem
	Function SetVariable:Int(variable:String, value:String)
		Return bmx_tess_setvariable(variable, value)
	End Function

	Rem
	bbdoc: Recognizes a rectangle from a pixmap and returns the result as a string.
	about: May be called many times for a single Init. The default rectangle is for the whole pixmap.
	<p>
	Currently has no error checking.
	</p>
	<p>Parameters:
	<ul>
	<li><b>pixmap</b> : </li>
	<li><b>x</b> : XXXXXXXXXXXX</li>
	<li><b>y</b> : XXXXXXXXXXXX</li>
	<li><b>width</b> : XXXXXXXXXXXX</li>
	<li><b>height</b> : XXXXXXXXXXXX</li>
	</ul>
	</p>
	End Rem
	Function Rect:String(pixmap:TPixmap, x:Int = -1, y:Int = -1, width:Int = -1, height:Int = -1)
		Local pix:TPixmap = pixmap
		
		' upgrade pixmap if required
		' needs to be at least 24bit...
		If pix.format < PF_BGR888 Then
			pix = pix.Convert(PF_RGB888)
		End If
		
		If x < 0 Then
			x = 0
		End If
		
		If y < 0 Then
			y = 0
		End If
		
		If width < 0 Then
			width = pix.width
		End If
		
		If height < 0 Then
			height = pix.height
		End If
		
		Return RectRaw(pix.pixels, BytesPerPixel[pix.format], pix.pitch, x, y, width, height)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function RectRaw:String(imagedata:Byte Ptr, bytesPerPixel:Int, bytesPerLine:Int, x:Int, y:Int, width:Int, height:Int)
		Return bmx_tess_rect(imagedata, bytesPerPixel, bytesPerLine, x, y, width, height)
	End Function


	Rem
	bbdoc: Frees up memory and forgets adaptive data.
	about: Call between pages or documents etc.
	End Rem
	Function Clear()
		bmx_tess_clear()
	End Function

	Rem
	bbdoc: Closes down tesseract and frees up memory.
	End Rem
	Function EndTess()
		bmx_tess_end()
	End Function

	Rem
	bbdoc: Checks whether a word is valid according to Tesseract's language model.
	returns: 0 if the string is invalid, non-zero if valid.
	End Rem
	Function IsValidWord:Int(word:String)
		Return bmx_tess_isvalidword(word)
	End Function


	' ye olde utf8 converter
	Function _convertUTF8ToMax:String(s:Byte Ptr)
		If s Then
			Local l:Int = _strlen(s)
			Local b:Short[] = New Short[l]
			Local bc:Int = -1
			Local c:Int
			Local d:Int
			Local e:Int
			For Local i:Int = 0 Until l
				bc:+1
				c = s[i]
				If c<128 
					b[bc] = c
					Continue
				End If
				i:+1
				d=s[i]
				If c<224 
					b[bc] = (c-192)*64+(d-128)
					Continue
				End If
				i:+1
				e = s[i]
				If c < 240 
					b[bc] = (c-224)*4096+(d-128)*64+(e-128)
					Continue
				End If
			Next
			Return String.fromshorts(b, bc + 1)
		End If
		
		Return ""
		
	End Function

End Type

