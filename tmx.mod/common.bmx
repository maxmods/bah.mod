' Copyright (c) 2015-2017 Bruce A Henderson
' All rights reserved.
' 
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
' 
'  * Redistributions of source code must retain the above copyright
'    notice, this list of conditions and the following disclaimer.
' 
'  * Redistributions in binary form must reproduce the above copyright
'    notice, this list of conditions and the following disclaimer in the
'    documentation and/or other materials provided with the distribution.
' 
' THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
' 
SuperStrict

Import BaH.LibXML

Import "source.bmx"

Extern

	Function tmx_load_buffer:Byte Ptr(buf:Byte Ptr, buflen:Int, filename:Byte Ptr)
	Function tmx_map_free(handle:Byte Ptr)

	Function bmx_tmx_map_width:Int(handle:Byte Ptr)
	Function bmx_tmx_map_height:Int(handle:Byte Ptr)
	Function bmx_tmx_map_tile_width:Int(handle:Byte Ptr)
	Function bmx_tmx_map_tile_height:Int(handle:Byte Ptr)
	Function bmx_tmx_map_backgroundcolor:Int(handle:Byte Ptr)
	Function bmx_tmx_map_orient:Int(handle:Byte Ptr)
	Function bmx_tmx_map_renderorder:Int(handle:Byte Ptr)
	Function bmx_tmx_map_ts_head:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_map_ly_head:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_map_properties:Byte Ptr(handle:Byte Ptr)

	Function bmx_tmx_tilesetlist_firstgid:Int(handle:Byte Ptr)
	Function bmx_tmx_tilesetlist_hasNext:Int(handle:Byte Ptr)
	Function bmx_tmx_tilesetlist_next:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_tilesetlist_tileset:Byte Ptr(handle:Byte Ptr)

	Function bmx_tmx_tileset_name:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_tileset_tile_width:Int(handle:Byte Ptr)
	Function bmx_tmx_tileset_tile_height:Int(handle:Byte Ptr)
	Function bmx_tmx_tileset_spacing:Int(handle:Byte Ptr)
	Function bmx_tmx_tileset_margin:Int(handle:Byte Ptr)
	Function bmx_tmx_tileset_x_offset:Int(handle:Byte Ptr)
	Function bmx_tmx_tileset_y_offset:Int(handle:Byte Ptr)
	Function bmx_tmx_tileset_image:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_tileset_properties:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_tileset_tiles:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_tileset_tilecount:Int(handle:Byte Ptr)

	Function bmx_tmx_layer_name:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_layer_color:Int(handle:Byte Ptr)
	Function bmx_tmx_layer_opacity:Double(handle:Byte Ptr)
	Function bmx_tmx_layer_visible:Int(handle:Byte Ptr)
	Function bmx_tmx_layer_x_offset:Int(handle:Byte Ptr)
	Function bmx_tmx_layer_y_offset:Int(handle:Byte Ptr)
	Function bmx_tmx_layer_type:Int(handle:Byte Ptr)
	Function bmx_tmx_layer_gids:Int Ptr(handle:Byte Ptr)
	Function bmx_tmx_layer_hasNext:Int(handle:Byte Ptr)
	Function bmx_tmx_layer_next:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_layer_properties:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_layer_objects:Byte Ptr(handle:Byte Ptr)

	Function bmx_tmx_property_name:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_property_type:Int(handle:Byte Ptr)
	Function bmx_tmx_property_int_value:Int(handle:Byte Ptr)
	Function bmx_tmx_property_float_value:Float(handle:Byte Ptr)
	Function bmx_tmx_property_bool_value:Int(handle:Byte Ptr)
	Function bmx_tmx_property_string_value:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_property_file_value:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_property_color_value:Int(handle:Byte Ptr)

	Function bmx_tmx_object_name:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_object_type:Int(handle:Byte Ptr)
	Function bmx_tmx_object_x:Double(handle:Byte Ptr)
	Function bmx_tmx_object_y:Double(handle:Byte Ptr)
	Function bmx_tmx_object_width:Double(handle:Byte Ptr)
	Function bmx_tmx_object_height:Double(handle:Byte Ptr)
	Function bmx_tmx_object_gid:Int(handle:Byte Ptr)
	Function bmx_tmx_object_points:Double Ptr Ptr(handle:Byte Ptr)
	Function bmx_tmx_object_points_len:Int(handle:Byte Ptr)
	Function bmx_tmx_object_visible:Int(handle:Byte Ptr)
	Function bmx_tmx_object_rotation:Double(handle:Byte Ptr)
	Function bmx_tmx_object_properties:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_object_hasNext:Int(handle:Byte Ptr)
	Function bmx_tmx_object_next:Byte Ptr(handle:Byte Ptr)

	Function bmx_tmx_image_source:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_image_trans:Int(handle:Byte Ptr)
	Function bmx_tmx_image_uses_trans:Int(handle:Byte Ptr)
	Function bmx_tmx_image_width:Int(handle:Byte Ptr)
	Function bmx_tmx_image_height:Int(handle:Byte Ptr)
	Function bmx_tmx_image_resource_image:Byte Ptr(handle:Byte Ptr)

	Function bmx_tmx_tile_id:Int(handle:Byte Ptr)
	Function bmx_tmx_tile_image:Byte Ptr(handle:Byte Ptr)
	Function bmx_tmx_tile_properties:Byte Ptr(handle:Byte Ptr)
	
	Function bmx_tmx_tilearray_get:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_tmx_properties_get:Byte Ptr(handle:Byte Ptr, key:Byte Ptr)
	
	Global tmx_errno:Int
	
