/******************************************************************************
 * $Id: isis2dataset.cpp 17786 2009-10-09 19:35:19Z warmerdam $
 *
 * Project:  ISIS Version 2 Driver
 * Purpose:  Implementation of ISIS2Dataset
 * Author:   Trent Hare (thare@usgs.gov),
 *           Robert Soricone (rsoricone@usgs.gov)
 *
 * NOTE: Original code authored by Trent and Robert and placed in the public 
 * domain as per US government policy.  I have (within my rights) appropriated 
 * it and placed it under the following license.  This is not intended to 
 * diminish Trent and Roberts contribution. 
 ******************************************************************************
 * Copyright (c) 2006, Frank Warmerdam <warmerdam@pobox.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#define NULL1 0
#define NULL2 -32768
//#define NULL3 0xFF7FFFFB //in hex
//Same as ESRI_GRID_FLOAT_NO_DATA
//#define NULL3 -340282346638528859811704183484516925440.0
#define NULL3 -3.4028226550889044521e+38

#ifndef PI
#  define PI 3.1415926535897932384626433832795
#endif

#include "rawdataset.h"
#include "ogr_spatialref.h"
#include "cpl_string.h" 
#include "nasakeywordhandler.h"

CPL_CVSID("$Id: isis2dataset.cpp 17786 2009-10-09 19:35:19Z warmerdam $");

CPL_C_START
void	GDALRegister_ISIS2(void);
CPL_C_END

/************************************************************************/
/* ==================================================================== */
/*			ISISDataset	version2	                */
/* ==================================================================== */
/************************************************************************/

class ISIS2Dataset : public RawDataset
{
    FILE	*fpImage;	// image data file.

    NASAKeywordHandler  oKeywords;
  
    int         bGotTransform;
    double      adfGeoTransform[6];
  
    CPLString   osProjection;

    int parse_label(const char *file, char *keyword, char *value);
    int strstrip(char instr[], char outstr[], int position);

    CPLString   oTempResult;

    void        CleanString( CPLString &osInput );

    const char *GetKeyword( const char *pszPath, 
                            const char *pszDefault = "");
    const char *GetKeywordSub( const char *pszPath, 
                               int iSubscript, 
                               const char *pszDefault = "");

public:
    ISIS2Dataset();
    ~ISIS2Dataset();
  
    virtual CPLErr GetGeoTransform( double * padfTransform );
    virtual const char *GetProjectionRef(void);
  
    static GDALDataset *Open( GDALOpenInfo * );
    static GDALDataset *Create( const char * pszFilename,
                                int nXSize, int nYSize, int nBands,
                                GDALDataType eType, char ** papszParmList );
};

/************************************************************************/
/*                            ISIS2Dataset()                            */
/************************************************************************/

ISIS2Dataset::ISIS2Dataset()
{
    fpImage = NULL;
    bGotTransform = FALSE;
    adfGeoTransform[0] = 0.0;
    adfGeoTransform[1] = 1.0;
    adfGeoTransform[2] = 0.0;
    adfGeoTransform[3] = 0.0;
    adfGeoTransform[4] = 0.0;
    adfGeoTransform[5] = 1.0;
}

/************************************************************************/
/*                            ~ISIS2Dataset()                            */
/************************************************************************/

ISIS2Dataset::~ISIS2Dataset()

{
    FlushCache();
    if( fpImage != NULL )
        VSIFCloseL( fpImage );
}

/************************************************************************/
/*                          GetProjectionRef()                          */
/************************************************************************/

const char *ISIS2Dataset::GetProjectionRef()

{
    if( strlen(osProjection) > 0 )
        return osProjection;
    else
        return GDALPamDataset::GetProjectionRef();
}

/************************************************************************/
/*                          GetGeoTransform()                           */
/************************************************************************/

CPLErr ISIS2Dataset::GetGeoTransform( double * padfTransform )

