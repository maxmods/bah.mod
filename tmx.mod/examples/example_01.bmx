SuperStrict

Framework BaH.TMX
Import BRL.StandardIO

Local map:TMXMap = TMXMap.Load("../tmx/examples/data/csv.tmx")

DumpMap(map)



Function DumpMap(map:TMXMap)

	If map Then
		Print "map={"
	
		Print "~torient=" + map.orient()
		Print "~trenderorder=" + map.orient()
		Print "~theight=" + map.height()
		Print "~twidth=" + map.width()
		Print "~ttheight=" + map.tileHeight()
		Print "~ttwidth=" + map.tileWidth()
		Print "~tbgcol=" + map.backgroundcolor()

		Print "}"
	Else
		Print "map={ (NULL) }"
	End If

	If map Then
		DumpTileset(map.tilesets())
		DumpLayer(map.layer(), map.height() * map.width())
		DumpProp(map.properties(), 0)
	End If

End Function

Function DumpTileset(tilesetList:TMXTilesetList)
		
	If tilesetList Then
	
		Local tileset:TMXTileset = tilesetList.tileset()
	
		Print "tileset={"
	
		Print "~tname=" + tileset.name()
		Print "~tfirstgid=" + tilesetList.firstgid()
		Print "~ttile_height=" + tileset.tileHeight()
		Print "~ttile_width=" + tileset.tileWidth()
		Print "~tmargin=" + tileset.margin()
		Print "~tspacing=" + tileset.spacing()
		Print "~tx_offset=" + tileset.xOffset()
		Print "~ty_offset=" + tileset.yOffset()

		DumpImage(tileset.image())
		DumpTile(tileset.tiles())
		DumpProp(tileset.properties(), 1)
		
		Print "}"
	Else
		Print "tileset={ (NULL) }"
	End If
	
	If tilesetList And tilesetList.HasNext() Then
		DumpTileset(tilesetList.NextTilesetList())
	End If
	
End Function

Function DumpLayer(layer:TMXLayer, count:Int)

	If layer Then
		Print "layer={"
	
		Print "~tname=" + layer.name()
		'Print "~tcolor=" + layer.color()
		Print "~tvisible=" + layer.visible()
		Print "~topacity=" + layer.opacity()
	
		Select layer.layerType()
			Case TMX_LAYER_TYPE_LAYER
				Local gids:Int Ptr = layer.gids()
				If gids Then
					Print "~ttype=Layer"
					Local s:String = "~ttiles="
					For Local i:Int = 0 Until count
						s :+ (gids[i] & TMX_FLIP_BITS_REMOVAL) + ","
					Next
					Print s
				End If
			Case TMX_LAYER_TYPE_OBJGR
				Print "~ttype=ObjectGroup"
				DumpObjects(layer.objects())
			Case TMX_LAYER_TYPE_IMAGE
				Print "~tx_offset=" + layer.xOffset()
				Print "~ty_offset=" + layer.yOffset()
				Print "~ttype=ImageLayer"
		End Select
		
		DumpProp(layer.properties(), 1)
		
		Print "}"
	Else
		Print "layer={ (NULL) }"
	End If

	If layer And layer.HasNext() Then
		DumpLayer(layer.NextLayer(), count)
	End If

End Function

Type TPropDumper Extends TMXPropertyIterator

	Field padding:String

	Method nextProperty(property:TMXProperty)
		Print padding + property.name() '+ "=" + prop.value()
	End Method

End Type

Function DumpProp(prop:TMXProperties, depth:Int)
	Local padding:String
	For Local i:Int = 0 Until depth
		padding :+ "~t"
	Next
	
	If prop Then
		Local dumper:TPropDumper = New TPropDumper
		dumper.padding = padding + "~t"
	
	
		Print padding + "properties={"
		
		prop.forEach(dumper)
		
		'While prop
		'	prop = prop.NextProperty()
		'Wend

		Print padding + "}"
	Else
		Print padding + "properties={ (NULL) }"
	End If

End Function

Function DumpObjects(obj:TMXObject)
	If obj Then
		Print "~tobject={"

		Print "~t~tname=" + obj.name()
		'Print "~t~tshape=" + GetShape(obj.shape())
		Print "~t~tx=" + obj.x()
		Print "~t~ty=" + obj.y()
		'Print "~t~tnumber of points=" + obj.pointsCount()
		Print "~t~trotation=" + obj.rotation()
		Print "~t~tvisible=" + obj.visible()
		'If obj.pointsCount Then
		'	DumpPoints(obj.points(), obj.pointsCount())
		'End If
		DumpProp(obj.properties(), 2)
	
		Print "~t}"
	Else
		Print "~tobject={ (NULL) }"
	End If

	If obj And obj.HasNext() Then
		DumpObjects(obj.NextObject())
	End If

End Function

Function DumpPoints(points:Double Ptr Ptr, count:Int)
	Local s:String = "~t~tpoints="
	
	For Local i:Int = 0 Until count
		s:+ " (" + points[i][0] + ", " + points[i][1] + ")"
	Next
	Print s
End Function

Function DumpImage(image:TMXImage)
	If image Then
		Print "~timage={"
		
		Print "~t~tsource=~q" + image.source() + "~q"
		Print "~t~theight=" + image.height()
		Print "~t~twidth=" + image.width()
		Print "~t~tuses_trans=" + image.usesTrans()
		Print "~t~ttrans=" + image.trans()

		Print "~t}"
	Else
		Print "~timage={ (NULL) }"
	End If
End Function

Function DumpTile(tileArray:TMXTileArray)

	For Local i:Int = 0 Until tileArray.tileCount()
		Local tile:TMXTile = tileArray.get(i)
		
		Print "~ttile={"

		Print "~t~tid=" + tile.id()
		DumpProp(tile.properties(), 2)
		
		Print "~t}"

	Next
	
End Function
Rem
Function GetShape:String(shape:Int)
	Select shape
		Case TMX_SHAPE_NONE
			Return "none"
		Case TMX_SHAPE_SQUARE
			Return "square"
		Case TMX_SHAPE_ELLIPSE
			Return "ellipse"
		Case TMX_SHAPE_POLYGON
			Return "polygon"
		Case TMX_SHAPE_POLYLINE
			Return "polyline"
		Default
			Return "unknown"
	End Select
End Function
End Rem
