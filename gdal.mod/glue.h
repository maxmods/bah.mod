/*
 Copyright (c) 2009 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#include "gdal_priv.h"
#include "gdal_alg.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#include "ogrsf_frmts.h"
#include "vrtdataset.h"


extern "C" {

#include "blitz.h"

	void _bah_gdal_GDALDataset__setGCP(BBArray * gcps, int i, const GDAL_GCP * list);


	GDALDataset * bmx_gdal_GDALOpen(BBString * filename, GDALAccess access);
	GDALDataset * bmx_gdal_GDALOpenShared(BBString * filename, GDALAccess access);
	void bmx_gdal_GDALAllRegister();
	
	BBString * bmx_gdal_GDALMajorObject_GetDescription(GDALMajorObject * handle);
	BBString * bmx_gdal_GDALMajorObject_GetMetadataItem(GDALMajorObject * handle, BBString * name, BBString * domain);
	BBArray * bmx_gdal_GDALMajorObject_GetMetadata(GDALMajorObject * handle, BBString * domain);
	CPLErr bmx_gdal_GDALMajorObject_SetMetadataItem(GDALMajorObject * handle, BBString * name, BBString * value, BBString * domain);
	CPLErr bmx_gdal_GDALMajorObject_SetMetadata(GDALMajorObject * handle, BBArray * metadata, BBString * domain);
	void bmx_gdal_GDALMajorObject_SetDescription(GDALMajorObject * handle, BBString * description);

	GDALDriver * bmx_gdal_GDALDataset_GetDriver(GDALDataset * handle);
	int bmx_gdal_GDALDataset_GetRasterXSize(GDALDataset * handle);
	int bmx_gdal_GDALDataset_GetRasterYSize(GDALDataset * handle);
	int bmx_gdal_GDALDataset_GetRasterCount(GDALDataset * handle);
	CPLErr bmx_gdal_GDALDataset_GetGeoTransform(GDALDataset * handle, BBArray * padfTransform);
	BBString * bmx_gdal_GDALDataset_GetProjectionRef(GDALDataset * handle);
	GDALRasterBand * bmx_gdal_GDALDataset_GetRasterBand(GDALDataset * handle, int index);
	void bmx_gdal_GDALDataset_Close(GDALDataset * handle);
	CPLErr bmx_gdal_GDALDataset_SetProjection(GDALDataset * handle, BBString * projection);
	CPLErr bmx_gdal_GDALDataset_SetGeoTransform(GDALDataset * handle, BBArray * transform);
	int bmx_gdal_GDALDataset_GetGCPCount(GDALDataset * handle);
	void bmx_gdal_GDALDataset_FlushCache(GDALDataset * handle);
	CPLErr bmx_gdal_GDALDataset_AddBand(GDALDataset * handle, GDALDataType dataType, BBArray * options);
	BBArray * bmx_gdal_GDALDataset_GetFileList(GDALDataset * handle);
	BBString * bmx_gdal_GDALDataset_GetGCPProjection(GDALDataset * handle);
	CPLErr bmx_gdal_GDALDataset_CreateMaskBand(GDALDataset * handle, int flags);
	int bmx_gdal_GDALDataset_GetShared(GDALDataset * handle);
	void bmx_gdal_GDALDataset_MarkAsShared(GDALDataset * handle);
	void bmx_gdal_GDALDataset_GetGCPs(GDALDataset * handle, BBArray * gcps);

	CPLErr bmx_gdal_GDALRasterBand_GenerateContour(GDALRasterBand * handle, double contourInterval, double contourBase, BBArray * fixedLevels,
		int useNoData, double noDataValue, OGRLayer * layer, int idField, int elevField);
	int bmx_gdal_GDALRasterBand_GetXSize(GDALRasterBand * handle);
	int bmx_gdal_GDALRasterBand_GetYSize(GDALRasterBand * handle);
	int bmx_gdal_GDALRasterBand_GetBand(GDALRasterBand * handle);
	GDALColorInterp bmx_gdal_GDALRasterBand_GetColorInterpretation(GDALRasterBand * handle);
	int bmx_gdal_GDALRasterBand_GetMaskFlags(GDALRasterBand * handle);
	GDALDataType bmx_gdal_GDALRasterBand_GetRasterDataType(GDALRasterBand * handle);
	double bmx_gdal_GDALRasterBand_GetMaximum(GDALRasterBand * handle, int * success);
	double bmx_gdal_GDALRasterBand_GetMinimum(GDALRasterBand * handle, int * success);
	double bmx_gdal_GDALRasterBand_GetNoDataValue(GDALRasterBand * handle, int * success);
	double bmx_gdal_GDALRasterBand_GetOffset(GDALRasterBand * handle, int * success);
	void bmx_gdal_GDALRasterBand_GetBlockSize(GDALRasterBand * handle, int * xSize, int * ySize);
	GDALAccess bmx_gdal_GDALRasterBand_GetAccess(GDALRasterBand * handle);
	CPLErr bmx_gdal_GDALRasterBand_FlushCache(GDALRasterBand * handle);
	BBArray * bmx_gdal_GDALRasterBand_GetCategoryNames(GDALRasterBand * handle);
	double bmx_gdal_GDALRasterBand_GetScale(GDALRasterBand * handle, int * success);
	BBString * bmx_gdal_GDALRasterBand_GetUnitType(GDALRasterBand * handle);
	CPLErr bmx_gdal_GDALRasterBand_Fill(GDALRasterBand * handle, double realValue, double imaginaryValue);
	CPLErr bmx_gdal_GDALRasterBand_SetNoDataValue(GDALRasterBand * handle, double value);
	CPLErr bmx_gdal_GDALRasterBand_SetColorInterpretation(GDALRasterBand * handle, GDALColorInterp interp);
	CPLErr bmx_gdal_GDALRasterBand_SetOffset(GDALRasterBand * handle, double offset);
	CPLErr bmx_gdal_GDALRasterBand_SetScale(GDALRasterBand * handle, double scale);
	CPLErr bmx_gdal_GDALRasterBand_SetUnitType(GDALRasterBand * handle, BBString * unitType);
	int bmx_gdal_GDALRasterBand_HasArbitraryOverviews(GDALRasterBand * handle);
	int bmx_gdal_GDALRasterBand_GetOverviewCount(GDALRasterBand * handle);
	GDALDataset * bmx_gdal_GDALRasterBand_GetDataset(GDALRasterBand * handle);
	CPLErr bmx_gdal_GDALRasterBand_SetCategoryNames(GDALRasterBand * handle, BBArray * names);
	CPLErr bmx_gdal_GDALRasterBand_GetStatistics(GDALRasterBand * handle, int approxOK, int force, double * _min, double * _max, double * _mean, double * _stddev);
	CPLErr bmx_gdal_GDALRasterBand_SetStatistics(GDALRasterBand * handle, double _min, double _max, double _mean, double _stddev);
	GDALRasterBand * bmx_gdal_GDALRasterBand_GetOverview(GDALRasterBand * handle, int index);
	GDALRasterBand * bmx_gdal_GDALRasterBand_GetRasterSampleOverview(GDALRasterBand * handle, int desiredSamples);
	CPLErr bmx_gdal_GDALRasterBand_CreateMaskBand(GDALRasterBand * handle, int flags);
	CPLErr bmx_gdal_GDALRasterBand_RasterIO(GDALRasterBand * handle, GDALRWFlag rwFlag, int xOff, int yOff, int xSize, int ySize, void * data, int bufXSize, int bufYSize,
			GDALDataType bufType, int pixelSpace, int lineSpace);
	CPLErr bmx_gdal_GDALRasterBand_ReadBlock(GDALRasterBand * handle, int xBlockOff, int yBlockOff, void * image);
	CPLErr bmx_gdal_GDALRasterBand_WriteBlock(GDALRasterBand * handle, int xBlockOff, int yBlockOff, void * image);
	GDALColorTable * bmx_gdal_GDALRasterBand_GetColorTable(GDALRasterBand * handle);

	int bmx_gdal_GDALDriverManager_GetDriverCount();
	GDALDriver * bmx_gdal_GDALDriverManager_GetDriver(int index);
	GDALDriver * bmx_gdal_GDALDriverManager_GetDriverByName(BBString * name);

	BBString * bmx_gdal_GDALDriver_GetShortName(GDALDriver * handle);
	BBString * bmx_gdal_GDALDriver_GetLongName(GDALDriver * handle);
	GDALDataset * bmx_gdal_GDALDriver_CreateCopy(GDALDriver * handle, BBString * filename, GDALDataset * sourceDataset, int strict, BBArray * options);
	CPLErr bmx_gdal_GDALDriver_DeleteDataset(GDALDriver * handle, BBString * filename);
	CPLErr bmx_gdal_GDALDriver_RenameDataset(GDALDriver * handle, BBString * newName, BBString * oldName);
	CPLErr bmx_gdal_GDALDriver_CopyFiles(GDALDriver * handle, BBString * newName, BBString * oldName);
	GDALDataset * bmx_gdal_GDALDriver_CreateDataset(GDALDriver * handle, BBString * filename, int xSize, int ySize, int bands, GDALDataType dataType, BBArray * paramList);

	GDAL_GCP * bmx_gdal_GDAL_GCP_create(BBString * id, BBString * info, double pixel, double line, double x, double y, double z);
	BBString * bmx_gdal_GDAL_GCP_GetID(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetId(GDAL_GCP * handle, BBString * id);
	BBString * bmx_gdal_GDAL_GCP_GetInfo(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetInfo(GDAL_GCP * handle, BBString * info);
	double bmx_gdal_GDAL_GCP_GetPixel(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetPixel(GDAL_GCP * handle, double pixel);
	double bmx_gdal_GDAL_GCP_GetLine(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetLine(GDAL_GCP * handle, double line);
	double bmx_gdal_GDAL_GCP_GetX(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetX(GDAL_GCP * handle, double x);
	double bmx_gdal_GDAL_GCP_GetY(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetY(GDAL_GCP * handle, double y);
	double bmx_gdal_GDAL_GCP_GetZ(GDAL_GCP * handle);
	void bmx_gdal_GDAL_GCP_SetZ(GDAL_GCP * handle, double z);
	void bmx_gdal_GDAL_GCP_free(GDAL_GCP * handle);

	OGRSpatialReference * bmx_gdal_OGRSpatialReference_create(BBString * ref);
	void bmx_gdal_OGRSpatialReference_free(OGRSpatialReference * handle);
	OGRSpatialReference * bmx_gdal_OGRSpatialReference_Clone(OGRSpatialReference * handle);
	OGRSpatialReference * bmx_gdal_OGRSpatialReference_CloneGeogCS(OGRSpatialReference * handle);
	BBString * bmx_gdal_OGRSpatialReference_ExportToWkt(OGRSpatialReference * handle, OGRErr * result);
	OGRErr bmx_gdal_OGRSpatialReference_morphToESRI(OGRSpatialReference * handle);
	OGRErr bmx_gdal_OGRSpatialReference_morphFromESRI(OGRSpatialReference * handle);
	OGRErr bmx_gdal_OGRSpatialReference_Validate(OGRSpatialReference * handle);
	OGRErr bmx_gdal_OGRSpatialReference_FixupOrdering(OGRSpatialReference * handle);
	OGRErr bmx_gdal_OGRSpatialReference_Fixup(OGRSpatialReference * handle);
	int bmx_gdal_OGRSpatialReference_EPSGTreatsAsLatLong(OGRSpatialReference * handle);
	OGRErr bmx_gdal_OGRSpatialReference_SetLinearUnitsAndUpdateParameters(OGRSpatialReference * handle, BBString * name, double inMeters);
	OGRErr bmx_gdal_OGRSpatialReference_SetLinearUnits(OGRSpatialReference * handle, BBString * name, double inMeters);
	BBString * bmx_gdal_OGRSpatialReference_GetLinearUnits(OGRSpatialReference * handle, double * units);
	OGRErr bmx_gdal_OGRSpatialReference_SetAngularUnits(OGRSpatialReference * handle, BBString * name, double inDegrees);
	BBString * bmx_gdal_OGRSpatialReference_GetAngularUnits(OGRSpatialReference * handle, double * units);
	BBString * bmx_gdal_OGRSpatialReference_GetPrimeMeridian(OGRSpatialReference * handle, double * meridian);
	int bmx_gdal_OGRSpatialReference_IsGeographic(OGRSpatialReference * handle);
	int bmx_gdal_OGRSpatialReference_IsProjected(OGRSpatialReference * handle);
	int bmx_gdal_OGRSpatialReference_IsLocal(OGRSpatialReference * handle);
	void bmx_gdal_OGRSpatialReference_Clear(OGRSpatialReference * handle);
	OGRErr bmx_gdal_OGRSpatialReference_SetLocalCS(OGRSpatialReference * handle, BBString * name);
	OGRErr bmx_gdal_OGRSpatialReference_SetProjCS(OGRSpatialReference * handle, BBString * name);
	OGRErr bmx_gdal_OGRSpatialReference_SetProjection(OGRSpatialReference * handle, BBString * name);
	OGRErr bmx_gdal_OGRSpatialReference_SetWellKnownGeogCS(OGRSpatialReference * handle, BBString * name);
	OGRErr bmx_gdal_OGRSpatialReference_SetFromUserInput(OGRSpatialReference * handle, BBString * definition);
	OGRErr bmx_gdal_OGRSpatialReference_GetSemiMajor(OGRSpatialReference * handle, double * semiMajor);
	OGRErr bmx_gdal_OGRSpatialReference_GetSemiMinor(OGRSpatialReference * handle, double * semiMinor);
	OGRErr bmx_gdal_OGRSpatialReference_GetInvFlattening(OGRSpatialReference * handle, double * invFlattening);
	OGRErr bmx_gdal_OGRSpatialReference_SetAuthority(OGRSpatialReference * handle, BBString * targetKey, BBString * authority, int code);
	OGRErr bmx_gdal_OGRSpatialReference_AutoIdentifyEPSG(OGRSpatialReference * handle);
	BBString * bmx_gdal_OGRSpatialReference_GetAuthorityCode(OGRSpatialReference * handle, BBString * targetKey);
	BBString * bmx_gdal_OGRSpatialReference_GetAuthorityName(OGRSpatialReference * handle, BBString * targetKey);
	BBString * bmx_gdal_OGRSpatialReference_GetExtension(OGRSpatialReference * handle, BBString * targetKey, BBString * name, BBString * defaultValue);
	OGRErr bmx_gdal_OGRSpatialReference_SetExtension(OGRSpatialReference * handle, BBString * targetKey, BBString * name, BBString * value);
	OGRErr bmx_gdal_OGRSpatialReference_SetProjParm(OGRSpatialReference * handle, BBString * name, double value);
	OGRErr bmx_gdal_OGRSpatialReference_GetProjParm(OGRSpatialReference * handle, BBString * name, double * result, double defaultValue);
	OGRErr bmx_gdal_OGRSpatialReference_SetNormProjParm(OGRSpatialReference * handle, BBString * name, double value);
	OGRErr bmx_gdal_OGRSpatialReference_GetNormProjParm(OGRSpatialReference * handle, BBString * name, double * result, double defaultValue);
	OGRErr bmx_gdal_OGRSpatialReference_SetUTM(OGRSpatialReference * handle, int zone, int north);
	int bmx_gdal_OGRSpatialReference_GetUTMZone(OGRSpatialReference * handle, int * north);
	int bmx_gdal_OGRSpatialReference_IsSameGeogCS(OGRSpatialReference * handle, const OGRSpatialReference * ref);
	int bmx_gdal_OGRSpatialReference_IsSame(OGRSpatialReference * handle, const OGRSpatialReference * ref);
	OGRErr bmx_gdal_OGRSpatialReference_SetGeogCS(OGRSpatialReference * handle, BBString * geogName, BBString * datumName, BBString * ellipsoidName, double semiMajor, 
			double invFlattening, BBString * pmName, double pmOffset, BBString * units, double convertToRadians);
	OGRErr bmx_gdal_OGRSpatialReference_CopyGeogCSFrom(OGRSpatialReference * handle, OGRSpatialReference * srcSRS);
	OGRErr bmx_gdal_OGRSpatialReference_SetTOWGS84(OGRSpatialReference * handle, double dx, double dy, double dz, double ex, double ey, double ez, double ppm);
	OGRErr bmx_gdal_OGRSpatialReference_GetTOWGS84(OGRSpatialReference * handle, BBArray * coef);

	void bmx_gdal_OGRRegisterAll();
	void bmx_gdal_OGRCleanupAll();

	int bmx_gdal_OGRSFDriverRegistrar_GetDriverCount();
	OGRSFDriver * bmx_gdal_OGRSFDriverRegistrar_GetDriver(int index);
	OGRSFDriver * bmx_gdal_OGRSFDriverRegistrar_GetDriverByName(BBString * name);
	int bmx_gdal_OGRSFDriverRegistrar_GetOpenDSCount();
	OGRDataSource * bmx_gdal_OGRSFDriverRegistrar_GetOpenDS(int ids);

	OGRDataSource * bmx_gdal_OGRSFDriver_CreateDataSource(OGRSFDriver * handle, BBString * name, BBArray * options);
	OGRDataSource * bmx_gdal_OGRSFDriver_Open(OGRSFDriver * handle, BBString * name, int update);
	int bmx_gdal_OGRSFDriver_TestCapability(OGRSFDriver * handle, BBString * capability);
	OGRErr bmx_gdal_OGRSFDriver_DeleteDataSource(OGRSFDriver * handle, BBString * datasource);
	BBString * bmx_gdal_OGRSFDriver_GetName(OGRSFDriver * handle);

	OGRLayer * bmx_gdal_OGRDataSource_CreateLayer(OGRDataSource * handle, BBString * name, OGRSpatialReference * spatialRef, OGRwkbGeometryType gType, BBArray * options);
	void bmx_gdal_OGRDataSource_free(OGRDataSource * handle);
	BBString * bmx_gdal_OGRDataSource_GetName(OGRDataSource * handle);
	int bmx_gdal_OGRDataSource_GetLayerCount(OGRDataSource * handle);
	OGRLayer * bmx_gdal_OGRDataSource_GetLayer(OGRDataSource * handle, int index);
	OGRLayer * bmx_gdal_OGRDataSource_GetLayerByName(OGRDataSource * handle, BBString * name);
	OGRErr bmx_gdal_OGRDataSource_DeleteLayer(OGRDataSource * handle, int layer);
	int bmx_gdal_OGRDataSource_TestCapability(OGRDataSource * handle, BBString * name);

	OGRFieldDefn * bmx_gdal_OGRFieldDefn_create(BBString * name, OGRFieldType fieldType);
	void bmx_gdal_OGRFieldDefn_free(OGRFieldDefn * handle);
	void bmx_gdal_OGRFieldDefn_SetWidth(OGRFieldDefn * handle, int width);
	void bmx_gdal_OGRFieldDefn_SetPrecision(OGRFieldDefn * handle, int precision);
	void bmx_gdal_OGRFieldDefn_SetName(OGRFieldDefn * handle, BBString * name);
	BBString * bmx_gdal_OGRFieldDefn_GetNameRef(OGRFieldDefn * handle);
	OGRFieldType bmx_gdal_OGRFieldDefn_GetType(OGRFieldDefn * handle);
	void bmx_gdal_OGRFieldDefn_SetType(OGRFieldDefn * handle, OGRFieldType fieldType);
	OGRJustification bmx_gdal_OGRFieldDefn_GetJustify(OGRFieldDefn * handle);
	void bmx_gdal_OGRFieldDefn_SetJustify(OGRFieldDefn * handle, OGRJustification justify);
	int bmx_gdal_OGRFieldDefn_GetWidth(OGRFieldDefn * handle);
	int bmx_gdal_OGRFieldDefn_GetPrecision(OGRFieldDefn * handle);
	void bmx_gdal_OGRFieldDefn_Set(OGRFieldDefn * handle, BBString * name, OGRFieldType fieldType, int width, int precision, OGRJustification justify);

	OGRErr bmx_gdal_OGRLayer_CreateField(OGRLayer * handle, OGRFieldDefn * fld, int approxOK);


	VRTDataset * bmx_gdal_VRTDataset_create(int width, int height);
	CPLErr bmx_gdal_VRTDataset_AddBand(VRTDataset * handle, GDALDataType dataType, BBArray * options);
	
	CPLErr bmx_gdal_VRTRasterBand_CopyCommonInfoFrom(VRTRasterBand * handle, GDALRasterBand * source);

	CPLErr bmx_gdal_VRTSourcedRasterBand_AddSimpleSource(VRTSourcedRasterBand * handle, GDALRasterBand *poSrcBand, int nSrcXOff, int nSrcYOff, 
		int nSrcXSize, int nSrcYSize, int nDstXOff, int nDstYOff, int nDstXSize, int nDstYSize,
		BBString * pszResampling, double dfNoDataValue);

	GDALPaletteInterp bmx_gdal_GDALColorTable_GetPaletteInterpretation(GDALColorTable * handle);
	int bmx_gdal_GDALColorTable_GetColorEntryCount(GDALColorTable * handle);

	char** bmx_bbStringArrayToStringList(BBArray * data);
	void bmx_StringListFree(char** list);
	BBArray * bbStringArrayFromStringList(char** list);
	
}
