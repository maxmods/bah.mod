' Copyright (c) 2015, Bruce A Henderson
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

Rem
bbdoc: Tiled map loader.
End Rem
Module BaH.TMX

ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: TMX - 2013, Bayle Jonathan"
ModuleInfo "Copyright: Wrapper - 2015,  Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release"

ModuleInfo "CC_OPTS: -DWANT_JSON -DWANT_XML"

Import BRL.Stream

Import "common.bmx"



Type TMXMap

	Field mapPtr:Byte Ptr
	
	Function Load:TMXMap(url:Object, filename:String = Null)
		Local data:Byte[] = LoadByteArray(url)
		If data Then
			Local map:TMXMap = New TMXMap
			If String(url) Then
				filename = String(url)
			End If
			
			Local f:Byte Ptr = filename.ToUTF8String()
			
			map.mapPtr = tmx_load_buf(data, data.length, f)
			
			MemFree(f)
			
			If Not map.mapPtr Then
				map = Null
			End If
			
			Return map
		End If
	End Function
	
	Method width:Int()
		Return bmx_tmx_map_width(mapPtr)
	End Method
	
	Method height:Int()
		Return bmx_tmx_map_height(mapPtr)
	End Method
	
	Method tileWidth:Int()
		Return bmx_tmx_map_tile_width(mapPtr)
	End Method
	
	Method tileHeight:Int()
		Return bmx_tmx_map_tile_height(mapPtr)
	End Method
	
	Method backgroundcolor:Int()
		Return bmx_tmx_map_backgroundcolor(mapPtr)
	End Method
	
	Method orient:Int()
		Return bmx_tmx_map_orient(mapPtr)
	End Method
	
	Method renderorder:Int()
		Return bmx_tmx_map_renderorder(mapPtr)
	End Method
	
	Method properties:TMXProperty()
	End Method
	
	Method tileset:TMXTileset()
		Return TMXTileset._create(bmx_tmx_map_ts_head(mapPtr))
	End Method
	
	Method layer:TMXLayer()
		Return TMXLayer._create(bmx_tmx_map_ly_head(mapPtr))
	End Method

	Method Delete()
		If mapPtr Then
			tmx_map_free(mapPtr)
			mapPtr = Null
		End If
	End Method
	
End Type

Type TMXLayer

	Field layerPtr:Byte Ptr

	Function _create:TMXLayer(layerPtr:Byte Ptr)
		If layerPtr Then
			Local this:TMXLayer = New TMXLayer
			this.layerPtr = layerPtr
			Return this
		End If
	End Function

	Method name:String()
		Return String.FromUTF8String(bmx_tmx_layer_name(layerPtr))
	End Method
	
	Method color:Int()
		Return bmx_tmx_layer_color(layerPtr)
	End Method
	
	Method opacity:Double()
		Return bmx_tmx_layer_opacity(layerPtr)
	End Method
	
	Method visible:Int()
		Return bmx_tmx_layer_visible(layerPtr)
	End Method
	
	Method xOffset:Int()
		Return bmx_tmx_layer_x_offset(layerPtr)
	End Method
	
	Method yOffset:Int()
		Return bmx_tmx_layer_y_offset(layerPtr)
	End Method
	
	Method layerType:Int()
		Return bmx_tmx_layer_type(layerPtr)
	End Method
	
	Method gids:Int Ptr()
		Return bmx_tmx_layer_gids(layerPtr)
	End Method
	
	Method properties:TMXProperty()
		Return TMXProperty._create(bmx_tmx_layer_properties(layerPtr))
	End Method
	
	Method objects:TMXObject()
		Return TMXObject._create(bmx_tmx_layer_objects(layerPtr))
	End Method

	Method HasNext:Int()
		Return bmx_tmx_layer_hasNext(layerPtr)
	End Method
	
	Method NextLayer:TMXLayer()
		Return _create(bmx_tmx_layer_next(layerPtr))
	End Method
	
End Type

