' Copyright (c) 2009 Bruce A Henderson
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

Import "source.bmx"

Extern

	Function bmx_gdal_GDALOpen:Byte Ptr(filename:String, access:Int)
	Function bmx_gdal_GDALAllRegister()

	Function bmx_gdal_GDALMajorObject_GetDescription:String(handle:Byte Ptr)
	Function bmx_gdal_GDALMajorObject_GetMetadataItem:String(handle:Byte Ptr, name:String, domain:String)

	Function bmx_gdal_GDALDataset_GetDriver:Byte Ptr(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterXSize:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterYSize:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterCount:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetGeoTransform:Int(handle:Byte Ptr, padfTransform:Double[])
	Function bmx_gdal_GDALDataset_GetProjectionRef:String(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterBand:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_gdal_GDALDataset_Close(handle:Byte Ptr)

	Function bmx_gdal_OGRRegisterAll()
	Function bmx_gdal_OGRCleanupAll()

	Function bmx_gdal_OGRSpatialReference_create:Byte Ptr(ref:String)
	Function bmx_gdal_OGRSpatialReference_free(handle:Byte Ptr)

	Function bmx_gdal_OGRSFDriverRegistrar_GetDriverCount:Int()
	Function bmx_gdal_OGRSFDriverRegistrar_GetDriver:Byte Ptr(index:Int)
	Function bmx_gdal_OGRSFDriverRegistrar_GetDriverByName:Byte Ptr(name:String)

	Function bmx_gdal_OGRSFDriver_CreateDataSource:Byte Ptr(handle:Byte Ptr, name:String, options:String[])

	Function bmx_gdal_OGRDataSource_CreateLayer:Byte Ptr(handle:Byte Ptr, name:String, spatialRef:Byte Ptr, gType:Int, options:String[])
	Function bmx_gdal_OGRDataSource_free(handle:Byte Ptr)

	Function bmx_gdal_GDALRasterBand_GenerateContour:Int(handle:Byte Ptr, contourInterval:Double, contourBase:Double, fixedLevels:Double[], ..
		useNoData:Int, noDataValue:Double, layer:Byte Ptr, idField:Int, elevField:Int)

	Function bmx_gdal_OGRFieldDefn_create:Byte Ptr(name:String, fieldType:Int)
	Function bmx_gdal_OGRFieldDefn_free(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_SetWidth(handle:Byte Ptr, width:Int)
	Function bmx_gdal_OGRFieldDefn_SetPrecision(handle:Byte Ptr, precision:Int)

	Function bmx_gdal_OGRLayer_CreateField:Int(handle:Byte Ptr, fld:Byte Ptr, approxOK:Int)


End Extern


Rem
bbdoc: Access Flag : Read only (no update) access.
End Rem
Const GA_ReadOnly:Int = 0
Rem
bbdoc: Access Flag : Read/write access.
End Rem
Const GA_Update:Int = 1


Const GDALMD_AREA_OR_POINT:String = "AREA_OR_POINT"
Const GDALMD_AOP_AREA:String = "Area"
Const GDALMD_AOP_POINT:String = "Point"
Const CPLE_WrongFormat:Int = 200
Const GDAL_DMD_LONGNAME:String = "DMD_LONGNAME"
Const GDAL_DMD_HELPTOPIC:String = "DMD_HELPTOPIC"
Const GDAL_DMD_MIMETYPE:String = "DMD_MIMETYPE"
Const GDAL_DMD_EXTENSION:String = "DMD_EXTENSION"
Const GDAL_DMD_CREATIONOPTIONLIST:String = "DMD_CREATIONOPTIONLIST"
Const GDAL_DMD_CREATIONDATATYPES:String = "DMD_CREATIONDATATYPES"
Const GDAL_DCAP_CREATE:String = "DCAP_CREATE"
Const GDAL_DCAP_CREATECOPY:String = "DCAP_CREATECOPY"
Const GDAL_DCAP_VIRTUALIO:String = "DCAP_VIRTUALIO"

Const GMF_ALL_VALID:Int = $01
Const GMF_PER_DATASET:Int = $02
Const GMF_ALPHA:Int = $04
Const GMF_NODATA:Int = $08


Const CE_None:Int = 0
Const CE_Debug:Int = 1
Const CE_Warning:Int = 2
Const CE_Failure:Int = 3
Const CE_Fatal:Int = 4

Rem
bbdoc: unknown type, non-standard
end rem
Const wkbUnknown:Int = 0
Rem
bbdoc: 0-dimensional geometric object, standard WKB
end rem
Const wkbPoint:Int = 1
Rem
bbdoc: 1-dimensional geometric object with linear interpolation between Points, standard WKB
end rem
Const wkbLineString:Int = 2
Rem
bbdoc: planar 2-dimensional geometric object defined by 1 exterior boundary and 0 or more interior boundaries, standard WKB
end rem
Const wkbPolygon:Int = 3
Rem
bbdoc: GeometryCollection of Points, standard WKB
end rem
Const wkbMultiPoint:Int = 4
Rem
bbdoc: GeometryCollection of LineStrings, standard WKB
end rem
Const wkbMultiLineString:Int = 5
Rem
bbdoc: GeometryCollection of Polygons, standard WKB
end rem
Const wkbMultiPolygon:Int = 6
Rem
bbdoc: geometric object that is a collection of 1 or more geometric objects, standard WKB
end rem
Const wkbGeometryCollection:Int = 7
Rem
bbdoc: non-standard, for pure attribute records
end rem
Const wkbNone:Int = 8
Rem
bbdoc: non-standard, just for createGeometry()
end rem
Const wkbLinearRing:Int = 9
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbPoint25D:Int = 10
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbLineString25D:Int = 11
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbPolygon25D:Int = 12
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbMultiPoint25D:Int = 13
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbMultiLineString25D:Int = 14
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbMultiPolygon25D:Int = 15
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbGeometryCollection25D:Int = 16


Rem
bbdoc: Simple 32bit integer
end rem
Const OFTInteger:Int = 0
Rem
bbdoc: List of 32bit integers
end rem
Const OFTIntegerList:Int = 1
Rem
bbdoc: Double Precision floating point
end rem
Const OFTReal:Int = 2
Rem
bbdoc: List of doubles
end rem
Const OFTRealList:Int = 3
Rem
bbdoc: String of ASCII chars
end rem
Const OFTString:Int = 4
Rem
bbdoc: Array of strings
end rem
Const OFTStringList:Int = 5
Rem
bbdoc: deprecated
end rem
Const OFTWideString:Int = 6
Rem
bbdoc: deprecated
end rem
Const OFTWideStringList:Int = 7
Rem
bbdoc: Raw Binary data
end rem
Const OFTBinary:Int = 8
Rem
bbdoc: Date
end rem
Const OFTDate:Int = 9
Rem
bbdoc: Time
end rem
Const OFTTime:Int = 10
Rem
bbdoc: Date and Time
end rem
Const OFTDateTime:Int = 11



