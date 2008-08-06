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

Import BRL.Bank
Import BRL.LinkedList
Import Pub.Win32

Import "main.bmx"

Import "-lkernel32"
Import "-lshell32"

Extern "win32"
	Function GetDiskFreeSpaceEx:Int(lpDirectoryName:Byte Ptr, lpFreeBytesAvailableToCaller:Long Var, lpTotalNumberOfBytes:Long Var, lpTotalNumberOfFreeBytes:Long Var) = "GetDiskFreeSpaceExA@16"
	Function GetVolumeInformation:Int(lpRootPathName:Byte Ptr, lpVolumeNameBuffer:Byte Ptr, nVolumeNameSize:Int, ..
		lpVolumeSerialNumber:Int Var, lpMaximumComponentLength:Int Var, lpFileSystemFlags:Int Var, lpFileSystemNameBuffer:Byte Ptr, nFileSystemNameSize:Int) = "GetVolumeInformationA@32"
	Function GetLogicalDrives:Int() = "GetLogicalDrives@0"
	Function SetErrorMode:Int(mode:Int) = "SetErrorMode@4"
	
	Function SHGetFolderPath:Int(hwndOwner:Byte Ptr, nFolder:Int, hToken:Byte Ptr, dwFlags:Int, pszPath:Short Ptr) = "SHGetFolderPathW@20"
End Extern

Const CSIDL_APPDATA:Int = $001A
Const CSIDL_DESKTOPDIRECTORY:Int = $0010
Const CSIDL_PERSONAL:Int = $0005
Const CSIDL_PROFILE:Int = $0028

Const SHGFP_TYPE_CURRENT:Int = 0

Const SEM_FAILCRITICALERRORS:Int = 1

Global winVolume_driver:TWinVolumeDriver = New TWinVolumeDriver

Type TWinVolumeDriver

	Method New()
		volume_driver = TWinVolume.Create()
	End Method

End Type


Type TWinVolume Extends TVolume

	Field vs:TVolSpace

	Function Create:TWinVolume()
		Local this:TWinVolume = New TWinVolume
		
		Return this
	End Function

	Method ListVolumes:TList()
		Local volumes:TList

		Local bitmap:Int = GetLogicalDrives()
		
		For Local i:Int = 1 To 26
			If bitmap & 1 Then

				If Not volumes Then
					volumes = New TList
				End If
				
				Local volume:TVolume = GetVolumeInfo(Chr(64 + i) + ":\")
				
				volumes.addLast(volume)
				
			End If
			
			bitmap:Shr 1
		Next


		Return volumes
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
		Local mode:Int = SetErrorMode(SEM_FAILCRITICALERRORS)

		Local volume:TWinVolume = New TWinVolume
		
		volume.volumeDevice = vol

		Local volname:TBank = CreateBank(256)
		Local filesys:TBank = CreateBank(256)
		Local snum:Int
		Local maxLength:Int
		Local flags:Int

		Local ret:Int = GetVolumeInformation(volume.volumeDevice, LockBank(volname), 256, ..
			snum, maxLength, flags, LockBank(filesys), 256)

		If ret Then
			volume.volumeName = String.fromCString(volname.buf())
			volume.volumeType = String.fromCString(filesys.buf())
			
			volume.vs = TVolSpace.GetDiskSpace(volume.volumeDevice)
			volume.volumeSize = volume.vs.tb
			volume.volumeFree = volume.vs.fb
			
			volume.available = True
		End If
		
		UnlockBank(volname)
		UnlockBank(filesys)
		
		SetErrorMode(mode)
				
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
		Local b:Short[] = New Short[MAX_PATH]
		
		Local ret:Int = SHGetFolderPath(Null, CSIDL_PROFILE, Null, SHGFP_TYPE_CURRENT, b)
		
		Return String.fromWString(b)
	End Method
	
	Method GetUserDesktopDir:String()
		Local b:Short[] = New Short[MAX_PATH]
		
		Local ret:Int = SHGetFolderPath(Null, CSIDL_DESKTOPDIRECTORY, Null, SHGFP_TYPE_CURRENT, b)
		
		Return String.fromWString(b)
	End Method
	
	Method GetUserAppDir:String()
		Local b:Short[] = New Short[MAX_PATH]
		
		Local ret:Int = SHGetFolderPath(Null, CSIDL_APPDATA, Null, SHGFP_TYPE_CURRENT, b)
		
		Return String.fromWString(b)
	End Method
	
	Method GetUserDocumentsDir:String()
		Local b:Short[] = New Short[MAX_PATH]
		
		Local ret:Int = SHGetFolderPath(Null, CSIDL_PERSONAL, Null, SHGFP_TYPE_CURRENT, b)
		
		Return String.fromWString(b)
	End Method

	Method GetCustomDir:String(dirType:Int)
		Return Null
	End Method
	
End Type

Type TVolSpace
	Field vol:String
	
	Field fbc:Long
	Field tb:Long
	Field fb:Long
	
	Function GetDiskSpace:TVolSpace(vol:String)
		Local this:TVolSpace = New TVolSpace
		
		Local dir:Byte Ptr = vol.toCString()

		Local ret:Int = GetDiskFreeSpaceEx(dir, this.fbc, this.tb, this.fb)
		
		If dir Then
			MemFree(dir)
		End If
		
		Return this
	End Function
	
	Method refresh:Int()
		Local mode:Int = SetErrorMode(SEM_FAILCRITICALERRORS)
		
		Local dir:Byte Ptr = vol.toCString()

		Local ret:Int = GetDiskFreeSpaceEx(dir, fbc, tb, fb)
		
		If dir Then
			MemFree(dir)
		End If
		
		SetErrorMode(mode)
		Return ret
	End Method

End Type
