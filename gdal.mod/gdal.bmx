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
ModuleInfo "CC_OPTS: -DHAVE_PROTOTYPES -DHAVE_UNSIGNED_CHAR -DHAVE_UNSIGNED_SHORT"
ModuleInfo "CC_OPTS: -DFRMT_gxf -DFRMT_gtiff -DFRMT_hfa -DFRMT_aigrid -DFRMT_aaigrid -DFRMT_ceos -DFRMT_ceos2"
ModuleInfo "CC_OPTS: -DFRMT_iso8211 -DFRMT_xpm -DFRMT_sdts -DFRMT_raw -DFRMT_dted -DFRMT_mem -DFRMT_jdem -DFRMT_envisat"
ModuleInfo "CC_OPTS: -DFRMT_elas -DFRMT_fit -DFRMT_vrt -DFRMT_usgsdem -DFRMT_l1b -DFRMT_nitf -DFRMT_bmp -DFRMT_pcidsk"
ModuleInfo "CC_OPTS: -DFRMT_airsar -DFRMT_rs2 -DFRMT_ilwis -DFRMT_rmf -DFRMT_leveller -DFRMT_sgi -DFRMT_srtmhgt -DFRMT_idrisi"
ModuleInfo "CC_OPTS: -DFRMT_gsg -DFRMT_ingr -DFRMT_ers -DFRMT_jaxapalsar -DFRMT_dimap -DFRMT_gff -DFRMT_cosar -DFRMT_pds"
ModuleInfo "CC_OPTS: -DFRMT_adrg -DFRMT_coasp -DFRMT_tsx -DFRMT_terragen -DFRMT_blx -DFRMT_msgn -DFRMT_wcs -DFRMT_wms"
ModuleInfo "CC_OPTS: -DFRMT_bsb -DFRMT_gif -DFRMT_png -DFRMT_pcraster -DFRMT_jpeg"

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
bbdoc: Opens a raster file as a GDALDataset.
about: This function works the same as GDALOpen(), but allows the sharing of GDALDataset handles for a dataset with other callers
to GDALOpenShared().
<p>
In particular, GDALOpenShared() will first consult it's list of currently open and shared GDALDataset's, and if the
GetDescription() name for one exactly matches the filename passed to GDALOpenShared() it will be referenced and returned.
</p>
End Rem
Function GDALOpenShared:GDALDataset(filename:String, access:Int)
	Return GDALDataset._create(bmx_gdal_GDALOpenShared(filename, access))
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
bbdoc: Object with metadata.
End Rem
Type GDALMajorObject

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Fetches object description.
	about: The semantics of the returned description are specific to the derived type. For GDALDatasets it is the dataset name.
	For GDALRasterBands it is actually a description (if supported) or "".
	End Rem
	Method GetDescription:String()
		Return bmx_gdal_GDALMajorObject_GetDescription(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches single metadata item.
	End Rem
	Method GetMetadataItem:String(name:String, domain:String = "")
		Return bmx_gdal_GDALMajorObject_GetMetadataItem(objectPtr, name, domain)
	End Method
	
	Rem
	bbdoc: Fetches metadata as a TMap of name/value pairs.
	about: Note that relatively few formats return any metadata at this time.
	End Rem
	Method GetMetadata:String[](domain:String = "")
		Return bmx_gdal_GDALMajorObject_GetMetadata(objectPtr, domain)
	End Method
	
	Rem
	bbdoc: Sets single metadata item.
	End Rem
	Method SetMetadataItem:Int(name:String, value:String, domain:String = "")
		Return bmx_gdal_GDALMajorObject_SetMetadataItem(objectPtr, name, value, domain)
	End Method
	
	Rem
	bbdoc: Sets metadata.
	End Rem
	Method SetMetadata:Int(metadata:String[], domain:String = "")
		Return bmx_gdal_GDALMajorObject_SetMetadata(objectPtr, metadata, domain)
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
	Method GetGeoTransform:Int(transform:Double[])
		Return bmx_gdal_GDALDataset_GetGeoTransform(objectPtr, transform)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetProjectionRef:String()
		Return bmx_gdal_GDALDataset_GetProjectionRef(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the projection reference string for this dataset.
	about: The string should be in OGC WKT or PROJ.4 format. An error may occur because of incorrectly specified projection strings,
	because the dataset is not writable, or because the dataset does not support the indicated projection. Many formats do not
	support writing projections.
	End Rem
	Method SetProjection:Int(projection:String)
		Return bmx_gdal_GDALDataset_SetProjection(objectPtr, projection)
	End Method

	Rem
	bbdoc: 
	End Rem	
	Method SetGeoTransform:Int(transform:Double[])
		Return bmx_gdal_GDALDataset_SetGeoTransform(objectPtr, transform)
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
	' TODO
	End Method
	
	Rem
	bbdoc: Get number of GCPs.
	End Rem
	Method GetGCPCount:Int()
		Return bmx_gdal_GDALDataset_GetGCPCount(objectPtr)
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
	
	Rem
	bbdoc: 
	End Rem
	Method GetShortName:String()
		Return bmx_gdal_GDALDriver_GetShortName(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLongName:String()
		Return bmx_gdal_GDALDriver_GetLongName(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method CreateCopy:GDALDataset(filename:String, sourceDataset:GDALDataset, _strict:Int, options:String[] = Null)
		Return GDALDataset._create(bmx_gdal_GDALDriver_CreateCopy(objectPtr, filename, sourceDataset.objectPtr, _strict, options))
	End Method

End Type

Rem
bbdoc: A single raster band (or channel).
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
	bbdoc: Fetches XSize of raster.
	End Rem
	Method GetXSize:Int()
		Return bmx_gdal_GDALRasterBand_GetXSize(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches YSize of raster.
	End Rem
	Method GetYSize:Int()
		Return bmx_gdal_GDALRasterBand_GetYSize(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches the band number.
	about: Returns the band that this GDALRasterBand objects represents within it's dataset. This method may return a value of 0 to
	indicate GDALRasterBand objects without an apparently relationship to a dataset, such as GDALRasterBands serving as overviews.
	End Rem
	Method GetBand:Int()
		Return bmx_gdal_GDALRasterBand_GetBand(objectPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetBlockSize(pnXSize:Int Var, pnYSize:Int Var)
	' TODO
	End Method
	
	Rem
	bbdoc: How should this band be interpreted as color?
	about: GCI_Undefined is returned when the format doesn't know anything about the color interpretation, otherwise one of
	GCI_GrayIndex, GCI_PaletteIndex, GCI_RedBand, GCI_GreenBand, GCI_BlueBand, GCI_AlphaBand, GCI_HueBand, GCI_SaturationBand, 
	GCI_LightnessBand, GCI_CyanBand, GCI_MagentaBand, GCI_YellowBand, GCI_BlackBand, GCI_YCbCr_YBand, GCI_YCbCr_CbBand or 
	GCI_YCbCr_CrBand.
	End Rem
	Method GetColorInterpretation:Int()
		Return bmx_gdal_GDALRasterBand_GetColorInterpretation(objectPtr)
	End Method

	Rem
	bbdoc: Returns the status flags of the mask band associated with the band.
	about: It returns an bitwise OR-ed set of status flags with the following available definitions that may be extended in the future:
	<ul>
	<li>GMF_ALL_VALID ($01): There are no invalid pixels, all mask values will be 255. When used this will normally be the only flag set.</li>
	<li>GMF_PER_DATASET ($02): The mask band is shared between all bands on the dataset.</li>
	<li>GMF_ALPHA	($04): The mask band is actually an alpha band and may have values other than 0 and 255.</li>
	<li>GMF_NODATA ($08): Indicates the mask is actually being generated from nodata values. (mutually exclusive of GMF_ALPHA)</li>
	</ul>
	The GDALRasterBand class includes a default implementation of GetMaskBand() that returns one of four default implementations :
	<ul>
	<li>If a corresponding .msk file exists it will be used for the mask band.</li>
	<li>If the dataset has a NODATA_VALUES metadata item, an instance of the new GDALNoDataValuesMaskBand class will be
	returned. GetMaskFlags() will return GMF_NODATA | GMF_PER_DATASET.</li>
	<li>If the band has a nodata value set, an instance of the new GDALNodataMaskRasterBand class will be returned.
	GetMaskFlags() will return GMF_NODATA.</li>
	<li>If there is no nodata value, but the dataset has an alpha band that seems to apply to this band (specific rules
	yet to be determined) and that is of type GDT_Byte then that alpha band will be returned, and the flags GMF_PER_DATASET
	and GMF_ALPHA will be returned in the flags.</li>
	<li>If neither of the above apply, an instance of the new GDALAllValidRasterBand class will be returned that has 255
	values for all pixels. The null flags will return GMF_ALL_VALID.</li>
	</ul>
	End Rem
	Method GetMaskFlags:Int()
		Return bmx_gdal_GDALRasterBand_GetMaskFlags(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches the maximum value for this band.
	about: For file formats that don't know this intrinsically, the maximum supported value for the data type will generally be returned.
	End Rem
	Method GetMaximum:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetMaximum(objectPtr, Varptr success)
	End Method
	
	Rem
	bbdoc: Fetches the minimum value for this band.
	about: For file formats that don't know this intrinsically, the minimum supported value for the data type will generally be returned.
	End Rem
	Method GetMinimum:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetMinimum(objectPtr, Varptr success)
	End Method
	
	Rem
	bbdoc: Fetches the no data value for this band.
	about: If there is no out of data value, an out of range value will generally be returned. The no data value for a band
	is generally a special marker value used to mark pixels that are not valid data. Such pixels should generally not be
	displayed, nor contribute to analysis operations.
	End Rem
	Method GetNoDataValue:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetNoDataValue(objectPtr, Varptr success)
	End Method
	
	Rem
	bbdoc: Fetches the raster value offset.
	about: This value (in combination with the GetScale() value) is used to transform raw pixel values into the units returned by
	GetUnits(). For example this might be used to store elevations in GUInt16 bands with a precision of 0.1, and starting from -100.
	<p>
	Units value = (raw value * scale) + offset
	</p>
	<p>
	For file formats that don't know this intrinsically a value of zero is returned.
	</p>
	End Rem
	Method GetOffset:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetOffset(objectPtr, Varptr success)
	End Method
	
	Rem
	bbdoc: Fetch the pixel data type for this band.
	End Rem
	Method GetRasterDataType:Int()
		Return bmx_gdal_GDALRasterBand_GetRasterDataType(objectPtr)
	End Method

	Rem
	bbdoc: Creates vector contours from raster DEM.
	about: This algorithm will generate contours vectors for the input raster band on the requested set of contour levels.
	The vector contours are written to the passed in OGR vector layer. Also, a NODATA value may be specified to identify pixels
	that should not be considered in contour line generation.
	<p>ALGORITHM RULES</p>
	<p>
	For contouring purposes raster pixel values are assumed to represent a point value at the center of the corresponding pixel
	region. For the purpose of contour generation we virtually connect each pixel center to the values to the left, right, top and
	bottom. We assume that the pixel value is linearly interpolated between the pixel centers along each line, and determine where
	(if any) contour lines will appear onlong these line segements. Then the contour crossings are connected.<p>
	</p>
	<p>
	This means that contour lines nodes won't actually be on pixel edges, but rather along vertical and horizontal lines connecting the
	pixel centers.
	<pre>
General Case:

      5 |                  | 3
     -- + ---------------- + -- 
        |                  |
        |                  |
        |                  |
        |                  |
     10 +                  |
        |\                 |
        | \                |
     -- + -+-------------- + -- 
     12 |  10              | 1


Saddle Point:

      5 |                  | 12
     -- + -------------+-- + -- 
        |               \  |
        |                 \|
        |                  + 
        |                  |
        +                  |
        |\                 |
        | \                |
     -- + -+-------------- + -- 
     12 |                  | 1

or:

      5 |                  | 12
     -- + -------------+-- + -- 
        |          __/     |
        |      ___/        |
        |  ___/          __+ 
        | /           __/  |
        +'         __/     |
        |       __/        |
        |   ,__/           |
     -- + -+-------------- + -- 
     12 |                  | 1
	</pre>
	</p>
	<p>Nodata:</p>
	<p>
	In the "nodata" case we treat the whole nodata pixel as a no-mans land. We extend the corner pixels near the nodata out to half
	way and then construct extra lines from those points to the center which is assigned an averaged value from the two nearby
	points (in this case (12+3+5)/3).
	<pre>
      5 |                  | 3
     -- + ---------------- + -- 
        |                  |
        |                  |
        |      6.7         |
        |        +---------+ 3
     10 +___     |          
        |   \____+ 10       
        |        |          
     -- + -------+        +    
     12 |       12           (nodata)

	</pre>
	</p>
	End Rem
	Method GenerateContour:Int(contourInterval:Double, contourBase:Double, fixedLevels:Double[], useNoData:Int, noDataValue:Double, ..
			layer:OGRLayer, idField:Int, elevField:Int)
		Return bmx_gdal_GDALRasterBand_GenerateContour(objectPtr, contourInterval, contourBase, fixedLevels, useNoData, noDataValue, ..
			layer.objectPtr, idField, elevField)
	End Method
	
End Type

Rem
bbdoc: Type for managing the registration of file format drivers.
End Rem
Type GDALDriverManager Extends GDALMajorObject

	Rem
	bbdoc: 
	End Rem
	Function GetDriverCount:Int()
		Return bmx_gdal_GDALDriverManager_GetDriverCount()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetDriver:GDALDriver(index:Int)
		Return GDALDriver._create(bmx_gdal_GDALDriverManager_GetDriver(index))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function GetDriverByName:GDALDriver(name:String)
		Return GDALDriver._create(bmx_gdal_GDALDriverManager_GetDriverByName(name))
	End Function

End Type


Rem
bbdoc: Registers all drivers. 
End Rem
Function OGRRegisterAll()
	bmx_gdal_OGRRegisterAll()
End Function

Rem
bbdoc: Cleans up all OGR related resources.
about: This function will destroy the OGRSFDriverRegistrar along with all registered drivers, and then cleanup long lived
OSR (OGRSpatialReference) and CPL resources. This may be called in an application when OGR services are no longer needed.
It is not normally required, but by freeing all dynamically allocated memory it can make memory leak testing easier.
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
	bbdoc: Fetches the number of registered drivers.
	End Rem
	Function GetDriverCount:Int()
		Return bmx_gdal_OGRSFDriverRegistrar_GetDriverCount()
	End Function
	
	Rem
	bbdoc: Fetches the indicated driver.
	End Rem
	Function GetDriver:OGRSFDriver(index:Int)
		Return OGRSFDriver._create(bmx_gdal_OGRSFDriverRegistrar_GetDriver(index))
	End Function
	
	Rem
	bbdoc: Fetches the indicated driver.
	End Rem
	Function GetDriverByName:OGRSFDriver(name:String)
		Return OGRSFDriver._create(bmx_gdal_OGRSFDriverRegistrar_GetDriverByName(name))
	End Function
	
	Rem
	bbdoc: Returns the number of opened datasources.
	End Rem
	Function GetOpenDSCount:Int()
		Return bmx_gdal_OGRSFDriverRegistrar_GetOpenDSCount()
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
	bbdoc: Returns the name of the data source.
	about: This string should be sufficient to open the data source if passed to the same OGRSFDriver that this data source
	was opened with, but it need not be exactly the same string that was used to open the data source. Normally this is a filename.
	End Rem
	Method GetName:String()
		Return bmx_gdal_OGRDataSource_GetName(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets the number of layers in this data source.
	End Rem
	Method GetLayerCount:Int()
		Return bmx_gdal_OGRDataSource_GetLayerCount(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches a layer by index.
	about: The returned layer remains owned by the OGRDataSource and should not be deleted by the application.
	End Rem
	Method GetLayer:OGRLayer(index:Int)
		Return OGRLayer._create(bmx_gdal_OGRDataSource_GetLayer(objectPtr, index))
	End Method
	
	Rem
	bbdoc: Fetches a layer by name.
	about: The returned layer remains owned by the OGRDataSource and should not be deleted by the application.
	End Rem
	Method GetLayerByName:OGRLayer(name:String)
		Return OGRLayer._create(bmx_gdal_OGRDataSource_GetLayerByName(objectPtr, name))
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
bbdoc: A layer of simple features, with access methods.
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
	
	Rem
	bbdoc: Creates a new field on a layer.
	about: You must use this to create new fields on a real layer. Internally the OGRFeatureDefn for the layer will be updated to reflect
	the new field. Applications should never modify the OGRFeatureDefn used by a layer directly.
	End Rem
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

Rem
bbdoc: 
End Rem
Type VRTDataset Extends GDALDataset

	Rem
	bbdoc: 
	End Rem
	Method Create:VRTDataset(width:Int, height:Int)
		objectPtr = bmx_gdal_VRTDataset_create(width, height)
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetRasterBand:VRTSourcedRasterBand(index:Int)
		Return VRTSourcedRasterBand._create(bmx_gdal_GDALDataset_GetRasterBand(objectPtr, index))
	End Method

	Method AddBand:Int(dataType:Int, options:String[] = Null)
		Return bmx_gdal_VRTDataset_AddBand(objectPtr, dataType, options)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type VRTRasterBand Extends GDALRasterBand

	Rem
	bbdoc: 
	End Rem
	Method CopyCommonInfoFrom:Int(source:GDALRasterBand)
		Return bmx_gdal_VRTRasterBand_CopyCommonInfoFrom(objectPtr, source.objectPtr)
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type VRTSourcedRasterBand Extends VRTRasterBand

	Function _create:VRTSourcedRasterBand(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:VRTSourcedRasterBand = New VRTSourcedRasterBand
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Method AddSimpleSource:Int( srcBand:GDALRasterBand, srcXOff:Int = -1, srcYOff:Int =-1, ..
			srcXSize:Int =-1, srcYSize:Int =-1, dstXOff:Int =-1, dstYOff:Int =-1, dstXSize:Int =-1, dstYSize:Int =-1, ..
			resampling:String = "near", noDataValue:Double = -1234.56)
	
		Return bmx_gdal_VRTSourcedRasterBand_AddSimpleSource(objectPtr, srcBand.objectPtr, srcXOff, srcYOff, srcXSize, srcYSize, ..
			dstXOff, dstYOff, dstXSize, dstYSize, resampling, noDataValue)
	
	End Method

End Type

Type VRTDriver Extends GDALDriver
End Type



