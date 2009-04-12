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

Rem
bbdoc: GDAL
about: Translator library for raster geospatial data formats.
End Rem
Module BaH.GDAL

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: GDAL - Copyright (c) 2000, Frank Warmerdam"
ModuleInfo "Copyright: Wrapper - 2009 Bruce A Henderson"

ModuleInfo "CC_OPTS: -DCPL_DISABLE_DLL -DCPL_DISABLE_STDCALL -DCURL_STATICLIB"
ModuleInfo "CC_OPTS: -DPIXARLOG_SUPPORT -DZIP_SUPPORT -DJPEG_SUPPORT -DOJPEG_SUPPORT -DBIGTIFF_SUPPORT"
ModuleInfo "CC_OPTS: -DHAVE_EXPAT -DUSE_IN_GDAL -DFALSE=0 -DTRUE=1 -DGDAL_SUPPORT"
ModuleInfo "CC_OPTS: -DFRMT_gxf -DFRMT_gtiff -DFRMT_hfa -DFRMT_aigrid -DFRMT_aaigrid -DFRMT_ceos -DFRMT_ceos2"
ModuleInfo "CC_OPTS: -DFRMT_iso8211 -DFRMT_xpm -DFRMT_sdts -DFRMT_raw -DFRMT_dted -DFRMT_mem -DFRMT_jdem -DFRMT_envisat"
ModuleInfo "CC_OPTS: -DFRMT_elas -DFRMT_fit -DFRMT_vrt -DFRMT_usgsdem -DFRMT_l1b -DFRMT_nitf -DFRMT_bmp -DFRMT_pcidsk"
ModuleInfo "CC_OPTS: -DFRMT_airsar -DFRMT_rs2 -DFRMT_ilwis -DFRMT_rmf -DFRMT_leveller -DFRMT_sgi -DFRMT_srtmhgt -DFRMT_idrisi"
ModuleInfo "CC_OPTS: -DFRMT_gsg -DFRMT_ingr -DFRMT_ers -DFRMT_jaxapalsar -DFRMT_dimap -DFRMT_gff -DFRMT_cosar -DFRMT_pds"
ModuleInfo "CC_OPTS: -DFRMT_adrg -DFRMT_coasp -DFRMT_tsx -DFRMT_terragen -DFRMT_blx -DFRMT_msgn -DFRMT_wcs -DFRMT_wms"
ModuleInfo "CC_OPTS: -DFRMT_bsb -DFRMT_gif -DFRMT_png -DFRMT_pcraster"

ModuleInfo "CC_OPTS: -DSHAPE_ENABLED -DTAB_ENABLED -DNTF_ENABLED -DSDTS_ENABLED -DTIGER_ENABLED -DS57_ENABLED -DDGN_ENABLED"
ModuleInfo "CC_OPTS: -DVRT_ENABLED -DREC_ENABLED -DMEM_ENABLED -DBNA_ENABLED -DCSV_ENABLED -DGML_ENABLED"
ModuleInfo "CC_OPTS: -DGPX_ENABLED -DKML_ENABLED -DGEOJSON_ENABLED -DGMT_ENABLED"
ModuleInfo "CC_OPTS: -DXPLANE_ENABLED -DAVCBIN_ENABLED -DGEOCONCEPT_ENABLED "

Import "common.bmx"

'
' Build Notes :
'
' port/cpl_config.h
'    Modified for WIN32 compile options
'


Rem
bbdoc: Opens a raster file as a GDALDataset.
about: This function will try to open the passed file, or virtual dataset name by invoking the Open method of each
registered GDALDriver in turn. The first successful open will result in a returned dataset. If all drivers fail then NULL is returned.
<p>
Several recommendations :
<ul>
<li>If you open a dataset object with GA_Update access, it is not recommanded to open a new dataset on the same underlying file.</li>
<li>The returned dataset should only be accessed by one thread at a time. If you want to use it from different threads, you must
add all necessary code (mutexes, etc.) to avoid concurrent use of the object. (Some drivers, such as GeoTIFF, maintain internal state
variables that are updated each time a new block is read, thus preventing concurrent use.)</li>
</ul>
</p>
End Rem
Function GDALOpen:GDALDataset(filename:String, access:Int)
	Return GDALDataset._create(bmx_gdal_GDALOpen(filename, access))
End Function

