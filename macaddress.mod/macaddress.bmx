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
bbdoc: MAC address access.
End Rem
Module BaH.MACAddress

' Volodymir (T-Rex) Tryapichko

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: MACAddressUtility - Volodymir (T-Rex) Tryapichko, 2009"
ModuleInfo "Copyright: Wrapper - 2010 Bruce A Henderson"

ModuleInfo "Version: 1.00"
ModuleInfo "History: Initial Release"



Import BRL.Blitz
Import "*.h"
Import "glue.cpp"
Import "MACAddressUtility.cpp"

?win32
Import "-lrpcrt4"
?

Extern
	Function bmx_getmacaddress:Int[]()
	Function bmx_getmacaddressstring:String(uppercase:Int)
End Extern


Rem
bbdoc: Retrieves the MAC address as an array of 6 ints.
End Rem
Function GetMACAddress:Int[]()
	Return bmx_getmacaddress()
End Function

Rem
bbdoc: Retrieves the MAC address as a String of hex values, in the form 00:00:00:00:00:00.
about: By default the characters are returned as upper case. Set @uppercase to False for lower case characters.
End Rem
Function GetMACAddressString:String(uppercase:Int = True)
	Return bmx_getmacaddressstring(uppercase)
End Function