Type TMXObject

	Field objectPtr:Byte Ptr
	
	Function _create:TMXObject(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TMXObject = New TMXObject
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method name:String()
		Return String.FromUTF8String(bmx_tmx_object_name(objectPtr))
	End Method
	
	Method shape:Int()
		Return bmx_tmx_object_shape(objectPtr)
	End Method
	
	Method x:Double()
		Return bmx_tmx_object_x(objectPtr)
	End Method
	
	Method y:Double()
		Return bmx_tmx_object_y(objectPtr)
	End Method
	
	Method width:Double()
		Return bmx_tmx_object_width(objectPtr)
	End Method
	
	Method height:Double()
		Return bmx_tmx_object_height(objectPtr)
	End Method
	
	Method gid:Int()
		Return bmx_tmx_object_gid(objectPtr)
	End Method
	
	Method points:Double Ptr Ptr()
		Return bmx_tmx_object_points(objectPtr)
	End Method
	
	Method pointsCount:Int()
		Return bmx_tmx_object_points_len(objectPtr)
	End Method
	
	Method visible:Int()
		Return bmx_tmx_object_visible(objectPtr)
	End Method
	
	Method rotation:Double()
		Return bmx_tmx_object_rotation(objectPtr)
	End Method
	
	Method properties:TMXProperty()
		Return TMXProperty._create(bmx_tmx_object_properties(objectPtr))
	End Method
	
	Method HasNext:Int()
		Return bmx_tmx_object_hasNext(objectPtr)
	End Method
	
	Method NextObject:TMXObject()
		Return _create(bmx_tmx_object_next(objectPtr))
	End Method
	
End Type

Type TMXTileset

	Field tilesetPtr:Byte Ptr
	
	Function _create:TMXTileset(tilesetPtr:Byte Ptr)
		If tilesetPtr Then
			Local this:TMXTileset = New TMXTileset
			this.tilesetPtr = tilesetPtr
			Return this
		End If
	End Function
	
	Method firstgid:Int()
		Return bmx_tmx_tileset_firstgid(tilesetPtr)
	End Method
	
	Method name:String()
		Return String.FromUTF8String(bmx_tmx_tileset_name(tilesetPtr))
	End Method
	
	Method tileWidth:Int()
		Return bmx_tmx_tileset_tile_width(tilesetPtr)
	End Method
	
	Method tileHeight:Int()
		Return bmx_tmx_tileset_tile_height(tilesetPtr)
	End Method
	
	Method spacing:Int()
		Return bmx_tmx_tileset_spacing(tilesetPtr)
	End Method
	
	Method margin:Int()
		Return bmx_tmx_tileset_margin(tilesetPtr)
	End Method
	
	Method xOffset:Int()
		Return bmx_tmx_tileset_x_offset(tilesetPtr)
	End Method
	
	Method yOffset:Int()
		Return bmx_tmx_tileset_y_offset(tilesetPtr)
	End Method
	
	Method image:TMXImage()
		Return TMXImage._create(bmx_tmx_tileset_image(tilesetPtr))
	End Method

	Method properties:TMXProperty()
		Return TMXProperty._create(bmx_tmx_tileset_properties(tilesetPtr))
	End Method

	Method tiles:TMXTile()
		Return TMXTile._create(bmx_tmx_tileset_tiles(tilesetPtr))
	End Method
				
	Method HasNext:Int()
		Return bmx_tmx_tileset_hasNext(tilesetPtr)
	End Method
	
	Method NextTileset:TMXTileset()
		Return _create(bmx_tmx_tileset_next(tilesetPtr))
	End Method
	
End Type

Type TMXImage

	Field imagePtr:Byte Ptr

	Function _create:TMXImage(imagePtr:Byte Ptr)
		If imagePtr Then
			Local this:TMXImage = New TMXImage
			this.imagePtr = imagePtr
			Return this
		End If
	End Function

	Method source:String()
		Return String.FromUTF8String(bmx_tmx_image_source(imagePtr))
	End Method
	
	Method trans:Int()
		Return bmx_tmx_image_trans(imagePtr)
	End Method
	
	Method usesTrans:Int()
		Return bmx_tmx_image_uses_trans(imagePtr)
	End Method
	
	Method width:Int()
		Return bmx_tmx_image_width(imagePtr)
	End Method
	
	Method height:Int()
		Return bmx_tmx_image_height(imagePtr)
	End Method
	
	Method resourceImage:Byte Ptr()
		Return bmx_tmx_image_resource_image(imagePtr)
	End Method
	
End Type

Type TMXTile

	Field tilePtr:Byte Ptr
	
	Function _create:TMXTile(tilePtr:Byte Ptr)
		If tilePtr Then
			Local this:TMXTile = New TMXTile
			this.tilePtr = tilePtr
			Return this
		End If
	End Function

	Method id:Int()
		Return bmx_tmx_tile_id(tilePtr)
	End Method
	
	Method image:TMXImage()
		Return TMXImage._create(bmx_tmx_tile_image(tilePtr))
	End Method
	
	Method properties:TMXProperty()
		Return TMXProperty._create(bmx_tmx_tile_properties(tilePtr))
	End Method

	Method HasNext:Int()
		Return bmx_tmx_tile_hasNext(tilePtr)
	End Method
	
	Method NextTile:TMXTile()
		Return _create(bmx_tmx_tile_next(tilePtr))
	End Method

End Type

Type TMXProperty

	Field propertPtr:Byte Ptr
	
	Function _create:TMXProperty(propertPtr:Byte Ptr)
		If propertPtr Then
			Local this:TMXProperty = New TMXProperty
			this.propertPtr = propertPtr
			Return this
		End If
	End Function

	Method name:String()
		Return String.FromUTF8String(bmx_tmx_property_name(propertPtr))
	End Method
	
	Method value:String()
		Return String.FromUTF8String(bmx_tmx_property_value(propertPtr))
	End Method

	Method HasNext:Int()
		Return bmx_tmx_property_hasNext(propertPtr)
	End Method
	
	Method NextProperty:TMXProperty()
		Return _create(bmx_tmx_property_next(propertPtr))
	End Method
	
End Type


Function TMXError:Int()
	Return tmx_errno
End Function
