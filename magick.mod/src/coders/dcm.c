/*
% Copyright (C) 2003-2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
% Copyright 1991-1999 E. I. du Pont de Nemours and Company
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            DDDD    CCCC  M   M                              %
%                            D   D  C      MM MM                              %
%                            D   D  C      M M M                              %
%                            D   D  C      M   M                              %
%                            DDDD    CCCC  M   M                              %
%                                                                             %
%                                                                             %
%                          Read DICOM Image Format.                           %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/attribute.h"
#include "magick/blob.h"
#include "magick/colormap.h"
#include "magick/constitute.h"
#include "magick/enhance.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
#define USE_GRAYMAP
#define GRAYSCALE_USES_PALETTE
*/

/* Defines for "best guess" fixes */
#define NEW_IMPLICIT_LOGIC
#define IGNORE_WINDOW_FOR_UNSPECIFIED_SCALE_TYPE

/*
  Function types for reading MSB/LSB shorts/longs
*/
typedef magick_uint16_t (DicomReadShortFunc)(Image *);
typedef magick_uint32_t (DicomReadLongFunc)(Image *);

/*
   DCM enums
*/
typedef enum
{
  DCM_TS_IMPL_LITTLE,
  DCM_TS_EXPL_LITTLE,
  DCM_TS_EXPL_BIG,
  DCM_TS_JPEG,
  DCM_TS_JPEG_LS,
  DCM_TS_JPEG_2000,
  DCM_TS_RLE
} Dicom_TS;

typedef enum
{
  DCM_MSB_LITTLE,
  DCM_MSB_BIG_PENDING,
  DCM_MSB_BIG
} Dicom_MSB;

typedef enum
{
  DCM_PI_MONOCHROME1,
  DCM_PI_MONOCHROME2,
  DCM_PI_PALETTE_COLOR,
  DCM_PI_RGB,
  DCM_PI_OTHER
} Dicom_PI;

typedef enum
{
  DCM_RT_OPTICAL_DENSITY,
  DCM_RT_HOUNSFIELD,
  DCM_RT_UNSPECIFIED,
  DCM_RT_UNKNOWN
} Dicom_RT;

typedef enum
{
  DCM_RS_NONE,
  DCM_RS_PRE,
  DCM_RS_POST
} Dicom_RS;

/*
  Dicom medical image declarations.
*/
typedef struct _DicomStream
{
  /*
    Values representing nature of image
  */
  unsigned long
    rows,
    columns;

  unsigned int
    number_scenes,
    samples_per_pixel,
    bits_allocated,
    significant_bits,
    bytes_per_pixel,
    max_value_in,
    max_value_out,
    high_bit,
    pixel_representation,
    interlace;

  Dicom_MSB
    msb_state;

  Dicom_PI
    phot_interp;  

  double
    window_center,
    window_width,
    rescale_intercept,
    rescale_slope;

  Dicom_TS
    transfer_syntax;

  Dicom_RT
    rescale_type;

  Dicom_RS
    rescaling;

  /*
    Array to store offset table for fragments within image
  */
  magick_uint32_t
    offset_ct;
  magick_uint32_t *
    offset_arr;

  /*
    Variables used to handle fragments and RLE compression
  */
  magick_uint32_t
    frag_bytes;

  magick_uint32_t
    rle_seg_ct,
    rle_seg_offsets[15];

  int
    rle_rep_ct,
    rle_rep_char;

  /*
    Max and minimum sample values within image used for post rescale mapping
  */
  int
    upper_lim,
    lower_lim;

  Quantum
    *rescale_map;

#if defined(USE_GRAYMAP)  
  unsigned short
    *graymap;
#endif

  /*
    Values representing last read element
  */
  unsigned short
    group,
    element;

  int
    index,
    datum;

  size_t
    quantum,
    length;

  unsigned char *
    data;

  /*
    Remaining fields for internal use by DCM_ReadElement and to read data
  */
  DicomReadShortFunc *
    funcReadShort;

  DicomReadLongFunc *
    funcReadLong;

  int
    explicit_file;

  unsigned int
    verbose;
} DicomStream;

/*
  Function type for parsing DICOM elements
*/
typedef MagickPassFail (DicomElemParseFunc)(Image *Image,DicomStream *dcm,ExceptionInfo *exception);

/*
  Forward declaration for parser functions
*/
static DicomElemParseFunc funcDCM_TransferSyntax;
static DicomElemParseFunc funcDCM_StudyDate;
static DicomElemParseFunc funcDCM_PatientName;
static DicomElemParseFunc funcDCM_TriggerTime;
static DicomElemParseFunc funcDCM_FieldOfView;
static DicomElemParseFunc funcDCM_SeriesNumber;
static DicomElemParseFunc funcDCM_ImagePosition;
static DicomElemParseFunc funcDCM_ImageOrientation;
static DicomElemParseFunc funcDCM_SliceLocation;
static DicomElemParseFunc funcDCM_SamplesPerPixel;
static DicomElemParseFunc funcDCM_PhotometricInterpretation;
static DicomElemParseFunc funcDCM_PlanarConfiguration;
static DicomElemParseFunc funcDCM_NumberOfFrames;
static DicomElemParseFunc funcDCM_Rows;
static DicomElemParseFunc funcDCM_Columns;
static DicomElemParseFunc funcDCM_BitsAllocated;
static DicomElemParseFunc funcDCM_BitsStored;
static DicomElemParseFunc funcDCM_HighBit;
static DicomElemParseFunc funcDCM_PixelRepresentation;
static DicomElemParseFunc funcDCM_WindowCenter;
static DicomElemParseFunc funcDCM_WindowWidth;
static DicomElemParseFunc funcDCM_RescaleIntercept;
static DicomElemParseFunc funcDCM_RescaleSlope;
static DicomElemParseFunc funcDCM_RescaleType;
static DicomElemParseFunc funcDCM_PaletteDescriptor;
static DicomElemParseFunc funcDCM_LUT;
static DicomElemParseFunc funcDCM_Palette;

/*
  Type for holding information on DICOM elements
*/
typedef struct _DicomInfo
{
  unsigned short
    group,
    element;

  char
    *vr,
    *description;

  DicomElemParseFunc
    *pfunc;
} DicomInfo;

