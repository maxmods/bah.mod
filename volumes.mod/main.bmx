' Copyright (c) 2007-2011 Bruce A Henderson
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

Import BRL.LinkedList

Global volume_driver:TVolume

Rem
bbdoc: A system Volume
End Rem
Type TVolume

	Rem
	bbdoc:  True if available, false if not.
	End Rem
	Field available:Int

	Rem
	bbdoc: The volume name.
	End Rem
	Field volumeName:String
	
	Rem
	bbdoc: The device name
	End Rem
	Field volumeDevice:String
	
	Rem
	bbdoc: The system type.
	End Rem
	Field volumeType:String

	Rem
	bbdoc: Cached volume size (in bytes)
	about: For the current size, #Refresh first.
	End Rem
	Field volumeSize:Long
	
	Rem
	bbdoc: Cached free space (in bytes)
	about: For the current free space, #Refresh first.
	End Rem
	Field volumeFree:Long

	Rem
	bbdoc: Refreshes size/free info for the volume.
	End Rem
	Method Refresh() Abstract

	' platform specific implementations
	Method ListVolumes:TList() Abstract
	
	Method GetVolumeFreeSpace:Long(vol:String) Abstract
	
	Method GetVolumeSize:Long(vol:String) Abstract

	Method GetVolumeInfo:TVolume(vol:String) Abstract
	
	Method GetUserHomeDir:String() Abstract
	Method GetUserDesktopDir:String() Abstract
	Method GetUserAppDir:String() Abstract
	Method GetUserDocumentsDir:String() Abstract
	
	Method GetCustomDir:String(dirType:Int) Abstract
	
End Type


Rem
bbdoc: Returns a list of volumes on the system.
End Rem
Function ListVolumes:TList()

	Return volume_driver.ListVolumes()

End Function

Rem
bbdoc: Returns the amount of free space (in bytes) on the given volume.
about: Parameters:
<ul>
<li><b>vol</b> : the name of the volume</li>
</ul>
End Rem
Function GetVolumeFreeSpace:Long(vol:String)

	Return volume_driver.GetVolumeFreeSpace(vol)

End Function

Rem
bbdoc: Returns the size (in bytes) of the given volume.
about: Parameters:
<ul>
<li><b>vol</b> : the name of the volume</li>
</ul>
End Rem
Function GetVolumeSize:Long(vol:String)

	Return volume_driver.GetVolumeSize(vol)

End Function

Rem
bbdoc: Populates and returns a #TVolume object.
about: Parameters:
<ul>
<li><b>vol</b> : the name of the volume</li>
</ul>
End Rem
Function GetVolumeInfo:TVolume(vol:String)

	Return volume_driver.GetVolumeInfo(vol)

End Function

Rem
bbdoc: Returns the user home directory.
about: The table lists examples for the various platforms -
<table align="center">
<tr><th>Platform</th><th>Example</th><th>Equivalent</th></tr>
<tr><td>Linux</td><td>/home/username</td><td>~</td></tr>
<tr><td>Mac OS</td><td>/Users/username</td><td>~</td></tr>
<tr><td>Win32</td><td>C:\Documents and Settings\username</td><td>&nbsp;</td></tr>
</table>
End Rem
Function GetUserHomeDir:String()
	Return volume_driver.GetUserHomeDir()
End Function

Rem
bbdoc: Returns the user Desktop directory.
about: The table lists examples for the various platforms -
<table align="center">
<tr><th>Platform</th><th>Example</th><th>Equivalent</th></tr>
<tr><td>Linux</td><td>/home/username/Desktop</td><td>~/Desktop</td></tr>
<tr><td>Mac OS</td><td>/Users/username/Desktop</td><td>~/Desktop</td></tr>
<tr><td>Win32</td><td>C:\Documents and Settings\username\Desktop</td><td>&nbsp;</td></tr>
</table>
End Rem
Function GetUserDesktopDir:String()
	Return volume_driver.GetUserDesktopDir()
End Function

Rem
bbdoc: Returns the user directory for application data.
about: The table lists examples for the various platforms -
<table align="center">
<tr><th>Platform</th><th>Example</th><th>Equivalent</th></tr>
<tr><td>Linux</td><td>/home/username</td><td>~</td></tr>
<tr><td>Mac OS</td><td>/Users/username/Library/Application Support</td><td>~/Library/Application Support</td></tr>
<tr><td>Win32</td><td>C:\Documents and Settings\username\Application Data</td><td>&nbsp;</td></tr>
</table>
End Rem
Function GetUserAppDir:String()
	Return volume_driver.GetUserAppDir()
End Function

Rem
bbdoc: Returns the user Documents directory.
about: The table lists examples for the various platforms -
<table align="center">
<tr><th>Platform</th><th>Example</th><th>Equivalent</th></tr>
<tr><td>Linux</td><td>/home/username/Documents</td><td>~/Documents</td></tr>
<tr><td>Mac OS</td><td>/Users/username/Documents</td><td>~/Documents</td></tr>
<tr><td>Win32</td><td>C:\Documents and Settings\username\My Documents</td><td>&nbsp;</td></tr>
</table>
End Rem
Function GetUserDocumentsDir:String()
	Return volume_driver.GetUserDocumentsDir()
End Function

Rem
bbdoc: Returns the custom directory path.
about: The following table lists valid @dirType -
<table align="center">
<tr><th>Platform</th><th>dirType</th><th>Description</th></tr>
<tr><td>Mac, Linux</td><td>DT_SHAREDUSERDATA</td><td>The Shared documents folder.</td></tr>
<tr><td>All</td><td>DT_USERPICTURES</td><td>The &quot;Pictures&quot; or &quot;My Pictures&quot; folder of the user.</td></tr>
<tr><td>All</td><td>DT_USERMUSIC</td><td>The &quot;Music&quot; or &quot;My Music&quot; folder of the user.</td></tr>
<tr><td>All</td><td>DT_USERMOVIES</td><td>The &quot;Movies&quot;, &quot;Videos&quot; or &quot;My Videos&quot; folder of the user.</td></tr>
<tr><td>Win32</td><td>CSIDL_xxxxxxxx</td><td>Any of the Windows-specific CSIDL identifiers that represent different folders on the system.</td></tr>
</table>
<p>Returns Null if @dirType is not valid for the platform.</p>
End Rem
Function GetCustomDir:String(dirType:Int)
	Return volume_driver.GetCustomDir(dirType)
End Function

' custom dir types

Const DT_SHAREDUSERDATA:Int = -1
Const DT_USERPICTURES:Int = -2
Const DT_USERMUSIC:Int = -3
Const DT_USERMOVIES:Int = -4


'
' NOTE : temporary home until we get official support for this function
'
Extern "c"
	Function strlen_( str:Byte Ptr )="strlen"
End Extern

Function bbStringFromUTF8String:String(s:Byte Ptr)
	If s Then
		Local l:Int = strlen_(s)
		Local b:Short[] = New Short[l]
		Local bc:Int = -1, c:Int, d:Int, e:Int
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