{
    if( bGotTransform )
    {
        memcpy( padfTransform, adfGeoTransform, sizeof(double) * 6 );
        return CE_None;
    }
    else
    {
        return GDALPamDataset::GetGeoTransform( padfTransform );
    }
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset *ISIS2Dataset::Open( GDALOpenInfo * poOpenInfo )
{
/* -------------------------------------------------------------------- */
/*      Does this look like a CUBE dataset?                             */
/* -------------------------------------------------------------------- */
    if( poOpenInfo->pabyHeader == NULL
        || strstr((const char *)poOpenInfo->pabyHeader,"^QUBE") == NULL )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Open the file using the large file API.                         */
/* -------------------------------------------------------------------- */
    FILE *fpQube = VSIFOpenL( poOpenInfo->pszFilename, "rb" );

    if( fpQube == NULL )
        return NULL;

    ISIS2Dataset 	*poDS;

    poDS = new ISIS2Dataset();

    if( ! poDS->oKeywords.Ingest( fpQube, 0 ) )
    {
        VSIFCloseL( fpQube );
        delete poDS;
        return NULL;
    }
    
    VSIFCloseL( fpQube );

/* -------------------------------------------------------------------- */
/*	We assume the user is pointing to the label (ie. .lab) file.  	*/
/* -------------------------------------------------------------------- */
    // QUBE can be inline or detached and point to an image name
    // ^QUBE = 76
    // ^QUBE = ("ui31s015.img",6441<BYTES>) - has another label on the image
    // ^QUBE = "ui31s015.img" - which implies no label or skip value

    const char *pszQube = poDS->GetKeyword( "^QUBE" );
    int nQube = atoi(pszQube);

    if( pszQube[0] == '"' || pszQube[0] == '(' )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "ISIS2 driver does not support detached images." );
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Check if file an ISIS2 header file?  Read a few lines of text   */
/*      searching for something starting with nrows or ncols.           */
/* -------------------------------------------------------------------- */
    GDALDataType eDataType = GDT_Byte;
    OGRSpatialReference oSRS;

    //image parameters
    int	nRows, nCols, nBands = 1;
    int nSkipBytes = 0;
    int itype;
    int  s_ix, s_iy, s_iz; // check SUFFIX_ITEMS params.
    int record_bytes;
    int	bNoDataSet = FALSE;
    char chByteOrder = 'M';  //default to MSB
 
    //Georef parameters
    double dfULXMap=0.5;
    double dfULYMap = 0.5;
    double dfXDim = 1.0;
    double dfYDim = 1.0;
    double dfNoData = 0.0;
    double xulcenter = 0.0;
    double yulcenter = 0.0;

    //projection parameters
    int	bProjectionSet = TRUE;
    double semi_major = 0.0;
    double semi_minor = 0.0;
    double iflattening = 0.0;
    float center_lat = 0.0;
    float center_lon = 0.0;
    float first_std_parallel = 0.0;
    float second_std_parallel = 0.0;
    FILE	*fp;

    /* -------------------------------------------------------------------- */
    /*      Checks to see if this is valid ISIS2 cube                       */
    /*      SUFFIX_ITEM tag in .cub file should be (0,0,0); no side-planes  */
    /* -------------------------------------------------------------------- */
    s_ix = atoi(poDS->GetKeywordSub( "QUBE.SUFFIX_ITEMS", 1 ));
    s_iy = atoi(poDS->GetKeywordSub( "QUBE.SUFFIX_ITEMS", 2 ));
    s_iz = atoi(poDS->GetKeywordSub( "QUBE.SUFFIX_ITEMS", 3 ));
     
    if( s_ix != 0 || s_iy != 0 || s_iz != 0 ) 
    {
        CPLError( CE_Failure, CPLE_OpenFailed, 
                  "*** ISIS 2 cube file has invalid SUFFIX_ITEMS parameters:\n"
                  "*** gdal isis2 driver requires (0, 0, 0), thus no sideplanes or backplanes\n"
                  "found: (%i, %i, %i)\n\n", s_ix, s_iy, s_iz );
        return NULL;
    } 

    /**************** end SUFFIX_ITEM check ***********************/
    
    
    /***********   Grab layout type (BSQ, BIP, BIL) ************/
    //  AXIS_NAME = (SAMPLE,LINE,BAND)
    /***********************************************************/
    const char *value;

    char szLayout[10] = "BSQ"; //default to band seq.
    value = poDS->GetKeyword( "QUBE.AXIS_NAME", "" );
    if (EQUAL(value,"(SAMPLE,LINE,BAND)") )
        strcpy(szLayout,"BSQ");
    else if (EQUAL(value,"(BAND,LINE,SAMPLE)") )
        strcpy(szLayout,"BIP");
    else if (EQUAL(value,"(SAMPLE,BAND,LINE)") || EQUAL(value,"") )
        strcpy(szLayout,"BSQ");
    else {
        CPLError( CE_Failure, CPLE_OpenFailed, 
                  "%s layout not supported. Abort\n\n", value);
        return NULL;
    }

    /***********   Grab samples lines band ************/
    nCols = atoi(poDS->GetKeywordSub("QUBE.CORE_ITEMS",1));
    nRows = atoi(poDS->GetKeywordSub("QUBE.CORE_ITEMS",2));
    nBands = atoi(poDS->GetKeywordSub("QUBE.CORE_ITEMS",3));
    
    /***********   Grab Qube record bytes  **********/
    record_bytes = atoi(poDS->GetKeyword("RECORD_BYTES"));

    if (nQube > 0)
        nSkipBytes = (nQube - 1) * record_bytes;     
    else
        nSkipBytes = 0;     
     
    /********   Grab format type - isis2 only supports 8,16,32 *******/
    itype = atoi(poDS->GetKeyword("QUBE.CORE_ITEM_BYTES",""));
    switch(itype) {
      case 1 :
        eDataType = GDT_Byte;
        dfNoData = NULL1;
        bNoDataSet = TRUE;
        break;
      case 2 :
        eDataType = GDT_Int16;
        dfNoData = NULL2;
        bNoDataSet = TRUE;
        break;
      case 4 :
        eDataType = GDT_Float32;
        dfNoData = NULL3;
        bNoDataSet = TRUE;
        break;
      default :
        CPLError( CE_Failure, CPLE_AppDefined,
                  "Itype of %d is not supported in ISIS 2.",
                  itype); 
        delete poDS;
        return NULL;
    }

    /***********   Grab samples lines band ************/
    value = poDS->GetKeyword( "QUBE.CORE_ITEM_TYPE" );
    if( (EQUAL(value,"PC_INTEGER")) || 
        (EQUAL(value,"PC_UNSIGNED_INTEGER")) || 
        (EQUAL(value,"PC_REAL")) ) {
        chByteOrder = 'I';
    }
    
    /***********   Grab Cellsize ************/
    value = poDS->GetKeyword("QUBE.IMAGE_MAP_PROJECTION.MAP_SCALE");
    if (strlen(value) > 0 ) {
        dfXDim = (float) atof(value) * 1000.0; /* convert from km to m */
        dfYDim = (float) atof(value) * 1000.0 * -1;
    }
    
    /***********   Grab LINE_PROJECTION_OFFSET ************/
    value = poDS->GetKeyword("QUBE.IMAGE_MAP_PROJECTION.LINE_PROJECTION_OFFSET");
    if (strlen(value) > 0) {
        yulcenter = (float) atof(value);
        yulcenter = ((yulcenter) * dfYDim);
        dfULYMap = yulcenter - (dfYDim/2);
    }
     
    /***********   Grab SAMPLE_PROJECTION_OFFSET ************/
    value = poDS->GetKeyword("QUBE.IMAGE_MAP_PROJECTION.SAMPLE_PROJECTION_OFFSET");
    if( strlen(value) > 0 ) {
        xulcenter = (float) atof(value);
        xulcenter = ((xulcenter) * dfXDim);
        dfULXMap = xulcenter - (dfXDim/2);
    }
     
    /***********  Grab TARGET_NAME  ************/
    /**** This is the planets name i.e. MARS ***/
    CPLString target_name = poDS->GetKeyword("QUBE.TARGET_NAME");
     
    /***********   Grab MAP_PROJECTION_TYPE ************/
    CPLString map_proj_name = 
        poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.MAP_PROJECTION_TYPE");
    poDS->CleanString( map_proj_name );

    /***********   Grab SEMI-MAJOR ************/
    semi_major = 
        atof(poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.A_AXIS_RADIUS")) * 1000.0;

    /***********   Grab semi-minor ************/
    semi_minor = 
        atof(poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.C_AXIS_RADIUS")) * 1000.0;

    /***********   Grab CENTER_LAT ************/
    center_lat = 
        atof(poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.CENTER_LATITUDE"));

    /***********   Grab CENTER_LON ************/
    center_lon = 
        atof(poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.CENTER_LONGITUDE"));

    /***********   Grab 1st std parallel ************/
    first_std_parallel = 
        atof(poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.FIRST_STANDARD_PARALLEL"));

    /***********   Grab 2nd std parallel ************/
    second_std_parallel = 
        atof(poDS->GetKeyword( "QUBE.IMAGE_MAP_PROJECTION.SECOND_STANDARD_PARALLEL"));
     
    /*** grab  PROJECTION_LATITUDE_TYPE = "PLANETOCENTRIC" ****/
    // Need to further study how ocentric/ographic will effect the gdal library.
    // So far we will use this fact to define a sphere or ellipse for some projections
    // Frank - may need to talk this over
    char bIsGeographic = TRUE;
    value = poDS->GetKeyword("CUBE.IMAGE_MAP_PROJECTION.PROJECTION_LATITUDE_TYPE");
    if (EQUAL( value, "\"PLANETOCENTRIC\"" ))
        bIsGeographic = FALSE; 
     
    CPLDebug("ISIS2","using projection %s", map_proj_name.c_str() );

    //Set oSRS projection and parameters
    if ((EQUAL( map_proj_name, "EQUIRECTANGULAR_CYLINDRICAL" )) ||
        (EQUAL( map_proj_name, "EQUIRECTANGULAR" )) ||
        (EQUAL( map_proj_name, "SIMPLE_CYLINDRICAL" )) ) {
        oSRS.OGRSpatialReference::SetEquirectangular2 ( 0.0, center_lon, center_lat, 0, 0 );
    } else if (EQUAL( map_proj_name, "ORTHOGRAPHIC" )) { 
        oSRS.OGRSpatialReference::SetOrthographic ( center_lat, center_lon, 0, 0 );
    } else if ((EQUAL( map_proj_name, "SINUSOIDAL" )) ||
               (EQUAL( map_proj_name, "SINUSOIDAL_EQUAL-AREA" ))) {
        oSRS.OGRSpatialReference::SetSinusoidal ( center_lon, 0, 0 );
    } else if (EQUAL( map_proj_name, "MERCATOR" )) {
        oSRS.OGRSpatialReference::SetMercator ( center_lat, center_lon, 1, 0, 0 );
    } else if (EQUAL( map_proj_name, "POLAR_STEREOGRAPHIC" )) {
        oSRS.OGRSpatialReference::SetPS ( center_lat, center_lon, 1, 0, 0 );
    } else if (EQUAL( map_proj_name, "TRANSVERSE_MERCATOR" )) {
        oSRS.OGRSpatialReference::SetTM ( center_lat, center_lon, 1, 0, 0 );
    } else if (EQUAL( map_proj_name, "LAMBERT_CONFORMAL_CONIC" )) {
        oSRS.OGRSpatialReference::SetLCC ( first_std_parallel, second_std_parallel, center_lat, center_lon, 0, 0 );
    } else {
        CPLDebug( "ISIS2",
                  "Dataset projection %s is not supported. Continuing...",
                  map_proj_name.c_str() );
        bProjectionSet = FALSE;
    }

    if (bProjectionSet) {
        //Create projection name, i.e. MERCATOR MARS and set as ProjCS keyword
        CPLString proj_target_name = map_proj_name + " " + target_name;
        oSRS.SetProjCS(proj_target_name); //set ProjCS keyword
     
        //The geographic/geocentric name will be the same basic name as the body name
        //'GCS' = Geographic/Geocentric Coordinate System
        CPLString geog_name = "GCS_" + target_name;
        
        //The datum and sphere names will be the same basic name aas the planet
        CPLString datum_name = "D_" + target_name;
        CPLString sphere_name = target_name; // + "_IAU_IAG");  //Might not be IAU defined so don't add
          
        //calculate inverse flattening from major and minor axis: 1/f = a/(a-b)
        if ((semi_major - semi_minor) < 0.0000001) 
            iflattening = 0;
        else
            iflattening = semi_major / (semi_major - semi_minor);
     
        //Set the body size but take into consideration which proj is being used to help w/ proj4 compatibility
        //The use of a Sphere, polar radius or ellipse here is based on how ISIS does it internally
        if ( ( (EQUAL( map_proj_name, "STEREOGRAPHIC" ) && (fabs(center_lat) == 90)) ) || 
             (EQUAL( map_proj_name, "POLAR_STEREOGRAPHIC" )))  
        {
            if (bIsGeographic) { 
                //Geograpraphic, so set an ellipse
                oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                                semi_major, iflattening, 
                                "Reference_Meridian", 0.0 );
            } else {
                //Geocentric, so force a sphere using the semi-minor axis. I hope... 
                sphere_name += "_polarRadius";
                oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                                semi_minor, 0.0, 
                                "Reference_Meridian", 0.0 );
            }
        }
        else if ( (EQUAL( map_proj_name, "SIMPLE_CYLINDRICAL" )) || 
                  (EQUAL( map_proj_name, "ORTHOGRAPHIC" )) ||
                  (EQUAL( map_proj_name, "STEREOGRAPHIC" )) ||
                  (EQUAL( map_proj_name, "SINUSOIDAL_EQUAL-AREA" )) ||
                  (EQUAL( map_proj_name, "SINUSOIDAL" ))  ) {
            //isis uses the sphereical equation for these projections so force a sphere
            oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                            semi_major, 0.0, 
                            "Reference_Meridian", 0.0 );
        } 
        else if  ((EQUAL( map_proj_name, "EQUIRECTANGULAR_CYLINDRICAL" )) || 
                  (EQUAL( map_proj_name, "EQUIRECTANGULAR" )) ) {
            //Calculate localRadius using ISIS3 simple elliptical method 
            //  not the more standard Radius of Curvature method
            //PI = 4 * atan(1);
            double radLat, localRadius;
            if (center_lon == 0) { //No need to calculate local radius
                oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                                semi_major, 0.0, 
                                "Reference_Meridian", 0.0 );
            } else {  
                radLat = center_lat * PI / 180;  // in radians
                localRadius = semi_major * semi_minor / sqrt(pow(semi_minor*cos(radLat),2)
                                                             + pow(semi_major*sin(radLat),2) );
                sphere_name += "_localRadius";
                oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                                localRadius, 0.0, 
                                "Reference_Meridian", 0.0 );
                CPLDebug( "ISIS2", "local radius: %f", localRadius);
            }
        } 
        else { 
            //All other projections: Mercator, Transverse Mercator, Lambert Conformal, etc.
            //Geographic, so set an ellipse
            if (bIsGeographic) {
                oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                                semi_major, iflattening, 
                                "Reference_Meridian", 0.0 );
            } else { 
                //Geocentric, so force a sphere. I hope... 
                oSRS.SetGeogCS( geog_name, datum_name, sphere_name,
                                semi_major, 0.0, 
                                "Reference_Meridian", 0.0 );
            }
        }
        

        // translate back into a projection string.
        char *pszResult = NULL;
        oSRS.exportToWkt( &pszResult );
        poDS->osProjection = pszResult;
        CPLFree( pszResult );
    }

