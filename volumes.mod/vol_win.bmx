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

Import BRL.Bank
Import BRL.LinkedList
Import Pub.Win32
Import BRL.Map

Import "main.bmx"

Import "-lkernel32"
Import "-lshell32"

Extern "win32"
	Function GetDiskFreeSpaceEx:Int(lpDirectoryName:Short Ptr, lpFreeBytesAvailableToCaller:Long Var, lpTotalNumberOfBytes:Long Var, lpTotalNumberOfFreeBytes:Long Var) = "GetDiskFreeSpaceExW@16"
	Function GetVolumeInformation:Int(lpRootPathName:Short Ptr, lpVolumeNameBuffer:Short Ptr, nVolumeNameSize:Int, ..
		lpVolumeSerialNumber:Int Var, lpMaximumComponentLength:Int Var, lpFileSystemFlags:Int Var, lpFileSystemNameBuffer:Short Ptr, nFileSystemNameSize:Int) = "GetVolumeInformationW@32"
	Function GetLogicalDrives:Int() = "GetLogicalDrives@0"
	Function SetErrorMode:Int(Mode:Int) = "SetErrorMode@4"

	' volumes
	Function FindFirstVolume:Int(volumeName:Short Ptr, bufferSize:Int) = "FindFirstVolumeW@8"
	Function FindNextVolume:Int(handle:Int, volumeName:Short Ptr, bufferSize:Int) = "FindNextVolumeW@12"
	Function FindVolumeClose:Int(handle:Int) = "FindVolumeClose@4"
	
	' volume paths
	Function GetVolumePathNamesForVolumeName:Int(volumeName:Short Ptr, volumePaths:Short Ptr, bufferSize:Int, copiedSize:Int Ptr) = "GetVolumePathNamesForVolumeNameW@16"
	
	Function SHGetFolderPath:Int(hwndOwner:Byte Ptr, nFolder:Int, hToken:Byte Ptr, dwFlags:Int, pszPath:Short Ptr) = "SHGetFolderPathW@20"
End Extern

Const CSIDL_ADMINTOOLS:Int = $0030
Const CSIDL_ALTSTARTUP:Int = $001d
Const CSIDL_APPDATA:Int = $001a
Const CSIDL_BITBUCKET:Int = $000a
Const CSIDL_CDBURN_AREA:Int = $003b
Const CSIDL_COMMON_ADMINTOOLS:Int = $002f
Const CSIDL_COMMON_ALTSTARTUP:Int = $001e
Const CSIDL_COMMON_APPDATA:Int = $0023
Const CSIDL_COMMON_DESKTOPDIRECTORY:Int = $0019
Const CSIDL_COMMON_DOCUMENTS:Int = $002e
Const CSIDL_COMMON_FAVORITES:Int = $001f
Const CSIDL_COMMON_MUSIC:Int = $0035
Const CSIDL_COMMON_OEM_LINKS:Int = $003a
Const CSIDL_COMMON_PICTURES:Int = $0036
Const CSIDL_COMMON_PROGRAMS:Int = $0017
Const CSIDL_COMMON_STARTMENU:Int = $0016
Const CSIDL_COMMON_STARTUP:Int = $0018
Const CSIDL_COMMON_TEMPLATES:Int = $002d
Const CSIDL_COMMON_VIDEO:Int = $0037
Const CSIDL_COMPUTERSNEARME:Int = $003d
Const CSIDL_CONNECTIONS:Int = $0031
Const CSIDL_CONTROLS:Int = $0003
Const CSIDL_COOKIES:Int = $0021
Const CSIDL_DESKTOP:Int = $0000
Const CSIDL_DESKTOPDIRECTORY:Int = $0010
Const CSIDL_DRIVES:Int = $0011
Const CSIDL_FAVORITES:Int = $0006
Const CSIDL_FLAG_CREATE:Int = $8000
Const CSIDL_FLAG_DONT_VERIFY:Int = $4000
Const CSIDL_FLAG_MASK:Int = $FF00
Const CSIDL_FLAG_NO_ALIAS:Int = $1000
Const CSIDL_FLAG_PER_USER_INIT:Int = $0800
Const CSIDL_FONTS:Int = $0014
Const CSIDL_HISTORY:Int = $0022
Const CSIDL_INTERNET:Int = $0001
Const CSIDL_INTERNET_CACHE:Int = $0020
Const CSIDL_LOCAL_APPDATA:Int = $001c
Const CSIDL_MYMUSIC:Int = $000d
Const CSIDL_MYPICTURES:Int = $0027
Const CSIDL_MYVIDEO:Int = $000e
Const CSIDL_NETHOOD:Int = $0013
Const CSIDL_NETWORK:Int = $0012
Const CSIDL_PERSONAL:Int = $0005
Const CSIDL_PRINTERS:Int = $0004
Const CSIDL_PRINTHOOD:Int = $001b
Const CSIDL_PROFILE:Int = $0028
Const CSIDL_PROGRAM_FILES:Int = $0026
Const CSIDL_PROGRAM_FILES_COMMON:Int = $002b
Const CSIDL_PROGRAM_FILES_COMMONX86:Int = $002c
Const CSIDL_PROGRAM_FILESX86:Int = $002a
Const CSIDL_PROGRAMS:Int = $0002
Const CSIDL_RECENT:Int = $0008
Const CSIDL_RESOURCES:Int = $0038
Const CSIDL_RESOURCES_LOCALIZED:Int = $0039
Const CSIDL_SENDTO:Int = $0009
Const CSIDL_STARTMENU:Int = $000b
Const CSIDL_STARTUP:Int = $0007
Const CSIDL_SYSTEM:Int = $0025
Const CSIDL_SYSTEMX86:Int = $0029
Const CSIDL_TEMPLATES:Int = $0015
Const CSIDL_WINDOWS:Int = $0024
Const CSIDL_MYDOCUMENTS:Int = CSIDL_PERSONAL

