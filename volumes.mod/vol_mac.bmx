' Copyright (c) 2007-2016 Bruce A Henderson
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
Import BRL.Bank

Import "main.bmx"
Import "macglue.c"

Extern

	Function FSFindFolder:Int( vRefNum:Int ,folderType:Int ,createFolder:Int ,foundRef:Byte Ptr )
	Function FSRefMakePath:Int( ref:Byte Ptr,path:Byte Ptr, maxPath:Int )
	
	Function bmx_vol_statfs_new:Byte Ptr()
	Function bmx_vol_statfs_free(fsPtr:Byte Ptr)
	Function bmx_vol_statfs:Int(vol:String, fsPtr:Byte Ptr)

	Function bmx_vol_getfsstat:Int(buf:Byte Ptr Ptr)
	Function bmx_vol_getfsstat_free(buf:Byte Ptr)
	Function bmx_vol_statfs_array_copy(fsPtr:Byte Ptr, buf:Byte Ptr, index:Int)
	Function bmx_vol_statfs_bsize:Int(fsPtr:Byte Ptr)
	Function bmx_vol_statfs_iosize:Int(fsPtr:Byte Ptr)
	Function bmx_vol_statfs_blocks(fsPtr:Byte Ptr, v:Long Ptr)
	Function bmx_vol_statfs_bfree(fsPtr:Byte Ptr, v:Long Ptr)
	Function bmx_vol_statfs_bavail(fsPtr:Byte Ptr, v:Long Ptr)
	Function bmx_vol_statfs_files(fsPtr:Byte Ptr, v:Long Ptr)
	Function bmx_vol_statfs_ffree(fsPtr:Byte Ptr, v:Long Ptr)
	Function bmx_vol_statfs_mntfromname:String(fsPtr:Byte Ptr)
	Function bmx_vol_statfs_mntonname:String(fsPtr:Byte Ptr)
	Function bmx_vol_statfs_fstypename:String(fsPtr:Byte Ptr)

End Extern

Rem
bbdoc: Read-only system hierarchy.
End Rem
Const kSystemDomain:Int = -32766
Rem
bbdoc: All users of a single machine have access to these resources.
End Rem
Const kLocalDomain:Int = -32765
Rem
bbdoc: All users configured to use a common network server has access to these resources.
End Rem
Const kNetworkDomain:Int = -32764
Rem
bbdoc: Resources that are private to the user.
about: Read/write.
End Rem
Const kUserDomain:Int = -32763
Const kDesktopFolderType:Int = Asc("d") Shl 24 | Asc("e") Shl 16 | Asc( "s") Shl 8 | Asc("k")
Const kCurrentUserFolderType:Int = Asc("c") Shl 24 | Asc("u") Shl 16 | Asc( "s") Shl 8 | Asc("r")
Const kApplicationSupportFolderType:Int = Asc("a") Shl 24 | Asc("s") Shl 16 | Asc( "u") Shl 8 | Asc("p")
Const kDocumentsFolderType:Int = Asc("d") Shl 24 | Asc("o") Shl 16 | Asc( "c") Shl 8 | Asc("s")
Const kSharedUserDataFolderType:Int = Asc("s") Shl 24 | Asc("d") Shl 16 | Asc( "a") Shl 8 | Asc("t")
Const kPictureDocumentsFolderType:Int = Asc("p") Shl 24 | Asc("d") Shl 16 | Asc( "o") Shl 8 | Asc("c")
Const kMusicDocumentsFolderType:Int = $B5646F63
Const kMovieDocumentsFolderType:Int = Asc("m") Shl 24 | Asc("d") Shl 16 | Asc( "o") Shl 8 | Asc("c")
Const kTrashFolderType:Int = Asc("t") Shl 24 | Asc("r") Shl 16 | Asc("s") Shl 8 | Asc("h") 
Const kWhereToEmptyTrashFolderType:Int = Asc("e") Shl 24 | Asc("m") Shl 16 | Asc("p") Shl 8 | Asc("t") 
Const kFontsFolderType:Int = Asc("f") Shl 24 | Asc("o") Shl 16 | Asc("n") Shl 8 | Asc("t") 
Const kPreferencesFolderType:Int = Asc("p") Shl 24 | Asc("r") Shl 16 | Asc("e") Shl 8 | Asc("f") 
Const kSystemPreferencesFolderType:Int = Asc("s") Shl 24 | Asc("p") Shl 16 | Asc("r") Shl 8 | Asc("f") 
Const kTemporaryFolderType:Int = Asc("t") Shl 24 | Asc("e") Shl 16 | Asc("m") Shl 8 | Asc("p") 
Const kChewableItemsFolderType:Int = Asc("f") Shl 24 | Asc("l") Shl 16 | Asc("n") Shl 8 | Asc("t") 
Const kTemporaryItemsInCacheDataFolderType:Int = Asc("v") Shl 24 | Asc("t") Shl 16 | Asc("m") Shl 8 | Asc("p") 
Const kApplicationsFolderType:Int = Asc("a") Shl 24 | Asc("p") Shl 16 | Asc("p") Shl 8 | Asc("s") 
Const kVolumeRootFolderType:Int = Asc("r") Shl 24 | Asc("o") Shl 16 | Asc("o") Shl 8 | Asc("t") 
Const kDomainTopLevelFolderType:Int = Asc("d") Shl 24 | Asc("t") Shl 16 | Asc("o") Shl 8 | Asc("p") 
Const kDomainLibraryFolderType:Int = Asc("d") Shl 24 | Asc("l") Shl 16 | Asc("i") Shl 8 | Asc("b") 
Const kUsersFolderType:Int = Asc("u") Shl 24 | Asc("s") Shl 16 | Asc("r") Shl 8 | Asc("s")
Const kInternetSitesFolderType:Int = Asc("s") Shl 24 | Asc("i") Shl 16 | Asc("t") Shl 8 | Asc("e") 
Const kPublicFolderType:Int = Asc("p") Shl 24 | Asc("u") Shl 16 | Asc("b") Shl 8 | Asc("b") 