Rem
bbdoc: Register all known configured GDAL drivers. 
about: Drives any of the following that are configured into GDAL. Many others as well haven't been updated in this
documentation (see <a href="http://gdal.org/formats_list.html">full list</a>):
<ul>
<li>GeoTIFF (GTiff) </li>
<li>Geosoft GXF (GXF) </li>
<li>Erdas Imagine (HFA) </li>
<li>CEOS (CEOS) </li>
<li>ELAS (ELAS) </li>
<li>Arc/Info Binary Grid (AIGrid) </li>
<li>SDTS Raster DEM (SDTS) </li>
<li>OGDI (OGDI) </li>
<li>ESRI Labelled BIL (EHdr) </li>
<li>PCI .aux Labelled Raw Raster (PAux) </li>
<li>HDF4 Hierachal Data Format Release 4 </li>
<li>HDF5 Hierachal Data Format Release 5 </li>
<li>GSAG Golden Software ASCII Grid </li>
<li>GSBG Golden Software Binary Grid </li>
</ul>
<p>
This function should generally be called once at the beginning of the application. 
</p>
End Rem
Function GDALAllRegister()
	bmx_gdal_GDALAllRegister()
End Function

Rem
bbdoc: 
End Rem
Type GDALMajorObject

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method GetDescription:String()
		Return bmx_gdal_GDALMajorObject_GetDescription(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMetadataItem:String(name:String, domain:String = "")
		Return bmx_gdal_GDALMajorObject_GetMetadataItem(objectPtr, name, domain)
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type GDALDataset Extends GDALMajorObject

	Function _create:GDALDataset(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:GDALDataset = New GDALDataset
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetDriver:GDALDriver()
		Return GDALDriver._create(bmx_gdal_GDALDataset_GetDriver(objectPtr))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetRasterXSize:Int()
		Return bmx_gdal_GDALDataset_GetRasterXSize(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetRasterYSize:Int()
		Return bmx_gdal_GDALDataset_GetRasterYSize(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetRasterCount:Int()
		Return bmx_gdal_GDALDataset_GetRasterCount(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetGeoTransform:Int(padfTransform:Double[])
		Return bmx_gdal_GDALDataset_GetGeoTransform(objectPtr, padfTransform)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetProjectionRef:String()
		Return bmx_gdal_GDALDataset_GetProjectionRef(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetRasterBand:GDALRasterBand(index:Int)
		Return GDALRasterBand._create(bmx_gdal_GDALDataset_GetRasterBand(objectPtr, index))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method FlushCache()
	End Method

	Rem
	bbdoc: Closes the dataset.
	End Rem
	Method Close()
		bmx_gdal_GDALDataset_Close(objectPtr)
	End Method
	
End Type



Rem
bbdoc: 
End Rem
Type GDALDriver Extends GDALMajorObject

	Function _create:GDALDriver(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:GDALDriver = New GDALDriver
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	


End Type

Rem
bbdoc: 
End Rem
Type GDALRasterBand Extends GDALMajorObject

	Function _create:GDALRasterBand(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:GDALRasterBand = New GDALRasterBand
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetXSize:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetYSize:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetBand:Int()
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetBlockSize(pnXSize:Int Var, pnYSize:Int Var)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetColorInterpretation:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMaskFlags:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMaximum:Double(pbSuccess:Int Var)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMinimum:Double(pbSuccess:Int Var)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetNoDataValue:Double(pbSuccess:Int Var)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetOffset:Double(pbSuccess:Int Var)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GenerateContour:Int(contourInterval:Double, contourBase:Double, fixedLevels:Double[], useNoData:Int, noDataValue:Double, ..
			layer:OGRLayer, idField:Int, elevField:Int)
		Return bmx_gdal_GDALRasterBand_GenerateContour(objectPtr, contourInterval, contourBase, fixedLevels, useNoData, noDataValue, ..
			layer.objectPtr, idField, elevField)
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Function OGRRegisterAll()
	bmx_gdal_OGRRegisterAll()
End Function

Rem
bbdoc: 
End Rem
Function OGRCleanupAll()
	bmx_gdal_OGRCleanupAll()
End Function

Rem
bbdoc: 
End Rem
Type OGRSFDriverRegistrar

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Function GetDriverCount:Int()
		Return bmx_gdal_OGRSFDriverRegistrar_GetDriverCount()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetDriver:OGRSFDriver(index:Int)
		Return OGRSFDriver._create(bmx_gdal_OGRSFDriverRegistrar_GetDriver(index))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetDriverByName:OGRSFDriver(name:String)
		Return OGRSFDriver._create(bmx_gdal_OGRSFDriverRegistrar_GetDriverByName(name))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetOpenDSCount:Int()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetOpenDS:OGRDataSource(ids:Int)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function AutoLoadDrivers()
	End Function

End Type


Rem
bbdoc: 
End Rem
Type OGRSFDriver

	Field objectPtr:Byte Ptr
	
	Function _create:OGRSFDriver(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:OGRSFDriver = New OGRSFDriver
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Open:OGRDataSource(name:String, update:Int = False)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TestCapability:Int(capability:String)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CreateDataSource:OGRDataSource(name:String, options:String[] = Null)
		If options Then
			Return OGRDataSource._create(bmx_gdal_OGRSFDriver_CreateDataSource(objectPtr, name, options))
		Else
			Return OGRDataSource._create(bmx_gdal_OGRSFDriver_CreateDataSource(objectPtr, name, Null))
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DeleteDataSource:Int(datasource:String)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetName:String()
	End Method
	
End Type

Rem
bbdoc: Destroys a datasource.
End Rem
Function DestroyDataSource(source:OGRDataSource)
' TODO
End Function

Rem
bbdoc: A data source.
about: A data source potentially consists of many layers (OGRLayer). A data source normally consists of one, or a related set
of files, though the name doesn't have to be a real item in the file system.
<p>
When an OGRDataSource is destroyed, all it's associated OGRLayers objects are also destroyed.
</p>
End Rem
Type OGRDataSource

	Field objectPtr:Byte Ptr

	Function _create:OGRDataSource(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:OGRDataSource = New OGRDataSource
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: 
	End Rem
	Method GetName:String()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLayerCount:Int()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLayer:OGRLayer(index:Int)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLayerByName:OGRLayer(name:String)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DeleteLayer:Int(index:Int)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TestCapability:Int(name:String)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method CreateLayer:OGRLayer(name:String, spatialRef:	OGRSpatialReference = Null, gType:Int = wkbUnknown, options:String[] = Null)
		If spatialRef Then
			If options Then
				Return OGRLayer._create(bmx_gdal_OGRDataSource_CreateLayer(objectPtr, name, spatialRef.objectPtr, gType, options))
			Else
				Return OGRLayer._create(bmx_gdal_OGRDataSource_CreateLayer(objectPtr, name, spatialRef.objectPtr, gType, Null))
			End If
		Else
			If options Then
				Return OGRLayer._create(bmx_gdal_OGRDataSource_CreateLayer(objectPtr, name, Null, gType, options))
			Else
				Return OGRLayer._create(bmx_gdal_OGRDataSource_CreateLayer(objectPtr, name, Null, gType, Null))
			End If
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_gdal_OGRDataSource_free(objectPtr)
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type


Rem
bbdoc: 
End Rem
Type OGRLayer

	Field objectPtr:Byte Ptr

	Function _create:OGRLayer(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:OGRLayer = New OGRLayer
			this.objectPtr = objectPtr
			Return this
		End If
	End Function
	
	Method CreateField:Int(fld:OGRFieldDefn, approxOK:Int = True)
		Return bmx_gdal_OGRLayer_CreateField(objectPtr, fld.objectPtr, approxOK)
	End Method

End Type

Rem
bbdoc: An OpenGIS Spatial Reference System.
about: Contains methods for converting between this object organization and well known text (WKT) format. This object is reference
counted as one instance of the object is normally shared between many OGRGeometry objects.
<p>
Normally application code can fetch needed parameter values for this SRS using GetAttrValue(), but in special cases the underlying
parse tree (or OGR_SRSNode objects) can be accessed more directly.
</p>
End Rem
Type OGRSpatialReference

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method Create:OGRSpatialReference(ref:String)
		objectPtr = bmx_gdal_OGRSpatialReference_create(ref)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			bmx_gdal_OGRSpatialReference_free(objectPtr)
			objectPtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type OGRFieldDefn

	Field objectPtr:Byte Ptr
	
	Method Create:OGRFieldDefn(name:String, fieldType:Int)
		objectPtr = bmx_gdal_OGRFieldDefn_create(name, fieldType)
		Return Self
	End Method
	
	Method SetWidth(width:Int)
		bmx_gdal_OGRFieldDefn_SetWidth(objectPtr, width)
	End Method
	
	Method SetPrecision(precision:Int)
		bmx_gdal_OGRFieldDefn_SetPrecision(objectPtr, precision)
	End Method
	
	Method Free()
		If objectPtr Then
			bmx_gdal_OGRFieldDefn_free(objectPtr)
			ObjectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type


