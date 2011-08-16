' Copyright 2008-2011 Bruce A Henderson
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


ModuleInfo "Version: 1.01"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: Wrapper - 2008-2011 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to Tesseract-3.x SVN rev 605."
ModuleInfo "History: Renamed Tess type to Tesseract. Tesseract can now use multiple instances instead of one global instance."
ModuleInfo "History: Latest Tesseract supports MinGW out of the box! :)"
ModuleInfo "History: "
ModuleInfo "History: 1.00 Initial Release (Tesseract-2.03 SVNr205)"


ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -DUSE_STD_NAMESPACE -DPI=3.14159265f"
ModuleInfo "CC_OPTS: -I../ccutil -I../cutil"
?win32
ModuleInfo "CC_OPTS: -D__MSW32__ -D__BLOB_T_DEFINED"
?

Import BRL.Pixmap

Import "common.bmx"


Rem
bbdoc: The Tesseract OCR engine.
about: Steps for success:
<ol>
<li>Call Init(datapath) to intialise the engine.</li>
<li>Call Rect(pixmap) to process an image and retrieve the text as String.</li>
<li>Optionally call Clear() between docs.</li>
<li>Optionally call EndTess() when you are finished using the engine.</li>
</ol>
End Rem
Type Tesseract

	Field tessPtr:Byte Ptr

	Method New()
		tessPtr = bmx_tess_new()
	End Method
	
	Method Delete()
		If tessPtr Then
			bmx_tess_free(tessPtr)
			tessPtr = Null
		End If
	End Method

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
	Method Init:Int(datapath:String, lang:String = "eng", numericMode:Int = False)
		Return bmx_tess_init(tessPtr, datapath, lang, numericMode)
	End Method

	Rem
	bbdoc: Sets the value of an internal 'parameter'.

	returns: False if the name lookup failed.
	about: Supply the name of the parameter and the value as a string, just as you would in a config file.
	<p>
	eg. Tess.SetVariable("tessedit_char_blacklist", "xyz") - to ignore x, y and z.<br>
	Or SetVariable("bln_numericmode", "1") - to set numeric-only mode.
	</p>
	<p>
	SetVariable may be used before Init, but settings will revert to defaults on EndTess().
	</p>
	<p>Parameters:
	<ul>
	<li><b>name</b> : Name of the variable to set.</li>
	<li><b>value</b> : New value for the variable.</li>
	</ul>
	</p>
	Note: Must be called after Init().
	End Rem
	Method SetVariable:Int(name:String, value:String)
		Return bmx_tess_setvariable(tessPtr, name, value)
	End Method
	
	Rem
	bbdoc: Fills in @value with the value of the parameter.
	returns: True if the parameter was found among Tesseract parameters, False otherwise.
	End Rem
	Method GetIntVariable:Int(name:String, value:Int Var)
		Return bmx_tess_getintvariable(tessPtr, name, Varptr value)
	End Method
	
	Rem
	bbdoc: Fills in @value with the value of the parameter.
	returns: True if the parameter was found among Tesseract parameters, False otherwise.
	End Rem
	Method GetBoolVariable:Int(name:String, value:Int Var)
		Return bmx_tess_getboolvariable(tessPtr, name, Varptr value)
	End Method
	
	Rem
	bbdoc: Fills in @value with the value of the parameter.
	returns: True if the parameter was found among Tesseract parameters, False otherwise.
	End Rem
	Method GetDoubleVariable:Int(name:String, value:Double Var)
		Return bmx_tess_getdoublevariable(tessPtr, name, Varptr value)
	End Method
	
	Rem
	bbdoc: Returns the value of a string variable, if found among Tesseract parameters.
	End Rem
	Method GetStringVariable:String(name:String)
		Return bmx_tess_getstringvariable(tessPtr, name)
	End Method

	Rem
	bbdoc: Recognizes a rectangle from a pixmap and returns the result as a string.
	about: May be called many times for a single Init. The default rectangle is for the whole pixmap.
	<p>
	Currently has no error checking.
	</p>
	<p>Parameters:
	<ul>
	<li><b>pixmap</b> : The pixmap to process. Will be converted up to 24bit if required.</li>
	<li><b>x</b> : X coordinate of process rectangle.</li>
	<li><b>y</b> : Y coordinate of process rectangle.</li>
	<li><b>width</b> : Width of rectangle.</li>
	<li><b>height</b> : Height of rectangle.</li>
	</ul>
	</p>
	End Rem
	Method Rect:String(pixmap:TPixmap, x:Int = -1, y:Int = -1, width:Int = -1, height:Int = -1)
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
	End Method

	Rem
	bbdoc: Recognizes a rectangle from raw pixel data and returns the result as a string.
	about: May be called many times for a single Init.
	<p>
	Currently has no error checking.
	</p>
	<p>Parameters:
	<ul>
	<li><b>imagedata</b> : Byte Ptr to the raw image data</li>
	<li><b>bytesPerPixel</b> : Number of bytes per pixel. eg. 4 for RGBA8888</li>
	<li><b>bytesPerLine</b> : The 'pitch' for the image.</li>
	<li><b>x</b> : X coordinate of process rectangle.</li>
	<li><b>y</b> : Y coordinate of process rectangle.</li>
	<li><b>width</b> : Width of rectangle.</li>
	<li><b>height</b> : Height of rectangle.</li>
	</ul>
	</p>
	End Rem
	Method RectRaw:String(imagedata:Byte Ptr, bytesPerPixel:Int, bytesPerLine:Int, x:Int, y:Int, width:Int, height:Int)
		Return bmx_tess_rect(tessPtr, imagedata, bytesPerPixel, bytesPerLine, x, y, width, height)
	End Method


	Rem
	bbdoc: Call between pages or documents etc to free up memory and forget adaptive data.
	End Rem
	Method ClearAdaptiveClassifier()
		bmx_tess_clearadaptiveclassifier(tessPtr)
	End Method

	Rem
	bbdoc: Closes down tesseract and free up all memory.
	about: EndTess() is equivalent to destructing and reconstructing your #Tesseract object. Once EndTess() has been used, none of the other API functions may be used
	other than Init and anything declared above it in the class definition.
	End Rem
	Method EndTess()
		bmx_tess_end(tessPtr)
	End Method

	Rem
	bbdoc: Checks whether a word is valid according to Tesseract's language model.
	returns: 0 if the string is invalid, non-zero if valid.
	about: Parameters:
	<ul>
	<li><b>word</b> : The word to test for validity.</li>
	</ul>
	End Rem
	Method IsValidWord:Int(word:String)
		Return bmx_tess_isvalidword(tessPtr, word)
	End Method

End Type