/* END ISIS2 Label Read */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
    
/* -------------------------------------------------------------------- */
/*      Did we get the required keywords?  If not we return with        */
/*      this never having been considered to be a match. This isn't     */
/*      an error!                                                       */
/* -------------------------------------------------------------------- */
    if( nRows < 1 || nCols < 1 || nBands < 1 )
    {
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Capture some information from the file that is of interest.     */
/* -------------------------------------------------------------------- */
    poDS->nRasterXSize = nCols;
    poDS->nRasterYSize = nRows;

/* -------------------------------------------------------------------- */
/*      Open target binary file.                                        */
/* -------------------------------------------------------------------- */
    
    if( poOpenInfo->eAccess == GA_ReadOnly )
        poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "rb" );
    else
        poDS->fpImage = VSIFOpenL( poOpenInfo->pszFilename, "r+b" );

    if( poDS->fpImage == NULL )
    {
        CPLError( CE_Failure, CPLE_OpenFailed, 
                  "Failed to open %s with write permission.\n%s", 
                  poOpenInfo->pszFilename, VSIStrerror( errno ) );
        delete poDS;
        return NULL;
    }

    poDS->eAccess = poOpenInfo->eAccess;

/* -------------------------------------------------------------------- */
/*      Compute the line offset.                                        */
/* -------------------------------------------------------------------- */
    int     nItemSize = GDALGetDataTypeSize(eDataType)/8;
    int		nLineOffset, nPixelOffset, nBandOffset;
    
    if( EQUAL(szLayout,"BIP") )
    {
        nPixelOffset = nItemSize * nBands;
        nLineOffset = nPixelOffset * nCols;
        nBandOffset = nItemSize;
    }
    else if( EQUAL(szLayout,"BSQ") )
    {
        nPixelOffset = nItemSize;
        nLineOffset = nPixelOffset * nCols;
        nBandOffset = nLineOffset * nRows;
    }
    else /* assume BIL */
    {
        nPixelOffset = nItemSize;
        nLineOffset = nItemSize * nBands * nCols;
        nBandOffset = nItemSize * nCols;
    }
    
