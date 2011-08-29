' Copyright (c) 2009-2011 Bruce A Henderson
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
bbdoc: SID player
End Rem
Module bah.Sid

Import BRL.Stream

Import "common.bmx"

Rem
bbdoc: 
End Rem
Type TSID Abstract

	Field sidPtr:Byte Ptr

	Global sid:TSID
	
	Field playing:Int

	Method Init()
		If Not sid Then
			sid = Self
		End If
		
		doInit()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Function GetSid:TSID()
		If Not sid Then
			Throw "No SID instance. You need to import one of the SID audio modules."
		End If
		
		Return sid
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Load:TSID(path:Object)
		Local this:TSID = GetSid()
		
		this.Free()
		this.Init()
		
		Local data:Byte[]
		
		Try
			data = LoadByteArray(path)
		Catch e:Object
			DebugLog "Unable to load music.."
			Return Null
		End Try

		this.sidload(data, data.length)
		
		
		Return this
	End Function
	
	Method sidload(data:Byte Ptr, size:Int)
		Stop()
		
		'c64Init()
		'synth_init(44100)
		
		If bmx_sid_load(sidPtr, data, size) Then
			doLoad()
		End If

	End Method
	
	Method doInit() Abstract
	
	Method doLoad() Abstract
	
	Method Stop() Abstract
	
	Rem
	bbdoc: 
	End Rem
	Method Play(track:Int = 0) Abstract

	Rem
	bbdoc: 
	End Rem
	Method Pause() Abstract
	
	Rem
	bbdoc: 
	End Rem
	Method TrackCount:Int()
		Return bmx_sid_trackcount(sidPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SongName:String()
		Return bmx_sid_songname(sidPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SongAuthor:String()
		Return bmx_sid_songauthor(sidPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SongCopyright:String()
		Return bmx_sid_songcopyright(sidPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Update()
	End Method
	
	Method SetVolume(volume:Float) Abstract

	Method Free() Abstract
	
End Type
