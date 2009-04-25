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
	Function bmx_gdal_GDALOpenShared:Byte Ptr(filename:String, access:Int)
	Function bmx_gdal_GDALAllRegister()

	Function bmx_gdal_GDALMajorObject_GetDescription:String(handle:Byte Ptr)
	Function bmx_gdal_GDALMajorObject_GetMetadataItem:String(handle:Byte Ptr, name:String, domain:String)
	Function bmx_gdal_GDALMajorObject_GetMetadata:String[](handle:Byte Ptr, domain:String)
	Function bmx_gdal_GDALMajorObject_SetMetadataItem:Int(handle:Byte Ptr, name:String, value:String, domain:String)
	Function bmx_gdal_GDALMajorObject_SetMetadata:Int(handle:Byte Ptr, metadata:String[], domain:String)
	Function bmx_gdal_GDALMajorObject_SetDescription(handle:Byte Ptr, description:String)

	Function bmx_gdal_GDALDataset_GetDriver:Byte Ptr(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterXSize:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterYSize:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterCount:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetGeoTransform:Int(handle:Byte Ptr, padfTransform:Double[])
	Function bmx_gdal_GDALDataset_GetProjectionRef:String(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetRasterBand:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_gdal_GDALDataset_Close(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_SetProjection:Int(handle:Byte Ptr, projection:String)
	Function bmx_gdal_GDALDataset_SetGeoTransform:Int(handle:Byte Ptr, transform:Double[])
	Function bmx_gdal_GDALDataset_GetGCPCount:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_FlushCache(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_AddBand:Int(handle:Byte Ptr, dataType:Int, options:String[])
	Function bmx_gdal_GDALDataset_GetFileList:String[](handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_GetGCPProjection:String(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_CreateMaskBand:Int(handle:Byte Ptr, flags:Int)
	Function bmx_gdal_GDALDataset_GetShared:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALDataset_MarkAsShared(handle:Byte Ptr)

	Function bmx_gdal_GDALDriverManager_GetDriverCount:Int()
	Function bmx_gdal_GDALDriverManager_GetDriver:Byte Ptr(index:Int)
	Function bmx_gdal_GDALDriverManager_GetDriverByName:Byte Ptr(name:String)

	Function bmx_gdal_GDALDriver_GetShortName:String(handle:Byte Ptr)
	Function bmx_gdal_GDALDriver_GetLongName:String(handle:Byte Ptr)
	Function bmx_gdal_GDALDriver_CreateCopy:Byte Ptr(handle:Byte Ptr, filename:String, sourceDataset:Byte Ptr, _strict:Int, options:String[])
	Function bmx_gdal_GDALDriver_DeleteDataset:Int(handle:Byte Ptr, filename:String)
	Function bmx_gdal_GDALDriver_RenameDataset:Int(handle:Byte Ptr, newName:String, oldName:String)
	Function bmx_gdal_GDALDriver_CopyFiles:Int(handle:Byte Ptr, newName:String, oldName:String)
	Function bmx_gdal_GDALDriver_CreateDataset:Byte Ptr(handle:Byte Ptr, filename:String, xSize:Int, ySize:Int, bands:Int, dataType:Int, paramList:String[])

	Function bmx_gdal_GDALRasterBand_GenerateContour:Int(handle:Byte Ptr, contourInterval:Double, contourBase:Double, fixedLevels:Double[], ..
		useNoData:Int, noDataValue:Double, layer:Byte Ptr, idField:Int, elevField:Int)
	Function bmx_gdal_GDALRasterBand_GetXSize:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetYSize:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetBand:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetColorInterpretation:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetMaskFlags:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetRasterDataType:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetMaximum:Double(handle:Byte Ptr, success:Int Ptr)
	Function bmx_gdal_GDALRasterBand_GetMinimum:Double(handle:Byte Ptr, success:Int Ptr)
	Function bmx_gdal_GDALRasterBand_GetNoDataValue:Double(handle:Byte Ptr, success:Int Ptr)
	Function bmx_gdal_GDALRasterBand_GetOffset:Double(handle:Byte Ptr, success:Int Ptr)
	Function bmx_gdal_GDALRasterBand_GetBlockSize(handle:Byte Ptr, xSize:Int Ptr, ySize:Int Ptr)
	Function bmx_gdal_GDALRasterBand_GetAccess:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_FlushCache:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetCategoryNames:String[](handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetScale:Double(handle:Byte Ptr, success:Int Ptr)
	Function bmx_gdal_GDALRasterBand_GetUnitType:String(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_Fill:Int(handle:Byte Ptr, realValue:Double, imaginaryValue:Double )
	Function bmx_gdal_GDALRasterBand_SetNoDataValue:Int(handle:Byte Ptr, value:Double)
	Function bmx_gdal_GDALRasterBand_SetColorInterpretation:Int(handle:Byte Ptr, interp:Int)
	Function bmx_gdal_GDALRasterBand_SetOffset:Int(handle:Byte Ptr, offset:Double)
	Function bmx_gdal_GDALRasterBand_SetScale:Int(handle:Byte Ptr, scale:Double)
	Function bmx_gdal_GDALRasterBand_SetUnitType:Int(handle:Byte Ptr, unitType:String)
	Function bmx_gdal_GDALRasterBand_HasArbitraryOverviews:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetOverviewCount:Int(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_GetDataset:Byte Ptr(handle:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_SetCategoryNames:Int(handle:Byte Ptr, names:String[])
	Function bmx_gdal_GDALRasterBand_GetStatistics:Int(handle:Byte Ptr, approxOK:Int, force:Int, _min:Double Ptr, _max:Double Ptr, _mean:Double Ptr, _stddev:Double Ptr)
	Function bmx_gdal_GDALRasterBand_SetStatistics:Int(handle:Byte Ptr, _min:Double, _max:Double, _mean:Double, _stddev:Double)
	Function bmx_gdal_GDALRasterBand_GetOverview:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_gdal_GDALRasterBand_GetRasterSampleOverview:Byte Ptr(handle:Byte Ptr, desiredSamples:Int)
	Function bmx_gdal_GDALRasterBand_CreateMaskBand:Int(handle:Byte Ptr, flags:Int)
	Function bmx_gdal_GDALRasterBand_RasterIO:Int(handle:Byte Ptr, rwFlag:Int, xOff:Int, yOff:Int, xSize:Int, ySize:Int, data:Byte Ptr, bufXSize:Int, bufYSize:Int, ..
			bufType:Int, pixelSpace:Int, lineSpace:Int)
	Function bmx_gdal_GDALRasterBand_ReadBlock:Int(handle:Byte Ptr, xBlockOff:Int, yBlockOff:Int, image:Byte Ptr)
	Function bmx_gdal_GDALRasterBand_WriteBlock:Int(handle:Byte Ptr, xBlockOff:Int, yBlockOff:Int, image:Byte Ptr)

	Function bmx_gdal_GDAL_GCP_create:Byte Ptr(id:String, info:String, pixel:Double, line:Double, x:Double, y:Double, z:Double)
	Function bmx_gdal_GDAL_GCP_GetID:String(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetId(handle:Byte Ptr, id:String)
	Function bmx_gdal_GDAL_GCP_GetInfo:String(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetInfo(handle:Byte Ptr, info:String)
	Function bmx_gdal_GDAL_GCP_GetPixel:Double(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetPixel(handle:Byte Ptr, pixel:Double)
	Function bmx_gdal_GDAL_GCP_GetLine:Double(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetLine(handle:Byte Ptr, line:Double)
	Function bmx_gdal_GDAL_GCP_GetX:Double(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetX(handle:Byte Ptr, x:Double)
	Function bmx_gdal_GDAL_GCP_GetY:Double(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetY(handle:Byte Ptr, y:Double)
	Function bmx_gdal_GDAL_GCP_GetZ:Double(handle:Byte Ptr)
	Function bmx_gdal_GDAL_GCP_SetZ(handle:Byte Ptr, z:Double)
	Function bmx_gdal_GDAL_GCP_free(handle:Byte Ptr)

	Function bmx_gdal_OGRRegisterAll()
	Function bmx_gdal_OGRCleanupAll()

	Function bmx_gdal_OGRSpatialReference_create:Byte Ptr(ref:String)
	Function bmx_gdal_OGRSpatialReference_free(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_Clone:Byte Ptr(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_CloneGeogCS:Byte Ptr(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_ExportToWkt:String(handle:Byte Ptr, result:Int Ptr)
	Function bmx_gdal_OGRSpatialReference_morphToESRI:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_morphFromESRI:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_Validate:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_FixupOrdering:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_Fixup:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_EPSGTreatsAsLatLong:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_SetLinearUnitsAndUpdateParameters:Int(handle:Byte Ptr, name:String, inMeters:Double)
	Function bmx_gdal_OGRSpatialReference_SetLinearUnits:Int(handle:Byte Ptr, name:String, inMeters:Double)
	Function bmx_gdal_OGRSpatialReference_GetLinearUnits:String(handle:Byte Ptr, units:Double Ptr)
	Function bmx_gdal_OGRSpatialReference_SetAngularUnits(handle:Byte Ptr, name:String, inDegrees:Double)
	Function bmx_gdal_OGRSpatialReference_GetAngularUnits:String(handle:Byte Ptr, units:Double Ptr)
	Function bmx_gdal_OGRSpatialReference_GetPrimeMeridian:String(handle:Byte Ptr, meridian:Double Ptr)
	Function bmx_gdal_OGRSpatialReference_IsGeographic:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_IsProjected:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_IsLocal:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_Clear(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_SetLocalCS:Int(handle:Byte Ptr, name:String)
	Function bmx_gdal_OGRSpatialReference_SetProjCS:Int(handle:Byte Ptr, name:String)
	Function bmx_gdal_OGRSpatialReference_SetProjection:Int(handle:Byte Ptr, name:String)
	Function bmx_gdal_OGRSpatialReference_SetWellKnownGeogCS:Int(handle:Byte Ptr, name:String)
	Function bmx_gdal_OGRSpatialReference_SetFromUserInput:Int(handle:Byte Ptr, definition:String)
	Function bmx_gdal_OGRSpatialReference_GetSemiMajor:Int(handle:Byte Ptr, semiMajor:Double Ptr)
	Function bmx_gdal_OGRSpatialReference_GetSemiMinor:Int(handle:Byte Ptr, semiMinor:Double Ptr)
	Function bmx_gdal_OGRSpatialReference_GetInvFlattening:Int(handle:Byte Ptr, invFlattening:Double Ptr)
	Function bmx_gdal_OGRSpatialReference_SetAuthority:Int(handle:Byte Ptr, targetKey:String, authority:String, code:Int)
	Function bmx_gdal_OGRSpatialReference_AutoIdentifyEPSG:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRSpatialReference_GetAuthorityCode:String(handle:Byte Ptr, targetKey:String)
	Function bmx_gdal_OGRSpatialReference_GetAuthorityName:String(handle:Byte Ptr, targetKey:String)
	Function bmx_gdal_OGRSpatialReference_GetExtension:String(handle:Byte Ptr, targetKey:String, name:String, defaultValue:String)
	Function bmx_gdal_OGRSpatialReference_SetExtension:Int(handle:Byte Ptr, targetKey:String, name:String, value:String)
	Function bmx_gdal_OGRSpatialReference_SetProjParm:Int(handle:Byte Ptr, name:String, value:Double)
	Function bmx_gdal_OGRSpatialReference_GetProjParm:Int(handle:Byte Ptr, name:String, result:Double Ptr, defaultValue:Double)
	Function bmx_gdal_OGRSpatialReference_SetNormProjParm:Int(handle:Byte Ptr, name:String, value:Double)
	Function bmx_gdal_OGRSpatialReference_GetNormProjParm:Int(handle:Byte Ptr, name:String, result:Double Ptr, defaultValue:Double)
	Function bmx_gdal_OGRSpatialReference_SetUTM:Int(handle:Byte Ptr, zone:Int, north:Int)
	Function bmx_gdal_OGRSpatialReference_GetUTMZone:Int(handle:Byte Ptr, north:Int Ptr)

	Function bmx_gdal_OGRSFDriverRegistrar_GetDriverCount:Int()
	Function bmx_gdal_OGRSFDriverRegistrar_GetDriver:Byte Ptr(index:Int)
	Function bmx_gdal_OGRSFDriverRegistrar_GetDriverByName:Byte Ptr(name:String)
	Function bmx_gdal_OGRSFDriverRegistrar_GetOpenDSCount:Int()
	Function bmx_gdal_OGRSFDriverRegistrar_GetOpenDS:Byte Ptr(ids:Int)

	Function bmx_gdal_OGRSFDriver_CreateDataSource:Byte Ptr(handle:Byte Ptr, name:String, options:String[])
	Function bmx_gdal_OGRSFDriver_Open:Byte Ptr(handle:Byte Ptr, name:String, update:Int)
	Function bmx_gdal_OGRSFDriver_TestCapability:Int(handle:Byte Ptr, capability:String)
	Function bmx_gdal_OGRSFDriver_DeleteDataSource:Int(handle:Byte Ptr, datasource:String)
	Function bmx_gdal_OGRSFDriver_GetName:String(handle:Byte Ptr)

	Function bmx_gdal_OGRDataSource_CreateLayer:Byte Ptr(handle:Byte Ptr, name:String, spatialRef:Byte Ptr, gType:Int, options:String[])
	Function bmx_gdal_OGRDataSource_free(handle:Byte Ptr)
	Function bmx_gdal_OGRDataSource_GetName:String(handle:Byte Ptr)
	Function bmx_gdal_OGRDataSource_GetLayerCount:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRDataSource_GetLayer:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_gdal_OGRDataSource_GetLayerByName:Byte Ptr(handle:Byte Ptr, name:String)
	Function bmx_gdal_OGRDataSource_DeleteLayer:Int(handle:Byte Ptr, layer:Int)
	Function bmx_gdal_OGRDataSource_TestCapability:Int(handle:Byte Ptr, name:String)

	Function bmx_gdal_OGRFieldDefn_create:Byte Ptr(name:String, fieldType:Int)
	Function bmx_gdal_OGRFieldDefn_free(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_SetWidth(handle:Byte Ptr, width:Int)
	Function bmx_gdal_OGRFieldDefn_SetPrecision(handle:Byte Ptr, precision:Int)
	Function bmx_gdal_OGRFieldDefn_SetName(handle:Byte Ptr, name:String)
	Function bmx_gdal_OGRFieldDefn_GetNameRef:String(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_GetType:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_SetType(handle:Byte Ptr, fieldType:Int)
	Function bmx_gdal_OGRFieldDefn_GetJustify:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_SetJustify(handle:Byte Ptr, justify:Int)
	Function bmx_gdal_OGRFieldDefn_GetWidth:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_GetPrecision:Int(handle:Byte Ptr)
	Function bmx_gdal_OGRFieldDefn_Set(handle:Byte Ptr, name:String, fieldType:Int, width:Int, precision:Int, justify:Int)

	Function bmx_gdal_OGRLayer_CreateField:Int(handle:Byte Ptr, fld:Byte Ptr, approxOK:Int)

	Function bmx_gdal_VRTDataset_create:Byte Ptr(width:Int, height:Int)
	Function bmx_gdal_VRTDataset_AddBand:Int(handle:Byte Ptr, dataType:Int, options:String[])
	
	Function bmx_gdal_VRTRasterBand_CopyCommonInfoFrom:Int(handle:Byte Ptr, source:Byte Ptr)

	Function bmx_gdal_VRTSourcedRasterBand_AddSimpleSource:Int(handle:Byte Ptr, srcBand:Byte Ptr, srcXOff:Int, srcYOff:Int, srcXSize:Int, srcYSize:Int, ..
			dstXOff:Int, dstYOff:Int, dstXSize:Int, dstYSize:Int, resampling:String, noDataValue:Double)

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
Const wkbNone:Int = 100
Rem
bbdoc: non-standard, just for createGeometry()
end rem
Const wkbLinearRing:Int = 101
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbPoint25D:Int = $80000001
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbLineString25D:Int = $80000002
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbPolygon25D:Int = $80000003
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbMultiPoint25D:Int = $80000004
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbMultiLineString25D:Int = $80000005
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbMultiPolygon25D:Int = $80000006
Rem
bbdoc: 2.5D extension as per 99-402
end rem
Const wkbGeometryCollection25D:Int = $80000007


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

Rem
bbdoc: 
End Rem
Const GDT_Unknown:Int = 0
Rem
bbdoc: Eight bit unsigned integer
End Rem
Const GDT_Byte:Int = 1
Rem
bbdoc: Sixteen bit unsigned integer
end rem
Const GDT_UInt16:Int = 2
Rem
bbdoc: Sixteen bit signed integer
end rem
Const GDT_Int16:Int = 3
Rem
bbdoc: Thirty two bit unsigned integer
end rem
Const GDT_UInt32:Int = 4
Rem
bbdoc: Thirty two bit signed integer
end rem
Const GDT_Int32:Int = 5
Rem
bbdoc: Thirty two bit floating point
end rem
Const GDT_Float32:Int = 6
Rem
bbdoc: Sixty four bit floating point
end rem
Const GDT_Float64:Int = 7
Rem
bbdoc: Complex Int16
end rem
Const GDT_CInt16:Int = 8
Rem
bbdoc: Complex Int32
end rem
Const GDT_CInt32:Int = 9
Rem
bbdoc: Complex Float32
end rem
Const GDT_CFloat32:Int = 10
Rem
bbdoc: Complex Float64
end rem
Const GDT_CFloat64:Int = 11


Const GCI_Undefined:Int = 0
Const GCI_GrayIndex:Int = 1
Const GCI_PaletteIndex:Int = 2
Const GCI_RedBand:Int = 3
Const GCI_GreenBand:Int = 4
Const GCI_BlueBand:Int = 5
Const GCI_AlphaBand:Int = 6
Const GCI_HueBand:Int = 7
Const GCI_SaturationBand:Int = 8
Const GCI_LightnessBand:Int = 9
Const GCI_CyanBand:Int = 10
Const GCI_MagentaBand:Int = 11
Const GCI_YellowBand:Int = 12
Const GCI_BlackBand:Int = 13
Const GCI_YCbCr_YBand:Int = 14
Const GCI_YCbCr_CbBand:Int = 15
Const GCI_YCbCr_CrBand:Int = 16

Const CE_None:Int = 0
Const CE_Debug:Int = 1
Const CE_Warning:Int = 2
Const CE_Failure:Int = 3
Const CE_Fatal:Int = 4

Const OJUndefined:Int = 0
Const OJLeft:Int = 1
Const OJRight:Int = 2

Const OLCRandomRead:String = "RandomRead"
Const OLCSequentialWrite:String = "SequentialWrite"
Const OLCRandomWrite:String = "RandomWrite"
Const OLCFastSpatialFilter:String = "FastSpatialFilter"
Const OLCFastFeatureCount:String = "FastFeatureCount"
Const OLCFastGetExtent:String = "FastGetExtent"
Const OLCCreateField:String = "CreateField"
Const OLCTransactions:String = "Transactions"
Const OLCDeleteFeature:String = "DeleteFeature"
Const OLCFastSetNextByIndex:String = "FastSetNextByIndex"
Const OLCStringsAsUTF8:String = "StringsAsUTF8"
Const ODsCCreateLayer:String = "CreateLayer"
Const ODsCDeleteLayer:String = "DeleteLayer"
Const ODrCCreateDataSource:String = "CreateDataSource"
Const ODrCDeleteDataSource:String = "DeleteDataSource"

Const OGRSTCNone:Int = 0
Const OGRSTCPen:Int = 1
Const OGRSTCBrush:Int = 2
Const OGRSTCSymbol:Int = 3
Const OGRSTCLabel:Int = 4
Const OGRSTCVector:Int = 5

Const OGRSTUGround:Int = 0
Const OGRSTUPixel:Int = 1
Const OGRSTUPoints:Int = 2
Const OGRSTUMM:Int = 3
Const OGRSTUCM:Int = 4
Const OGRSTUInches:Int = 5

Const OGRSTPenColor:Int = 0
Const OGRSTPenWidth:Int = 1
Const OGRSTPenPattern:Int = 2
Const OGRSTPenId:Int = 3
Const OGRSTPenPerOffset:Int = 4
Const OGRSTPenCap:Int = 5
Const OGRSTPenJoin:Int = 6
Const OGRSTPenPriority:Int = 7
 
Const OGRSTBrushFColor:Int = 0
Const OGRSTBrushBColor:Int = 1
Const OGRSTBrushId:Int = 2
Const OGRSTBrushAngle:Int = 3
Const OGRSTBrushSize:Int = 4
Const OGRSTBrushDx:Int = 5
Const OGRSTBrushDy:Int = 6
Const OGRSTBrushPriority:Int = 7
  
Const OGRSTSymbolId:Int = 0
Const OGRSTSymbolAngle:Int = 1
Const OGRSTSymbolColor:Int = 2
Const OGRSTSymbolSize:Int = 3
Const OGRSTSymbolDx:Int = 4
Const OGRSTSymbolDy:Int = 5
Const OGRSTSymbolStep:Int = 6
Const OGRSTSymbolPerp:Int = 7
Const OGRSTSymbolOffset:Int = 8
Const OGRSTSymbolPriority:Int = 9
Const OGRSTSymbolFontName:Int = 10
Const OGRSTSymbolOColor:Int = 11

Const OGRSTLabelFontName:Int = 0
Const OGRSTLabelSize:Int = 1
Const OGRSTLabelTextString:Int = 2
Const OGRSTLabelAngle:Int = 3
Const OGRSTLabelFColor:Int = 4
Const OGRSTLabelBColor:Int = 5
Const OGRSTLabelPlacement:Int = 6
Const OGRSTLabelAnchor:Int = 7
Const OGRSTLabelDx:Int = 8
Const OGRSTLabelDy:Int = 9
Const OGRSTLabelPerp:Int = 10
Const OGRSTLabelBold:Int = 11
Const OGRSTLabelItalic:Int = 12
Const OGRSTLabelUnderline:Int = 13
Const OGRSTLabelPriority:Int = 14
Const OGRSTLabelStrikeout:Int = 15
Const OGRSTLabelStretch:Int = 16
Const OGRSTLabelAdjHor:Int = 17
Const OGRSTLabelAdjVert:Int = 18
Const OGRSTLabelHColor:Int = 19
Const OGRSTLabelOColor:Int = 20
Const OGRSTLabelLast:Int = 21

Const SRS_PT_ALBERS_CONIC_EQUAL_AREA:String = "Albers_Conic_Equal_Area"
Const SRS_PT_AZIMUTHAL_EQUIDISTANT:String = "Azimuthal_Equidistant"
Const SRS_PT_CASSINI_SOLDNER:String = "Cassini_Soldner"
Const SRS_PT_CYLINDRICAL_EQUAL_AREA:String = "Cylindrical_Equal_Area"
Const SRS_PT_BONNE:String = "Bonne"
Const SRS_PT_ECKERT_I:String = "Eckert_I"
Const SRS_PT_ECKERT_II:String = "Eckert_II"
Const SRS_PT_ECKERT_III:String = "Eckert_III"
Const SRS_PT_ECKERT_IV:String = "Eckert_IV"
Const SRS_PT_ECKERT_V:String = "Eckert_V"
Const SRS_PT_ECKERT_VI:String = "Eckert_VI"
Const SRS_PT_EQUIDISTANT_CONIC:String = "Equidistant_Conic"
Const SRS_PT_EQUIRECTANGULAR:String = "Equirectangular"
Const SRS_PT_GALL_STEREOGRAPHIC:String = "Gall_Stereographic"
Const SRS_PT_GAUSSSCHREIBERTMERCATOR:String = "Gauss_Schreiber_Transverse_Mercator"
Const SRS_PT_GEOSTATIONARY_SATELLITE:String = "Geostationary_Satellite"
Const SRS_PT_GOODE_HOMOLOSINE:String = "Goode_Homolosine"
Const SRS_PT_GNOMONIC:String = "Gnomonic"
Const SRS_PT_HOTINE_OBLIQUE_MERCATOR:String = "Hotine_Oblique_Mercator"
Const SRS_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN:String = "Hotine_Oblique_Mercator_Two_Point_Natural_Origin"
Const SRS_PT_LABORDE_OBLIQUE_MERCATOR:String = "Laborde_Oblique_Mercator"
Const SRS_PT_LAMBERT_CONFORMAL_CONIC_1SP:String = "Lambert_Conformal_Conic_1SP"
Const SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP:String = "Lambert_Conformal_Conic_2SP"
Const SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP_BELGIUM:String = "Lambert_Conformal_Conic_2SP_Belgium"
Const SRS_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA:String = "Lambert_Azimuthal_Equal_Area"
Const SRS_PT_MERCATOR_1SP:String = "Mercator_1SP"
Const SRS_PT_MERCATOR_2SP:String = "Mercator_2SP"
Const SRS_PT_MILLER_CYLINDRICAL:String = "Miller_Cylindrical"
Const SRS_PT_MOLLWEIDE:String = "Mollweide"
Const SRS_PT_NEW_ZEALAND_MAP_GRID:String = "New_Zealand_Map_Grid"
Const SRS_PT_OBLIQUE_STEREOGRAPHIC:String = "Oblique_Stereographic"
Const SRS_PT_ORTHOGRAPHIC:String = "Orthographic"
Const SRS_PT_POLAR_STEREOGRAPHIC:String = "Polar_Stereographic"
Const SRS_PT_POLYCONIC:String = "Polyconic"
Const SRS_PT_ROBINSON:String = "Robinson"
Const SRS_PT_SINUSOIDAL:String = "Sinusoidal"
Const SRS_PT_STEREOGRAPHIC:String = "Stereographic"
Const SRS_PT_SWISS_OBLIQUE_CYLINDRICAL:String = "Swiss_Oblique_Cylindrical"
Const SRS_PT_TRANSVERSE_MERCATOR:String = "Transverse_Mercator"
Const SRS_PT_TRANSVERSE_MERCATOR_SOUTH_ORIENTED:String = "Transverse_Mercator_South_Orientated"
 
' special mapinfo variants on Transverse Mercator
Const SRS_PT_TRANSVERSE_MERCATOR_MI_21:String = "Transverse_Mercator_MapInfo_21"
Const SRS_PT_TRANSVERSE_MERCATOR_MI_22:String = "Transverse_Mercator_MapInfo_22"
Const SRS_PT_TRANSVERSE_MERCATOR_MI_23:String = "Transverse_Mercator_MapInfo_23"
Const SRS_PT_TRANSVERSE_MERCATOR_MI_24:String = "Transverse_Mercator_MapInfo_24"
Const SRS_PT_TRANSVERSE_MERCATOR_MI_25:String = "Transverse_Mercator_MapInfo_25"

Const SRS_PT_TUNISIA_MINING_GRID:String = "Tunisia_Mining_Grid"
Const SRS_PT_TWO_POINT_EQUIDISTANT:String = "Two_Point_Equidistant"
Const SRS_PT_VANDERGRINTEN:String = "VanDerGrinten"
Const SRS_PT_KROVAK:String = "Krovak"
Const SRS_PT_IMW_POLYCONIC:String = "International_Map_of_the_World_Polyconic"
Const SRS_PT_WAGNER_I:String = "Wagner_I"
Const SRS_PT_WAGNER_II:String = "Wagner_II"
Const SRS_PT_WAGNER_III:String = "Wagner_III"
Const SRS_PT_WAGNER_IV:String = "Wagner_IV"
Const SRS_PT_WAGNER_V:String = "Wagner_V"
Const SRS_PT_WAGNER_VI:String = "Wagner_VI"
Const SRS_PT_WAGNER_VII:String = "Wagner_VII"

Const SRS_PP_CENTRAL_MERIDIAN:String = "central_meridian"
Const SRS_PP_SCALE_FACTOR:String = "scale_factor"
Const SRS_PP_STANDARD_PARALLEL_1:String = "standard_parallel_1"
Const SRS_PP_STANDARD_PARALLEL_2:String = "standard_parallel_2"
Const SRS_PP_PSEUDO_STD_PARALLEL_1:String = "pseudo_standard_parallel_1"
Const SRS_PP_LONGITUDE_OF_CENTER:String = "longitude_of_center"
Const SRS_PP_LATITUDE_OF_CENTER:String = "latitude_of_center"
Const SRS_PP_LONGITUDE_OF_ORIGIN:String = "longitude_of_origin"
Const SRS_PP_LATITUDE_OF_ORIGIN:String = "latitude_of_origin"
Const SRS_PP_FALSE_EASTING:String = "false_easting"
Const SRS_PP_FALSE_NORTHING:String = "false_northing"
Const SRS_PP_AZIMUTH:String = "azimuth"
Const SRS_PP_LONGITUDE_OF_POINT_1:String = "longitude_of_point_1"
Const SRS_PP_LATITUDE_OF_POINT_1:String = "latitude_of_point_1"
Const SRS_PP_LONGITUDE_OF_POINT_2:String = "longitude_of_point_2"
Const SRS_PP_LATITUDE_OF_POINT_2:String = "latitude_of_point_2"
Const SRS_PP_LONGITUDE_OF_POINT_3:String = "longitude_of_point_3"
Const SRS_PP_LATITUDE_OF_POINT_3:String = "latitude_of_point_3"
Const SRS_PP_RECTIFIED_GRID_ANGLE:String = "rectified_grid_angle"
Const SRS_PP_LANDSAT_NUMBER:String = "landsat_number"
Const SRS_PP_PATH_NUMBER:String = "path_number"
Const SRS_PP_PERSPECTIVE_POINT_HEIGHT:String = "perspective_point_height"
Const SRS_PP_SATELLITE_HEIGHT:String = "satellite_height"
Const SRS_PP_FIPSZONE:String = "fipszone"
Const SRS_PP_ZONE:String = "zone"
Const SRS_PP_LATITUDE_OF_1ST_POINT:String = "Latitude_Of_1st_Point"
Const SRS_PP_LONGITUDE_OF_1ST_POINT:String = "Longitude_Of_1st_Point"
Const SRS_PP_LATITUDE_OF_2ND_POINT:String = "Latitude_Of_2nd_Point"
Const SRS_PP_LONGITUDE_OF_2ND_POINT:String = "Longitude_Of_2nd_Point"

Const SRS_UL_METER:String = "Meter"
Const SRS_UL_FOOT:String = "Foot (International)" 
Const SRS_UL_FOOT_CONV:String = "0.3048"
Const SRS_UL_US_FOOT:String = "Foot_US" 
Const SRS_UL_US_FOOT_CONV:String = "0.3048006096012192"
Const SRS_UL_NAUTICAL_MILE:String = "Nautical Mile"
Const SRS_UL_NAUTICAL_MILE_CONV:String = "1852.0"
Const SRS_UL_LINK:String = "Link"          ' Based on US Foot
Const SRS_UL_LINK_CONV:String = "0.20116684023368047"
Const SRS_UL_CHAIN:String = "Chain"         ' based on US Foot
Const SRS_UL_CHAIN_CONV:String = "20.116684023368047"
Const SRS_UL_ROD:String = "Rod"           ' based on US Foot
Const SRS_UL_ROD_CONV:String = "5.02921005842012"

Rem
bbdoc: Read data.
End Rem
Const GF_Read:Int = 0
Rem
bbdoc: Write data
End Rem
Const GF_Write:Int = 1