/* -------------------------------------------------------------------- */
/*      Create band information objects.                                */
/* -------------------------------------------------------------------- */
    int i;

    poDS->nBands = nBands;;
    for( i = 0; i < poDS->nBands; i++ )
    {
        RawRasterBand	*poBand;

        poBand = 
            new RawRasterBand( poDS, i+1, poDS->fpImage,
                               nSkipBytes + nBandOffset * i, 
                               nPixelOffset, nLineOffset, eDataType,
#ifdef CPL_LSB                               
                               chByteOrder == 'I' || chByteOrder == 'L',
#else
                               chByteOrder == 'M',
#endif        
                               TRUE );

        if( bNoDataSet )
            poBand->SetNoDataValue( dfNoData );

        poDS->SetBand( i+1, poBand );

        // Set offset/scale values at the PAM level.
        poBand->SetOffset( 
            CPLAtofM(poDS->GetKeyword("QUBE.CORE_BASE","0.0")));
        poBand->SetScale( 
            CPLAtofM(poDS->GetKeyword("QUBE.CORE_MULTIPLIER","1.0")));
    }

/* -------------------------------------------------------------------- */
/*      Check for a .prj file. For isis2 I would like to keep this in   */
/* -------------------------------------------------------------------- */
    CPLString osPath, osName;

    osPath = CPLGetPath( poOpenInfo->pszFilename );
    osName = CPLGetBasename(poOpenInfo->pszFilename);
    const char  *pszPrjFile = CPLFormCIFilename( osPath, osName, "prj" );

    fp = VSIFOpen( pszPrjFile, "r" );
    if( fp != NULL )
    {
        char	**papszLines;
        OGRSpatialReference oSRS;

        VSIFClose( fp );
        
        papszLines = CSLLoad( pszPrjFile );

        if( oSRS.importFromESRI( papszLines ) == OGRERR_NONE )
        {
            char *pszResult = NULL;
            oSRS.exportToWkt( &pszResult );
            poDS->osProjection = pszResult;
            CPLFree( pszResult );
        }

        CSLDestroy( papszLines );
    }

    
    if( dfULYMap != 0.5 || dfULYMap != 0.5 || dfXDim != 1.0 || dfYDim != 1.0 )
    {
        poDS->bGotTransform = TRUE;
        poDS->adfGeoTransform[0] = dfULXMap;
        poDS->adfGeoTransform[1] = dfXDim;
        poDS->adfGeoTransform[2] = 0.0;
        poDS->adfGeoTransform[3] = dfULYMap;
        poDS->adfGeoTransform[4] = 0.0;
        poDS->adfGeoTransform[5] = dfYDim;
    }
    
    if( !poDS->bGotTransform )
        poDS->bGotTransform = 
            GDALReadWorldFile( poOpenInfo->pszFilename, "cbw", 
                               poDS->adfGeoTransform );

    if( !poDS->bGotTransform )
        poDS->bGotTransform = 
            GDALReadWorldFile( poOpenInfo->pszFilename, "wld", 
                               poDS->adfGeoTransform );

