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

// *****************************************************

char** bmx_bbStringArrayToStringList(BBArray * data) {

	char** list = NULL;

	int n = data->scales[0];
	
	if (n > 0) {
		BBString **s=(BBString**)BBARRAYDATA( data, data->dims );
	
		char** list = (char **)CPLMalloc((n+1)*sizeof(char*));
		char** c = list;
		
		for( int i=0;i<n;++i ){
			*c = bbStringToCString(s[i]);
			c++;
		}
	}

	return list;
}

void bmx_StringListFree(char** list) {
    char **c;

    if (list)
    {
        c = list;
        while(*c != NULL)
        {
            bbMemFree(*c);
            c++;
        }

        CPLFree(list);
    }
}


// *****************************************************

GDALDataset * bmx_gdal_GDALOpen(BBString * filename, GDALAccess access) {
	char *f = bbStringToCString( filename );
	GDALDataset* dataset = (GDALDataset*)GDALOpen(f, access);
	bbMemFree(f);
	return dataset;
}

GDALDataset * bmx_gdal_GDALOpenShared(BBString * filename, GDALAccess access) {
	char *f = bbStringToCString( filename );
	GDALDataset* dataset = (GDALDataset*)GDALOpenShared(f, access);
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

BBArray * bmx_gdal_GDALMajorObject_GetMetadata(GDALMajorObject * handle, BBString * domain) {

	char *d = bbStringToCString( domain );
	
	char ** meta = handle->GetMetadata(d);
	int count = CSLCount(meta);
	BBArray *p = 0;
	
	if (count > 0) {
		p = bbArrayNew1D( "$",count );
		BBString **s=(BBString**)BBARRAYDATA( p,p->dims );
		for( int i = 0 ; i < count ; ++i ){
			s[i] = bbStringFromCString( meta[i] );
			BBRETAIN( s[i] );
		}
	} else {
		p = &bbEmptyArray;
	}

	bbMemFree(d);
	
	return p;
}

CPLErr bmx_gdal_GDALMajorObject_SetMetadataItem(GDALDataset * handle, BBString * name, BBString * value, BBString * domain) {
	char *n = bbStringToCString( name );
	char *v = bbStringToCString( value );
	char *d = bbStringToCString( domain );
	
	CPLErr res = handle->SetMetadataItem(n, v, d);
	
	bbMemFree(n);
	bbMemFree(v);
	bbMemFree(d);
	return res;
}

CPLErr bmx_gdal_GDALMajorObject_SetMetadata(GDALDataset * handle, BBArray * metadata, BBString * domain) {

	char *d = bbStringToCString( domain );
	char** list = bmx_bbStringArrayToStringList(metadata);
	
	CPLErr res = CE_None;
	
	if (list) {
		res = handle->SetMetadata(list, d);
		bmx_StringListFree(list);
	}
	bbMemFree(d);

	return res;

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

CPLErr bmx_gdal_GDALDataset_SetProjection(GDALDataset * handle, BBString * projection) {
	char *p = bbStringToCString( projection );
	CPLErr res = handle->SetProjection(p);
	bbMemFree(p);
	return res;
}

CPLErr bmx_gdal_GDALDataset_SetGeoTransform(GDALDataset * handle, BBArray * transform) {
	double *d=(double*)BBARRAYDATA( transform, transform->dims );
	return handle->SetGeoTransform(d);
}

int bmx_gdal_GDALDataset_GetGCPCount(GDALDataset * handle) {
	return handle->GetGCPCount();
}


// *****************************************************

CPLErr bmx_gdal_GDALRasterBand_GenerateContour(GDALRasterBand * handle, double contourInterval, double contourBase, BBArray * fixedLevels,
		int useNoData, double noDataValue, OGRLayer * layer, int idField, int elevField) {

	int count = fixedLevels->scales[0];
	
	CPLErr res = GDALContourGenerate(handle, contourInterval, contourBase, count, (count > 0) ? (double*)BBARRAYDATA( fixedLevels, fixedLevels->dims ) : NULL, 
		useNoData, noDataValue, layer, idField, elevField, NULL, NULL);
	
	return res;
}

int bmx_gdal_GDALRasterBand_GetXSize(GDALRasterBand * handle) {
	return handle->GetXSize();
}

int bmx_gdal_GDALRasterBand_GetYSize(GDALRasterBand * handle) {
	return handle->GetYSize();
}

int bmx_gdal_GDALRasterBand_GetBand(GDALRasterBand * handle) {
	return handle->GetBand();
}

GDALColorInterp bmx_gdal_GDALRasterBand_GetColorInterpretation(GDALRasterBand * handle) {
	return handle->GetColorInterpretation();
}

int bmx_gdal_GDALRasterBand_GetMaskFlags(GDALRasterBand * handle) {
	return handle->GetMaskFlags();
}

GDALDataType bmx_gdal_GDALRasterBand_GetRasterDataType(GDALRasterBand * handle) {
	return handle->GetRasterDataType();
}

double bmx_gdal_GDALRasterBand_GetMaximum(GDALRasterBand * handle, int * success) {
	return handle->GetMaximum(success);
}

double bmx_gdal_GDALRasterBand_GetMinimum(GDALRasterBand * handle, int * success) {
	return handle->GetMinimum(success);
}

double bmx_gdal_GDALRasterBand_GetNoDataValue(GDALRasterBand * handle, int * success) {
	return handle->GetNoDataValue(success);
}

double bmx_gdal_GDALRasterBand_GetOffset(GDALRasterBand * handle, int * success) {
	return handle->GetOffset(success);
}


// *****************************************************

int bmx_gdal_GDALDriverManager_GetDriverCount() {
	return GetGDALDriverManager()->GetDriverCount();
}

GDALDriver * bmx_gdal_GDALDriverManager_GetDriver(int index) {
	return GetGDALDriverManager()->GetDriver(index);
}

GDALDriver * bmx_gdal_GDALDriverManager_GetDriverByName(BBString * name) {
	char *n = bbStringToCString( name );
	GDALDriver * driver = GetGDALDriverManager()->GetDriverByName(n);
	bbMemFree(n);
	return driver;
}


// *****************************************************

BBString * bmx_gdal_GDALDriver_GetShortName(GDALDriver * handle) {
	return bbStringFromCString(GDALGetDriverShortName(handle));
}

BBString * bmx_gdal_GDALDriver_GetLongName(GDALDriver * handle) {
	return bbStringFromCString(GDALGetDriverLongName(handle));
}

GDALDataset * bmx_gdal_GDALDriver_CreateCopy(GDALDriver * handle, BBString * filename, GDALDataset * sourceDataset, int strict, BBArray * options) {
	char *n = bbStringToCString( filename );
	
	GDALDataset * dataset = 0;
	
	if (options && (options->scales[0] > 0)) {
		char** list = bmx_bbStringArrayToStringList(options);
		dataset = handle->CreateCopy(n, sourceDataset, strict, list, NULL, NULL);
		bmx_StringListFree(list);
	} else {
		dataset = handle->CreateCopy(n, sourceDataset, strict, NULL, NULL, NULL);
	}
	
	bbMemFree(n);
	return dataset;
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

int bmx_gdal_OGRSFDriverRegistrar_GetOpenDSCount() {
	return OGRSFDriverRegistrar::GetRegistrar()->GetOpenDSCount();
}


// *****************************************************

OGRDataSource * bmx_gdal_OGRSFDriver_CreateDataSource(OGRSFDriver * handle, BBString * name, BBArray * options) {
	OGRDataSource * source = 0;
	
	char *n = bbStringToCString( name );
	
	if (options && (options->scales[0] > 0)) {
		char** list = bmx_bbStringArrayToStringList(options);
		source = handle->CreateDataSource(n, list);
		bmx_StringListFree(list);
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
		char** list = bmx_bbStringArrayToStringList(options);
		layer = handle->CreateLayer(n, spatialRef, gType, list);
		bmx_StringListFree(list);
	} else {
		layer = handle->CreateLayer(n, spatialRef, gType, NULL);
	}
	
	bbMemFree(n);
	return layer;
}

void bmx_gdal_OGRDataSource_free(OGRDataSource * handle) {
	OGR_DS_Destroy(handle);
}

BBString * bmx_gdal_OGRDataSource_GetName(OGRDataSource * handle) {
	return bbStringFromCString(handle->GetName());
}

int bmx_gdal_OGRDataSource_GetLayerCount(OGRDataSource * handle) {
	return handle->GetLayerCount();
}

OGRLayer * bmx_gdal_OGRDataSource_GetLayer(OGRDataSource * handle, int index) {
	return handle->GetLayer(index);
}

OGRLayer * bmx_gdal_OGRDataSource_GetLayerByName(OGRDataSource * handle, BBString * name) {
	char *n = bbStringToCString( name );
	OGRLayer * layer = handle->GetLayerByName(n);
	
	bbMemFree(n);
	return layer;
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


// *****************************************************

VRTDataset * bmx_gdal_VRTDataset_create(int width, int height) {
	return new VRTDataset(width, height);
}

CPLErr bmx_gdal_VRTDataset_AddBand(VRTDataset * handle, GDALDataType dataType, BBArray * options) {
	CPLErr res = CE_None;
	if (options && (options->scales[0] > 0)) {
		char** list = bmx_bbStringArrayToStringList(options);
		res = handle->AddBand(dataType, list);
		bmx_StringListFree(list);
	} else {
		res = handle->AddBand(dataType, NULL);
	}
	
	return res;
}

// *****************************************************

CPLErr bmx_gdal_VRTRasterBand_CopyCommonInfoFrom(VRTRasterBand * handle, GDALRasterBand * source) {
	return handle->CopyCommonInfoFrom(source);
}

// *****************************************************

CPLErr bmx_gdal_VRTSourcedRasterBand_AddSimpleSource(VRTSourcedRasterBand * handle, GDALRasterBand *poSrcBand, int nSrcXOff, int nSrcYOff, 
		int nSrcXSize, int nSrcYSize, int nDstXOff, int nDstYOff, int nDstXSize, int nDstYSize,
		BBString * pszResampling, double dfNoDataValue) {

	char *r = bbStringToCString( pszResampling );

	CPLErr res = handle->AddSimpleSource(poSrcBand, nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize, nDstXOff, nDstYOff, nDstXSize, nDstYSize, r, dfNoDataValue);

	bbMemFree(r);
	return res;
}


