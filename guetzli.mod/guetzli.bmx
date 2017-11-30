' 
' Copyright 2017 Bruce A Henderson
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
bbdoc: Guetzli JPEG Saver
End Rem
Module BaH.Guetzli

ModuleInfo "Version: 1.01"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: Wrapper - 2017 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to latest guetzli (cb5e4a8)"
ModuleInfo "History: Added user-TStream support."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."

ModuleInfo "CPP_OPTS: -std=c++11"

Import BRL.Stream
Import BRL.Pixmap

Import "common.bmx"


' Notes
'
' MacOS version minimum = 10.9
'


Private

Function writefunc:Int( buf:Byte Ptr,count:Int,src:Object )
	Local stream:TStream = TStream(src)
	Return stream.Write( buf,count )
End Function

Public

Rem
bbdoc: Saves pixmap as Jpeg.
End Rem
Function SavePixmapJPeg:Int( pixmap:TPixmap, url:Object, quality:Int = DEFAULT_JPEQ_QUALITY )

	Assert quality>=1 And quality<=100

	Local stream:TStream
	
	If TStream(url) Then
		stream = TStream(url)
	Else	
		stream = WriteStream( url )
	End If
	
	If Not stream Return False
	
	pixmap = pixmap.convert(PF_RGB888)

	Local pix:Byte Ptr = pixmap.PixelPtr( 0,0 )

	bmx_guetzli_savejpg(stream, writefunc, pixmap.width, pixmap.height, pixmap.pitch, pix, quality)

	' only close if we created the stream
	If Not TStream(url) Then
		stream.Close
	End If
	
	Return True
End Function