Type Tstatfs

	Field fsPtr:Byte Ptr

	Method New()
		fsPtr = bmx_vol_statfs_new()
	End Method
	
	Method copy(buf:Byte Ptr, index:Int)
		bmx_vol_statfs_array_copy(fsPtr, buf, index)
	End Method
	
	Method _bsize:Int()
		Return bmx_vol_statfs_bsize(fsPtr)
	End Method

	Method _iosize:Int()
		Return bmx_vol_statfs_iosize(fsPtr)
	End Method

	Method _blocks:Long()
		Local v:Long
		bmx_vol_statfs_blocks(fsPtr, Varptr v)
		Return v
	End Method

	Method _bfree:Long()
		Local v:Long
		bmx_vol_statfs_bfree(fsPtr, Varptr v)
		Return v
	End Method

	Method _bavail:Long()
		Local v:Long
		bmx_vol_statfs_bavail(fsPtr, Varptr v)
		Return v
	End Method
	
	Method _files:Long()
		Local v:Long
		bmx_vol_statfs_files(fsPtr, Varptr v)
		Return v
	End Method
	
	Method _ffree:Long()
		Local v:Long
		bmx_vol_statfs_ffree(fsPtr, Varptr v)
		Return v
	End Method
	
	Method size:Long()
		Return _bsize() * _blocks()
	End Method

	Method sizeFree:Long()
		Return _bsize() * _bavail()
	End Method
	
	Method mountedFileSystem:String()
		Return bmx_vol_statfs_mntfromname(fsPtr)
	End Method
	
	Method dirOfMount:String()
		Return bmx_vol_statfs_mntonname(fsPtr)
	End Method

	Method fileSystemType:String()
		Return bmx_vol_statfs_fstypename(fsPtr)
	End Method
	
	Method Delete()
		If fsPtr Then
			bmx_vol_statfs_free(fsPtr)
			fsPtr = Null
		End If
	End Method
	
End Type

Global macVolume_driver:TMacVolumeDriver = New TMacVolumeDriver

Type TMacVolumeDriver

	Method New()
		volume_driver = TMacVolume.Create()
	End Method

End Type


