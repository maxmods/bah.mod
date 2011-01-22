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
Import BRL.Bank

Import "main.bmx"

Extern
	Function _statfs:Int(path:Byte Ptr, buf:Byte Ptr) = "statfs"
	Function _getfsstat:Int(structs:Byte Ptr, bufSize:Int, flags:Int) = "getfsstat"

	Function FSFindFolder:Int( vRefNum:Int ,folderType:Int ,createFolder:Int ,foundRef:Byte Ptr )
	Function FSRefMakePath:Int( ref:Byte Ptr,path:Byte Ptr, maxPath:Int )
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
	Const f_otype:Int = 0    ' Type of file system (reserved: zero) */
	Const f_oflags:Int = 2   ' copy of mount flags (reserved: zero) */
	Const f_bsize:Int = 4    ' fundamental file system block size */
	Const f_iosize:Int = 8   ' optimal transfer block size */
	Const f_blocks:Int = 12   ' total data blocks in file system */
	Const f_bfree:Int = 16    ' free blocks in fs */
	Const f_bavail:Int = 20   ' free blocks avail To non-superuser */
	Const f_files:Int = 24    ' total file nodes in file system */
	Const f_ffree:Int = 28    ' free file nodes in fs */
	Const f_fsid:Int = 32     ' file system id (Super-user only) */
	Const f_owner:Int = 40    ' user that mounted the file system (uid_t)
	Const f_reserved1:Int = 44        ' reserved For future use */
	Const f_type:Int = 46     ' Type of file system (reserved) */
	Const f_flags:Int = 48    ' copy of mount flags (reserved) */
	Const f_reserved2a:Int = 52     ' reserved For future use */
	Const f_reserved2b:Int = 56     ' reserved For future use */
	Const f_fstypename:Int = 60 ' fs Type name */
	Const f_mntonname:Int = 75    ' directory on which mounted */
	Const f_mntfromname:Int = 165  ' mounted file system */
	Const f_reserved3:Int = 255        ' reserved For future use */
	Const f_reserved4:Int = 264     ' reserved For future use */
	
	Field bank:TBank = CreateBank(272)

	Method makestatic(b:Byte Ptr)
		bank = CreateStaticBank(b, 272)
	End Method
	
	Method copy(b:TBank, offset:Int)
		CopyBank(b, offset, bank, 0, 272)
	End Method
	
	Method free()
		bank.unlock()
	End Method
	
	Method _bsize:Int()
		Return bank.PeekInt(f_bsize)
	End Method

	Method _iosize:Int()
		Return bank.PeekInt(f_iosize)
	End Method

	Method _blocks:Int()
		Return bank.PeekInt(f_blocks)
	End Method

	Method _bfree:Int()
		Return bank.PeekInt(f_bfree)
	End Method

	Method _bavail:Int()
		Return bank.PeekInt(f_bavail)
	End Method
	
	Method size:Long()
		Return Long(_bsize()) * Long(_blocks())
	End Method

	Method sizeFree:Long()
		Return Long(_bsize()) * Long(_bavail())
	End Method
	
	Method mountedFileSystem:String()
		Return bbStringFromUTF8String(bank.lock() + f_mntfromname)
	End Method
	
	Method dirOfMount:String()
		Return bbStringFromUTF8String(bank.lock() + f_mntonname)
	End Method

	Method fileSystemType:String()
		Return bbStringFromUTF8String(bank.lock() + f_fstypename)
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
	
		' make room for 30 volumes...
		Local bufSize:Int = 30 * 272
		Local bank:TBank = CreateBank(bufSize)
		Local b:Byte Ptr = bank.lock()
		
		Local count:Int = _getfsstat(b, bufSize, 0)
	
		Local fs:TStatfs = New Tstatfs
	
		For Local i:Int = 0 Until count
		
			If Not volumes Then
				volumes = New TList
			End If
		
			fs.makestatic(b + i * 272)
		
			Local volume:TMacVolume = New TMacVolume

			volume.vs = New TVolSpace
			volume.vs.fs.copy(fs.bank, 0)
			
			volume.volumeName = volume.vs.fs.dirOfMount()
			volume.volumeDevice = volume.vs.fs.mountedFileSystem()
			volume.volumeType = volume.vs.fs.fileSystemType()
		
			volume.volumeSize = volume.vs.size()
			volume.volumeFree = volume.vs.free()
	
			volume.available = True
			
			volumes.addLast(volume)
		Next
	
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
	
	Method getPath:String(folderType:Int, flags:Int = 0)
		Local buf:Byte[1024],ref:Byte[80]
		
		If flags Then
			If FSFindFolder( flags, folderType, False, ref ) Return Null
		Else
			If FSFindFolder( kUserDomain, folderType, False, ref ) Return Null
		End If
		If FSRefMakePath( ref,buf,1024 ) Return Null
		
		Return bbStringFromUTF8String( buf )
	End Method

	Method GetUserHomeDir:String()
		Return getPath(kCurrentUserFolderType)
	End Method
	
	Method GetUserDesktopDir:String()
		Return getPath(kDesktopFolderType)
	End Method
	
	Method GetUserAppDir:String()
		Return getPath(kApplicationSupportFolderType)
	End Method
	
	Method GetUserDocumentsDir:String()
		Return getPath(kDocumentsFolderType)
	End Method

	Method GetCustomDir:String(dirType:Int, flags:Int = 0)
		If dirType < 0 And dirType > -10 Then
			Select dirType
				Case DT_SHAREDUSERDATA
					Return getPath(kSharedUserDataFolderType)
				Case DT_USERPICTURES
					Return getPath(kPictureDocumentsFolderType)
				Case DT_USERMUSIC
					Return getPath(kMusicDocumentsFolderType)
				Case DT_USERMOVIES
					Return getPath(kMovieDocumentsFolderType)
			End Select
		Else
			Return getPath(dirType, flags)
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
		Return _statfs(vol, fs.bank.lock())
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
