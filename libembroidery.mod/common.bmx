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

Import "source.bmx"


Extern

	Function embFormatList_create:Byte Ptr()
	Function embFormat_extension:Byte Ptr(handle:Byte Ptr)
	Function embFormat_description:Byte Ptr(handle:Byte Ptr)
	Function embFormat_readerState:Int(handle:Byte Ptr)
	Function embFormat_writerState:Int(handle:Byte Ptr)
	Function bmx_libembroidery_EmbFormatList_next:Byte Ptr(handle:Byte Ptr)
	Function embFormatList_free(handle:Byte Ptr)

	Function embPattern_create:Byte Ptr()
	Function embPattern_free(handle:Byte Ptr)
	Function embPattern_hideStitchesOverLength(handle:Byte Ptr, length:Int)
	Function embPattern_fixColorCount(handle:Byte Ptr)
	Function embPattern_addStitchAbs(handle:Byte Ptr, x:Double, y:Double, flags:Int, isAutoColorIndex:Int)
	Function embPattern_addStitchRel(handle:Byte Ptr, dx:Double, dy:Double, flags:Int, isAutoColorIndex:Int)
	Function embPattern_changeColor(handle:Byte Ptr, index:Int)
	Function embPattern_scale(handle:Byte Ptr, scale:Double)
	Function embPattern_flipHorizontal(handle:Byte Ptr)
	Function embPattern_flipVertical(handle:Byte Ptr)
	Function embPattern_flip(handle:Byte Ptr, horz:Int, vert:Int)
	Function embPattern_combineJumpStitches(handle:Byte Ptr)
	Function embPattern_correctForMaxStitchLength(handle:Byte Ptr, maxStitchLength:Double, maxJumpLength:Double)
	Function embPattern_center(handle:Byte Ptr)
	Function embPattern_addCircleObjectAbs(handle:Byte Ptr, cx:Double, cy:Double, radius:Double)
	Function embPattern_addEllipseObjectAbs(handle:Byte Ptr, cx:Double, cy:Double, rx:Double, ry:Double)
	Function embPattern_addLineObjectAbs(handle:Byte Ptr, x1:Double, y1:Double, x2:Double, y2:Double)
	Function embPattern_addPathObjectAbs(handle:Byte Ptr, obj:Byte Ptr)
	Function embPattern_addPointObjectAbs(handle:Byte Ptr, x:Double, y:Double)
	Function embPattern_addPolygonObjectAbs(handle:Byte Ptr, obj:Byte Ptr)
	Function embPattern_addPolylineObjectAbs(handle:Byte Ptr, obj:Byte Ptr)
	Function embPattern_addRectObjectAbs(handle:Byte Ptr, x:Double, y:Double, w:Double, h:Double)
	Function bmx_libembroidery_EmbPattern_stitchlist:Byte Ptr(handle:Byte Ptr)
	Function bmx_libembroidery_EmbPattern_threadlist:Byte Ptr(handle:Byte Ptr)
	Function embPattern_read:Int(handle:Byte Ptr, filename:Byte Ptr)
	Function embPattern_write:Int(handle:Byte Ptr, filename:Byte Ptr)
	Function embPattern_copyPolylinesToStitchList(handle:Byte Ptr)
	Function bmx_libembroidery_EmbPattern_currentcolorindex:Int(handle:Byte Ptr)

	Function embThreadList_count:Int(handle:Byte Ptr)
	Function embThreadList_empty:Int(handle:Byte Ptr)
	Function embThreadList_free(handle:Byte Ptr)
	
	Function embPointList_create:Byte Ptr(x:Double, y:Double)
	Function embPointList_count:Int(handle:Byte Ptr)
	Function embPointList_free(handle:Byte Ptr)
	
	Function embPathObject_free(handle:Byte Ptr)
	
	Function embPolylineObject_free(handle:Byte Ptr)
	
	Function embPolygonObject_free(handle:Byte Ptr)

	Function embStitchList_count:Int(handle:Byte Ptr)
	Function embStitchList_empty:Int(handle:Byte Ptr)
	Function embStitchList_free(handle:Byte Ptr)

	Function embFlagList_count:Int(handle:Byte Ptr)
	Function embFlagList_empty:Int(handle:Byte Ptr)
	Function embFlagList_free(handle:Byte Ptr)
End Extern

Rem
bbdoc: Stitch to (xx, yy)
End Rem
Const STITCH_NORMAL:Int = 0
Rem
bbdoc: Move to (xx, yy)
End Rem
Const STITCH_JUMP:Int = 1
Rem
bbdoc: Trim and move to (xx, yy)
End Rem
Const STITCH_TRIM:Int = 2
Rem
bbdoc: Pause machine for thread change
End Rem
Const STITCH_STOP:Int = 4
Rem
bbdoc: Sequin
End Rem
Const STITCH_SEQUIN:Int = 8
Rem
bbdoc: End of program
End Rem
Const STITCH_END:Int = 16

