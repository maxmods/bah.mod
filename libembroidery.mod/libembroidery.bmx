' Copyright (c) 2016 Bruce A Henderson
'
'
' This software is provided 'as-is', without any express or implied
' warranty. In no event will the authors be held liable for any damages
' arising from the use of this software.
'
' Permission is granted to anyone to use this software for any purpose,
' including commercial applications, and to alter it and redistribute it
' freely, subject to the following restrictions:
'
'    1. The origin of this software must not be misrepresented; you must not
'    claim that you wrote the original software. If you use this software
'    in a product, an acknowledgment in the product documentation would be
'    appreciated but is not required.
'
'    2. Altered source versions must be plainly marked as such, and must not be
'    misrepresented as being the original software.
'
'    3. This notice may not be removed or altered from any source
'    distribution.
'
SuperStrict

Module BaH.libembroidery

ModuleInfo "Version: 1.00"
ModuleInfo "License: zlib"
ModuleInfo "Copyright: libembroidery - 2011-2014 Embroidermodder, Jonathan Greig, Josh Varga and all other contributers"
ModuleInfo "Copyright: Wrapper - 2016 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"


ModuleInfo "CC_OPTS: -DLIBEMBROIDERY_STATIC -DEMB_CALL="


?bmxng

Import "common.bmx"