/* -------------------------------------------------------------------- */ 
/*      Initialize any PAM information.                                 */ 
/* -------------------------------------------------------------------- */ 
    poDS->SetDescription( poOpenInfo->pszFilename ); 
    poDS->TryLoadXML(); 

/* -------------------------------------------------------------------- */
/*      Check for overviews.                                            */
/* -------------------------------------------------------------------- */
    poDS->oOvManager.Initialize( poDS, poOpenInfo->pszFilename );

    return( poDS );
}

/************************************************************************/
/*                             GetKeyword()                             */
/************************************************************************/

const char *ISIS2Dataset::GetKeyword( const char *pszPath, 
                                      const char *pszDefault )

{
    return oKeywords.GetKeyword( pszPath, pszDefault );
}

/************************************************************************/
/*                            GetKeywordSub()                           */
/************************************************************************/

const char *ISIS2Dataset::GetKeywordSub( const char *pszPath, 
                                         int iSubscript,
                                         const char *pszDefault )

{
    const char *pszResult = oKeywords.GetKeyword( pszPath, NULL );
    
    if( pszResult == NULL )
        return pszDefault;

    if( pszResult[0] != '(' )
        return pszDefault;

    char **papszTokens = CSLTokenizeString2( pszResult, "(,)", 
                                             CSLT_HONOURSTRINGS );

    if( iSubscript <= CSLCount(papszTokens) )
    {
        oTempResult = papszTokens[iSubscript-1];
        CSLDestroy( papszTokens );
        return oTempResult.c_str();
    }
    else
    {
        CSLDestroy( papszTokens );
        return pszDefault;
    }
}

