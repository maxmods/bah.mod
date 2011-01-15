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


Import "source.bmx"


Extern
	'Function _strlen:Int(s:Byte Ptr) = "strlen"

	Function bmx_tess_new:Byte Ptr()
	Function bmx_tess_free(handle:Byte Ptr)
	Function bmx_tess_init:Int(handle:Byte Ptr, datapath:String, lang:String, numericMode:Int)
	Function bmx_tess_rect:String(handle:Byte Ptr, imagedata:Byte Ptr, bytesPerPixel:Int, bytesPerLine:Int, x:Int, y:Int, width:Int, height:Int)
	Function bmx_tess_setvariable:Int(handle:Byte Ptr, variable:String, value:String)
	Function bmx_tess_clearadaptiveclassifier(handle:Byte Ptr)
	Function bmx_tess_end(handle:Byte Ptr)
	Function bmx_tess_isvalidword:Int(handle:Byte Ptr, word:String)
	Function bmx_tess_setvariableifinit:Int(handle:Byte Ptr, name:String, value:String)
	Function bmx_tess_getintvariable:Int(handle:Byte Ptr, name:String, value:Int Ptr)
	Function bmx_tess_getboolvariable:Int(handle:Byte Ptr, name:String, value:Int Ptr)
	Function bmx_tess_getdoublevariable:Int(handle:Byte Ptr, name:String, value:Double Ptr)
	Function bmx_tess_getstringvariable:String(handle:Byte Ptr, name:String)


End Extern