Rem
bbdoc: 
End Rem
Type TEmbFormatList

	Field listPtr:Byte Ptr
	Field curPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TEmbFormatList()
		listPtr = embFormatList_create()
		If listPtr Then
			curPtr = listPtr
			Return Self
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Extension:String()
		Return String.FromCString(embFormat_extension(curPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Description:String()
		Return String.FromCString(embFormat_description(curPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Reader:Int()
		Return embFormat_readerState(curPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Writer:Int()
		Return embFormat_writerState(curPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method NextFormat:TEmbFormatList()
		' reset to start
		If Not curPtr Then
			curPtr = listPtr
		End If

		curPtr = bmx_libembroidery_EmbFormatList_next(curPtr)
		If curPtr Then
			Return Self
		End If
	End Method
	
	Method Delete()
		If listPtr Then
			embFormatList_free(listPtr)
			listPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TEmbPattern

	Field patternPtr:Byte Ptr
	
	Method Create:TEmbPattern()
		patternPtr = embPattern_create()
		If patternPtr Then
			Return Self
		End If
	End Method

	Rem
	bbdoc:
	End Rem
	Method HideStitchesOverLength(length:Int)
		embPattern_hideStitchesOverLength(patternPtr, length)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method FixColorCount()
		embPattern_fixColorCount(patternPtr)
	End Method
	
	
	Rem
	bbdoc:
	End Rem
	Method AddStitchAbs(x:Double, y:Double, flags:Int, isAutoColorIndex:Int)
		embPattern_addStitchAbs(patternPtr, x, y, flags, isAutoColorIndex)
	End Method
	
	Rem
	bbdoc: Adds a stitch to the pattern at the relative position dx, dy to the previous stitch.
	about: Positive y is up. Units are in millimeters. 
	End Rem
	Method AddStitchRel(dx:Double, dy:Double, flags:Int, isAutoColorIndex:Int)
		embPattern_addStitchRel(patternPtr, dx, dy, flags, isAutoColorIndex)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method ChangeColor(index:Int)
		embPattern_changeColor(patternPtr, index)
	End Method
	
	Rem
	bbdoc: Very simple scaling of the x and y axis for every point.
	about: Doesn't insert or delete stitches to preserve density. 
	End Rem
	Method Scale(scale:Double)
		embPattern_scale(patternPtr, scale)
	End Method
	
	Rem
	bbdoc: Returns an EmbRect that encapsulates all stitches and objects in the pattern.
	End Rem
	Method CalcBoundingBox:SEmbRect()
		Return embPattern_calcBoundingBox(patternPtr)
	End Method
	
	Rem
	bbdoc: Flips the entire pattern horizontally about the y-axis. 
	End Rem
	Method FlipHorizontal()
		embPattern_flipHorizontal(patternPtr)
	End Method
	
	Rem
	bbdoc: Flips the entire pattern vertically about the x-axis. 
	End Rem
	Method FlipVertical()
		embPattern_flipVertical(patternPtr)
	End Method
	
	Rem
	bbdoc: Flips the entire pattern horizontally about the x-axis if @horz is true, or vertically about the y-axis if @vert is true. 
	End Rem
	Method Flip(horz:Int, vert:Int)
		embPattern_flip(patternPtr, horz, vert)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method CombineJumpStitches()
		embPattern_combineJumpStitches(patternPtr)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method CorrectForMaxStitchLength(maxStitchLength:Double, maxJumpLength:Double)
		embPattern_correctForMaxStitchLength(patternPtr, maxStitchLength, maxJumpLength)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method Center()
		embPattern_center(patternPtr)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddCircleAbs(cx:Double, cy:Double, radius:Double)
		embPattern_addCircleObjectAbs(patternPtr, cx, cy, radius)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddEllipseAbs(cx:Double, cy:Double, rx:Double, ry:Double)
		embPattern_addEllipseObjectAbs(patternPtr, cx, cy, rx, ry)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddLineAbs(x1:Double, y1:Double, x2:Double, y2:Double)
		embPattern_addLineObjectAbs(patternPtr, x1, y1, x2, y2)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddPathObjectAbs(obj:TEmbPathObject)
		obj._owner = False
		embPattern_addPathObjectAbs(patternPtr, obj.pathPtr)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddPointObjectAbs(x:Double, y:Double)
		embPattern_addPointObjectAbs(patternPtr, x, y)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddPolygonObjectAbs(obj:TEmbPolygonObject)
		obj._owner = False
		embPattern_addPolygonObjectAbs(patternPtr, obj.polyPtr)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddPolylineObjectAbs(obj:TEmbPolylineObject)
		obj._owner = False
		embPattern_addPolylineObjectAbs(patternPtr, obj.polyPtr)
	End Method
	
	Rem
	bbdoc:
	End Rem
	Method AddRectAbs(x:Double, y:Double, w:Double, h:Double)
		embPattern_addRectObjectAbs(patternPtr, x, y, w, h)
	End Method
	
	Rem
	bbdoc: Returns the stitch list for the pattern.
	End Rem
	Method StitchList:TEmbStitchList()
		Return TEmbStitchList._create(bmx_libembroidery_EmbPattern_stitchlist(patternPtr))
	End Method
	
	Rem
	bbdoc: Returns the thread list for the pattern.
	End Rem
	Method ThreadList:TEmbThreadList()
		Return TEmbThreadList._create(bmx_libembroidery_EmbPattern_threadlist(patternPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CopyPolylinesToStitchList()
		embPattern_copyPolylinesToStitchList(patternPtr)
	End Method
	
	Rem
	bbdoc: Reads a file with the given filename and loads the data into the pattern.
	returns: True if successful, otherwise returns False.
	End Rem
	Method Read:Int(filename:String)
		Local f:Byte Ptr = filename.ToUTF8String()
		Local res:Int = embPattern_read(patternPtr, f)
		MemFree(f)
		Return res
	End Method
	
	Rem
	bbdoc: Writes the data from the pattern to a file with the given filename.
	returns: True if successful, otherwise returns False.
	End Rem
	Method Write:Int(filename:String)
		Local f:Byte Ptr = filename.ToUTF8String()
		Local res:Int = embPattern_write(patternPtr, f)
		MemFree(f)
		Return res
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CurrentColorIndex:Int()
		Return bmx_libembroidery_EmbPattern_currentcolorindex(patternPtr)
	End Method
		
	Method Delete()
		If patternPtr Then
			embPattern_free(patternPtr)
			patternPtr = Null
		End If
	End Method
	
End Type

Extern
	Function embThreadList_create:Byte Ptr(data:SEmbThread)
	Function embThreadList_add:Byte Ptr(handle:Byte Ptr, data:SEmbThread)
	Function bmx_libembroidery_EmbThreadList_thread(handle:Byte Ptr, data:SEmbThread Var)
	Function bmx_libembroidery_EmbThreadList_next:Byte Ptr(handle:Byte Ptr)
	Function bmx_libembroidery_EmbThreadList_getat(handle:Byte Ptr, index:Int, data:SEmbThread Var)
	
	Function embPattern_calcBoundingBox:SEmbRect(handle:Byte Ptr)
	
	Function embPointList_add:Byte Ptr(handle:Byte Ptr, data:SEmbPoint)
	Function bmx_libembroidery_EmbPointList_point(handle:Byte Ptr, data:SEmbPoint Var)
	Function bmx_libembroidery_EmbPointList_next:Byte Ptr(handle:Byte Ptr)
	
	Function embPolylineObject_create:Byte Ptr(pointList:Byte Ptr, color:SEmbColor, lineType:Int)
	
	Function embPathObject_create:Byte Ptr(pointList:Byte Ptr, flagList:Byte Ptr, color:SEmbColor, lineType:Int)
	
	Function embPolygonObject_create:Byte Ptr(pointList:Byte Ptr, color:SEmbColor, lineType:Int)

	Function embStitchList_create:Byte Ptr(data:SEmbStitch)
	Function embStitchList_add:Byte Ptr(handle:Byte Ptr, data:SEmbStitch)
	Function bmx_libembroidery_EmbStitchList_stitch(handle:Byte Ptr, data:SEmbStitch Var)
	Function bmx_libembroidery_EmbStitchList_next:Byte Ptr(handle:Byte Ptr)

	Function embFlagList_create:Byte Ptr(data:Int)
	Function embFlagList_add:Byte Ptr(handle:Byte Ptr, data:Int)
	Function bmx_libembroidery_EmbFlagList_flag:Int(handle:Byte Ptr)
	Function bmx_libembroidery_EmbFlagList_next:Byte Ptr(handle:Byte Ptr)
	
End Extern

Rem
bbdoc: 
End Rem
Type TEmbThreadList
	
	Field listPtr:Byte Ptr
	Field curPtr:Byte Ptr
	
	Field _owner:Int = True

	Rem
	bbdoc: 
	End Rem
	Method Create:TEmbThreadList(data:SEmbThread)
		listPtr = embThreadList_create(data)
		If listPtr Then
			curPtr = listPtr
			Return Self
		End If
	End Method

	Function _create:TEmbThreadList(listPtr:Byte Ptr)
		If listPtr Then
			Local this:TEmbThreadList = New TEmbThreadList
			this.listPtr = listPtr
			this.curPtr = listPtr
			this._owner = False
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Thread:SEmbThread()
		Local t:SEmbThread
		bmx_libembroidery_EmbThreadList_thread(curPtr, Varptr t)
		Return t
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Add:TEmbThreadList(data:SEmbThread)
		Last()
		curPtr = embThreadList_add(curPtr, data)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetAt:SEmbThread(index:Int)
		Local t:SEmbThread
		bmx_libembroidery_EmbThreadList_getat(listPtr, index, Varptr t)
		Return t
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method NextThread:TEmbThreadList()
		' reset to start
		If Not curPtr Then
			curPtr = listPtr
		End If
		
		curPtr = bmx_libembroidery_EmbThreadList_next(curPtr)
		If curPtr Then
			Return Self
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method First:TEmbThreadList()
		curPtr = listPtr
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Last:TEmbThreadList()
		First()
		Local i:Int = 1
		While i < Count()
			NextThread()
			i :+ 1
		Wend
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Count:Int()
		Return embThreadList_count(listPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method IsEmpty:Int()
		Return embThreadList_empty(listPtr)
	End Method
	
	Method Delete()
		If listPtr And _owner Then
			embThreadList_free(listPtr)
			listPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TEmbPointList

	Field listPtr:Byte Ptr
	Field curPtr:Byte Ptr
	Field _owner:Int = True
	
	Method Create:TEmbPointList(x:Double, y:Double)
		listPtr = embPointList_create(x, y)
		If listPtr Then
			curPtr = listPtr
			Return Self
		End If
	End Method
	
	Method Add:TEmbPointList(data:SEmbPoint)
		Last()
		curPtr = embPointList_add(curPtr, data)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Point:SEmbPoint()
		Local p:SEmbPoint
		bmx_libembroidery_EmbPointList_point(curPtr, Varptr p)
		Return p
	End Method
	
	Method Count:Int()
		Return embPointList_count(listPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method First:TEmbPointList()
		curPtr = listPtr
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Last:TEmbPointList()
		First()
		Local i:Int = 1
		While i < Count()
			NextPoint()
			i :+ 1
		Wend
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method NextPoint:TEmbPointList()
		' reset to start
		If Not curPtr Then
			curPtr = listPtr
		End If
		
		curPtr = bmx_libembroidery_EmbPointList_next(curPtr)
		If curPtr Then
			Return Self
		End If
	End Method

	Method Delete()
		If listPtr And _owner Then
			embPointList_free(listPtr)
			listPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TEmbPathObject
	Field pathPtr:Byte Ptr
	Field _owner:Int = True
	
	Method Create:TEmbPathObject(pointList:TEmbPointList, flagList:TEmbFlagList, color:SEmbColor, lineType:Int)
		pointList._owner = False
		flagList._owner = False
		pathPtr = embPathObject_create(pointList.listPtr, flagList.listPtr, color, lineType)
		If pathPtr Then
			Return Self
		End If
	End Method
	
	Method Delete()
		If pathPtr And _owner Then
			embPathObject_free(pathPtr)
			pathPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TEmbPolygonObject
	Field polyPtr:Byte Ptr
	Field _owner:Int = True
	
	Method Create:TEmbPolygonObject(pointList:TEmbPointList, color:SEmbColor, lineType:Int)
		pointList._owner = False
		polyPtr = embPolygonObject_create(pointList.listPtr, color, lineType)
		Return Self
	End Method
	
	Method Delete()
		If polyPtr And _owner Then
			embPolygonObject_free(polyPtr)
			polyPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TEmbPolylineObject
	Field polyPtr:Byte Ptr
	Field _owner:Int = True
	
	Method Create:TEmbPolylineObject(pointList:TEmbPointList, color:SEmbColor, lineType:Int)
		polyPtr = embPolylineObject_create(pointList.listPtr, color, lineType)
		Return Self
	End Method
	
	Method Delete()
		If polyPtr And _owner Then
			embPolylineObject_free(polyPtr)
			polyPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TEmbFlagList
	Field listPtr:Byte Ptr

	Field curPtr:Byte Ptr
	Field _owner:Int = True
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TEmbFlagList(data:Int)
		listPtr = embFlagList_create(data)
		If listPtr Then
			curPtr = listPtr
			Return Self
		End If
	End Method
	
	Function _create:TEmbFlagList(listPtr:Byte Ptr)
		If listPtr Then
			Local this:TEmbFlagList = New TEmbFlagList
			this.listPtr = listPtr
			this.curPtr = listPtr
			this._owner = False
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Add:TEmbFlagList(data:Int)
		Last()
		curPtr = embFlagList_add(curPtr, data)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Flag:Int()
		Return bmx_libembroidery_EmbFlagList_flag(curPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Count:Int()
		Return embFlagList_count(listPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method First:TEmbFlagList()
		curPtr = listPtr
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Last:TEmbFlagList()
		First()
		Local i:Int = 1
		While i < Count()
			NextFlag()
			i :+ 1
		Wend
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method NextFlag:TEmbFlagList()
		' reset to start
		If Not curPtr Then
			curPtr = listPtr
		End If
		
		curPtr = bmx_libembroidery_EmbFlagList_next(curPtr)
		If curPtr Then
			Return Self
		End If
	End Method

	Method Delete()
		If listPtr And _owner Then
			embFlagList_free(listPtr)
			listPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TEmbStitchList

	Field listPtr:Byte Ptr

	Field curPtr:Byte Ptr
	Field _owner:Int = True
	
	Method Create:TEmbStitchList(data:SEmbStitch)
		listPtr = embStitchList_create(data)
		If listPtr Then
			curPtr = listPtr
			Return Self
		End If
	End Method
	
	Function _create:TEmbStitchList(listPtr:Byte Ptr)
		If listPtr Then
			Local this:TEmbStitchList = New TEmbStitchList
			this.listPtr = listPtr
			this.curPtr = listPtr
			this._owner = False
			Return this
		End If
	End Function
	
	Method Add:TEmbStitchList(data:SEmbStitch)
		Last()
		curPtr = embStitchList_add(curPtr, data)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Stitch:SEmbStitch()
		Local s:SEmbStitch
		bmx_libembroidery_EmbStitchList_stitch(curPtr, Varptr s)
		Return s
	End Method
	
	Method Count:Int()
		Return embStitchList_count(listPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method First:TEmbStitchList()
		curPtr = listPtr
		Return Self
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Last:TEmbStitchList()
		First()
		Local i:Int = 1
		While i < Count()
			NextStitch()
			i :+ 1
		Wend
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method NextStitch:TEmbStitchList()
		' reset to start
		If Not curPtr Then
			curPtr = listPtr
		End If
		
		curPtr = bmx_libembroidery_EmbStitchList_next(curPtr)
		If curPtr Then
			Return Self
		End If
	End Method

	Method Delete()
		If listPtr And _owner Then
			embStitchList_free(listPtr)
			listPtr = Null
		End If
	End Method

End Type



Struct SEmbColor

	Field r:Byte
	Field g:Byte
	Field b:Byte

	Method New(r:Int, g:Int, b:Int)
		Self.r = r
		Self.g = g
		Self.b = b
	End Method

End Struct


Struct SEmbThread

	Field color:SEmbColor
	Field description:Byte Ptr
	Field catalogNumber:Byte Ptr

End Struct

Struct SEmbRect

	Field top:Double
	Field Left:Double
	Field bottom:Double
	Field Right:Double
	
End Struct

Struct SEmbRectObject
	Field rect:SEmbRect
	Field rotation:Double
	Field radius:Double
	
	Field lineType:Int
	Field color:SEmbColor
End Struct

Rem
bbdoc: Absolute position (not relative)
about: Positive is up, units are in mm
End Rem
Struct SEmbPoint
	Field xx:Double
	Field yy:Double
End Struct

Rem
bbdoc: 
End Rem
Struct SEmbStitch

	Rem
	bbdoc: One of @STITCH_NORMAL, @STITCH_JUMP, @STITCH_TRIM, @STITCH_STOP, @STITCH_SEQUIN or @STITCH_END.
	End Rem
	Field flags:Int
	Rem
	bbdoc: Absolute x position (not relative)
	about: positive is up, units are in mm
	End Rem
	Field xx:Double
	Rem
	bbdoc: Absolute y position (not relative) 
	about: positive is up, units are in mm
	End Rem
	Field yy:Double
	Rem
	bbdoc: color number for this stitch
	End Rem
	Field colorIndex:Int

End Struct

?