/************************************************************************/
/*                            CleanString()                             */
/*                                                                      */
/* Removes single or double quotes, and converts spaces to underscores. */
/* The change is made in-place to CPLString.                            */
/************************************************************************/

void ISIS2Dataset::CleanString( CPLString &osInput )

{
   if(  ( osInput.size() < 2 ) ||
        ((osInput.at(0) != '"'   || osInput.at(osInput.size()-1) != '"' ) &&
        ( osInput.at(0) != '\'' || osInput.at(osInput.size()-1) != '\'')) )
        return;

    char *pszWrk = CPLStrdup(osInput.c_str() + 1);
    int i;

    pszWrk[strlen(pszWrk)-1] = '\0';
    
    for( i = 0; pszWrk[i] != '\0'; i++ )
    {
        if( pszWrk[i] == ' ' )
            pszWrk[i] = '_';
    }

    osInput = pszWrk;
    CPLFree( pszWrk );
}

/************************************************************************/
/*                         GDALRegister_ISIS2()                         */
/************************************************************************/

void GDALRegister_ISIS2()

{
    GDALDriver	*poDriver;

    if( GDALGetDriverByName( "ISIS2" ) == NULL )
    {
        poDriver = new GDALDriver();
        
        poDriver->SetDescription( "ISIS2" );
        poDriver->SetMetadataItem( GDAL_DMD_LONGNAME, 
                                   "USGS Astrogeology ISIS cube (Version 2)" );
        poDriver->SetMetadataItem( GDAL_DMD_HELPTOPIC, 
                                   "frmt_various.html#ISIS2" );

        poDriver->pfnOpen = ISIS2Dataset::Open;

        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}