Const SHGFP_TYPE_CURRENT:Int = 0

Const SEM_FAILCRITICALERRORS:Int = 1

Global winVolume_driver:TWinVolumeDriver = New TWinVolumeDriver

Type TWinVolumeDriver

	Method New()
		volume_driver = TWinVolume.Create()
	End Method

End Type


Type TWinVolume Extends TVolume

	Const PATH_MAX:Int = $104

	Field vs:TVolSpace

	Function Create:TWinVolume()
		Local this:TWinVolume = New TWinVolume
		
		Return this
	End Function

	Method ListVolumes:TList()
		Local volumes:TMap

		' create buffer
		Local nameBuffer:Short[] = New Short[PATH_MAX]
		Local mpBuffer:Short[] = New Short[PATH_MAX]
		
		' get the first volume
		Local handle:Int = FindFirstVolume(nameBuffer, PATH_MAX)
		If handle Then

			volumes = New TMap

			While True

				' retrieve the paths
				Local pathsBuffer:Short[] = New Short[PATH_MAX]
				Local bufferSize:Int
				
				Local paths:String[]

				If GetVolumePathNamesForVolumeName(nameBuffer, pathsBuffer, Self.PATH_MAX, Varptr bufferSize) Then

					paths = String.FromShorts(pathsBuffer, bufferSize).Trim().split("~0")

				' Some error occured – if the buffer was too small we will set it to the
				' right size and try it again
				Else If bufferSize > PATH_MAX Then

					If GetVolumePathNamesForVolumeName(nameBuffer, pathsBuffer, bufferSize, Varptr bufferSize) Then
						paths = String.FromShorts(pathsBuffer, bufferSize).Trim().split("~0")
					EndIf

				EndIf

				For Local path:String = EachIn paths
					volumes.Insert(path, GetVolumeInfo(path))
				Next

				' get the next volume or quit the loop if there is none
				If Not FindNextVolume(handle, nameBuffer, PATH_MAX) Then
					Exit
				End If
			Wend

			' end the volumes search
			FindVolumeClose(handle)

		End If


		' now look for missing drives...
		Local bitmap:Int = GetLogicalDrives()
		
		For Local i:Int = 1 To 26
			If bitmap & 1 Then

				If Not volumes Then
					volumes = New TMap
				End If
				
				Local path:String = Chr(64 + i) + ":\"
				
				If Not volumes.Contains(path) Then
					volumes.Insert(path, GetVolumeInfo(path))
				End If
				
			End If
			
			bitmap:Shr 1
		Next
		
		' return a list of volumes
		If volumes Then
			Local list:TList = New TList
			For Local volume:TVolume = EachIn volumes.Values()
				list.AddLast(volume)
			Next
			
			Return list
		Else
			Return Null
		End If

	End Method
	
	Method GetVolumeFreeSpace:Long(vol:String)

		Local _vs:TVolSpace = TVolSpace.GetDiskSpace(vol)
		
		Return _vs.fb
	End Method

	Method GetVolumeSize:Long(vol:String)

		Local _vs:TVolSpace = TVolSpace.GetDiskSpace(vol)
		
		Return _vs.tb
	End Method
	
	Method GetVolumeInfo:TVolume(vol:String)
		Local Mode:Int = SetErrorMode(SEM_FAILCRITICALERRORS)

		Local volume:TWinVolume = New TWinVolume
		
		volume.volumeDevice = vol

		Local volname:Short[PATH_MAX]
		Local filesys:Short[PATH_MAX]
		Local snum:Int
		Local maxLength:Int
		Local flags:Int

		Local ret:Int = GetVolumeInformation(volume.volumeDevice, volname, PATH_MAX, ..
			snum, maxLength, flags, filesys, PATH_MAX)

		If ret Then
			volume.volumeName = String.fromWString(volname)
			volume.volumeType = String.fromWString(filesys)
			
			volume.vs = TVolSpace.GetDiskSpace(volume.volumeDevice)
			volume.volumeSize = volume.vs.tb
			volume.volumeFree = volume.vs.fb
			
			volume.available = True
		End If
		
		SetErrorMode(Mode)
				
		Return volume
	End Method

	Method Refresh()
		If Not vs Then
			Return
		End If
		
		Local ret:Int = vs.refresh()
		
		If ret Then
			volumeSize = vs.tb
			volumeFree = vs.fb
			
			available = True
		Else
			available = False
		End If
		
	End Method

	Method GetUserHomeDir:String()
		Return _getFolderPath(CSIDL_PROFILE)
	End Method
	
	Method GetUserDesktopDir:String()
		Return _getFolderPath(CSIDL_DESKTOPDIRECTORY)
	End Method
	
	Method GetUserAppDir:String()
		Return _getFolderPath(CSIDL_APPDATA)
	End Method
	
	Method GetUserDocumentsDir:String()
		Return _getFolderPath(CSIDL_PERSONAL)
	End Method

	Method GetCustomDir:String(dirType:Int, flags:Int = 0)
		If dirType < 0 Then
			Select dirType
				Case DT_USERPICTURES
					Return _getFolderPath(CSIDL_MYPICTURES)
				Case DT_USERMUSIC
					Return _getFolderPath(CSIDL_MYMUSIC)
				Case DT_USERMOVIES
					Return _getFolderPath(CSIDL_MYVIDEO)
			End Select
		Else
			' assume these are CSIDL paths
			Return _getFolderPath(dirType)
		End If
		
		Return Null
	End Method
	
	Method _getFolderPath:String(kind:Int)
		Local b:Short[] = New Short[MAX_PATH]
		
		Local ret:Int = SHGetFolderPath(Null, kind, Null, SHGFP_TYPE_CURRENT, b)
		
		Return String.fromWString(b)
	End Method
	
End Type

Type TVolSpace
	Field vol:String
	
	Field fbc:Long
	Field tb:Long
	Field fb:Long
	
	Function GetDiskSpace:TVolSpace(vol:String)
		Local this:TVolSpace = New TVolSpace
		
		Local dir:Short Ptr = vol.toWString()

		Local ret:Int = GetDiskFreeSpaceEx(dir, this.fbc, this.tb, this.fb)
		
		If dir Then
			MemFree(dir)
		End If
		
		Return this
	End Function
	
	Method refresh:Int()
		Local Mode:Int = SetErrorMode(SEM_FAILCRITICALERRORS)
		
		Local dir:Short Ptr = vol.toWString()

		Local ret:Int = GetDiskFreeSpaceEx(dir, fbc, tb, fb)
		
		If dir Then
			MemFree(dir)
		End If
		
		SetErrorMode(Mode)
		Return ret
	End Method

End Type
