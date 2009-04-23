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
about: Geospatial Data Abstraction Library.
<p>
A translator library for raster geospatial data formats.
</p>
End Rem
Module BaH.GDAL

ModuleInfo "Version: 1.00"
ModuleInfo "License: Wrapper - MIT"
ModuleInfo "License: GDAL - Copyright (c) 2000, Frank Warmerdam"
ModuleInfo "License: PROJ.4 - Copyright (c) 2000, Frank Warmerdam"
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
ModuleInfo "CC_OPTS: -DFRMT_bsb -DFRMT_gif -DFRMT_png -DFRMT_pcraster -DFRMT_jpeg -DFRMT_rik -DFRMT_grib -DFRMT_jpeg2000"
ModuleInfo "CC_OPTS: -DHAVE_LIBPROJ -DPROJ_STATIC -DHAVE_JASPER"

ModuleInfo "CC_OPTS: -DSHAPE_ENABLED -DTAB_ENABLED -DNTF_ENABLED -DSDTS_ENABLED -DTIGER_ENABLED -DS57_ENABLED -DDGN_ENABLED"
ModuleInfo "CC_OPTS: -DVRT_ENABLED -DREC_ENABLED -DMEM_ENABLED -DBNA_ENABLED -DCSV_ENABLED -DGML_ENABLED"
ModuleInfo "CC_OPTS: -DGPX_ENABLED -DKML_ENABLED -DGEOJSON_ENABLED -DGMT_ENABLED"
ModuleInfo "CC_OPTS: -DXPLANE_ENABLED -DAVCBIN_ENABLED -DGEOCONCEPT_ENABLED "

