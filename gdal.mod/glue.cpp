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

#include "glue.h"


GDALDataset * bmx_gdal_GDALOpen(BBString * filename, GDALAccess access) {
	char *f = bbStringToCString( filename );
	GDALDataset* dataset = (GDALDataset*)GDALOpen(f, access);
	bbMemFree(f);
	return dataset;
}

void bmx_gdal_GDALAllRegister() {
	GDALAllRegister();
}


// *****************************************************

BBString * bmx_gdal_GDALMajorObject_GetDescription(GDALMajorObject * handle) {
	return bbStringFromCString(handle->GetDescription());
}

BBString * bmx_gdal_GDALMajorObject_GetMetadataItem(GDALMajorObject * handle, BBString * name, BBString * domain) {
	char *n = bbStringToCString( name );
	char *d = bbStringToCString( domain );
	BBString * item = bbStringFromCString(handle->GetMetadataItem(n, d));
	bbMemFree(n);
	bbMemFree(d);
	return item;
}

// *****************************************************

GDALDriver * bmx_gdal_GDALDataset_GetDriver(GDALDataset * handle) {
	return handle->GetDriver();
}

int bmx_gdal_GDALDataset_GetRasterXSize(GDALDataset * handle) {
	return handle->GetRasterXSize();
}

int bmx_gdal_GDALDataset_GetRasterYSize(GDALDataset * handle) {
	return handle->GetRasterYSize();
}

int bmx_gdal_GDALDataset_GetRasterCount(GDALDataset * handle) {
	return handle->GetRasterCount();
}

CPLErr bmx_gdal_GDALDataset_GetGeoTransform(GDALDataset * handle, BBArray * padfTransform) {
	double *d=(double*)BBARRAYDATA( padfTransform, padfTransform->dims );
	return handle->GetGeoTransform(d);
}

BBString * bmx_gdal_GDALDataset_GetProjectionRef(GDALDataset * handle) {
	const char * ref = handle->GetProjectionRef();
	if (ref) {
		return bbStringFromCString(ref);
	} else {
		return &bbEmptyString;
	}
}

GDALRasterBand * bmx_gdal_GDALDataset_GetRasterBand(GDALDataset * handle, int index) {
	return handle->GetRasterBand(index);
}

void bmx_gdal_GDALDataset_Close(GDALDataset * handle) {
	GDALClose(handle);
}

// *****************************************************

CPLErr bmx_gdal_GDALRasterBand_GenerateContour(GDALRasterBand * handle, double contourInterval, double contourBase, BBArray * fixedLevels,
		int useNoData, double noDataValue, OGRLayer * layer, int idField, int elevField) {

	int count = fixedLevels->scales[0];
	
	CPLErr res = GDALContourGenerate(handle, contourInterval, contourBase, count, (count > 0) ? (double*)BBARRAYDATA( fixedLevels, fixedLevels->dims ) : NULL, 
		useNoData, noDataValue, layer, idField, elevField, NULL, NULL);
	
	return res;
}


// *****************************************************

void bmx_gdal_OGRRegisterAll() {
	OGRRegisterAll();
}

void bmx_gdal_OGRCleanupAll() {
	OGRCleanupAll();
}

// *****************************************************

OGRSpatialReference * bmx_gdal_OGRSpatialReference_create(BBString * ref) {
	char *r = bbStringToCString( ref );
	OGRSpatialReference * srs = new OGRSpatialReference(r);
	bbMemFree(r);
	return srs;
}

void bmx_gdal_OGRSpatialReference_free(OGRSpatialReference * handle) {
	delete handle;
}


// *****************************************************

int bmx_gdal_OGRSFDriverRegistrar_GetDriverCount() {
	return OGRSFDriverRegistrar::GetRegistrar()->GetDriverCount();
}

OGRSFDriver * bmx_gdal_OGRSFDriverRegistrar_GetDriver(int index) {
	return OGRSFDriverRegistrar::GetRegistrar()->GetDriver(index);
}

OGRSFDriver * bmx_gdal_OGRSFDriverRegistrar_GetDriverByName(BBString * name) {
	char *n = bbStringToCString( name );
	OGRSFDriver * driver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(n);
	bbMemFree(n);
	return driver;
}

// *****************************************************

OGRDataSource * bmx_gdal_OGRSFDriver_CreateDataSource(OGRSFDriver * handle, BBString * name, BBArray * options) {
	OGRDataSource * source = 0;
	
	char *n = bbStringToCString( name );
	
	if (options && (options->scales[0] > 0)) {
		// TODO
	} else {
		source = handle->CreateDataSource(n, NULL);
	}
	
	bbMemFree(n);
	return source;
}


// *****************************************************

OGRLayer * bmx_gdal_OGRDataSource_CreateLayer(OGRDataSource * handle, BBString * name, OGRSpatialReference * spatialRef, OGRwkbGeometryType gType, BBArray * options) {
	OGRLayer * layer = 0;
	
	char *n = bbStringToCString( name );
	
	if (options && (options->scales[0] > 0)) {
		// TODO
	} else {
		layer = handle->CreateLayer(n, spatialRef, gType, NULL);
	}
	
	bbMemFree(n);
	return layer;
}

void bmx_gdal_OGRDataSource_free(OGRDataSource * handle) {
	OGR_DS_Destroy(handle);
}

// *****************************************************

OGRFieldDefn * bmx_gdal_OGRFieldDefn_create(BBString * name, OGRFieldType fieldType) {

	char *n = bbStringToCString( name );

	OGRFieldDefn * def =  new OGRFieldDefn(n, fieldType);
	
	bbMemFree(n);
	return def;
}

void bmx_gdal_OGRFieldDefn_free(OGRFieldDefn * handle) {
	delete handle;
}

void bmx_gdal_OGRFieldDefn_SetWidth(OGRFieldDefn * handle, int width) {
	handle->SetWidth(width);
}

void bmx_gdal_OGRFieldDefn_SetPrecision(OGRFieldDefn * handle, int precision) {
	handle->SetPrecision(precision);
}

// *****************************************************

OGRErr bmx_gdal_OGRLayer_CreateField(OGRLayer * handle, OGRFieldDefn * fld, int approxOK) {
	return handle->CreateField(fld, approxOK);
}