End Extern

Const TMX_FLIPPED_HORIZONTALLY:Int = $80000000
Const TMX_FLIPPED_VERTICALLY:Int = $40000000
Const TMX_FLIPPED_DIAGONALLY:Int = $20000000
Const TMX_FLIP_BITS_REMOVAL:Int = $1FFFFFFF

Rem
bbdoc: No error so far
EndRem
Const TMX_ERROR_NONE:Int = 0
Rem
bbdoc: See the message for more details
EndRem
Const TMX_ERROR_UNKN:Int = 1
Rem
bbdoc: Invalid argument
EndRem
Const TMX_ERROR_INVAL:Int = 2
Rem
bbdoc: Mem alloc
EndRem
Const TMX_ERROR_ALLOC:Int = 8
Rem
bbdoc: privileges needed
EndRem
Const TMX_ERROR_ACCESS:Int = 10
Rem
bbdoc: File not found
EndRem
Const TMX_ERROR_NOENT:Int = 11
Rem
bbdoc: Unsupproted/Unknown file format
EndRem
Const TMX_ERROR_FORMAT:Int = 12
Rem
bbdoc: Unsupproted/Unknown data encoding/compression
EndRem
Const TMX_ERROR_ENCCMP:Int = 13
Rem
bbdoc: Fonctionnality not enabled
EndRem
Const TMX_ERROR_FONCT:Int = 16
Rem
bbdoc: B64 bad data
EndRem
Const TMX_ERROR_BDATA:Int = 20
Rem
bbdoc: Zlib corrupted data
EndRem
Const TMX_ERROR_ZDATA:Int = 21
Rem
bbdoc: XML corrupted data
EndRem
Const TMX_ERROR_XDATA:Int = 22
Rem
bbdoc: JSON corrupted data
EndRem
Const TMX_ERROR_JDATA:Int = 23
Rem
bbdoc: CSV corrupted data
EndRem
Const TMX_ERROR_CDATA:Int = 24
Rem
bbdoc: Missing element, incomplete source
EndRem
Const TMX_ERROR_MISSEL:Int = 30


Const TMX_ORIENT_NONE:Int = 0
Const TMX_ORIENT_ORT:Int = 1
Const TMX_ORIENT_ISO:Int = 2
Const TMX_ORIENT_STA:Int = 3

Const TMX_RENDERORDER_NONE:Int = 0
Const TMX_RENDERORDER_RIGHTDOWN:Int = 1
Const TMX_RENDERORDER_RIGHTUP:Int = 2
Const TMX_RENDERORDER_LEFTDOWN:Int = 3
Const TMX_RENDERORDER_LEFTUP:Int = 4

Const TMX_LAYER_TYPE_NONE:Int = 0
Const TMX_LAYER_TYPE_LAYER:Int = 1
Const TMX_LAYER_TYPE_OBJGR:Int = 2
Const TMX_LAYER_TYPE_IMAGE:Int = 3
Const TMX_LAYER_TYPE_GROUP:Int = 4

Const TMX_OBJECT_NONE:Int = 0
Const TMX_OBJECT_SQUARE:Int = 1
Const TMX_OBJECT_POLYGON:Int = 2
Const TMX_OBJECT_POLYLINE:Int = 3
Const TMX_OBJECT_ELLIPSE:Int = 4
Const TMX_OBJECT_TILE:Int = 5
Const TMX_OBJECT_TEXT:Int = 6

Const TMX_PROPERTY_NONE:Int = 0
Const TMX_PROPERTY_INT:Int = 1
Const TMX_PROPERTY_FLOAT:Int = 2
Const TMX_PROPERTY_BOOL:Int = 3
Const TMX_PROPERTY_STRING:Int = 4
Const TMX_PROPERTY_COLOR:Int = 5
Const TMX_PROPERTY_FILE:Int = 6
