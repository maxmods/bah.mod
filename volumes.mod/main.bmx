' Copyright (c) 2007 Bruce A Henderson
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