ModuleInfo "CC_OPTS: -fexceptions"

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
	bbdoc: Sets object description.
	about: The semantics of the description are specific to the derived type. For GDALDatasets it
	is the dataset name. For GDALRasterBands it is actually a description (if supported) or "".
	<p>
	Normally application code should not set the "description" for GDALDatasets. It is handled internally.
	</p>
	End Rem
	Method SetDescription(description:String)
		bmx_gdal_GDALMajorObject_SetDescription(objectPtr, description)
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
bbdoc: A set of associated raster bands, usually from one file.
about: A dataset encapsulating one or more raster bands.
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
	bbdoc: Fetches raster width in pixels.
	End Rem
	Method GetRasterXSize:Int()
		Return bmx_gdal_GDALDataset_GetRasterXSize(objectPtr)
	End Method

	Rem
	bbdoc: Fetches raster height in pixels.
	End Rem
	Method GetRasterYSize:Int()
		Return bmx_gdal_GDALDataset_GetRasterYSize(objectPtr)
	End Method

	Rem
	bbdoc: Fetches the number of raster bands on this dataset.
	End Rem
	Method GetRasterCount:Int()
		Return bmx_gdal_GDALDataset_GetRasterCount(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches a band object for a dataset.
	End Rem
	Method GetRasterBand:GDALRasterBand(index:Int)
		Return GDALRasterBand._create(bmx_gdal_GDALDataset_GetRasterBand(objectPtr, index))
	End Method
	
	Rem
	bbdoc: Flushes all write cached data to disk.
	about: Any raster (or other GDAL) data written via GDAL calls, but buffered internally will be
	written to disk.
	<p>
	Using this method does not prevent use from calling GDALClose() to properly close a dataset
	and ensure that important data not addressed by FlushCache() is written in the file.
	</p>
	End Rem
	Method FlushCache()
		bmx_gdal_GDALDataset_FlushCache(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches the projection definition string for this dataset.
	about: The returned string defines the projection coordinate system of the image in OpenGIS WKT
	format. It should be suitable for use with the OGRSpatialReference class.
	<p>
	When a projection definition is not available an empty string is returned.
	</p>
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
	bbdoc: Fetches the affine transformation coefficients.
	about: Fetches the coefficients for transforming between pixel/line (P,L) raster space, and projection
	coordinates (Xp,Yp) space.
	<pre>
   Xp = transform[0] + P * transform[1] + L * transform[2]
   Yp = transform[3] + P * transform[4] + L * transform[5]
	</pre>
	<p>
	In a north up image, padfTransform[1] is the pixel width, and padfTransform[5] is the pixel height.
	The upper left corner of the upper left pixel is at position (padfTransform[0],padfTransform[3]).
	</p>
	<p>
	The default transform is (0,1,0,0,0,1) and should be returned even when a CE_Failure error is returned,
	such as for formats that don't support transformation to projection coordinates.
	</p>
	<p>
	NOTE: GetGeoTransform() isn't expressive enough to handle the variety of OGC Grid Coverages pixel/line
	to projection transformation schemes. Eventually this method will be depreciated in favour of a more
	general scheme.
	</p>
	End Rem
	Method GetGeoTransform:Int(transform:Double[])
		Return bmx_gdal_GDALDataset_GetGeoTransform(objectPtr, transform)
	End Method

	Rem
	bbdoc: Set the affine transformation coefficients.
	aboout: See GetGeoTransform() for details on the meaning of the @transform coefficients.
	End Rem
	Method SetGeoTransform:Int(transform:Double[])
		Return bmx_gdal_GDALDataset_SetGeoTransform(objectPtr, transform)
	End Method

	Rem
	bbdoc: Adds a band to a dataset.
	about: This method will add a new band to the dataset if the underlying format supports this action.
	Most formats do not.
	<p>
	Note that the new GDALRasterBand is not returned. It may be fetched after successful completion of
	the method by calling GetRasterBand(GetRasterCount()-1) as the newest band will always be the last band.
	</p>
	End Rem
	Method AddBand:Int(dataType:Int, options:String[] = Null)
		Return bmx_gdal_GDALDataset_AddBand(objectPtr, dataType, options)
	End Method

	Rem
	bbdoc: Fetches the driver to which this dataset relates.
	End Rem
	Method GetDriver:GDALDriver()
		Return GDALDriver._create(bmx_gdal_GDALDataset_GetDriver(objectPtr))
	End Method

	Rem
	bbdoc: Fetches files forming dataset.
	about: Returns a list of files believed to be part of this dataset. If it returns an empty list
	of files it means there is believed to be no local file system files associated with the dataset
	(for instance a virtual dataset).
	<p>
	The returned filenames will normally be relative or absolute paths depending on the path used to
	originally open the dataset.
	</p>
	End Rem
	Method GetFileList:String[]()
		Return bmx_gdal_GDALDataset_GetFileList(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets number of GCPs.
	End Rem
	Method GetGCPCount:Int()
		Return bmx_gdal_GDALDataset_GetGCPCount(objectPtr)
	End Method
	
	Rem
	bbdoc: Gets output projection for GCPs.
	End Rem
	Method GetGCPProjection:String()
		Return bmx_gdal_GDALDataset_GetGCPProjection(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches the list of GCPs.
	about: It should not be modified, and may change on the next GDAL call.
	End Rem
	Method GetGCPs:GDAL_GCP[]()
		Local gcps:GDAL_GCP[] = New GDAL_GCP[GetGCPCount()]
		bmx_gdal_GDALDataset_GetGCPs(objectPtr, gcps)
		Return gcps
	End Method
	
	Function _setGCP(gcps:GDAL_GCP[], index:Int, gcp:Byte Ptr)
		gcps[index] = GDAL_GCP._create(gcp)
	End Function
		
	Method SetGCPs(gcps:GDAL_GCP[], projection:String)
	' TODO
	End Method
	
	Method AdviseRead:Int(xOff:Int, yOff:Int, xSize:Int, ySize:Int, bufXSize:Int, bufYSize:Int, ..
			eDT:Int, bandMap:Int[], options:String[])
	' TODO
	End Method
	
	Rem
	bbdoc: Adds a mask band to the dataset.
	about: The default implementation is based on similar rules to the .ovr handling implemented using
	the GDALDefaultOverviews object. A TIFF file with the extension .msk will be created with the same
	basename as the original file, and it will have one band. The mask images will be deflate compressed
	tiled images with the same block size as the original image if possible.
	End Rem
	Method CreateMaskBand:Int(flags:Int)
		Return bmx_gdal_GDALDataset_CreateMaskBand(objectPtr, flags)
	End Method
	
	Method RasterIO:Int(eRWFlag:Int, xOff:Int, yOff:Int, xSize:Int, ySize:Int, data:Byte Ptr, ..
			bufXSize:Int, bufYSize:Int, bufType:Int, bandMap:Int[], pixelSpace:Int, lineSpace:Int, bandSpace:Int)
	' TODO
	End Method

	Rem
	bbdoc: Returns shared flag.
	returns: TRUE if the GDALDataset is available for sharing, or FALSE if not. 
	End Rem
	Method GetShared:Int()
		Return bmx_gdal_GDALDataset_GetShared(objectPtr)
	End Method
	
	Rem
	bbdoc: Marks this dataset as available for sharing. 
	End Rem
	Method MarkAsShared()
		bmx_gdal_GDALDataset_MarkAsShared(objectPtr)
	End Method

	Rem
	bbdoc: Closes the dataset.
	End Rem
	Method Close()
		bmx_gdal_GDALDataset_Close(objectPtr)
	End Method
	
End Type

Extern
	Function bmx_gdal_GDALDataset_GetGCPs(handle:Byte Ptr, gcps:GDAL_GCP[])
End Extern

Rem
bbdoc: Format specific driver.
about: An instance of this type is created for each supported format, and manages information about the format.
<p>
This roughly corresponds to a file format, though some drivers may be gateways to many formats through a
secondary multi-library. 
</p>
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
	bbdoc: Creates a new dataset with this driver.
	about: What argument values are legal for particular drivers is driver specific, and there is no way
	to query in advance to establish legal values.
	<p>
	That method will try to validate the creation option list passed to the driver with the
	GDALValidateCreationOptions() method. This check can be disabled by defining the configuration option
	GDAL_VALIDATE_CREATION_OPTIONS=NO.
	</p>
	<p>
	After you have finished working with the returned dataset, it is required to close it with Close().
	This does not only close the file handle, but also ensures that all the data and metadata has been
	written to the dataset (FlushCache() is not sufficient for that purpose).
	</p>
	End Rem
	Method CreateDataset:GDALDataset(filename:String, xSize:Int, ySize:Int, bands:Int, dataType:Int, paramList:String[] = Null)
		Return GDALDataset._create(bmx_gdal_GDALDriver_CreateDataset(objectPtr, filename, xSize, ySize, bands, dataType, paramList))
	End Method
	
	Rem
	bbdoc: Deletes the named dataset.
	about: The driver will attempt to delete the named dataset in a driver specific fashion. Full featured
	drivers will delete all associated files, database objects, or whatever is appropriate. The default
	behaviour when no driver specific behaviour is provided is to attempt to delete the passed name as a
	single file.
	<p>
	It is unwise to have open dataset handles on this dataset when it is deleted.
	</p>
	End Rem
	Method DeleteDataset:Int(filename:String)
		Return bmx_gdal_GDALDriver_DeleteDataset(objectPtr, filename)
	End Method
	
	Rem
	bbdoc: Renames a dataset.
	about: This may including moving the dataset to a new directory or even a new filesystem.
	<p>
	It is unwise to have open dataset handles on this dataset when it is being renamed.
	</p>
	End Rem
	Method RenameDataset:Int(newName:String, oldName:String)
		Return bmx_gdal_GDALDriver_RenameDataset(objectPtr, newName, oldName)
	End Method
	
	Rem
	bbdoc: Copies the files of a dataset.
	about: Copy all the files associated with a dataset.
	End Rem
	Method CopyFiles:Int(newName:String, oldName:String)
		Return bmx_gdal_GDALDriver_CopyFiles(objectPtr, newName, oldName)
	End Method
	
	Rem
	bbdoc: Returns the short name of driver.
	about: This is the string that can be passed to GetDriverByName()
	<p>
	For the GeoTIFF driver, this is "GTiff"
	</p>
	End Rem
	Method GetShortName:String()
		Return bmx_gdal_GDALDriver_GetShortName(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the long name of a driver.
	about: For the GeoTIFF driver, this is "GeoTIFF"
	End Rem
	Method GetLongName:String()
		Return bmx_gdal_GDALDriver_GetLongName(objectPtr)
	End Method

	Rem
	bbdoc: Creates a copy of a dataset.
	about: This method will attempt to create a copy of a raster dataset with the indicated filename,
	and in this drivers format. Band number, size, type, projection, geotransform and so forth are all
	to be copied from the provided template dataset.
	<p>
	Note that many sequential write once formats (such as JPEG and PNG) don't implement the CreateDataset() method
	but do implement this CreateCopy() method. If the driver doesn't implement CreateCopy(), but does
	implement CreateDataset() then the default CreateCopy() mechanism built on calling CreateDataset() will be used.
	</p>
	<p>
	It is intended that CreateCopy() will often be used with a source dataset which is a virtual dataset
	allowing configuration of band types, and other information without actually duplicating raster data
	(see the VRT driver). This is what is done by the gdal_translate utility for example.
	</p>
	<p>
	That function will try to validate the creation option list passed to the driver with the
	GDALValidateCreationOptions() method. This check can be disabled by defining the configuration
	option GDAL_VALIDATE_CREATION_OPTIONS=NO.
	</p>
	<p>
	After you have finished working with the returned dataset, it is required to close it with Close().
	This does not only close the file handle, but also ensures that all the data and metadata has been
	written to the dataset (GDALFlushCache() is not sufficient for that purpose).
	</p>
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
	bbdoc: Fetches the "natural" block size of this band.
	about: GDAL contains a concept of the natural block size of rasters so that applications can organized
	data access efficiently for some file formats. The natural block size is the block size that is most
	efficient for accessing the format. For many formats this is simple a whole scanline in which case
	xSize is set to GetXSize(), and ySize is set to 1.
	<p>
	However, for tiled images this will typically be the tile size.
	</p>
	<p>
	Note that the X and Y block sizes don't have to divide the image size evenly, meaning that right
	and bottom edge blocks may be incomplete. See ReadBlock() for an example of code dealing with these
	issues.
	</p>
	End Rem
	Method GetBlockSize(xSize:Int Var, ySize:Int Var)
		bmx_gdal_GDALRasterBand_GetBlockSize(objectPtr, Varptr xSize, Varptr ySize)
	End Method
	
	Rem
	bbdoc: Fetches the owning dataset handle.
	about: Note that some GDALRasterBands are not considered to be a part of a dataset, such as
	overviews or other "freestanding" bands.
	End Rem
	Method GetDataset:GDALDataset()
		Return GDALDataset._create(bmx_gdal_GDALRasterBand_GetDataset(objectPtr))
	End Method
	
	Rem
	bbdoc: Fetches the pixel data type for this band.
	End Rem
	Method GetRasterDataType:Int()
		Return bmx_gdal_GDALRasterBand_GetRasterDataType(objectPtr)
	End Method
	
	Rem
	bbdoc: Finds out if we have update permission for this band.
	End Rem
	Method GetAccess:Int()
		bmx_gdal_GDALRasterBand_GetAccess(objectPtr)
	End Method
	
	Rem
	bbdoc: Flushes raster data cache.
	about: This call will recover memory used to cache data blocks for this raster band, and ensure
	that new requests are referred to the underlying driver.
	End Rem
	Method FlushCache:Int()
		Return bmx_gdal_GDALRasterBand_FlushCache(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches the list of category names for this raster.
	about: Raster values without associated names will have an empty string
	in the returned list. The first entry in the list is for raster values of zero, and so on.
	End Rem
	Method GetCategoryNames:String[]()
		Return bmx_gdal_GDALRasterBand_GetCategoryNames(objectPtr)
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
	bbdoc: Fetches the minimum value for this band.
	about: For file formats that don't know this intrinsically, the minimum supported value for the data type will generally be returned.
	End Rem
	Method GetMinimum:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetMinimum(objectPtr, Varptr success)
	End Method
	
	Rem
	bbdoc: Fetches the maximum value for this band.
	about: For file formats that don't know this intrinsically, the maximum supported value for the data type will generally be returned.
	End Rem
	Method GetMaximum:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetMaximum(objectPtr, Varptr success)
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
	bbdoc: Fetches the raster value scale.
	about: This value (in combination with the GetOffset() value) is used to transform raw pixel values
	into the units returned by GetUnits(). For example this might be used to store elevations in GUInt16
	bands with a precision of 0.1, and starting from -100.
	<p>
	Units value = (raw value * scale) + offset
	</p>
	<p>
	For file formats that don't know this intrinsically a value of one is returned.
	</p>
	End Rem
	Method GetScale:Double(success:Int Var)
		Return bmx_gdal_GDALRasterBand_GetScale(objectPtr, Varptr success)
	End Method
	
	Rem
	bbdoc: Returns raster unit type.
	about: Returns a name for the units of this raster's values. For instance, it might be "m" for an
	elevation model in meters, or "ft" for feet. If no units are available, a value of "" will be
	returned. The returned string should not be modified, nor freed by the calling application.
	End Rem
	Method GetUnitType:String()
		Return bmx_gdal_GDALRasterBand_GetUnitType(objectPtr)
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
	
	Method GetColorTable:GDALColorTable()
	' TODO
	End Method
	
	Rem
	bbdoc: Fills this band with a constant value.
	about: GDAL makes no guarantees about what values pixels in newly created files are set to, so this
	method can be used to clear a band to a specified "default" value. The fill value is passed in as a
	double but this will be converted to the underlying type before writing to the file. An optional second
	argument allows the imaginary component of a complex constant value to be specified.
	End Rem
	Method Fill:Int(realValue:Double, imaginaryValue:Double = 0)
		Return bmx_gdal_GDALRasterBand_Fill(objectPtr, realValue, imaginaryValue)
	End Method
	
	Rem
	bbdoc: Sets the category names for this band.
	about: See the GetCategoryNames() method for more on the interpretation of category names.
	End Rem
	Method SetCategoryNames:Int(names:String[])
		Return bmx_gdal_GDALRasterBand_SetCategoryNames(objectPtr, names)
	End Method
	
	Rem
	bbdoc: Sets the no data value for this band.
	about: To clear the nodata value, just set it with an "out of range" value. Complex band no data values
	must have an imagery component of zero.
	End Rem
	Method SetNoDataValue:Int(value:Double)
		Return bmx_gdal_GDALRasterBand_SetNoDataValue(objectPtr, value)
	End Method
	
	Method SetColorTable:Int(table:GDALColorTable)
	' TODO
	End Method
	
	Rem
	bbdoc: Sets color interpretation of a band.
	End Rem
	Method SetColorInterpretation:Int(interp:Int)
		Return bmx_gdal_GDALRasterBand_SetColorInterpretation(objectPtr, interp)
	End Method
	
	Rem
	bbdoc: Sets scaling offset.
	about: Very few formats implement this method. When not implemented it will issue a CPLE_NotSupported
	error and return CE_Failure.
	End Rem
	Method SetOffset:Int(offset:Double)
		Return bmx_gdal_GDALRasterBand_SetOffset(objectPtr, offset)
	End Method
	
	Rem
	bbdoc: Sets scaling ratio.
	about: Very few formats implement this method. When not implemented it will issue a CPLE_NotSupported
	error and return CE_Failure.
	End Rem
	Method SetScale:Int(scale:Double)
		Return bmx_gdal_GDALRasterBand_SetScale(objectPtr, scale)
	End Method
	
	Rem
	bbdoc: Sets unit type.
	about: Set the unit type for a raster band. Values should be one of "" (the default indicating it is
	unknown), "m" indicating meters, or "ft" indicating feet, though other nonstandard values are allowed.
	End Rem
	Method SetUnitType:Int(unitType:String)
		Return bmx_gdal_GDALRasterBand_SetUnitType(objectPtr, unitType)
	End Method
	
	Rem
	bbdoc: Fetches image statistics.
	about: Returns the minimum, maximum, mean and standard deviation of all pixel values in this band.
	If approximate statistics are sufficient, the bApproxOK flag can be set to true in which case overviews,
	or a subset of image tiles may be used in computing the statistics.
	<p>
	If @force is FALSE results will only be returned if it can be done quickly (ie. without scanning
	the data). If @force is FALSE and results cannot be returned efficiently, the method will return
	CE_Warning but no warning will have been issued. This is a non-standard use of the CE_Warning return
	value to indicate "nothing done".
	</p>
	<p>
	Note that file formats using PAM (Persistent Auxilary Metadata) services will generally cache
	statistics in the .pam file allowing fast fetch after the first request.
	</p>
	End Rem
	Method GetStatistics:Int(approxOK:Int, force:Int, _min:Double Var, _max:Double Var, _mean:Double Var, _stddev:Double Var)
		Return bmx_gdal_GDALRasterBand_GetStatistics(objectPtr, approxOK, force, Varptr _min, Varptr _max, Varptr _mean, Varptr _stddev)
	End Method
	
	Rem
	bbdoc: Sets statistics on band.
	about: This method can be used to store min/max/mean/standard deviation statistics on a raster band.
	<p>
	The default implementation stores them as metadata, and will only work on formats that can save
	arbitrary metadata. This method cannot detect whether metadata will be properly saved and so may
	return CE_None even if the statistics will never be saved.
	</p>
	End Rem
	Method SetStatistics:Int(_min:Double, _max:Double, _mean:Double, _stddev:Double)
		Return bmx_gdal_GDALRasterBand_SetStatistics(objectPtr, _min, _max, _mean, _stddev)
	End Method
	
	Rem
	bbdoc: Checks for arbitrary overviews.
	about: This returns TRUE if the underlying datastore can compute arbitrary overviews efficiently,
	such as is the case with OGDI over a network. Datastores with arbitrary overviews don't generally
	have any fixed overviews, but the RasterIO() method can be used in downsampling mode to get overview
	data efficiently.
	End Rem
	Method HasArbitraryOverviews:Int()
		Return bmx_gdal_GDALRasterBand_HasArbitraryOverviews(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of overview layers available. 
	End Rem
	Method GetOverviewCount:Int()
		Return bmx_gdal_GDALRasterBand_GetOverviewCount(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches overview raster band object. 
	End Rem
	Method GetOverview:GDALRasterBand(index:Int)
		Return GDALRasterBand._create(bmx_gdal_GDALRasterBand_GetOverview(objectPtr, index))
	End Method
	
	Rem
	bbdoc: Fetches best sampling overview.
	about: Returns the most reduced overview of the given band that still satisfies the desired number
	of samples. This method can be used with zero as the number of desired samples to fetch the most
	reduced overview. The same band as was passed in will be returned if it has not overviews, or if none
	of the overviews have enough samples.
	End Rem
	Method GetRasterSampleOverview:GDALRasterBand(desiredSamples:Int)
		Return GDALRasterBand._create(bmx_gdal_GDALRasterBand_GetRasterSampleOverview(objectPtr, desiredSamples))
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
	
	Rem
	bbdoc: Adds a mask band to the current band.
	about: The default implementation of the CreateMaskBand() method is implemented based on similar
	rules to the .ovr handling implemented using the GDALDefaultOverviews object. A TIFF file with the
	extension .msk will be created with the same basename as the original file, and it will have as
	many bands as the original image (or just one for GMF_PER_DATASET). The mask images will be deflate
	compressed tiled images with the same block size as the original image if possible.
	End Rem
	Method CreateMaskBand:Int(flags:Int)
		Return bmx_gdal_GDALRasterBand_CreateMaskBand(objectPtr, flags)
	End Method
	
	Rem
	bbdoc: Reads/writes a region of image data for this band.
	about: This method allows reading a region of a GDALRasterBand into a buffer, or writing data from a buffer into a region of
	a GDALRasterBand. It automatically takes care of data type translation if the data type (@bufType) of the buffer is different
	than that of the GDALRasterBand. The method also takes care of image decimation / replication if the buffer size (@bufXSize x
	@bufYSize) is different than the size of the region being accessed (@xSize x @ySize).
	<p>
	The @pixelSpace and nLineSpace parameters allow reading into or writing from unusually organized buffers. This is primarily used
	for buffers containing more than one bands raster data in interleaved format.
	</p>
	<p>
	Some formats may efficiently implement decimation into a buffer by reading from lower resolution overview images.
	</p>
	<p>
	For highest performance full resolution data access, read and write on "block boundaries" as returned by GetBlockSize(), or use
	the ReadBlock() and WriteBlock() methods.
	</p>
	End Rem
	Method RasterIO:Int(rwFlag:Int, xOff:Int, yOff:Int, xSize:Int, ySize:Int, data:Byte Ptr, bufXSize:Int, bufYSize:Int, ..
			bufType:Int, pixelSpace:Int, lineSpace:Int)
		Return bmx_gdal_GDALRasterBand_RasterIO(objectPtr, rwFlag, xOff, yOff, xSize, ySize, data, bufXSize, bufYSize, ..
			bufType, pixelSpace, lineSpace)
	End Method
	
	Rem
	bbdoc: Reads a block of image data efficiently.
	about: This method accesses a "natural" block from the raster band without resampling, or data type conversion. For a more
	generalized, but potentially less efficient access use RasterIO().
	End Rem
	Method ReadBlock:Int(xBlockOff:Int, yBlockOff:Int, image:Byte Ptr)
		Return bmx_gdal_GDALRasterBand_ReadBlock(objectPtr, xBlockOff, yBlockOff, image)
	End Method
	
	Rem
	bbdoc: Writes a block of image data efficiently.
	about: This method accesses a "natural" block from the raster band without resampling, or data type conversion. For a more
	generalized, but potentially less efficient access use RasterIO().
	End Rem
	Method WriteBlock:Int(xBlockOff:Int, yBlockOff:Int, image:Byte Ptr)
		Return bmx_gdal_GDALRasterBand_WriteBlock(objectPtr, xBlockOff, yBlockOff, image)
	End Method
	
End Type

Rem
bbdoc: Manages the registration of file format drivers.
End Rem
Type GDALDriverManager Extends GDALMajorObject

	Rem
	bbdoc: Fetches the number of registered drivers.
	End Rem
	Function GetDriverCount:Int()
		Return bmx_gdal_GDALDriverManager_GetDriverCount()
	End Function
	
	Rem
	bbdoc: Fetches driver by index. 
	End Rem
	Function GetDriver:GDALDriver(index:Int)
		Return GDALDriver._create(bmx_gdal_GDALDriverManager_GetDriver(index))
	End Function
	
	Rem
	bbdoc: Fetches a driver based on the short name. 
	End Rem
	Function GetDriverByName:GDALDriver(name:String)
		Return GDALDriver._create(bmx_gdal_GDALDriverManager_GetDriverByName(name))
	End Function

End Type

Rem
bbdoc: Ground Control Point.
End Rem
Type GDAL_GCP

	Field objectPtr:Byte Ptr
	Field owner:Int
	
	Function _create:GDAL_GCP(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:GDAL_GCP = New GDAL_GCP
			this.objectPtr = objectPtr
			this.owner = False
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:GDAL_GCP(id:String, info:String, pixel:Double, line:Double, x:Double, y:Double, z:Double)
		objectPtr = bmx_gdal_GDAL_GCP_create(id, info, pixel, line, x, y, z)
		owner = True
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method GetId:String()
		Return bmx_gdal_GDAL_GCP_GetID(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetId(id:String)
		bmx_gdal_GDAL_GCP_SetId(objectPtr, id)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetInfo:String()
		Return bmx_gdal_GDAL_GCP_GetInfo(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetInfo(info:String)
		bmx_gdal_GDAL_GCP_SetInfo(objectPtr, info)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetPixel:Double()
		Return bmx_gdal_GDAL_GCP_GetPixel(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetPixel(pixel:Double)
		bmx_gdal_GDAL_GCP_SetPixel(objectPtr, pixel)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetLine:Double()
		Return bmx_gdal_GDAL_GCP_GetLine(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetLine(line:Double)
		bmx_gdal_GDAL_GCP_SetLine(objectPtr, line)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetX:Double()
		Return bmx_gdal_GDAL_GCP_GetX(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetX(x:Double)
		bmx_gdal_GDAL_GCP_SetX(objectPtr, x)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetY:Double()
		Return bmx_gdal_GDAL_GCP_GetY(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetY(y:Double)
		bmx_gdal_GDAL_GCP_SetY(objectPtr, y)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetZ:Double()
		Return bmx_gdal_GDAL_GCP_GetZ(objectPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetZ(z:Double)
		bmx_gdal_GDAL_GCP_SetZ(objectPtr, z)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If objectPtr Then
			If owner Then
				bmx_gdal_GDAL_GCP_free(objectPtr)
			End If
			objectPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A color table / palette. 
End Rem
Type GDALColorTable

	Field objectPtr:Byte Ptr
	
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
bbdoc: Manager for OGRSFDriver instances that will be used to try and open datasources.
about: Normally the registrar is populated with standard drivers using the OGRRegisterAll() function.
The driver registrar and all registered drivers may be cleaned up on shutdown using OGRCleanupAll(). 
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
	bbdoc: Return the ids th datasource opened. 
	End Rem
	Function GetOpenDS:OGRDataSource(ids:Int)
		Return OGRDataSource._create(bmx_gdal_OGRSFDriverRegistrar_GetOpenDS(ids))
	End Function
	
End Type


Rem
bbdoc: Represents an operational format driver.
about: One OGRSFDriver derived type will normally exist for each file format registered for use,
regardless of whether a file has or will be opened. The list of available drivers is normally managed
by the OGRSFDriverRegistrar. 
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
	bbdoc: Attempts to open file with this driver.
	about: This method is what OGRSFDriverRegistrar uses to implement its Open() method. See it for more
	details.
	<p>
	Note, drivers do not normally set their own (internal) m_poDriver value, so a direct call to this
	method (instead of indirectly via OGRSFDriverRegistrar) will usually result in a datasource that
	does not know what driver it relates to if GetDriver() is called on the datasource. The application
	may directly call SetDriver() after opening with this method to avoid this problem.
	End Rem
	Method Open:OGRDataSource(name:String, update:Int = False)
		Return OGRDataSource._create(bmx_gdal_OGRSFDriver_Open(objectPtr, name, update))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method TestCapability:Int(capability:String)
		Return bmx_gdal_OGRSFDriver_TestCapability(objectPtr, capability)
	End Method
	
	Rem
	bbdoc: Attempts to create a new data source based on the passed driver.
	about: The @options argument can be used to control driver specific creation options.
	These options are normally documented in the format specific documentation.
	End Rem
	Method CreateDataSource:OGRDataSource(name:String, options:String[] = Null)
		If options Then
			Return OGRDataSource._create(bmx_gdal_OGRSFDriver_CreateDataSource(objectPtr, name, options))
		Else
			Return OGRDataSource._create(bmx_gdal_OGRSFDriver_CreateDataSource(objectPtr, name, Null))
		End If
	End Method
	
	Rem
	bbdoc: Destroys a datasource.
	about: Destroy the named datasource. Normally it would be safest if the datasource was not open at the time.
	<p>
	Whether this is a supported operation on this driver case be tested using TestCapability() on
	ODrCDeleteDataSource.
	</p>
	End Rem
	Method DeleteDataSource:Int(datasource:String)
		Return bmx_gdal_OGRSFDriver_DeleteDataSource(objectPtr, datasource)
	End Method
	
	Rem
	bbdoc: Fetches name of driver (file format).
	about: This name should be relatively short (10-40 characters), and should reflect the underlying file
	format. For instance "ESRI Shapefile".
	End Rem
	Method GetName:String()
		Return bmx_gdal_OGRSFDriver_GetName(objectPtr)
	End Method
	
End Type

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
	bbdoc: Deletes the indicated layer from the datasource.
	about: If this method is supported the ODsCDeleteLayer capability will test TRUE on the OGRDataSource.
	End Rem
	Method DeleteLayer:Int(layer:Int)
		Return bmx_gdal_OGRDataSource_DeleteLayer(objectPtr, layer)
	End Method
	
	Rem
	bbdoc: Tests if capability is available.
	about: One of the following data source capability names can be passed into this method, and a
	TRUE or FALSE value will be returned indicating whether or not the capability is available
	for this object.
	<ul>
	<li>ODsCCreateLayer: True if this datasource can create new layers.</li>
	</ul>
	End Rem
	Method TestCapability:Int(name:String)
		Return bmx_gdal_OGRDataSource_TestCapability(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Attempts to create a new layer on the data source with the indicated name, coordinate system, geometry type.
	about: The @options argument can be used to control driver specific creation options. These options are
	normally documented in the format specific documentation.
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
	bbdoc: Destroys the datasource and frees its resources.
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

	Function _create:OGRSpatialReference(objectPtr:Byte Ptr)
		If objectPtr Then
			Local this:OGRSpatialReference = New OGRSpatialReference
			this.objectPtr = objectPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Creates a new OGRSpatialReference object.
	about: Takes an optional string argument which if passed should be a WKT representation of an SRS.
	Passing this is equivalent to not passing it, and then calling ImportFromWkt() with the WKT string.
	End Rem
	Method Create:OGRSpatialReference(ref:String)
		objectPtr = bmx_gdal_OGRSpatialReference_create(ref)
		Return Self
	End Method
	
	Rem
	bbdoc: Makes a duplicate of this OGRSpatialReference.
	End Rem
	Method Clone:OGRSpatialReference()
		Return OGRSpatialReference._create(bmx_gdal_OGRSpatialReference_Clone(objectPtr))
	End Method
	
	Rem
	bbdoc: Makes a duplicate of the GEOGCS node of this OGRSpatialReference object.
	End Rem
	Method CloneGeogCS:OGRSpatialReference()
		Return OGRSpatialReference._create(bmx_gdal_OGRSpatialReference_CloneGeogCS(objectPtr))
	End Method
	
	Rem
	bbdoc: Converts this SRS into WKT format.
	End Rem
	Method ExportToWkt:Int(result:String Var)
		Local res:Int
		result = bmx_gdal_OGRSpatialReference_ExportToWkt(objectPtr, Varptr res)
		Return res
	End Method
	
	Method ExportToPrettyWkt:Int(result:String Var, simplify:Int = False)
	' TODO
	End Method
	
	Rem
	bbdoc: Converts in place to ESRI WKT format.
	about: The value nodes of this coordinate system are modified in various manners more closely map onto
	the ESRI concept of WKT format. This includes renaming a variety of projections and arguments, and stripping
	out nodes note recognised by ESRI (like AUTHORITY and AXIS).
	End Rem
	Method MorphToESRI:Int()
		Return bmx_gdal_OGRSpatialReference_morphToESRI(objectPtr)
	End Method
	
	Rem
	bbdoc: Converts in place from ESRI WKT format.
	about: The value notes of this coordinate system are modified in various manners to adhere more closely to
	the WKT standard. This mostly involves translating a variety of ESRI names for projections, arguments and
	datums to "standard" names, as defined by Adam Gawne-Cain's reference translation of EPSG to WKT for the
	CT specification.
	End Rem
	Method MorphFromESRI:Int()
		Return bmx_gdal_OGRSpatialReference_morphFromESRI(objectPtr)
	End Method
	
	Rem
	bbdoc: Validates SRS tokens.
	about: This method attempts to verify that the spatial reference system is well formed, and consists of known tokens.
	The validation is not comprehensive.
	End Rem
	Method Validate:Int()
		Return bmx_gdal_OGRSpatialReference_Validate(objectPtr)
	End Method
	
	Method StripCTParms:Int(node:OGR_SRSNode = Null)
	' TODO
	End Method
	
	Rem
	bbdoc: Corrects parameter ordering to match CT Specification.
	about: Some mechanisms to create WKT using OGRSpatialReference, and some imported WKT fail to maintain the
	order of parameters required according to the BNF definitions in the OpenGIS SF-SQL and CT Specifications.
	This method attempts to massage things back into the required order.
	End Rem
	Method FixupOrdering:Int()
		Return bmx_gdal_OGRSpatialReference_FixupOrdering(objectPtr)
	End Method
	
	Rem
	bbdoc: Fixup as needed.
	about: Some mechanisms to create WKT using OGRSpatialReference, and some imported WKT, are not valid according
	to the OGC CT specification. This method attempts to fill in any missing defaults that are required, and fixup
	ordering problems (using OSRFixupOrdering()) so that the resulting WKT is valid.
	<p>
	This method should be expected to evolve over time to as problems are discovered. The following are amoung the
	fixup actions this method will take:
	<ul>
	<li>Fixup the ordering of nodes to match the BNF WKT ordering, using the FixupOrdering() method.</li>
	<li>Add missing linear or angular units nodes.</li>
	</ul>
	</p>
	End Rem
	Method Fixup:Int()
		Return bmx_gdal_OGRSpatialReference_Fixup(objectPtr)
	End Method
	
	Rem
	bbdoc: Returns TRUE if EPSG feels this geographic coordinate system should be treated as having lat/long coordinate ordering.
	about: Currently this returns TRUE for all geographic coordinate systems with an EPSG code set, and AXIS
	values set defining it as lat, long. Note that coordinate systems with an EPSG code and no axis settings
	will be assumed to not be lat/long.
	<p>
	FALSE will be returned for all coordinate systems that are not geographic, or that do not have an EPSG code set.
	</p>
	End Rem
	Method EPSGTreatsAsLatLong:Int()
		Return bmx_gdal_OGRSpatialReference_EPSGTreatsAsLatLong(objectPtr)
	End Method

	Rem
	bbdoc: Sets the linear units for the projection.
	about: This method creates a UNITS subnode with the specified values as a child of the PROJCS or LOCAL_CS node.
	It works the same as the SetLinearUnits() method, but it also updates all existing linear projection parameter
	values from the old units to the new units.
	End Rem
	Method SetLinearUnitsAndUpdateParameters:Int(name:String, inMeters:Double)
		Return bmx_gdal_OGRSpatialReference_SetLinearUnitsAndUpdateParameters(objectPtr, name, inMeters)
	End Method

	Rem
	bbdoc: Sets the linear units for the projection.
	about: This method creates a UNITS subnode with the specified values as a child of the PROJCS or LOCAL_CS node.
	End Rem
	Method SetLinearUnits:Int(name:String, inMeters:Double)
		Return bmx_gdal_OGRSpatialReference_SetLinearUnits(objectPtr, name, inMeters)
	End Method

	Rem
	bbdoc: Fetches linear projection units.
	about: If no units are available, a value of "Meters" and 1.0 will be assumed. This method only checks directly
	under the PROJCS or LOCAL_CS node for units.
	End Rem
	Method GetLinearUnits:Double(name:String Var)
		Local units:Double
		name = bmx_gdal_OGRSpatialReference_GetLinearUnits(objectPtr, Varptr units)
		Return units
	End Method

	Rem
	bbdoc: Sets the angular units for the geographic coordinate system.
	about: This method creates a UNITS subnode with the specified values as a child of the GEOGCS node.
	End Rem
	Method SetAngularUnits:Int(name:String, inDegrees:Double)
		Return bmx_gdal_OGRSpatialReference_SetAngularUnits(objectPtr, name, inDegrees)
	End Method

	Rem
	bbdoc: Fetches angular geographic coordinate system units.
	about: If no units are available, a value of "degree" and SRS_UA_DEGREE_CONV will be assumed. This method only
	checks directly under the GEOGCS node for units.
	End Rem
	Method GetAngularUnits:Double(name:String Var)
		Local units:Double
		name = bmx_gdal_OGRSpatialReference_GetAngularUnits(objectPtr, Varptr units)
		Return units
	End Method

	Rem
	bbdoc: Fetches prime meridian info.
	about: Returns the offset of the prime meridian from greenwich in degrees, and the prime meridian name
	(if requested). If no PRIMEM value exists in the coordinate system definition a value of "Greenwich" and an
	offset of 0.0 is assumed.
	End Rem
	Method GetPrimeMeridian:Double(name:String Var)
		Local meridian:Double
		name = bmx_gdal_OGRSpatialReference_GetPrimeMeridian(objectPtr, Varptr meridian)
		Return meridian
	End Method

	Rem
	bbdoc: Checks if geographic coordinate system.
	End Rem
	Method IsGeographic:Int()
		Return bmx_gdal_OGRSpatialReference_IsGeographic(objectPtr)
	End Method

	Rem
	bbdoc: Checks if projected coordinate system. 
	End Rem
	Method IsProjected:Int()
		Return bmx_gdal_OGRSpatialReference_IsProjected(objectPtr)
	End Method

	Rem
	bbdoc: Checks if local coordinate system.
	End Rem
	Method IsLocal:Int()
		Return bmx_gdal_OGRSpatialReference_IsLocal(objectPtr)
	End Method

	Method IsSameGeogCS:Int(ref:OGRSpatialReference)
	' TODO
	End Method

	Method IsSame:Int(ref:OGRSpatialReference)
	' TODO
	End Method

	Rem
	bbdoc: Wipes current definition.
	about: Returns OGRSpatialReference to a state with no definition, as it exists when first created. It does
	not affect reference counts. 
	End Rem
	Method Clear()
		bmx_gdal_OGRSpatialReference_Clear(objectPtr)
	End Method

	Rem
	bbdoc: Sets the user visible LOCAL_CS name.
	about: This method is will ensure a LOCAL_CS node is created as the root, and set the provided name on it.
	It must be used before SetLinearUnits().
	End Rem
	Method SetLocalCS:Int(name:String)
		Return bmx_gdal_OGRSpatialReference_SetLocalCS(objectPtr, name)
	End Method

	Rem
	bbdoc: Sets the user visible PROJCS name.
	about: This method is will ensure a PROJCS node is created as the root, and set the provided name on it.
	If used on a GEOGCS coordinate system, the GEOGCS node will be demoted to be a child of the new PROJCS root.
	End Rem
	Method SetProjCS:Int(name:String)
		Return bmx_gdal_OGRSpatialReference_SetProjCS(objectPtr, name)
	End Method

	Rem
	bbdoc: Sets a projection name. 
	End Rem
	Method SetProjection:Int(name:String)
		Return bmx_gdal_OGRSpatialReference_SetProjection(objectPtr, name)
	End Method

	Method SetGeogCS:Int(geogName:String, datumName:String, ellipsoidName:String, semiMajor:Double, ..
			invFlattening:Double, pmName:String = Null, pmOffset:Double = 0.0, units:String = Null, ..
			convertToRadians:Double = 0.0)
	' TODO
	End Method

	Rem
	bbdoc: Sets a GeogCS based on well known name.
	about: This may be called on an empty OGRSpatialReference to make a geographic coordinate system, or on
	something with an existing PROJCS node to set the underlying geographic coordinate system of a projected
	coordinate system.
	<p>
	The following well known text values are currently supported:
	<ul>
	<li>"WGS84": same as "EPSG:4326" but has no dependence on EPSG data files.</li>
	<li>"WGS72": same as "EPSG:4322" but has no dependence on EPSG data files.</li>
	<li>"NAD27": same as "EPSG:4267" but has no dependence on EPSG data files.</li>
	<li>"NAD83": same as "EPSG:4269" but has no dependence on EPSG data files.</li>
	<li>"EPSG:n": same as doing an ImportFromEPSG(n).</li>
	</ul>
	</p>
	End Rem
	Method SetWellKnownGeogCS:Int(name:String)
		Return bmx_gdal_OGRSpatialReference_SetWellKnownGeogCS(objectPtr, name)
	End Method

	Method CopyGeogCSFrom:Int(srcSRS:OGRSpatialReference)
	' TODO
	End Method

	Rem
	bbdoc: Sets spatial reference from various text formats.
	about: This method will examine the provided input, and try to deduce the format, and then use it to
	initialize the spatial reference system. It may take the following forms:
	<ol>
	<li>Well Known Text definition - passed on to ImportFromWkt().
	<li>"EPSG:n" - number passed on to ImportFromEPSG().
	<li>"EPSGA:n" - number passed on to ImportFromEPSGA().
	<li>"AUTO:proj_id,unit_id,lon0,lat0" - WMS auto projections.
	<li>"urn:ogc:def:crs:EPSG::n" - ogc urns
	<li>PROJ.4 definitions - passed on to ImportFromProj4().
	<li>filename - file read for WKT, XML or PROJ.4 definition.
	<li>well known name accepted by SetWellKnownGeogCS(), such as NAD27, NAD83, WGS84 or WGS72.
	<li>WKT (directly or in a file) in ESRI format should be prefixed with ESRI:: to trigger an automatic
	KorphFromESRI().
	</ul>
	<p>
	It is expected that this method will be extended in the future to support XML and perhaps a simplified
	"minilanguage" for indicating common UTM and State Plane definitions.
	</p>
	<p>
	This method is intended to be flexible, but by it's nature it is imprecise as it must guess information
	about the format intended. When possible applications should call the specific method appropriate if the
	input is known to be in a particular format.
	</p>
	End Rem
	Method SetFromUserInput:Int(definition:String)
		Return bmx_gdal_OGRSpatialReference_SetFromUserInput(objectPtr, definition)
	End Method

	Method SetTOWGS84:Int(dfDX:Double, dy:Double, dz:Double, ex:Double = 0.0, ey:Double = 0.0, ez:Double = 0.0, ppm:Double = 0.0)
	' TODO
	End Method

	Method GetTOWGS84:Int(padfCoef:Double[])
	' TODO
	End Method

	Rem
	bbdoc: Gets spheroid semi major axis.
	End Rem
	Method GetSemiMajor:Int(semiMajor:Double Var)
		Return bmx_gdal_OGRSpatialReference_GetSemiMajor(objectPtr, Varptr semiMajor)
	End Method

	Rem
	bbdoc: Gets spheroid semi minor axis.
	End Rem
	Method GetSemiMinor:Int(semiMinor:Double Var)
		Return bmx_gdal_OGRSpatialReference_GetSemiMinor(objectPtr, Varptr semiMinor)
	End Method

	Rem
	bbdoc: Gets spheroid inverse flattening.
	End Rem
	Method GetInvFlattening:Int(invFlattening:Double Var)
		Return bmx_gdal_OGRSpatialReference_GetInvFlattening(objectPtr, Varptr invFlattening)
	End Method

	Rem
	bbdoc: Sets the authority for a node.
	End Rem
	Method SetAuthority:Int(targetKey:String, authority:String, code:Int)
		Return bmx_gdal_OGRSpatialReference_SetAuthority(objectPtr, targetKey, authority, code)
	End Method

	Rem
	bbdoc: Sets EPSG authority info if possible.
	about: This method inspects a WKT definition, and adds EPSG authority nodes where an aspect of the coordinate
	system can be easily and safely corresponded with an EPSG identifier. In practice, this method will evolve over
	time. In theory it can add authority nodes for any object (ie. spheroid, datum, GEOGCS, units, and PROJCS) that
	could have an authority node. Mostly this is useful to inserting appropriate PROJCS codes for common
	formulations (like UTM n WGS84).
	<p>
	If it successful the OGRSpatialReference is updated in place, and the method return OGRERR_NONE. If the method
	fails to identify the general coordinate system OGRERR_UNSUPPORTED_SRS is returned but no error message is
	posted via CPLError().
	</p>
	End Rem
	Method AutoIdentifyEPSG:Int()
		Return bmx_gdal_OGRSpatialReference_AutoIdentifyEPSG(objectPtr)
	End Method

	Rem
	bbdoc: Gets the authority code for a node.
	about: This method is used to query an AUTHORITY[] node from within the WKT tree, and fetch the code value.
	<p>
	While in theory values may be non-numeric, for the EPSG authority all code values should be integral.
	</p>
	End Rem
	Method GetAuthorityCode:String(targetKey:String)
		Return bmx_gdal_OGRSpatialReference_GetAuthorityCode(objectPtr, targetKey)
	End Method

	Rem
	bbdoc: Gets the authority name for a node.
	about: This method is used to query an AUTHORITY[] node from within the WKT tree, and fetch the authority name value.
	<p>
	The most common authority is "EPSG".
	</p>
	End Rem
	Method GetAuthorityName:String(targetKey:String)
		Return bmx_gdal_OGRSpatialReference_GetAuthorityName(objectPtr, targetKey)
	End Method

	Rem
	bbdoc: Fetches extension value.
	about: Fetches the value of the named EXTENSION item for the identified target node.
	End Rem
	Method GetExtension:String(targetKey:String, name:String, defaultValue:String = Null)
		Return bmx_gdal_OGRSpatialReference_GetExtension(objectPtr, targetKey, name, defaultValue)
	End Method

	Rem
	bbdoc: Sets extension value.
	about: Sets the value of the named EXTENSION item for the identified target node.
	End Rem
	Method SetExtension:Int(targetKey:String, name:String, value:String)
		Return bmx_gdal_OGRSpatialReference_SetExtension(objectPtr, targetKey, name, value)
	End Method

	Method FindProjParm:Int(parameter:String, PROJCS:OGR_SRSNode = Null)
	' TODO
	End Method

	Rem
	bbdoc: Sets a projection parameter value.
	about: Adds a new PARAMETER under the PROJCS with the indicated name and value.
	<p>
	Please check http://www.remotesensing.org/geotiff/proj_list pages for legal parameter names for specific projections.
	</p>
	End Rem
	Method SetProjParm:Int(name:String, value:Double)
		Return bmx_gdal_OGRSpatialReference_SetProjParm(objectPtr, name, value)
	End Method

	Rem
	bbdoc: Fetches a projection parameter value.
	about: NOTE: This code should be modified to translate non degree angles into degrees based on the GEOGCS unit.
	This has not yet been done.
	End Rem
	Method GetProjParm:Int(name:String, result:Double Var, defaultValue:Double = 0.0)
		Return bmx_gdal_OGRSpatialReference_GetProjParm(objectPtr, name, Varptr result, defaultValue)
	End Method

	Rem
	bbdoc: Sets a projection parameter with a normalized value.
	about: This method is the same as SetProjParm() except that the value of the parameter passed in is assumed
	to be in "normalized" form (decimal degrees for angular values, meters for linear values. The values are
	converted in a form suitable for the GEOGCS and linear units in effect.
	End Rem
	Method SetNormProjParm:Int(name:String, value:Double)
		Return bmx_gdal_OGRSpatialReference_SetNormProjParm(objectPtr, name, value)
	End Method

	Rem
	bbdoc: Fetches a normalized projection parameter value.
	about: This method is the same as GetProjParm() except that the value of the parameter is "normalized"
	into degrees or meters depending on whether it is linear or angular.
	End Rem
	Method GetNormProjParm:Int(name:String, result:Double Var, defaultValue:Double = 0.0)
		Return bmx_gdal_OGRSpatialReference_GetNormProjParm(objectPtr, name, Varptr result, defaultValue)
	End Method

	Rem
	bbdoc: Sets UTM projection definition.
	about: Universal Transverse Mercator
	<p>
	This will generate a projection definition with the full set of transverse mercator projection parameters
	for the given UTM zone. If no PROJCS[] description is set yet, one will be set to look like "UTM Zone %d,
	{Northern, Southern} Hemisphere".
	</p>
	End Rem
	Method SetUTM:Int(zone:Int, north:Int = True)
		Return bmx_gdal_OGRSpatialReference_SetUTM(objectPtr, zone, north)
	End Method
	
	Rem
	bbdoc: Gets utm zone information.
	End Rem
	Method GetUTMZone:Int(north:Int Var)
		Return bmx_gdal_OGRSpatialReference_GetUTMZone(objectPtr, Varptr north)
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
bbdoc: Definition of an attribute of an OGRFeatureDefn.
End Rem
Type OGRFieldDefn

	Field objectPtr:Byte Ptr
	
	Rem
	bbdoc: Creates a new OGRFieldDefn object.
	End Rem
	Method Create:OGRFieldDefn(name:String, fieldType:Int)
		objectPtr = bmx_gdal_OGRFieldDefn_create(name, fieldType)
		Return Self
	End Method
	
	Rem
	bbdoc: Resets the name of this field.
	End Rem
	Method SetName(name:String)
		bmx_gdal_OGRFieldDefn_SetName(objectPtr, name)
	End Method
	
	Rem
	bbdoc: Fetches name of this field.
	End Rem
	Method GetNameRef:String()
		Return bmx_gdal_OGRFieldDefn_GetNameRef(objectPtr)
	End Method
	
	Rem
	bbdoc: Fetches type of this field.
	about: One of OFTInteger, OFTIntegerList, OFTReal, OFTRealList, OFTString, OFTStringList,
	OFTBinary, OFTDate, OFTTime or OFTDateTime.
	End Rem
	Method GetType:Int()
		Return bmx_gdal_OGRFieldDefn_GetType(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the type of this field.
	about: This should never be done to an OGRFieldDefn that is already part of an OGRFeatureDefn.
	<p>
	One of OFTInteger, OFTIntegerList, OFTReal, OFTRealList, OFTString, OFTStringList,
	OFTBinary, OFTDate, OFTTime or OFTDateTime.
	</p>
	End Rem
	Method SetType(fieldType:Int)
		bmx_gdal_OGRFieldDefn_SetType(objectPtr, fieldType)
	End Method
	
	Rem
	bbdoc: Gets the justification for this field.
	about: One of OJUndefined, OJLeft or OJRight.
	End Rem
	Method GetJustify:Int()
		Return bmx_gdal_OGRFieldDefn_GetJustify(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the justification for this field.
	about: One of OJUndefined, OJLeft or OJRight.
	End Rem
	Method SetJustify(justify:Int)
		bmx_gdal_OGRFieldDefn_SetJustify(objectPtr, justify)
	End Method
	
	Rem
	bbdoc: Gets the formatting width for this field.
	End Rem
	Method GetWidth:Int()
		Return bmx_gdal_OGRFieldDefn_GetWidth(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the formatting width for this field in characters.
	End Rem
	Method SetWidth(width:Int)
		bmx_gdal_OGRFieldDefn_SetWidth(objectPtr, width)
	End Method
	
	Rem
	bbdoc: Get the formatting precision for this field.
	about: This should normally be zero for fields of types other than OFTReal. 
	End Rem
	Method GetPrecision:Int()
		Return bmx_gdal_OGRFieldDefn_GetPrecision(objectPtr)
	End Method
	
	Rem
	bbdoc: Sets the formatting precision for this field in characters.
	about: This should normally be zero for fields of types other than OFTReal.
	End Rem
	Method SetPrecision(precision:Int)
		bmx_gdal_OGRFieldDefn_SetPrecision(objectPtr, precision)
	End Method
	
	Rem
	bbdoc: Sets defining parameters for a field in one call.
	End Rem
	Method Set(name:String, fieldType:Int, width:Int = 0, precision:Int = 0, justify:Int = OJUndefined)
		bmx_gdal_OGRFieldDefn_Set(objectPtr, name, fieldType, width, precision, justify)
	End Method
	
	Rem
	bbdoc: 
	End Rem
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


Type OGR_SRSNode
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

	Rem
	bbdoc: 
	End Rem
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

	Rem
	bbdoc: 
	End Rem
	Method AddSimpleSource:Int( srcBand:GDALRasterBand, srcXOff:Int = -1, srcYOff:Int =-1, ..
			srcXSize:Int =-1, srcYSize:Int =-1, dstXOff:Int =-1, dstYOff:Int =-1, dstXSize:Int =-1, dstYSize:Int =-1, ..
			resampling:String = "near", noDataValue:Double = -1234.56)
	
		Return bmx_gdal_VRTSourcedRasterBand_AddSimpleSource(objectPtr, srcBand.objectPtr, srcXOff, srcYOff, srcXSize, srcYSize, ..
			dstXOff, dstYOff, dstXSize, dstYSize, resampling, noDataValue)
	
	End Method

End Type

Type VRTDriver Extends GDALDriver
End Type

