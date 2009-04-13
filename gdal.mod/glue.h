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

	GDALDataset * bmx_gdal_GDALOpen(BBString * filename, GDALAccess access);
	GDALDataset * bmx_gdal_GDALOpenShared(BBString * filename, GDALAccess access);
	void bmx_gdal_GDALAllRegister();
	
	BBString * bmx_gdal_GDALMajorObject_GetDescription(GDALMajorObject * handle);
	BBString * bmx_gdal_GDALMajorObject_GetMetadataItem(GDALMajorObject * handle, BBString * name, BBString * domain);
	BBArray * bmx_gdal_GDALMajorObject_GetMetadata(GDALMajorObject * handle, BBString * domain);
	CPLErr bmx_gdal_GDALMajorObject_SetMetadataItem(GDALDataset * handle, BBString * name, BBString * value, BBString * domain);
	CPLErr bmx_gdal_GDALMajorObject_SetMetadata(GDALDataset * handle, BBArray * metadata, BBString * domain);

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

	int bmx_gdal_GDALDriverManager_GetDriverCount();
	GDALDriver * bmx_gdal_GDALDriverManager_GetDriver(int index);
	GDALDriver * bmx_gdal_GDALDriverManager_GetDriverByName(BBString * name);

	BBString * bmx_gdal_GDALDriver_GetShortName(GDALDriver * handle);
	BBString * bmx_gdal_GDALDriver_GetLongName(GDALDriver * handle);
	GDALDataset * bmx_gdal_GDALDriver_CreateCopy(GDALDriver * handle, BBString * filename, GDALDataset * sourceDataset, int strict, BBArray * options);

	OGRSpatialReference * bmx_gdal_OGRSpatialReference_create(BBString * ref);
	void bmx_gdal_OGRSpatialReference_free(OGRSpatialReference * handle);

	void bmx_gdal_OGRRegisterAll();
	void bmx_gdal_OGRCleanupAll();

	int bmx_gdal_OGRSFDriverRegistrar_GetDriverCount();
	OGRSFDriver * bmx_gdal_OGRSFDriverRegistrar_GetDriver(int index);
	OGRSFDriver * bmx_gdal_OGRSFDriverRegistrar_GetDriverByName(BBString * name);
	int bmx_gdal_OGRSFDriverRegistrar_GetOpenDSCount();

	OGRDataSource * bmx_gdal_OGRSFDriver_CreateDataSource(OGRSFDriver * handle, BBString * name, BBArray * options);

	OGRLayer * bmx_gdal_OGRDataSource_CreateLayer(OGRDataSource * handle, BBString * name, OGRSpatialReference * spatialRef, OGRwkbGeometryType gType, BBArray * options);
	void bmx_gdal_OGRDataSource_free(OGRDataSource * handle);
	BBString * bmx_gdal_OGRDataSource_GetName(OGRDataSource * handle);
	int bmx_gdal_OGRDataSource_GetLayerCount(OGRDataSource * handle);
	OGRLayer * bmx_gdal_OGRDataSource_GetLayer(OGRDataSource * handle, int index);
	OGRLayer * bmx_gdal_OGRDataSource_GetLayerByName(OGRDataSource * handle, BBString * name);

	OGRFieldDefn * bmx_gdal_OGRFieldDefn_create(BBString * name, OGRFieldType fieldType);
	void bmx_gdal_OGRFieldDefn_free(OGRFieldDefn * handle);
	void bmx_gdal_OGRFieldDefn_SetWidth(OGRFieldDefn * handle, int width);
	void bmx_gdal_OGRFieldDefn_SetPrecision(OGRFieldDefn * handle, int precision);

	OGRErr bmx_gdal_OGRLayer_CreateField(OGRLayer * handle, OGRFieldDefn * fld, int approxOK);


	VRTDataset * bmx_gdal_VRTDataset_create(int width, int height);
	CPLErr bmx_gdal_VRTDataset_AddBand(VRTDataset * handle, GDALDataType dataType, BBArray * options);
	
	CPLErr bmx_gdal_VRTRasterBand_CopyCommonInfoFrom(VRTRasterBand * handle, GDALRasterBand * source);

	CPLErr bmx_gdal_VRTSourcedRasterBand_AddSimpleSource(VRTSourcedRasterBand * handle, GDALRasterBand *poSrcBand, int nSrcXOff, int nSrcYOff, 
		int nSrcXSize, int nSrcYSize, int nDstXOff, int nDstYOff, int nDstXSize, int nDstYSize,
		BBString * pszResampling, double dfNoDataValue);

	char** bmx_bbStringArrayToStringList(BBArray * data);
	void bmx_StringListFree(char** list);

}
