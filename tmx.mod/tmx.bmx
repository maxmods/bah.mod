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

Rem
bbdoc: Tiled map loader.
End Rem
Module BaH.TMX

ModuleInfo "Version: 1.01"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: TMX - 2013-2017 Bayle Jonathan"
ModuleInfo "Copyright: Wrapper - 2015-2017  Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to TMX 1.0.0."
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
			
			map.mapPtr = tmx_load_buffer(data, data.length, f)
			
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

	Method properties:TMXProperties()
		Return TMXProperties._create(bmx_tmx_map_properties(mapPtr))
	End Method

	Method tilesets:TMXTilesetList()
		Return TMXTilesetList._create(bmx_tmx_map_ts_head(mapPtr))
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

Type TMXTilesetList
	
	Field listPtr:Byte Ptr

	Function _create:TMXTilesetList(listPtr:Byte Ptr)
		If listPtr Then
			Local this:TMXTilesetList = New TMXTilesetList
			this.listPtr = listPtr
			Return this
		End If		
	End Function

	Method firstgid:Int()
		Return bmx_tmx_tilesetlist_firstgid(listPtr)
	End Method
	
	Method tileset:TMXTileset()
		Return TMXTileset._create(bmx_tmx_tilesetlist_tileset(listPtr))
	End Method
	
	Method HasNext:Int()
		Return bmx_tmx_tilesetlist_hasNext(listPtr)
	End Method
	
	Method NextTilesetList:TMXTilesetList()
		Return _create(bmx_tmx_tilesetlist_next(listPtr))
	End Method
	
End Type

Type TMXLayer

	Field layerPtr:Byte Ptr

	Function _create:TMXLayer(layerPtr:Byte Ptr)
		If layerPtr Then
			Local this:TMXLayer
			Select bmx_tmx_layer_type(layerPtr)
				Case TMX_LAYER_TYPE_LAYER
					this = New TMXLayer
				Case TMX_LAYER_TYPE_OBJGR
					this = New TMXObjectGroupLayer
				Case TMX_LAYER_TYPE_IMAGE
					this = New TMXImageLayer
				Case TMX_LAYER_TYPE_GROUP
					this = New TMXGroupLayer
			End Select
			this.layerPtr = layerPtr
			Return this
		End If
	End Function

	Method name:String()
		Return String.FromUTF8String(bmx_tmx_layer_name(layerPtr))
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

	Method properties:TMXProperties()
		Return TMXProperties._create(bmx_tmx_layer_properties(layerPtr))
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

Type TMXObjectGroupLayer Extends TMXLayer
	
	Method color:Int()
		Return bmx_tmx_layer_color(layerPtr)
	End Method

End Type

Type TMXImageLayer Extends TMXLayer
End Type

Type TMXGroupLayer Extends TMXLayer
End Type