/*
  Array holding information on DICOM elements
*/
static const DicomInfo
  dicom_info[] =
  {
    { 0x0000, 0x0000, (char *) "UL", (char *) "Group Length", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0001, (char *) "UL", (char *) "Command Length to End", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0002, (char *) "UI", (char *) "Affected SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0003, (char *) "UI", (char *) "Requested SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0010, (char *) "LO", (char *) "Command Recognition Code", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0100, (char *) "US", (char *) "Command Field", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0110, (char *) "US", (char *) "Message ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0120, (char *) "US", (char *) "Message ID Being Responded To", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0200, (char *) "AE", (char *) "Initiator", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0300, (char *) "AE", (char *) "Receiver", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0400, (char *) "AE", (char *) "Find Location", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0600, (char *) "AE", (char *) "Move Destination", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0700, (char *) "US", (char *) "Priority", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0800, (char *) "US", (char *) "Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0850, (char *) "US", (char *) "Number of Matches", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0860, (char *) "US", (char *) "Response Sequence Number", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0900, (char *) "US", (char *) "Status", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0901, (char *) "AT", (char *) "Offending Element", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0902, (char *) "LO", (char *) "Exception Comment", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x0903, (char *) "US", (char *) "Exception ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1000, (char *) "UI", (char *) "Affected SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1001, (char *) "UI", (char *) "Requested SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1002, (char *) "US", (char *) "Event Type ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1005, (char *) "AT", (char *) "Attribute Identifier List", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1008, (char *) "US", (char *) "Action Type ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1020, (char *) "US", (char *) "Number of Remaining Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1021, (char *) "US", (char *) "Number of Completed Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1022, (char *) "US", (char *) "Number of Failed Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1023, (char *) "US", (char *) "Number of Warning Suboperations", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1030, (char *) "AE", (char *) "Move Originator Application Entity Title", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x1031, (char *) "US", (char *) "Move Originator Message ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x4000, (char *) "LO", (char *) "Dialog Receiver", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x4010, (char *) "LO", (char *) "Terminal Type", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5010, (char *) "SH", (char *) "Message Set ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5020, (char *) "SH", (char *) "End Message Set", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5110, (char *) "LO", (char *) "Display Format", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5120, (char *) "LO", (char *) "Page Position ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5130, (char *) "LO", (char *) "Text Format ID", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5140, (char *) "LO", (char *) "Normal Reverse", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5150, (char *) "LO", (char *) "Add Gray Scale", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5160, (char *) "LO", (char *) "Borders", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5170, (char *) "IS", (char *) "Copies", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5180, (char *) "LO", (char *) "OldMagnificationType", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x5190, (char *) "LO", (char *) "Erase", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x51a0, (char *) "LO", (char *) "Print", (DicomElemParseFunc *) NULL },
    { 0x0000, 0x51b0, (char *) "US", (char *) "Overlays", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0000, (char *) "UL", (char *) "Meta Element Group Length", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0001, (char *) "OB", (char *) "File Meta Information Version", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0002, (char *) "UI", (char *) "Media Storage SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0003, (char *) "UI", (char *) "Media Storage SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0010, (char *) "UI", (char *) "Transfer Syntax UID", (DicomElemParseFunc *) funcDCM_TransferSyntax },
    { 0x0002, 0x0012, (char *) "UI", (char *) "Implementation Class UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0013, (char *) "SH", (char *) "Implementation Version Name", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0016, (char *) "AE", (char *) "Source Application Entity Title", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0100, (char *) "UI", (char *) "Private Information Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0002, 0x0102, (char *) "OB", (char *) "Private Information", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0000, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0008, (char *) "US", (char *) "ISI Command Field", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0011, (char *) "US", (char *) "Attach ID Application Code", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0012, (char *) "UL", (char *) "Attach ID Message Count", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0013, (char *) "DA", (char *) "Attach ID Date", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0014, (char *) "TM", (char *) "Attach ID Time", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0020, (char *) "US", (char *) "Message Type", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0030, (char *) "DA", (char *) "Max Waiting Date", (DicomElemParseFunc *) NULL },
    { 0x0003, 0x0031, (char *) "TM", (char *) "Max Waiting Time", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x0000, (char *) "UL", (char *) "File Set Group Length", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1130, (char *) "CS", (char *) "File Set ID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1141, (char *) "CS", (char *) "File Set Descriptor File ID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1142, (char *) "CS", (char *) "File Set Descriptor File Specific Character Set", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1200, (char *) "UL", (char *) "Root Directory Entity First Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1202, (char *) "UL", (char *) "Root Directory Entity Last Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1212, (char *) "US", (char *) "File Set Consistency Flag", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1220, (char *) "SQ", (char *) "Directory Record Sequence", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1400, (char *) "UL", (char *) "Next Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1410, (char *) "US", (char *) "Record In Use Flag", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1420, (char *) "UL", (char *) "Referenced Lower Level Directory Entity Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1430, (char *) "CS", (char *) "Directory Record Type", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1432, (char *) "UI", (char *) "Private Record UID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1500, (char *) "CS", (char *) "Referenced File ID", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1504, (char *) "UL", (char *) "MRDR Directory Record Offset", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1510, (char *) "UI", (char *) "Referenced SOP Class UID In File", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1511, (char *) "UI", (char *) "Referenced SOP Instance UID In File", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1512, (char *) "UI", (char *) "Referenced Transfer Syntax UID In File", (DicomElemParseFunc *) NULL },
    { 0x0004, 0x1600, (char *) "UL", (char *) "Number of References", (DicomElemParseFunc *) NULL },
    { 0x0005, 0x0000, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0006, 0x0000, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0000, (char *) "UL", (char *) "Identifying Group Length", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0001, (char *) "UL", (char *) "Length to End", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0005, (char *) "CS", (char *) "Specific Character Set", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0008, (char *) "CS", (char *) "Image Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0010, (char *) "LO", (char *) "Recognition Code", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0012, (char *) "DA", (char *) "Instance Creation Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0013, (char *) "TM", (char *) "Instance Creation Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0014, (char *) "UI", (char *) "Instance Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0016, (char *) "UI", (char *) "SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0018, (char *) "UI", (char *) "SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0020, (char *) "DA", (char *) "Study Date", (DicomElemParseFunc *) funcDCM_StudyDate },
    { 0x0008, 0x0021, (char *) "DA", (char *) "Series Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0022, (char *) "DA", (char *) "Acquisition Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0023, (char *) "DA", (char *) "Image Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0024, (char *) "DA", (char *) "Overlay Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0025, (char *) "DA", (char *) "Curve Date", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0030, (char *) "TM", (char *) "Study Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0031, (char *) "TM", (char *) "Series Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0032, (char *) "TM", (char *) "Acquisition Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0033, (char *) "TM", (char *) "Image Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0034, (char *) "TM", (char *) "Overlay Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0035, (char *) "TM", (char *) "Curve Time", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0040, (char *) "xs", (char *) "Old Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0041, (char *) "xs", (char *) "Old Data Set Subtype", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0042, (char *) "CS", (char *) "Nuclear Medicine Series Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0050, (char *) "SH", (char *) "Accession Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0052, (char *) "CS", (char *) "Query/Retrieve Level", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0054, (char *) "AE", (char *) "Retrieve AE Title", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0058, (char *) "UI", (char *) "Failed SOP Instance UID List", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0060, (char *) "CS", (char *) "Modality", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0062, (char *) "SQ", (char *) "Modality Subtype", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0064, (char *) "CS", (char *) "Conversion Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0068, (char *) "CS", (char *) "Presentation Intent Type", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0070, (char *) "LO", (char *) "Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0080, (char *) "LO", (char *) "Institution Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0081, (char *) "ST", (char *) "Institution Address", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0082, (char *) "SQ", (char *) "Institution Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0090, (char *) "PN", (char *) "Referring Physician's Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0092, (char *) "ST", (char *) "Referring Physician's Address", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0094, (char *) "SH", (char *) "Referring Physician's Telephone Numbers", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0100, (char *) "SH", (char *) "Code Value", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0102, (char *) "SH", (char *) "Coding Scheme Designator", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0103, (char *) "SH", (char *) "Coding Scheme Version", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0104, (char *) "LO", (char *) "Code Meaning", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0105, (char *) "CS", (char *) "Mapping Resource", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x0106, (char *) "DT", (char *) "Context Group Version", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010b, (char *) "CS", (char *) "Code Set Extension Flag", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010c, (char *) "UI", (char *) "Private Coding Scheme Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010d, (char *) "UI", (char *) "Code Set Extension Creator UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x010f, (char *) "CS", (char *) "Context Identifier", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1000, (char *) "LT", (char *) "Network ID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1010, (char *) "SH", (char *) "Station Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1030, (char *) "LO", (char *) "Study Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1032, (char *) "SQ", (char *) "Procedure Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x103e, (char *) "LO", (char *) "Series Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1040, (char *) "LO", (char *) "Institutional Department Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1048, (char *) "PN", (char *) "Physician of Record", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1050, (char *) "PN", (char *) "Performing Physician's Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1060, (char *) "PN", (char *) "Name of Physician(s) Reading Study", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1070, (char *) "PN", (char *) "Operator's Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1080, (char *) "LO", (char *) "Admitting Diagnosis Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1084, (char *) "SQ", (char *) "Admitting Diagnosis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1090, (char *) "LO", (char *) "Manufacturer's Model Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1100, (char *) "SQ", (char *) "Referenced Results Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1110, (char *) "SQ", (char *) "Referenced Study Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1111, (char *) "SQ", (char *) "Referenced Study Component Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1115, (char *) "SQ", (char *) "Referenced Series Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1120, (char *) "SQ", (char *) "Referenced Patient Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1125, (char *) "SQ", (char *) "Referenced Visit Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1130, (char *) "SQ", (char *) "Referenced Overlay Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1140, (char *) "SQ", (char *) "Referenced Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1145, (char *) "SQ", (char *) "Referenced Curve Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1148, (char *) "SQ", (char *) "Referenced Previous Waveform", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x114a, (char *) "SQ", (char *) "Referenced Simultaneous Waveforms", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x114c, (char *) "SQ", (char *) "Referenced Subsequent Waveform", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1150, (char *) "UI", (char *) "Referenced SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1155, (char *) "UI", (char *) "Referenced SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1160, (char *) "IS", (char *) "Referenced Frame Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1195, (char *) "UI", (char *) "Transaction UID", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1197, (char *) "US", (char *) "Failure Reason", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1198, (char *) "SQ", (char *) "Failed SOP Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x1199, (char *) "SQ", (char *) "Referenced SOP Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2110, (char *) "CS", (char *) "Old Lossy Image Compression", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2111, (char *) "ST", (char *) "Derivation Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2112, (char *) "SQ", (char *) "Source Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2120, (char *) "SH", (char *) "Stage Name", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2122, (char *) "IS", (char *) "Stage Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2124, (char *) "IS", (char *) "Number of Stages", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2128, (char *) "IS", (char *) "View Number", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2129, (char *) "IS", (char *) "Number of Event Timers", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x212a, (char *) "IS", (char *) "Number of Views in Stage", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2130, (char *) "DS", (char *) "Event Elapsed Time(s)", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2132, (char *) "LO", (char *) "Event Timer Name(s)", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2142, (char *) "IS", (char *) "Start Trim", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2143, (char *) "IS", (char *) "Stop Trim", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2144, (char *) "IS", (char *) "Recommended Display Frame Rate", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2200, (char *) "CS", (char *) "Transducer Position", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2204, (char *) "CS", (char *) "Transducer Orientation", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2208, (char *) "CS", (char *) "Anatomic Structure", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2218, (char *) "SQ", (char *) "Anatomic Region Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2220, (char *) "SQ", (char *) "Anatomic Region Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2228, (char *) "SQ", (char *) "Primary Anatomic Structure Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2230, (char *) "SQ", (char *) "Primary Anatomic Structure Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2240, (char *) "SQ", (char *) "Transducer Position Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2242, (char *) "SQ", (char *) "Transducer Position Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2244, (char *) "SQ", (char *) "Transducer Orientation Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2246, (char *) "SQ", (char *) "Transducer Orientation Modifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2251, (char *) "SQ", (char *) "Anatomic Structure Space Or Region Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2253, (char *) "SQ", (char *) "Anatomic Portal Of Entrance Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2255, (char *) "SQ", (char *) "Anatomic Approach Direction Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2256, (char *) "ST", (char *) "Anatomic Perspective Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2257, (char *) "SQ", (char *) "Anatomic Perspective Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2258, (char *) "ST", (char *) "Anatomic Location Of Examining Instrument Description", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x2259, (char *) "SQ", (char *) "Anatomic Location Of Examining Instrument Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x225a, (char *) "SQ", (char *) "Anatomic Structure Space Or Region Modifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x225c, (char *) "SQ", (char *) "OnAxis Background Anatomic Structure Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0008, 0x4000, (char *) "LT", (char *) "Identifying Comments", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0001, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0002, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0003, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0004, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0005, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0006, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0007, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0008, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0009, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000a, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000b, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000c, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000d, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000e, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x000f, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0011, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0012, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0013, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0014, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0015, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0016, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0017, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0018, (char *) "LT", (char *) "Data Set Identifier", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x001a, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x001e, (char *) "UI", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0021, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0022, (char *) "SH", (char *) "User Orientation", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0023, (char *) "SL", (char *) "Initiation Type", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0024, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0025, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0026, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0027, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0029, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002a, (char *) "SL", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002c, (char *) "LO", (char *) "Series Comments", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002d, (char *) "SL", (char *) "Track Beat Average", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002e, (char *) "FD", (char *) "Distance Prescribed", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x002f, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0031, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0032, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0034, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0035, (char *) "SL", (char *) "Gantry Locus Type", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0037, (char *) "SL", (char *) "Starting Heart Rate", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0038, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0039, (char *) "SL", (char *) "RR Window Offset", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x003a, (char *) "SL", (char *) "Percent Cycle Imaged", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x003e, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x003f, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0040, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0041, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0042, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0043, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0050, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0051, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0060, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0061, (char *) "LT", (char *) "Series Unique Identifier", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0070, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0080, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x0091, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e2, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e3, (char *) "UI", (char *) "Equipment UID", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e6, (char *) "SH", (char *) "Genesis Version Now", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e7, (char *) "UL", (char *) "Exam Record Checksum", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e8, (char *) "UL", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00e9, (char *) "SL", (char *) "Actual Series Data Time Stamp", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f2, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f3, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f4, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f5, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f6, (char *) "LT", (char *) "PDM Data Object Type Extension", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00f8, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x00fb, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x1002, (char *) "OB", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x1003, (char *) "OB", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0009, 0x1010, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0000, (char *) "UL", (char *) "Patient Group Length", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0010, (char *) "PN", (char *) "Patient's Name", (DicomElemParseFunc *) funcDCM_PatientName },
    { 0x0010, 0x0020, (char *) "LO", (char *) "Patient's ID", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0021, (char *) "LO", (char *) "Issuer of Patient's ID", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0030, (char *) "DA", (char *) "Patient's Birth Date", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0032, (char *) "TM", (char *) "Patient's Birth Time", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0040, (char *) "CS", (char *) "Patient's Sex", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x0050, (char *) "SQ", (char *) "Patient's Insurance Plan Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1000, (char *) "LO", (char *) "Other Patient's ID's", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1001, (char *) "PN", (char *) "Other Patient's Names", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1005, (char *) "PN", (char *) "Patient's Birth Name", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1010, (char *) "AS", (char *) "Patient's Age", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1020, (char *) "DS", (char *) "Patient's Size", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1030, (char *) "DS", (char *) "Patient's Weight", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1040, (char *) "LO", (char *) "Patient's Address", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1050, (char *) "LT", (char *) "Insurance Plan Identification", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1060, (char *) "PN", (char *) "Patient's Mother's Birth Name", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1080, (char *) "LO", (char *) "Military Rank", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1081, (char *) "LO", (char *) "Branch of Service", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x1090, (char *) "LO", (char *) "Medical Record Locator", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2000, (char *) "LO", (char *) "Medical Alerts", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2110, (char *) "LO", (char *) "Contrast Allergies", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2150, (char *) "LO", (char *) "Country of Residence", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2152, (char *) "LO", (char *) "Region of Residence", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2154, (char *) "SH", (char *) "Patients Telephone Numbers", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2160, (char *) "SH", (char *) "Ethnic Group", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x2180, (char *) "SH", (char *) "Occupation", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21a0, (char *) "CS", (char *) "Smoking Status", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21b0, (char *) "LT", (char *) "Additional Patient History", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21c0, (char *) "US", (char *) "Pregnancy Status", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21d0, (char *) "DA", (char *) "Last Menstrual Date", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x21f0, (char *) "LO", (char *) "Patients Religious Preference", (DicomElemParseFunc *) NULL },
    { 0x0010, 0x4000, (char *) "LT", (char *) "Patient Comments", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0001, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0002, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0003, (char *) "LT", (char *) "Patient UID", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0004, (char *) "LT", (char *) "Patient ID", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000b, (char *) "SL", (char *) "Effective Series Duration", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000c, (char *) "SL", (char *) "Num Beats", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x000d, (char *) "LO", (char *) "Radio Nuclide Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0011, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0012, (char *) "LO", (char *) "Dataset Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0013, (char *) "LO", (char *) "Dataset Type", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0015, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0016, (char *) "SL", (char *) "Energy Number", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0017, (char *) "SL", (char *) "RR Interval Window Number", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0018, (char *) "SL", (char *) "MG Bin Number", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0019, (char *) "FD", (char *) "Radius Of Rotation", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001a, (char *) "SL", (char *) "Detector Count Zone", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001b, (char *) "SL", (char *) "Num Energy Windows", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001c, (char *) "SL", (char *) "Energy Offset", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001d, (char *) "SL", (char *) "Energy Range", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x001f, (char *) "SL", (char *) "Image Orientation", (DicomElemParseFunc *) funcDCM_ImageOrientation },
    { 0x0011, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0021, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0022, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0023, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0024, (char *) "SL", (char *) "FOV Mask Y Cutoff Angle", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0025, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0026, (char *) "SL", (char *) "Table Orientation", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0027, (char *) "SL", (char *) "ROI Top Left", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0028, (char *) "SL", (char *) "ROI Bottom Right", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0031, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0032, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0033, (char *) "LO", (char *) "Energy Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0034, (char *) "LO", (char *) "Spatial Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0035, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0036, (char *) "LO", (char *) "Uniformity Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0037, (char *) "LO", (char *) "Acquisition Specific Correct Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0038, (char *) "SL", (char *) "Byte Order", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003a, (char *) "SL", (char *) "Picture Format", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003b, (char *) "FD", (char *) "Pixel Scale", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003c, (char *) "FD", (char *) "Pixel Offset", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003e, (char *) "SL", (char *) "FOV Shape", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x003f, (char *) "SL", (char *) "Dataset Flags", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0040, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0041, (char *) "LT", (char *) "Medical Alerts", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0042, (char *) "LT", (char *) "Contrast Allergies", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0044, (char *) "FD", (char *) "Threshold Center", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0045, (char *) "FD", (char *) "Threshold Width", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0046, (char *) "SL", (char *) "Interpolation Type", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0055, (char *) "FD", (char *) "Period", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x0056, (char *) "FD", (char *) "ElapsedTime", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00a1, (char *) "DA", (char *) "Patient Registration Date", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00a2, (char *) "TM", (char *) "Patient Registration Time", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00b0, (char *) "LT", (char *) "Patient Last Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00b2, (char *) "LT", (char *) "Patient First Name", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00b4, (char *) "LT", (char *) "Patient Hospital Status", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00bc, (char *) "TM", (char *) "Current Location Time", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00c0, (char *) "LT", (char *) "Patient Insurance Status", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00d0, (char *) "LT", (char *) "Patient Billing Type", (DicomElemParseFunc *) NULL },
    { 0x0011, 0x00d2, (char *) "LT", (char *) "Patient Billing Address", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0000, (char *) "LT", (char *) "Modifying Physician", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0011, (char *) "SL", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0012, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0016, (char *) "SL", (char *) "AutoTrack Peak", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0017, (char *) "SL", (char *) "AutoTrack Width", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0018, (char *) "FD", (char *) "Transmission Scan Time", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0019, (char *) "FD", (char *) "Transmission Mask Width", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001a, (char *) "FD", (char *) "Copper Attenuator Thickness", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001c, (char *) "FD", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001d, (char *) "FD", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x001e, (char *) "FD", (char *) "Tomo View Offset", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0020, (char *) "LT", (char *) "Patient Name", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0022, (char *) "LT", (char *) "Patient Id", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0026, (char *) "LT", (char *) "Study Comments", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0030, (char *) "DA", (char *) "Patient Birthdate", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0031, (char *) "DS", (char *) "Patient Weight", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0032, (char *) "LT", (char *) "Patients Maiden Name", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0033, (char *) "LT", (char *) "Referring Physician", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0034, (char *) "LT", (char *) "Admitting Diagnosis", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0035, (char *) "LT", (char *) "Patient Sex", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0040, (char *) "LT", (char *) "Procedure Description", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0042, (char *) "LT", (char *) "Patient Rest Direction", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0044, (char *) "LT", (char *) "Patient Position", (DicomElemParseFunc *) NULL },
    { 0x0013, 0x0046, (char *) "LT", (char *) "View Direction", (DicomElemParseFunc *) NULL },
    { 0x0015, 0x0001, (char *) "DS", (char *) "Stenosis Calibration Ratio", (DicomElemParseFunc *) NULL },
    { 0x0015, 0x0002, (char *) "DS", (char *) "Stenosis Magnification", (DicomElemParseFunc *) NULL },
    { 0x0015, 0x0003, (char *) "DS", (char *) "Cardiac Calibration Ratio", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0000, (char *) "UL", (char *) "Acquisition Group Length", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0010, (char *) "LO", (char *) "Contrast/Bolus Agent", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0012, (char *) "SQ", (char *) "Contrast/Bolus Agent Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0014, (char *) "SQ", (char *) "Contrast/Bolus Administration Route Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0015, (char *) "CS", (char *) "Body Part Examined", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0020, (char *) "CS", (char *) "Scanning Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0021, (char *) "CS", (char *) "Sequence Variant", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0022, (char *) "CS", (char *) "Scan Options", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0023, (char *) "CS", (char *) "MR Acquisition Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0024, (char *) "SH", (char *) "Sequence Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0025, (char *) "CS", (char *) "Angio Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0026, (char *) "SQ", (char *) "Intervention Drug Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0027, (char *) "TM", (char *) "Intervention Drug Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0028, (char *) "DS", (char *) "Intervention Drug Dose", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0029, (char *) "SQ", (char *) "Intervention Drug Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x002a, (char *) "SQ", (char *) "Additional Drug Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0030, (char *) "LO", (char *) "Radionuclide", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0031, (char *) "LO", (char *) "Radiopharmaceutical", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0032, (char *) "DS", (char *) "Energy Window Centerline", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0033, (char *) "DS", (char *) "Energy Window Total Width", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0034, (char *) "LO", (char *) "Intervention Drug Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0035, (char *) "TM", (char *) "Intervention Drug Start Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0036, (char *) "SQ", (char *) "Intervention Therapy Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0037, (char *) "CS", (char *) "Therapy Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0038, (char *) "CS", (char *) "Intervention Status", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0039, (char *) "CS", (char *) "Therapy Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0040, (char *) "IS", (char *) "Cine Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0050, (char *) "DS", (char *) "Slice Thickness", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0060, (char *) "DS", (char *) "KVP", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0070, (char *) "IS", (char *) "Counts Accumulated", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0071, (char *) "CS", (char *) "Acquisition Termination Condition", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0072, (char *) "DS", (char *) "Effective Series Duration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0073, (char *) "CS", (char *) "Acquisition Start Condition", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0074, (char *) "IS", (char *) "Acquisition Start Condition Data", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0075, (char *) "IS", (char *) "Acquisition Termination Condition Data", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0080, (char *) "DS", (char *) "Repetition Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0081, (char *) "DS", (char *) "Echo Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0082, (char *) "DS", (char *) "Inversion Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0083, (char *) "DS", (char *) "Number of Averages", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0084, (char *) "DS", (char *) "Imaging Frequency", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0085, (char *) "SH", (char *) "Imaged Nucleus", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0086, (char *) "IS", (char *) "Echo Number(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0087, (char *) "DS", (char *) "Magnetic Field Strength", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0088, (char *) "DS", (char *) "Spacing Between Slices", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0089, (char *) "IS", (char *) "Number of Phase Encoding Steps", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0090, (char *) "DS", (char *) "Data Collection Diameter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0091, (char *) "IS", (char *) "Echo Train Length", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0093, (char *) "DS", (char *) "Percent Sampling", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0094, (char *) "DS", (char *) "Percent Phase Field of View", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x0095, (char *) "DS", (char *) "Pixel Bandwidth", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1000, (char *) "LO", (char *) "Device Serial Number", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1004, (char *) "LO", (char *) "Plate ID", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1010, (char *) "LO", (char *) "Secondary Capture Device ID", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1012, (char *) "DA", (char *) "Date of Secondary Capture", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1014, (char *) "TM", (char *) "Time of Secondary Capture", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1016, (char *) "LO", (char *) "Secondary Capture Device Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1018, (char *) "LO", (char *) "Secondary Capture Device Manufacturer Model Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1019, (char *) "LO", (char *) "Secondary Capture Device Software Version(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1020, (char *) "LO", (char *) "Software Version(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1022, (char *) "SH", (char *) "Video Image Format Acquired", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1023, (char *) "LO", (char *) "Digital Image Format Acquired", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1030, (char *) "LO", (char *) "Protocol Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1040, (char *) "LO", (char *) "Contrast/Bolus Route", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1041, (char *) "DS", (char *) "Contrast/Bolus Volume", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1042, (char *) "TM", (char *) "Contrast/Bolus Start Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1043, (char *) "TM", (char *) "Contrast/Bolus Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1044, (char *) "DS", (char *) "Contrast/Bolus Total Dose", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1045, (char *) "IS", (char *) "Syringe Counts", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1046, (char *) "DS", (char *) "Contrast Flow Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1047, (char *) "DS", (char *) "Contrast Flow Duration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1048, (char *) "CS", (char *) "Contrast/Bolus Ingredient", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1049, (char *) "DS", (char *) "Contrast/Bolus Ingredient Concentration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1050, (char *) "DS", (char *) "Spatial Resolution", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1060, (char *) "DS", (char *) "Trigger Time", (DicomElemParseFunc *) funcDCM_TriggerTime },
    { 0x0018, 0x1061, (char *) "LO", (char *) "Trigger Source or Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1062, (char *) "IS", (char *) "Nominal Interval", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1063, (char *) "DS", (char *) "Frame Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1064, (char *) "LO", (char *) "Framing Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1065, (char *) "DS", (char *) "Frame Time Vector", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1066, (char *) "DS", (char *) "Frame Delay", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1067, (char *) "DS", (char *) "Image Trigger Delay", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1068, (char *) "DS", (char *) "Group Time Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1069, (char *) "DS", (char *) "Trigger Time Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x106a, (char *) "CS", (char *) "Synchronization Trigger", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x106b, (char *) "UI", (char *) "Synchronization Frame of Reference", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x106e, (char *) "UL", (char *) "Trigger Sample Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1070, (char *) "LO", (char *) "Radiopharmaceutical Route", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1071, (char *) "DS", (char *) "Radiopharmaceutical Volume", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1072, (char *) "TM", (char *) "Radiopharmaceutical Start Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1073, (char *) "TM", (char *) "Radiopharmaceutical Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1074, (char *) "DS", (char *) "Radionuclide Total Dose", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1075, (char *) "DS", (char *) "Radionuclide Half Life", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1076, (char *) "DS", (char *) "Radionuclide Positron Fraction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1077, (char *) "DS", (char *) "Radiopharmaceutical Specific Activity", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1080, (char *) "CS", (char *) "Beat Rejection Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1081, (char *) "IS", (char *) "Low R-R Value", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1082, (char *) "IS", (char *) "High R-R Value", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1083, (char *) "IS", (char *) "Intervals Acquired", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1084, (char *) "IS", (char *) "Intervals Rejected", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1085, (char *) "LO", (char *) "PVC Rejection", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1086, (char *) "IS", (char *) "Skip Beats", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1088, (char *) "IS", (char *) "Heart Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1090, (char *) "IS", (char *) "Cardiac Number of Images", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1094, (char *) "IS", (char *) "Trigger Window", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1100, (char *) "DS", (char *) "Reconstruction Diameter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1110, (char *) "DS", (char *) "Distance Source to Detector", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1111, (char *) "DS", (char *) "Distance Source to Patient", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1114, (char *) "DS", (char *) "Estimated Radiographic Magnification Factor", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1120, (char *) "DS", (char *) "Gantry/Detector Tilt", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1121, (char *) "DS", (char *) "Gantry/Detector Slew", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1130, (char *) "DS", (char *) "Table Height", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1131, (char *) "DS", (char *) "Table Traverse", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1134, (char *) "CS", (char *) "Table Motion", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1135, (char *) "DS", (char *) "Table Vertical Increment", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1136, (char *) "DS", (char *) "Table Lateral Increment", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1137, (char *) "DS", (char *) "Table Longitudinal Increment", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1138, (char *) "DS", (char *) "Table Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x113a, (char *) "CS", (char *) "Table Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1140, (char *) "CS", (char *) "Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1141, (char *) "DS", (char *) "Angular Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1142, (char *) "DS", (char *) "Radial Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1143, (char *) "DS", (char *) "Scan Arc", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1144, (char *) "DS", (char *) "Angular Step", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1145, (char *) "DS", (char *) "Center of Rotation Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1146, (char *) "DS", (char *) "Rotation Offset", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1147, (char *) "CS", (char *) "Field of View Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1149, (char *) "IS", (char *) "Field of View Dimension(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1150, (char *) "IS", (char *) "Exposure Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1151, (char *) "IS", (char *) "X-ray Tube Current", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1152, (char *) "IS", (char *) "Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1153, (char *) "IS", (char *) "Exposure in uAs", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1154, (char *) "DS", (char *) "AveragePulseWidth", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1155, (char *) "CS", (char *) "RadiationSetting", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1156, (char *) "CS", (char *) "Rectification Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x115a, (char *) "CS", (char *) "RadiationMode", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x115e, (char *) "DS", (char *) "ImageAreaDoseProduct", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1160, (char *) "SH", (char *) "Filter Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1161, (char *) "LO", (char *) "TypeOfFilters", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1162, (char *) "DS", (char *) "IntensifierSize", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1164, (char *) "DS", (char *) "ImagerPixelSpacing", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1166, (char *) "CS", (char *) "Grid", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1170, (char *) "IS", (char *) "Generator Power", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1180, (char *) "SH", (char *) "Collimator/Grid Name", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1181, (char *) "CS", (char *) "Collimator Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1182, (char *) "IS", (char *) "Focal Distance", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1183, (char *) "DS", (char *) "X Focus Center", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1184, (char *) "DS", (char *) "Y Focus Center", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1190, (char *) "DS", (char *) "Focal Spot(s)", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1191, (char *) "CS", (char *) "Anode Target Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x11a0, (char *) "DS", (char *) "Body Part Thickness", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x11a2, (char *) "DS", (char *) "Compression Force", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1200, (char *) "DA", (char *) "Date of Last Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1201, (char *) "TM", (char *) "Time of Last Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1210, (char *) "SH", (char *) "Convolution Kernel", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1240, (char *) "IS", (char *) "Upper/Lower Pixel Values", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1242, (char *) "IS", (char *) "Actual Frame Duration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1243, (char *) "IS", (char *) "Count Rate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1244, (char *) "US", (char *) "Preferred Playback Sequencing", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1250, (char *) "SH", (char *) "Receiving Coil", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1251, (char *) "SH", (char *) "Transmitting Coil", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1260, (char *) "SH", (char *) "Plate Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1261, (char *) "LO", (char *) "Phosphor Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1300, (char *) "DS", (char *) "Scan Velocity", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1301, (char *) "CS", (char *) "Whole Body Technique", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1302, (char *) "IS", (char *) "Scan Length", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1310, (char *) "US", (char *) "Acquisition Matrix", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1312, (char *) "CS", (char *) "Phase Encoding Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1314, (char *) "DS", (char *) "Flip Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1315, (char *) "CS", (char *) "Variable Flip Angle Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1316, (char *) "DS", (char *) "SAR", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1318, (char *) "DS", (char *) "dB/dt", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1400, (char *) "LO", (char *) "Acquisition Device Processing Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1401, (char *) "LO", (char *) "Acquisition Device Processing Code", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1402, (char *) "CS", (char *) "Cassette Orientation", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1403, (char *) "CS", (char *) "Cassette Size", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1404, (char *) "US", (char *) "Exposures on Plate", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1405, (char *) "IS", (char *) "Relative X-ray Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1450, (char *) "DS", (char *) "Column Angulation", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1460, (char *) "DS", (char *) "Tomo Layer Height", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1470, (char *) "DS", (char *) "Tomo Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1480, (char *) "DS", (char *) "Tomo Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1490, (char *) "CS", (char *) "Tomo Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1491, (char *) "CS", (char *) "Tomo Class", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1495, (char *) "IS", (char *) "Number of Tomosynthesis Source Images", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1500, (char *) "CS", (char *) "PositionerMotion", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1508, (char *) "CS", (char *) "Positioner Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1510, (char *) "DS", (char *) "PositionerPrimaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1511, (char *) "DS", (char *) "PositionerSecondaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1520, (char *) "DS", (char *) "PositionerPrimaryAngleIncrement", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1521, (char *) "DS", (char *) "PositionerSecondaryAngleIncrement", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1530, (char *) "DS", (char *) "DetectorPrimaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1531, (char *) "DS", (char *) "DetectorSecondaryAngle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1600, (char *) "CS", (char *) "Shutter Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1602, (char *) "IS", (char *) "Shutter Left Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1604, (char *) "IS", (char *) "Shutter Right Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1606, (char *) "IS", (char *) "Shutter Upper Horizontal Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1608, (char *) "IS", (char *) "Shutter Lower Horizonta lEdge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1610, (char *) "IS", (char *) "Center of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1612, (char *) "IS", (char *) "Radius of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1620, (char *) "IS", (char *) "Vertices of Polygonal Shutter", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1622, (char *) "US", (char *) "Shutter Presentation Value", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1623, (char *) "US", (char *) "Shutter Overlay Group", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1700, (char *) "CS", (char *) "Collimator Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1702, (char *) "IS", (char *) "Collimator Left Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1704, (char *) "IS", (char *) "Collimator Right Vertical Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1706, (char *) "IS", (char *) "Collimator Upper Horizontal Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1708, (char *) "IS", (char *) "Collimator Lower Horizontal Edge", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1710, (char *) "IS", (char *) "Center of Circular Collimator", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1712, (char *) "IS", (char *) "Radius of Circular Collimator", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1720, (char *) "IS", (char *) "Vertices of Polygonal Collimator", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1800, (char *) "CS", (char *) "Acquisition Time Synchronized", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1801, (char *) "SH", (char *) "Time Source", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x1802, (char *) "CS", (char *) "Time Distribution Protocol", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x4000, (char *) "LT", (char *) "Acquisition Comments", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5000, (char *) "SH", (char *) "Output Power", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5010, (char *) "LO", (char *) "Transducer Data", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5012, (char *) "DS", (char *) "Focus Depth", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5020, (char *) "LO", (char *) "Processing Function", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5021, (char *) "LO", (char *) "Postprocessing Function", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5022, (char *) "DS", (char *) "Mechanical Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5024, (char *) "DS", (char *) "Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5026, (char *) "DS", (char *) "Cranial Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5027, (char *) "DS", (char *) "Soft Tissue Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5028, (char *) "DS", (char *) "Soft Tissue-Focus Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5029, (char *) "DS", (char *) "Soft Tissue-Surface Thermal Index", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5030, (char *) "DS", (char *) "Dynamic Range", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5040, (char *) "DS", (char *) "Total Gain", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5050, (char *) "IS", (char *) "Depth of Scan Field", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5100, (char *) "CS", (char *) "Patient Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5101, (char *) "CS", (char *) "View Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5104, (char *) "SQ", (char *) "Projection Eponymous Name Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5210, (char *) "DS", (char *) "Image Transformation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x5212, (char *) "DS", (char *) "Image Translation Vector", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6000, (char *) "DS", (char *) "Sensitivity", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6011, (char *) "IS", (char *) "Sequence of Ultrasound Regions", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6012, (char *) "US", (char *) "Region Spatial Format", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6014, (char *) "US", (char *) "Region Data Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6016, (char *) "UL", (char *) "Region Flags", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6018, (char *) "UL", (char *) "Region Location Min X0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x601a, (char *) "UL", (char *) "Region Location Min Y0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x601c, (char *) "UL", (char *) "Region Location Max X1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x601e, (char *) "UL", (char *) "Region Location Max Y1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6020, (char *) "SL", (char *) "Reference Pixel X0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6022, (char *) "SL", (char *) "Reference Pixel Y0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6024, (char *) "US", (char *) "Physical Units X Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6026, (char *) "US", (char *) "Physical Units Y Direction", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6028, (char *) "FD", (char *) "Reference Pixel Physical Value X", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x602a, (char *) "US", (char *) "Reference Pixel Physical Value Y", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x602c, (char *) "US", (char *) "Physical Delta X", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x602e, (char *) "US", (char *) "Physical Delta Y", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6030, (char *) "UL", (char *) "Transducer Frequency", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6031, (char *) "CS", (char *) "Transducer Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6032, (char *) "UL", (char *) "Pulse Repetition Frequency", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6034, (char *) "FD", (char *) "Doppler Correction Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6036, (char *) "FD", (char *) "Steering Angle", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6038, (char *) "UL", (char *) "Doppler Sample Volume X Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x603a, (char *) "UL", (char *) "Doppler Sample Volume Y Position", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x603c, (char *) "UL", (char *) "TM-Line Position X0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x603e, (char *) "UL", (char *) "TM-Line Position Y0", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6040, (char *) "UL", (char *) "TM-Line Position X1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6042, (char *) "UL", (char *) "TM-Line Position Y1", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6044, (char *) "US", (char *) "Pixel Component Organization", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6046, (char *) "UL", (char *) "Pixel Component Mask", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6048, (char *) "UL", (char *) "Pixel Component Range Start", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x604a, (char *) "UL", (char *) "Pixel Component Range Stop", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x604c, (char *) "US", (char *) "Pixel Component Physical Units", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x604e, (char *) "US", (char *) "Pixel Component Data Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6050, (char *) "UL", (char *) "Number of Table Break Points", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6052, (char *) "UL", (char *) "Table of X Break Points", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6054, (char *) "FD", (char *) "Table of Y Break Points", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6056, (char *) "UL", (char *) "Number of Table Entries", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x6058, (char *) "UL", (char *) "Table of Pixel Values", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x605a, (char *) "FL", (char *) "Table of Parameter Values", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7000, (char *) "CS", (char *) "Detector Conditions Nominal Flag", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7001, (char *) "DS", (char *) "Detector Temperature", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7004, (char *) "CS", (char *) "Detector Type", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7005, (char *) "CS", (char *) "Detector Configuration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7006, (char *) "LT", (char *) "Detector Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7008, (char *) "LT", (char *) "Detector Mode", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x700a, (char *) "SH", (char *) "Detector ID", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x700c, (char *) "DA", (char *) "Date of Last Detector Calibration ", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x700e, (char *) "TM", (char *) "Time of Last Detector Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7010, (char *) "IS", (char *) "Exposures on Detector Since Last Calibration", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7011, (char *) "IS", (char *) "Exposures on Detector Since Manufactured", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7012, (char *) "DS", (char *) "Detector Time Since Last Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7014, (char *) "DS", (char *) "Detector Active Time", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7016, (char *) "DS", (char *) "Detector Activation Offset From Exposure", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x701a, (char *) "DS", (char *) "Detector Binning", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7020, (char *) "DS", (char *) "Detector Element Physical Size", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7022, (char *) "DS", (char *) "Detector Element Spacing", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7024, (char *) "CS", (char *) "Detector Active Shape", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7026, (char *) "DS", (char *) "Detector Active Dimensions", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7028, (char *) "DS", (char *) "Detector Active Origin", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7030, (char *) "DS", (char *) "Field of View Origin", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7032, (char *) "DS", (char *) "Field of View Rotation", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7034, (char *) "CS", (char *) "Field of View Horizontal Flip", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7040, (char *) "LT", (char *) "Grid Absorbing Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7041, (char *) "LT", (char *) "Grid Spacing Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7042, (char *) "DS", (char *) "Grid Thickness", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7044, (char *) "DS", (char *) "Grid Pitch", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7046, (char *) "IS", (char *) "Grid Aspect Ratio", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7048, (char *) "DS", (char *) "Grid Period", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x704c, (char *) "DS", (char *) "Grid Focal Distance", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7050, (char *) "LT", (char *) "Filter Material", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7052, (char *) "DS", (char *) "Filter Thickness Minimum", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7054, (char *) "DS", (char *) "Filter Thickness Maximum", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7060, (char *) "CS", (char *) "Exposure Control Mode", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7062, (char *) "LT", (char *) "Exposure Control Mode Description", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7064, (char *) "CS", (char *) "Exposure Status", (DicomElemParseFunc *) NULL },
    { 0x0018, 0x7065, (char *) "DS", (char *) "Phototimer Setting", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0001, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0002, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0003, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0004, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0005, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0006, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0007, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0008, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0009, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000b, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000c, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000d, (char *) "TM", (char *) "Time", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x000f, (char *) "DS", (char *) "Horizontal Frame Of Reference", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0011, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0012, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0013, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0014, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0015, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0016, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0017, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0018, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0019, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001b, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001c, (char *) "CS", (char *) "Dose", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001d, (char *) "IS", (char *) "Side Mark", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x001f, (char *) "DS", (char *) "Exposure Duration", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0021, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0022, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0023, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0024, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0025, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0026, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0027, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0028, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0029, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002b, (char *) "DS", (char *) "Xray Off Position", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002c, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002d, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x002f, (char *) "DS", (char *) "Trigger Frequency", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0031, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0032, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0033, (char *) "UN", (char *) "ECG 2 Offset 2", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0034, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0036, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0038, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0039, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003b, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003c, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x003f, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0040, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0041, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0042, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0043, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0044, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0045, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0046, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0047, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0048, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0049, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004b, (char *) "SL", (char *) "Data Size For Scan Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004c, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x004e, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0050, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0051, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0052, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0053, (char *) "LT", (char *) "Barcode", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0054, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0055, (char *) "DS", (char *) "Receiver Reference Gain", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0056, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0057, (char *) "SS", (char *) "CT Water Number", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0058, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005c, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005d, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x005f, (char *) "SL", (char *) "Increment Between Channels", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0060, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0061, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0062, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0063, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0064, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0065, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0066, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0067, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0068, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0069, (char *) "UL", (char *) "Convolution Mode", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006b, (char *) "SS", (char *) "Field Of View In Detector Cells", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006c, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x006e, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0070, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0071, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0072, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0073, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0074, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0075, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0076, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0077, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0078, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007a, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007c, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007d, (char *) "DS", (char *) "Second Echo", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x007f, (char *) "DS", (char *) "Table Delta", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0080, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0081, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0082, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0083, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0084, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0085, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0086, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0087, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0088, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008b, (char *) "SS", (char *) "Actual Receive Gain Digital", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008c, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008d, (char *) "DS", (char *) "Delay After Trigger", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008e, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x008f, (char *) "SS", (char *) "Swap Phase Frequency", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0090, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0091, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0092, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0093, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0094, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0095, (char *) "SS", (char *) "Analog Receiver Gain", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0096, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0097, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0098, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x0099, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009a, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009b, (char *) "SS", (char *) "Pulse Sequence Mode", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009c, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009d, (char *) "DT", (char *) "Pulse Sequence Date", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x009f, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a0, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a1, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a3, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a4, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a5, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a6, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a7, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a8, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00a9, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00aa, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ab, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ac, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ad, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ae, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00af, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b0, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b1, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b3, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b4, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b5, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b6, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b7, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b8, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00b9, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ba, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00bb, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00bc, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00bd, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00be, (char *) "DS", (char *) "Projection Angle", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c0, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c1, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c3, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c4, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c5, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c6, (char *) "SS", (char *) "SAT Location H", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c7, (char *) "SS", (char *) "SAT Location F", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c8, (char *) "SS", (char *) "SAT Thickness R L", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00c9, (char *) "SS", (char *) "SAT Thickness A P", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ca, (char *) "SS", (char *) "SAT Thickness H F", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cb, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cc, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cd, (char *) "SS", (char *) "Thickness Disclaimer", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ce, (char *) "SS", (char *) "Prescan Type", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00cf, (char *) "SS", (char *) "Prescan Status", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d0, (char *) "SH", (char *) "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d1, (char *) "DS", (char *) "Flow Sensitivity", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d3, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d4, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d5, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d6, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d7, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d8, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00d9, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00da, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00db, (char *) "DS", (char *) "Back Projector Coefficient", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00dc, (char *) "SS", (char *) "Primary Speed Correction Used", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00dd, (char *) "SS", (char *) "Overrange Correction Used", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00de, (char *) "DS", (char *) "Dynamic Z Alpha Value", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00df, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e0, (char *) "DS", (char *) "User Data", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e1, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e3, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e4, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e5, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e6, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e8, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00e9, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00eb, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00ec, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f0, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f1, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f3, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f4, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x00f9, (char *) "DS", (char *) "Transmission Gain", (DicomElemParseFunc *) NULL },
    { 0x0019, 0x1015, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0000, (char *) "UL", (char *) "Relationship Group Length", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x000d, (char *) "UI", (char *) "Study Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x000e, (char *) "UI", (char *) "Series Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0010, (char *) "SH", (char *) "Study ID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0011, (char *) "IS", (char *) "Series Number", (DicomElemParseFunc *) funcDCM_SeriesNumber },
    { 0x0020, 0x0012, (char *) "IS", (char *) "Acquisition Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0013, (char *) "IS", (char *) "Instance (formerly Image) Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0014, (char *) "IS", (char *) "Isotope Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0015, (char *) "IS", (char *) "Phase Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0016, (char *) "IS", (char *) "Interval Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0017, (char *) "IS", (char *) "Time Slot Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0018, (char *) "IS", (char *) "Angle Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0020, (char *) "CS", (char *) "Patient Orientation", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0022, (char *) "IS", (char *) "Overlay Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0024, (char *) "IS", (char *) "Curve Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0026, (char *) "IS", (char *) "LUT Number", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0030, (char *) "DS", (char *) "Image Position", (DicomElemParseFunc *) funcDCM_ImagePosition },
    { 0x0020, 0x0032, (char *) "DS", (char *) "Image Position (Patient)", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0035, (char *) "DS", (char *) "Image Orientation", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0037, (char *) "DS", (char *) "Image Orientation (Patient)", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0050, (char *) "DS", (char *) "Location", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0052, (char *) "UI", (char *) "Frame of Reference UID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0060, (char *) "CS", (char *) "Laterality", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0062, (char *) "CS", (char *) "Image Laterality", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0070, (char *) "LT", (char *) "Image Geometry Type", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0080, (char *) "LO", (char *) "Masking Image", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0100, (char *) "IS", (char *) "Temporal Position Identifier", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0105, (char *) "IS", (char *) "Number of Temporal Positions", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x0110, (char *) "DS", (char *) "Temporal Resolution", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1000, (char *) "IS", (char *) "Series in Study", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1001, (char *) "DS", (char *) "Acquisitions in Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1002, (char *) "IS", (char *) "Images in Acquisition", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1003, (char *) "IS", (char *) "Images in Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1004, (char *) "IS", (char *) "Acquisitions in Study", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1005, (char *) "IS", (char *) "Images in Study", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1020, (char *) "LO", (char *) "Reference", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1040, (char *) "LO", (char *) "Position Reference Indicator", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1041, (char *) "DS", (char *) "Slice Location", (DicomElemParseFunc *) funcDCM_SliceLocation },
    { 0x0020, 0x1070, (char *) "IS", (char *) "Other Study Numbers", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1200, (char *) "IS", (char *) "Number of Patient Related Studies", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1202, (char *) "IS", (char *) "Number of Patient Related Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1204, (char *) "IS", (char *) "Number of Patient Related Images", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1206, (char *) "IS", (char *) "Number of Study Related Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x1208, (char *) "IS", (char *) "Number of Study Related Series", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3100, (char *) "LO", (char *) "Source Image IDs", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3401, (char *) "LO", (char *) "Modifying Device ID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3402, (char *) "LO", (char *) "Modified Image ID", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3403, (char *) "xs", (char *) "Modified Image Date", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3404, (char *) "LO", (char *) "Modifying Device Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3405, (char *) "xs", (char *) "Modified Image Time", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x3406, (char *) "xs", (char *) "Modified Image Description", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x4000, (char *) "LT", (char *) "Image Comments", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x5000, (char *) "AT", (char *) "Original Image Identification", (DicomElemParseFunc *) NULL },
    { 0x0020, 0x5002, (char *) "LO", (char *) "Original Image Identification Nomenclature", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0001, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0002, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0003, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0004, (char *) "DS", (char *) "VOI Position", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0005, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0006, (char *) "IS", (char *) "CSI Matrix Size Original", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0007, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0008, (char *) "DS", (char *) "Spatial Grid Shift", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0009, (char *) "DS", (char *) "Signal Limits Minimum", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0011, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0012, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0013, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0014, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0015, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0016, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0017, (char *) "DS", (char *) "EPI Operation Mode Flag", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0018, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0019, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0021, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0022, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0024, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0025, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0026, (char *) "IS", (char *) "Image Pixel Offset", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0031, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0032, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0034, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0035, (char *) "SS", (char *) "Series From Which Prescribed", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0036, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0037, (char *) "SS", (char *) "Screen Format", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0039, (char *) "DS", (char *) "Slab Thickness", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0040, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0041, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0042, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0043, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0044, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0045, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0046, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0047, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0048, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0049, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x004a, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x004e, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x004f, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0050, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0051, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0052, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0053, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0054, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0055, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0056, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0057, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0058, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0059, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005a, (char *) "SL", (char *) "Integer Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005b, (char *) "DS", (char *) "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005c, (char *) "DS", (char *) "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005d, (char *) "DS", (char *) "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005e, (char *) "DS", (char *) "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x005f, (char *) "DS", (char *) "Float Slop", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0060, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0061, (char *) "DS", (char *) "Image Normal", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0062, (char *) "IS", (char *) "Reference Type Code", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0063, (char *) "DS", (char *) "Image Distance", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0065, (char *) "US", (char *) "Image Positioning History Mask", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x006a, (char *) "DS", (char *) "Image Row", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x006b, (char *) "DS", (char *) "Image Column", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0070, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0071, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0072, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0073, (char *) "DS", (char *) "Second Repetition Time", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0075, (char *) "DS", (char *) "Light Brightness", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0076, (char *) "DS", (char *) "Light Contrast", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x007a, (char *) "IS", (char *) "Overlay Threshold", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x007b, (char *) "IS", (char *) "Surface Threshold", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x007c, (char *) "IS", (char *) "Grey Scale Threshold", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0080, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0081, (char *) "DS", (char *) "Auto Window Level Alpha", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0082, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0083, (char *) "DS", (char *) "Auto Window Level Window", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0084, (char *) "DS", (char *) "Auto Window Level Level", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0090, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0091, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0092, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0093, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0094, (char *) "DS", (char *) "EPI Change Value of X Component", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0095, (char *) "DS", (char *) "EPI Change Value of Y Component", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x0096, (char *) "DS", (char *) "EPI Change Value of Z Component", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a0, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a1, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a2, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a3, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a4, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00a7, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00b0, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0021, 0x00c0, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0001, (char *) "SL", (char *) "Number Of Series In Study", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0002, (char *) "SL", (char *) "Number Of Unarchived Series", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0040, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0050, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0060, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0070, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0074, (char *) "SL", (char *) "Number Of Updates To Info", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x007d, (char *) "SS", (char *) "Indicates If Study Has Complete Info", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0080, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x0090, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0023, 0x00ff, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0000, (char *) "UL", (char *) "Group Length", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0006, (char *) "SS", (char *) "Last Pulse Sequence Used", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0007, (char *) "SL", (char *) "Images In Series", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0010, (char *) "SS", (char *) "Landmark Counter", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0011, (char *) "SS", (char *) "Number Of Acquisitions", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0014, (char *) "SL", (char *) "Indicates Number Of Updates To Info", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0017, (char *) "SL", (char *) "Series Complete Flag", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0018, (char *) "SL", (char *) "Number Of Images Archived", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x0019, (char *) "SL", (char *) "Last Image Number Used", (DicomElemParseFunc *) NULL },
    { 0x0025, 0x001a, (char *) "SH", (char *) "Primary Receiver Suite And Host", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0000, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0006, (char *) "SL", (char *) "Image Archive Flag", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0010, (char *) "SS", (char *) "Scout Type", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0011, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0012, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0013, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0014, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0015, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0016, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001c, (char *) "SL", (char *) "Vma Mamp", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001d, (char *) "SS", (char *) "Vma Phase", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001e, (char *) "SL", (char *) "Vma Mod", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x001f, (char *) "SL", (char *) "Vma Clip", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0020, (char *) "SS", (char *) "Smart Scan On Off Flag", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0030, (char *) "SH", (char *) "Foreign Image Revision", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0031, (char *) "SS", (char *) "Imaging Mode", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0032, (char *) "SS", (char *) "Pulse Sequence", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0033, (char *) "SL", (char *) "Imaging Options", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0035, (char *) "SS", (char *) "Plane Type", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0036, (char *) "SL", (char *) "Oblique Plane", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0040, (char *) "SH", (char *) "RAS Letter Of Image Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0041, (char *) "FL", (char *) "Image Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0042, (char *) "FL", (char *) "Center R Coord Of Plane Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0043, (char *) "FL", (char *) "Center A Coord Of Plane Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0044, (char *) "FL", (char *) "Center S Coord Of Plane Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0045, (char *) "FL", (char *) "Normal R Coord", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0046, (char *) "FL", (char *) "Normal A Coord", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0047, (char *) "FL", (char *) "Normal S Coord", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0048, (char *) "FL", (char *) "R Coord Of Top Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0049, (char *) "FL", (char *) "A Coord Of Top Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004a, (char *) "FL", (char *) "S Coord Of Top Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004b, (char *) "FL", (char *) "R Coord Of Bottom Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004c, (char *) "FL", (char *) "A Coord Of Bottom Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x004d, (char *) "FL", (char *) "S Coord Of Bottom Right Corner", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0050, (char *) "FL", (char *) "Table Start Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0051, (char *) "FL", (char *) "Table End Location", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0052, (char *) "SH", (char *) "RAS Letter For Side Of Image", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0053, (char *) "SH", (char *) "RAS Letter For Anterior Posterior", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0054, (char *) "SH", (char *) "RAS Letter For Scout Start Loc", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0055, (char *) "SH", (char *) "RAS Letter For Scout End Loc", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0060, (char *) "FL", (char *) "Image Dimension X", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0061, (char *) "FL", (char *) "Image Dimension Y", (DicomElemParseFunc *) NULL },
    { 0x0027, 0x0062, (char *) "FL", (char *) "Number Of Excitations", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0000, (char *) "UL", (char *) "Image Presentation Group Length", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0002, (char *) "US", (char *) "Samples per Pixel", (DicomElemParseFunc *) funcDCM_SamplesPerPixel },
    { 0x0028, 0x0004, (char *) "CS", (char *) "Photometric Interpretation", (DicomElemParseFunc *) funcDCM_PhotometricInterpretation },
    { 0x0028, 0x0005, (char *) "US", (char *) "Image Dimensions", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0006, (char *) "US", (char *) "Planar Configuration", (DicomElemParseFunc *) funcDCM_PlanarConfiguration },
    { 0x0028, 0x0008, (char *) "IS", (char *) "Number of Frames", (DicomElemParseFunc *) funcDCM_NumberOfFrames },
    { 0x0028, 0x0009, (char *) "AT", (char *) "Frame Increment Pointer", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0010, (char *) "US", (char *) "Rows", (DicomElemParseFunc *) funcDCM_Rows },
    { 0x0028, 0x0011, (char *) "US", (char *) "Columns", (DicomElemParseFunc *) funcDCM_Columns },
    { 0x0028, 0x0012, (char *) "US", (char *) "Planes", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0014, (char *) "US", (char *) "Ultrasound Color Data Present", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0030, (char *) "DS", (char *) "Pixel Spacing", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0031, (char *) "DS", (char *) "Zoom Factor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0032, (char *) "DS", (char *) "Zoom Center", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0034, (char *) "IS", (char *) "Pixel Aspect Ratio", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0040, (char *) "LO", (char *) "Image Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0050, (char *) "LT", (char *) "Manipulated Image", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0051, (char *) "CS", (char *) "Corrected Image", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x005f, (char *) "LO", (char *) "Compression Recognition Code", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0060, (char *) "LO", (char *) "Compression Code", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0061, (char *) "SH", (char *) "Compression Originator", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0062, (char *) "SH", (char *) "Compression Label", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0063, (char *) "SH", (char *) "Compression Description", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0065, (char *) "LO", (char *) "Compression Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0066, (char *) "AT", (char *) "Compression Step Pointers", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0068, (char *) "US", (char *) "Repeat Interval", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0069, (char *) "US", (char *) "Bits Grouped", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0070, (char *) "US", (char *) "Perimeter Table", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0071, (char *) "xs", (char *) "Perimeter Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0080, (char *) "US", (char *) "Predictor Rows", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0081, (char *) "US", (char *) "Predictor Columns", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0082, (char *) "US", (char *) "Predictor Constants", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0090, (char *) "LO", (char *) "Blocked Pixels", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0091, (char *) "US", (char *) "Block Rows", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0092, (char *) "US", (char *) "Block Columns", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0093, (char *) "US", (char *) "Row Overlap", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0094, (char *) "US", (char *) "Column Overlap", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0100, (char *) "US", (char *) "Bits Allocated", (DicomElemParseFunc *) funcDCM_BitsAllocated },
    { 0x0028, 0x0101, (char *) "US", (char *) "Bits Stored", (DicomElemParseFunc *) funcDCM_BitsStored },
    { 0x0028, 0x0102, (char *) "US", (char *) "High Bit", (DicomElemParseFunc *) funcDCM_HighBit },
    { 0x0028, 0x0103, (char *) "US", (char *) "Pixel Representation", (DicomElemParseFunc *) funcDCM_PixelRepresentation },
    { 0x0028, 0x0104, (char *) "xs", (char *) "Smallest Valid Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0105, (char *) "xs", (char *) "Largest Valid Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0106, (char *) "xs", (char *) "Smallest Image Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0107, (char *) "xs", (char *) "Largest Image Pixel Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0108, (char *) "xs", (char *) "Smallest Pixel Value in Series", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0109, (char *) "xs", (char *) "Largest Pixel Value in Series", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0110, (char *) "xs", (char *) "Smallest Pixel Value in Plane", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0111, (char *) "xs", (char *) "Largest Pixel Value in Plane", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0120, (char *) "xs", (char *) "Pixel Padding Value", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0121, (char *) "xs", (char *) "Pixel Padding Range Limit", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0200, (char *) "xs", (char *) "Image Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0300, (char *) "CS", (char *) "Quality Control Image", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0301, (char *) "CS", (char *) "Burned In Annotation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0400, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0401, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0402, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0403, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0404, (char *) "AT", (char *) "Details of Coefficients", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0700, (char *) "LO", (char *) "DCT Label", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0701, (char *) "LO", (char *) "Data Block Description", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0702, (char *) "AT", (char *) "Data Block", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0710, (char *) "US", (char *) "Normalization Factor Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0720, (char *) "US", (char *) "Zonal Map Number Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0721, (char *) "AT", (char *) "Zonal Map Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0722, (char *) "US", (char *) "Zonal Map Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0730, (char *) "US", (char *) "Adaptive Map Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0740, (char *) "US", (char *) "Code Number Format", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0800, (char *) "LO", (char *) "Code Label", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0802, (char *) "US", (char *) "Number of Tables", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0803, (char *) "AT", (char *) "Code Table Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0804, (char *) "US", (char *) "Bits For Code Word", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x0808, (char *) "AT", (char *) "Image Data Location", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1040, (char *) "CS", (char *) "Pixel Intensity Relationship", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1041, (char *) "SS", (char *) "Pixel Intensity Relationship Sign", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1050, (char *) "DS", (char *) "Window Center", (DicomElemParseFunc *) funcDCM_WindowCenter },
    { 0x0028, 0x1051, (char *) "DS", (char *) "Window Width", (DicomElemParseFunc *) funcDCM_WindowWidth },
    { 0x0028, 0x1052, (char *) "DS", (char *) "Rescale Intercept", (DicomElemParseFunc *) funcDCM_RescaleIntercept },
    { 0x0028, 0x1053, (char *) "DS", (char *) "Rescale Slope", (DicomElemParseFunc *) funcDCM_RescaleSlope },
    { 0x0028, 0x1054, (char *) "LO", (char *) "Rescale Type", (DicomElemParseFunc *) funcDCM_RescaleType },
    { 0x0028, 0x1055, (char *) "LO", (char *) "Window Center & Width Explanation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1080, (char *) "LO", (char *) "Gray Scale", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1090, (char *) "CS", (char *) "Recommended Viewing Mode", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1100, (char *) "xs", (char *) "Gray Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1101, (char *) "xs", (char *) "Red Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) funcDCM_PaletteDescriptor },
    { 0x0028, 0x1102, (char *) "xs", (char *) "Green Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) funcDCM_PaletteDescriptor },
    { 0x0028, 0x1103, (char *) "xs", (char *) "Blue Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) funcDCM_PaletteDescriptor },
    { 0x0028, 0x1111, (char *) "OW", (char *) "Large Red Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1112, (char *) "OW", (char *) "Large Green Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1113, (char *) "OW", (char *) "Large Blue Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1199, (char *) "UI", (char *) "Palette Color Lookup Table UID", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1200, (char *) "xs", (char *) "Gray Lookup Table Data", (DicomElemParseFunc *) funcDCM_LUT },
    { 0x0028, 0x1201, (char *) "OW", (char *) "Red Palette Color Lookup Table Data", (DicomElemParseFunc *) funcDCM_Palette },
    { 0x0028, 0x1202, (char *) "OW", (char *) "Green Palette Color Lookup Table Data", (DicomElemParseFunc *) funcDCM_Palette },
    { 0x0028, 0x1203, (char *) "OW", (char *) "Blue Palette Color Lookup Table Data", (DicomElemParseFunc *) funcDCM_Palette },
    { 0x0028, 0x1211, (char *) "OW", (char *) "Large Red Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1212, (char *) "OW", (char *) "Large Green Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1213, (char *) "OW", (char *) "Large Blue Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1214, (char *) "UI", (char *) "Large Palette Color Lookup Table UID", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1221, (char *) "OW", (char *) "Segmented Red Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1222, (char *) "OW", (char *) "Segmented Green Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1223, (char *) "OW", (char *) "Segmented Blue Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x1300, (char *) "CS", (char *) "Implant Present", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x2110, (char *) "CS", (char *) "Lossy Image Compression", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x2112, (char *) "DS", (char *) "Lossy Image Compression Ratio", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3000, (char *) "SQ", (char *) "Modality LUT Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3002, (char *) "US", (char *) "LUT Descriptor", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3003, (char *) "LO", (char *) "LUT Explanation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3004, (char *) "LO", (char *) "Modality LUT Type", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x3006, (char *) "US", (char *) "LUT Data", (DicomElemParseFunc *) funcDCM_LUT },
    { 0x0028, 0x3010, (char *) "xs", (char *) "VOI LUT Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x4000, (char *) "LT", (char *) "Image Presentation Comments", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x5000, (char *) "SQ", (char *) "Biplane Acquisition Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6010, (char *) "US", (char *) "Representative Frame Number", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6020, (char *) "US", (char *) "Frame Numbers of Interest", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6022, (char *) "LO", (char *) "Frame of Interest Description", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6030, (char *) "US", (char *) "Mask Pointer", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6040, (char *) "US", (char *) "R Wave Pointer", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6100, (char *) "SQ", (char *) "Mask Subtraction Sequence", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6101, (char *) "CS", (char *) "Mask Operation", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6102, (char *) "US", (char *) "Applicable Frame Range", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6110, (char *) "US", (char *) "Mask Frame Numbers", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6112, (char *) "US", (char *) "Contrast Frame Averaging", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6114, (char *) "FL", (char *) "Mask Sub-Pixel Shift", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6120, (char *) "SS", (char *) "TID Offset", (DicomElemParseFunc *) NULL },
    { 0x0028, 0x6190, (char *) "ST", (char *) "Mask Operation Explanation", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0001, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0002, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0003, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0004, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0005, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0006, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0007, (char *) "SL", (char *) "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0008, (char *) "SH", (char *) "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0009, (char *) "SH", (char *) "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000a, (char *) "SS", (char *) "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000c, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000e, (char *) "CS", (char *) "Zoom Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x000f, (char *) "CS", (char *) "Zoom Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0011, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0013, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0015, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0016, (char *) "SL", (char *) "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0017, (char *) "SL", (char *) "Lower Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0018, (char *) "SL", (char *) "Upper Range Of Pixels", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x001a, (char *) "SL", (char *) "Length Of Total Info In Bytes", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x001e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x001f, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0022, (char *) "IS", (char *) "Pixel Quality Value", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0025, (char *) "LT", (char *) "Processed Pixel Data Quality", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0026, (char *) "SS", (char *) "Version Of Info Structure", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0031, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0032, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0033, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0034, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0035, (char *) "SL", (char *) "Advantage Comp Underflow", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0038, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0040, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0041, (char *) "DS", (char *) "Magnifying Glass Rectangle", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0043, (char *) "DS", (char *) "Magnifying Glass Factor", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0044, (char *) "US", (char *) "Magnifying Glass Function", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x004e, (char *) "CS", (char *) "Magnifying Glass Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x004f, (char *) "CS", (char *) "Magnifying Glass Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0050, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0051, (char *) "LT", (char *) "Exposure Code", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0052, (char *) "LT", (char *) "Sort Code", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0053, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0060, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0061, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0067, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0070, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0071, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0072, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0077, (char *) "CS", (char *) "Window Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0078, (char *) "LT", (char *) "ECG Display Printing ID", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0079, (char *) "CS", (char *) "ECG Display Printing", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x007e, (char *) "CS", (char *) "ECG Display Printing Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x007f, (char *) "CS", (char *) "ECG Display Printing Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0080, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0081, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0082, (char *) "IS", (char *) "View Zoom", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0083, (char *) "IS", (char *) "View Transform", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x008e, (char *) "CS", (char *) "Physiological Display Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x008f, (char *) "CS", (char *) "Physiological Display Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0090, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x0099, (char *) "LT", (char *) "Shutter Type", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00a0, (char *) "US", (char *) "Rows of Rectangular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00a1, (char *) "US", (char *) "Columns of Rectangular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00a2, (char *) "US", (char *) "Origin of Rectangular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00b0, (char *) "US", (char *) "Radius of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00b2, (char *) "US", (char *) "Origin of Circular Shutter", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c0, (char *) "LT", (char *) "Functional Shutter ID", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c1, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c3, (char *) "IS", (char *) "Scan Resolution", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00c4, (char *) "IS", (char *) "Field of View", (DicomElemParseFunc *) funcDCM_FieldOfView },
    { 0x0029, 0x00c5, (char *) "LT", (char *) "Field Of Shutter Rectangle", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00ce, (char *) "CS", (char *) "Shutter Enable Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00cf, (char *) "CS", (char *) "Shutter Select Status", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00d0, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00d1, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0029, 0x00d5, (char *) "LT", (char *) "Slice Thickness", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0010, (char *) "LT", (char *) "Request UID", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0012, (char *) "LT", (char *) "Examination Reason", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0030, (char *) "DA", (char *) "Requested Date", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0032, (char *) "TM", (char *) "Worklist Request Start Time", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0033, (char *) "TM", (char *) "Worklist Request End Time", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0045, (char *) "LT", (char *) "Requesting Physician", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x004a, (char *) "TM", (char *) "Requested Time", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0050, (char *) "LT", (char *) "Requested Physician", (DicomElemParseFunc *) NULL },
    { 0x0031, 0x0080, (char *) "LT", (char *) "Requested Location", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0000, (char *) "UL", (char *) "Study Group Length", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x000a, (char *) "CS", (char *) "Study Status ID", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x000c, (char *) "CS", (char *) "Study Priority ID", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0012, (char *) "LO", (char *) "Study ID Issuer", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0032, (char *) "DA", (char *) "Study Verified Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0033, (char *) "TM", (char *) "Study Verified Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0034, (char *) "DA", (char *) "Study Read Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x0035, (char *) "TM", (char *) "Study Read Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1000, (char *) "DA", (char *) "Scheduled Study Start Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1001, (char *) "TM", (char *) "Scheduled Study Start Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1010, (char *) "DA", (char *) "Scheduled Study Stop Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1011, (char *) "TM", (char *) "Scheduled Study Stop Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1020, (char *) "LO", (char *) "Scheduled Study Location", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1021, (char *) "AE", (char *) "Scheduled Study Location AE Title(s)", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1030, (char *) "LO", (char *) "Reason for Study", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1032, (char *) "PN", (char *) "Requesting Physician", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1033, (char *) "LO", (char *) "Requesting Service", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1040, (char *) "DA", (char *) "Study Arrival Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1041, (char *) "TM", (char *) "Study Arrival Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1050, (char *) "DA", (char *) "Study Completion Date", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1051, (char *) "TM", (char *) "Study Completion Time", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1055, (char *) "CS", (char *) "Study Component Status ID", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1060, (char *) "LO", (char *) "Requested Procedure Description", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1064, (char *) "SQ", (char *) "Requested Procedure Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x1070, (char *) "LO", (char *) "Requested Contrast Agent", (DicomElemParseFunc *) NULL },
    { 0x0032, 0x4000, (char *) "LT", (char *) "Study Comments", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0001, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0002, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0005, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0006, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0033, 0x0010, (char *) "LT", (char *) "Patient Study UID", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0010, (char *) "LO", (char *) "ReferringDepartment", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0020, (char *) "US", (char *) "ScreenNumber", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0040, (char *) "SH", (char *) "LeftOrientation", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0042, (char *) "SH", (char *) "RightOrientation", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0050, (char *) "CS", (char *) "Inversion", (DicomElemParseFunc *) NULL },
    { 0x0037, 0x0060, (char *) "US", (char *) "DSA", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0000, (char *) "UL", (char *) "Visit Group Length", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0004, (char *) "SQ", (char *) "Referenced Patient Alias Sequence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0008, (char *) "CS", (char *) "Visit Status ID", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0010, (char *) "LO", (char *) "Admission ID", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0011, (char *) "LO", (char *) "Issuer of Admission ID", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0016, (char *) "LO", (char *) "Route of Admissions", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001a, (char *) "DA", (char *) "Scheduled Admission Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001b, (char *) "TM", (char *) "Scheduled Admission Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001c, (char *) "DA", (char *) "Scheduled Discharge Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001d, (char *) "TM", (char *) "Scheduled Discharge Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x001e, (char *) "LO", (char *) "Scheduled Patient Institution Residence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0020, (char *) "DA", (char *) "Admitting Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0021, (char *) "TM", (char *) "Admitting Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0030, (char *) "DA", (char *) "Discharge Date", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0032, (char *) "TM", (char *) "Discharge Time", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0040, (char *) "LO", (char *) "Discharge Diagnosis Description", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0044, (char *) "SQ", (char *) "Discharge Diagnosis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0050, (char *) "LO", (char *) "Special Needs", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0300, (char *) "LO", (char *) "Current Patient Location", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0400, (char *) "LO", (char *) "Patient's Institution Residence", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x0500, (char *) "LO", (char *) "Patient State", (DicomElemParseFunc *) NULL },
    { 0x0038, 0x4000, (char *) "LT", (char *) "Visit Comments", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0080, (char *) "IS", (char *) "Private Entity Number", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0085, (char *) "DA", (char *) "Private Entity Date", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0090, (char *) "TM", (char *) "Private Entity Time", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x0095, (char *) "LO", (char *) "Private Entity Launch Command", (DicomElemParseFunc *) NULL },
    { 0x0039, 0x00aa, (char *) "CS", (char *) "Private Entity Type", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0002, (char *) "SQ", (char *) "Waveform Sequence", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0005, (char *) "US", (char *) "Waveform Number of Channels", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0010, (char *) "UL", (char *) "Waveform Number of Samples", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x001a, (char *) "DS", (char *) "Sampling Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0020, (char *) "SH", (char *) "Group Label", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0103, (char *) "CS", (char *) "Waveform Sample Value Representation", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0122, (char *) "OB", (char *) "Waveform Padding Value", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0200, (char *) "SQ", (char *) "Channel Definition", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0202, (char *) "IS", (char *) "Waveform Channel Number", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0203, (char *) "SH", (char *) "Channel Label", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0205, (char *) "CS", (char *) "Channel Status", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0208, (char *) "SQ", (char *) "Channel Source", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0209, (char *) "SQ", (char *) "Channel Source Modifiers", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x020a, (char *) "SQ", (char *) "Differential Channel Source", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x020b, (char *) "SQ", (char *) "Differential Channel Source Modifiers", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0210, (char *) "DS", (char *) "Channel Sensitivity", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0211, (char *) "SQ", (char *) "Channel Sensitivity Units", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0212, (char *) "DS", (char *) "Channel Sensitivity Correction Factor", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0213, (char *) "DS", (char *) "Channel Baseline", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0214, (char *) "DS", (char *) "Channel Time Skew", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0215, (char *) "DS", (char *) "Channel Sample Skew", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0216, (char *) "OB", (char *) "Channel Minimum Value", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0217, (char *) "OB", (char *) "Channel Maximum Value", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0218, (char *) "DS", (char *) "Channel Offset", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x021a, (char *) "US", (char *) "Bits Per Sample", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0220, (char *) "DS", (char *) "Filter Low Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0221, (char *) "DS", (char *) "Filter High Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0222, (char *) "DS", (char *) "Notch Filter Frequency", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x0223, (char *) "DS", (char *) "Notch Filter Bandwidth", (DicomElemParseFunc *) NULL },
    { 0x003a, 0x1000, (char *) "OB", (char *) "Waveform Data", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0001, (char *) "AE", (char *) "Scheduled Station AE Title", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0002, (char *) "DA", (char *) "Scheduled Procedure Step Start Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0003, (char *) "TM", (char *) "Scheduled Procedure Step Start Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0004, (char *) "DA", (char *) "Scheduled Procedure Step End Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0005, (char *) "TM", (char *) "Scheduled Procedure Step End Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0006, (char *) "PN", (char *) "Scheduled Performing Physician Name", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0007, (char *) "LO", (char *) "Scheduled Procedure Step Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0008, (char *) "SQ", (char *) "Scheduled Action Item Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0009, (char *) "SH", (char *) "Scheduled Procedure Step ID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0010, (char *) "SH", (char *) "Scheduled Station Name", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0011, (char *) "SH", (char *) "Scheduled Procedure Step Location", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0012, (char *) "LO", (char *) "Pre-Medication", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0020, (char *) "CS", (char *) "Scheduled Procedure Step Status", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0100, (char *) "SQ", (char *) "Scheduled Procedure Step Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0302, (char *) "US", (char *) "Entrance Dose", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0303, (char *) "US", (char *) "Exposed Area", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0306, (char *) "DS", (char *) "Distance Source to Entrance", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0307, (char *) "DS", (char *) "Distance Source to Support", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0310, (char *) "ST", (char *) "Comments On Radiation Dose", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0312, (char *) "DS", (char *) "X-Ray Output", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0314, (char *) "DS", (char *) "Half Value Layer", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0316, (char *) "DS", (char *) "Organ Dose", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0318, (char *) "CS", (char *) "Organ Exposed", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0400, (char *) "LT", (char *) "Comments On Scheduled Procedure Step", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x050a, (char *) "LO", (char *) "Specimen Accession Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0550, (char *) "SQ", (char *) "Specimen Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0551, (char *) "LO", (char *) "Specimen Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0552, (char *) "SQ", (char *) "Specimen Description Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0553, (char *) "ST", (char *) "Specimen Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0555, (char *) "SQ", (char *) "Acquisition Context Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x0556, (char *) "ST", (char *) "Acquisition Context Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x059a, (char *) "SQ", (char *) "Specimen Type Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x06fa, (char *) "LO", (char *) "Slide Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x071a, (char *) "SQ", (char *) "Image Center Point Coordinates Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x072a, (char *) "DS", (char *) "X Offset In Slide Coordinate System", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x073a, (char *) "DS", (char *) "Y Offset In Slide Coordinate System", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x074a, (char *) "DS", (char *) "Z Offset In Slide Coordinate System", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x08d8, (char *) "SQ", (char *) "Pixel Spacing Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x08da, (char *) "SQ", (char *) "Coordinate System Axis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x08ea, (char *) "SQ", (char *) "Measurement Units Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x09f8, (char *) "SQ", (char *) "Vital Stain Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1001, (char *) "SH", (char *) "Requested Procedure ID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1002, (char *) "LO", (char *) "Reason For Requested Procedure", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1003, (char *) "SH", (char *) "Requested Procedure Priority", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1004, (char *) "LO", (char *) "Patient Transport Arrangements", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1005, (char *) "LO", (char *) "Requested Procedure Location", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1006, (char *) "SH", (char *) "Placer Order Number of Procedure", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1007, (char *) "SH", (char *) "Filler Order Number of Procedure", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1008, (char *) "LO", (char *) "Confidentiality Code", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1009, (char *) "SH", (char *) "Reporting Priority", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1010, (char *) "PN", (char *) "Names of Intended Recipients of Results", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x1400, (char *) "LT", (char *) "Requested Procedure Comments", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2001, (char *) "LO", (char *) "Reason For Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2004, (char *) "DA", (char *) "Issue Date of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2005, (char *) "TM", (char *) "Issue Time of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2006, (char *) "SH", (char *) "Placer Order Number of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2007, (char *) "SH", (char *) "Filler Order Number of Imaging Service Request", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2008, (char *) "PN", (char *) "Order Entered By", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2009, (char *) "SH", (char *) "Order Enterer Location", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2010, (char *) "SH", (char *) "Order Callback Phone Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x2400, (char *) "LT", (char *) "Imaging Service Request Comments", (DicomElemParseFunc *) NULL },
    { 0x0040, 0x3001, (char *) "LO", (char *) "Confidentiality Constraint On Patient Data", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa007, (char *) "CS", (char *) "Findings Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa020, (char *) "SQ", (char *) "Findings Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa021, (char *) "UI", (char *) "Findings Group UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa022, (char *) "UI", (char *) "Referenced Findings Group UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa023, (char *) "DA", (char *) "Findings Group Recording Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa024, (char *) "TM", (char *) "Findings Group Recording Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa026, (char *) "SQ", (char *) "Findings Source Category Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa027, (char *) "LO", (char *) "Documenting Organization", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa028, (char *) "SQ", (char *) "Documenting Organization Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa032, (char *) "LO", (char *) "History Reliability Qualifier Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa043, (char *) "SQ", (char *) "Concept Name Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa047, (char *) "LO", (char *) "Measurement Precision Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa057, (char *) "CS", (char *) "Urgency or Priority Alerts", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa060, (char *) "LO", (char *) "Sequencing Indicator", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa066, (char *) "SQ", (char *) "Document Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa067, (char *) "PN", (char *) "Document Author", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa068, (char *) "SQ", (char *) "Document Author Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa070, (char *) "SQ", (char *) "Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa073, (char *) "LO", (char *) "Object String Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa074, (char *) "OB", (char *) "Object Binary Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa075, (char *) "PN", (char *) "Documenting Observer", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa076, (char *) "SQ", (char *) "Documenting Observer Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa078, (char *) "SQ", (char *) "Observation Subject Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa080, (char *) "SQ", (char *) "Person Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa085, (char *) "SQ", (char *) "Procedure Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa088, (char *) "LO", (char *) "Object Directory String Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa089, (char *) "OB", (char *) "Object Directory Binary Identifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa090, (char *) "CS", (char *) "History Reliability Qualifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa0a0, (char *) "CS", (char *) "Referenced Type of Data", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa0b0, (char *) "US", (char *) "Referenced Waveform Channels", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa110, (char *) "DA", (char *) "Date of Document or Verbal Transaction", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa112, (char *) "TM", (char *) "Time of Document Creation or Verbal Transaction", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa121, (char *) "DA", (char *) "Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa122, (char *) "TM", (char *) "Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa123, (char *) "PN", (char *) "Person Name", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa124, (char *) "SQ", (char *) "Referenced Person Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa125, (char *) "CS", (char *) "Report Status ID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa130, (char *) "CS", (char *) "Temporal Range Type", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa132, (char *) "UL", (char *) "Referenced Sample Offsets", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa136, (char *) "US", (char *) "Referenced Frame Numbers", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa138, (char *) "DS", (char *) "Referenced Time Offsets", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa13a, (char *) "DT", (char *) "Referenced Datetime", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa160, (char *) "UT", (char *) "Text Value", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa167, (char *) "SQ", (char *) "Observation Category Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa168, (char *) "SQ", (char *) "Concept Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa16a, (char *) "ST", (char *) "Bibliographic Citation", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa170, (char *) "CS", (char *) "Observation Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa171, (char *) "UI", (char *) "Observation UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa172, (char *) "UI", (char *) "Referenced Observation UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa173, (char *) "CS", (char *) "Referenced Observation Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa174, (char *) "CS", (char *) "Referenced Object Observation Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa180, (char *) "US", (char *) "Annotation Group Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa192, (char *) "DA", (char *) "Observation Date", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa193, (char *) "TM", (char *) "Observation Time", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa194, (char *) "CS", (char *) "Measurement Automation", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa195, (char *) "SQ", (char *) "Concept Name Code Sequence Modifier", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa224, (char *) "ST", (char *) "Identification Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa290, (char *) "CS", (char *) "Coordinates Set Geometric Type", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa296, (char *) "SQ", (char *) "Algorithm Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa297, (char *) "ST", (char *) "Algorithm Description", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa29a, (char *) "SL", (char *) "Pixel Coordinates Set", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa300, (char *) "SQ", (char *) "Measured Value Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa307, (char *) "PN", (char *) "Current Observer", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa30a, (char *) "DS", (char *) "Numeric Value", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa313, (char *) "SQ", (char *) "Referenced Accession Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa33a, (char *) "ST", (char *) "Report Status Comment", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa340, (char *) "SQ", (char *) "Procedure Context Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa352, (char *) "PN", (char *) "Verbal Source", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa353, (char *) "ST", (char *) "Address", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa354, (char *) "LO", (char *) "Telephone Number", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa358, (char *) "SQ", (char *) "Verbal Source Identifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa380, (char *) "SQ", (char *) "Report Detail Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa402, (char *) "UI", (char *) "Observation Subject UID", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa403, (char *) "CS", (char *) "Observation Subject Class", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa404, (char *) "SQ", (char *) "Observation Subject Type Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa600, (char *) "CS", (char *) "Observation Subject Context Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa601, (char *) "CS", (char *) "Observer Context Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa603, (char *) "CS", (char *) "Procedure Context Flag", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa730, (char *) "SQ", (char *) "Observations Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa731, (char *) "SQ", (char *) "Relationship Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa732, (char *) "SQ", (char *) "Relationship Type Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa744, (char *) "SQ", (char *) "Language Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xa992, (char *) "ST", (char *) "Uniform Resource Locator", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xb020, (char *) "SQ", (char *) "Annotation Sequence", (DicomElemParseFunc *) NULL },
    { 0x0040, 0xdb73, (char *) "SQ", (char *) "Relationship Type Code Sequence Modifier", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0000, (char *) "LT", (char *) "Papyrus Comments", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0011, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0012, (char *) "UL", (char *) "Pixel Offset", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0013, (char *) "SQ", (char *) "Image Identifier Sequence", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0014, (char *) "SQ", (char *) "External File Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0015, (char *) "US", (char *) "Number of Images", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0020, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0021, (char *) "UI", (char *) "Referenced SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0022, (char *) "UI", (char *) "Referenced SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0031, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0032, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0034, (char *) "DA", (char *) "Modified Date", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0036, (char *) "TM", (char *) "Modified Time", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0040, (char *) "LT", (char *) "Owner Name", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0041, (char *) "UI", (char *) "Referenced Image SOP Class UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0042, (char *) "UI", (char *) "Referenced Image SOP Instance UID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0050, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0060, (char *) "UL", (char *) "Number of Images", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x0062, (char *) "UL", (char *) "Number of Other", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a0, (char *) "LT", (char *) "External Folder Element DSID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a1, (char *) "US", (char *) "External Folder Element Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a2, (char *) "LT", (char *) "External Folder Element File Location", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00a3, (char *) "UL", (char *) "External Folder Element Length", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b0, (char *) "LT", (char *) "Internal Folder Element DSID", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b1, (char *) "US", (char *) "Internal Folder Element Data Set Type", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b2, (char *) "UL", (char *) "Internal Offset To Data Set", (DicomElemParseFunc *) NULL },
    { 0x0041, 0x00b3, (char *) "UL", (char *) "Internal Offset To Image", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0001, (char *) "SS", (char *) "Bitmap Of Prescan Options", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0002, (char *) "SS", (char *) "Gradient Offset In X", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0003, (char *) "SS", (char *) "Gradient Offset In Y", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0004, (char *) "SS", (char *) "Gradient Offset In Z", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0005, (char *) "SS", (char *) "Image Is Original Or Unoriginal", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0006, (char *) "SS", (char *) "Number Of EPI Shots", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0007, (char *) "SS", (char *) "Views Per Segment", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0008, (char *) "SS", (char *) "Respiratory Rate In BPM", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0009, (char *) "SS", (char *) "Respiratory Trigger Point", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000a, (char *) "SS", (char *) "Type Of Receiver Used", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000b, (char *) "DS", (char *) "Peak Rate Of Change Of Gradient Field", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000c, (char *) "DS", (char *) "Limits In Units Of Percent", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000d, (char *) "DS", (char *) "PSD Estimated Limit", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000e, (char *) "DS", (char *) "PSD Estimated Limit In Tesla Per Second", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x000f, (char *) "DS", (char *) "SAR Avg Head", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0010, (char *) "US", (char *) "Window Value", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0011, (char *) "US", (char *) "Total Input Views", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0012, (char *) "SS", (char *) "Xray Chain", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0013, (char *) "SS", (char *) "Recon Kernel Parameters", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0014, (char *) "SS", (char *) "Calibration Parameters", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0015, (char *) "SS", (char *) "Total Output Views", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0016, (char *) "SS", (char *) "Number Of Overranges", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0017, (char *) "DS", (char *) "IBH Image Scale Factors", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0018, (char *) "DS", (char *) "BBH Coefficients", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0019, (char *) "SS", (char *) "Number Of BBH Chains To Blend", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001a, (char *) "SL", (char *) "Starting Channel Number", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001b, (char *) "SS", (char *) "PPScan Parameters", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001c, (char *) "SS", (char *) "GE Image Integrity", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001d, (char *) "SS", (char *) "Level Value", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001e, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x001f, (char *) "SL", (char *) "Max Overranges In A View", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0020, (char *) "DS", (char *) "Avg Overranges All Views", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0021, (char *) "SS", (char *) "Corrected Afterglow Terms", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0025, (char *) "SS", (char *) "Reference Channels", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0026, (char *) "US", (char *) "No Views Ref Channels Blocked", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0027, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0028, (char *) "OB", (char *) "Unique Image Identifier", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0029, (char *) "OB", (char *) "Histogram Tables", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002a, (char *) "OB", (char *) "User Defined Data", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002b, (char *) "SS", (char *) "Private Scan Options", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002c, (char *) "SS", (char *) "Effective Echo Spacing", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002d, (char *) "SH", (char *) "String Slop Field 1", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002e, (char *) "SH", (char *) "String Slop Field 2", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x002f, (char *) "SS", (char *) "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0030, (char *) "SS", (char *) "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0031, (char *) "DS", (char *) "RA Coord Of Target Recon Centre", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0032, (char *) "SS", (char *) "Raw Data Type", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0033, (char *) "FL", (char *) "Neg Scan Spacing", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0034, (char *) "IS", (char *) "Offset Frequency", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0035, (char *) "UL", (char *) "User Usage Tag", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0036, (char *) "UL", (char *) "User Fill Map MSW", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0037, (char *) "UL", (char *) "User Fill Map LSW", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0038, (char *) "FL", (char *) "User 25 To User 48", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0039, (char *) "IS", (char *) "Slop Integer 6 To Slop Integer 9", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0040, (char *) "FL", (char *) "Trigger On Position", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0041, (char *) "FL", (char *) "Degree Of Rotation", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0042, (char *) "SL", (char *) "DAS Trigger Source", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0043, (char *) "SL", (char *) "DAS Fpa Gain", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0044, (char *) "SL", (char *) "DAS Output Source", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0045, (char *) "SL", (char *) "DAS Ad Input", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0046, (char *) "SL", (char *) "DAS Cal Mode", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0047, (char *) "SL", (char *) "DAS Cal Frequency", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0048, (char *) "SL", (char *) "DAS Reg Xm", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x0049, (char *) "SL", (char *) "DAS Auto Zero", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004a, (char *) "SS", (char *) "Starting Channel Of View", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004b, (char *) "SL", (char *) "DAS Xm Pattern", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004c, (char *) "SS", (char *) "TGGC Trigger Mode", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004d, (char *) "FL", (char *) "Start Scan To Xray On Delay", (DicomElemParseFunc *) NULL },
    { 0x0043, 0x004e, (char *) "FL", (char *) "Duration Of Xray On", (DicomElemParseFunc *) NULL },
    { 0x0044, 0x0000, (char *) "UI", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0004, (char *) "CS", (char *) "AES", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0006, (char *) "DS", (char *) "Angulation", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0009, (char *) "DS", (char *) "Real Magnification Factor", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x000b, (char *) "CS", (char *) "Senograph Type", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x000c, (char *) "DS", (char *) "Integration Time", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x000d, (char *) "DS", (char *) "ROI Origin X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0011, (char *) "DS", (char *) "Receptor Size cm X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0012, (char *) "IS", (char *) "Receptor Size Pixels X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0013, (char *) "ST", (char *) "Screen", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0014, (char *) "DS", (char *) "Pixel Pitch Microns", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0015, (char *) "IS", (char *) "Pixel Depth Bits", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0016, (char *) "IS", (char *) "Binning Factor X and Y", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001b, (char *) "CS", (char *) "Clinical View", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001d, (char *) "DS", (char *) "Mean Of Raw Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001e, (char *) "DS", (char *) "Mean Of Offset Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x001f, (char *) "DS", (char *) "Mean Of Corrected Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0020, (char *) "DS", (char *) "Mean Of Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0021, (char *) "DS", (char *) "Mean Of Log Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0022, (char *) "DS", (char *) "Standard Deviation Of Raw Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0023, (char *) "DS", (char *) "Standard Deviation Of Corrected Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0024, (char *) "DS", (char *) "Standard Deviation Of Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0025, (char *) "DS", (char *) "Standard Deviation Of Log Region Gray Levels", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0026, (char *) "OB", (char *) "MAO Buffer", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0027, (char *) "IS", (char *) "Set Number", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0028, (char *) "CS", (char *) "WindowingType (LINEAR or GAMMA)", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0029, (char *) "DS", (char *) "WindowingParameters", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x002a, (char *) "IS", (char *) "Crosshair Cursor X Coordinates", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x002b, (char *) "IS", (char *) "Crosshair Cursor Y Coordinates", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x0039, (char *) "US", (char *) "Vignette Rows", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003a, (char *) "US", (char *) "Vignette Columns", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003b, (char *) "US", (char *) "Vignette Bits Allocated", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003c, (char *) "US", (char *) "Vignette Bits Stored", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003d, (char *) "US", (char *) "Vignette High Bit", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003e, (char *) "US", (char *) "Vignette Pixel Representation", (DicomElemParseFunc *) NULL },
    { 0x0045, 0x003f, (char *) "OB", (char *) "Vignette Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0001, (char *) "SQ", (char *) "Reconstruction Parameters Sequence", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0050, (char *) "UL", (char *) "Volume Voxel Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0051, (char *) "UL", (char *) "Volume Segment Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0053, (char *) "US", (char *) "Volume Slice Size", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0054, (char *) "US", (char *) "Volume Slice Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0055, (char *) "SL", (char *) "Volume Threshold Value", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0057, (char *) "DS", (char *) "Volume Voxel Ratio", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0058, (char *) "DS", (char *) "Volume Voxel Size", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0059, (char *) "US", (char *) "Volume Z Position Size", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0060, (char *) "DS", (char *) "Volume Base Line", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0061, (char *) "DS", (char *) "Volume Center Point", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0063, (char *) "SL", (char *) "Volume Skew Base", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0064, (char *) "DS", (char *) "Volume Registration Transform Rotation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0065, (char *) "DS", (char *) "Volume Registration Transform Translation Vector", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0070, (char *) "DS", (char *) "KVP List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0071, (char *) "IS", (char *) "XRay Tube Current List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0072, (char *) "IS", (char *) "Exposure List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0080, (char *) "LO", (char *) "Acquisition DLX Identifier", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0085, (char *) "SQ", (char *) "Acquisition DLX 2D Series Sequence", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0089, (char *) "DS", (char *) "Contrast Agent Volume List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x008a, (char *) "US", (char *) "Number Of Injections", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x008b, (char *) "US", (char *) "Frame Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0096, (char *) "IS", (char *) "Used Frames", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0091, (char *) "LO", (char *) "XA 3D Reconstruction Algorithm Name", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0092, (char *) "CS", (char *) "XA 3D Reconstruction Algorithm Version", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0093, (char *) "DA", (char *) "DLX Calibration Date", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0094, (char *) "TM", (char *) "DLX Calibration Time", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0095, (char *) "CS", (char *) "DLX Calibration Status", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0098, (char *) "US", (char *) "Transform Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x0099, (char *) "SQ", (char *) "Transform Sequence", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x009a, (char *) "DS", (char *) "Transform Rotation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x009b, (char *) "DS", (char *) "Transform Translation Vector", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x009c, (char *) "LO", (char *) "Transform Label", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b1, (char *) "US", (char *) "Wireframe Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b2, (char *) "US", (char *) "Location System", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b0, (char *) "SQ", (char *) "Wireframe List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b5, (char *) "LO", (char *) "Wireframe Name", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b6, (char *) "LO", (char *) "Wireframe Group Name", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b7, (char *) "LO", (char *) "Wireframe Color", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b8, (char *) "SL", (char *) "Wireframe Attributes", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00b9, (char *) "SL", (char *) "Wireframe Point Count", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00ba, (char *) "SL", (char *) "Wireframe Timestamp", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00bb, (char *) "SQ", (char *) "Wireframe Point List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00bc, (char *) "DS", (char *) "Wireframe Points Coordinates", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00c0, (char *) "DS", (char *) "Volume Upper Left High Corner RAS", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00c1, (char *) "DS", (char *) "Volume Slice To RAS Rotation Matrix", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00c2, (char *) "DS", (char *) "Volume Upper Left High Corner TLOC", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d1, (char *) "OB", (char *) "Volume Segment List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d2, (char *) "OB", (char *) "Volume Gradient List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d3, (char *) "OB", (char *) "Volume Density List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d4, (char *) "OB", (char *) "Volume Z Position List", (DicomElemParseFunc *) NULL },
    { 0x0047, 0x00d5, (char *) "OB", (char *) "Volume Original Index List", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0000, (char *) "UL", (char *) "Calibration Group Length", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0004, (char *) "CS", (char *) "Calibration Object", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0010, (char *) "SQ", (char *) "DeviceSequence", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0014, (char *) "DS", (char *) "DeviceLength", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0016, (char *) "DS", (char *) "DeviceDiameter", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0017, (char *) "CS", (char *) "DeviceDiameterUnits", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0018, (char *) "DS", (char *) "DeviceVolume", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0019, (char *) "DS", (char *) "InterMarkerDistance", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0020, (char *) "LO", (char *) "DeviceDescription", (DicomElemParseFunc *) NULL },
    { 0x0050, 0x0030, (char *) "SQ", (char *) "CodedInterventionDeviceSequence", (DicomElemParseFunc *) NULL },
    { 0x0051, 0x0010, (char *) "xs", (char *) "Image Text", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0000, (char *) "UL", (char *) "Nuclear Acquisition Group Length", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0010, (char *) "US", (char *) "Energy Window Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0011, (char *) "US", (char *) "Number of Energy Windows", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0012, (char *) "SQ", (char *) "Energy Window Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0013, (char *) "SQ", (char *) "Energy Window Range Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0014, (char *) "DS", (char *) "Energy Window Lower Limit", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0015, (char *) "DS", (char *) "Energy Window Upper Limit", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0016, (char *) "SQ", (char *) "Radiopharmaceutical Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0017, (char *) "IS", (char *) "Residual Syringe Counts", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0018, (char *) "SH", (char *) "Energy Window Name", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0020, (char *) "US", (char *) "Detector Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0021, (char *) "US", (char *) "Number of Detectors", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0022, (char *) "SQ", (char *) "Detector Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0030, (char *) "US", (char *) "Phase Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0031, (char *) "US", (char *) "Number of Phases", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0032, (char *) "SQ", (char *) "Phase Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0033, (char *) "US", (char *) "Number of Frames In Phase", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0036, (char *) "IS", (char *) "Phase Delay", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0038, (char *) "IS", (char *) "Pause Between Frames", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0050, (char *) "US", (char *) "Rotation Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0051, (char *) "US", (char *) "Number of Rotations", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0052, (char *) "SQ", (char *) "Rotation Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0053, (char *) "US", (char *) "Number of Frames In Rotation", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0060, (char *) "US", (char *) "R-R Interval Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0061, (char *) "US", (char *) "Number of R-R Intervals", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0062, (char *) "SQ", (char *) "Gated Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0063, (char *) "SQ", (char *) "Data Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0070, (char *) "US", (char *) "Time Slot Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0071, (char *) "US", (char *) "Number of Time Slots", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0072, (char *) "SQ", (char *) "Time Slot Information Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0073, (char *) "DS", (char *) "Time Slot Time", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0080, (char *) "US", (char *) "Slice Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0081, (char *) "US", (char *) "Number of Slices", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0090, (char *) "US", (char *) "Angular View Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0100, (char *) "US", (char *) "Time Slice Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0101, (char *) "US", (char *) "Number Of Time Slices", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0200, (char *) "DS", (char *) "Start Angle", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0202, (char *) "CS", (char *) "Type of Detector Motion", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0210, (char *) "IS", (char *) "Trigger Vector", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0211, (char *) "US", (char *) "Number of Triggers in Phase", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0220, (char *) "SQ", (char *) "View Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0222, (char *) "SQ", (char *) "View Modifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0300, (char *) "SQ", (char *) "Radionuclide Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0302, (char *) "SQ", (char *) "Radiopharmaceutical Route Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0304, (char *) "SQ", (char *) "Radiopharmaceutical Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0306, (char *) "SQ", (char *) "Calibration Data Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0308, (char *) "US", (char *) "Energy Window Number", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0400, (char *) "SH", (char *) "Image ID", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0410, (char *) "SQ", (char *) "Patient Orientation Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0412, (char *) "SQ", (char *) "Patient Orientation Modifier Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x0414, (char *) "SQ", (char *) "Patient Gantry Relationship Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1000, (char *) "CS", (char *) "Positron Emission Tomography Series Type", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1001, (char *) "CS", (char *) "Positron Emission Tomography Units", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1002, (char *) "CS", (char *) "Counts Source", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1004, (char *) "CS", (char *) "Reprojection Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1100, (char *) "CS", (char *) "Randoms Correction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1101, (char *) "LO", (char *) "Attenuation Correction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1102, (char *) "CS", (char *) "Decay Correction", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1103, (char *) "LO", (char *) "Reconstruction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1104, (char *) "LO", (char *) "Detector Lines of Response Used", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1105, (char *) "LO", (char *) "Scatter Correction Method", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1200, (char *) "DS", (char *) "Axial Acceptance", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1201, (char *) "IS", (char *) "Axial Mash", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1202, (char *) "IS", (char *) "Transverse Mash", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1203, (char *) "DS", (char *) "Detector Element Size", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1210, (char *) "DS", (char *) "Coincidence Window Width", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1220, (char *) "CS", (char *) "Secondary Counts Type", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1300, (char *) "DS", (char *) "Frame Reference Time", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1310, (char *) "IS", (char *) "Primary Prompts Counts Accumulated", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1311, (char *) "IS", (char *) "Secondary Counts Accumulated", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1320, (char *) "DS", (char *) "Slice Sensitivity Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1321, (char *) "DS", (char *) "Decay Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1322, (char *) "DS", (char *) "Dose Calibration Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1323, (char *) "DS", (char *) "Scatter Fraction Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1324, (char *) "DS", (char *) "Dead Time Factor", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1330, (char *) "US", (char *) "Image Index", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1400, (char *) "CS", (char *) "Counts Included", (DicomElemParseFunc *) NULL },
    { 0x0054, 0x1401, (char *) "CS", (char *) "Dead Time Correction Flag", (DicomElemParseFunc *) NULL },
    { 0x0055, 0x0046, (char *) "LT", (char *) "Current Ward", (DicomElemParseFunc *) NULL },
    { 0x0058, 0x0000, (char *) "SQ", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3000, (char *) "SQ", (char *) "Histogram Sequence", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3002, (char *) "US", (char *) "Histogram Number of Bins", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3004, (char *) "xs", (char *) "Histogram First Bin Value", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3006, (char *) "xs", (char *) "Histogram Last Bin Value", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3008, (char *) "US", (char *) "Histogram Bin Width", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3010, (char *) "LO", (char *) "Histogram Explanation", (DicomElemParseFunc *) NULL },
    { 0x0060, 0x3020, (char *) "UL", (char *) "Histogram Data", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0001, (char *) "SQ", (char *) "Graphic Annotation Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0002, (char *) "CS", (char *) "Graphic Layer", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0003, (char *) "CS", (char *) "Bounding Box Annotation Units", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0004, (char *) "CS", (char *) "Anchor Point Annotation Units", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0005, (char *) "CS", (char *) "Graphic Annotation Units", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0006, (char *) "ST", (char *) "Unformatted Text Value", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0008, (char *) "SQ", (char *) "Text Object Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0009, (char *) "SQ", (char *) "Graphic Object Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0010, (char *) "FL", (char *) "Bounding Box TLHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0011, (char *) "FL", (char *) "Bounding Box BRHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0014, (char *) "FL", (char *) "Anchor Point", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0015, (char *) "CS", (char *) "Anchor Point Visibility", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0020, (char *) "US", (char *) "Graphic Dimensions", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0021, (char *) "US", (char *) "Number Of Graphic Points", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0022, (char *) "FL", (char *) "Graphic Data", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0023, (char *) "CS", (char *) "Graphic Type", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0024, (char *) "CS", (char *) "Graphic Filled", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0040, (char *) "IS", (char *) "Image Rotation", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0041, (char *) "CS", (char *) "Image Horizontal Flip", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0050, (char *) "US", (char *) "Displayed Area TLHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0051, (char *) "US", (char *) "Displayed Area BRHC", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0060, (char *) "SQ", (char *) "Graphic Layer Sequence", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0062, (char *) "IS", (char *) "Graphic Layer Order", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0066, (char *) "US", (char *) "Graphic Layer Recommended Display Value", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0068, (char *) "LO", (char *) "Graphic Layer Description", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0080, (char *) "CS", (char *) "Presentation Label", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0081, (char *) "LO", (char *) "Presentation Description", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0082, (char *) "DA", (char *) "Presentation Creation Date", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0083, (char *) "TM", (char *) "Presentation Creation Time", (DicomElemParseFunc *) NULL },
    { 0x0070, 0x0084, (char *) "PN", (char *) "Presentation Creator's Name", (DicomElemParseFunc *) NULL },
    { 0x0087, 0x0010, (char *) "CS", (char *) "Media Type", (DicomElemParseFunc *) NULL },
    { 0x0087, 0x0020, (char *) "CS", (char *) "Media Location", (DicomElemParseFunc *) NULL },
    { 0x0087, 0x0050, (char *) "IS", (char *) "Estimated Retrieve Time", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0000, (char *) "UL", (char *) "Storage Group Length", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0130, (char *) "SH", (char *) "Storage Media FileSet ID", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0140, (char *) "UI", (char *) "Storage Media FileSet UID", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0200, (char *) "SQ", (char *) "Icon Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0904, (char *) "LO", (char *) "Topic Title", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0906, (char *) "ST", (char *) "Topic Subject", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0910, (char *) "LO", (char *) "Topic Author", (DicomElemParseFunc *) NULL },
    { 0x0088, 0x0912, (char *) "LO", (char *) "Topic Key Words", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0001, (char *) "LT", (char *) "Examination Folder ID", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0004, (char *) "UL", (char *) "Folder Reported Status", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0005, (char *) "LT", (char *) "Folder Reporting Radiologist", (DicomElemParseFunc *) NULL },
    { 0x0095, 0x0007, (char *) "LT", (char *) "SIENET ISA PLA", (DicomElemParseFunc *) NULL },
    { 0x0099, 0x0002, (char *) "UL", (char *) "Data Object Attributes", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0001, (char *) "US", (char *) "Data Dictionary Version", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0014, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0022, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0023, (char *) "DS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0024, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0025, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x00e1, 0x0040, (char *) "SH", (char *) "Offset From CT MR Images", (DicomElemParseFunc *) NULL },
    { 0x0193, 0x0002, (char *) "DS", (char *) "RIS Key", (DicomElemParseFunc *) NULL },
    { 0x0307, 0x0001, (char *) "UN", (char *) "RIS Worklist IMGEF", (DicomElemParseFunc *) NULL },
    { 0x0309, 0x0001, (char *) "UN", (char *) "RIS Report IMGEF", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0000, (char *) "SH", (char *) "Implementation Version", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0020, (char *) "DS", (char *) "Relative Table Position", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0021, (char *) "DS", (char *) "Relative Table Height", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0030, (char *) "SH", (char *) "Surview Direction", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0031, (char *) "DS", (char *) "Surview Length", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0050, (char *) "SH", (char *) "Image View Type", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0070, (char *) "DS", (char *) "Batch Number", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0071, (char *) "DS", (char *) "Batch Size", (DicomElemParseFunc *) NULL },
    { 0x0601, 0x0072, (char *) "DS", (char *) "Batch Slice Number", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0001, (char *) "US", (char *) "Run Length Triplet", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0002, (char *) "US", (char *) "Huffman Table Size", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0003, (char *) "US", (char *) "Huffman Table Triplet", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0004, (char *) "US", (char *) "Shift Table Size", (DicomElemParseFunc *) NULL },
    { 0x1000, 0x0005, (char *) "US", (char *) "Shift Table Triplet", (DicomElemParseFunc *) NULL },
    { 0x1010, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x1369, 0x0000, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0000, (char *) "UL", (char *) "Film Session Group Length", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0010, (char *) "IS", (char *) "Number of Copies", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0020, (char *) "CS", (char *) "Print Priority", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0030, (char *) "CS", (char *) "Medium Type", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0040, (char *) "CS", (char *) "Film Destination", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0050, (char *) "LO", (char *) "Film Session Label", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0060, (char *) "IS", (char *) "Memory Allocation", (DicomElemParseFunc *) NULL },
    { 0x2000, 0x0500, (char *) "SQ", (char *) "Referenced Film Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0000, (char *) "UL", (char *) "Film Box Group Length", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0010, (char *) "ST", (char *) "Image Display Format", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0030, (char *) "CS", (char *) "Annotation Display Format ID", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0040, (char *) "CS", (char *) "Film Orientation", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0050, (char *) "CS", (char *) "Film Size ID", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0060, (char *) "CS", (char *) "Magnification Type", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0080, (char *) "CS", (char *) "Smoothing Type", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0100, (char *) "CS", (char *) "Border Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0110, (char *) "CS", (char *) "Empty Image Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0120, (char *) "US", (char *) "Min Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0130, (char *) "US", (char *) "Max Density", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0140, (char *) "CS", (char *) "Trim", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0150, (char *) "ST", (char *) "Configuration Information", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0500, (char *) "SQ", (char *) "Referenced Film Session Sequence", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0510, (char *) "SQ", (char *) "Referenced Image Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2010, 0x0520, (char *) "SQ", (char *) "Referenced Basic Annotation Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0000, (char *) "UL", (char *) "Image Box Group Length", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0010, (char *) "US", (char *) "Image Box Position", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0020, (char *) "CS", (char *) "Polarity", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0030, (char *) "DS", (char *) "Requested Image Size", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0110, (char *) "SQ", (char *) "Preformatted Grayscale Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0111, (char *) "SQ", (char *) "Preformatted Color Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0130, (char *) "SQ", (char *) "Referenced Image Overlay Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2020, 0x0140, (char *) "SQ", (char *) "Referenced VOI LUT Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2030, 0x0000, (char *) "UL", (char *) "Annotation Group Length", (DicomElemParseFunc *) NULL },
    { 0x2030, 0x0010, (char *) "US", (char *) "Annotation Position", (DicomElemParseFunc *) NULL },
    { 0x2030, 0x0020, (char *) "LO", (char *) "Text String", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0000, (char *) "UL", (char *) "Overlay Box Group Length", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0010, (char *) "SQ", (char *) "Referenced Overlay Plane Sequence", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0011, (char *) "US", (char *) "Referenced Overlay Plane Groups", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0060, (char *) "CS", (char *) "Overlay Magnification Type", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0070, (char *) "CS", (char *) "Overlay Smoothing Type", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0080, (char *) "CS", (char *) "Overlay Foreground Density", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0090, (char *) "CS", (char *) "Overlay Mode", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0100, (char *) "CS", (char *) "Threshold Density", (DicomElemParseFunc *) NULL },
    { 0x2040, 0x0500, (char *) "SQ", (char *) "Referenced Overlay Image Box Sequence", (DicomElemParseFunc *) NULL },
    { 0x2050, 0x0010, (char *) "SQ", (char *) "Presentation LUT Sequence", (DicomElemParseFunc *) NULL },
    { 0x2050, 0x0020, (char *) "CS", (char *) "Presentation LUT Shape", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0000, (char *) "UL", (char *) "Print Job Group Length", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0020, (char *) "CS", (char *) "Execution Status", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0030, (char *) "CS", (char *) "Execution Status Info", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0040, (char *) "DA", (char *) "Creation Date", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0050, (char *) "TM", (char *) "Creation Time", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0070, (char *) "AE", (char *) "Originator", (DicomElemParseFunc *) NULL },
    { 0x2100, 0x0500, (char *) "SQ", (char *) "Referenced Print Job Sequence", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0000, (char *) "UL", (char *) "Printer Group Length", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0010, (char *) "CS", (char *) "Printer Status", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0020, (char *) "CS", (char *) "Printer Status Info", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0030, (char *) "LO", (char *) "Printer Name", (DicomElemParseFunc *) NULL },
    { 0x2110, 0x0099, (char *) "SH", (char *) "Print Queue ID", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0002, (char *) "SH", (char *) "RT Image Label", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0003, (char *) "LO", (char *) "RT Image Name", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0004, (char *) "ST", (char *) "RT Image Description", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x000a, (char *) "CS", (char *) "Reported Values Origin", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x000c, (char *) "CS", (char *) "RT Image Plane", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x000e, (char *) "DS", (char *) "X-Ray Image Receptor Angle", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0010, (char *) "DS", (char *) "RTImageOrientation", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0011, (char *) "DS", (char *) "Image Plane Pixel Spacing", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0012, (char *) "DS", (char *) "RT Image Position", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0020, (char *) "SH", (char *) "Radiation Machine Name", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0022, (char *) "DS", (char *) "Radiation Machine SAD", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0024, (char *) "DS", (char *) "Radiation Machine SSD", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0026, (char *) "DS", (char *) "RT Image SID", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0028, (char *) "DS", (char *) "Source to Reference Object Distance", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0029, (char *) "IS", (char *) "Fraction Number", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0030, (char *) "SQ", (char *) "Exposure Sequence", (DicomElemParseFunc *) NULL },
    { 0x3002, 0x0032, (char *) "DS", (char *) "Meterset Exposure", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0001, (char *) "CS", (char *) "DVH Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0002, (char *) "CS", (char *) "Dose Units", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0004, (char *) "CS", (char *) "Dose Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0006, (char *) "LO", (char *) "Dose Comment", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0008, (char *) "DS", (char *) "Normalization Point", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x000a, (char *) "CS", (char *) "Dose Summation Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x000c, (char *) "DS", (char *) "GridFrame Offset Vector", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x000e, (char *) "DS", (char *) "Dose Grid Scaling", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0010, (char *) "SQ", (char *) "RT Dose ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0012, (char *) "DS", (char *) "Dose Value", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0040, (char *) "DS", (char *) "DVH Normalization Point", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0042, (char *) "DS", (char *) "DVH Normalization Dose Value", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0050, (char *) "SQ", (char *) "DVH Sequence", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0052, (char *) "DS", (char *) "DVH Dose Scaling", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0054, (char *) "CS", (char *) "DVH Volume Units", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0056, (char *) "IS", (char *) "DVH Number of Bins", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0058, (char *) "DS", (char *) "DVH Data", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0060, (char *) "SQ", (char *) "DVH Referenced ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0062, (char *) "CS", (char *) "DVH ROI Contribution Type", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0070, (char *) "DS", (char *) "DVH Minimum Dose", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0072, (char *) "DS", (char *) "DVH Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x3004, 0x0074, (char *) "DS", (char *) "DVH Mean Dose", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0002, (char *) "SH", (char *) "Structure Set Label", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0004, (char *) "LO", (char *) "Structure Set Name", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0006, (char *) "ST", (char *) "Structure Set Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0008, (char *) "DA", (char *) "Structure Set Date", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0009, (char *) "TM", (char *) "Structure Set Time", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0010, (char *) "SQ", (char *) "Referenced Frame of Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0012, (char *) "SQ", (char *) "RT Referenced Study Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0014, (char *) "SQ", (char *) "RT Referenced Series Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0016, (char *) "SQ", (char *) "Contour Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0020, (char *) "SQ", (char *) "Structure Set ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0022, (char *) "IS", (char *) "ROI Number", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0024, (char *) "UI", (char *) "Referenced Frame of Reference UID", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0026, (char *) "LO", (char *) "ROI Name", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0028, (char *) "ST", (char *) "ROI Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x002a, (char *) "IS", (char *) "ROI Display Color", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x002c, (char *) "DS", (char *) "ROI Volume", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0030, (char *) "SQ", (char *) "RT Related ROI Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0033, (char *) "CS", (char *) "RT ROI Relationship", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0036, (char *) "CS", (char *) "ROI Generation Algorithm", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0038, (char *) "LO", (char *) "ROI Generation Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0039, (char *) "SQ", (char *) "ROI Contour Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0040, (char *) "SQ", (char *) "Contour Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0042, (char *) "CS", (char *) "Contour Geometric Type", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0044, (char *) "DS", (char *) "Contour SlabT hickness", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0045, (char *) "DS", (char *) "Contour Offset Vector", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0046, (char *) "IS", (char *) "Number of Contour Points", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0050, (char *) "DS", (char *) "Contour Data", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0080, (char *) "SQ", (char *) "RT ROI Observations Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0082, (char *) "IS", (char *) "Observation Number", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0084, (char *) "IS", (char *) "Referenced ROI Number", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0085, (char *) "SH", (char *) "ROI Observation Label", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0086, (char *) "SQ", (char *) "RT ROI Identification Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x0088, (char *) "ST", (char *) "ROI Observation Description", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00a0, (char *) "SQ", (char *) "Related RT ROI Observations Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00a4, (char *) "CS", (char *) "RT ROI Interpreted Type", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00a6, (char *) "PN", (char *) "ROI Interpreter", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00b0, (char *) "SQ", (char *) "ROI Physical Properties Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00b2, (char *) "CS", (char *) "ROI Physical Property", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00b4, (char *) "DS", (char *) "ROI Physical Property Value", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c0, (char *) "SQ", (char *) "Frame of Reference Relationship Sequence", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c2, (char *) "UI", (char *) "Related Frame of Reference UID", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c4, (char *) "CS", (char *) "Frame of Reference Transformation Type", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c6, (char *) "DS", (char *) "Frame of Reference Transformation Matrix", (DicomElemParseFunc *) NULL },
    { 0x3006, 0x00c8, (char *) "LO", (char *) "Frame of Reference Transformation Comment", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0002, (char *) "SH", (char *) "RT Plan Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0003, (char *) "LO", (char *) "RT Plan Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0004, (char *) "ST", (char *) "RT Plan Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0006, (char *) "DA", (char *) "RT Plan Date", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0007, (char *) "TM", (char *) "RT Plan Time", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0009, (char *) "LO", (char *) "Treatment Protocols", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000a, (char *) "CS", (char *) "Treatment Intent", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000b, (char *) "LO", (char *) "Treatment Sites", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000c, (char *) "CS", (char *) "RT Plan Geometry", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x000e, (char *) "ST", (char *) "Prescription Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0010, (char *) "SQ", (char *) "Dose ReferenceSequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0012, (char *) "IS", (char *) "Dose ReferenceNumber", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0014, (char *) "CS", (char *) "Dose Reference Structure Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0016, (char *) "LO", (char *) "Dose ReferenceDescription", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0018, (char *) "DS", (char *) "Dose Reference Point Coordinates", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x001a, (char *) "DS", (char *) "Nominal Prior Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0020, (char *) "CS", (char *) "Dose Reference Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0021, (char *) "DS", (char *) "Constraint Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0022, (char *) "DS", (char *) "Delivery Warning Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0023, (char *) "DS", (char *) "Delivery Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0025, (char *) "DS", (char *) "Target Minimum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0026, (char *) "DS", (char *) "Target Prescription Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0027, (char *) "DS", (char *) "Target Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0028, (char *) "DS", (char *) "Target Underdose Volume Fraction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002a, (char *) "DS", (char *) "Organ at Risk Full-volume Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002b, (char *) "DS", (char *) "Organ at Risk Limit Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002c, (char *) "DS", (char *) "Organ at Risk Maximum Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x002d, (char *) "DS", (char *) "Organ at Risk Overdose Volume Fraction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0040, (char *) "SQ", (char *) "Tolerance Table Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0042, (char *) "IS", (char *) "Tolerance Table Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0043, (char *) "SH", (char *) "Tolerance Table Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0044, (char *) "DS", (char *) "Gantry Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0046, (char *) "DS", (char *) "Beam Limiting Device Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0048, (char *) "SQ", (char *) "Beam Limiting Device Tolerance Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x004a, (char *) "DS", (char *) "Beam Limiting Device Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x004c, (char *) "DS", (char *) "Patient Support Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x004e, (char *) "DS", (char *) "Table Top Eccentric Angle Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0051, (char *) "DS", (char *) "Table Top Vertical Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0052, (char *) "DS", (char *) "Table Top Longitudinal Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0053, (char *) "DS", (char *) "Table Top Lateral Position Tolerance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0055, (char *) "CS", (char *) "RT Plan Relationship", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0070, (char *) "SQ", (char *) "Fraction Group Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0071, (char *) "IS", (char *) "Fraction Group Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0078, (char *) "IS", (char *) "Number of Fractions Planned", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0079, (char *) "IS", (char *) "Number of Fractions Per Day", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x007a, (char *) "IS", (char *) "Repeat Fraction Cycle Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x007b, (char *) "LT", (char *) "Fraction Pattern", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0080, (char *) "IS", (char *) "Number of Beams", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0082, (char *) "DS", (char *) "Beam Dose Specification Point", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0084, (char *) "DS", (char *) "Beam Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0086, (char *) "DS", (char *) "Beam Meterset", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00a0, (char *) "IS", (char *) "Number of Brachy Application Setups", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00a2, (char *) "DS", (char *) "Brachy Application Setup Dose Specification Point", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00a4, (char *) "DS", (char *) "Brachy Application Setup Dose", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b0, (char *) "SQ", (char *) "Beam Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b2, (char *) "SH", (char *) "Treatment Machine Name ", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b3, (char *) "CS", (char *) "Primary Dosimeter Unit", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b4, (char *) "DS", (char *) "Source-Axis Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b6, (char *) "SQ", (char *) "Beam Limiting Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00b8, (char *) "CS", (char *) "RT Beam Limiting Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ba, (char *) "DS", (char *) "Source to Beam Limiting Device Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00bc, (char *) "IS", (char *) "Number of Leaf/Jaw Pairs", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00be, (char *) "DS", (char *) "Leaf Position Boundaries", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c0, (char *) "IS", (char *) "Beam Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c2, (char *) "LO", (char *) "Beam Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c3, (char *) "ST", (char *) "Beam Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c4, (char *) "CS", (char *) "Beam Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c6, (char *) "CS", (char *) "Radiation Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00c8, (char *) "IS", (char *) "Reference Image Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ca, (char *) "SQ", (char *) "Planned Verification Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00cc, (char *) "LO", (char *) "Imaging Device Specific Acquisition Parameters", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ce, (char *) "CS", (char *) "Treatment Delivery Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d0, (char *) "IS", (char *) "Number of Wedges", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d1, (char *) "SQ", (char *) "Wedge Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d2, (char *) "IS", (char *) "Wedge Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d3, (char *) "CS", (char *) "Wedge Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d4, (char *) "SH", (char *) "Wedge ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d5, (char *) "IS", (char *) "Wedge Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d6, (char *) "DS", (char *) "Wedge Factor", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00d8, (char *) "DS", (char *) "Wedge Orientation", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00da, (char *) "DS", (char *) "Source to Wedge Tray Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e0, (char *) "IS", (char *) "Number of Compensators", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e1, (char *) "SH", (char *) "Material ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e2, (char *) "DS", (char *) "Total Compensator Tray Factor", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e3, (char *) "SQ", (char *) "Compensator Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e4, (char *) "IS", (char *) "Compensator Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e5, (char *) "SH", (char *) "Compensator ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e6, (char *) "DS", (char *) "Source to Compensator Tray Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e7, (char *) "IS", (char *) "Compensator Rows", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e8, (char *) "IS", (char *) "Compensator Columns", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00e9, (char *) "DS", (char *) "Compensator Pixel Spacing", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ea, (char *) "DS", (char *) "Compensator Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00eb, (char *) "DS", (char *) "Compensator Transmission Data", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ec, (char *) "DS", (char *) "Compensator Thickness Data", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00ed, (char *) "IS", (char *) "Number of Boli", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f0, (char *) "IS", (char *) "Number of Blocks", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f2, (char *) "DS", (char *) "Total Block Tray Factor", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f4, (char *) "SQ", (char *) "Block Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f5, (char *) "SH", (char *) "Block Tray ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f6, (char *) "DS", (char *) "Source to Block Tray Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00f8, (char *) "CS", (char *) "Block Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00fa, (char *) "CS", (char *) "Block Divergence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00fc, (char *) "IS", (char *) "Block Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x00fe, (char *) "LO", (char *) "Block Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0100, (char *) "DS", (char *) "Block Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0102, (char *) "DS", (char *) "Block Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0104, (char *) "IS", (char *) "Block Number of Points", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0106, (char *) "DS", (char *) "Block Data", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0107, (char *) "SQ", (char *) "Applicator Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0108, (char *) "SH", (char *) "Applicator ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0109, (char *) "CS", (char *) "Applicator Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x010a, (char *) "LO", (char *) "Applicator Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x010c, (char *) "DS", (char *) "Cumulative Dose Reference Coefficient", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x010e, (char *) "DS", (char *) "Final Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0110, (char *) "IS", (char *) "Number of Control Points", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0111, (char *) "SQ", (char *) "Control Point Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0112, (char *) "IS", (char *) "Control Point Index", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0114, (char *) "DS", (char *) "Nominal Beam Energy", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0115, (char *) "DS", (char *) "Dose Rate Set", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0116, (char *) "SQ", (char *) "Wedge Position Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0118, (char *) "CS", (char *) "Wedge Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011a, (char *) "SQ", (char *) "Beam Limiting Device Position Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011c, (char *) "DS", (char *) "Leaf Jaw Positions", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011e, (char *) "DS", (char *) "Gantry Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x011f, (char *) "CS", (char *) "Gantry Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0120, (char *) "DS", (char *) "Beam Limiting Device Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0121, (char *) "CS", (char *) "Beam Limiting Device Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0122, (char *) "DS", (char *) "Patient Support Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0123, (char *) "CS", (char *) "Patient Support Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0124, (char *) "DS", (char *) "Table Top Eccentric Axis Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0125, (char *) "DS", (char *) "Table Top Eccentric Angle", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0126, (char *) "CS", (char *) "Table Top Eccentric Rotation Direction", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0128, (char *) "DS", (char *) "Table Top Vertical Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0129, (char *) "DS", (char *) "Table Top Longitudinal Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x012a, (char *) "DS", (char *) "Table Top Lateral Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x012c, (char *) "DS", (char *) "Isocenter Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x012e, (char *) "DS", (char *) "Surface Entry Point", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0130, (char *) "DS", (char *) "Source to Surface Distance", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0134, (char *) "DS", (char *) "Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0180, (char *) "SQ", (char *) "Patient Setup Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0182, (char *) "IS", (char *) "Patient Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0184, (char *) "LO", (char *) "Patient Additional Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0190, (char *) "SQ", (char *) "Fixation Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0192, (char *) "CS", (char *) "Fixation Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0194, (char *) "SH", (char *) "Fixation Device Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0196, (char *) "ST", (char *) "Fixation Device Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0198, (char *) "SH", (char *) "Fixation Device Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a0, (char *) "SQ", (char *) "Shielding Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a2, (char *) "CS", (char *) "Shielding Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a4, (char *) "SH", (char *) "Shielding Device Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a6, (char *) "ST", (char *) "Shielding Device Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01a8, (char *) "SH", (char *) "Shielding Device Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b0, (char *) "CS", (char *) "Setup Technique", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b2, (char *) "ST", (char *) "Setup TechniqueDescription", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b4, (char *) "SQ", (char *) "Setup Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b6, (char *) "CS", (char *) "Setup Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01b8, (char *) "SH", (char *) "Setup Device Label", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01ba, (char *) "ST", (char *) "Setup Device Description", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01bc, (char *) "DS", (char *) "Setup Device Parameter", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d0, (char *) "ST", (char *) "Setup ReferenceDescription", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d2, (char *) "DS", (char *) "Table Top Vertical Setup Displacement", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d4, (char *) "DS", (char *) "Table Top Longitudinal Setup Displacement", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x01d6, (char *) "DS", (char *) "Table Top Lateral Setup Displacement", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0200, (char *) "CS", (char *) "Brachy Treatment Technique", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0202, (char *) "CS", (char *) "Brachy Treatment Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0206, (char *) "SQ", (char *) "Treatment Machine Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0210, (char *) "SQ", (char *) "Source Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0212, (char *) "IS", (char *) "Source Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0214, (char *) "CS", (char *) "Source Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0216, (char *) "LO", (char *) "Source Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0218, (char *) "DS", (char *) "Active Source Diameter", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x021a, (char *) "DS", (char *) "Active Source Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0222, (char *) "DS", (char *) "Source Encapsulation Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0224, (char *) "DS", (char *) "Source Encapsulation Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0226, (char *) "LO", (char *) "Source IsotopeName", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0228, (char *) "DS", (char *) "Source Isotope Half Life", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x022a, (char *) "DS", (char *) "Reference Air Kerma Rate", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x022c, (char *) "DA", (char *) "Air Kerma Rate Reference Date", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x022e, (char *) "TM", (char *) "Air Kerma Rate Reference Time", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0230, (char *) "SQ", (char *) "Application Setup Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0232, (char *) "CS", (char *) "Application Setup Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0234, (char *) "IS", (char *) "Application Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0236, (char *) "LO", (char *) "Application Setup Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0238, (char *) "LO", (char *) "Application Setup Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0240, (char *) "IS", (char *) "Template Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0242, (char *) "SH", (char *) "Template Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0244, (char *) "LO", (char *) "Template Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0250, (char *) "DS", (char *) "Total Reference Air Kerma", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0260, (char *) "SQ", (char *) "Brachy Accessory Device Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0262, (char *) "IS", (char *) "Brachy Accessory Device Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0263, (char *) "SH", (char *) "Brachy Accessory Device ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0264, (char *) "CS", (char *) "Brachy Accessory Device Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0266, (char *) "LO", (char *) "Brachy Accessory Device Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x026a, (char *) "DS", (char *) "Brachy Accessory Device Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x026c, (char *) "DS", (char *) "Brachy Accessory Device Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0280, (char *) "SQ", (char *) "Channel Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0282, (char *) "IS", (char *) "Channel Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0284, (char *) "DS", (char *) "Channel Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0286, (char *) "DS", (char *) "Channel Total Time", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0288, (char *) "CS", (char *) "Source Movement Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x028a, (char *) "IS", (char *) "Number of Pulses", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x028c, (char *) "DS", (char *) "Pulse Repetition Interval", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0290, (char *) "IS", (char *) "Source Applicator Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0291, (char *) "SH", (char *) "Source Applicator ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0292, (char *) "CS", (char *) "Source Applicator Type", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0294, (char *) "LO", (char *) "Source Applicator Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0296, (char *) "DS", (char *) "Source Applicator Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x0298, (char *) "LO", (char *) "Source Applicator Manufacturer", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x029c, (char *) "DS", (char *) "Source Applicator Wall Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x029e, (char *) "DS", (char *) "Source Applicator Wall Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02a0, (char *) "DS", (char *) "Source Applicator Step Size", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02a2, (char *) "IS", (char *) "Transfer Tube Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02a4, (char *) "DS", (char *) "Transfer Tube Length", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b0, (char *) "SQ", (char *) "Channel Shield Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b2, (char *) "IS", (char *) "Channel Shield Number", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b3, (char *) "SH", (char *) "Channel Shield ID", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b4, (char *) "LO", (char *) "Channel Shield Name", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02b8, (char *) "DS", (char *) "Channel Shield Nominal Thickness", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02ba, (char *) "DS", (char *) "Channel Shield Nominal Transmission", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02c8, (char *) "DS", (char *) "Final Cumulative Time Weight", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d0, (char *) "SQ", (char *) "Brachy Control Point Sequence", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d2, (char *) "DS", (char *) "Control Point Relative Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d4, (char *) "DS", (char *) "Control Point 3D Position", (DicomElemParseFunc *) NULL },
    { 0x300a, 0x02d6, (char *) "DS", (char *) "Cumulative Time Weight", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0002, (char *) "SQ", (char *) "Referenced RT Plan Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0004, (char *) "SQ", (char *) "Referenced Beam Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0006, (char *) "IS", (char *) "Referenced Beam Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0007, (char *) "IS", (char *) "Referenced Reference Image Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0008, (char *) "DS", (char *) "Start Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0009, (char *) "DS", (char *) "End Cumulative Meterset Weight", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x000a, (char *) "SQ", (char *) "Referenced Brachy Application Setup Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x000c, (char *) "IS", (char *) "Referenced Brachy Application Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x000e, (char *) "IS", (char *) "Referenced Source Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0020, (char *) "SQ", (char *) "Referenced Fraction Group Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0022, (char *) "IS", (char *) "Referenced Fraction Group Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0040, (char *) "SQ", (char *) "Referenced Verification Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0042, (char *) "SQ", (char *) "Referenced Reference Image Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0050, (char *) "SQ", (char *) "Referenced Dose Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0051, (char *) "IS", (char *) "Referenced Dose Reference Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0055, (char *) "SQ", (char *) "Brachy Referenced Dose Reference Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0060, (char *) "SQ", (char *) "Referenced Structure Set Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x006a, (char *) "IS", (char *) "Referenced Patient Setup Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x0080, (char *) "SQ", (char *) "Referenced Dose Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00a0, (char *) "IS", (char *) "Referenced Tolerance Table Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00b0, (char *) "SQ", (char *) "Referenced Bolus Sequence", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00c0, (char *) "IS", (char *) "Referenced Wedge Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00d0, (char *) "IS", (char *) "Referenced Compensato rNumber", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00e0, (char *) "IS", (char *) "Referenced Block Number", (DicomElemParseFunc *) NULL },
    { 0x300c, 0x00f0, (char *) "IS", (char *) "Referenced Control Point", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0002, (char *) "CS", (char *) "Approval Status", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0004, (char *) "DA", (char *) "Review Date", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0005, (char *) "TM", (char *) "Review Time", (DicomElemParseFunc *) NULL },
    { 0x300e, 0x0008, (char *) "PN", (char *) "Reviewer Name", (DicomElemParseFunc *) NULL },
    { 0x4000, 0x0000, (char *) "UL", (char *) "Text Group Length", (DicomElemParseFunc *) NULL },
    { 0x4000, 0x0010, (char *) "LT", (char *) "Text Arbitrary", (DicomElemParseFunc *) NULL },
    { 0x4000, 0x4000, (char *) "LT", (char *) "Text Comments", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0000, (char *) "UL", (char *) "Results Group Length", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0040, (char *) "SH", (char *) "Results ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0042, (char *) "LO", (char *) "Results ID Issuer", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0050, (char *) "SQ", (char *) "Referenced Interpretation Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x00ff, (char *) "CS", (char *) "Report Production Status", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0100, (char *) "DA", (char *) "Interpretation Recorded Date", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0101, (char *) "TM", (char *) "Interpretation Recorded Time", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0102, (char *) "PN", (char *) "Interpretation Recorder", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0103, (char *) "LO", (char *) "Reference to Recorded Sound", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0108, (char *) "DA", (char *) "Interpretation Transcription Date", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0109, (char *) "TM", (char *) "Interpretation Transcription Time", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x010a, (char *) "PN", (char *) "Interpretation Transcriber", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x010b, (char *) "ST", (char *) "Interpretation Text", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x010c, (char *) "PN", (char *) "Interpretation Author", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0111, (char *) "SQ", (char *) "Interpretation Approver Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0112, (char *) "DA", (char *) "Interpretation Approval Date", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0113, (char *) "TM", (char *) "Interpretation Approval Time", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0114, (char *) "PN", (char *) "Physician Approving Interpretation", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0115, (char *) "LT", (char *) "Interpretation Diagnosis Description", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0117, (char *) "SQ", (char *) "InterpretationDiagnosis Code Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0118, (char *) "SQ", (char *) "Results Distribution List Sequence", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0119, (char *) "PN", (char *) "Distribution Name", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x011a, (char *) "LO", (char *) "Distribution Address", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0200, (char *) "SH", (char *) "Interpretation ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0202, (char *) "LO", (char *) "Interpretation ID Issuer", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0210, (char *) "CS", (char *) "Interpretation Type ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0212, (char *) "CS", (char *) "Interpretation Status ID", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x0300, (char *) "ST", (char *) "Impressions", (DicomElemParseFunc *) NULL },
    { 0x4008, 0x4000, (char *) "ST", (char *) "Results Comments", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0001, (char *) "LT", (char *) "Report ID", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0020, (char *) "LT", (char *) "Report Status", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0030, (char *) "DA", (char *) "Report Creation Date", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x0070, (char *) "LT", (char *) "Report Approving Physician", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x00e0, (char *) "LT", (char *) "Report Text", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x00e1, (char *) "LT", (char *) "Report Author", (DicomElemParseFunc *) NULL },
    { 0x4009, 0x00e3, (char *) "LT", (char *) "Reporting Radiologist", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0000, (char *) "UL", (char *) "Curve Group Length", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0005, (char *) "US", (char *) "Curve Dimensions", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0010, (char *) "US", (char *) "Number of Points", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0020, (char *) "CS", (char *) "Type of Data", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0022, (char *) "LO", (char *) "Curve Description", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0030, (char *) "SH", (char *) "Axis Units", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0040, (char *) "SH", (char *) "Axis Labels", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0103, (char *) "US", (char *) "Data Value Representation", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0104, (char *) "US", (char *) "Minimum Coordinate Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0105, (char *) "US", (char *) "Maximum Coordinate Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0106, (char *) "SH", (char *) "Curve Range", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0110, (char *) "US", (char *) "Curve Data Descriptor", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0112, (char *) "US", (char *) "Coordinate Start Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x0114, (char *) "US", (char *) "Coordinate Step Value", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x1001, (char *) "CS", (char *) "Curve Activation Layer", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2000, (char *) "US", (char *) "Audio Type", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2002, (char *) "US", (char *) "Audio Sample Format", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2004, (char *) "US", (char *) "Number of Channels", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2006, (char *) "UL", (char *) "Number of Samples", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2008, (char *) "UL", (char *) "Sample Rate", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x200a, (char *) "UL", (char *) "Total Time", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x200c, (char *) "xs", (char *) "Audio Sample Data", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x200e, (char *) "LT", (char *) "Audio Comments", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2500, (char *) "LO", (char *) "Curve Label", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2600, (char *) "SQ", (char *) "CurveReferenced Overlay Sequence", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x2610, (char *) "US", (char *) "CurveReferenced Overlay Group", (DicomElemParseFunc *) NULL },
    { 0x5000, 0x3000, (char *) "OW", (char *) "Curve Data", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0000, (char *) "UL", (char *) "Overlay Group Length", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0001, (char *) "US", (char *) "Gray Palette Color Lookup Table Descriptor", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0002, (char *) "US", (char *) "Gray Palette Color Lookup Table Data", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0010, (char *) "US", (char *) "Overlay Rows", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0011, (char *) "US", (char *) "Overlay Columns", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0012, (char *) "US", (char *) "Overlay Planes", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0015, (char *) "IS", (char *) "Number of Frames in Overlay", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0022, (char *) "LO", (char *) "Overlay Description", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0040, (char *) "CS", (char *) "Overlay Type", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0045, (char *) "CS", (char *) "Overlay Subtype", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0050, (char *) "SS", (char *) "Overlay Origin", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0051, (char *) "US", (char *) "Image Frame Origin", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0052, (char *) "US", (char *) "Plane Origin", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0060, (char *) "LO", (char *) "Overlay Compression Code", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0061, (char *) "SH", (char *) "Overlay Compression Originator", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0062, (char *) "SH", (char *) "Overlay Compression Label", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0063, (char *) "SH", (char *) "Overlay Compression Description", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0066, (char *) "AT", (char *) "Overlay Compression Step Pointers", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0068, (char *) "US", (char *) "Overlay Repeat Interval", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0069, (char *) "US", (char *) "Overlay Bits Grouped", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0100, (char *) "US", (char *) "Overlay Bits Allocated", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0102, (char *) "US", (char *) "Overlay Bit Position", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0110, (char *) "LO", (char *) "Overlay Format", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0200, (char *) "xs", (char *) "Overlay Location", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0800, (char *) "LO", (char *) "Overlay Code Label", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0802, (char *) "US", (char *) "Overlay Number of Tables", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0803, (char *) "AT", (char *) "Overlay Code Table Location", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x0804, (char *) "US", (char *) "Overlay Bits For Code Word", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1001, (char *) "CS", (char *) "Overlay Activation Layer", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1100, (char *) "US", (char *) "Overlay Descriptor - Gray", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1101, (char *) "US", (char *) "Overlay Descriptor - Red", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1102, (char *) "US", (char *) "Overlay Descriptor - Green", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1103, (char *) "US", (char *) "Overlay Descriptor - Blue", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1200, (char *) "US", (char *) "Overlays - Gray", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1201, (char *) "US", (char *) "Overlays - Red", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1202, (char *) "US", (char *) "Overlays - Green", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1203, (char *) "US", (char *) "Overlays - Blue", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1301, (char *) "IS", (char *) "ROI Area", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1302, (char *) "DS", (char *) "ROI Mean", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1303, (char *) "DS", (char *) "ROI Standard Deviation", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x1500, (char *) "LO", (char *) "Overlay Label", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x3000, (char *) "OW", (char *) "Overlay Data", (DicomElemParseFunc *) NULL },
    { 0x6000, 0x4000, (char *) "LT", (char *) "Overlay Comments", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x0000, (char *) "UN", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x0010, (char *) "LO", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x1010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x6001, 0x1030, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x6021, 0x0000, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x6021, 0x0010, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7001, 0x0010, (char *) "LT", (char *) "Dummy", (DicomElemParseFunc *) NULL },
    { 0x7003, 0x0010, (char *) "LT", (char *) "Info", (DicomElemParseFunc *) NULL },
    { 0x7005, 0x0010, (char *) "LT", (char *) "Dummy", (DicomElemParseFunc *) NULL },
    { 0x7000, 0x0004, (char *) "ST", (char *) "TextAnnotation", (DicomElemParseFunc *) NULL },
    { 0x7000, 0x0005, (char *) "IS", (char *) "Box", (DicomElemParseFunc *) NULL },
    { 0x7000, 0x0007, (char *) "IS", (char *) "ArrowEnd", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0000, (char *) "UL", (char *) "Pixel Data Group Length", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0010, (char *) "xs", (char *) "Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0020, (char *) "OW", (char *) "Coefficients SDVN", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0030, (char *) "OW", (char *) "Coefficients SDHN", (DicomElemParseFunc *) NULL },
    { 0x7fe0, 0x0040, (char *) "OW", (char *) "Coefficients SDDN", (DicomElemParseFunc *) NULL },
    { 0x7fe1, 0x0010, (char *) "xs", (char *) "Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0000, (char *) "UL", (char *) "Variable Pixel Data Group Length", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0010, (char *) "xs", (char *) "Variable Pixel Data", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0011, (char *) "US", (char *) "Variable Next Data Group", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0020, (char *) "OW", (char *) "Variable Coefficients SDVN", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0030, (char *) "OW", (char *) "Variable Coefficients SDHN", (DicomElemParseFunc *) NULL },
    { 0x7f00, 0x0040, (char *) "OW", (char *) "Variable Coefficients SDDN", (DicomElemParseFunc *) NULL },
    { 0x7fe1, 0x0000, (char *) "OB", (char *) "Binary Data", (DicomElemParseFunc *) NULL },
    { 0x7fe3, 0x0000, (char *) "LT", (char *) "Image Graphics Format Code", (DicomElemParseFunc *) NULL },
    { 0x7fe3, 0x0010, (char *) "OB", (char *) "Image Graphics", (DicomElemParseFunc *) NULL },
    { 0x7fe3, 0x0020, (char *) "OB", (char *) "Image Graphics Dummy", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0001, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0002, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0003, (char *) "xs", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0004, (char *) "IS", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0005, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0007, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0008, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0009, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000a, (char *) "LT", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000b, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000c, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x000d, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0x7ff1, 0x0010, (char *) "US", (char *) "?", (DicomElemParseFunc *) NULL },
    { 0xfffc, 0xfffc, (char *) "OB", (char *) "Data Set Trailing Padding", (DicomElemParseFunc *) NULL },
    { 0xfffe, 0xe000, (char *) "!!", (char *) "Item", (DicomElemParseFunc *) NULL },
    { 0xfffe, 0xe00d, (char *) "!!", (char *) "Item Delimitation Item", (DicomElemParseFunc *) NULL },
    { 0xfffe, 0xe0dd, (char *) "!!", (char *) "Sequence Delimitation Item", (DicomElemParseFunc *) NULL },
    { 0xffff, 0xffff, (char *) "xs", (char *) "", (DicomElemParseFunc *) NULL }
  };

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I s D C M                                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method IsDCM returns True if the image format type, identified by the
%  magick string, is DCM.
%
%  The format of the ReadDCMImage method is:
%
%      unsigned int IsDCM(const unsigned char *magick,const size_t length)
%
%  A description of each parameter follows:
%
%    o status:  Method IsDCM returns True if the image format type is DCM.
%
%    o magick: This string is generally the first few bytes of an image file
%      or blob.
%
%    o length: Specifies the length of the magick string.
%
%
*/
static MagickPassFail IsDCM(const unsigned char *magick,const size_t length)
{
  if (length < 132)
    return(False);
  if (LocaleNCompare((char *) (magick+128),"DICM",4) == 0)
    return(True);
  return(False);
}

static MagickPassFail DCM_InitDCM(DicomStream *dcm,int verbose)
{
  dcm->columns=0;
  dcm->rows=0;
  dcm->samples_per_pixel=1;
  dcm->bits_allocated=8;
  dcm->significant_bits=0;
  dcm->high_bit=0;
  dcm->bytes_per_pixel=1;
  dcm->max_value_in=255;
  dcm->max_value_out=255;
  dcm->pixel_representation=0;
  dcm->transfer_syntax=DCM_TS_IMPL_LITTLE;
  dcm->interlace=0;
  dcm->msb_state=DCM_MSB_LITTLE;
  dcm->phot_interp=DCM_PI_MONOCHROME2;
  dcm->window_center=0;
  dcm->window_width=0;
  dcm->rescale_intercept=0;
  dcm->rescale_slope=1;
  dcm->number_scenes=1;
  dcm->data=NULL;
  dcm->upper_lim=0;
  dcm->lower_lim=0;
  dcm->rescale_map=NULL;
  dcm->rescale_type=DCM_RT_HOUNSFIELD;
  dcm->rescaling=DCM_RS_NONE;
  dcm->offset_ct=0;
  dcm->offset_arr=NULL;
  dcm->frag_bytes=0;
  dcm->rle_rep_ct=0;
#if defined(USE_GRAYMAP)
  dcm->graymap=(unsigned short *) NULL;
#endif
  dcm->funcReadShort=ReadBlobLSBShort;
  dcm->funcReadLong=ReadBlobLSBLong;
  dcm->explicit_file=False;
  dcm->verbose=verbose;

  return MagickPass;
}

/*
  Parse functions for DICOM elements
*/
static MagickPassFail funcDCM_TransferSyntax(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  int
    type,
    subtype;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  p=(char *) dcm->data;
  if (strncmp(p,"1.2.840.10008.1.2",17) == 0)
    {
      if (*(p+17) == 0)
        {
          dcm->transfer_syntax = DCM_TS_IMPL_LITTLE;
          return MagickPass;
        }
      type=0;
      subtype=0;
      sscanf(p+17,".%d.%d",&type,&subtype);
      switch (type)
        {
        case 1:
          dcm->transfer_syntax = DCM_TS_EXPL_LITTLE;
          break;
        case 2:
          dcm->transfer_syntax = DCM_TS_EXPL_BIG;
          dcm->msb_state=DCM_MSB_BIG_PENDING;
          break;
        case 4:
          if ((subtype >= 80) && (subtype <= 81))
            dcm->transfer_syntax = DCM_TS_JPEG_LS;
          else
          if ((subtype >= 90) && (subtype <= 93))
            dcm->transfer_syntax = DCM_TS_JPEG_2000;
          else
            dcm->transfer_syntax = DCM_TS_JPEG;
          break;
        case 5:
          dcm->transfer_syntax = DCM_TS_RLE;
          break;
        }
    }
  return MagickPass;
}

static MagickPassFail funcDCM_StudyDate(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"StudyDate",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_PatientName(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"PatientName",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_TriggerTime(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"TriggerTime",(char *) dcm->data);
  return MagickPass;  
}

static MagickPassFail funcDCM_FieldOfView(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  (void) SetImageAttribute(image,"FieldOfView",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_SeriesNumber(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"SeriesNumber",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_ImagePosition(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"ImagePosition",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_ImageOrientation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"ImageOrientation",(char *) dcm->data);
  return MagickPass; 
}

static MagickPassFail funcDCM_SliceLocation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  (void) SetImageAttribute(image,"SliceLocation",(char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_SamplesPerPixel(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);
  dcm->samples_per_pixel=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_PhotometricInterpretation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char photometric[MaxTextExtent];
  int i;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  for (i=0; i < (long) Min(dcm->length, MaxTextExtent-1); i++)
    photometric[i]=dcm->data[i];
  photometric[i]='\0';

  if (strncmp(photometric,"MONOCHROME1",11) == 0)
    dcm->phot_interp = DCM_PI_MONOCHROME1;
  else
    if (strncmp(photometric,"MONOCHROME2",11) == 0)
      dcm->phot_interp = DCM_PI_MONOCHROME2;
    else
      if (strncmp(photometric,"PALETTE COLOR",13) == 0)
        dcm->phot_interp = DCM_PI_PALETTE_COLOR;
      else
        if (strncmp(photometric,"RGB",3) == 0)
          dcm->phot_interp = DCM_PI_RGB;
        else
          dcm->phot_interp = DCM_PI_OTHER;
  return MagickPass;
}

static MagickPassFail funcDCM_PlanarConfiguration(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->interlace=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_NumberOfFrames(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->number_scenes=atoi((char *) dcm->data);
  return MagickPass;
}

static MagickPassFail funcDCM_Rows(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->rows=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_Columns(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->columns=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_BitsAllocated(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->bits_allocated=dcm->datum;
  dcm->bytes_per_pixel=1;
  if (dcm->datum > 8)
    dcm->bytes_per_pixel=2;
  return MagickPass;
}

static MagickPassFail funcDCM_BitsStored(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);

  dcm->significant_bits=dcm->datum;
  dcm->bytes_per_pixel=1;
  if (dcm->significant_bits > 8)
    dcm->bytes_per_pixel=2;
  dcm->max_value_in=(1 << dcm->significant_bits)-1;
  dcm->max_value_out=dcm->max_value_in;
  image->depth=Min(dcm->significant_bits,QuantumDepth);
  return MagickPass;
}

static MagickPassFail funcDCM_HighBit(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->high_bit=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_PixelRepresentation(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  dcm->pixel_representation=dcm->datum;
  return MagickPass;
}

static MagickPassFail funcDCM_WindowCenter(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->window_center=atof(p);
  return MagickPass;
}

static MagickPassFail funcDCM_WindowWidth(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->window_width=atof(p);
  return MagickPass;
}

static MagickPassFail funcDCM_RescaleIntercept(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->rescale_intercept=atof(p);
  return MagickPass;
}

static MagickPassFail funcDCM_RescaleSlope(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    *p;

  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  p = strrchr((char *) dcm->data,'\\');
  if (p)
    p++;
  else
    p=(char *) dcm->data;
  dcm->rescale_slope=atof(p);
  return MagickPass;
}

static MagickPassFail funcDCM_RescaleType(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  ARG_NOT_USED(image);
  ARG_NOT_USED(exception);

  if (strncmp((char *) dcm->data,"OD",2) == 0)
    dcm->rescale_type=DCM_RT_OPTICAL_DENSITY;
  else if (strncmp((char *) dcm->data,"HU",2) == 0)
    dcm->rescale_type=DCM_RT_HOUNSFIELD;
  else if (strncmp((char *) dcm->data,"US",2) == 0)
    dcm->rescale_type=DCM_RT_UNSPECIFIED;
  else
    dcm->rescale_type=DCM_RT_UNKNOWN;
  return MagickPass;
}

static MagickPassFail funcDCM_PaletteDescriptor(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  /*
    Palette descriptor tables
    element = 1101/2/3 = for red/green/blue palette
    val 0 = # of entries in LUT (0 for 65535)
    val 1 = min pix value in palette (ie pix <= val1 -> palette[0])
    val 2 = # bits in LUT (8 or 16)
    NB Required by specification to be the same for each color

    #1 - check the same each time
    #2 - use scale_remap to map vals <= (val1) to first entry
    and vals >= (val1 + palette size) to last entry
    #3 - if (val2) == 8, use UINT8 values instead of UINT16
    #4 - Check for UINT8 values packed into low bits of UINT16
    entries as per spec
  */
  ARG_NOT_USED(image);
  ARG_NOT_USED(dcm);
  ARG_NOT_USED(exception);

  return MagickPass;
}

static MagickPassFail funcDCM_LUT(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
#if defined(USE_GRAYMAP)  
  /*
    1200 = grey, LUT data 3006 = LUT data
  */
  unsigned long
    colors;

  register unsigned long
    i;

  colors=dcm->length/dcm->bytes_per_pixel;
  dcm->datum=(long) colors;
  dcm->graymap=MagickAllocateArray(unsigned short *,colors,sizeof(unsigned short));
  if (dcm->graymap == (unsigned short *) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
      return MagickFail;
    }
  for (i=0; i < (long) colors; i++)
    if (dcm->bytes_per_pixel == 1)
      dcm->graymap[i]=dcm->data[i];
    else
      dcm->graymap[i]=(unsigned short) ((short *) dcm->data)[i];
#else
  ARG_NOT_USED(image);
  ARG_NOT_USED(dcm);
  ARG_NOT_USED(exception);
#endif
  return MagickPass;
}

static MagickPassFail funcDCM_Palette(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  register long
    i;

  unsigned char
    *p;

  unsigned short
    index;

  /*
    Initialize colormap (entries are always 16 bit)
    1201/2/3 = red/green/blue palette
  */
  if (image->colormap == (PixelPacket *) NULL)
    {
      /*
        Allocate color map first time in
      */
      if (!AllocateImageColormap(image,dcm->length))
        {
          ThrowException(exception,ResourceLimitError,UnableToCreateColormap,image->filename);
          return MagickFail;
        }
    }

  /*
    Check that palette size matches previous one(s)
  */
  if (dcm->length != image->colors)
    {
      ThrowException(exception,ResourceLimitError,UnableToCreateColormap,image->filename);
      return MagickFail;
    }

  p=dcm->data;
  for (i=0; i < (long) image->colors; i++)
    {
      if (dcm->msb_state == DCM_MSB_BIG)
        index=(*p << 8) | *(p+1);
      else
        index=*p | (*(p+1) << 8);
      if (dcm->element == 0x1201)
        image->colormap[i].red=ScaleShortToQuantum(index);
      else if (dcm->element == 0x1202)
        image->colormap[i].green=ScaleShortToQuantum(index);
      else
        image->colormap[i].blue=ScaleShortToQuantum(index);
      p+=2;
    }
  return MagickPass;
}

static magick_uint8_t DCM_RLE_ReadByte(Image *image, DicomStream *dcm)
{
  if (dcm->rle_rep_ct == 0)
    {
      int
        rep_ct,
        rep_char;

      /* We need to read the next command pair */     
      if (dcm->frag_bytes <= 2)
        dcm->frag_bytes = 0;
      else
        dcm->frag_bytes -= 2;

      rep_ct=ReadBlobByte(image);
      rep_char=ReadBlobByte(image);
      if (rep_ct == 128)
        {
          /* Illegal value */
          return 0;
        }
      else
      if (rep_ct < 128)
        {
          /* (rep_ct+1) literal bytes */
          dcm->rle_rep_ct=rep_ct;
          dcm->rle_rep_char=-1;
          return (magick_uint8_t)rep_char;
        }
      else
        {
          /* (257-rep_ct) repeated bytes */
          dcm->rle_rep_ct=256-rep_ct;
          dcm->rle_rep_char=rep_char;
          return (magick_uint8_t)rep_char;
        }
    }

  dcm->rle_rep_ct--;
  if (dcm->rle_rep_char >= 0)
    return dcm->rle_rep_char;

  if (dcm->frag_bytes > 0)
    dcm->frag_bytes--;
  return ReadBlobByte(image);
}

static magick_uint16_t DCM_RLE_ReadShort(Image *image, DicomStream *dcm)
{
  return (DCM_RLE_ReadByte(image,dcm) << 4) | DCM_RLE_ReadByte(image,dcm);
}

static MagickPassFail DCM_ReadElement(Image *image, DicomStream *dcm,ExceptionInfo *exception)
{
  unsigned int
    use_explicit;

  char
    explicit_vr[MaxTextExtent],
    implicit_vr[MaxTextExtent];

  register long
    i;

  /*
    Read group and element IDs
  */
  image->offset=(long) TellBlob(image);
  dcm->group=dcm->funcReadShort(image);
  if ((dcm->msb_state == DCM_MSB_BIG_PENDING) && (dcm->group != 2))
    {
      dcm->group = (dcm->group << 8) | (dcm->group >> 8);
      dcm->funcReadShort=ReadBlobMSBShort;
      dcm->funcReadLong=ReadBlobMSBLong;
      dcm->msb_state=DCM_MSB_BIG;
    }
  dcm->element=dcm->funcReadShort(image);
  dcm->data=(unsigned char *) NULL;
  dcm->quantum=0;
  if (EOFBlob(image))
    {
      ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
      return MagickFail;
    }
  /*
    Find corresponding VR for this group and element.
  */
  for (i=0; dicom_info[i].group < 0xffffU; i++)
    if ((dcm->group == dicom_info[i].group) &&
        (dcm->element == dicom_info[i].element))
      break;
  dcm->index=i;

  /*
    Check for "explicitness", but meta-file headers always explicit.
  */
  if (ReadBlob(image,2,(char *) explicit_vr) != 2)
    {
      ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
      return MagickFail;
    }
  explicit_vr[2]='\0';
  (void) strlcpy(implicit_vr,dicom_info[dcm->index].vr,MaxTextExtent);

#if defined(NEW_IMPLICIT_LOGIC)
  use_explicit=False;
  if (isupper((int) *explicit_vr) && isupper((int) *(explicit_vr+1)))
    {
      /* Explicit VR looks to be valid */
      if (strcmp(explicit_vr,implicit_vr) == 0)
        {
          /* Explicit VR matches implicit VR so assume that it is explicit */
          use_explicit=True;
        }
      else if ((dcm->group & 1) || strcmp(implicit_vr,"xs") == 0)
        {
          /*
            We *must* use explicit type under two conditions
            1) group is odd and therefore private
            2) element vr is set as "xs" ie is not of a fixed type
          */
          use_explicit=True;
          strcpy(implicit_vr,explicit_vr);
        }
    }
  if ((!use_explicit) || (strcmp(implicit_vr,"!!") == 0))
    {
      /* Use implicit logic */
      (void) SeekBlob(image,(ExtendedSignedIntegralType) -2,SEEK_CUR);
      dcm->quantum=4;
    }
  else
    {
      /* Use explicit logic */
      dcm->quantum=2;
      if ((strcmp(explicit_vr,"OB") == 0) ||
          (strcmp(explicit_vr,"OW") == 0) ||
          (strcmp(explicit_vr,"OF") == 0) ||
          (strcmp(explicit_vr,"SQ") == 0) ||
          (strcmp(explicit_vr,"UN") == 0) ||
          (strcmp(explicit_vr,"UT") == 0))
        {
          (void) dcm->funcReadShort(image);
          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
          dcm->quantum=4;
        }
    }
#else
  if (!dcm->explicit_file && (dcm->group != 0x0002))
    dcm->explicit_file=isupper((int) *explicit_vr) && isupper((int) *(explicit_vr+1));
  use_explicit=((dcm->group == 0x0002) || dcm->explicit_file);
  if (use_explicit && (strcmp(implicit_vr,"xs") == 0))
    (void) strlcpy(implicit_vr,explicit_vr,MaxTextExtent);
  if (!use_explicit || (strcmp(implicit_vr,"!!") == 0))
    {
      (void) SeekBlob(image,(ExtendedSignedIntegralType) -2,SEEK_CUR);
      dcm->quantum=4;
    }
  else
    {
      /*
        Assume explicit type.
      */
      dcm->quantum=2;
      if ((strcmp(explicit_vr,"OB") == 0) ||
          (strcmp(explicit_vr,"UN") == 0) ||
          (strcmp(explicit_vr,"OW") == 0) || (strcmp(explicit_vr,"SQ") == 0))
        {
          (void) dcm->funcReadShort(image)
            if (EOFBlob(image))
              {
                ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
                return MagickFail;
              }
          dcm->quantum=4;
        }
    }
#endif

  dcm->datum=0;
  if (dcm->quantum == 4)
    {
      dcm->datum=(long) dcm->funcReadLong(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if (dcm->quantum == 2)
    {
      dcm->datum=(long) dcm->funcReadShort(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  dcm->quantum=0;
  dcm->length=1;
  if (dcm->datum != 0)
    {
      if ((strcmp(implicit_vr,"SS") == 0) ||
          (strcmp(implicit_vr,"US") == 0) ||
          (strcmp(implicit_vr,"OW") == 0))
        dcm->quantum=2;
      else if ((strcmp(implicit_vr,"UL") == 0) ||
              (strcmp(implicit_vr,"SL") == 0) ||
              (strcmp(implicit_vr,"FL") == 0) ||
              (strcmp(implicit_vr,"OF") == 0))
        dcm->quantum=4;
      else  if (strcmp(implicit_vr,"FD") == 0)
        dcm->quantum=8;
      else
        dcm->quantum=1;

      if (dcm->datum != -1)
        {
          dcm->length=(size_t) dcm->datum/dcm->quantum;
        }
      else
        {
          /*
            Sequence and item of undefined length.
          */
          dcm->quantum=0;
          dcm->length=0;
        }
    }
  /*
    Display Dicom info.
  */
  if (dcm->verbose)
    {
      if (!use_explicit)
        explicit_vr[0]='\0';
      (void) fprintf(stdout,"0x%04lX %4lu %.1024s-%.1024s (0x%04x,0x%04x)",
                     image->offset,(unsigned long) dcm->length,implicit_vr,explicit_vr,
                     dcm->group,dcm->element);
      if (dicom_info[dcm->index].description != (char *) NULL)
        (void) fprintf(stdout," %.1024s",dicom_info[dcm->index].description);
      (void) fprintf(stdout,": ");
    }
  if ((dcm->group == 0x7FE0) && (dcm->element == 0x0010))
    {
      if (dcm->verbose)
        (void) fprintf(stdout,"\n");
      return MagickPass;
    }
  /*
    Allocate array and read data into it
  */
  if ((dcm->length == 1) && (dcm->quantum == 1))
    {
      if ((dcm->datum=ReadBlobByte(image)) == EOF)
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if ((dcm->length == 1) && (dcm->quantum == 2))
    {
      dcm->datum=dcm->funcReadShort(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if ((dcm->length == 1) && (dcm->quantum == 4))
    {
      dcm->datum=(long) dcm->funcReadLong(image);
      if (EOFBlob(image))
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
    }
  else if ((dcm->quantum != 0) && (dcm->length != 0))
    {
      size_t
        size;

      if (dcm->length > ((~0UL)/dcm->quantum))
        {
          ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
          return MagickFail;
        }
      dcm->data=MagickAllocateArray(unsigned char *,(dcm->length+1),dcm->quantum);
      if (dcm->data == (unsigned char *) NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
      size=dcm->quantum*dcm->length;
      if (ReadBlob(image,size,(char *) dcm->data) != size)
        {
          ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
          return MagickFail;
        }
      dcm->data[size]=0;
    }

  if (dcm->verbose)
    {
      /*
        Display data
      */
      if (dcm->data == (unsigned char *) NULL)
        {
          (void) fprintf(stdout,"%d\n",dcm->datum);
        }
      else
        {
          for (i=0; i < (long) Max(dcm->length,4); i++)
            if (!isprint(dcm->data[i]))
              break;
          if ((i != (long) dcm->length) && (dcm->length <= 4))
            {
              long
                j,
                bin_datum;

              bin_datum=0;
              for (j=(long) dcm->length-1; j >= 0; j--)
                bin_datum=256*bin_datum+dcm->data[j];
              (void) fprintf(stdout,"%lu\n",bin_datum);
            }
          else
            {
              for (i=0; i < (long) dcm->length; i++)
                if (isprint(dcm->data[i]))
                  (void) fprintf(stdout,"%c",dcm->data[i]);
                else
                  (void) fprintf(stdout,"%c",'.');
              (void) fprintf(stdout,"\n");
            }
        }
    }
  return MagickPass;
}

static MagickPassFail DCM_SetupColormap(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  if ((image->previous) && (image->previous->colormap != (PixelPacket*)NULL))
    {
      size_t
        length;

      /*
        Clone colormap from previous image
      */
      image->storage_class=PseudoClass;
      image->colors=image->previous->colors;
      length=image->colors*sizeof(PixelPacket);
      image->colormap=MagickAllocateMemory(PixelPacket *,length);
      if (image->colormap == (PixelPacket *) NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
      (void) memcpy(image->colormap,image->previous->colormap,length);
    }
  else
    {
      /*
        Create new colormap
      */
      if (AllocateImageColormap(image,dcm->max_value_out+1) == MagickFail)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
    }
  return MagickPass;
}

static MagickPassFail DCM_SetupRescaleMap(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  /*
    rescale_map maps input sample range -> output colormap range combining rescale
    and window transforms, palette scaling and palette inversion (for MONOCHROME1)
    as well as allowing for pixel_representation of 1 which causes input samples to
    be treated as signed
  */
  double
    win_center,
    win_width,
    Xr,
    Xw_min,
    Xw_max;

  unsigned long
    i;

  if (dcm->rescaling == DCM_RS_NONE)
    return MagickPass;

  if (dcm->rescale_map == (Quantum *) NULL)
    {
      dcm->rescale_map=MagickAllocateArray(Quantum *,dcm->max_value_in+1,sizeof(Quantum));
      if (dcm->rescale_map == NULL)
        {
          ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
          return MagickFail;
        }
    }

  if (dcm->window_width == 0)
    { /* zero window width */
      if (dcm->upper_lim > dcm->lower_lim)
        {
          /* Use known range within image */
          win_width=(dcm->upper_lim-dcm->lower_lim+1)*dcm->rescale_slope;
          win_center=((dcm->upper_lim+dcm->lower_lim)/2)*dcm->rescale_slope+dcm->rescale_intercept;
        }
      else
        {
          /* Use full sample range and hope for the best */
          win_width=(dcm->max_value_in+1)*dcm->rescale_slope;
          if (dcm->pixel_representation == 1)
            win_center=dcm->rescale_intercept;
          else
            win_center=win_width/2 + dcm->rescale_intercept;
        }
    }
  else
    {
      win_width=dcm->window_width;
      win_center=dcm->window_center;
    }
  Xw_min = win_center - 0.5 - ((win_width-1)/2);
  Xw_max = win_center - 0.5 + ((win_width-1)/2);
  for (i=0; i < (dcm->max_value_in+1); i++)
    {
      if ((dcm->pixel_representation == 1) && (i >= (1U << (dcm->significant_bits-1))))
        Xr = -((dcm->max_value_in+1-i) * dcm->rescale_slope) + dcm->rescale_intercept;
      else
        Xr = (i * dcm->rescale_slope) + dcm->rescale_intercept;
      if (Xr <= Xw_min)
        dcm->rescale_map[i]=0;
      else if (Xr >= Xw_max)
        dcm->rescale_map[i]=dcm->max_value_out;
      else
        dcm->rescale_map[i]=(Quantum)(((Xr-Xw_min)/(win_width-1))*dcm->max_value_out+0.5);
    }  
  if (dcm->phot_interp == DCM_PI_MONOCHROME1)
    for (i=0; i < (dcm->max_value_in+1); i++)
      dcm->rescale_map[i]=dcm->max_value_out-dcm->rescale_map[i];
  return MagickPass;
}

void DCM_SetRescaling(DicomStream *dcm,int avoid_scaling)
{
  /*
    If avoid_scaling is True then scaling will only be applied where necessary ie
    input bit depth exceeds quantum size.

    ### TODO : Should this throw an error rather than setting DCM_RS_PRE?
  */
  dcm->rescaling=DCM_RS_NONE;
  dcm->max_value_out=dcm->max_value_in;

  if (dcm->phot_interp == DCM_PI_PALETTE_COLOR)
    {
      if (dcm->max_value_in >= MaxColormapSize)
        {
          dcm->max_value_out=MaxColormapSize-1;
          dcm->rescaling=DCM_RS_PRE;
        }
      return;
    }

  if ((dcm->phot_interp == DCM_PI_MONOCHROME1) ||
      (dcm->phot_interp == DCM_PI_MONOCHROME2))
    {
      if ((dcm->transfer_syntax == DCM_TS_JPEG) ||
          (dcm->transfer_syntax == DCM_TS_JPEG_LS) ||
          (dcm->transfer_syntax == DCM_TS_JPEG_2000))
        {
          /* Encapsulated non-native grayscale images are post rescaled by default */
          if (!avoid_scaling)
            dcm->rescaling=DCM_RS_POST;
        }
#if defined(GRAYSCALE_USES_PALETTE)
      else if (dcm->max_value_in >= MaxColormapSize)
        {
          dcm->max_value_out=MaxColormapSize-1;
          dcm->rescaling=DCM_RS_PRE;
        }
#else
      else if (dcm->max_value_in > MaxRGB)
        {
          dcm->max_value_out=MaxRGB;
          dcm->rescaling=DCM_RS_PRE;
        }
#endif
      else if (!avoid_scaling)
        {
#if !defined(GRAYSCALE_USES_PALETTE)
          dcm->max_value_out=MaxRGB;
#endif
          dcm->rescaling=DCM_RS_POST;
        }
      return;
    }

  if (avoid_scaling || (dcm->max_value_in == MaxRGB))
    return;

  dcm->max_value_out=MaxRGB;
  dcm->rescaling=DCM_RS_PRE;
}

static MagickPassFail DCM_PostRescaleImage(Image *image,DicomStream *dcm,unsigned long ScanLimits,ExceptionInfo *exception)
{
  unsigned long
    y,
    x;

  register PixelPacket
    *q;

  if (ScanLimits)
    {
      /*
        Causes rescan for upper/lower limits - used for encapsulated images
      */
      unsigned int
        l;

      for (y=0; y < image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            return MagickFail;

          if (image->storage_class == PseudoClass)
            {
              register IndexPacket
                *indexes;

              indexes=AccessMutableIndexes(image);
              for (x=0; x < image->columns; x++)
                {
                  l=indexes[x];
                  if (dcm->pixel_representation == 1)
                    if (l > ((dcm->max_value_in >> 1)))
                      l = dcm->max_value_in-l+1;
                  if (l < (unsigned int) dcm->lower_lim)
                    dcm->lower_lim = l;
                  if (l > (unsigned int) dcm->upper_lim)
                    dcm->upper_lim = l;
                }
            }
          else
            {
              for (x=0; x < image->columns; x++)
                {
                  l=q->green;
                  if (dcm->pixel_representation == 1)
                    if (l > (dcm->max_value_in >> 1))
                      l = dcm->max_value_in-l+1;
                  if (l < (unsigned int) dcm->lower_lim)
                    dcm->lower_lim = l;
                  if (l > (unsigned int) dcm->upper_lim)
                    dcm->upper_lim = l;
                  q++;
                }
            }
        }

      if (image->storage_class == PseudoClass)
        {
          /* Handle compressed range by reallocating palette */
          if (!AllocateImageColormap(image,dcm->upper_lim+1))
            {
              ThrowException(exception,ResourceLimitError,UnableToCreateColormap,image->filename);
              return MagickFail;
            }
          return MagickPass;
        }
    }

  DCM_SetupRescaleMap(image,dcm,exception);
  for (y=0; y < image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;

      if (image->storage_class == PseudoClass)
        {
          register IndexPacket
            *indexes;

          indexes=AccessMutableIndexes(image);
          for (x=0; x < image->columns; x++)
            indexes[x]=dcm->rescale_map[indexes[x]];
        }
      else
        {
          for (x=0; x < image->columns; x++)
            {
              q->red=dcm->rescale_map[q->red];
              q->green=dcm->rescale_map[q->green];
              q->blue=dcm->rescale_map[q->blue];
              q++;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      /*
        if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows,exception,
        LoadImageText,image->filename))
        return MagickFail;
      */
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadPaletteImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  long
    y;

  register long
    x;

  register PixelPacket
    *q;

  register IndexPacket
    *indexes;

  unsigned short
    index;

  unsigned char
    byte;

  byte=0;

  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;
      indexes=AccessMutableIndexes(image);
      for (x=0; x < (long) image->columns; x++)
        {
          if (dcm->bytes_per_pixel == 1)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadByte(image,dcm);
              else
                index=ReadBlobByte(image);
            }
          else
          if (dcm->bits_allocated != 12)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadShort(image,dcm);
              else
                index=dcm->funcReadShort(image);
            }
          else
            {
              if (x & 0x01)
                {
                  /* ### TODO - Check whether logic needs altering if msb_state != DCM_MSB_LITTLE */
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadByte(image,dcm);
                  else
                    index=ReadBlobByte(image);
                  index=(index << 4) | byte;
                }
              else
                {
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadByte(image,dcm);
                  else
                    index=dcm->funcReadShort(image);
                  byte=index >> 12;
                  index&=0xfff;
                }
            }
          index&=dcm->max_value_in;
          if (dcm->rescaling == DCM_RS_PRE)
            {
              /*
                ### TODO - must read as Direct Class so look up
                red/green/blue values in palette table, processed via
                rescale_map
              */
            }
          else
            {
#if defined(USE_GRAYMAP)
              if (dcm->graymap != (unsigned short *) NULL)
                index=dcm->graymap[index];
#endif
              index=(IndexPacket) (index);
              VerifyColormapIndex(image,index);
              indexes[x]=index;
            }

          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
                                      image->columns,image->rows))
            return MagickFail;
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadGrayscaleImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  long
    y;

  register long
    x;

  register PixelPacket
    *q;

#if defined(GRAYSCALE_USES_PALETTE)
  register IndexPacket
    *indexes;
#endif

  unsigned short
    index;

  unsigned char
    byte;

  dcm->lower_lim = dcm->max_value_in;
  dcm->upper_lim = -(dcm->lower_lim);
  byte=0;
  for (y=0; y < (long) image->rows; y++)
    {
      q=SetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;
#if defined(GRAYSCALE_USES_PALETTE)
      indexes=AccessMutableIndexes(image);
#endif
      for (x=0; x < (long) image->columns; x++)
        {
          if (dcm->bytes_per_pixel == 1)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadByte(image,dcm);
              else
                index=ReadBlobByte(image);
            }
          else
          if (dcm->bits_allocated != 12)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                index=DCM_RLE_ReadShort(image,dcm);
              else
                index=dcm->funcReadShort(image);
            }
          else
            {
              if (x & 0x01)
                {
                  /* ### TODO - Check whether logic needs altering if msb_state != DCM_MSB_LITTLE */
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadByte(image,dcm);
                  else
                    index=ReadBlobByte(image);
                  index=(index << 4) | byte;
                }
              else
                {
                  if (dcm->transfer_syntax == DCM_TS_RLE)
                    index=DCM_RLE_ReadShort(image,dcm);
                  else
                    index=dcm->funcReadShort(image);
                  byte=index >> 12;
                  index&=0xfff;
                }
            }
          index&=dcm->max_value_in;
          if (dcm->rescaling == DCM_RS_POST)
            {
              unsigned int
                l;

              l = index;
              if (dcm->pixel_representation == 1)
                if (l > (dcm->max_value_in >> 1))
                  l = dcm->max_value_in-l+1;
              if ((int) l < dcm->lower_lim)
                dcm->lower_lim = l;
              if ((int) l > dcm->upper_lim)
                dcm->upper_lim = l;
            }
#if defined(GRAYSCALE_USES_PALETTE)
          if (dcm->rescaling == DCM_RS_PRE)
            indexes[x]=dcm->rescale_map[index];
          else
            indexes[x]=index;
#else
          if (dcm->rescaling == DCM_RS_PRE)
            index=dcm->rescale_map[index];
          q->red=index;
          q->green=index;
          q->blue=index;
          q++;
#endif
          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
                                      image->columns,image->rows))
            return MagickFail;
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadPlanarRGBImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  unsigned long
    plane,
    y,
    x;

  register PixelPacket
    *q;

  for (plane=0; plane < dcm->samples_per_pixel; plane++)
    {
      for (y=0; y < image->rows; y++)
        {
          q=GetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            return MagickFail;

          for (x=0; x < image->columns; x++)
            {
              switch ((int) plane)
                {
                  case 0:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->red=ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm));
                    else
                      q->red=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 1:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->green=ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm));
                    else
                      q->green=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 2:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->blue=ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm));
                    else
                      q->blue=ScaleCharToQuantum(ReadBlobByte(image));
                    break;
                  case 3:
                    if (dcm->transfer_syntax == DCM_TS_RLE)
                      q->opacity=ScaleCharToQuantum((Quantum)(MaxRGB-ScaleCharToQuantum(DCM_RLE_ReadByte(image,dcm))));
                    else
                      q->opacity=ScaleCharToQuantum((Quantum)(MaxRGB-ScaleCharToQuantum(ReadBlobByte(image))));
                    break;
                }
              if (EOFBlob(image))
                {
                  ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
                  return MagickFail;
                }
              q++;
            }
          if (!SyncImagePixels(image))
            return MagickFail;
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,exception,
                                          LoadImageText,image->filename,
                                          image->columns,image->rows))
                return MagickFail;
        }
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadRGBImage(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  unsigned long
    i,
    y,
    x;

  register PixelPacket
    *q;

  Quantum
    blue,
    green,
    red;

  unsigned char
    byte;

  red=0;
  green=0;
  blue=0;
  i=0;
  byte=0;

  for (y=0; y < image->rows; y++)
    {
      q=GetImagePixels(image,0,y,image->columns,1);
      if (q == (PixelPacket *) NULL)
        return MagickFail;

      for (x=0; x < image->columns; x++)
        {
          if (dcm->bytes_per_pixel == 1)
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                {
                  red=DCM_RLE_ReadByte(image,dcm);
                  green=DCM_RLE_ReadByte(image,dcm);
                  blue=DCM_RLE_ReadByte(image,dcm);

                }
              else
                {
                  red=ReadBlobByte(image);
                  green=ReadBlobByte(image);
                  blue=ReadBlobByte(image);
                }
            }
          else
            {
              if (dcm->transfer_syntax == DCM_TS_RLE)
                {
                  red=DCM_RLE_ReadShort(image,dcm);
                  green=DCM_RLE_ReadShort(image,dcm);
                  blue=DCM_RLE_ReadShort(image,dcm);
                }
              else
                {
                  red=dcm->funcReadShort(image);
                  green=dcm->funcReadShort(image);
                  blue=dcm->funcReadShort(image);
                }
            }
          red&=dcm->max_value_in;
          green&=dcm->max_value_in;
          blue&=dcm->max_value_in;
          if (dcm->rescaling == DCM_RS_PRE)
            {
              red=dcm->rescale_map[red];
              green=dcm->rescale_map[green];
              blue=dcm->rescale_map[blue];
            }

          q->red=(Quantum) red;
          q->green=(Quantum) green;
          q->blue=(Quantum) blue;
          q++;
          if (EOFBlob(image))
            {
              ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,image->filename);
              return MagickFail;
            }
        }
      if (!SyncImagePixels(image))
        return MagickFail;
      if (image->previous == (Image *) NULL)
        if (QuantumTick(y,image->rows))
          if (!MagickMonitorFormatted(y,image->rows,exception,
                                      LoadImageText,image->filename,
                                      image->columns,image->rows))
            return MagickFail;
    }
  return MagickPass;
}

static MagickPassFail DCM_ReadOffsetTable(Image *image,DicomStream *dcm,ExceptionInfo *exception)
{
  magick_uint32_t
    base_offset,
    tag,
    length,
    i;

  tag=(dcm->funcReadShort(image) << 16) | dcm->funcReadShort(image);
  length=dcm->funcReadLong(image);
  if (tag != 0xFFFEE000)
    return MagickFail;

  dcm->offset_ct=length >> 2;
  if (dcm->offset_ct == 0)
    return MagickPass;

  if (dcm->offset_ct != dcm->number_scenes)
    {
      ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
      return MagickFail;
    }

  dcm->offset_arr=MagickAllocateArray(magick_uint32_t *,dcm->offset_ct,sizeof(magick_uint32_t));  
  if (dcm->offset_arr == (magick_uint32_t) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,image->filename);
      return MagickFail;
    }

  for (i=0; i < dcm->offset_ct; i++)
  {
    dcm->offset_arr[i]=dcm->funcReadLong(image);
    if (EOFBlob(image))
      return MagickFail;
  }
  base_offset=(magick_uint32_t)TellBlob(image);
  for (i=0; i < dcm->offset_ct; i++)
    dcm->offset_arr[i]+=base_offset;

  /*
     Seek first fragment of first frame if necessary (although it shouldn't be...)
  */
  if (TellBlob(image) != dcm->offset_arr[0])
    SeekBlob(image,dcm->offset_arr[0],SEEK_SET);
  return MagickPass;
}

static MagickPassFail DCM_ReadNonNativeImages(Image **image,const ImageInfo *image_info,DicomStream *dcm,ExceptionInfo *exception)
{
  char
    filename[MaxTextExtent];

  FILE
    *file;

  int
    c;

  Image
    *image_list,
    *next_image;

  ImageInfo
    *clone_info;

  magick_uint32_t
    scene,
    tag,
    status,
    length,
    i;

  image_list=(Image *)NULL;

  /*
    Read offset table
  */
  if (DCM_ReadOffsetTable(*image,dcm,exception) == MagickFail)
    return MagickFail;

  status=MagickPass;
  for (scene=0; scene < dcm->number_scenes; scene++)
    {
      /*
        Use temporary file to hold extracted data stream
      */
      file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
      if (file == (FILE *) NULL)
        {
          ThrowException(exception,FileOpenError,UnableToCreateTemporaryFile,filename);
          return MagickFail;
        }

      status=MagickPass;
      do
        {
          /*
            Read fragment tag
          */
          tag=(dcm->funcReadShort(*image) << 16) | dcm->funcReadShort(*image);
          length=dcm->funcReadLong(*image);
          if (EOFBlob(*image))
            {
              status=MagickFail;
              break;
            }

          if (tag == 0xFFFEE0DD)
            {
              /* Sequence delimiter tag */
              break;
            }
          else
            if (tag != 0xFFFEE000)
              {
                status=MagickFail;
                break;
              }

          /*
            Copy this fragment to the temporary file
          */
          while (length > 0)
            {
              c=ReadBlobByte(*image);
              if (c == EOF)
                {
                  status=MagickFail;
                  break;
                }
              (void) fputc(c,file);
              length--;
            }

          if (dcm->offset_ct == 0)
            {
              /*
                Assume one fragment per frame so break loop unless we are in the last frame
              */
              if (scene < dcm->number_scenes-1)
                break;
            }
          else
            {
              /* Lookk for end of multi-fragment frames by checking for against offset table */
              length=TellBlob(*image);
              for (i=0; i <= dcm->offset_ct; i++)
                if (length == dcm->offset_arr[i])
                  {
                    break;
                  }
            }
        } while (status == MagickPass);

      (void) fclose(file);
      if (status == MagickPass)
        {
          clone_info=CloneImageInfo(image_info);
          clone_info->blob=(void *) NULL;
          clone_info->length=0;
          if (dcm->transfer_syntax == DCM_TS_JPEG_2000)
            FormatString(clone_info->filename,"jp2:%.1024s",filename);
          else
            FormatString(clone_info->filename,"jpeg:%.1024s",filename);
          next_image=ReadImage(clone_info,exception);
          DestroyImageInfo(clone_info);
          if (next_image == (Image*)NULL)
            {
              status=MagickFail;
            }
          else
            if (dcm->rescaling == DCM_RS_POST)
              {
                /*
                  ### TODO: ???
                  We cannot guarantee integrity of input data since libjpeg may already have
                  downsampled 12- or 16-bit jpegs. Best guess at the moment is to recalculate
                  scaling using the image depth (unless avoid-scaling is in force)
                */
                /* Allow for libjpeg having changed depth of image */
                dcm->significant_bits=next_image->depth;
                dcm->bytes_per_pixel=1;
                if (dcm->significant_bits > 8)
                  dcm->bytes_per_pixel=2;
                dcm->max_value_in=(1 << dcm->significant_bits)-1;
                dcm->max_value_out=dcm->max_value_in;
                status=DCM_PostRescaleImage(next_image,dcm,True,exception);
              }
          if (status == MagickPass)
            {
              strcpy(next_image->filename,(*image)->filename);
              next_image->scene=scene;
              if (image_list == (Image*)NULL)
                image_list=next_image;
              else
                AppendImageToList(&image_list,next_image);
            }
        }
      (void) LiberateTemporaryFile(filename);
    }
  DestroyImage(*image);
  *image=image_list;
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d D C M I m a g e                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadDCMImage reads a Digital Imaging and Communications in Medicine
%  (DICOM) file and returns it.  It It allocates the memory necessary for the
%  new Image structure and returns a pointer to the new image.
%
%  The format of the ReadDCMImage method is:
%
%      Image *ReadDCMImage(const ImageInfo *image_info,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadDCMImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadDCMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
  char
    magick[MaxTextExtent];

  Image
    *image;

  int
    scene;

  size_t
    count;

  unsigned long
    status;

  DicomStream
    dcm;

  /*
    Open image file
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFail)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);

  /*
    Read DCM preamble
  */
  if ((count=ReadBlob(image,128,(char *) magick)) != 128)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if ((count=ReadBlob(image,4,(char *) magick)) != 4)
    ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
  if (image->logging)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                          "magick: \"%.4s\"",magick);
  if (LocaleNCompare((char *) magick,"DICM",4) != 0)
    (void) SeekBlob(image,0L,SEEK_SET);

  /*
    Loop to read DCM image header one element at a time
  */
  (void) DCM_InitDCM(&dcm,image_info->verbose);
  status=DCM_ReadElement(image,&dcm,exception);
  while ((status == MagickPass) && ((dcm.group != 0x7FE0) || (dcm.element != 0x0010)))
    {
      DicomElemParseFunc
        *pfunc;

      pfunc=dicom_info[dcm.index].pfunc;
      if (pfunc != (DicomElemParseFunc *)NULL)
        status=pfunc(image,&dcm,exception);
      MagickFreeMemory(dcm.data);
      dcm.data = NULL;
      if (status == MagickPass)
        status=DCM_ReadElement(image,&dcm,exception);
    }
#if defined(IGNORE_WINDOW_FOR_UNSPECIFIED_SCALE_TYPE)
  if (dcm.rescale_type == DCM_RT_UNSPECIFIED)
    {
      dcm.window_width=0;
      dcm.rescale_slope=1;
      dcm.rescale_intercept=0;
    }
#endif
  DCM_SetRescaling(&dcm,(AccessDefinition(image_info,"dcm","avoid-scaling") != NULL));
  /*
    Now process the image data
  */
  if (status == MagickFail)
    ; 
  else
    if ((dcm.columns == 0) || (dcm.rows == 0))
      {
        ThrowException(exception,CorruptImageError,ImproperImageHeader,image->filename);
        status=MagickFail;
      }
    else if ((dcm.transfer_syntax != DCM_TS_IMPL_LITTLE) &&
             (dcm.transfer_syntax != DCM_TS_EXPL_LITTLE) &&
             (dcm.transfer_syntax != DCM_TS_EXPL_BIG) &&
             (dcm.transfer_syntax != DCM_TS_RLE))
      {
        status=DCM_ReadNonNativeImages(&image,image_info,&dcm,exception);
        dcm.number_scenes=0;
      }
    else if (dcm.rescaling != DCM_RS_POST)
      {
        status=DCM_SetupRescaleMap(image,&dcm,exception);
      }

  if (dcm.transfer_syntax == DCM_TS_RLE)
    status=DCM_ReadOffsetTable(image,&dcm,exception);

  /* Loop to process all scenes in image */
  if (status == MagickFail) dcm.number_scenes = 0;
  for (scene=0; scene < (long) dcm.number_scenes; scene++)
    {
      if (dcm.transfer_syntax == DCM_TS_RLE)
        {
          magick_uint32_t
            tag,
            length;

          /*
            Discard any remaining bytes from last fragment
          */
          if (dcm.frag_bytes)
            SeekBlob(image,dcm.frag_bytes,SEEK_CUR);

          /*
           Read fragment tag
          */
          tag=(dcm.funcReadShort(image) << 16) | dcm.funcReadShort(image);
          length=dcm.funcReadLong(image);
          if ((tag != 0xFFFEE000) || (length <= 64) || EOFBlob(image))
            {
              status=MagickFail;
              ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
              break;
            }

          /*
            Set up decompression state
          */
          dcm.frag_bytes=length;
          dcm.rle_rep_ct=0;

          /*
            Read RLE segment table
          */
          dcm.rle_seg_ct=dcm.funcReadLong(image);
          for (length=0; length < 15; length++)
            dcm.rle_seg_offsets[length]=dcm.funcReadLong(image);
          dcm.frag_bytes-=64;
          if (EOFBlob(image))
            {
              status=MagickFail;
              ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
              break;
            }
          if (dcm.rle_seg_ct > 1)
            {
              fprintf(stdout,"Multiple RLE segments in frame are not supported\n");
              status=MagickFail;
              break;
            }
        }

      /*
        Initialize image structure.
      */
      image->columns=dcm.columns;
      image->rows=dcm.rows;
      image->interlace=(dcm.interlace==1)?PlaneInterlace:NoInterlace;
#if defined(GRAYSCALE_USES_PALETTE)
      if ((image->colormap == (PixelPacket *) NULL) && (dcm.samples_per_pixel == 1))
#else
        if ((image->colormap == (PixelPacket *) NULL) && (dcm.phot_interp == DCM_PI_PALETTE_COLOR))
#endif
          {
            status=DCM_SetupColormap(image,&dcm,exception);
            if (status == MagickFail)
              break;
          }
      if (image_info->ping)
        break;

      /*
        Process image according to type
      */
      if (dcm.samples_per_pixel == 1)
        {
          if (dcm.phot_interp == DCM_PI_PALETTE_COLOR)
            status=DCM_ReadPaletteImage(image,&dcm,exception);
          else
            status=DCM_ReadGrayscaleImage(image,&dcm,exception);
        }
      else
        {
          if (image->interlace == PlaneInterlace)
            status=DCM_ReadPlanarRGBImage(image,&dcm,exception);
          else
            status=DCM_ReadRGBImage(image,&dcm,exception);
        }
      if (status != MagickPass)
        break;

      if ((dcm.rescaling == DCM_RS_PRE) &&
          ((dcm.phot_interp == DCM_PI_MONOCHROME1) ||
           (dcm.phot_interp == DCM_PI_MONOCHROME2)))
        {
          NormalizeImage(image);
        }
      else
        if (dcm.rescaling == DCM_RS_POST)
          {
            status = DCM_PostRescaleImage(image,&dcm,False,exception);
            if (status != MagickPass)
              break;
          }

      /*
        Proceed to next image.
      */
      if (image_info->subrange != 0)
        if (image->scene >= (image_info->subimage+image_info->subrange-1))
          break;
      if (scene < (long) (dcm.number_scenes-1))
        {
          /*
            Allocate next image structure.
          */
          AllocateNextImage(image_info,image);
          if (image->next == (Image *) NULL)
            {
              status=MagickFail;
              break;
            }
          image=SyncNextImageInList(image);
          status=MagickMonitorFormatted(TellBlob(image),GetBlobSize(image),
                                        exception,LoadImagesText,
                                        image->filename);
          if (status == MagickFail)
            break;
        }
    }

  /*
    Free allocated resources
  */

  if (dcm.offset_arr != NULL)
    MagickFreeMemory(dcm.offset_arr);    
  if (dcm.data != NULL)
    MagickFreeMemory(dcm.data);
#if defined(USE_GRAYMAP)
  if (dcm.graymap != (unsigned short *) NULL)
    MagickFreeMemory(dcm.graymap);
#endif
  if (dcm.rescale_map != (Quantum *) NULL)
    MagickFreeMemory(dcm.rescale_map);
  if (status == MagickPass)
    {
      while (image->previous != (Image *) NULL)
        image=image->previous;
      CloseBlob(image);
      return(image);
    }
  else
    {
      DestroyImageList(image);
      return((Image *) NULL);
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r D C M I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterDCMImage adds attributes for the DCM image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterDCMImage method is:
%
%      RegisterDCMImage(void)
%
*/
ModuleExport void RegisterDCMImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("DCM");
  entry->decoder=(DecoderHandler) ReadDCMImage;
  entry->magick=(MagickHandler) IsDCM;
  entry->adjoin=False;
  entry->seekable_stream=True;
  entry->description="Digital Imaging and Communications in Medicine image";
  entry->note="See http://medical.nema.org/ for information on DICOM.";
  entry->module="DCM";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r D C M I m a g e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterDCMImage removes format registrations made by the
%  DCM module from the list of supported formats.
%
%  The format of the UnregisterDCMImage method is:
%
%      UnregisterDCMImage(void)
%
*/
ModuleExport void UnregisterDCMImage(void)
{
  (void) UnregisterMagickInfo("DCM");
}
/*
   ### TODO :
   #1 Fixes on palette support:
                - Handle palette images where # of entries > MaxColormapSize - create image
                  as Direct class, store the original palette (scaled to MaxRGB) and then map
                  input values via modified palette to output RGB values.
        - Honour palette/LUT descriptors (ie values <= min value map to first
                  entry, value = (min_value + 1) maps to second entry, and so on, whilst
                  values >= (min value + palette/LUT size) map to last entry.
   #2 Use ImportImagePixelArea?
   #3 Handling of encapsulated JPEGs which downsample to 8 bit via
      libjpeg. These lose accuracy before we can rescale to handle the
          issues of PR=1 + window center/width + rescale slope/intercept on
          MONOCHROME1 or 2. Worst case : CT-MONO2-16-chest. Currently images
          are post-rescaled based on sample range. For PseudoClass grayscales
          this is done by colormap manipulation only.
   #4 JPEG/JPEG-LS/JPEG 2000: Check that multi frame handling in DCM_ReadEncapImages
      is ok
   #5 Support LUTs?
   #6 Pixel Padding value/range - make transparent or allow use to specify a color?
*/
