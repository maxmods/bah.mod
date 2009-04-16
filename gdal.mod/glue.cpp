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

BBArray * bbStringArrayFromStringList(char** list) {
	int count = CSLCount(list);
	BBArray *p = 0;
	
	if (count > 0) {
		p = bbArrayNew1D( "$",count );
		BBString **s=(BBString**)BBARRAYDATA( p,p->dims );
		for( int i = 0 ; i < count ; ++i ){
			s[i] = bbStringFromCString( list[i] );
			BBRETAIN( s[i] );
		}
	} else {
		p = &bbEmptyArray;
	}

	return p;
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

CPLErr bmx_gdal_GDALMajorObject_SetMetadataItem(GDALMajorObject * handle, BBString * name, BBString * value, BBString * domain) {
	char *n = bbStringToCString( name );
	char *v = bbStringToCString( value );
	char *d = bbStringToCString( domain );
	
	CPLErr res = handle->SetMetadataItem(n, v, d);
	
	bbMemFree(n);
	bbMemFree(v);
	bbMemFree(d);
	return res;
}

CPLErr bmx_gdal_GDALMajorObject_SetMetadata(GDALMajorObject * handle, BBArray * metadata, BBString * domain) {

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

void bmx_gdal_GDALMajorObject_SetDescription(GDALMajorObject * handle, BBString * description) {
	char *d = bbStringToCString( description );
	handle->SetDescription(d);
	bbMemFree(d);
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

void bmx_gdal_GDALDataset_FlushCache(GDALDataset * handle) {
	handle->FlushCache();
}

CPLErr bmx_gdal_GDALDataset_AddBand(GDALDataset * handle, GDALDataType dataType, BBArray * options) {
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

BBArray * bmx_gdal_GDALDataset_GetFileList(GDALDataset * handle) {
	char ** list = handle->GetFileList();
	BBArray * p = bbStringArrayFromStringList(list);
	CSLDestroy(list);	
	return p;
}

BBString * bmx_gdal_GDALDataset_GetGCPProjection(GDALDataset * handle) {
	return bbStringFromCString(handle->GetGCPProjection());
}

CPLErr bmx_gdal_GDALDataset_CreateMaskBand(GDALDataset * handle, int flags) {
	return handle->CreateMaskBand(flags);
}

int bmx_gdal_GDALDataset_GetShared(GDALDataset * handle) {
	return handle->GetShared();
}

void bmx_gdal_GDALDataset_MarkAsShared(GDALDataset * handle) {
	handle->MarkAsShared();
}

void bmx_gdal_GDALDataset_GetGCPs(GDALDataset * handle, BBArray * gcps) {
	int n = gcps->scales[0];
	
	const GDAL_GCP * list = handle->GetGCPs();
	
	for (int i = 0; i < n; i++) {
		_bah_gdal_GDALDataset__setGCP(gcps, i, list);
		list++;
	}
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

void bmx_gdal_GDALRasterBand_GetBlockSize(GDALRasterBand * handle, int * xSize, int * ySize) {
	handle->GetBlockSize(xSize, ySize);
}

GDALAccess bmx_gdal_GDALRasterBand_GetAccess(GDALRasterBand * handle) {
	return handle->GetAccess();
}

CPLErr bmx_gdal_GDALRasterBand_FlushCache(GDALRasterBand * handle) {
	return handle->FlushCache();
}

BBArray * bmx_gdal_GDALRasterBand_GetCategoryNames(GDALRasterBand * handle) {
	char ** list = handle->GetCategoryNames();
	BBArray * p = bbStringArrayFromStringList(list);
	CSLDestroy(list);	
	return p;
}

double bmx_gdal_GDALRasterBand_GetScale(GDALRasterBand * handle, int * success) {
	return handle->GetScale(success);
}

BBString * bmx_gdal_GDALRasterBand_GetUnitType(GDALRasterBand * handle) {
	return bbStringFromCString(handle->GetUnitType());
}

CPLErr bmx_gdal_GDALRasterBand_Fill(GDALRasterBand * handle, double realValue, double imaginaryValue) {
	return handle->Fill(realValue, imaginaryValue);
}

CPLErr bmx_gdal_GDALRasterBand_SetNoDataValue(GDALRasterBand * handle, double value) {
	return handle->SetNoDataValue(value);
}

CPLErr bmx_gdal_GDALRasterBand_SetColorInterpretation(GDALRasterBand * handle, GDALColorInterp interp) {
	return handle->SetColorInterpretation(interp);
}

CPLErr bmx_gdal_GDALRasterBand_SetOffset(GDALRasterBand * handle, double offset) {
	return handle->SetOffset(offset);
}

CPLErr bmx_gdal_GDALRasterBand_SetScale(GDALRasterBand * handle, double scale) {
	return handle->SetScale(scale);
}

CPLErr bmx_gdal_GDALRasterBand_SetUnitType(GDALRasterBand * handle, BBString * unitType) {
	char *t = bbStringToCString( unitType );
	CPLErr res = handle->SetUnitType(t);
	bbMemFree(t);
	return res;
}

int bmx_gdal_GDALRasterBand_HasArbitraryOverviews(GDALRasterBand * handle) {
	return handle->HasArbitraryOverviews();
}

int bmx_gdal_GDALRasterBand_GetOverviewCount(GDALRasterBand * handle) {
	return handle->GetOverviewCount();
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

CPLErr bmx_gdal_GDALDriver_DeleteDataset(GDALDriver * handle, BBString * filename) {
	char *f = bbStringToCString( filename );
	CPLErr res = handle->Delete(f);
	bbMemFree(f);
	return res;
}

CPLErr bmx_gdal_GDALDriver_RenameDataset(GDALDriver * handle, BBString * newName, BBString * oldName) {
	char *n = bbStringToCString( newName );
	char *o = bbStringToCString( oldName );
	CPLErr res = handle->Rename(n, o);
	bbMemFree(n);
	bbMemFree(o);
	return res;
}

CPLErr bmx_gdal_GDALDriver_CopyFiles(GDALDriver * handle, BBString * newName, BBString * oldName) {
	char *n = bbStringToCString( newName );
	char *o = bbStringToCString( oldName );
	CPLErr res = handle->CopyFiles(n, o);
	bbMemFree(n);
	bbMemFree(o);
	return res;
}

GDALDataset * bmx_gdal_GDALDriver_CreateDataset(GDALDriver * handle, BBString * filename, int xSize, int ySize, int bands, GDALDataType dataType, BBArray * paramList) {
	char *n = bbStringToCString( filename );
	
	GDALDataset * dataset = 0;
	
	if (paramList && (paramList->scales[0] > 0)) {
		char** list = bmx_bbStringArrayToStringList(paramList);
		dataset = handle->Create(n, xSize, ySize, bands, dataType, list);
		bmx_StringListFree(list);
	} else {
		dataset = handle->Create(n, xSize, ySize, bands, dataType, NULL);
	}
	
	bbMemFree(n);
	return dataset;
}

// *****************************************************

GDAL_GCP * bmx_gdal_GDAL_GCP_create(BBString * id, BBString * info, double pixel, double line, double x, double y, double z) {
	// TODO
}

BBString * bmx_gdal_GDAL_GCP_GetID(GDAL_GCP * handle) {
	return bbStringFromCString(handle->pszId);
}

void bmx_gdal_GDAL_GCP_SetId(GDAL_GCP * handle, BBString * id) {
	CPLFree( handle->pszId );
	char *n = bbStringToCString( id );
	handle->pszId = CPLStrdup(n);
	bbMemFree(n);
}

BBString * bmx_gdal_GDAL_GCP_GetInfo(GDAL_GCP * handle) {
	return bbStringFromCString(handle->pszInfo);
}

void bmx_gdal_GDAL_GCP_SetInfo(GDAL_GCP * handle, BBString * info) {
	CPLFree( handle->pszInfo );
	char *n = bbStringToCString( info );
	handle->pszInfo = CPLStrdup(n);
	bbMemFree(n);
}

double bmx_gdal_GDAL_GCP_GetPixel(GDAL_GCP * handle) {
	return handle->dfGCPPixel;
}

void bmx_gdal_GDAL_GCP_SetPixel(GDAL_GCP * handle, double pixel) {
	handle->dfGCPPixel = pixel;
}

double bmx_gdal_GDAL_GCP_GetLine(GDAL_GCP * handle) {
	return handle->dfGCPLine;
}

void bmx_gdal_GDAL_GCP_SetLine(GDAL_GCP * handle, double line) {
	handle->dfGCPLine = line;
}

double bmx_gdal_GDAL_GCP_GetX(GDAL_GCP * handle) {
	return handle->dfGCPX;
}

void bmx_gdal_GDAL_GCP_SetX(GDAL_GCP * handle, double x) {
	handle->dfGCPX = x;
}

double bmx_gdal_GDAL_GCP_GetY(GDAL_GCP * handle) {
	return handle->dfGCPY;
}

void bmx_gdal_GDAL_GCP_SetY(GDAL_GCP * handle, double y) {
	handle->dfGCPY = y;
}

double bmx_gdal_GDAL_GCP_GetZ(GDAL_GCP * handle) {
	return handle->dfGCPZ;
}

void bmx_gdal_GDAL_GCP_SetZ(GDAL_GCP * handle, double z) {
	handle->dfGCPZ = z;
}

void bmx_gdal_GDAL_GCP_free(GDAL_GCP * handle) {
	if (handle->pszId != NULL) {
		CPLFree( handle->pszId );
	}
	if (handle->pszInfo != NULL) {
		CPLFree( handle->pszInfo );
	}
	CPLFree(handle);
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

OGRDataSource * bmx_gdal_OGRSFDriverRegistrar_GetOpenDS(int ids) {
	return OGRSFDriverRegistrar::GetRegistrar()->GetOpenDS(ids);
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

OGRDataSource * bmx_gdal_OGRSFDriver_Open(OGRSFDriver * handle, BBString * name, int update) {
	char *n = bbStringToCString( name );
	OGRDataSource * source = handle->Open(n, update);
	bbMemFree(n);
	return source;
}

int bmx_gdal_OGRSFDriver_TestCapability(OGRSFDriver * handle, BBString * capability) {
	char *c = bbStringToCString( capability );
	int res = handle->TestCapability(c);
	bbMemFree(c);
	return res;
}

OGRErr bmx_gdal_OGRSFDriver_DeleteDataSource(OGRSFDriver * handle, BBString * datasource) {
	char *n = bbStringToCString( datasource );
	OGRErr res = handle->DeleteDataSource(n);
	bbMemFree(n);
	return res;
}

BBString * bmx_gdal_OGRSFDriver_GetName(OGRSFDriver * handle) {
	return bbStringFromCString(handle->GetName());
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

OGRErr bmx_gdal_OGRDataSource_DeleteLayer(OGRDataSource * handle, int layer) {
	return handle->DeleteLayer(layer);
}

int bmx_gdal_OGRDataSource_TestCapability(OGRDataSource * handle, BBString * name) {
	char *n = bbStringToCString( name );
	int res = handle->TestCapability(n);
	bbMemFree(n);
	return res;
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