Type TMXObject

	Field objectPtr:Byte Ptr
	
	Function _create:TMXObject(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:TMXObject
			Select bmx_tmx_object_type(objectPtr)
				Case TMX_OBJECT_SQUARE, TMX_OBJECT_POLYGON, TMX_OBJECT_POLYLINE, TMX_OBJECT_ELLIPSE
					this = New TMXShapeObject
				Case TMX_OBJECT_TILE
					this = New TMXTileObject
				Case TMX_OBJECT_TEXT
					this = New TMXTextObject
			End Select
			
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method name:String()
		Return String.FromUTF8String(bmx_tmx_object_name(objectPtr))
	End Method
	
	Method objectType:Int()
		Return bmx_tmx_object_type(objectPtr)
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
	
	Method visible:Int()
		Return bmx_tmx_object_visible(objectPtr)
	End Method
	
	Method rotation:Double()
		Return bmx_tmx_object_rotation(objectPtr)
	End Method

	Method properties:TMXProperties()
		Return TMXProperties._create(bmx_tmx_object_properties(objectPtr))
	End Method

	Method HasNext:Int()
		Return bmx_tmx_object_hasNext(objectPtr)
	End Method
	
	Method NextObject:TMXObject()
		Return _create(bmx_tmx_object_next(objectPtr))
	End Method
	
End Type

Type TMXShapeObject Extends TMXObject

	Method points:Double Ptr Ptr()
		Return bmx_tmx_object_points(objectPtr)
	End Method
	
	Method pointsCount:Int()
		Return bmx_tmx_object_points_len(objectPtr)
	End Method

End Type

Type TMXTextObject Extends TMXObject

End Type

Type TMXTileObject Extends TMXObject

	Method gid:Int()
		Return bmx_tmx_object_gid(objectPtr)
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

	Method properties:TMXProperties()
		Return TMXProperties._create(bmx_tmx_tileset_properties(tilesetPtr))
	End Method

	Method tiles:TMXTileArray()
		Return TMXTileArray._create(bmx_tmx_tileset_tiles(tilesetPtr), bmx_tmx_tileset_tilecount(tilesetPtr))
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

Type TMXTileArray

	Field arrayPtr:Byte Ptr
	Field count:Int

	Function _create:TMXTileArray(arrayPtr:Byte Ptr, count:Int)
		If arrayPtr Then
			Local this:TMXTileArray = New TMXTileArray
			this.arrayPtr = arrayPtr
			this.count = count
			Return this
		End If
	End Function

	Method tileCount:Int()
		Return count
	End Method
	
	Method get:TMXTile(index:Int)
		If index >= 0 And index < count Then
			Return TMXTile._create(bmx_tmx_tilearray_get(arrayPtr, index))
		End If
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

	Method properties:TMXProperties()
		Return TMXProperties._create(bmx_tmx_tile_properties(tilePtr))
	End Method
	
End Type

Type TMXProperties

	Field propertiesPtr:Byte Ptr

	Function _create:TMXProperties(propertiesPtr:Byte Ptr)
		If propertiesPtr Then
			Local this:TMXProperties = New TMXProperties
			this.propertiesPtr = propertiesPtr
			Return this
		End If
	End Function

	Method get:TMXProperty(key:String)
		Local k:Byte Ptr = key.ToUTF8String()
		Local prop:TMXProperty = TMXProperty._create(bmx_tmx_properties_get(propertiesPtr, k))
		MemFree(k)
		Return prop
	End Method
	
	Method forEach(iter:TMXPropertyIterator)
		tmx_property_foreach(propertiesPtr, TMXPropertyIterator._next, iter)
	End Method

End Type

Extern
	Function tmx_property_foreach(handle:Byte Ptr, cb(p:Byte Ptr, i:TMXPropertyIterator), d:TMXPropertyIterator)
End Extern

Type TMXPropertyIterator Abstract

	Method nextProperty(property:TMXProperty) Abstract

	Function _next(propertyPtr:Byte Ptr, iter:TMXPropertyIterator) { nomangle }
		iter.nextProperty(TMXProperty._create(propertyPtr))
	End Function
	
End Type

Type TMXProperty

	Field propertyPtr:Byte Ptr
	
	Function _create:TMXProperty(propertyPtr:Byte Ptr)
		If propertyPtr Then
			Local this:TMXProperty
			Select bmx_tmx_property_type(propertyPtr)
				Case TMX_PROPERTY_INT
					this = New TMXIntProperty
				Case TMX_PROPERTY_FLOAT
					this = New TMXFloatProperty
				Case TMX_PROPERTY_BOOL
					this = New TMXBoolProperty
				Case TMX_PROPERTY_STRING
					this = New TMXStringProperty
				Case TMX_PROPERTY_FILE
					this = New TMXFileProperty
				Case TMX_PROPERTY_COLOR
					this = New TMXColorProperty
			End Select
			this.propertyPtr = propertyPtr
			Return this
		End If
	End Function

	Method name:String()
		Return String.FromUTF8String(bmx_tmx_property_name(propertyPtr))
	End Method
	
	Method intValue:Int()
	End Method

	Method floatValue:Float()
	End Method

	Method boolValue:Int()
	End Method

	Method stringValue:String()
	End Method

	Method fileValue:String()
	End Method
	
	Method colorValue:Int()
	End Method

End Type

Type TMXIntProperty Extends TMXProperty

	Method intValue:Int()
		Return bmx_tmx_property_int_value(propertyPtr)
	End Method
	
End Type

Type TMXFloatProperty Extends TMXProperty

	Method floatValue:Float()
		Return bmx_tmx_property_float_value(propertyPtr)
	End Method
	
End Type

Type TMXBoolProperty Extends TMXProperty

	Method boolValue:Int()
		Return bmx_tmx_property_bool_value(propertyPtr)
	End Method
	
End Type

Type TMXStringProperty Extends TMXProperty

	Method stringValue:String()
		Return String.FromUTF8String(bmx_tmx_property_string_value(propertyPtr))
	End Method
	
End Type

Type TMXFileProperty Extends TMXProperty

	Method fileValue:String()
		Return String.FromUTF8String(bmx_tmx_property_file_value(propertyPtr))
	End Method
	
End Type

Type TMXColorProperty Extends TMXProperty

	Method colorValue:Int()
		Return bmx_tmx_property_color_value(propertyPtr)
	End Method
	
End Type


Function TMXError:Int()
	Return tmx_errno
End Function