Type TMacVolume Extends TVolume

	Field vs:TVolSpace

	Function Create:TMacVolume()
		Local this:TMacVolume = New TMacVolume
		
		Return this
	End Function


	Method Refresh()
	
		If Not vs Then
			Return
		End If
		
		vs.refresh()
		
		If vs Then
			volumeSize = vs.size()
			volumeFree = vs.free()
			available = True
		Else
			available = False
		End If

	End Method

	Method ListVolumes:TList()
		Local volumes:TList

		' temp buffer for array of structs
		Local b:Byte Ptr
		
		Local count:Int = bmx_vol_getfsstat(Varptr b)
	
		For Local i:Int = 0 Until count
		
			If Not volumes Then
				volumes = New TList
			End If
		
			Local volume:TMacVolume = New TMacVolume

			volume.vs = New TVolSpace
			volume.vs.fs.copy(b, i)
			
			volume.volumeName = volume.vs.fs.dirOfMount()
			volume.volumeDevice = volume.vs.fs.mountedFileSystem()
			volume.volumeType = volume.vs.fs.fileSystemType()
		
			volume.volumeSize = volume.vs.size()
			volume.volumeFree = volume.vs.free()
	
			volume.available = True
			
			volumes.addLast(volume)
		Next
		
		' free buffer
		bmx_vol_getfsstat_free(b)
	
		Return volumes
	End Method
	
	Method GetVolumeFreeSpace:Long(vol:String)
	
		Local _vs:TVolSpace = TVolSpace.GetDiskSpace(vol)
		
		If _vs Then
			Return _vs.free()
		End If
		
		Return 0
	End Method
	
	Method GetVolumeSize:Long(vol:String)
	
		Local _vs:TVolSpace = TVolSpace.GetDiskSpace(vol)

		If _vs Then
			Return _vs.size()
		End If
		
		Return 0
	End Method

	Method GetVolumeInfo:TVolume(vol:String)
		Local volume:TMacVolume = New TMacVolume


		Local vs:TVolSpace = TVolSpace.GetDiskSpace(vol)
		If vs Then
			volume.volumeName = vs.fs.dirOfMount()
			volume.volumeDevice = vs.fs.mountedFileSystem()
			volume.volumeType = vs.fs.fileSystemType()

			volume.volumeSize = vs.size()
			volume.volumeFree = vs.free()
			
			volume.available = True
		End If

		
		Return volume
	End Method
	
	Method GetPath:String(folderType:Int, flags:Int = 0)
		Local buf:Byte[1024],ref:Byte[80]
		
		If flags Then
			If FSFindFolder( flags, folderType, False, ref ) Return Null
		Else
			If FSFindFolder( kUserDomain, folderType, False, ref ) Return Null
		End If
		If FSRefMakePath( ref,buf,1024 ) Return Null
		
		Return String.FromUTF8String( buf )
	End Method

	Method GetUserHomeDir:String()
		Return GetPath(kCurrentUserFolderType)
	End Method
	
	Method GetUserDesktopDir:String()
		Return GetPath(kDesktopFolderType)
	End Method
	
	Method GetUserAppDir:String()
		Return GetPath(kApplicationSupportFolderType)
	End Method
	
	Method GetUserDocumentsDir:String()
		Return GetPath(kDocumentsFolderType)
	End Method

	Method GetCustomDir:String(dirType:Int, flags:Int = 0)
		If dirType < 0 And dirType > -10 Then
			Select dirType
				Case DT_SHAREDUSERDATA
					Return GetPath(kSharedUserDataFolderType)
				Case DT_USERPICTURES
					Return GetPath(kPictureDocumentsFolderType)
				Case DT_USERMUSIC
					Return GetPath(kMusicDocumentsFolderType)
				Case DT_USERMOVIES
					Return GetPath(kMovieDocumentsFolderType)
			End Select
		Else
			Return GetPath(dirType, flags)
		End If
		
		Return Null
	End Method

End Type

Type TVolSpace
	Field vol:String
	Field fs:Tstatfs = New Tstatfs
	
	Function GetDiskSpace:TVolSpace(vol:String)
		Local this:TVolSpace = New TVolSpace
		
		this.vol = vol

		If this.refresh() <> 0 Then
			Return Null
		End If

		Return this
	End Function
	
	Method refresh:Int()
		Return bmx_vol_statfs(vol, fs.fsPtr)
	End Method
	
	Method size:Long()
		If fs Then
			Return fs.size()
		End If
	End Method
	
	Method free:Long()
		If fs Then
			Return fs.sizeFree()
		End If
	End Method
End Type
