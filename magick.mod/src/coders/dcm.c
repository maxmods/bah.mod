/*
% Copyright (C) 2003 GraphicsMagick Group
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
#include "magick/pixel_cache.h"
#include "magick/color.h"
#include "magick/constitute.h"
#include "magick/enhance.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/monitor.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Dicom medical image declarations.
*/

typedef struct _DicomInfo
{
  unsigned short
    group,
    element;

  char
    *vr,
    *description;
} DicomInfo;

static const DicomInfo
  dicom_info[] =
  {
    { 0x0000, 0x0000, (char *) "UL", (char *) "Group Length" },
    { 0x0000, 0x0001, (char *) "UL", (char *) "Command Length to End" },
    { 0x0000, 0x0002, (char *) "UI", (char *) "Affected SOP Class UID" },
    { 0x0000, 0x0003, (char *) "UI", (char *) "Requested SOP Class UID" },
    { 0x0000, 0x0010, (char *) "LO", (char *) "Command Recognition Code" },
    { 0x0000, 0x0100, (char *) "US", (char *) "Command Field" },
    { 0x0000, 0x0110, (char *) "US", (char *) "Message ID" },
    { 0x0000, 0x0120, (char *) "US", (char *) "Message ID Being Responded To" },
    { 0x0000, 0x0200, (char *) "AE", (char *) "Initiator" },
    { 0x0000, 0x0300, (char *) "AE", (char *) "Receiver" },
    { 0x0000, 0x0400, (char *) "AE", (char *) "Find Location" },
    { 0x0000, 0x0600, (char *) "AE", (char *) "Move Destination" },
    { 0x0000, 0x0700, (char *) "US", (char *) "Priority" },
    { 0x0000, 0x0800, (char *) "US", (char *) "Data Set Type" },
    { 0x0000, 0x0850, (char *) "US", (char *) "Number of Matches" },
    { 0x0000, 0x0860, (char *) "US", (char *) "Response Sequence Number" },
    { 0x0000, 0x0900, (char *) "US", (char *) "Status" },
    { 0x0000, 0x0901, (char *) "AT", (char *) "Offending Element" },
    { 0x0000, 0x0902, (char *) "LO", (char *) "Exception Comment" },
    { 0x0000, 0x0903, (char *) "US", (char *) "Exception ID" },
    { 0x0000, 0x1000, (char *) "UI", (char *) "Affected SOP Instance UID" },
    { 0x0000, 0x1001, (char *) "UI", (char *) "Requested SOP Instance UID" },
    { 0x0000, 0x1002, (char *) "US", (char *) "Event Type ID" },
    { 0x0000, 0x1005, (char *) "AT", (char *) "Attribute Identifier List" },
    { 0x0000, 0x1008, (char *) "US", (char *) "Action Type ID" },
    { 0x0000, 0x1020, (char *) "US", (char *) "Number of Remaining Suboperations" },
    { 0x0000, 0x1021, (char *) "US", (char *) "Number of Completed Suboperations" },
    { 0x0000, 0x1022, (char *) "US", (char *) "Number of Failed Suboperations" },
    { 0x0000, 0x1023, (char *) "US", (char *) "Number of Warning Suboperations" },
    { 0x0000, 0x1030, (char *) "AE", (char *) "Move Originator Application Entity Title" },
    { 0x0000, 0x1031, (char *) "US", (char *) "Move Originator Message ID" },
    { 0x0000, 0x4000, (char *) "LO", (char *) "Dialog Receiver" },
    { 0x0000, 0x4010, (char *) "LO", (char *) "Terminal Type" },
    { 0x0000, 0x5010, (char *) "SH", (char *) "Message Set ID" },
    { 0x0000, 0x5020, (char *) "SH", (char *) "End Message Set" },
    { 0x0000, 0x5110, (char *) "LO", (char *) "Display Format" },
    { 0x0000, 0x5120, (char *) "LO", (char *) "Page Position ID" },
    { 0x0000, 0x5130, (char *) "LO", (char *) "Text Format ID" },
    { 0x0000, 0x5140, (char *) "LO", (char *) "Normal Reverse" },
    { 0x0000, 0x5150, (char *) "LO", (char *) "Add Gray Scale" },
    { 0x0000, 0x5160, (char *) "LO", (char *) "Borders" },
    { 0x0000, 0x5170, (char *) "IS", (char *) "Copies" },
    { 0x0000, 0x5180, (char *) "LO", (char *) "OldMagnificationType" },
    { 0x0000, 0x5190, (char *) "LO", (char *) "Erase" },
    { 0x0000, 0x51a0, (char *) "LO", (char *) "Print" },
    { 0x0000, 0x51b0, (char *) "US", (char *) "Overlays" },
    { 0x0002, 0x0000, (char *) "UL", (char *) "Meta Element Group Length" },
    { 0x0002, 0x0001, (char *) "OB", (char *) "File Meta Information Version" },
    { 0x0002, 0x0002, (char *) "UI", (char *) "Media Storage SOP Class UID" },
    { 0x0002, 0x0003, (char *) "UI", (char *) "Media Storage SOP Instance UID" },
    { 0x0002, 0x0010, (char *) "UI", (char *) "Transfer Syntax UID" },
    { 0x0002, 0x0012, (char *) "UI", (char *) "Implementation Class UID" },
    { 0x0002, 0x0013, (char *) "SH", (char *) "Implementation Version Name" },
    { 0x0002, 0x0016, (char *) "AE", (char *) "Source Application Entity Title" },
    { 0x0002, 0x0100, (char *) "UI", (char *) "Private Information Creator UID" },
    { 0x0002, 0x0102, (char *) "OB", (char *) "Private Information" },
    { 0x0003, 0x0000, (char *) "US", (char *) "?" },
    { 0x0003, 0x0008, (char *) "US", (char *) "ISI Command Field" },
    { 0x0003, 0x0011, (char *) "US", (char *) "Attach ID Application Code" },
    { 0x0003, 0x0012, (char *) "UL", (char *) "Attach ID Message Count" },
    { 0x0003, 0x0013, (char *) "DA", (char *) "Attach ID Date" },
    { 0x0003, 0x0014, (char *) "TM", (char *) "Attach ID Time" },
    { 0x0003, 0x0020, (char *) "US", (char *) "Message Type" },
    { 0x0003, 0x0030, (char *) "DA", (char *) "Max Waiting Date" },
    { 0x0003, 0x0031, (char *) "TM", (char *) "Max Waiting Time" },
    { 0x0004, 0x0000, (char *) "UL", (char *) "File Set Group Length" },
    { 0x0004, 0x1130, (char *) "CS", (char *) "File Set ID" },
    { 0x0004, 0x1141, (char *) "CS", (char *) "File Set Descriptor File ID" },
    { 0x0004, 0x1142, (char *) "CS", (char *) "File Set Descriptor File Specific Character Set" },
    { 0x0004, 0x1200, (char *) "UL", (char *) "Root Directory Entity First Directory Record Offset" },
    { 0x0004, 0x1202, (char *) "UL", (char *) "Root Directory Entity Last Directory Record Offset" },
    { 0x0004, 0x1212, (char *) "US", (char *) "File Set Consistency Flag" },
    { 0x0004, 0x1220, (char *) "SQ", (char *) "Directory Record Sequence" },
    { 0x0004, 0x1400, (char *) "UL", (char *) "Next Directory Record Offset" },
    { 0x0004, 0x1410, (char *) "US", (char *) "Record In Use Flag" },
    { 0x0004, 0x1420, (char *) "UL", (char *) "Referenced Lower Level Directory Entity Offset" },
    { 0x0004, 0x1430, (char *) "CS", (char *) "Directory Record Type" },
    { 0x0004, 0x1432, (char *) "UI", (char *) "Private Record UID" },
    { 0x0004, 0x1500, (char *) "CS", (char *) "Referenced File ID" },
    { 0x0004, 0x1504, (char *) "UL", (char *) "MRDR Directory Record Offset" },
    { 0x0004, 0x1510, (char *) "UI", (char *) "Referenced SOP Class UID In File" },
    { 0x0004, 0x1511, (char *) "UI", (char *) "Referenced SOP Instance UID In File" },
    { 0x0004, 0x1512, (char *) "UI", (char *) "Referenced Transfer Syntax UID In File" },
    { 0x0004, 0x1600, (char *) "UL", (char *) "Number of References" },
    { 0x0005, 0x0000, (char *) "US", (char *) "?" },
    { 0x0006, 0x0000, (char *) "US", (char *) "?" },
    { 0x0008, 0x0000, (char *) "UL", (char *) "Identifying Group Length" },
    { 0x0008, 0x0001, (char *) "UL", (char *) "Length to End" },
    { 0x0008, 0x0005, (char *) "CS", (char *) "Specific Character Set" },
    { 0x0008, 0x0008, (char *) "CS", (char *) "Image Type" },
    { 0x0008, 0x0010, (char *) "LO", (char *) "Recognition Code" },
    { 0x0008, 0x0012, (char *) "DA", (char *) "Instance Creation Date" },
    { 0x0008, 0x0013, (char *) "TM", (char *) "Instance Creation Time" },
    { 0x0008, 0x0014, (char *) "UI", (char *) "Instance Creator UID" },
    { 0x0008, 0x0016, (char *) "UI", (char *) "SOP Class UID" },
    { 0x0008, 0x0018, (char *) "UI", (char *) "SOP Instance UID" },
    { 0x0008, 0x0020, (char *) "DA", (char *) "Study Date" },
    { 0x0008, 0x0021, (char *) "DA", (char *) "Series Date" },
    { 0x0008, 0x0022, (char *) "DA", (char *) "Acquisition Date" },
    { 0x0008, 0x0023, (char *) "DA", (char *) "Image Date" },
    { 0x0008, 0x0024, (char *) "DA", (char *) "Overlay Date" },
    { 0x0008, 0x0025, (char *) "DA", (char *) "Curve Date" },
    { 0x0008, 0x0030, (char *) "TM", (char *) "Study Time" },
    { 0x0008, 0x0031, (char *) "TM", (char *) "Series Time" },
    { 0x0008, 0x0032, (char *) "TM", (char *) "Acquisition Time" },
    { 0x0008, 0x0033, (char *) "TM", (char *) "Image Time" },
    { 0x0008, 0x0034, (char *) "TM", (char *) "Overlay Time" },
    { 0x0008, 0x0035, (char *) "TM", (char *) "Curve Time" },
    { 0x0008, 0x0040, (char *) "xs", (char *) "Old Data Set Type" },
    { 0x0008, 0x0041, (char *) "xs", (char *) "Old Data Set Subtype" },
    { 0x0008, 0x0042, (char *) "CS", (char *) "Nuclear Medicine Series Type" },
    { 0x0008, 0x0050, (char *) "SH", (char *) "Accession Number" },
    { 0x0008, 0x0052, (char *) "CS", (char *) "Query/Retrieve Level" },
    { 0x0008, 0x0054, (char *) "AE", (char *) "Retrieve AE Title" },
    { 0x0008, 0x0058, (char *) "UI", (char *) "Failed SOP Instance UID List" },
    { 0x0008, 0x0060, (char *) "CS", (char *) "Modality" },
    { 0x0008, 0x0062, (char *) "SQ", (char *) "Modality Subtype" },
    { 0x0008, 0x0064, (char *) "CS", (char *) "Conversion Type" },
    { 0x0008, 0x0068, (char *) "CS", (char *) "Presentation Intent Type" },
    { 0x0008, 0x0070, (char *) "LO", (char *) "Manufacturer" },
    { 0x0008, 0x0080, (char *) "LO", (char *) "Institution Name" },
    { 0x0008, 0x0081, (char *) "ST", (char *) "Institution Address" },
    { 0x0008, 0x0082, (char *) "SQ", (char *) "Institution Code Sequence" },
    { 0x0008, 0x0090, (char *) "PN", (char *) "Referring Physician's Name" },
    { 0x0008, 0x0092, (char *) "ST", (char *) "Referring Physician's Address" },
    { 0x0008, 0x0094, (char *) "SH", (char *) "Referring Physician's Telephone Numbers" },
    { 0x0008, 0x0100, (char *) "SH", (char *) "Code Value" },
    { 0x0008, 0x0102, (char *) "SH", (char *) "Coding Scheme Designator" },
    { 0x0008, 0x0103, (char *) "SH", (char *) "Coding Scheme Version" },
    { 0x0008, 0x0104, (char *) "LO", (char *) "Code Meaning" },
    { 0x0008, 0x0105, (char *) "CS", (char *) "Mapping Resource" },
    { 0x0008, 0x0106, (char *) "DT", (char *) "Context Group Version" },
    { 0x0008, 0x010b, (char *) "CS", (char *) "Code Set Extension Flag" },
    { 0x0008, 0x010c, (char *) "UI", (char *) "Private Coding Scheme Creator UID" },
    { 0x0008, 0x010d, (char *) "UI", (char *) "Code Set Extension Creator UID" },
    { 0x0008, 0x010f, (char *) "CS", (char *) "Context Identifier" },
    { 0x0008, 0x1000, (char *) "LT", (char *) "Network ID" },
    { 0x0008, 0x1010, (char *) "SH", (char *) "Station Name" },
    { 0x0008, 0x1030, (char *) "LO", (char *) "Study Description" },
    { 0x0008, 0x1032, (char *) "SQ", (char *) "Procedure Code Sequence" },
    { 0x0008, 0x103e, (char *) "LO", (char *) "Series Description" },
    { 0x0008, 0x1040, (char *) "LO", (char *) "Institutional Department Name" },
    { 0x0008, 0x1048, (char *) "PN", (char *) "Physician of Record" },
    { 0x0008, 0x1050, (char *) "PN", (char *) "Performing Physician's Name" },
    { 0x0008, 0x1060, (char *) "PN", (char *) "Name of Physician(s) Reading Study" },
    { 0x0008, 0x1070, (char *) "PN", (char *) "Operator's Name" },
    { 0x0008, 0x1080, (char *) "LO", (char *) "Admitting Diagnosis Description" },
    { 0x0008, 0x1084, (char *) "SQ", (char *) "Admitting Diagnosis Code Sequence" },
    { 0x0008, 0x1090, (char *) "LO", (char *) "Manufacturer's Model Name" },
    { 0x0008, 0x1100, (char *) "SQ", (char *) "Referenced Results Sequence" },
    { 0x0008, 0x1110, (char *) "SQ", (char *) "Referenced Study Sequence" },
    { 0x0008, 0x1111, (char *) "SQ", (char *) "Referenced Study Component Sequence" },
    { 0x0008, 0x1115, (char *) "SQ", (char *) "Referenced Series Sequence" },
    { 0x0008, 0x1120, (char *) "SQ", (char *) "Referenced Patient Sequence" },
    { 0x0008, 0x1125, (char *) "SQ", (char *) "Referenced Visit Sequence" },
    { 0x0008, 0x1130, (char *) "SQ", (char *) "Referenced Overlay Sequence" },
    { 0x0008, 0x1140, (char *) "SQ", (char *) "Referenced Image Sequence" },
    { 0x0008, 0x1145, (char *) "SQ", (char *) "Referenced Curve Sequence" },
    { 0x0008, 0x1148, (char *) "SQ", (char *) "Referenced Previous Waveform" },
    { 0x0008, 0x114a, (char *) "SQ", (char *) "Referenced Simultaneous Waveforms" },
    { 0x0008, 0x114c, (char *) "SQ", (char *) "Referenced Subsequent Waveform" },
    { 0x0008, 0x1150, (char *) "UI", (char *) "Referenced SOP Class UID" },
    { 0x0008, 0x1155, (char *) "UI", (char *) "Referenced SOP Instance UID" },
    { 0x0008, 0x1160, (char *) "IS", (char *) "Referenced Frame Number" },
    { 0x0008, 0x1195, (char *) "UI", (char *) "Transaction UID" },
    { 0x0008, 0x1197, (char *) "US", (char *) "Failure Reason" },
    { 0x0008, 0x1198, (char *) "SQ", (char *) "Failed SOP Sequence" },
    { 0x0008, 0x1199, (char *) "SQ", (char *) "Referenced SOP Sequence" },
    { 0x0008, 0x2110, (char *) "CS", (char *) "Old Lossy Image Compression" },
    { 0x0008, 0x2111, (char *) "ST", (char *) "Derivation Description" },
    { 0x0008, 0x2112, (char *) "SQ", (char *) "Source Image Sequence" },
    { 0x0008, 0x2120, (char *) "SH", (char *) "Stage Name" },
    { 0x0008, 0x2122, (char *) "IS", (char *) "Stage Number" },
    { 0x0008, 0x2124, (char *) "IS", (char *) "Number of Stages" },
    { 0x0008, 0x2128, (char *) "IS", (char *) "View Number" },
    { 0x0008, 0x2129, (char *) "IS", (char *) "Number of Event Timers" },
    { 0x0008, 0x212a, (char *) "IS", (char *) "Number of Views in Stage" },
    { 0x0008, 0x2130, (char *) "DS", (char *) "Event Elapsed Time(s)" },
    { 0x0008, 0x2132, (char *) "LO", (char *) "Event Timer Name(s)" },
    { 0x0008, 0x2142, (char *) "IS", (char *) "Start Trim" },
    { 0x0008, 0x2143, (char *) "IS", (char *) "Stop Trim" },
    { 0x0008, 0x2144, (char *) "IS", (char *) "Recommended Display Frame Rate" },
    { 0x0008, 0x2200, (char *) "CS", (char *) "Transducer Position" },
    { 0x0008, 0x2204, (char *) "CS", (char *) "Transducer Orientation" },
    { 0x0008, 0x2208, (char *) "CS", (char *) "Anatomic Structure" },
    { 0x0008, 0x2218, (char *) "SQ", (char *) "Anatomic Region Sequence" },
    { 0x0008, 0x2220, (char *) "SQ", (char *) "Anatomic Region Modifier Sequence" },
    { 0x0008, 0x2228, (char *) "SQ", (char *) "Primary Anatomic Structure Sequence" },
    { 0x0008, 0x2230, (char *) "SQ", (char *) "Primary Anatomic Structure Modifier Sequence" },
    { 0x0008, 0x2240, (char *) "SQ", (char *) "Transducer Position Sequence" },
    { 0x0008, 0x2242, (char *) "SQ", (char *) "Transducer Position Modifier Sequence" },
    { 0x0008, 0x2244, (char *) "SQ", (char *) "Transducer Orientation Sequence" },
    { 0x0008, 0x2246, (char *) "SQ", (char *) "Transducer Orientation Modifier Sequence" },
    { 0x0008, 0x2251, (char *) "SQ", (char *) "Anatomic Structure Space Or Region Code Sequence" },
    { 0x0008, 0x2253, (char *) "SQ", (char *) "Anatomic Portal Of Entrance Code Sequence" },
    { 0x0008, 0x2255, (char *) "SQ", (char *) "Anatomic Approach Direction Code Sequence" },
    { 0x0008, 0x2256, (char *) "ST", (char *) "Anatomic Perspective Description" },
    { 0x0008, 0x2257, (char *) "SQ", (char *) "Anatomic Perspective Code Sequence" },
    { 0x0008, 0x2258, (char *) "ST", (char *) "Anatomic Location Of Examining Instrument Description" },
    { 0x0008, 0x2259, (char *) "SQ", (char *) "Anatomic Location Of Examining Instrument Code Sequence" },
    { 0x0008, 0x225a, (char *) "SQ", (char *) "Anatomic Structure Space Or Region Modifier Code Sequence" },
    { 0x0008, 0x225c, (char *) "SQ", (char *) "OnAxis Background Anatomic Structure Code Sequence" },
    { 0x0008, 0x4000, (char *) "LT", (char *) "Identifying Comments" },
    { 0x0009, 0x0000, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0001, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0002, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0003, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0004, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0005, (char *) "UN", (char *) "?" },
    { 0x0009, 0x0006, (char *) "UN", (char *) "?" },
    { 0x0009, 0x0007, (char *) "UN", (char *) "?" },
    { 0x0009, 0x0008, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0009, (char *) "LT", (char *) "?" },
    { 0x0009, 0x000a, (char *) "IS", (char *) "?" },
    { 0x0009, 0x000b, (char *) "IS", (char *) "?" },
    { 0x0009, 0x000c, (char *) "IS", (char *) "?" },
    { 0x0009, 0x000d, (char *) "IS", (char *) "?" },
    { 0x0009, 0x000e, (char *) "IS", (char *) "?" },
    { 0x0009, 0x000f, (char *) "UN", (char *) "?" },
    { 0x0009, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0011, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0012, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0013, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0014, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0015, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0016, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0017, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0018, (char *) "LT", (char *) "Data Set Identifier" },
    { 0x0009, 0x001a, (char *) "US", (char *) "?" },
    { 0x0009, 0x001e, (char *) "UI", (char *) "?" },
    { 0x0009, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0021, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0022, (char *) "SH", (char *) "User Orientation" },
    { 0x0009, 0x0023, (char *) "SL", (char *) "Initiation Type" },
    { 0x0009, 0x0024, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0025, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0026, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0027, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0029, (char *) "xs", (char *) "?" },
    { 0x0009, 0x002a, (char *) "SL", (char *) "?" },
    { 0x0009, 0x002c, (char *) "LO", (char *) "Series Comments" },
    { 0x0009, 0x002d, (char *) "SL", (char *) "Track Beat Average" },
    { 0x0009, 0x002e, (char *) "FD", (char *) "Distance Prescribed" },
    { 0x0009, 0x002f, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0031, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0032, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0034, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0035, (char *) "SL", (char *) "Gantry Locus Type" },
    { 0x0009, 0x0037, (char *) "SL", (char *) "Starting Heart Rate" },
    { 0x0009, 0x0038, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0039, (char *) "SL", (char *) "RR Window Offset" },
    { 0x0009, 0x003a, (char *) "SL", (char *) "Percent Cycle Imaged" },
    { 0x0009, 0x003e, (char *) "US", (char *) "?" },
    { 0x0009, 0x003f, (char *) "US", (char *) "?" },
    { 0x0009, 0x0040, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0041, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0042, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0043, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0050, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0051, (char *) "xs", (char *) "?" },
    { 0x0009, 0x0060, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0061, (char *) "LT", (char *) "Series Unique Identifier" },
    { 0x0009, 0x0070, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0080, (char *) "LT", (char *) "?" },
    { 0x0009, 0x0091, (char *) "LT", (char *) "?" },
    { 0x0009, 0x00e2, (char *) "LT", (char *) "?" },
    { 0x0009, 0x00e3, (char *) "UI", (char *) "Equipment UID" },
    { 0x0009, 0x00e6, (char *) "SH", (char *) "Genesis Version Now" },
    { 0x0009, 0x00e7, (char *) "UL", (char *) "Exam Record Checksum" },
    { 0x0009, 0x00e8, (char *) "UL", (char *) "?" },
    { 0x0009, 0x00e9, (char *) "SL", (char *) "Actual Series Data Time Stamp" },
    { 0x0009, 0x00f2, (char *) "UN", (char *) "?" },
    { 0x0009, 0x00f3, (char *) "UN", (char *) "?" },
    { 0x0009, 0x00f4, (char *) "LT", (char *) "?" },
    { 0x0009, 0x00f5, (char *) "xs", (char *) "?" },
    { 0x0009, 0x00f6, (char *) "LT", (char *) "PDM Data Object Type Extension" },
    { 0x0009, 0x00f8, (char *) "US", (char *) "?" },
    { 0x0009, 0x00fb, (char *) "IS", (char *) "?" },
    { 0x0009, 0x1002, (char *) "OB", (char *) "?" },
    { 0x0009, 0x1003, (char *) "OB", (char *) "?" },
    { 0x0009, 0x1010, (char *) "UN", (char *) "?" },
    { 0x0010, 0x0000, (char *) "UL", (char *) "Patient Group Length" },
    { 0x0010, 0x0010, (char *) "PN", (char *) "Patient's Name" },
    { 0x0010, 0x0020, (char *) "LO", (char *) "Patient's ID" },
    { 0x0010, 0x0021, (char *) "LO", (char *) "Issuer of Patient's ID" },
    { 0x0010, 0x0030, (char *) "DA", (char *) "Patient's Birth Date" },
    { 0x0010, 0x0032, (char *) "TM", (char *) "Patient's Birth Time" },
    { 0x0010, 0x0040, (char *) "CS", (char *) "Patient's Sex" },
    { 0x0010, 0x0050, (char *) "SQ", (char *) "Patient's Insurance Plan Code Sequence" },
    { 0x0010, 0x1000, (char *) "LO", (char *) "Other Patient's ID's" },
    { 0x0010, 0x1001, (char *) "PN", (char *) "Other Patient's Names" },
    { 0x0010, 0x1005, (char *) "PN", (char *) "Patient's Birth Name" },
    { 0x0010, 0x1010, (char *) "AS", (char *) "Patient's Age" },
    { 0x0010, 0x1020, (char *) "DS", (char *) "Patient's Size" },
    { 0x0010, 0x1030, (char *) "DS", (char *) "Patient's Weight" },
    { 0x0010, 0x1040, (char *) "LO", (char *) "Patient's Address" },
    { 0x0010, 0x1050, (char *) "LT", (char *) "Insurance Plan Identification" },
    { 0x0010, 0x1060, (char *) "PN", (char *) "Patient's Mother's Birth Name" },
    { 0x0010, 0x1080, (char *) "LO", (char *) "Military Rank" },
    { 0x0010, 0x1081, (char *) "LO", (char *) "Branch of Service" },
    { 0x0010, 0x1090, (char *) "LO", (char *) "Medical Record Locator" },
    { 0x0010, 0x2000, (char *) "LO", (char *) "Medical Alerts" },
    { 0x0010, 0x2110, (char *) "LO", (char *) "Contrast Allergies" },
    { 0x0010, 0x2150, (char *) "LO", (char *) "Country of Residence" },
    { 0x0010, 0x2152, (char *) "LO", (char *) "Region of Residence" },
    { 0x0010, 0x2154, (char *) "SH", (char *) "Patients Telephone Numbers" },
    { 0x0010, 0x2160, (char *) "SH", (char *) "Ethnic Group" },
    { 0x0010, 0x2180, (char *) "SH", (char *) "Occupation" },
    { 0x0010, 0x21a0, (char *) "CS", (char *) "Smoking Status" },
    { 0x0010, 0x21b0, (char *) "LT", (char *) "Additional Patient History" },
    { 0x0010, 0x21c0, (char *) "US", (char *) "Pregnancy Status" },
    { 0x0010, 0x21d0, (char *) "DA", (char *) "Last Menstrual Date" },
    { 0x0010, 0x21f0, (char *) "LO", (char *) "Patients Religious Preference" },
    { 0x0010, 0x4000, (char *) "LT", (char *) "Patient Comments" },
    { 0x0011, 0x0001, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0002, (char *) "US", (char *) "?" },
    { 0x0011, 0x0003, (char *) "LT", (char *) "Patient UID" },
    { 0x0011, 0x0004, (char *) "LT", (char *) "Patient ID" },
    { 0x0011, 0x000a, (char *) "xs", (char *) "?" },
    { 0x0011, 0x000b, (char *) "SL", (char *) "Effective Series Duration" },
    { 0x0011, 0x000c, (char *) "SL", (char *) "Num Beats" },
    { 0x0011, 0x000d, (char *) "LO", (char *) "Radio Nuclide Name" },
    { 0x0011, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0011, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0012, (char *) "LO", (char *) "Dataset Name" },
    { 0x0011, 0x0013, (char *) "LO", (char *) "Dataset Type" },
    { 0x0011, 0x0015, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0016, (char *) "SL", (char *) "Energy Number" },
    { 0x0011, 0x0017, (char *) "SL", (char *) "RR Interval Window Number" },
    { 0x0011, 0x0018, (char *) "SL", (char *) "MG Bin Number" },
    { 0x0011, 0x0019, (char *) "FD", (char *) "Radius Of Rotation" },
    { 0x0011, 0x001a, (char *) "SL", (char *) "Detector Count Zone" },
    { 0x0011, 0x001b, (char *) "SL", (char *) "Num Energy Windows" },
    { 0x0011, 0x001c, (char *) "SL", (char *) "Energy Offset" },
    { 0x0011, 0x001d, (char *) "SL", (char *) "Energy Range" },
    { 0x0011, 0x001f, (char *) "SL", (char *) "Image Orientation" },
    { 0x0011, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0021, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0022, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0023, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0024, (char *) "SL", (char *) "FOV Mask Y Cutoff Angle" },
    { 0x0011, 0x0025, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0026, (char *) "SL", (char *) "Table Orientation" },
    { 0x0011, 0x0027, (char *) "SL", (char *) "ROI Top Left" },
    { 0x0011, 0x0028, (char *) "SL", (char *) "ROI Bottom Right" },
    { 0x0011, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0031, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0032, (char *) "UN", (char *) "?" },
    { 0x0011, 0x0033, (char *) "LO", (char *) "Energy Correct Name" },
    { 0x0011, 0x0034, (char *) "LO", (char *) "Spatial Correct Name" },
    { 0x0011, 0x0035, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0036, (char *) "LO", (char *) "Uniformity Correct Name" },
    { 0x0011, 0x0037, (char *) "LO", (char *) "Acquisition Specific Correct Name" },
    { 0x0011, 0x0038, (char *) "SL", (char *) "Byte Order" },
    { 0x0011, 0x003a, (char *) "SL", (char *) "Picture Format" },
    { 0x0011, 0x003b, (char *) "FD", (char *) "Pixel Scale" },
    { 0x0011, 0x003c, (char *) "FD", (char *) "Pixel Offset" },
    { 0x0011, 0x003e, (char *) "SL", (char *) "FOV Shape" },
    { 0x0011, 0x003f, (char *) "SL", (char *) "Dataset Flags" },
    { 0x0011, 0x0040, (char *) "xs", (char *) "?" },
    { 0x0011, 0x0041, (char *) "LT", (char *) "Medical Alerts" },
    { 0x0011, 0x0042, (char *) "LT", (char *) "Contrast Allergies" },
    { 0x0011, 0x0044, (char *) "FD", (char *) "Threshold Center" },
    { 0x0011, 0x0045, (char *) "FD", (char *) "Threshold Width" },
    { 0x0011, 0x0046, (char *) "SL", (char *) "Interpolation Type" },
    { 0x0011, 0x0055, (char *) "FD", (char *) "Period" },
    { 0x0011, 0x0056, (char *) "FD", (char *) "ElapsedTime" },
    { 0x0011, 0x00a1, (char *) "DA", (char *) "Patient Registration Date" },
    { 0x0011, 0x00a2, (char *) "TM", (char *) "Patient Registration Time" },
    { 0x0011, 0x00b0, (char *) "LT", (char *) "Patient Last Name" },
    { 0x0011, 0x00b2, (char *) "LT", (char *) "Patient First Name" },
    { 0x0011, 0x00b4, (char *) "LT", (char *) "Patient Hospital Status" },
    { 0x0011, 0x00bc, (char *) "TM", (char *) "Current Location Time" },
    { 0x0011, 0x00c0, (char *) "LT", (char *) "Patient Insurance Status" },
    { 0x0011, 0x00d0, (char *) "LT", (char *) "Patient Billing Type" },
    { 0x0011, 0x00d2, (char *) "LT", (char *) "Patient Billing Address" },
    { 0x0013, 0x0000, (char *) "LT", (char *) "Modifying Physician" },
    { 0x0013, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0013, 0x0011, (char *) "SL", (char *) "?" },
    { 0x0013, 0x0012, (char *) "xs", (char *) "?" },
    { 0x0013, 0x0016, (char *) "SL", (char *) "AutoTrack Peak" },
    { 0x0013, 0x0017, (char *) "SL", (char *) "AutoTrack Width" },
    { 0x0013, 0x0018, (char *) "FD", (char *) "Transmission Scan Time" },
    { 0x0013, 0x0019, (char *) "FD", (char *) "Transmission Mask Width" },
    { 0x0013, 0x001a, (char *) "FD", (char *) "Copper Attenuator Thickness" },
    { 0x0013, 0x001c, (char *) "FD", (char *) "?" },
    { 0x0013, 0x001d, (char *) "FD", (char *) "?" },
    { 0x0013, 0x001e, (char *) "FD", (char *) "Tomo View Offset" },
    { 0x0013, 0x0020, (char *) "LT", (char *) "Patient Name" },
    { 0x0013, 0x0022, (char *) "LT", (char *) "Patient Id" },
    { 0x0013, 0x0026, (char *) "LT", (char *) "Study Comments" },
    { 0x0013, 0x0030, (char *) "DA", (char *) "Patient Birthdate" },
    { 0x0013, 0x0031, (char *) "DS", (char *) "Patient Weight" },
    { 0x0013, 0x0032, (char *) "LT", (char *) "Patients Maiden Name" },
    { 0x0013, 0x0033, (char *) "LT", (char *) "Referring Physician" },
    { 0x0013, 0x0034, (char *) "LT", (char *) "Admitting Diagnosis" },
    { 0x0013, 0x0035, (char *) "LT", (char *) "Patient Sex" },
    { 0x0013, 0x0040, (char *) "LT", (char *) "Procedure Description" },
    { 0x0013, 0x0042, (char *) "LT", (char *) "Patient Rest Direction" },
    { 0x0013, 0x0044, (char *) "LT", (char *) "Patient Position" },
    { 0x0013, 0x0046, (char *) "LT", (char *) "View Direction" },
    { 0x0015, 0x0001, (char *) "DS", (char *) "Stenosis Calibration Ratio" },
    { 0x0015, 0x0002, (char *) "DS", (char *) "Stenosis Magnification" },
    { 0x0015, 0x0003, (char *) "DS", (char *) "Cardiac Calibration Ratio" },
    { 0x0018, 0x0000, (char *) "UL", (char *) "Acquisition Group Length" },
    { 0x0018, 0x0010, (char *) "LO", (char *) "Contrast/Bolus Agent" },
    { 0x0018, 0x0012, (char *) "SQ", (char *) "Contrast/Bolus Agent Sequence" },
    { 0x0018, 0x0014, (char *) "SQ", (char *) "Contrast/Bolus Administration Route Sequence" },
    { 0x0018, 0x0015, (char *) "CS", (char *) "Body Part Examined" },
    { 0x0018, 0x0020, (char *) "CS", (char *) "Scanning Sequence" },
    { 0x0018, 0x0021, (char *) "CS", (char *) "Sequence Variant" },
    { 0x0018, 0x0022, (char *) "CS", (char *) "Scan Options" },
    { 0x0018, 0x0023, (char *) "CS", (char *) "MR Acquisition Type" },
    { 0x0018, 0x0024, (char *) "SH", (char *) "Sequence Name" },
    { 0x0018, 0x0025, (char *) "CS", (char *) "Angio Flag" },
    { 0x0018, 0x0026, (char *) "SQ", (char *) "Intervention Drug Information Sequence" },
    { 0x0018, 0x0027, (char *) "TM", (char *) "Intervention Drug Stop Time" },
    { 0x0018, 0x0028, (char *) "DS", (char *) "Intervention Drug Dose" },
    { 0x0018, 0x0029, (char *) "SQ", (char *) "Intervention Drug Code Sequence" },
    { 0x0018, 0x002a, (char *) "SQ", (char *) "Additional Drug Sequence" },
    { 0x0018, 0x0030, (char *) "LO", (char *) "Radionuclide" },
    { 0x0018, 0x0031, (char *) "LO", (char *) "Radiopharmaceutical" },
    { 0x0018, 0x0032, (char *) "DS", (char *) "Energy Window Centerline" },
    { 0x0018, 0x0033, (char *) "DS", (char *) "Energy Window Total Width" },
    { 0x0018, 0x0034, (char *) "LO", (char *) "Intervention Drug Name" },
    { 0x0018, 0x0035, (char *) "TM", (char *) "Intervention Drug Start Time" },
    { 0x0018, 0x0036, (char *) "SQ", (char *) "Intervention Therapy Sequence" },
    { 0x0018, 0x0037, (char *) "CS", (char *) "Therapy Type" },
    { 0x0018, 0x0038, (char *) "CS", (char *) "Intervention Status" },
    { 0x0018, 0x0039, (char *) "CS", (char *) "Therapy Description" },
    { 0x0018, 0x0040, (char *) "IS", (char *) "Cine Rate" },
    { 0x0018, 0x0050, (char *) "DS", (char *) "Slice Thickness" },
    { 0x0018, 0x0060, (char *) "DS", (char *) "KVP" },
    { 0x0018, 0x0070, (char *) "IS", (char *) "Counts Accumulated" },
    { 0x0018, 0x0071, (char *) "CS", (char *) "Acquisition Termination Condition" },
    { 0x0018, 0x0072, (char *) "DS", (char *) "Effective Series Duration" },
    { 0x0018, 0x0073, (char *) "CS", (char *) "Acquisition Start Condition" },
    { 0x0018, 0x0074, (char *) "IS", (char *) "Acquisition Start Condition Data" },
    { 0x0018, 0x0075, (char *) "IS", (char *) "Acquisition Termination Condition Data" },
    { 0x0018, 0x0080, (char *) "DS", (char *) "Repetition Time" },
    { 0x0018, 0x0081, (char *) "DS", (char *) "Echo Time" },
    { 0x0018, 0x0082, (char *) "DS", (char *) "Inversion Time" },
    { 0x0018, 0x0083, (char *) "DS", (char *) "Number of Averages" },
    { 0x0018, 0x0084, (char *) "DS", (char *) "Imaging Frequency" },
    { 0x0018, 0x0085, (char *) "SH", (char *) "Imaged Nucleus" },
    { 0x0018, 0x0086, (char *) "IS", (char *) "Echo Number(s)" },
    { 0x0018, 0x0087, (char *) "DS", (char *) "Magnetic Field Strength" },
    { 0x0018, 0x0088, (char *) "DS", (char *) "Spacing Between Slices" },
    { 0x0018, 0x0089, (char *) "IS", (char *) "Number of Phase Encoding Steps" },
    { 0x0018, 0x0090, (char *) "DS", (char *) "Data Collection Diameter" },
    { 0x0018, 0x0091, (char *) "IS", (char *) "Echo Train Length" },
    { 0x0018, 0x0093, (char *) "DS", (char *) "Percent Sampling" },
    { 0x0018, 0x0094, (char *) "DS", (char *) "Percent Phase Field of View" },
    { 0x0018, 0x0095, (char *) "DS", (char *) "Pixel Bandwidth" },
    { 0x0018, 0x1000, (char *) "LO", (char *) "Device Serial Number" },
    { 0x0018, 0x1004, (char *) "LO", (char *) "Plate ID" },
    { 0x0018, 0x1010, (char *) "LO", (char *) "Secondary Capture Device ID" },
    { 0x0018, 0x1012, (char *) "DA", (char *) "Date of Secondary Capture" },
    { 0x0018, 0x1014, (char *) "TM", (char *) "Time of Secondary Capture" },
    { 0x0018, 0x1016, (char *) "LO", (char *) "Secondary Capture Device Manufacturer" },
    { 0x0018, 0x1018, (char *) "LO", (char *) "Secondary Capture Device Manufacturer Model Name" },
    { 0x0018, 0x1019, (char *) "LO", (char *) "Secondary Capture Device Software Version(s)" },
    { 0x0018, 0x1020, (char *) "LO", (char *) "Software Version(s)" },
    { 0x0018, 0x1022, (char *) "SH", (char *) "Video Image Format Acquired" },
    { 0x0018, 0x1023, (char *) "LO", (char *) "Digital Image Format Acquired" },
    { 0x0018, 0x1030, (char *) "LO", (char *) "Protocol Name" },
    { 0x0018, 0x1040, (char *) "LO", (char *) "Contrast/Bolus Route" },
    { 0x0018, 0x1041, (char *) "DS", (char *) "Contrast/Bolus Volume" },
    { 0x0018, 0x1042, (char *) "TM", (char *) "Contrast/Bolus Start Time" },
    { 0x0018, 0x1043, (char *) "TM", (char *) "Contrast/Bolus Stop Time" },
    { 0x0018, 0x1044, (char *) "DS", (char *) "Contrast/Bolus Total Dose" },
    { 0x0018, 0x1045, (char *) "IS", (char *) "Syringe Counts" },
    { 0x0018, 0x1046, (char *) "DS", (char *) "Contrast Flow Rate" },
    { 0x0018, 0x1047, (char *) "DS", (char *) "Contrast Flow Duration" },
    { 0x0018, 0x1048, (char *) "CS", (char *) "Contrast/Bolus Ingredient" },
    { 0x0018, 0x1049, (char *) "DS", (char *) "Contrast/Bolus Ingredient Concentration" },
    { 0x0018, 0x1050, (char *) "DS", (char *) "Spatial Resolution" },
    { 0x0018, 0x1060, (char *) "DS", (char *) "Trigger Time" },
    { 0x0018, 0x1061, (char *) "LO", (char *) "Trigger Source or Type" },
    { 0x0018, 0x1062, (char *) "IS", (char *) "Nominal Interval" },
    { 0x0018, 0x1063, (char *) "DS", (char *) "Frame Time" },
    { 0x0018, 0x1064, (char *) "LO", (char *) "Framing Type" },
    { 0x0018, 0x1065, (char *) "DS", (char *) "Frame Time Vector" },
    { 0x0018, 0x1066, (char *) "DS", (char *) "Frame Delay" },
    { 0x0018, 0x1067, (char *) "DS", (char *) "Image Trigger Delay" },
    { 0x0018, 0x1068, (char *) "DS", (char *) "Group Time Offset" },
    { 0x0018, 0x1069, (char *) "DS", (char *) "Trigger Time Offset" },
    { 0x0018, 0x106a, (char *) "CS", (char *) "Synchronization Trigger" },
    { 0x0018, 0x106b, (char *) "UI", (char *) "Synchronization Frame of Reference" },
    { 0x0018, 0x106e, (char *) "UL", (char *) "Trigger Sample Position" },
    { 0x0018, 0x1070, (char *) "LO", (char *) "Radiopharmaceutical Route" },
    { 0x0018, 0x1071, (char *) "DS", (char *) "Radiopharmaceutical Volume" },
    { 0x0018, 0x1072, (char *) "TM", (char *) "Radiopharmaceutical Start Time" },
    { 0x0018, 0x1073, (char *) "TM", (char *) "Radiopharmaceutical Stop Time" },
    { 0x0018, 0x1074, (char *) "DS", (char *) "Radionuclide Total Dose" },
    { 0x0018, 0x1075, (char *) "DS", (char *) "Radionuclide Half Life" },
    { 0x0018, 0x1076, (char *) "DS", (char *) "Radionuclide Positron Fraction" },
    { 0x0018, 0x1077, (char *) "DS", (char *) "Radiopharmaceutical Specific Activity" },
    { 0x0018, 0x1080, (char *) "CS", (char *) "Beat Rejection Flag" },
    { 0x0018, 0x1081, (char *) "IS", (char *) "Low R-R Value" },
    { 0x0018, 0x1082, (char *) "IS", (char *) "High R-R Value" },
    { 0x0018, 0x1083, (char *) "IS", (char *) "Intervals Acquired" },
    { 0x0018, 0x1084, (char *) "IS", (char *) "Intervals Rejected" },
    { 0x0018, 0x1085, (char *) "LO", (char *) "PVC Rejection" },
    { 0x0018, 0x1086, (char *) "IS", (char *) "Skip Beats" },
    { 0x0018, 0x1088, (char *) "IS", (char *) "Heart Rate" },
    { 0x0018, 0x1090, (char *) "IS", (char *) "Cardiac Number of Images" },
    { 0x0018, 0x1094, (char *) "IS", (char *) "Trigger Window" },
    { 0x0018, 0x1100, (char *) "DS", (char *) "Reconstruction Diameter" },
    { 0x0018, 0x1110, (char *) "DS", (char *) "Distance Source to Detector" },
    { 0x0018, 0x1111, (char *) "DS", (char *) "Distance Source to Patient" },
    { 0x0018, 0x1114, (char *) "DS", (char *) "Estimated Radiographic Magnification Factor" },
    { 0x0018, 0x1120, (char *) "DS", (char *) "Gantry/Detector Tilt" },
    { 0x0018, 0x1121, (char *) "DS", (char *) "Gantry/Detector Slew" },
    { 0x0018, 0x1130, (char *) "DS", (char *) "Table Height" },
    { 0x0018, 0x1131, (char *) "DS", (char *) "Table Traverse" },
    { 0x0018, 0x1134, (char *) "CS", (char *) "Table Motion" },
    { 0x0018, 0x1135, (char *) "DS", (char *) "Table Vertical Increment" },
    { 0x0018, 0x1136, (char *) "DS", (char *) "Table Lateral Increment" },
    { 0x0018, 0x1137, (char *) "DS", (char *) "Table Longitudinal Increment" },
    { 0x0018, 0x1138, (char *) "DS", (char *) "Table Angle" },
    { 0x0018, 0x113a, (char *) "CS", (char *) "Table Type" },
    { 0x0018, 0x1140, (char *) "CS", (char *) "Rotation Direction" },
    { 0x0018, 0x1141, (char *) "DS", (char *) "Angular Position" },
    { 0x0018, 0x1142, (char *) "DS", (char *) "Radial Position" },
    { 0x0018, 0x1143, (char *) "DS", (char *) "Scan Arc" },
    { 0x0018, 0x1144, (char *) "DS", (char *) "Angular Step" },
    { 0x0018, 0x1145, (char *) "DS", (char *) "Center of Rotation Offset" },
    { 0x0018, 0x1146, (char *) "DS", (char *) "Rotation Offset" },
    { 0x0018, 0x1147, (char *) "CS", (char *) "Field of View Shape" },
    { 0x0018, 0x1149, (char *) "IS", (char *) "Field of View Dimension(s)" },
    { 0x0018, 0x1150, (char *) "IS", (char *) "Exposure Time" },
    { 0x0018, 0x1151, (char *) "IS", (char *) "X-ray Tube Current" },
    { 0x0018, 0x1152, (char *) "IS", (char *) "Exposure" },
    { 0x0018, 0x1153, (char *) "IS", (char *) "Exposure in uAs" },
    { 0x0018, 0x1154, (char *) "DS", (char *) "AveragePulseWidth" },
    { 0x0018, 0x1155, (char *) "CS", (char *) "RadiationSetting" },
    { 0x0018, 0x1156, (char *) "CS", (char *) "Rectification Type" },
    { 0x0018, 0x115a, (char *) "CS", (char *) "RadiationMode" },
    { 0x0018, 0x115e, (char *) "DS", (char *) "ImageAreaDoseProduct" },
    { 0x0018, 0x1160, (char *) "SH", (char *) "Filter Type" },
    { 0x0018, 0x1161, (char *) "LO", (char *) "TypeOfFilters" },
    { 0x0018, 0x1162, (char *) "DS", (char *) "IntensifierSize" },
    { 0x0018, 0x1164, (char *) "DS", (char *) "ImagerPixelSpacing" },
    { 0x0018, 0x1166, (char *) "CS", (char *) "Grid" },
    { 0x0018, 0x1170, (char *) "IS", (char *) "Generator Power" },
    { 0x0018, 0x1180, (char *) "SH", (char *) "Collimator/Grid Name" },
    { 0x0018, 0x1181, (char *) "CS", (char *) "Collimator Type" },
    { 0x0018, 0x1182, (char *) "IS", (char *) "Focal Distance" },
    { 0x0018, 0x1183, (char *) "DS", (char *) "X Focus Center" },
    { 0x0018, 0x1184, (char *) "DS", (char *) "Y Focus Center" },
    { 0x0018, 0x1190, (char *) "DS", (char *) "Focal Spot(s)" },
    { 0x0018, 0x1191, (char *) "CS", (char *) "Anode Target Material" },
    { 0x0018, 0x11a0, (char *) "DS", (char *) "Body Part Thickness" },
    { 0x0018, 0x11a2, (char *) "DS", (char *) "Compression Force" },
    { 0x0018, 0x1200, (char *) "DA", (char *) "Date of Last Calibration" },
    { 0x0018, 0x1201, (char *) "TM", (char *) "Time of Last Calibration" },
    { 0x0018, 0x1210, (char *) "SH", (char *) "Convolution Kernel" },
    { 0x0018, 0x1240, (char *) "IS", (char *) "Upper/Lower Pixel Values" },
    { 0x0018, 0x1242, (char *) "IS", (char *) "Actual Frame Duration" },
    { 0x0018, 0x1243, (char *) "IS", (char *) "Count Rate" },
    { 0x0018, 0x1244, (char *) "US", (char *) "Preferred Playback Sequencing" },
    { 0x0018, 0x1250, (char *) "SH", (char *) "Receiving Coil" },
    { 0x0018, 0x1251, (char *) "SH", (char *) "Transmitting Coil" },
    { 0x0018, 0x1260, (char *) "SH", (char *) "Plate Type" },
    { 0x0018, 0x1261, (char *) "LO", (char *) "Phosphor Type" },
    { 0x0018, 0x1300, (char *) "DS", (char *) "Scan Velocity" },
    { 0x0018, 0x1301, (char *) "CS", (char *) "Whole Body Technique" },
    { 0x0018, 0x1302, (char *) "IS", (char *) "Scan Length" },
    { 0x0018, 0x1310, (char *) "US", (char *) "Acquisition Matrix" },
    { 0x0018, 0x1312, (char *) "CS", (char *) "Phase Encoding Direction" },
    { 0x0018, 0x1314, (char *) "DS", (char *) "Flip Angle" },
    { 0x0018, 0x1315, (char *) "CS", (char *) "Variable Flip Angle Flag" },
    { 0x0018, 0x1316, (char *) "DS", (char *) "SAR" },
    { 0x0018, 0x1318, (char *) "DS", (char *) "dB/dt" },
    { 0x0018, 0x1400, (char *) "LO", (char *) "Acquisition Device Processing Description" },
    { 0x0018, 0x1401, (char *) "LO", (char *) "Acquisition Device Processing Code" },
    { 0x0018, 0x1402, (char *) "CS", (char *) "Cassette Orientation" },
    { 0x0018, 0x1403, (char *) "CS", (char *) "Cassette Size" },
    { 0x0018, 0x1404, (char *) "US", (char *) "Exposures on Plate" },
    { 0x0018, 0x1405, (char *) "IS", (char *) "Relative X-ray Exposure" },
    { 0x0018, 0x1450, (char *) "DS", (char *) "Column Angulation" },
    { 0x0018, 0x1460, (char *) "DS", (char *) "Tomo Layer Height" },
    { 0x0018, 0x1470, (char *) "DS", (char *) "Tomo Angle" },
    { 0x0018, 0x1480, (char *) "DS", (char *) "Tomo Time" },
    { 0x0018, 0x1490, (char *) "CS", (char *) "Tomo Type" },
    { 0x0018, 0x1491, (char *) "CS", (char *) "Tomo Class" },
    { 0x0018, 0x1495, (char *) "IS", (char *) "Number of Tomosynthesis Source Images" },
    { 0x0018, 0x1500, (char *) "CS", (char *) "PositionerMotion" },
    { 0x0018, 0x1508, (char *) "CS", (char *) "Positioner Type" },
    { 0x0018, 0x1510, (char *) "DS", (char *) "PositionerPrimaryAngle" },
    { 0x0018, 0x1511, (char *) "DS", (char *) "PositionerSecondaryAngle" },
    { 0x0018, 0x1520, (char *) "DS", (char *) "PositionerPrimaryAngleIncrement" },
    { 0x0018, 0x1521, (char *) "DS", (char *) "PositionerSecondaryAngleIncrement" },
    { 0x0018, 0x1530, (char *) "DS", (char *) "DetectorPrimaryAngle" },
    { 0x0018, 0x1531, (char *) "DS", (char *) "DetectorSecondaryAngle" },
    { 0x0018, 0x1600, (char *) "CS", (char *) "Shutter Shape" },
    { 0x0018, 0x1602, (char *) "IS", (char *) "Shutter Left Vertical Edge" },
    { 0x0018, 0x1604, (char *) "IS", (char *) "Shutter Right Vertical Edge" },
    { 0x0018, 0x1606, (char *) "IS", (char *) "Shutter Upper Horizontal Edge" },
    { 0x0018, 0x1608, (char *) "IS", (char *) "Shutter Lower Horizonta lEdge" },
    { 0x0018, 0x1610, (char *) "IS", (char *) "Center of Circular Shutter" },
    { 0x0018, 0x1612, (char *) "IS", (char *) "Radius of Circular Shutter" },
    { 0x0018, 0x1620, (char *) "IS", (char *) "Vertices of Polygonal Shutter" },
    { 0x0018, 0x1622, (char *) "US", (char *) "Shutter Presentation Value" },
    { 0x0018, 0x1623, (char *) "US", (char *) "Shutter Overlay Group" },
    { 0x0018, 0x1700, (char *) "CS", (char *) "Collimator Shape" },
    { 0x0018, 0x1702, (char *) "IS", (char *) "Collimator Left Vertical Edge" },
    { 0x0018, 0x1704, (char *) "IS", (char *) "Collimator Right Vertical Edge" },
    { 0x0018, 0x1706, (char *) "IS", (char *) "Collimator Upper Horizontal Edge" },
    { 0x0018, 0x1708, (char *) "IS", (char *) "Collimator Lower Horizontal Edge" },
    { 0x0018, 0x1710, (char *) "IS", (char *) "Center of Circular Collimator" },
    { 0x0018, 0x1712, (char *) "IS", (char *) "Radius of Circular Collimator" },
    { 0x0018, 0x1720, (char *) "IS", (char *) "Vertices of Polygonal Collimator" },
    { 0x0018, 0x1800, (char *) "CS", (char *) "Acquisition Time Synchronized" },
    { 0x0018, 0x1801, (char *) "SH", (char *) "Time Source" },
    { 0x0018, 0x1802, (char *) "CS", (char *) "Time Distribution Protocol" },
    { 0x0018, 0x4000, (char *) "LT", (char *) "Acquisition Comments" },
    { 0x0018, 0x5000, (char *) "SH", (char *) "Output Power" },
    { 0x0018, 0x5010, (char *) "LO", (char *) "Transducer Data" },
    { 0x0018, 0x5012, (char *) "DS", (char *) "Focus Depth" },
    { 0x0018, 0x5020, (char *) "LO", (char *) "Processing Function" },
    { 0x0018, 0x5021, (char *) "LO", (char *) "Postprocessing Function" },
    { 0x0018, 0x5022, (char *) "DS", (char *) "Mechanical Index" },
    { 0x0018, 0x5024, (char *) "DS", (char *) "Thermal Index" },
    { 0x0018, 0x5026, (char *) "DS", (char *) "Cranial Thermal Index" },
    { 0x0018, 0x5027, (char *) "DS", (char *) "Soft Tissue Thermal Index" },
    { 0x0018, 0x5028, (char *) "DS", (char *) "Soft Tissue-Focus Thermal Index" },
    { 0x0018, 0x5029, (char *) "DS", (char *) "Soft Tissue-Surface Thermal Index" },
    { 0x0018, 0x5030, (char *) "DS", (char *) "Dynamic Range" },
    { 0x0018, 0x5040, (char *) "DS", (char *) "Total Gain" },
    { 0x0018, 0x5050, (char *) "IS", (char *) "Depth of Scan Field" },
    { 0x0018, 0x5100, (char *) "CS", (char *) "Patient Position" },
    { 0x0018, 0x5101, (char *) "CS", (char *) "View Position" },
    { 0x0018, 0x5104, (char *) "SQ", (char *) "Projection Eponymous Name Code Sequence" },
    { 0x0018, 0x5210, (char *) "DS", (char *) "Image Transformation Matrix" },
    { 0x0018, 0x5212, (char *) "DS", (char *) "Image Translation Vector" },
    { 0x0018, 0x6000, (char *) "DS", (char *) "Sensitivity" },
    { 0x0018, 0x6011, (char *) "IS", (char *) "Sequence of Ultrasound Regions" },
    { 0x0018, 0x6012, (char *) "US", (char *) "Region Spatial Format" },
    { 0x0018, 0x6014, (char *) "US", (char *) "Region Data Type" },
    { 0x0018, 0x6016, (char *) "UL", (char *) "Region Flags" },
    { 0x0018, 0x6018, (char *) "UL", (char *) "Region Location Min X0" },
    { 0x0018, 0x601a, (char *) "UL", (char *) "Region Location Min Y0" },
    { 0x0018, 0x601c, (char *) "UL", (char *) "Region Location Max X1" },
    { 0x0018, 0x601e, (char *) "UL", (char *) "Region Location Max Y1" },
    { 0x0018, 0x6020, (char *) "SL", (char *) "Reference Pixel X0" },
    { 0x0018, 0x6022, (char *) "SL", (char *) "Reference Pixel Y0" },
    { 0x0018, 0x6024, (char *) "US", (char *) "Physical Units X Direction" },
    { 0x0018, 0x6026, (char *) "US", (char *) "Physical Units Y Direction" },
    { 0x0018, 0x6028, (char *) "FD", (char *) "Reference Pixel Physical Value X" },
    { 0x0018, 0x602a, (char *) "US", (char *) "Reference Pixel Physical Value Y" },
    { 0x0018, 0x602c, (char *) "US", (char *) "Physical Delta X" },
    { 0x0018, 0x602e, (char *) "US", (char *) "Physical Delta Y" },
    { 0x0018, 0x6030, (char *) "UL", (char *) "Transducer Frequency" },
    { 0x0018, 0x6031, (char *) "CS", (char *) "Transducer Type" },
    { 0x0018, 0x6032, (char *) "UL", (char *) "Pulse Repetition Frequency" },
    { 0x0018, 0x6034, (char *) "FD", (char *) "Doppler Correction Angle" },
    { 0x0018, 0x6036, (char *) "FD", (char *) "Steering Angle" },
    { 0x0018, 0x6038, (char *) "UL", (char *) "Doppler Sample Volume X Position" },
    { 0x0018, 0x603a, (char *) "UL", (char *) "Doppler Sample Volume Y Position" },
    { 0x0018, 0x603c, (char *) "UL", (char *) "TM-Line Position X0" },
    { 0x0018, 0x603e, (char *) "UL", (char *) "TM-Line Position Y0" },
    { 0x0018, 0x6040, (char *) "UL", (char *) "TM-Line Position X1" },
    { 0x0018, 0x6042, (char *) "UL", (char *) "TM-Line Position Y1" },
    { 0x0018, 0x6044, (char *) "US", (char *) "Pixel Component Organization" },
    { 0x0018, 0x6046, (char *) "UL", (char *) "Pixel Component Mask" },
    { 0x0018, 0x6048, (char *) "UL", (char *) "Pixel Component Range Start" },
    { 0x0018, 0x604a, (char *) "UL", (char *) "Pixel Component Range Stop" },
    { 0x0018, 0x604c, (char *) "US", (char *) "Pixel Component Physical Units" },
    { 0x0018, 0x604e, (char *) "US", (char *) "Pixel Component Data Type" },
    { 0x0018, 0x6050, (char *) "UL", (char *) "Number of Table Break Points" },
    { 0x0018, 0x6052, (char *) "UL", (char *) "Table of X Break Points" },
    { 0x0018, 0x6054, (char *) "FD", (char *) "Table of Y Break Points" },
    { 0x0018, 0x6056, (char *) "UL", (char *) "Number of Table Entries" },
    { 0x0018, 0x6058, (char *) "UL", (char *) "Table of Pixel Values" },
    { 0x0018, 0x605a, (char *) "FL", (char *) "Table of Parameter Values" },
    { 0x0018, 0x7000, (char *) "CS", (char *) "Detector Conditions Nominal Flag" },
    { 0x0018, 0x7001, (char *) "DS", (char *) "Detector Temperature" },
    { 0x0018, 0x7004, (char *) "CS", (char *) "Detector Type" },
    { 0x0018, 0x7005, (char *) "CS", (char *) "Detector Configuration" },
    { 0x0018, 0x7006, (char *) "LT", (char *) "Detector Description" },
    { 0x0018, 0x7008, (char *) "LT", (char *) "Detector Mode" },
    { 0x0018, 0x700a, (char *) "SH", (char *) "Detector ID" },
    { 0x0018, 0x700c, (char *) "DA", (char *) "Date of Last Detector Calibration " },
    { 0x0018, 0x700e, (char *) "TM", (char *) "Time of Last Detector Calibration" },
    { 0x0018, 0x7010, (char *) "IS", (char *) "Exposures on Detector Since Last Calibration" },
    { 0x0018, 0x7011, (char *) "IS", (char *) "Exposures on Detector Since Manufactured" },
    { 0x0018, 0x7012, (char *) "DS", (char *) "Detector Time Since Last Exposure" },
    { 0x0018, 0x7014, (char *) "DS", (char *) "Detector Active Time" },
    { 0x0018, 0x7016, (char *) "DS", (char *) "Detector Activation Offset From Exposure" },
    { 0x0018, 0x701a, (char *) "DS", (char *) "Detector Binning" },
    { 0x0018, 0x7020, (char *) "DS", (char *) "Detector Element Physical Size" },
    { 0x0018, 0x7022, (char *) "DS", (char *) "Detector Element Spacing" },
    { 0x0018, 0x7024, (char *) "CS", (char *) "Detector Active Shape" },
    { 0x0018, 0x7026, (char *) "DS", (char *) "Detector Active Dimensions" },
    { 0x0018, 0x7028, (char *) "DS", (char *) "Detector Active Origin" },
    { 0x0018, 0x7030, (char *) "DS", (char *) "Field of View Origin" },
    { 0x0018, 0x7032, (char *) "DS", (char *) "Field of View Rotation" },
    { 0x0018, 0x7034, (char *) "CS", (char *) "Field of View Horizontal Flip" },
    { 0x0018, 0x7040, (char *) "LT", (char *) "Grid Absorbing Material" },
    { 0x0018, 0x7041, (char *) "LT", (char *) "Grid Spacing Material" },
    { 0x0018, 0x7042, (char *) "DS", (char *) "Grid Thickness" },
    { 0x0018, 0x7044, (char *) "DS", (char *) "Grid Pitch" },
    { 0x0018, 0x7046, (char *) "IS", (char *) "Grid Aspect Ratio" },
    { 0x0018, 0x7048, (char *) "DS", (char *) "Grid Period" },
    { 0x0018, 0x704c, (char *) "DS", (char *) "Grid Focal Distance" },
    { 0x0018, 0x7050, (char *) "LT", (char *) "Filter Material" },
    { 0x0018, 0x7052, (char *) "DS", (char *) "Filter Thickness Minimum" },
    { 0x0018, 0x7054, (char *) "DS", (char *) "Filter Thickness Maximum" },
    { 0x0018, 0x7060, (char *) "CS", (char *) "Exposure Control Mode" },
    { 0x0018, 0x7062, (char *) "LT", (char *) "Exposure Control Mode Description" },
    { 0x0018, 0x7064, (char *) "CS", (char *) "Exposure Status" },
    { 0x0018, 0x7065, (char *) "DS", (char *) "Phototimer Setting" },
    { 0x0019, 0x0000, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0001, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0002, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0003, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0004, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0005, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0006, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0007, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0008, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0009, (char *) "xs", (char *) "?" },
    { 0x0019, 0x000a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x000b, (char *) "DS", (char *) "?" },
    { 0x0019, 0x000c, (char *) "US", (char *) "?" },
    { 0x0019, 0x000d, (char *) "TM", (char *) "Time" },
    { 0x0019, 0x000e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x000f, (char *) "DS", (char *) "Horizontal Frame Of Reference" },
    { 0x0019, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0011, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0012, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0013, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0014, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0015, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0016, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0017, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0018, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0019, (char *) "xs", (char *) "?" },
    { 0x0019, 0x001a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x001b, (char *) "xs", (char *) "?" },
    { 0x0019, 0x001c, (char *) "CS", (char *) "Dose" },
    { 0x0019, 0x001d, (char *) "IS", (char *) "Side Mark" },
    { 0x0019, 0x001e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x001f, (char *) "DS", (char *) "Exposure Duration" },
    { 0x0019, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0021, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0022, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0023, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0024, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0025, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0026, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0027, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0028, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0029, (char *) "IS", (char *) "?" },
    { 0x0019, 0x002a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x002b, (char *) "DS", (char *) "Xray Off Position" },
    { 0x0019, 0x002c, (char *) "xs", (char *) "?" },
    { 0x0019, 0x002d, (char *) "US", (char *) "?" },
    { 0x0019, 0x002e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x002f, (char *) "DS", (char *) "Trigger Frequency" },
    { 0x0019, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0031, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0032, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0033, (char *) "UN", (char *) "ECG 2 Offset 2" },
    { 0x0019, 0x0034, (char *) "US", (char *) "?" },
    { 0x0019, 0x0036, (char *) "US", (char *) "?" },
    { 0x0019, 0x0038, (char *) "US", (char *) "?" },
    { 0x0019, 0x0039, (char *) "xs", (char *) "?" },
    { 0x0019, 0x003a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x003b, (char *) "LT", (char *) "?" },
    { 0x0019, 0x003c, (char *) "xs", (char *) "?" },
    { 0x0019, 0x003e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x003f, (char *) "UN", (char *) "?" },
    { 0x0019, 0x0040, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0041, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0042, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0043, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0044, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0045, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0046, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0047, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0048, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0049, (char *) "US", (char *) "?" },
    { 0x0019, 0x004a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x004b, (char *) "SL", (char *) "Data Size For Scan Data" },
    { 0x0019, 0x004c, (char *) "US", (char *) "?" },
    { 0x0019, 0x004e, (char *) "US", (char *) "?" },
    { 0x0019, 0x0050, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0051, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0052, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0053, (char *) "LT", (char *) "Barcode" },
    { 0x0019, 0x0054, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0055, (char *) "DS", (char *) "Receiver Reference Gain" },
    { 0x0019, 0x0056, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0057, (char *) "SS", (char *) "CT Water Number" },
    { 0x0019, 0x0058, (char *) "xs", (char *) "?" },
    { 0x0019, 0x005a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x005c, (char *) "xs", (char *) "?" },
    { 0x0019, 0x005d, (char *) "US", (char *) "?" },
    { 0x0019, 0x005e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x005f, (char *) "SL", (char *) "Increment Between Channels" },
    { 0x0019, 0x0060, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0061, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0062, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0063, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0064, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0065, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0066, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0067, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0068, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0069, (char *) "UL", (char *) "Convolution Mode" },
    { 0x0019, 0x006a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x006b, (char *) "SS", (char *) "Field Of View In Detector Cells" },
    { 0x0019, 0x006c, (char *) "US", (char *) "?" },
    { 0x0019, 0x006e, (char *) "US", (char *) "?" },
    { 0x0019, 0x0070, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0071, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0072, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0073, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0074, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0075, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0076, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0077, (char *) "US", (char *) "?" },
    { 0x0019, 0x0078, (char *) "US", (char *) "?" },
    { 0x0019, 0x007a, (char *) "US", (char *) "?" },
    { 0x0019, 0x007c, (char *) "US", (char *) "?" },
    { 0x0019, 0x007d, (char *) "DS", (char *) "Second Echo" },
    { 0x0019, 0x007e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x007f, (char *) "DS", (char *) "Table Delta" },
    { 0x0019, 0x0080, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0081, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0082, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0083, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0084, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0085, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0086, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0087, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0088, (char *) "xs", (char *) "?" },
    { 0x0019, 0x008a, (char *) "xs", (char *) "?" },
    { 0x0019, 0x008b, (char *) "SS", (char *) "Actual Receive Gain Digital" },
    { 0x0019, 0x008c, (char *) "US", (char *) "?" },
    { 0x0019, 0x008d, (char *) "DS", (char *) "Delay After Trigger" },
    { 0x0019, 0x008e, (char *) "US", (char *) "?" },
    { 0x0019, 0x008f, (char *) "SS", (char *) "Swap Phase Frequency" },
    { 0x0019, 0x0090, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0091, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0092, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0093, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0094, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0095, (char *) "SS", (char *) "Analog Receiver Gain" },
    { 0x0019, 0x0096, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0097, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0098, (char *) "xs", (char *) "?" },
    { 0x0019, 0x0099, (char *) "US", (char *) "?" },
    { 0x0019, 0x009a, (char *) "US", (char *) "?" },
    { 0x0019, 0x009b, (char *) "SS", (char *) "Pulse Sequence Mode" },
    { 0x0019, 0x009c, (char *) "xs", (char *) "?" },
    { 0x0019, 0x009d, (char *) "DT", (char *) "Pulse Sequence Date" },
    { 0x0019, 0x009e, (char *) "xs", (char *) "?" },
    { 0x0019, 0x009f, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a0, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a1, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a2, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a3, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a4, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a5, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a6, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a7, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a8, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00a9, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00aa, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00ab, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00ac, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00ad, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00ae, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00af, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b0, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b1, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b2, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b3, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b4, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b5, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00b6, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00b7, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00b8, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00b9, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00ba, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00bb, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00bc, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00bd, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00be, (char *) "DS", (char *) "Projection Angle" },
    { 0x0019, 0x00c0, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00c1, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00c2, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00c3, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00c4, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00c5, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00c6, (char *) "SS", (char *) "SAT Location H" },
    { 0x0019, 0x00c7, (char *) "SS", (char *) "SAT Location F" },
    { 0x0019, 0x00c8, (char *) "SS", (char *) "SAT Thickness R L" },
    { 0x0019, 0x00c9, (char *) "SS", (char *) "SAT Thickness A P" },
    { 0x0019, 0x00ca, (char *) "SS", (char *) "SAT Thickness H F" },
    { 0x0019, 0x00cb, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00cc, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00cd, (char *) "SS", (char *) "Thickness Disclaimer" },
    { 0x0019, 0x00ce, (char *) "SS", (char *) "Prescan Type" },
    { 0x0019, 0x00cf, (char *) "SS", (char *) "Prescan Status" },
    { 0x0019, 0x00d0, (char *) "SH", (char *) "Raw Data Type" },
    { 0x0019, 0x00d1, (char *) "DS", (char *) "Flow Sensitivity" },
    { 0x0019, 0x00d2, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d3, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d4, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d5, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d6, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d7, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d8, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00d9, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00da, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00db, (char *) "DS", (char *) "Back Projector Coefficient" },
    { 0x0019, 0x00dc, (char *) "SS", (char *) "Primary Speed Correction Used" },
    { 0x0019, 0x00dd, (char *) "SS", (char *) "Overrange Correction Used" },
    { 0x0019, 0x00de, (char *) "DS", (char *) "Dynamic Z Alpha Value" },
    { 0x0019, 0x00df, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00e0, (char *) "DS", (char *) "User Data" },
    { 0x0019, 0x00e1, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00e2, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00e3, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00e4, (char *) "LT", (char *) "?" },
    { 0x0019, 0x00e5, (char *) "IS", (char *) "?" },
    { 0x0019, 0x00e6, (char *) "US", (char *) "?" },
    { 0x0019, 0x00e8, (char *) "DS", (char *) "?" },
    { 0x0019, 0x00e9, (char *) "DS", (char *) "?" },
    { 0x0019, 0x00eb, (char *) "DS", (char *) "?" },
    { 0x0019, 0x00ec, (char *) "US", (char *) "?" },
    { 0x0019, 0x00f0, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00f1, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00f2, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00f3, (char *) "xs", (char *) "?" },
    { 0x0019, 0x00f4, (char *) "LT", (char *) "?" },
    { 0x0019, 0x00f9, (char *) "DS", (char *) "Transmission Gain" },
    { 0x0019, 0x1015, (char *) "UN", (char *) "?" },
    { 0x0020, 0x0000, (char *) "UL", (char *) "Relationship Group Length" },
    { 0x0020, 0x000d, (char *) "UI", (char *) "Study Instance UID" },
    { 0x0020, 0x000e, (char *) "UI", (char *) "Series Instance UID" },
    { 0x0020, 0x0010, (char *) "SH", (char *) "Study ID" },
    { 0x0020, 0x0011, (char *) "IS", (char *) "Series Number" },
    { 0x0020, 0x0012, (char *) "IS", (char *) "Acquisition Number" },
    { 0x0020, 0x0013, (char *) "IS", (char *) "Instance (formerly Image) Number" },
    { 0x0020, 0x0014, (char *) "IS", (char *) "Isotope Number" },
    { 0x0020, 0x0015, (char *) "IS", (char *) "Phase Number" },
    { 0x0020, 0x0016, (char *) "IS", (char *) "Interval Number" },
    { 0x0020, 0x0017, (char *) "IS", (char *) "Time Slot Number" },
    { 0x0020, 0x0018, (char *) "IS", (char *) "Angle Number" },
    { 0x0020, 0x0020, (char *) "CS", (char *) "Patient Orientation" },
    { 0x0020, 0x0022, (char *) "IS", (char *) "Overlay Number" },
    { 0x0020, 0x0024, (char *) "IS", (char *) "Curve Number" },
    { 0x0020, 0x0026, (char *) "IS", (char *) "LUT Number" },
    { 0x0020, 0x0030, (char *) "DS", (char *) "Image Position" },
    { 0x0020, 0x0032, (char *) "DS", (char *) "Image Position (Patient)" },
    { 0x0020, 0x0035, (char *) "DS", (char *) "Image Orientation" },
    { 0x0020, 0x0037, (char *) "DS", (char *) "Image Orientation (Patient)" },
    { 0x0020, 0x0050, (char *) "DS", (char *) "Location" },
    { 0x0020, 0x0052, (char *) "UI", (char *) "Frame of Reference UID" },
    { 0x0020, 0x0060, (char *) "CS", (char *) "Laterality" },
    { 0x0020, 0x0062, (char *) "CS", (char *) "Image Laterality" },
    { 0x0020, 0x0070, (char *) "LT", (char *) "Image Geometry Type" },
    { 0x0020, 0x0080, (char *) "LO", (char *) "Masking Image" },
    { 0x0020, 0x0100, (char *) "IS", (char *) "Temporal Position Identifier" },
    { 0x0020, 0x0105, (char *) "IS", (char *) "Number of Temporal Positions" },
    { 0x0020, 0x0110, (char *) "DS", (char *) "Temporal Resolution" },
    { 0x0020, 0x1000, (char *) "IS", (char *) "Series in Study" },
    { 0x0020, 0x1001, (char *) "DS", (char *) "Acquisitions in Series" },
    { 0x0020, 0x1002, (char *) "IS", (char *) "Images in Acquisition" },
    { 0x0020, 0x1003, (char *) "IS", (char *) "Images in Series" },
    { 0x0020, 0x1004, (char *) "IS", (char *) "Acquisitions in Study" },
    { 0x0020, 0x1005, (char *) "IS", (char *) "Images in Study" },
    { 0x0020, 0x1020, (char *) "LO", (char *) "Reference" },
    { 0x0020, 0x1040, (char *) "LO", (char *) "Position Reference Indicator" },
    { 0x0020, 0x1041, (char *) "DS", (char *) "Slice Location" },
    { 0x0020, 0x1070, (char *) "IS", (char *) "Other Study Numbers" },
    { 0x0020, 0x1200, (char *) "IS", (char *) "Number of Patient Related Studies" },
    { 0x0020, 0x1202, (char *) "IS", (char *) "Number of Patient Related Series" },
    { 0x0020, 0x1204, (char *) "IS", (char *) "Number of Patient Related Images" },
    { 0x0020, 0x1206, (char *) "IS", (char *) "Number of Study Related Series" },
    { 0x0020, 0x1208, (char *) "IS", (char *) "Number of Study Related Series" },
    { 0x0020, 0x3100, (char *) "LO", (char *) "Source Image IDs" },
    { 0x0020, 0x3401, (char *) "LO", (char *) "Modifying Device ID" },
    { 0x0020, 0x3402, (char *) "LO", (char *) "Modified Image ID" },
    { 0x0020, 0x3403, (char *) "xs", (char *) "Modified Image Date" },
    { 0x0020, 0x3404, (char *) "LO", (char *) "Modifying Device Manufacturer" },
    { 0x0020, 0x3405, (char *) "xs", (char *) "Modified Image Time" },
    { 0x0020, 0x3406, (char *) "xs", (char *) "Modified Image Description" },
    { 0x0020, 0x4000, (char *) "LT", (char *) "Image Comments" },
    { 0x0020, 0x5000, (char *) "AT", (char *) "Original Image Identification" },
    { 0x0020, 0x5002, (char *) "LO", (char *) "Original Image Identification Nomenclature" },
    { 0x0021, 0x0000, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0001, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0002, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0003, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0004, (char *) "DS", (char *) "VOI Position" },
    { 0x0021, 0x0005, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0006, (char *) "IS", (char *) "CSI Matrix Size Original" },
    { 0x0021, 0x0007, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0008, (char *) "DS", (char *) "Spatial Grid Shift" },
    { 0x0021, 0x0009, (char *) "DS", (char *) "Signal Limits Minimum" },
    { 0x0021, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0011, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0012, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0013, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0014, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0015, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0016, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0017, (char *) "DS", (char *) "EPI Operation Mode Flag" },
    { 0x0021, 0x0018, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0019, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0021, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0022, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0024, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0025, (char *) "US", (char *) "?" },
    { 0x0021, 0x0026, (char *) "IS", (char *) "Image Pixel Offset" },
    { 0x0021, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0031, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0032, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0034, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0035, (char *) "SS", (char *) "Series From Which Prescribed" },
    { 0x0021, 0x0036, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0037, (char *) "SS", (char *) "Screen Format" },
    { 0x0021, 0x0039, (char *) "DS", (char *) "Slab Thickness" },
    { 0x0021, 0x0040, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0041, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0042, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0043, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0044, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0045, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0046, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0047, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0048, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0049, (char *) "xs", (char *) "?" },
    { 0x0021, 0x004a, (char *) "xs", (char *) "?" },
    { 0x0021, 0x004e, (char *) "US", (char *) "?" },
    { 0x0021, 0x004f, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0050, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0051, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0052, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0053, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0054, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0055, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0056, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0057, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0058, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0059, (char *) "xs", (char *) "?" },
    { 0x0021, 0x005a, (char *) "SL", (char *) "Integer Slop" },
    { 0x0021, 0x005b, (char *) "DS", (char *) "Float Slop" },
    { 0x0021, 0x005c, (char *) "DS", (char *) "Float Slop" },
    { 0x0021, 0x005d, (char *) "DS", (char *) "Float Slop" },
    { 0x0021, 0x005e, (char *) "DS", (char *) "Float Slop" },
    { 0x0021, 0x005f, (char *) "DS", (char *) "Float Slop" },
    { 0x0021, 0x0060, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0061, (char *) "DS", (char *) "Image Normal" },
    { 0x0021, 0x0062, (char *) "IS", (char *) "Reference Type Code" },
    { 0x0021, 0x0063, (char *) "DS", (char *) "Image Distance" },
    { 0x0021, 0x0065, (char *) "US", (char *) "Image Positioning History Mask" },
    { 0x0021, 0x006a, (char *) "DS", (char *) "Image Row" },
    { 0x0021, 0x006b, (char *) "DS", (char *) "Image Column" },
    { 0x0021, 0x0070, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0071, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0072, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0073, (char *) "DS", (char *) "Second Repetition Time" },
    { 0x0021, 0x0075, (char *) "DS", (char *) "Light Brightness" },
    { 0x0021, 0x0076, (char *) "DS", (char *) "Light Contrast" },
    { 0x0021, 0x007a, (char *) "IS", (char *) "Overlay Threshold" },
    { 0x0021, 0x007b, (char *) "IS", (char *) "Surface Threshold" },
    { 0x0021, 0x007c, (char *) "IS", (char *) "Grey Scale Threshold" },
    { 0x0021, 0x0080, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0081, (char *) "DS", (char *) "Auto Window Level Alpha" },
    { 0x0021, 0x0082, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0083, (char *) "DS", (char *) "Auto Window Level Window" },
    { 0x0021, 0x0084, (char *) "DS", (char *) "Auto Window Level Level" },
    { 0x0021, 0x0090, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0091, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0092, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0093, (char *) "xs", (char *) "?" },
    { 0x0021, 0x0094, (char *) "DS", (char *) "EPI Change Value of X Component" },
    { 0x0021, 0x0095, (char *) "DS", (char *) "EPI Change Value of Y Component" },
    { 0x0021, 0x0096, (char *) "DS", (char *) "EPI Change Value of Z Component" },
    { 0x0021, 0x00a0, (char *) "xs", (char *) "?" },
    { 0x0021, 0x00a1, (char *) "DS", (char *) "?" },
    { 0x0021, 0x00a2, (char *) "xs", (char *) "?" },
    { 0x0021, 0x00a3, (char *) "LT", (char *) "?" },
    { 0x0021, 0x00a4, (char *) "LT", (char *) "?" },
    { 0x0021, 0x00a7, (char *) "LT", (char *) "?" },
    { 0x0021, 0x00b0, (char *) "IS", (char *) "?" },
    { 0x0021, 0x00c0, (char *) "IS", (char *) "?" },
    { 0x0023, 0x0000, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0001, (char *) "SL", (char *) "Number Of Series In Study" },
    { 0x0023, 0x0002, (char *) "SL", (char *) "Number Of Unarchived Series" },
    { 0x0023, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0040, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0050, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0060, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0070, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0074, (char *) "SL", (char *) "Number Of Updates To Info" },
    { 0x0023, 0x007d, (char *) "SS", (char *) "Indicates If Study Has Complete Info" },
    { 0x0023, 0x0080, (char *) "xs", (char *) "?" },
    { 0x0023, 0x0090, (char *) "xs", (char *) "?" },
    { 0x0023, 0x00ff, (char *) "US", (char *) "?" },
    { 0x0025, 0x0000, (char *) "UL", (char *) "Group Length" },
    { 0x0025, 0x0006, (char *) "SS", (char *) "Last Pulse Sequence Used" },
    { 0x0025, 0x0007, (char *) "SL", (char *) "Images In Series" },
    { 0x0025, 0x0010, (char *) "SS", (char *) "Landmark Counter" },
    { 0x0025, 0x0011, (char *) "SS", (char *) "Number Of Acquisitions" },
    { 0x0025, 0x0014, (char *) "SL", (char *) "Indicates Number Of Updates To Info" },
    { 0x0025, 0x0017, (char *) "SL", (char *) "Series Complete Flag" },
    { 0x0025, 0x0018, (char *) "SL", (char *) "Number Of Images Archived" },
    { 0x0025, 0x0019, (char *) "SL", (char *) "Last Image Number Used" },
    { 0x0025, 0x001a, (char *) "SH", (char *) "Primary Receiver Suite And Host" },
    { 0x0027, 0x0000, (char *) "US", (char *) "?" },
    { 0x0027, 0x0006, (char *) "SL", (char *) "Image Archive Flag" },
    { 0x0027, 0x0010, (char *) "SS", (char *) "Scout Type" },
    { 0x0027, 0x0011, (char *) "UN", (char *) "?" },
    { 0x0027, 0x0012, (char *) "IS", (char *) "?" },
    { 0x0027, 0x0013, (char *) "IS", (char *) "?" },
    { 0x0027, 0x0014, (char *) "IS", (char *) "?" },
    { 0x0027, 0x0015, (char *) "IS", (char *) "?" },
    { 0x0027, 0x0016, (char *) "LT", (char *) "?" },
    { 0x0027, 0x001c, (char *) "SL", (char *) "Vma Mamp" },
    { 0x0027, 0x001d, (char *) "SS", (char *) "Vma Phase" },
    { 0x0027, 0x001e, (char *) "SL", (char *) "Vma Mod" },
    { 0x0027, 0x001f, (char *) "SL", (char *) "Vma Clip" },
    { 0x0027, 0x0020, (char *) "SS", (char *) "Smart Scan On Off Flag" },
    { 0x0027, 0x0030, (char *) "SH", (char *) "Foreign Image Revision" },
    { 0x0027, 0x0031, (char *) "SS", (char *) "Imaging Mode" },
    { 0x0027, 0x0032, (char *) "SS", (char *) "Pulse Sequence" },
    { 0x0027, 0x0033, (char *) "SL", (char *) "Imaging Options" },
    { 0x0027, 0x0035, (char *) "SS", (char *) "Plane Type" },
    { 0x0027, 0x0036, (char *) "SL", (char *) "Oblique Plane" },
    { 0x0027, 0x0040, (char *) "SH", (char *) "RAS Letter Of Image Location" },
    { 0x0027, 0x0041, (char *) "FL", (char *) "Image Location" },
    { 0x0027, 0x0042, (char *) "FL", (char *) "Center R Coord Of Plane Image" },
    { 0x0027, 0x0043, (char *) "FL", (char *) "Center A Coord Of Plane Image" },
    { 0x0027, 0x0044, (char *) "FL", (char *) "Center S Coord Of Plane Image" },
    { 0x0027, 0x0045, (char *) "FL", (char *) "Normal R Coord" },
    { 0x0027, 0x0046, (char *) "FL", (char *) "Normal A Coord" },
    { 0x0027, 0x0047, (char *) "FL", (char *) "Normal S Coord" },
    { 0x0027, 0x0048, (char *) "FL", (char *) "R Coord Of Top Right Corner" },
    { 0x0027, 0x0049, (char *) "FL", (char *) "A Coord Of Top Right Corner" },
    { 0x0027, 0x004a, (char *) "FL", (char *) "S Coord Of Top Right Corner" },
    { 0x0027, 0x004b, (char *) "FL", (char *) "R Coord Of Bottom Right Corner" },
    { 0x0027, 0x004c, (char *) "FL", (char *) "A Coord Of Bottom Right Corner" },
    { 0x0027, 0x004d, (char *) "FL", (char *) "S Coord Of Bottom Right Corner" },
    { 0x0027, 0x0050, (char *) "FL", (char *) "Table Start Location" },
    { 0x0027, 0x0051, (char *) "FL", (char *) "Table End Location" },
    { 0x0027, 0x0052, (char *) "SH", (char *) "RAS Letter For Side Of Image" },
    { 0x0027, 0x0053, (char *) "SH", (char *) "RAS Letter For Anterior Posterior" },
    { 0x0027, 0x0054, (char *) "SH", (char *) "RAS Letter For Scout Start Loc" },
    { 0x0027, 0x0055, (char *) "SH", (char *) "RAS Letter For Scout End Loc" },
    { 0x0027, 0x0060, (char *) "FL", (char *) "Image Dimension X" },
    { 0x0027, 0x0061, (char *) "FL", (char *) "Image Dimension Y" },
    { 0x0027, 0x0062, (char *) "FL", (char *) "Number Of Excitations" },
    { 0x0028, 0x0000, (char *) "UL", (char *) "Image Presentation Group Length" },
    { 0x0028, 0x0002, (char *) "US", (char *) "Samples per Pixel" },
    { 0x0028, 0x0004, (char *) "CS", (char *) "Photometric Interpretation" },
    { 0x0028, 0x0005, (char *) "US", (char *) "Image Dimensions" },
    { 0x0028, 0x0006, (char *) "US", (char *) "Planar Configuration" },
    { 0x0028, 0x0008, (char *) "IS", (char *) "Number of Frames" },
    { 0x0028, 0x0009, (char *) "AT", (char *) "Frame Increment Pointer" },
    { 0x0028, 0x0010, (char *) "US", (char *) "Rows" },
    { 0x0028, 0x0011, (char *) "US", (char *) "Columns" },
    { 0x0028, 0x0012, (char *) "US", (char *) "Planes" },
    { 0x0028, 0x0014, (char *) "US", (char *) "Ultrasound Color Data Present" },
    { 0x0028, 0x0030, (char *) "DS", (char *) "Pixel Spacing" },
    { 0x0028, 0x0031, (char *) "DS", (char *) "Zoom Factor" },
    { 0x0028, 0x0032, (char *) "DS", (char *) "Zoom Center" },
    { 0x0028, 0x0034, (char *) "IS", (char *) "Pixel Aspect Ratio" },
    { 0x0028, 0x0040, (char *) "LO", (char *) "Image Format" },
    { 0x0028, 0x0050, (char *) "LT", (char *) "Manipulated Image" },
    { 0x0028, 0x0051, (char *) "CS", (char *) "Corrected Image" },
    { 0x0028, 0x005f, (char *) "LO", (char *) "Compression Recognition Code" },
    { 0x0028, 0x0060, (char *) "LO", (char *) "Compression Code" },
    { 0x0028, 0x0061, (char *) "SH", (char *) "Compression Originator" },
    { 0x0028, 0x0062, (char *) "SH", (char *) "Compression Label" },
    { 0x0028, 0x0063, (char *) "SH", (char *) "Compression Description" },
    { 0x0028, 0x0065, (char *) "LO", (char *) "Compression Sequence" },
    { 0x0028, 0x0066, (char *) "AT", (char *) "Compression Step Pointers" },
    { 0x0028, 0x0068, (char *) "US", (char *) "Repeat Interval" },
    { 0x0028, 0x0069, (char *) "US", (char *) "Bits Grouped" },
    { 0x0028, 0x0070, (char *) "US", (char *) "Perimeter Table" },
    { 0x0028, 0x0071, (char *) "xs", (char *) "Perimeter Value" },
    { 0x0028, 0x0080, (char *) "US", (char *) "Predictor Rows" },
    { 0x0028, 0x0081, (char *) "US", (char *) "Predictor Columns" },
    { 0x0028, 0x0082, (char *) "US", (char *) "Predictor Constants" },
    { 0x0028, 0x0090, (char *) "LO", (char *) "Blocked Pixels" },
    { 0x0028, 0x0091, (char *) "US", (char *) "Block Rows" },
    { 0x0028, 0x0092, (char *) "US", (char *) "Block Columns" },
    { 0x0028, 0x0093, (char *) "US", (char *) "Row Overlap" },
    { 0x0028, 0x0094, (char *) "US", (char *) "Column Overlap" },
    { 0x0028, 0x0100, (char *) "US", (char *) "Bits Allocated" },
    { 0x0028, 0x0101, (char *) "US", (char *) "Bits Stored" },
    { 0x0028, 0x0102, (char *) "US", (char *) "High Bit" },
    { 0x0028, 0x0103, (char *) "US", (char *) "Pixel Representation" },
    { 0x0028, 0x0104, (char *) "xs", (char *) "Smallest Valid Pixel Value" },
    { 0x0028, 0x0105, (char *) "xs", (char *) "Largest Valid Pixel Value" },
    { 0x0028, 0x0106, (char *) "xs", (char *) "Smallest Image Pixel Value" },
    { 0x0028, 0x0107, (char *) "xs", (char *) "Largest Image Pixel Value" },
    { 0x0028, 0x0108, (char *) "xs", (char *) "Smallest Pixel Value in Series" },
    { 0x0028, 0x0109, (char *) "xs", (char *) "Largest Pixel Value in Series" },
    { 0x0028, 0x0110, (char *) "xs", (char *) "Smallest Pixel Value in Plane" },
    { 0x0028, 0x0111, (char *) "xs", (char *) "Largest Pixel Value in Plane" },
    { 0x0028, 0x0120, (char *) "xs", (char *) "Pixel Padding Value" },
    { 0x0028, 0x0200, (char *) "xs", (char *) "Image Location" },
    { 0x0028, 0x0300, (char *) "CS", (char *) "Quality Control Image" },
    { 0x0028, 0x0301, (char *) "CS", (char *) "Burned In Annotation" },
    { 0x0028, 0x0400, (char *) "xs", (char *) "?" },
    { 0x0028, 0x0401, (char *) "xs", (char *) "?" },
    { 0x0028, 0x0402, (char *) "xs", (char *) "?" },
    { 0x0028, 0x0403, (char *) "xs", (char *) "?" },
    { 0x0028, 0x0404, (char *) "AT", (char *) "Details of Coefficients" },
    { 0x0028, 0x0700, (char *) "LO", (char *) "DCT Label" },
    { 0x0028, 0x0701, (char *) "LO", (char *) "Data Block Description" },
    { 0x0028, 0x0702, (char *) "AT", (char *) "Data Block" },
    { 0x0028, 0x0710, (char *) "US", (char *) "Normalization Factor Format" },
    { 0x0028, 0x0720, (char *) "US", (char *) "Zonal Map Number Format" },
    { 0x0028, 0x0721, (char *) "AT", (char *) "Zonal Map Location" },
    { 0x0028, 0x0722, (char *) "US", (char *) "Zonal Map Format" },
    { 0x0028, 0x0730, (char *) "US", (char *) "Adaptive Map Format" },
    { 0x0028, 0x0740, (char *) "US", (char *) "Code Number Format" },
    { 0x0028, 0x0800, (char *) "LO", (char *) "Code Label" },
    { 0x0028, 0x0802, (char *) "US", (char *) "Number of Tables" },
    { 0x0028, 0x0803, (char *) "AT", (char *) "Code Table Location" },
    { 0x0028, 0x0804, (char *) "US", (char *) "Bits For Code Word" },
    { 0x0028, 0x0808, (char *) "AT", (char *) "Image Data Location" },
    { 0x0028, 0x1040, (char *) "CS", (char *) "Pixel Intensity Relationship" },
    { 0x0028, 0x1041, (char *) "SS", (char *) "Pixel Intensity Relationship Sign" },
    { 0x0028, 0x1050, (char *) "DS", (char *) "Window Center" },
    { 0x0028, 0x1051, (char *) "DS", (char *) "Window Width" },
    { 0x0028, 0x1052, (char *) "DS", (char *) "Rescale Intercept" },
    { 0x0028, 0x1053, (char *) "DS", (char *) "Rescale Slope" },
    { 0x0028, 0x1054, (char *) "LO", (char *) "Rescale Type" },
    { 0x0028, 0x1055, (char *) "LO", (char *) "Window Center & Width Explanation" },
    { 0x0028, 0x1080, (char *) "LO", (char *) "Gray Scale" },
    { 0x0028, 0x1090, (char *) "CS", (char *) "Recommended Viewing Mode" },
    { 0x0028, 0x1100, (char *) "xs", (char *) "Gray Lookup Table Descriptor" },
    { 0x0028, 0x1101, (char *) "xs", (char *) "Red Palette Color Lookup Table Descriptor" },
    { 0x0028, 0x1102, (char *) "xs", (char *) "Green Palette Color Lookup Table Descriptor" },
    { 0x0028, 0x1103, (char *) "xs", (char *) "Blue Palette Color Lookup Table Descriptor" },
    { 0x0028, 0x1111, (char *) "OW", (char *) "Large Red Palette Color Lookup Table Descriptor" },
    { 0x0028, 0x1112, (char *) "OW", (char *) "Large Green Palette Color Lookup Table Descriptor" },
    { 0x0028, 0x1113, (char *) "OW", (char *) "Large Blue Palette Color Lookup Table Descriptor" },
    { 0x0028, 0x1199, (char *) "UI", (char *) "Palette Color Lookup Table UID" },
    { 0x0028, 0x1200, (char *) "xs", (char *) "Gray Lookup Table Data" },
    { 0x0028, 0x1201, (char *) "OW", (char *) "Red Palette Color Lookup Table Data" },
    { 0x0028, 0x1202, (char *) "OW", (char *) "Green Palette Color Lookup Table Data" },
    { 0x0028, 0x1203, (char *) "OW", (char *) "Blue Palette Color Lookup Table Data" },
    { 0x0028, 0x1211, (char *) "OW", (char *) "Large Red Palette Color Lookup Table Data" },
    { 0x0028, 0x1212, (char *) "OW", (char *) "Large Green Palette Color Lookup Table Data" },
    { 0x0028, 0x1213, (char *) "OW", (char *) "Large Blue Palette Color Lookup Table Data" },
    { 0x0028, 0x1214, (char *) "UI", (char *) "Large Palette Color Lookup Table UID" },
    { 0x0028, 0x1221, (char *) "OW", (char *) "Segmented Red Palette Color Lookup Table Data" },
    { 0x0028, 0x1222, (char *) "OW", (char *) "Segmented Green Palette Color Lookup Table Data" },
    { 0x0028, 0x1223, (char *) "OW", (char *) "Segmented Blue Palette Color Lookup Table Data" },
    { 0x0028, 0x1300, (char *) "CS", (char *) "Implant Present" },
    { 0x0028, 0x2110, (char *) "CS", (char *) "Lossy Image Compression" },
    { 0x0028, 0x2112, (char *) "DS", (char *) "Lossy Image Compression Ratio" },
    { 0x0028, 0x3000, (char *) "SQ", (char *) "Modality LUT Sequence" },
    { 0x0028, 0x3002, (char *) "US", (char *) "LUT Descriptor" },
    { 0x0028, 0x3003, (char *) "LO", (char *) "LUT Explanation" },
    { 0x0028, 0x3004, (char *) "LO", (char *) "Modality LUT Type" },
    { 0x0028, 0x3006, (char *) "US", (char *) "LUT Data" },
    { 0x0028, 0x3010, (char *) "xs", (char *) "VOI LUT Sequence" },
    { 0x0028, 0x4000, (char *) "LT", (char *) "Image Presentation Comments" },
    { 0x0028, 0x5000, (char *) "SQ", (char *) "Biplane Acquisition Sequence" },
    { 0x0028, 0x6010, (char *) "US", (char *) "Representative Frame Number" },
    { 0x0028, 0x6020, (char *) "US", (char *) "Frame Numbers of Interest" },
    { 0x0028, 0x6022, (char *) "LO", (char *) "Frame of Interest Description" },
    { 0x0028, 0x6030, (char *) "US", (char *) "Mask Pointer" },
    { 0x0028, 0x6040, (char *) "US", (char *) "R Wave Pointer" },
    { 0x0028, 0x6100, (char *) "SQ", (char *) "Mask Subtraction Sequence" },
    { 0x0028, 0x6101, (char *) "CS", (char *) "Mask Operation" },
    { 0x0028, 0x6102, (char *) "US", (char *) "Applicable Frame Range" },
    { 0x0028, 0x6110, (char *) "US", (char *) "Mask Frame Numbers" },
    { 0x0028, 0x6112, (char *) "US", (char *) "Contrast Frame Averaging" },
    { 0x0028, 0x6114, (char *) "FL", (char *) "Mask Sub-Pixel Shift" },
    { 0x0028, 0x6120, (char *) "SS", (char *) "TID Offset" },
    { 0x0028, 0x6190, (char *) "ST", (char *) "Mask Operation Explanation" },
    { 0x0029, 0x0000, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0001, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0002, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0003, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0004, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0005, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0006, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0007, (char *) "SL", (char *) "Lower Range Of Pixels" },
    { 0x0029, 0x0008, (char *) "SH", (char *) "Lower Range Of Pixels" },
    { 0x0029, 0x0009, (char *) "SH", (char *) "Lower Range Of Pixels" },
    { 0x0029, 0x000a, (char *) "SS", (char *) "Lower Range Of Pixels" },
    { 0x0029, 0x000c, (char *) "xs", (char *) "?" },
    { 0x0029, 0x000e, (char *) "CS", (char *) "Zoom Enable Status" },
    { 0x0029, 0x000f, (char *) "CS", (char *) "Zoom Select Status" },
    { 0x0029, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0011, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0013, (char *) "LT", (char *) "?" },
    { 0x0029, 0x0015, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0016, (char *) "SL", (char *) "Lower Range Of Pixels" },
    { 0x0029, 0x0017, (char *) "SL", (char *) "Lower Range Of Pixels" },
    { 0x0029, 0x0018, (char *) "SL", (char *) "Upper Range Of Pixels" },
    { 0x0029, 0x001a, (char *) "SL", (char *) "Length Of Total Info In Bytes" },
    { 0x0029, 0x001e, (char *) "xs", (char *) "?" },
    { 0x0029, 0x001f, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0022, (char *) "IS", (char *) "Pixel Quality Value" },
    { 0x0029, 0x0025, (char *) "LT", (char *) "Processed Pixel Data Quality" },
    { 0x0029, 0x0026, (char *) "SS", (char *) "Version Of Info Structure" },
    { 0x0029, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0031, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0032, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0033, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0034, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0035, (char *) "SL", (char *) "Advantage Comp Underflow" },
    { 0x0029, 0x0038, (char *) "US", (char *) "?" },
    { 0x0029, 0x0040, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0041, (char *) "DS", (char *) "Magnifying Glass Rectangle" },
    { 0x0029, 0x0043, (char *) "DS", (char *) "Magnifying Glass Factor" },
    { 0x0029, 0x0044, (char *) "US", (char *) "Magnifying Glass Function" },
    { 0x0029, 0x004e, (char *) "CS", (char *) "Magnifying Glass Enable Status" },
    { 0x0029, 0x004f, (char *) "CS", (char *) "Magnifying Glass Select Status" },
    { 0x0029, 0x0050, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0051, (char *) "LT", (char *) "Exposure Code" },
    { 0x0029, 0x0052, (char *) "LT", (char *) "Sort Code" },
    { 0x0029, 0x0053, (char *) "LT", (char *) "?" },
    { 0x0029, 0x0060, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0061, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0067, (char *) "LT", (char *) "?" },
    { 0x0029, 0x0070, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0071, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0072, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0077, (char *) "CS", (char *) "Window Select Status" },
    { 0x0029, 0x0078, (char *) "LT", (char *) "ECG Display Printing ID" },
    { 0x0029, 0x0079, (char *) "CS", (char *) "ECG Display Printing" },
    { 0x0029, 0x007e, (char *) "CS", (char *) "ECG Display Printing Enable Status" },
    { 0x0029, 0x007f, (char *) "CS", (char *) "ECG Display Printing Select Status" },
    { 0x0029, 0x0080, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0081, (char *) "xs", (char *) "?" },
    { 0x0029, 0x0082, (char *) "IS", (char *) "View Zoom" },
    { 0x0029, 0x0083, (char *) "IS", (char *) "View Transform" },
    { 0x0029, 0x008e, (char *) "CS", (char *) "Physiological Display Enable Status" },
    { 0x0029, 0x008f, (char *) "CS", (char *) "Physiological Display Select Status" },
    { 0x0029, 0x0090, (char *) "IS", (char *) "?" },
    { 0x0029, 0x0099, (char *) "LT", (char *) "Shutter Type" },
    { 0x0029, 0x00a0, (char *) "US", (char *) "Rows of Rectangular Shutter" },
    { 0x0029, 0x00a1, (char *) "US", (char *) "Columns of Rectangular Shutter" },
    { 0x0029, 0x00a2, (char *) "US", (char *) "Origin of Rectangular Shutter" },
    { 0x0029, 0x00b0, (char *) "US", (char *) "Radius of Circular Shutter" },
    { 0x0029, 0x00b2, (char *) "US", (char *) "Origin of Circular Shutter" },
    { 0x0029, 0x00c0, (char *) "LT", (char *) "Functional Shutter ID" },
    { 0x0029, 0x00c1, (char *) "xs", (char *) "?" },
    { 0x0029, 0x00c3, (char *) "IS", (char *) "Scan Resolution" },
    { 0x0029, 0x00c4, (char *) "IS", (char *) "Field of View" },
    { 0x0029, 0x00c5, (char *) "LT", (char *) "Field Of Shutter Rectangle" },
    { 0x0029, 0x00ce, (char *) "CS", (char *) "Shutter Enable Status" },
    { 0x0029, 0x00cf, (char *) "CS", (char *) "Shutter Select Status" },
    { 0x0029, 0x00d0, (char *) "IS", (char *) "?" },
    { 0x0029, 0x00d1, (char *) "IS", (char *) "?" },
    { 0x0029, 0x00d5, (char *) "LT", (char *) "Slice Thickness" },
    { 0x0031, 0x0010, (char *) "LT", (char *) "Request UID" },
    { 0x0031, 0x0012, (char *) "LT", (char *) "Examination Reason" },
    { 0x0031, 0x0030, (char *) "DA", (char *) "Requested Date" },
    { 0x0031, 0x0032, (char *) "TM", (char *) "Worklist Request Start Time" },
    { 0x0031, 0x0033, (char *) "TM", (char *) "Worklist Request End Time" },
    { 0x0031, 0x0045, (char *) "LT", (char *) "Requesting Physician" },
    { 0x0031, 0x004a, (char *) "TM", (char *) "Requested Time" },
    { 0x0031, 0x0050, (char *) "LT", (char *) "Requested Physician" },
    { 0x0031, 0x0080, (char *) "LT", (char *) "Requested Location" },
    { 0x0032, 0x0000, (char *) "UL", (char *) "Study Group Length" },
    { 0x0032, 0x000a, (char *) "CS", (char *) "Study Status ID" },
    { 0x0032, 0x000c, (char *) "CS", (char *) "Study Priority ID" },
    { 0x0032, 0x0012, (char *) "LO", (char *) "Study ID Issuer" },
    { 0x0032, 0x0032, (char *) "DA", (char *) "Study Verified Date" },
    { 0x0032, 0x0033, (char *) "TM", (char *) "Study Verified Time" },
    { 0x0032, 0x0034, (char *) "DA", (char *) "Study Read Date" },
    { 0x0032, 0x0035, (char *) "TM", (char *) "Study Read Time" },
    { 0x0032, 0x1000, (char *) "DA", (char *) "Scheduled Study Start Date" },
    { 0x0032, 0x1001, (char *) "TM", (char *) "Scheduled Study Start Time" },
    { 0x0032, 0x1010, (char *) "DA", (char *) "Scheduled Study Stop Date" },
    { 0x0032, 0x1011, (char *) "TM", (char *) "Scheduled Study Stop Time" },
    { 0x0032, 0x1020, (char *) "LO", (char *) "Scheduled Study Location" },
    { 0x0032, 0x1021, (char *) "AE", (char *) "Scheduled Study Location AE Title(s)" },
    { 0x0032, 0x1030, (char *) "LO", (char *) "Reason for Study" },
    { 0x0032, 0x1032, (char *) "PN", (char *) "Requesting Physician" },
    { 0x0032, 0x1033, (char *) "LO", (char *) "Requesting Service" },
    { 0x0032, 0x1040, (char *) "DA", (char *) "Study Arrival Date" },
    { 0x0032, 0x1041, (char *) "TM", (char *) "Study Arrival Time" },
    { 0x0032, 0x1050, (char *) "DA", (char *) "Study Completion Date" },
    { 0x0032, 0x1051, (char *) "TM", (char *) "Study Completion Time" },
    { 0x0032, 0x1055, (char *) "CS", (char *) "Study Component Status ID" },
    { 0x0032, 0x1060, (char *) "LO", (char *) "Requested Procedure Description" },
    { 0x0032, 0x1064, (char *) "SQ", (char *) "Requested Procedure Code Sequence" },
    { 0x0032, 0x1070, (char *) "LO", (char *) "Requested Contrast Agent" },
    { 0x0032, 0x4000, (char *) "LT", (char *) "Study Comments" },
    { 0x0033, 0x0001, (char *) "UN", (char *) "?" },
    { 0x0033, 0x0002, (char *) "UN", (char *) "?" },
    { 0x0033, 0x0005, (char *) "UN", (char *) "?" },
    { 0x0033, 0x0006, (char *) "UN", (char *) "?" },
    { 0x0033, 0x0010, (char *) "LT", (char *) "Patient Study UID" },
    { 0x0037, 0x0010, (char *) "LO", (char *) "ReferringDepartment" },
    { 0x0037, 0x0020, (char *) "US", (char *) "ScreenNumber" },
    { 0x0037, 0x0040, (char *) "SH", (char *) "LeftOrientation" },
    { 0x0037, 0x0042, (char *) "SH", (char *) "RightOrientation" },
    { 0x0037, 0x0050, (char *) "CS", (char *) "Inversion" },
    { 0x0037, 0x0060, (char *) "US", (char *) "DSA" },
    { 0x0038, 0x0000, (char *) "UL", (char *) "Visit Group Length" },
    { 0x0038, 0x0004, (char *) "SQ", (char *) "Referenced Patient Alias Sequence" },
    { 0x0038, 0x0008, (char *) "CS", (char *) "Visit Status ID" },
    { 0x0038, 0x0010, (char *) "LO", (char *) "Admission ID" },
    { 0x0038, 0x0011, (char *) "LO", (char *) "Issuer of Admission ID" },
    { 0x0038, 0x0016, (char *) "LO", (char *) "Route of Admissions" },
    { 0x0038, 0x001a, (char *) "DA", (char *) "Scheduled Admission Date" },
    { 0x0038, 0x001b, (char *) "TM", (char *) "Scheduled Admission Time" },
    { 0x0038, 0x001c, (char *) "DA", (char *) "Scheduled Discharge Date" },
    { 0x0038, 0x001d, (char *) "TM", (char *) "Scheduled Discharge Time" },
    { 0x0038, 0x001e, (char *) "LO", (char *) "Scheduled Patient Institution Residence" },
    { 0x0038, 0x0020, (char *) "DA", (char *) "Admitting Date" },
    { 0x0038, 0x0021, (char *) "TM", (char *) "Admitting Time" },
    { 0x0038, 0x0030, (char *) "DA", (char *) "Discharge Date" },
    { 0x0038, 0x0032, (char *) "TM", (char *) "Discharge Time" },
    { 0x0038, 0x0040, (char *) "LO", (char *) "Discharge Diagnosis Description" },
    { 0x0038, 0x0044, (char *) "SQ", (char *) "Discharge Diagnosis Code Sequence" },
    { 0x0038, 0x0050, (char *) "LO", (char *) "Special Needs" },
    { 0x0038, 0x0300, (char *) "LO", (char *) "Current Patient Location" },
    { 0x0038, 0x0400, (char *) "LO", (char *) "Patient's Institution Residence" },
    { 0x0038, 0x0500, (char *) "LO", (char *) "Patient State" },
    { 0x0038, 0x4000, (char *) "LT", (char *) "Visit Comments" },
    { 0x0039, 0x0080, (char *) "IS", (char *) "Private Entity Number" },
    { 0x0039, 0x0085, (char *) "DA", (char *) "Private Entity Date" },
    { 0x0039, 0x0090, (char *) "TM", (char *) "Private Entity Time" },
    { 0x0039, 0x0095, (char *) "LO", (char *) "Private Entity Launch Command" },
    { 0x0039, 0x00aa, (char *) "CS", (char *) "Private Entity Type" },
    { 0x003a, 0x0002, (char *) "SQ", (char *) "Waveform Sequence" },
    { 0x003a, 0x0005, (char *) "US", (char *) "Waveform Number of Channels" },
    { 0x003a, 0x0010, (char *) "UL", (char *) "Waveform Number of Samples" },
    { 0x003a, 0x001a, (char *) "DS", (char *) "Sampling Frequency" },
    { 0x003a, 0x0020, (char *) "SH", (char *) "Group Label" },
    { 0x003a, 0x0103, (char *) "CS", (char *) "Waveform Sample Value Representation" },
    { 0x003a, 0x0122, (char *) "OB", (char *) "Waveform Padding Value" },
    { 0x003a, 0x0200, (char *) "SQ", (char *) "Channel Definition" },
    { 0x003a, 0x0202, (char *) "IS", (char *) "Waveform Channel Number" },
    { 0x003a, 0x0203, (char *) "SH", (char *) "Channel Label" },
    { 0x003a, 0x0205, (char *) "CS", (char *) "Channel Status" },
    { 0x003a, 0x0208, (char *) "SQ", (char *) "Channel Source" },
    { 0x003a, 0x0209, (char *) "SQ", (char *) "Channel Source Modifiers" },
    { 0x003a, 0x020a, (char *) "SQ", (char *) "Differential Channel Source" },
    { 0x003a, 0x020b, (char *) "SQ", (char *) "Differential Channel Source Modifiers" },
    { 0x003a, 0x0210, (char *) "DS", (char *) "Channel Sensitivity" },
    { 0x003a, 0x0211, (char *) "SQ", (char *) "Channel Sensitivity Units" },
    { 0x003a, 0x0212, (char *) "DS", (char *) "Channel Sensitivity Correction Factor" },
    { 0x003a, 0x0213, (char *) "DS", (char *) "Channel Baseline" },
    { 0x003a, 0x0214, (char *) "DS", (char *) "Channel Time Skew" },
    { 0x003a, 0x0215, (char *) "DS", (char *) "Channel Sample Skew" },
    { 0x003a, 0x0216, (char *) "OB", (char *) "Channel Minimum Value" },
    { 0x003a, 0x0217, (char *) "OB", (char *) "Channel Maximum Value" },
    { 0x003a, 0x0218, (char *) "DS", (char *) "Channel Offset" },
    { 0x003a, 0x021a, (char *) "US", (char *) "Bits Per Sample" },
    { 0x003a, 0x0220, (char *) "DS", (char *) "Filter Low Frequency" },
    { 0x003a, 0x0221, (char *) "DS", (char *) "Filter High Frequency" },
    { 0x003a, 0x0222, (char *) "DS", (char *) "Notch Filter Frequency" },
    { 0x003a, 0x0223, (char *) "DS", (char *) "Notch Filter Bandwidth" },
    { 0x003a, 0x1000, (char *) "OB", (char *) "Waveform Data" },
    { 0x0040, 0x0001, (char *) "AE", (char *) "Scheduled Station AE Title" },
    { 0x0040, 0x0002, (char *) "DA", (char *) "Scheduled Procedure Step Start Date" },
    { 0x0040, 0x0003, (char *) "TM", (char *) "Scheduled Procedure Step Start Time" },
    { 0x0040, 0x0004, (char *) "DA", (char *) "Scheduled Procedure Step End Date" },
    { 0x0040, 0x0005, (char *) "TM", (char *) "Scheduled Procedure Step End Time" },
    { 0x0040, 0x0006, (char *) "PN", (char *) "Scheduled Performing Physician Name" },
    { 0x0040, 0x0007, (char *) "LO", (char *) "Scheduled Procedure Step Description" },
    { 0x0040, 0x0008, (char *) "SQ", (char *) "Scheduled Action Item Code Sequence" },
    { 0x0040, 0x0009, (char *) "SH", (char *) "Scheduled Procedure Step ID" },
    { 0x0040, 0x0010, (char *) "SH", (char *) "Scheduled Station Name" },
    { 0x0040, 0x0011, (char *) "SH", (char *) "Scheduled Procedure Step Location" },
    { 0x0040, 0x0012, (char *) "LO", (char *) "Pre-Medication" },
    { 0x0040, 0x0020, (char *) "CS", (char *) "Scheduled Procedure Step Status" },
    { 0x0040, 0x0100, (char *) "SQ", (char *) "Scheduled Procedure Step Sequence" },
    { 0x0040, 0x0302, (char *) "US", (char *) "Entrance Dose" },
    { 0x0040, 0x0303, (char *) "US", (char *) "Exposed Area" },
    { 0x0040, 0x0306, (char *) "DS", (char *) "Distance Source to Entrance" },
    { 0x0040, 0x0307, (char *) "DS", (char *) "Distance Source to Support" },
    { 0x0040, 0x0310, (char *) "ST", (char *) "Comments On Radiation Dose" },
    { 0x0040, 0x0312, (char *) "DS", (char *) "X-Ray Output" },
    { 0x0040, 0x0314, (char *) "DS", (char *) "Half Value Layer" },
    { 0x0040, 0x0316, (char *) "DS", (char *) "Organ Dose" },
    { 0x0040, 0x0318, (char *) "CS", (char *) "Organ Exposed" },
    { 0x0040, 0x0400, (char *) "LT", (char *) "Comments On Scheduled Procedure Step" },
    { 0x0040, 0x050a, (char *) "LO", (char *) "Specimen Accession Number" },
    { 0x0040, 0x0550, (char *) "SQ", (char *) "Specimen Sequence" },
    { 0x0040, 0x0551, (char *) "LO", (char *) "Specimen Identifier" },
    { 0x0040, 0x0552, (char *) "SQ", (char *) "Specimen Description Sequence" },
    { 0x0040, 0x0553, (char *) "ST", (char *) "Specimen Description" },
    { 0x0040, 0x0555, (char *) "SQ", (char *) "Acquisition Context Sequence" },
    { 0x0040, 0x0556, (char *) "ST", (char *) "Acquisition Context Description" },
    { 0x0040, 0x059a, (char *) "SQ", (char *) "Specimen Type Code Sequence" },
    { 0x0040, 0x06fa, (char *) "LO", (char *) "Slide Identifier" },
    { 0x0040, 0x071a, (char *) "SQ", (char *) "Image Center Point Coordinates Sequence" },
    { 0x0040, 0x072a, (char *) "DS", (char *) "X Offset In Slide Coordinate System" },
    { 0x0040, 0x073a, (char *) "DS", (char *) "Y Offset In Slide Coordinate System" },
    { 0x0040, 0x074a, (char *) "DS", (char *) "Z Offset In Slide Coordinate System" },
    { 0x0040, 0x08d8, (char *) "SQ", (char *) "Pixel Spacing Sequence" },
    { 0x0040, 0x08da, (char *) "SQ", (char *) "Coordinate System Axis Code Sequence" },
    { 0x0040, 0x08ea, (char *) "SQ", (char *) "Measurement Units Code Sequence" },
    { 0x0040, 0x09f8, (char *) "SQ", (char *) "Vital Stain Code Sequence" },
    { 0x0040, 0x1001, (char *) "SH", (char *) "Requested Procedure ID" },
    { 0x0040, 0x1002, (char *) "LO", (char *) "Reason For Requested Procedure" },
    { 0x0040, 0x1003, (char *) "SH", (char *) "Requested Procedure Priority" },
    { 0x0040, 0x1004, (char *) "LO", (char *) "Patient Transport Arrangements" },
    { 0x0040, 0x1005, (char *) "LO", (char *) "Requested Procedure Location" },
    { 0x0040, 0x1006, (char *) "SH", (char *) "Placer Order Number of Procedure" },
    { 0x0040, 0x1007, (char *) "SH", (char *) "Filler Order Number of Procedure" },
    { 0x0040, 0x1008, (char *) "LO", (char *) "Confidentiality Code" },
    { 0x0040, 0x1009, (char *) "SH", (char *) "Reporting Priority" },
    { 0x0040, 0x1010, (char *) "PN", (char *) "Names of Intended Recipients of Results" },
    { 0x0040, 0x1400, (char *) "LT", (char *) "Requested Procedure Comments" },
    { 0x0040, 0x2001, (char *) "LO", (char *) "Reason For Imaging Service Request" },
    { 0x0040, 0x2004, (char *) "DA", (char *) "Issue Date of Imaging Service Request" },
    { 0x0040, 0x2005, (char *) "TM", (char *) "Issue Time of Imaging Service Request" },
    { 0x0040, 0x2006, (char *) "SH", (char *) "Placer Order Number of Imaging Service Request" },
    { 0x0040, 0x2007, (char *) "SH", (char *) "Filler Order Number of Imaging Service Request" },
    { 0x0040, 0x2008, (char *) "PN", (char *) "Order Entered By" },
    { 0x0040, 0x2009, (char *) "SH", (char *) "Order Enterer Location" },
    { 0x0040, 0x2010, (char *) "SH", (char *) "Order Callback Phone Number" },
    { 0x0040, 0x2400, (char *) "LT", (char *) "Imaging Service Request Comments" },
    { 0x0040, 0x3001, (char *) "LO", (char *) "Confidentiality Constraint On Patient Data" },
    { 0x0040, 0xa007, (char *) "CS", (char *) "Findings Flag" },
    { 0x0040, 0xa020, (char *) "SQ", (char *) "Findings Sequence" },
    { 0x0040, 0xa021, (char *) "UI", (char *) "Findings Group UID" },
    { 0x0040, 0xa022, (char *) "UI", (char *) "Referenced Findings Group UID" },
    { 0x0040, 0xa023, (char *) "DA", (char *) "Findings Group Recording Date" },
    { 0x0040, 0xa024, (char *) "TM", (char *) "Findings Group Recording Time" },
    { 0x0040, 0xa026, (char *) "SQ", (char *) "Findings Source Category Code Sequence" },
    { 0x0040, 0xa027, (char *) "LO", (char *) "Documenting Organization" },
    { 0x0040, 0xa028, (char *) "SQ", (char *) "Documenting Organization Identifier Code Sequence" },
    { 0x0040, 0xa032, (char *) "LO", (char *) "History Reliability Qualifier Description" },
    { 0x0040, 0xa043, (char *) "SQ", (char *) "Concept Name Code Sequence" },
    { 0x0040, 0xa047, (char *) "LO", (char *) "Measurement Precision Description" },
    { 0x0040, 0xa057, (char *) "CS", (char *) "Urgency or Priority Alerts" },
    { 0x0040, 0xa060, (char *) "LO", (char *) "Sequencing Indicator" },
    { 0x0040, 0xa066, (char *) "SQ", (char *) "Document Identifier Code Sequence" },
    { 0x0040, 0xa067, (char *) "PN", (char *) "Document Author" },
    { 0x0040, 0xa068, (char *) "SQ", (char *) "Document Author Identifier Code Sequence" },
    { 0x0040, 0xa070, (char *) "SQ", (char *) "Identifier Code Sequence" },
    { 0x0040, 0xa073, (char *) "LO", (char *) "Object String Identifier" },
    { 0x0040, 0xa074, (char *) "OB", (char *) "Object Binary Identifier" },
    { 0x0040, 0xa075, (char *) "PN", (char *) "Documenting Observer" },
    { 0x0040, 0xa076, (char *) "SQ", (char *) "Documenting Observer Identifier Code Sequence" },
    { 0x0040, 0xa078, (char *) "SQ", (char *) "Observation Subject Identifier Code Sequence" },
    { 0x0040, 0xa080, (char *) "SQ", (char *) "Person Identifier Code Sequence" },
    { 0x0040, 0xa085, (char *) "SQ", (char *) "Procedure Identifier Code Sequence" },
    { 0x0040, 0xa088, (char *) "LO", (char *) "Object Directory String Identifier" },
    { 0x0040, 0xa089, (char *) "OB", (char *) "Object Directory Binary Identifier" },
    { 0x0040, 0xa090, (char *) "CS", (char *) "History Reliability Qualifier" },
    { 0x0040, 0xa0a0, (char *) "CS", (char *) "Referenced Type of Data" },
    { 0x0040, 0xa0b0, (char *) "US", (char *) "Referenced Waveform Channels" },
    { 0x0040, 0xa110, (char *) "DA", (char *) "Date of Document or Verbal Transaction" },
    { 0x0040, 0xa112, (char *) "TM", (char *) "Time of Document Creation or Verbal Transaction" },
    { 0x0040, 0xa121, (char *) "DA", (char *) "Date" },
    { 0x0040, 0xa122, (char *) "TM", (char *) "Time" },
    { 0x0040, 0xa123, (char *) "PN", (char *) "Person Name" },
    { 0x0040, 0xa124, (char *) "SQ", (char *) "Referenced Person Sequence" },
    { 0x0040, 0xa125, (char *) "CS", (char *) "Report Status ID" },
    { 0x0040, 0xa130, (char *) "CS", (char *) "Temporal Range Type" },
    { 0x0040, 0xa132, (char *) "UL", (char *) "Referenced Sample Offsets" },
    { 0x0040, 0xa136, (char *) "US", (char *) "Referenced Frame Numbers" },
    { 0x0040, 0xa138, (char *) "DS", (char *) "Referenced Time Offsets" },
    { 0x0040, 0xa13a, (char *) "DT", (char *) "Referenced Datetime" },
    { 0x0040, 0xa160, (char *) "UT", (char *) "Text Value" },
    { 0x0040, 0xa167, (char *) "SQ", (char *) "Observation Category Code Sequence" },
    { 0x0040, 0xa168, (char *) "SQ", (char *) "Concept Code Sequence" },
    { 0x0040, 0xa16a, (char *) "ST", (char *) "Bibliographic Citation" },
    { 0x0040, 0xa170, (char *) "CS", (char *) "Observation Class" },
    { 0x0040, 0xa171, (char *) "UI", (char *) "Observation UID" },
    { 0x0040, 0xa172, (char *) "UI", (char *) "Referenced Observation UID" },
    { 0x0040, 0xa173, (char *) "CS", (char *) "Referenced Observation Class" },
    { 0x0040, 0xa174, (char *) "CS", (char *) "Referenced Object Observation Class" },
    { 0x0040, 0xa180, (char *) "US", (char *) "Annotation Group Number" },
    { 0x0040, 0xa192, (char *) "DA", (char *) "Observation Date" },
    { 0x0040, 0xa193, (char *) "TM", (char *) "Observation Time" },
    { 0x0040, 0xa194, (char *) "CS", (char *) "Measurement Automation" },
    { 0x0040, 0xa195, (char *) "SQ", (char *) "Concept Name Code Sequence Modifier" },
    { 0x0040, 0xa224, (char *) "ST", (char *) "Identification Description" },
    { 0x0040, 0xa290, (char *) "CS", (char *) "Coordinates Set Geometric Type" },
    { 0x0040, 0xa296, (char *) "SQ", (char *) "Algorithm Code Sequence" },
    { 0x0040, 0xa297, (char *) "ST", (char *) "Algorithm Description" },
    { 0x0040, 0xa29a, (char *) "SL", (char *) "Pixel Coordinates Set" },
    { 0x0040, 0xa300, (char *) "SQ", (char *) "Measured Value Sequence" },
    { 0x0040, 0xa307, (char *) "PN", (char *) "Current Observer" },
    { 0x0040, 0xa30a, (char *) "DS", (char *) "Numeric Value" },
    { 0x0040, 0xa313, (char *) "SQ", (char *) "Referenced Accession Sequence" },
    { 0x0040, 0xa33a, (char *) "ST", (char *) "Report Status Comment" },
    { 0x0040, 0xa340, (char *) "SQ", (char *) "Procedure Context Sequence" },
    { 0x0040, 0xa352, (char *) "PN", (char *) "Verbal Source" },
    { 0x0040, 0xa353, (char *) "ST", (char *) "Address" },
    { 0x0040, 0xa354, (char *) "LO", (char *) "Telephone Number" },
    { 0x0040, 0xa358, (char *) "SQ", (char *) "Verbal Source Identifier Code Sequence" },
    { 0x0040, 0xa380, (char *) "SQ", (char *) "Report Detail Sequence" },
    { 0x0040, 0xa402, (char *) "UI", (char *) "Observation Subject UID" },
    { 0x0040, 0xa403, (char *) "CS", (char *) "Observation Subject Class" },
    { 0x0040, 0xa404, (char *) "SQ", (char *) "Observation Subject Type Code Sequence" },
    { 0x0040, 0xa600, (char *) "CS", (char *) "Observation Subject Context Flag" },
    { 0x0040, 0xa601, (char *) "CS", (char *) "Observer Context Flag" },
    { 0x0040, 0xa603, (char *) "CS", (char *) "Procedure Context Flag" },
    { 0x0040, 0xa730, (char *) "SQ", (char *) "Observations Sequence" },
    { 0x0040, 0xa731, (char *) "SQ", (char *) "Relationship Sequence" },
    { 0x0040, 0xa732, (char *) "SQ", (char *) "Relationship Type Code Sequence" },
    { 0x0040, 0xa744, (char *) "SQ", (char *) "Language Code Sequence" },
    { 0x0040, 0xa992, (char *) "ST", (char *) "Uniform Resource Locator" },
    { 0x0040, 0xb020, (char *) "SQ", (char *) "Annotation Sequence" },
    { 0x0040, 0xdb73, (char *) "SQ", (char *) "Relationship Type Code Sequence Modifier" },
    { 0x0041, 0x0000, (char *) "LT", (char *) "Papyrus Comments" },
    { 0x0041, 0x0010, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0011, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0012, (char *) "UL", (char *) "Pixel Offset" },
    { 0x0041, 0x0013, (char *) "SQ", (char *) "Image Identifier Sequence" },
    { 0x0041, 0x0014, (char *) "SQ", (char *) "External File Reference Sequence" },
    { 0x0041, 0x0015, (char *) "US", (char *) "Number of Images" },
    { 0x0041, 0x0020, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0021, (char *) "UI", (char *) "Referenced SOP Class UID" },
    { 0x0041, 0x0022, (char *) "UI", (char *) "Referenced SOP Instance UID" },
    { 0x0041, 0x0030, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0031, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0032, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0034, (char *) "DA", (char *) "Modified Date" },
    { 0x0041, 0x0036, (char *) "TM", (char *) "Modified Time" },
    { 0x0041, 0x0040, (char *) "LT", (char *) "Owner Name" },
    { 0x0041, 0x0041, (char *) "UI", (char *) "Referenced Image SOP Class UID" },
    { 0x0041, 0x0042, (char *) "UI", (char *) "Referenced Image SOP Instance UID" },
    { 0x0041, 0x0050, (char *) "xs", (char *) "?" },
    { 0x0041, 0x0060, (char *) "UL", (char *) "Number of Images" },
    { 0x0041, 0x0062, (char *) "UL", (char *) "Number of Other" },
    { 0x0041, 0x00a0, (char *) "LT", (char *) "External Folder Element DSID" },
    { 0x0041, 0x00a1, (char *) "US", (char *) "External Folder Element Data Set Type" },
    { 0x0041, 0x00a2, (char *) "LT", (char *) "External Folder Element File Location" },
    { 0x0041, 0x00a3, (char *) "UL", (char *) "External Folder Element Length" },
    { 0x0041, 0x00b0, (char *) "LT", (char *) "Internal Folder Element DSID" },
    { 0x0041, 0x00b1, (char *) "US", (char *) "Internal Folder Element Data Set Type" },
    { 0x0041, 0x00b2, (char *) "UL", (char *) "Internal Offset To Data Set" },
    { 0x0041, 0x00b3, (char *) "UL", (char *) "Internal Offset To Image" },
    { 0x0043, 0x0001, (char *) "SS", (char *) "Bitmap Of Prescan Options" },
    { 0x0043, 0x0002, (char *) "SS", (char *) "Gradient Offset In X" },
    { 0x0043, 0x0003, (char *) "SS", (char *) "Gradient Offset In Y" },
    { 0x0043, 0x0004, (char *) "SS", (char *) "Gradient Offset In Z" },
    { 0x0043, 0x0005, (char *) "SS", (char *) "Image Is Original Or Unoriginal" },
    { 0x0043, 0x0006, (char *) "SS", (char *) "Number Of EPI Shots" },
    { 0x0043, 0x0007, (char *) "SS", (char *) "Views Per Segment" },
    { 0x0043, 0x0008, (char *) "SS", (char *) "Respiratory Rate In BPM" },
    { 0x0043, 0x0009, (char *) "SS", (char *) "Respiratory Trigger Point" },
    { 0x0043, 0x000a, (char *) "SS", (char *) "Type Of Receiver Used" },
    { 0x0043, 0x000b, (char *) "DS", (char *) "Peak Rate Of Change Of Gradient Field" },
    { 0x0043, 0x000c, (char *) "DS", (char *) "Limits In Units Of Percent" },
    { 0x0043, 0x000d, (char *) "DS", (char *) "PSD Estimated Limit" },
    { 0x0043, 0x000e, (char *) "DS", (char *) "PSD Estimated Limit In Tesla Per Second" },
    { 0x0043, 0x000f, (char *) "DS", (char *) "SAR Avg Head" },
    { 0x0043, 0x0010, (char *) "US", (char *) "Window Value" },
    { 0x0043, 0x0011, (char *) "US", (char *) "Total Input Views" },
    { 0x0043, 0x0012, (char *) "SS", (char *) "Xray Chain" },
    { 0x0043, 0x0013, (char *) "SS", (char *) "Recon Kernel Parameters" },
    { 0x0043, 0x0014, (char *) "SS", (char *) "Calibration Parameters" },
    { 0x0043, 0x0015, (char *) "SS", (char *) "Total Output Views" },
    { 0x0043, 0x0016, (char *) "SS", (char *) "Number Of Overranges" },
    { 0x0043, 0x0017, (char *) "DS", (char *) "IBH Image Scale Factors" },
    { 0x0043, 0x0018, (char *) "DS", (char *) "BBH Coefficients" },
    { 0x0043, 0x0019, (char *) "SS", (char *) "Number Of BBH Chains To Blend" },
    { 0x0043, 0x001a, (char *) "SL", (char *) "Starting Channel Number" },
    { 0x0043, 0x001b, (char *) "SS", (char *) "PPScan Parameters" },
    { 0x0043, 0x001c, (char *) "SS", (char *) "GE Image Integrity" },
    { 0x0043, 0x001d, (char *) "SS", (char *) "Level Value" },
    { 0x0043, 0x001e, (char *) "xs", (char *) "?" },
    { 0x0043, 0x001f, (char *) "SL", (char *) "Max Overranges In A View" },
    { 0x0043, 0x0020, (char *) "DS", (char *) "Avg Overranges All Views" },
    { 0x0043, 0x0021, (char *) "SS", (char *) "Corrected Afterglow Terms" },
    { 0x0043, 0x0025, (char *) "SS", (char *) "Reference Channels" },
    { 0x0043, 0x0026, (char *) "US", (char *) "No Views Ref Channels Blocked" },
    { 0x0043, 0x0027, (char *) "xs", (char *) "?" },
    { 0x0043, 0x0028, (char *) "OB", (char *) "Unique Image Identifier" },
    { 0x0043, 0x0029, (char *) "OB", (char *) "Histogram Tables" },
    { 0x0043, 0x002a, (char *) "OB", (char *) "User Defined Data" },
    { 0x0043, 0x002b, (char *) "SS", (char *) "Private Scan Options" },
    { 0x0043, 0x002c, (char *) "SS", (char *) "Effective Echo Spacing" },
    { 0x0043, 0x002d, (char *) "SH", (char *) "String Slop Field 1" },
    { 0x0043, 0x002e, (char *) "SH", (char *) "String Slop Field 2" },
    { 0x0043, 0x002f, (char *) "SS", (char *) "Raw Data Type" },
    { 0x0043, 0x0030, (char *) "SS", (char *) "Raw Data Type" },
    { 0x0043, 0x0031, (char *) "DS", (char *) "RA Coord Of Target Recon Centre" },
    { 0x0043, 0x0032, (char *) "SS", (char *) "Raw Data Type" },
    { 0x0043, 0x0033, (char *) "FL", (char *) "Neg Scan Spacing" },
    { 0x0043, 0x0034, (char *) "IS", (char *) "Offset Frequency" },
    { 0x0043, 0x0035, (char *) "UL", (char *) "User Usage Tag" },
    { 0x0043, 0x0036, (char *) "UL", (char *) "User Fill Map MSW" },
    { 0x0043, 0x0037, (char *) "UL", (char *) "User Fill Map LSW" },
    { 0x0043, 0x0038, (char *) "FL", (char *) "User 25 To User 48" },
    { 0x0043, 0x0039, (char *) "IS", (char *) "Slop Integer 6 To Slop Integer 9" },
    { 0x0043, 0x0040, (char *) "FL", (char *) "Trigger On Position" },
    { 0x0043, 0x0041, (char *) "FL", (char *) "Degree Of Rotation" },
    { 0x0043, 0x0042, (char *) "SL", (char *) "DAS Trigger Source" },
    { 0x0043, 0x0043, (char *) "SL", (char *) "DAS Fpa Gain" },
    { 0x0043, 0x0044, (char *) "SL", (char *) "DAS Output Source" },
    { 0x0043, 0x0045, (char *) "SL", (char *) "DAS Ad Input" },
    { 0x0043, 0x0046, (char *) "SL", (char *) "DAS Cal Mode" },
    { 0x0043, 0x0047, (char *) "SL", (char *) "DAS Cal Frequency" },
    { 0x0043, 0x0048, (char *) "SL", (char *) "DAS Reg Xm" },
    { 0x0043, 0x0049, (char *) "SL", (char *) "DAS Auto Zero" },
    { 0x0043, 0x004a, (char *) "SS", (char *) "Starting Channel Of View" },
    { 0x0043, 0x004b, (char *) "SL", (char *) "DAS Xm Pattern" },
    { 0x0043, 0x004c, (char *) "SS", (char *) "TGGC Trigger Mode" },
    { 0x0043, 0x004d, (char *) "FL", (char *) "Start Scan To Xray On Delay" },
    { 0x0043, 0x004e, (char *) "FL", (char *) "Duration Of Xray On" },
    { 0x0044, 0x0000, (char *) "UI", (char *) "?" },
    { 0x0045, 0x0004, (char *) "CS", (char *) "AES" },
    { 0x0045, 0x0006, (char *) "DS", (char *) "Angulation" },
    { 0x0045, 0x0009, (char *) "DS", (char *) "Real Magnification Factor" },
    { 0x0045, 0x000b, (char *) "CS", (char *) "Senograph Type" },
    { 0x0045, 0x000c, (char *) "DS", (char *) "Integration Time" },
    { 0x0045, 0x000d, (char *) "DS", (char *) "ROI Origin X and Y" },
    { 0x0045, 0x0011, (char *) "DS", (char *) "Receptor Size cm X and Y" },
    { 0x0045, 0x0012, (char *) "IS", (char *) "Receptor Size Pixels X and Y" },
    { 0x0045, 0x0013, (char *) "ST", (char *) "Screen" },
    { 0x0045, 0x0014, (char *) "DS", (char *) "Pixel Pitch Microns" },
    { 0x0045, 0x0015, (char *) "IS", (char *) "Pixel Depth Bits" },
    { 0x0045, 0x0016, (char *) "IS", (char *) "Binning Factor X and Y" },
    { 0x0045, 0x001b, (char *) "CS", (char *) "Clinical View" },
    { 0x0045, 0x001d, (char *) "DS", (char *) "Mean Of Raw Gray Levels" },
    { 0x0045, 0x001e, (char *) "DS", (char *) "Mean Of Offset Gray Levels" },
    { 0x0045, 0x001f, (char *) "DS", (char *) "Mean Of Corrected Gray Levels" },
    { 0x0045, 0x0020, (char *) "DS", (char *) "Mean Of Region Gray Levels" },
    { 0x0045, 0x0021, (char *) "DS", (char *) "Mean Of Log Region Gray Levels" },
    { 0x0045, 0x0022, (char *) "DS", (char *) "Standard Deviation Of Raw Gray Levels" },
    { 0x0045, 0x0023, (char *) "DS", (char *) "Standard Deviation Of Corrected Gray Levels" },
    { 0x0045, 0x0024, (char *) "DS", (char *) "Standard Deviation Of Region Gray Levels" },
    { 0x0045, 0x0025, (char *) "DS", (char *) "Standard Deviation Of Log Region Gray Levels" },
    { 0x0045, 0x0026, (char *) "OB", (char *) "MAO Buffer" },
    { 0x0045, 0x0027, (char *) "IS", (char *) "Set Number" },
    { 0x0045, 0x0028, (char *) "CS", (char *) "WindowingType (LINEAR or GAMMA)" },
    { 0x0045, 0x0029, (char *) "DS", (char *) "WindowingParameters" },
    { 0x0045, 0x002a, (char *) "IS", (char *) "Crosshair Cursor X Coordinates" },
    { 0x0045, 0x002b, (char *) "IS", (char *) "Crosshair Cursor Y Coordinates" },
    { 0x0045, 0x0039, (char *) "US", (char *) "Vignette Rows" },
    { 0x0045, 0x003a, (char *) "US", (char *) "Vignette Columns" },
    { 0x0045, 0x003b, (char *) "US", (char *) "Vignette Bits Allocated" },
    { 0x0045, 0x003c, (char *) "US", (char *) "Vignette Bits Stored" },
    { 0x0045, 0x003d, (char *) "US", (char *) "Vignette High Bit" },
    { 0x0045, 0x003e, (char *) "US", (char *) "Vignette Pixel Representation" },
    { 0x0045, 0x003f, (char *) "OB", (char *) "Vignette Pixel Data" },
    { 0x0047, 0x0001, (char *) "SQ", (char *) "Reconstruction Parameters Sequence" },
    { 0x0047, 0x0050, (char *) "UL", (char *) "Volume Voxel Count" },
    { 0x0047, 0x0051, (char *) "UL", (char *) "Volume Segment Count" },
    { 0x0047, 0x0053, (char *) "US", (char *) "Volume Slice Size" },
    { 0x0047, 0x0054, (char *) "US", (char *) "Volume Slice Count" },
    { 0x0047, 0x0055, (char *) "SL", (char *) "Volume Threshold Value" },
    { 0x0047, 0x0057, (char *) "DS", (char *) "Volume Voxel Ratio" },
    { 0x0047, 0x0058, (char *) "DS", (char *) "Volume Voxel Size" },
    { 0x0047, 0x0059, (char *) "US", (char *) "Volume Z Position Size" },
    { 0x0047, 0x0060, (char *) "DS", (char *) "Volume Base Line" },
    { 0x0047, 0x0061, (char *) "DS", (char *) "Volume Center Point" },
    { 0x0047, 0x0063, (char *) "SL", (char *) "Volume Skew Base" },
    { 0x0047, 0x0064, (char *) "DS", (char *) "Volume Registration Transform Rotation Matrix" },
    { 0x0047, 0x0065, (char *) "DS", (char *) "Volume Registration Transform Translation Vector" },
    { 0x0047, 0x0070, (char *) "DS", (char *) "KVP List" },
    { 0x0047, 0x0071, (char *) "IS", (char *) "XRay Tube Current List" },
    { 0x0047, 0x0072, (char *) "IS", (char *) "Exposure List" },
    { 0x0047, 0x0080, (char *) "LO", (char *) "Acquisition DLX Identifier" },
    { 0x0047, 0x0085, (char *) "SQ", (char *) "Acquisition DLX 2D Series Sequence" },
    { 0x0047, 0x0089, (char *) "DS", (char *) "Contrast Agent Volume List" },
    { 0x0047, 0x008a, (char *) "US", (char *) "Number Of Injections" },
    { 0x0047, 0x008b, (char *) "US", (char *) "Frame Count" },
    { 0x0047, 0x0096, (char *) "IS", (char *) "Used Frames" },
    { 0x0047, 0x0091, (char *) "LO", (char *) "XA 3D Reconstruction Algorithm Name" },
    { 0x0047, 0x0092, (char *) "CS", (char *) "XA 3D Reconstruction Algorithm Version" },
    { 0x0047, 0x0093, (char *) "DA", (char *) "DLX Calibration Date" },
    { 0x0047, 0x0094, (char *) "TM", (char *) "DLX Calibration Time" },
    { 0x0047, 0x0095, (char *) "CS", (char *) "DLX Calibration Status" },
    { 0x0047, 0x0098, (char *) "US", (char *) "Transform Count" },
    { 0x0047, 0x0099, (char *) "SQ", (char *) "Transform Sequence" },
    { 0x0047, 0x009a, (char *) "DS", (char *) "Transform Rotation Matrix" },
    { 0x0047, 0x009b, (char *) "DS", (char *) "Transform Translation Vector" },
    { 0x0047, 0x009c, (char *) "LO", (char *) "Transform Label" },
    { 0x0047, 0x00b1, (char *) "US", (char *) "Wireframe Count" },
    { 0x0047, 0x00b2, (char *) "US", (char *) "Location System" },
    { 0x0047, 0x00b0, (char *) "SQ", (char *) "Wireframe List" },
    { 0x0047, 0x00b5, (char *) "LO", (char *) "Wireframe Name" },
    { 0x0047, 0x00b6, (char *) "LO", (char *) "Wireframe Group Name" },
    { 0x0047, 0x00b7, (char *) "LO", (char *) "Wireframe Color" },
    { 0x0047, 0x00b8, (char *) "SL", (char *) "Wireframe Attributes" },
    { 0x0047, 0x00b9, (char *) "SL", (char *) "Wireframe Point Count" },
    { 0x0047, 0x00ba, (char *) "SL", (char *) "Wireframe Timestamp" },
    { 0x0047, 0x00bb, (char *) "SQ", (char *) "Wireframe Point List" },
    { 0x0047, 0x00bc, (char *) "DS", (char *) "Wireframe Points Coordinates" },
    { 0x0047, 0x00c0, (char *) "DS", (char *) "Volume Upper Left High Corner RAS" },
    { 0x0047, 0x00c1, (char *) "DS", (char *) "Volume Slice To RAS Rotation Matrix" },
    { 0x0047, 0x00c2, (char *) "DS", (char *) "Volume Upper Left High Corner TLOC" },
    { 0x0047, 0x00d1, (char *) "OB", (char *) "Volume Segment List" },
    { 0x0047, 0x00d2, (char *) "OB", (char *) "Volume Gradient List" },
    { 0x0047, 0x00d3, (char *) "OB", (char *) "Volume Density List" },
    { 0x0047, 0x00d4, (char *) "OB", (char *) "Volume Z Position List" },
    { 0x0047, 0x00d5, (char *) "OB", (char *) "Volume Original Index List" },
    { 0x0050, 0x0000, (char *) "UL", (char *) "Calibration Group Length" },
    { 0x0050, 0x0004, (char *) "CS", (char *) "Calibration Object" },
    { 0x0050, 0x0010, (char *) "SQ", (char *) "DeviceSequence" },
    { 0x0050, 0x0014, (char *) "DS", (char *) "DeviceLength" },
    { 0x0050, 0x0016, (char *) "DS", (char *) "DeviceDiameter" },
    { 0x0050, 0x0017, (char *) "CS", (char *) "DeviceDiameterUnits" },
    { 0x0050, 0x0018, (char *) "DS", (char *) "DeviceVolume" },
    { 0x0050, 0x0019, (char *) "DS", (char *) "InterMarkerDistance" },
    { 0x0050, 0x0020, (char *) "LO", (char *) "DeviceDescription" },
    { 0x0050, 0x0030, (char *) "SQ", (char *) "CodedInterventionDeviceSequence" },
    { 0x0051, 0x0010, (char *) "xs", (char *) "Image Text" },
    { 0x0054, 0x0000, (char *) "UL", (char *) "Nuclear Acquisition Group Length" },
    { 0x0054, 0x0010, (char *) "US", (char *) "Energy Window Vector" },
    { 0x0054, 0x0011, (char *) "US", (char *) "Number of Energy Windows" },
    { 0x0054, 0x0012, (char *) "SQ", (char *) "Energy Window Information Sequence" },
    { 0x0054, 0x0013, (char *) "SQ", (char *) "Energy Window Range Sequence" },
    { 0x0054, 0x0014, (char *) "DS", (char *) "Energy Window Lower Limit" },
    { 0x0054, 0x0015, (char *) "DS", (char *) "Energy Window Upper Limit" },
    { 0x0054, 0x0016, (char *) "SQ", (char *) "Radiopharmaceutical Information Sequence" },
    { 0x0054, 0x0017, (char *) "IS", (char *) "Residual Syringe Counts" },
    { 0x0054, 0x0018, (char *) "SH", (char *) "Energy Window Name" },
    { 0x0054, 0x0020, (char *) "US", (char *) "Detector Vector" },
    { 0x0054, 0x0021, (char *) "US", (char *) "Number of Detectors" },
    { 0x0054, 0x0022, (char *) "SQ", (char *) "Detector Information Sequence" },
    { 0x0054, 0x0030, (char *) "US", (char *) "Phase Vector" },
    { 0x0054, 0x0031, (char *) "US", (char *) "Number of Phases" },
    { 0x0054, 0x0032, (char *) "SQ", (char *) "Phase Information Sequence" },
    { 0x0054, 0x0033, (char *) "US", (char *) "Number of Frames In Phase" },
    { 0x0054, 0x0036, (char *) "IS", (char *) "Phase Delay" },
    { 0x0054, 0x0038, (char *) "IS", (char *) "Pause Between Frames" },
    { 0x0054, 0x0050, (char *) "US", (char *) "Rotation Vector" },
    { 0x0054, 0x0051, (char *) "US", (char *) "Number of Rotations" },
    { 0x0054, 0x0052, (char *) "SQ", (char *) "Rotation Information Sequence" },
    { 0x0054, 0x0053, (char *) "US", (char *) "Number of Frames In Rotation" },
    { 0x0054, 0x0060, (char *) "US", (char *) "R-R Interval Vector" },
    { 0x0054, 0x0061, (char *) "US", (char *) "Number of R-R Intervals" },
    { 0x0054, 0x0062, (char *) "SQ", (char *) "Gated Information Sequence" },
    { 0x0054, 0x0063, (char *) "SQ", (char *) "Data Information Sequence" },
    { 0x0054, 0x0070, (char *) "US", (char *) "Time Slot Vector" },
    { 0x0054, 0x0071, (char *) "US", (char *) "Number of Time Slots" },
    { 0x0054, 0x0072, (char *) "SQ", (char *) "Time Slot Information Sequence" },
    { 0x0054, 0x0073, (char *) "DS", (char *) "Time Slot Time" },
    { 0x0054, 0x0080, (char *) "US", (char *) "Slice Vector" },
    { 0x0054, 0x0081, (char *) "US", (char *) "Number of Slices" },
    { 0x0054, 0x0090, (char *) "US", (char *) "Angular View Vector" },
    { 0x0054, 0x0100, (char *) "US", (char *) "Time Slice Vector" },
    { 0x0054, 0x0101, (char *) "US", (char *) "Number Of Time Slices" },
    { 0x0054, 0x0200, (char *) "DS", (char *) "Start Angle" },
    { 0x0054, 0x0202, (char *) "CS", (char *) "Type of Detector Motion" },
    { 0x0054, 0x0210, (char *) "IS", (char *) "Trigger Vector" },
    { 0x0054, 0x0211, (char *) "US", (char *) "Number of Triggers in Phase" },
    { 0x0054, 0x0220, (char *) "SQ", (char *) "View Code Sequence" },
    { 0x0054, 0x0222, (char *) "SQ", (char *) "View Modifier Code Sequence" },
    { 0x0054, 0x0300, (char *) "SQ", (char *) "Radionuclide Code Sequence" },
    { 0x0054, 0x0302, (char *) "SQ", (char *) "Radiopharmaceutical Route Code Sequence" },
    { 0x0054, 0x0304, (char *) "SQ", (char *) "Radiopharmaceutical Code Sequence" },
    { 0x0054, 0x0306, (char *) "SQ", (char *) "Calibration Data Sequence" },
    { 0x0054, 0x0308, (char *) "US", (char *) "Energy Window Number" },
    { 0x0054, 0x0400, (char *) "SH", (char *) "Image ID" },
    { 0x0054, 0x0410, (char *) "SQ", (char *) "Patient Orientation Code Sequence" },
    { 0x0054, 0x0412, (char *) "SQ", (char *) "Patient Orientation Modifier Code Sequence" },
    { 0x0054, 0x0414, (char *) "SQ", (char *) "Patient Gantry Relationship Code Sequence" },
    { 0x0054, 0x1000, (char *) "CS", (char *) "Positron Emission Tomography Series Type" },
    { 0x0054, 0x1001, (char *) "CS", (char *) "Positron Emission Tomography Units" },
    { 0x0054, 0x1002, (char *) "CS", (char *) "Counts Source" },
    { 0x0054, 0x1004, (char *) "CS", (char *) "Reprojection Method" },
    { 0x0054, 0x1100, (char *) "CS", (char *) "Randoms Correction Method" },
    { 0x0054, 0x1101, (char *) "LO", (char *) "Attenuation Correction Method" },
    { 0x0054, 0x1102, (char *) "CS", (char *) "Decay Correction" },
    { 0x0054, 0x1103, (char *) "LO", (char *) "Reconstruction Method" },
    { 0x0054, 0x1104, (char *) "LO", (char *) "Detector Lines of Response Used" },
    { 0x0054, 0x1105, (char *) "LO", (char *) "Scatter Correction Method" },
    { 0x0054, 0x1200, (char *) "DS", (char *) "Axial Acceptance" },
    { 0x0054, 0x1201, (char *) "IS", (char *) "Axial Mash" },
    { 0x0054, 0x1202, (char *) "IS", (char *) "Transverse Mash" },
    { 0x0054, 0x1203, (char *) "DS", (char *) "Detector Element Size" },
    { 0x0054, 0x1210, (char *) "DS", (char *) "Coincidence Window Width" },
    { 0x0054, 0x1220, (char *) "CS", (char *) "Secondary Counts Type" },
    { 0x0054, 0x1300, (char *) "DS", (char *) "Frame Reference Time" },
    { 0x0054, 0x1310, (char *) "IS", (char *) "Primary Prompts Counts Accumulated" },
    { 0x0054, 0x1311, (char *) "IS", (char *) "Secondary Counts Accumulated" },
    { 0x0054, 0x1320, (char *) "DS", (char *) "Slice Sensitivity Factor" },
    { 0x0054, 0x1321, (char *) "DS", (char *) "Decay Factor" },
    { 0x0054, 0x1322, (char *) "DS", (char *) "Dose Calibration Factor" },
    { 0x0054, 0x1323, (char *) "DS", (char *) "Scatter Fraction Factor" },
    { 0x0054, 0x1324, (char *) "DS", (char *) "Dead Time Factor" },
    { 0x0054, 0x1330, (char *) "US", (char *) "Image Index" },
    { 0x0054, 0x1400, (char *) "CS", (char *) "Counts Included" },
    { 0x0054, 0x1401, (char *) "CS", (char *) "Dead Time Correction Flag" },
    { 0x0055, 0x0046, (char *) "LT", (char *) "Current Ward" },
    { 0x0058, 0x0000, (char *) "SQ", (char *) "?" },
    { 0x0060, 0x3000, (char *) "SQ", (char *) "Histogram Sequence" },
    { 0x0060, 0x3002, (char *) "US", (char *) "Histogram Number of Bins" },
    { 0x0060, 0x3004, (char *) "xs", (char *) "Histogram First Bin Value" },
    { 0x0060, 0x3006, (char *) "xs", (char *) "Histogram Last Bin Value" },
    { 0x0060, 0x3008, (char *) "US", (char *) "Histogram Bin Width" },
    { 0x0060, 0x3010, (char *) "LO", (char *) "Histogram Explanation" },
    { 0x0060, 0x3020, (char *) "UL", (char *) "Histogram Data" },
    { 0x0070, 0x0001, (char *) "SQ", (char *) "Graphic Annotation Sequence" },
    { 0x0070, 0x0002, (char *) "CS", (char *) "Graphic Layer" },
    { 0x0070, 0x0003, (char *) "CS", (char *) "Bounding Box Annotation Units" },
    { 0x0070, 0x0004, (char *) "CS", (char *) "Anchor Point Annotation Units" },
    { 0x0070, 0x0005, (char *) "CS", (char *) "Graphic Annotation Units" },
    { 0x0070, 0x0006, (char *) "ST", (char *) "Unformatted Text Value" },
    { 0x0070, 0x0008, (char *) "SQ", (char *) "Text Object Sequence" },
    { 0x0070, 0x0009, (char *) "SQ", (char *) "Graphic Object Sequence" },
    { 0x0070, 0x0010, (char *) "FL", (char *) "Bounding Box TLHC" },
    { 0x0070, 0x0011, (char *) "FL", (char *) "Bounding Box BRHC" },
    { 0x0070, 0x0014, (char *) "FL", (char *) "Anchor Point" },
    { 0x0070, 0x0015, (char *) "CS", (char *) "Anchor Point Visibility" },
    { 0x0070, 0x0020, (char *) "US", (char *) "Graphic Dimensions" },
    { 0x0070, 0x0021, (char *) "US", (char *) "Number Of Graphic Points" },
    { 0x0070, 0x0022, (char *) "FL", (char *) "Graphic Data" },
    { 0x0070, 0x0023, (char *) "CS", (char *) "Graphic Type" },
    { 0x0070, 0x0024, (char *) "CS", (char *) "Graphic Filled" },
    { 0x0070, 0x0040, (char *) "IS", (char *) "Image Rotation" },
    { 0x0070, 0x0041, (char *) "CS", (char *) "Image Horizontal Flip" },
    { 0x0070, 0x0050, (char *) "US", (char *) "Displayed Area TLHC" },
    { 0x0070, 0x0051, (char *) "US", (char *) "Displayed Area BRHC" },
    { 0x0070, 0x0060, (char *) "SQ", (char *) "Graphic Layer Sequence" },
    { 0x0070, 0x0062, (char *) "IS", (char *) "Graphic Layer Order" },
    { 0x0070, 0x0066, (char *) "US", (char *) "Graphic Layer Recommended Display Value" },
    { 0x0070, 0x0068, (char *) "LO", (char *) "Graphic Layer Description" },
    { 0x0070, 0x0080, (char *) "CS", (char *) "Presentation Label" },
    { 0x0070, 0x0081, (char *) "LO", (char *) "Presentation Description" },
    { 0x0070, 0x0082, (char *) "DA", (char *) "Presentation Creation Date" },
    { 0x0070, 0x0083, (char *) "TM", (char *) "Presentation Creation Time" },
    { 0x0070, 0x0084, (char *) "PN", (char *) "Presentation Creator's Name" },
    { 0x0087, 0x0010, (char *) "CS", (char *) "Media Type" },
    { 0x0087, 0x0020, (char *) "CS", (char *) "Media Location" },
    { 0x0087, 0x0050, (char *) "IS", (char *) "Estimated Retrieve Time" },
    { 0x0088, 0x0000, (char *) "UL", (char *) "Storage Group Length" },
    { 0x0088, 0x0130, (char *) "SH", (char *) "Storage Media FileSet ID" },
    { 0x0088, 0x0140, (char *) "UI", (char *) "Storage Media FileSet UID" },
    { 0x0088, 0x0200, (char *) "SQ", (char *) "Icon Image Sequence" },
    { 0x0088, 0x0904, (char *) "LO", (char *) "Topic Title" },
    { 0x0088, 0x0906, (char *) "ST", (char *) "Topic Subject" },
    { 0x0088, 0x0910, (char *) "LO", (char *) "Topic Author" },
    { 0x0088, 0x0912, (char *) "LO", (char *) "Topic Key Words" },
    { 0x0095, 0x0001, (char *) "LT", (char *) "Examination Folder ID" },
    { 0x0095, 0x0004, (char *) "UL", (char *) "Folder Reported Status" },
    { 0x0095, 0x0005, (char *) "LT", (char *) "Folder Reporting Radiologist" },
    { 0x0095, 0x0007, (char *) "LT", (char *) "SIENET ISA PLA" },
    { 0x0099, 0x0002, (char *) "UL", (char *) "Data Object Attributes" },
    { 0x00e1, 0x0001, (char *) "US", (char *) "Data Dictionary Version" },
    { 0x00e1, 0x0014, (char *) "LT", (char *) "?" },
    { 0x00e1, 0x0022, (char *) "DS", (char *) "?" },
    { 0x00e1, 0x0023, (char *) "DS", (char *) "?" },
    { 0x00e1, 0x0024, (char *) "LT", (char *) "?" },
    { 0x00e1, 0x0025, (char *) "LT", (char *) "?" },
    { 0x00e1, 0x0040, (char *) "SH", (char *) "Offset From CT MR Images" },
    { 0x0193, 0x0002, (char *) "DS", (char *) "RIS Key" },
    { 0x0307, 0x0001, (char *) "UN", (char *) "RIS Worklist IMGEF" },
    { 0x0309, 0x0001, (char *) "UN", (char *) "RIS Report IMGEF" },
    { 0x0601, 0x0000, (char *) "SH", (char *) "Implementation Version" },
    { 0x0601, 0x0020, (char *) "DS", (char *) "Relative Table Position" },
    { 0x0601, 0x0021, (char *) "DS", (char *) "Relative Table Height" },
    { 0x0601, 0x0030, (char *) "SH", (char *) "Surview Direction" },
    { 0x0601, 0x0031, (char *) "DS", (char *) "Surview Length" },
    { 0x0601, 0x0050, (char *) "SH", (char *) "Image View Type" },
    { 0x0601, 0x0070, (char *) "DS", (char *) "Batch Number" },
    { 0x0601, 0x0071, (char *) "DS", (char *) "Batch Size" },
    { 0x0601, 0x0072, (char *) "DS", (char *) "Batch Slice Number" },
    { 0x1000, 0x0000, (char *) "xs", (char *) "?" },
    { 0x1000, 0x0001, (char *) "US", (char *) "Run Length Triplet" },
    { 0x1000, 0x0002, (char *) "US", (char *) "Huffman Table Size" },
    { 0x1000, 0x0003, (char *) "US", (char *) "Huffman Table Triplet" },
    { 0x1000, 0x0004, (char *) "US", (char *) "Shift Table Size" },
    { 0x1000, 0x0005, (char *) "US", (char *) "Shift Table Triplet" },
    { 0x1010, 0x0000, (char *) "xs", (char *) "?" },
    { 0x1369, 0x0000, (char *) "US", (char *) "?" },
    { 0x2000, 0x0000, (char *) "UL", (char *) "Film Session Group Length" },
    { 0x2000, 0x0010, (char *) "IS", (char *) "Number of Copies" },
    { 0x2000, 0x0020, (char *) "CS", (char *) "Print Priority" },
    { 0x2000, 0x0030, (char *) "CS", (char *) "Medium Type" },
    { 0x2000, 0x0040, (char *) "CS", (char *) "Film Destination" },
    { 0x2000, 0x0050, (char *) "LO", (char *) "Film Session Label" },
    { 0x2000, 0x0060, (char *) "IS", (char *) "Memory Allocation" },
    { 0x2000, 0x0500, (char *) "SQ", (char *) "Referenced Film Box Sequence" },
    { 0x2010, 0x0000, (char *) "UL", (char *) "Film Box Group Length" },
    { 0x2010, 0x0010, (char *) "ST", (char *) "Image Display Format" },
    { 0x2010, 0x0030, (char *) "CS", (char *) "Annotation Display Format ID" },
    { 0x2010, 0x0040, (char *) "CS", (char *) "Film Orientation" },
    { 0x2010, 0x0050, (char *) "CS", (char *) "Film Size ID" },
    { 0x2010, 0x0060, (char *) "CS", (char *) "Magnification Type" },
    { 0x2010, 0x0080, (char *) "CS", (char *) "Smoothing Type" },
    { 0x2010, 0x0100, (char *) "CS", (char *) "Border Density" },
    { 0x2010, 0x0110, (char *) "CS", (char *) "Empty Image Density" },
    { 0x2010, 0x0120, (char *) "US", (char *) "Min Density" },
    { 0x2010, 0x0130, (char *) "US", (char *) "Max Density" },
    { 0x2010, 0x0140, (char *) "CS", (char *) "Trim" },
    { 0x2010, 0x0150, (char *) "ST", (char *) "Configuration Information" },
    { 0x2010, 0x0500, (char *) "SQ", (char *) "Referenced Film Session Sequence" },
    { 0x2010, 0x0510, (char *) "SQ", (char *) "Referenced Image Box Sequence" },
    { 0x2010, 0x0520, (char *) "SQ", (char *) "Referenced Basic Annotation Box Sequence" },
    { 0x2020, 0x0000, (char *) "UL", (char *) "Image Box Group Length" },
    { 0x2020, 0x0010, (char *) "US", (char *) "Image Box Position" },
    { 0x2020, 0x0020, (char *) "CS", (char *) "Polarity" },
    { 0x2020, 0x0030, (char *) "DS", (char *) "Requested Image Size" },
    { 0x2020, 0x0110, (char *) "SQ", (char *) "Preformatted Grayscale Image Sequence" },
    { 0x2020, 0x0111, (char *) "SQ", (char *) "Preformatted Color Image Sequence" },
    { 0x2020, 0x0130, (char *) "SQ", (char *) "Referenced Image Overlay Box Sequence" },
    { 0x2020, 0x0140, (char *) "SQ", (char *) "Referenced VOI LUT Box Sequence" },
    { 0x2030, 0x0000, (char *) "UL", (char *) "Annotation Group Length" },
    { 0x2030, 0x0010, (char *) "US", (char *) "Annotation Position" },
    { 0x2030, 0x0020, (char *) "LO", (char *) "Text String" },
    { 0x2040, 0x0000, (char *) "UL", (char *) "Overlay Box Group Length" },
    { 0x2040, 0x0010, (char *) "SQ", (char *) "Referenced Overlay Plane Sequence" },
    { 0x2040, 0x0011, (char *) "US", (char *) "Referenced Overlay Plane Groups" },
    { 0x2040, 0x0060, (char *) "CS", (char *) "Overlay Magnification Type" },
    { 0x2040, 0x0070, (char *) "CS", (char *) "Overlay Smoothing Type" },
    { 0x2040, 0x0080, (char *) "CS", (char *) "Overlay Foreground Density" },
    { 0x2040, 0x0090, (char *) "CS", (char *) "Overlay Mode" },
    { 0x2040, 0x0100, (char *) "CS", (char *) "Threshold Density" },
    { 0x2040, 0x0500, (char *) "SQ", (char *) "Referenced Overlay Image Box Sequence" },
    { 0x2050, 0x0010, (char *) "SQ", (char *) "Presentation LUT Sequence" },
    { 0x2050, 0x0020, (char *) "CS", (char *) "Presentation LUT Shape" },
    { 0x2100, 0x0000, (char *) "UL", (char *) "Print Job Group Length" },
    { 0x2100, 0x0020, (char *) "CS", (char *) "Execution Status" },
    { 0x2100, 0x0030, (char *) "CS", (char *) "Execution Status Info" },
    { 0x2100, 0x0040, (char *) "DA", (char *) "Creation Date" },
    { 0x2100, 0x0050, (char *) "TM", (char *) "Creation Time" },
    { 0x2100, 0x0070, (char *) "AE", (char *) "Originator" },
    { 0x2100, 0x0500, (char *) "SQ", (char *) "Referenced Print Job Sequence" },
    { 0x2110, 0x0000, (char *) "UL", (char *) "Printer Group Length" },
    { 0x2110, 0x0010, (char *) "CS", (char *) "Printer Status" },
    { 0x2110, 0x0020, (char *) "CS", (char *) "Printer Status Info" },
    { 0x2110, 0x0030, (char *) "LO", (char *) "Printer Name" },
    { 0x2110, 0x0099, (char *) "SH", (char *) "Print Queue ID" },
    { 0x3002, 0x0002, (char *) "SH", (char *) "RT Image Label" },
    { 0x3002, 0x0003, (char *) "LO", (char *) "RT Image Name" },
    { 0x3002, 0x0004, (char *) "ST", (char *) "RT Image Description" },
    { 0x3002, 0x000a, (char *) "CS", (char *) "Reported Values Origin" },
    { 0x3002, 0x000c, (char *) "CS", (char *) "RT Image Plane" },
    { 0x3002, 0x000e, (char *) "DS", (char *) "X-Ray Image Receptor Angle" },
    { 0x3002, 0x0010, (char *) "DS", (char *) "RTImageOrientation" },
    { 0x3002, 0x0011, (char *) "DS", (char *) "Image Plane Pixel Spacing" },
    { 0x3002, 0x0012, (char *) "DS", (char *) "RT Image Position" },
    { 0x3002, 0x0020, (char *) "SH", (char *) "Radiation Machine Name" },
    { 0x3002, 0x0022, (char *) "DS", (char *) "Radiation Machine SAD" },
    { 0x3002, 0x0024, (char *) "DS", (char *) "Radiation Machine SSD" },
    { 0x3002, 0x0026, (char *) "DS", (char *) "RT Image SID" },
    { 0x3002, 0x0028, (char *) "DS", (char *) "Source to Reference Object Distance" },
    { 0x3002, 0x0029, (char *) "IS", (char *) "Fraction Number" },
    { 0x3002, 0x0030, (char *) "SQ", (char *) "Exposure Sequence" },
    { 0x3002, 0x0032, (char *) "DS", (char *) "Meterset Exposure" },
    { 0x3004, 0x0001, (char *) "CS", (char *) "DVH Type" },
    { 0x3004, 0x0002, (char *) "CS", (char *) "Dose Units" },
    { 0x3004, 0x0004, (char *) "CS", (char *) "Dose Type" },
    { 0x3004, 0x0006, (char *) "LO", (char *) "Dose Comment" },
    { 0x3004, 0x0008, (char *) "DS", (char *) "Normalization Point" },
    { 0x3004, 0x000a, (char *) "CS", (char *) "Dose Summation Type" },
    { 0x3004, 0x000c, (char *) "DS", (char *) "GridFrame Offset Vector" },
    { 0x3004, 0x000e, (char *) "DS", (char *) "Dose Grid Scaling" },
    { 0x3004, 0x0010, (char *) "SQ", (char *) "RT Dose ROI Sequence" },
    { 0x3004, 0x0012, (char *) "DS", (char *) "Dose Value" },
    { 0x3004, 0x0040, (char *) "DS", (char *) "DVH Normalization Point" },
    { 0x3004, 0x0042, (char *) "DS", (char *) "DVH Normalization Dose Value" },
    { 0x3004, 0x0050, (char *) "SQ", (char *) "DVH Sequence" },
    { 0x3004, 0x0052, (char *) "DS", (char *) "DVH Dose Scaling" },
    { 0x3004, 0x0054, (char *) "CS", (char *) "DVH Volume Units" },
    { 0x3004, 0x0056, (char *) "IS", (char *) "DVH Number of Bins" },
    { 0x3004, 0x0058, (char *) "DS", (char *) "DVH Data" },
    { 0x3004, 0x0060, (char *) "SQ", (char *) "DVH Referenced ROI Sequence" },
    { 0x3004, 0x0062, (char *) "CS", (char *) "DVH ROI Contribution Type" },
    { 0x3004, 0x0070, (char *) "DS", (char *) "DVH Minimum Dose" },
    { 0x3004, 0x0072, (char *) "DS", (char *) "DVH Maximum Dose" },
    { 0x3004, 0x0074, (char *) "DS", (char *) "DVH Mean Dose" },
    { 0x3006, 0x0002, (char *) "SH", (char *) "Structure Set Label" },
    { 0x3006, 0x0004, (char *) "LO", (char *) "Structure Set Name" },
    { 0x3006, 0x0006, (char *) "ST", (char *) "Structure Set Description" },
    { 0x3006, 0x0008, (char *) "DA", (char *) "Structure Set Date" },
    { 0x3006, 0x0009, (char *) "TM", (char *) "Structure Set Time" },
    { 0x3006, 0x0010, (char *) "SQ", (char *) "Referenced Frame of Reference Sequence" },
    { 0x3006, 0x0012, (char *) "SQ", (char *) "RT Referenced Study Sequence" },
    { 0x3006, 0x0014, (char *) "SQ", (char *) "RT Referenced Series Sequence" },
    { 0x3006, 0x0016, (char *) "SQ", (char *) "Contour Image Sequence" },
    { 0x3006, 0x0020, (char *) "SQ", (char *) "Structure Set ROI Sequence" },
    { 0x3006, 0x0022, (char *) "IS", (char *) "ROI Number" },
    { 0x3006, 0x0024, (char *) "UI", (char *) "Referenced Frame of Reference UID" },
    { 0x3006, 0x0026, (char *) "LO", (char *) "ROI Name" },
    { 0x3006, 0x0028, (char *) "ST", (char *) "ROI Description" },
    { 0x3006, 0x002a, (char *) "IS", (char *) "ROI Display Color" },
    { 0x3006, 0x002c, (char *) "DS", (char *) "ROI Volume" },
    { 0x3006, 0x0030, (char *) "SQ", (char *) "RT Related ROI Sequence" },
    { 0x3006, 0x0033, (char *) "CS", (char *) "RT ROI Relationship" },
    { 0x3006, 0x0036, (char *) "CS", (char *) "ROI Generation Algorithm" },
    { 0x3006, 0x0038, (char *) "LO", (char *) "ROI Generation Description" },
    { 0x3006, 0x0039, (char *) "SQ", (char *) "ROI Contour Sequence" },
    { 0x3006, 0x0040, (char *) "SQ", (char *) "Contour Sequence" },
    { 0x3006, 0x0042, (char *) "CS", (char *) "Contour Geometric Type" },
    { 0x3006, 0x0044, (char *) "DS", (char *) "Contour SlabT hickness" },
    { 0x3006, 0x0045, (char *) "DS", (char *) "Contour Offset Vector" },
    { 0x3006, 0x0046, (char *) "IS", (char *) "Number of Contour Points" },
    { 0x3006, 0x0050, (char *) "DS", (char *) "Contour Data" },
    { 0x3006, 0x0080, (char *) "SQ", (char *) "RT ROI Observations Sequence" },
    { 0x3006, 0x0082, (char *) "IS", (char *) "Observation Number" },
    { 0x3006, 0x0084, (char *) "IS", (char *) "Referenced ROI Number" },
    { 0x3006, 0x0085, (char *) "SH", (char *) "ROI Observation Label" },
    { 0x3006, 0x0086, (char *) "SQ", (char *) "RT ROI Identification Code Sequence" },
    { 0x3006, 0x0088, (char *) "ST", (char *) "ROI Observation Description" },
    { 0x3006, 0x00a0, (char *) "SQ", (char *) "Related RT ROI Observations Sequence" },
    { 0x3006, 0x00a4, (char *) "CS", (char *) "RT ROI Interpreted Type" },
    { 0x3006, 0x00a6, (char *) "PN", (char *) "ROI Interpreter" },
    { 0x3006, 0x00b0, (char *) "SQ", (char *) "ROI Physical Properties Sequence" },
    { 0x3006, 0x00b2, (char *) "CS", (char *) "ROI Physical Property" },
    { 0x3006, 0x00b4, (char *) "DS", (char *) "ROI Physical Property Value" },
    { 0x3006, 0x00c0, (char *) "SQ", (char *) "Frame of Reference Relationship Sequence" },
    { 0x3006, 0x00c2, (char *) "UI", (char *) "Related Frame of Reference UID" },
    { 0x3006, 0x00c4, (char *) "CS", (char *) "Frame of Reference Transformation Type" },
    { 0x3006, 0x00c6, (char *) "DS", (char *) "Frame of Reference Transformation Matrix" },
    { 0x3006, 0x00c8, (char *) "LO", (char *) "Frame of Reference Transformation Comment" },
    { 0x300a, 0x0002, (char *) "SH", (char *) "RT Plan Label" },
    { 0x300a, 0x0003, (char *) "LO", (char *) "RT Plan Name" },
    { 0x300a, 0x0004, (char *) "ST", (char *) "RT Plan Description" },
    { 0x300a, 0x0006, (char *) "DA", (char *) "RT Plan Date" },
    { 0x300a, 0x0007, (char *) "TM", (char *) "RT Plan Time" },
    { 0x300a, 0x0009, (char *) "LO", (char *) "Treatment Protocols" },
    { 0x300a, 0x000a, (char *) "CS", (char *) "Treatment Intent" },
    { 0x300a, 0x000b, (char *) "LO", (char *) "Treatment Sites" },
    { 0x300a, 0x000c, (char *) "CS", (char *) "RT Plan Geometry" },
    { 0x300a, 0x000e, (char *) "ST", (char *) "Prescription Description" },
    { 0x300a, 0x0010, (char *) "SQ", (char *) "Dose ReferenceSequence" },
    { 0x300a, 0x0012, (char *) "IS", (char *) "Dose ReferenceNumber" },
    { 0x300a, 0x0014, (char *) "CS", (char *) "Dose Reference Structure Type" },
    { 0x300a, 0x0016, (char *) "LO", (char *) "Dose ReferenceDescription" },
    { 0x300a, 0x0018, (char *) "DS", (char *) "Dose Reference Point Coordinates" },
    { 0x300a, 0x001a, (char *) "DS", (char *) "Nominal Prior Dose" },
    { 0x300a, 0x0020, (char *) "CS", (char *) "Dose Reference Type" },
    { 0x300a, 0x0021, (char *) "DS", (char *) "Constraint Weight" },
    { 0x300a, 0x0022, (char *) "DS", (char *) "Delivery Warning Dose" },
    { 0x300a, 0x0023, (char *) "DS", (char *) "Delivery Maximum Dose" },
    { 0x300a, 0x0025, (char *) "DS", (char *) "Target Minimum Dose" },
    { 0x300a, 0x0026, (char *) "DS", (char *) "Target Prescription Dose" },
    { 0x300a, 0x0027, (char *) "DS", (char *) "Target Maximum Dose" },
    { 0x300a, 0x0028, (char *) "DS", (char *) "Target Underdose Volume Fraction" },
    { 0x300a, 0x002a, (char *) "DS", (char *) "Organ at Risk Full-volume Dose" },
    { 0x300a, 0x002b, (char *) "DS", (char *) "Organ at Risk Limit Dose" },
    { 0x300a, 0x002c, (char *) "DS", (char *) "Organ at Risk Maximum Dose" },
    { 0x300a, 0x002d, (char *) "DS", (char *) "Organ at Risk Overdose Volume Fraction" },
    { 0x300a, 0x0040, (char *) "SQ", (char *) "Tolerance Table Sequence" },
    { 0x300a, 0x0042, (char *) "IS", (char *) "Tolerance Table Number" },
    { 0x300a, 0x0043, (char *) "SH", (char *) "Tolerance Table Label" },
    { 0x300a, 0x0044, (char *) "DS", (char *) "Gantry Angle Tolerance" },
    { 0x300a, 0x0046, (char *) "DS", (char *) "Beam Limiting Device Angle Tolerance" },
    { 0x300a, 0x0048, (char *) "SQ", (char *) "Beam Limiting Device Tolerance Sequence" },
    { 0x300a, 0x004a, (char *) "DS", (char *) "Beam Limiting Device Position Tolerance" },
    { 0x300a, 0x004c, (char *) "DS", (char *) "Patient Support Angle Tolerance" },
    { 0x300a, 0x004e, (char *) "DS", (char *) "Table Top Eccentric Angle Tolerance" },
    { 0x300a, 0x0051, (char *) "DS", (char *) "Table Top Vertical Position Tolerance" },
    { 0x300a, 0x0052, (char *) "DS", (char *) "Table Top Longitudinal Position Tolerance" },
    { 0x300a, 0x0053, (char *) "DS", (char *) "Table Top Lateral Position Tolerance" },
    { 0x300a, 0x0055, (char *) "CS", (char *) "RT Plan Relationship" },
    { 0x300a, 0x0070, (char *) "SQ", (char *) "Fraction Group Sequence" },
    { 0x300a, 0x0071, (char *) "IS", (char *) "Fraction Group Number" },
    { 0x300a, 0x0078, (char *) "IS", (char *) "Number of Fractions Planned" },
    { 0x300a, 0x0079, (char *) "IS", (char *) "Number of Fractions Per Day" },
    { 0x300a, 0x007a, (char *) "IS", (char *) "Repeat Fraction Cycle Length" },
    { 0x300a, 0x007b, (char *) "LT", (char *) "Fraction Pattern" },
    { 0x300a, 0x0080, (char *) "IS", (char *) "Number of Beams" },
    { 0x300a, 0x0082, (char *) "DS", (char *) "Beam Dose Specification Point" },
    { 0x300a, 0x0084, (char *) "DS", (char *) "Beam Dose" },
    { 0x300a, 0x0086, (char *) "DS", (char *) "Beam Meterset" },
    { 0x300a, 0x00a0, (char *) "IS", (char *) "Number of Brachy Application Setups" },
    { 0x300a, 0x00a2, (char *) "DS", (char *) "Brachy Application Setup Dose Specification Point" },
    { 0x300a, 0x00a4, (char *) "DS", (char *) "Brachy Application Setup Dose" },
    { 0x300a, 0x00b0, (char *) "SQ", (char *) "Beam Sequence" },
    { 0x300a, 0x00b2, (char *) "SH", (char *) "Treatment Machine Name " },
    { 0x300a, 0x00b3, (char *) "CS", (char *) "Primary Dosimeter Unit" },
    { 0x300a, 0x00b4, (char *) "DS", (char *) "Source-Axis Distance" },
    { 0x300a, 0x00b6, (char *) "SQ", (char *) "Beam Limiting Device Sequence" },
    { 0x300a, 0x00b8, (char *) "CS", (char *) "RT Beam Limiting Device Type" },
    { 0x300a, 0x00ba, (char *) "DS", (char *) "Source to Beam Limiting Device Distance" },
    { 0x300a, 0x00bc, (char *) "IS", (char *) "Number of Leaf/Jaw Pairs" },
    { 0x300a, 0x00be, (char *) "DS", (char *) "Leaf Position Boundaries" },
    { 0x300a, 0x00c0, (char *) "IS", (char *) "Beam Number" },
    { 0x300a, 0x00c2, (char *) "LO", (char *) "Beam Name" },
    { 0x300a, 0x00c3, (char *) "ST", (char *) "Beam Description" },
    { 0x300a, 0x00c4, (char *) "CS", (char *) "Beam Type" },
    { 0x300a, 0x00c6, (char *) "CS", (char *) "Radiation Type" },
    { 0x300a, 0x00c8, (char *) "IS", (char *) "Reference Image Number" },
    { 0x300a, 0x00ca, (char *) "SQ", (char *) "Planned Verification Image Sequence" },
    { 0x300a, 0x00cc, (char *) "LO", (char *) "Imaging Device Specific Acquisition Parameters" },
    { 0x300a, 0x00ce, (char *) "CS", (char *) "Treatment Delivery Type" },
    { 0x300a, 0x00d0, (char *) "IS", (char *) "Number of Wedges" },
    { 0x300a, 0x00d1, (char *) "SQ", (char *) "Wedge Sequence" },
    { 0x300a, 0x00d2, (char *) "IS", (char *) "Wedge Number" },
    { 0x300a, 0x00d3, (char *) "CS", (char *) "Wedge Type" },
    { 0x300a, 0x00d4, (char *) "SH", (char *) "Wedge ID" },
    { 0x300a, 0x00d5, (char *) "IS", (char *) "Wedge Angle" },
    { 0x300a, 0x00d6, (char *) "DS", (char *) "Wedge Factor" },
    { 0x300a, 0x00d8, (char *) "DS", (char *) "Wedge Orientation" },
    { 0x300a, 0x00da, (char *) "DS", (char *) "Source to Wedge Tray Distance" },
    { 0x300a, 0x00e0, (char *) "IS", (char *) "Number of Compensators" },
    { 0x300a, 0x00e1, (char *) "SH", (char *) "Material ID" },
    { 0x300a, 0x00e2, (char *) "DS", (char *) "Total Compensator Tray Factor" },
    { 0x300a, 0x00e3, (char *) "SQ", (char *) "Compensator Sequence" },
    { 0x300a, 0x00e4, (char *) "IS", (char *) "Compensator Number" },
    { 0x300a, 0x00e5, (char *) "SH", (char *) "Compensator ID" },
    { 0x300a, 0x00e6, (char *) "DS", (char *) "Source to Compensator Tray Distance" },
    { 0x300a, 0x00e7, (char *) "IS", (char *) "Compensator Rows" },
    { 0x300a, 0x00e8, (char *) "IS", (char *) "Compensator Columns" },
    { 0x300a, 0x00e9, (char *) "DS", (char *) "Compensator Pixel Spacing" },
    { 0x300a, 0x00ea, (char *) "DS", (char *) "Compensator Position" },
    { 0x300a, 0x00eb, (char *) "DS", (char *) "Compensator Transmission Data" },
    { 0x300a, 0x00ec, (char *) "DS", (char *) "Compensator Thickness Data" },
    { 0x300a, 0x00ed, (char *) "IS", (char *) "Number of Boli" },
    { 0x300a, 0x00f0, (char *) "IS", (char *) "Number of Blocks" },
    { 0x300a, 0x00f2, (char *) "DS", (char *) "Total Block Tray Factor" },
    { 0x300a, 0x00f4, (char *) "SQ", (char *) "Block Sequence" },
    { 0x300a, 0x00f5, (char *) "SH", (char *) "Block Tray ID" },
    { 0x300a, 0x00f6, (char *) "DS", (char *) "Source to Block Tray Distance" },
    { 0x300a, 0x00f8, (char *) "CS", (char *) "Block Type" },
    { 0x300a, 0x00fa, (char *) "CS", (char *) "Block Divergence" },
    { 0x300a, 0x00fc, (char *) "IS", (char *) "Block Number" },
    { 0x300a, 0x00fe, (char *) "LO", (char *) "Block Name" },
    { 0x300a, 0x0100, (char *) "DS", (char *) "Block Thickness" },
    { 0x300a, 0x0102, (char *) "DS", (char *) "Block Transmission" },
    { 0x300a, 0x0104, (char *) "IS", (char *) "Block Number of Points" },
    { 0x300a, 0x0106, (char *) "DS", (char *) "Block Data" },
    { 0x300a, 0x0107, (char *) "SQ", (char *) "Applicator Sequence" },
    { 0x300a, 0x0108, (char *) "SH", (char *) "Applicator ID" },
    { 0x300a, 0x0109, (char *) "CS", (char *) "Applicator Type" },
    { 0x300a, 0x010a, (char *) "LO", (char *) "Applicator Description" },
    { 0x300a, 0x010c, (char *) "DS", (char *) "Cumulative Dose Reference Coefficient" },
    { 0x300a, 0x010e, (char *) "DS", (char *) "Final Cumulative Meterset Weight" },
    { 0x300a, 0x0110, (char *) "IS", (char *) "Number of Control Points" },
    { 0x300a, 0x0111, (char *) "SQ", (char *) "Control Point Sequence" },
    { 0x300a, 0x0112, (char *) "IS", (char *) "Control Point Index" },
    { 0x300a, 0x0114, (char *) "DS", (char *) "Nominal Beam Energy" },
    { 0x300a, 0x0115, (char *) "DS", (char *) "Dose Rate Set" },
    { 0x300a, 0x0116, (char *) "SQ", (char *) "Wedge Position Sequence" },
    { 0x300a, 0x0118, (char *) "CS", (char *) "Wedge Position" },
    { 0x300a, 0x011a, (char *) "SQ", (char *) "Beam Limiting Device Position Sequence" },
    { 0x300a, 0x011c, (char *) "DS", (char *) "Leaf Jaw Positions" },
    { 0x300a, 0x011e, (char *) "DS", (char *) "Gantry Angle" },
    { 0x300a, 0x011f, (char *) "CS", (char *) "Gantry Rotation Direction" },
    { 0x300a, 0x0120, (char *) "DS", (char *) "Beam Limiting Device Angle" },
    { 0x300a, 0x0121, (char *) "CS", (char *) "Beam Limiting Device Rotation Direction" },
    { 0x300a, 0x0122, (char *) "DS", (char *) "Patient Support Angle" },
    { 0x300a, 0x0123, (char *) "CS", (char *) "Patient Support Rotation Direction" },
    { 0x300a, 0x0124, (char *) "DS", (char *) "Table Top Eccentric Axis Distance" },
    { 0x300a, 0x0125, (char *) "DS", (char *) "Table Top Eccentric Angle" },
    { 0x300a, 0x0126, (char *) "CS", (char *) "Table Top Eccentric Rotation Direction" },
    { 0x300a, 0x0128, (char *) "DS", (char *) "Table Top Vertical Position" },
    { 0x300a, 0x0129, (char *) "DS", (char *) "Table Top Longitudinal Position" },
    { 0x300a, 0x012a, (char *) "DS", (char *) "Table Top Lateral Position" },
    { 0x300a, 0x012c, (char *) "DS", (char *) "Isocenter Position" },
    { 0x300a, 0x012e, (char *) "DS", (char *) "Surface Entry Point" },
    { 0x300a, 0x0130, (char *) "DS", (char *) "Source to Surface Distance" },
    { 0x300a, 0x0134, (char *) "DS", (char *) "Cumulative Meterset Weight" },
    { 0x300a, 0x0180, (char *) "SQ", (char *) "Patient Setup Sequence" },
    { 0x300a, 0x0182, (char *) "IS", (char *) "Patient Setup Number" },
    { 0x300a, 0x0184, (char *) "LO", (char *) "Patient Additional Position" },
    { 0x300a, 0x0190, (char *) "SQ", (char *) "Fixation Device Sequence" },
    { 0x300a, 0x0192, (char *) "CS", (char *) "Fixation Device Type" },
    { 0x300a, 0x0194, (char *) "SH", (char *) "Fixation Device Label" },
    { 0x300a, 0x0196, (char *) "ST", (char *) "Fixation Device Description" },
    { 0x300a, 0x0198, (char *) "SH", (char *) "Fixation Device Position" },
    { 0x300a, 0x01a0, (char *) "SQ", (char *) "Shielding Device Sequence" },
    { 0x300a, 0x01a2, (char *) "CS", (char *) "Shielding Device Type" },
    { 0x300a, 0x01a4, (char *) "SH", (char *) "Shielding Device Label" },
    { 0x300a, 0x01a6, (char *) "ST", (char *) "Shielding Device Description" },
    { 0x300a, 0x01a8, (char *) "SH", (char *) "Shielding Device Position" },
    { 0x300a, 0x01b0, (char *) "CS", (char *) "Setup Technique" },
    { 0x300a, 0x01b2, (char *) "ST", (char *) "Setup TechniqueDescription" },
    { 0x300a, 0x01b4, (char *) "SQ", (char *) "Setup Device Sequence" },
    { 0x300a, 0x01b6, (char *) "CS", (char *) "Setup Device Type" },
    { 0x300a, 0x01b8, (char *) "SH", (char *) "Setup Device Label" },
    { 0x300a, 0x01ba, (char *) "ST", (char *) "Setup Device Description" },
    { 0x300a, 0x01bc, (char *) "DS", (char *) "Setup Device Parameter" },
    { 0x300a, 0x01d0, (char *) "ST", (char *) "Setup ReferenceDescription" },
    { 0x300a, 0x01d2, (char *) "DS", (char *) "Table Top Vertical Setup Displacement" },
    { 0x300a, 0x01d4, (char *) "DS", (char *) "Table Top Longitudinal Setup Displacement" },
    { 0x300a, 0x01d6, (char *) "DS", (char *) "Table Top Lateral Setup Displacement" },
    { 0x300a, 0x0200, (char *) "CS", (char *) "Brachy Treatment Technique" },
    { 0x300a, 0x0202, (char *) "CS", (char *) "Brachy Treatment Type" },
    { 0x300a, 0x0206, (char *) "SQ", (char *) "Treatment Machine Sequence" },
    { 0x300a, 0x0210, (char *) "SQ", (char *) "Source Sequence" },
    { 0x300a, 0x0212, (char *) "IS", (char *) "Source Number" },
    { 0x300a, 0x0214, (char *) "CS", (char *) "Source Type" },
    { 0x300a, 0x0216, (char *) "LO", (char *) "Source Manufacturer" },
    { 0x300a, 0x0218, (char *) "DS", (char *) "Active Source Diameter" },
    { 0x300a, 0x021a, (char *) "DS", (char *) "Active Source Length" },
    { 0x300a, 0x0222, (char *) "DS", (char *) "Source Encapsulation Nominal Thickness" },
    { 0x300a, 0x0224, (char *) "DS", (char *) "Source Encapsulation Nominal Transmission" },
    { 0x300a, 0x0226, (char *) "LO", (char *) "Source IsotopeName" },
    { 0x300a, 0x0228, (char *) "DS", (char *) "Source Isotope Half Life" },
    { 0x300a, 0x022a, (char *) "DS", (char *) "Reference Air Kerma Rate" },
    { 0x300a, 0x022c, (char *) "DA", (char *) "Air Kerma Rate Reference Date" },
    { 0x300a, 0x022e, (char *) "TM", (char *) "Air Kerma Rate Reference Time" },
    { 0x300a, 0x0230, (char *) "SQ", (char *) "Application Setup Sequence" },
    { 0x300a, 0x0232, (char *) "CS", (char *) "Application Setup Type" },
    { 0x300a, 0x0234, (char *) "IS", (char *) "Application Setup Number" },
    { 0x300a, 0x0236, (char *) "LO", (char *) "Application Setup Name" },
    { 0x300a, 0x0238, (char *) "LO", (char *) "Application Setup Manufacturer" },
    { 0x300a, 0x0240, (char *) "IS", (char *) "Template Number" },
    { 0x300a, 0x0242, (char *) "SH", (char *) "Template Type" },
    { 0x300a, 0x0244, (char *) "LO", (char *) "Template Name" },
    { 0x300a, 0x0250, (char *) "DS", (char *) "Total Reference Air Kerma" },
    { 0x300a, 0x0260, (char *) "SQ", (char *) "Brachy Accessory Device Sequence" },
    { 0x300a, 0x0262, (char *) "IS", (char *) "Brachy Accessory Device Number" },
    { 0x300a, 0x0263, (char *) "SH", (char *) "Brachy Accessory Device ID" },
    { 0x300a, 0x0264, (char *) "CS", (char *) "Brachy Accessory Device Type" },
    { 0x300a, 0x0266, (char *) "LO", (char *) "Brachy Accessory Device Name" },
    { 0x300a, 0x026a, (char *) "DS", (char *) "Brachy Accessory Device Nominal Thickness" },
    { 0x300a, 0x026c, (char *) "DS", (char *) "Brachy Accessory Device Nominal Transmission" },
    { 0x300a, 0x0280, (char *) "SQ", (char *) "Channel Sequence" },
    { 0x300a, 0x0282, (char *) "IS", (char *) "Channel Number" },
    { 0x300a, 0x0284, (char *) "DS", (char *) "Channel Length" },
    { 0x300a, 0x0286, (char *) "DS", (char *) "Channel Total Time" },
    { 0x300a, 0x0288, (char *) "CS", (char *) "Source Movement Type" },
    { 0x300a, 0x028a, (char *) "IS", (char *) "Number of Pulses" },
    { 0x300a, 0x028c, (char *) "DS", (char *) "Pulse Repetition Interval" },
    { 0x300a, 0x0290, (char *) "IS", (char *) "Source Applicator Number" },
    { 0x300a, 0x0291, (char *) "SH", (char *) "Source Applicator ID" },
    { 0x300a, 0x0292, (char *) "CS", (char *) "Source Applicator Type" },
    { 0x300a, 0x0294, (char *) "LO", (char *) "Source Applicator Name" },
    { 0x300a, 0x0296, (char *) "DS", (char *) "Source Applicator Length" },
    { 0x300a, 0x0298, (char *) "LO", (char *) "Source Applicator Manufacturer" },
    { 0x300a, 0x029c, (char *) "DS", (char *) "Source Applicator Wall Nominal Thickness" },
    { 0x300a, 0x029e, (char *) "DS", (char *) "Source Applicator Wall Nominal Transmission" },
    { 0x300a, 0x02a0, (char *) "DS", (char *) "Source Applicator Step Size" },
    { 0x300a, 0x02a2, (char *) "IS", (char *) "Transfer Tube Number" },
    { 0x300a, 0x02a4, (char *) "DS", (char *) "Transfer Tube Length" },
    { 0x300a, 0x02b0, (char *) "SQ", (char *) "Channel Shield Sequence" },
    { 0x300a, 0x02b2, (char *) "IS", (char *) "Channel Shield Number" },
    { 0x300a, 0x02b3, (char *) "SH", (char *) "Channel Shield ID" },
    { 0x300a, 0x02b4, (char *) "LO", (char *) "Channel Shield Name" },
    { 0x300a, 0x02b8, (char *) "DS", (char *) "Channel Shield Nominal Thickness" },
    { 0x300a, 0x02ba, (char *) "DS", (char *) "Channel Shield Nominal Transmission" },
    { 0x300a, 0x02c8, (char *) "DS", (char *) "Final Cumulative Time Weight" },
    { 0x300a, 0x02d0, (char *) "SQ", (char *) "Brachy Control Point Sequence" },
    { 0x300a, 0x02d2, (char *) "DS", (char *) "Control Point Relative Position" },
    { 0x300a, 0x02d4, (char *) "DS", (char *) "Control Point 3D Position" },
    { 0x300a, 0x02d6, (char *) "DS", (char *) "Cumulative Time Weight" },
    { 0x300c, 0x0002, (char *) "SQ", (char *) "Referenced RT Plan Sequence" },
    { 0x300c, 0x0004, (char *) "SQ", (char *) "Referenced Beam Sequence" },
    { 0x300c, 0x0006, (char *) "IS", (char *) "Referenced Beam Number" },
    { 0x300c, 0x0007, (char *) "IS", (char *) "Referenced Reference Image Number" },
    { 0x300c, 0x0008, (char *) "DS", (char *) "Start Cumulative Meterset Weight" },
    { 0x300c, 0x0009, (char *) "DS", (char *) "End Cumulative Meterset Weight" },
    { 0x300c, 0x000a, (char *) "SQ", (char *) "Referenced Brachy Application Setup Sequence" },
    { 0x300c, 0x000c, (char *) "IS", (char *) "Referenced Brachy Application Setup Number" },
    { 0x300c, 0x000e, (char *) "IS", (char *) "Referenced Source Number" },
    { 0x300c, 0x0020, (char *) "SQ", (char *) "Referenced Fraction Group Sequence" },
    { 0x300c, 0x0022, (char *) "IS", (char *) "Referenced Fraction Group Number" },
    { 0x300c, 0x0040, (char *) "SQ", (char *) "Referenced Verification Image Sequence" },
    { 0x300c, 0x0042, (char *) "SQ", (char *) "Referenced Reference Image Sequence" },
    { 0x300c, 0x0050, (char *) "SQ", (char *) "Referenced Dose Reference Sequence" },
    { 0x300c, 0x0051, (char *) "IS", (char *) "Referenced Dose Reference Number" },
    { 0x300c, 0x0055, (char *) "SQ", (char *) "Brachy Referenced Dose Reference Sequence" },
    { 0x300c, 0x0060, (char *) "SQ", (char *) "Referenced Structure Set Sequence" },
    { 0x300c, 0x006a, (char *) "IS", (char *) "Referenced Patient Setup Number" },
    { 0x300c, 0x0080, (char *) "SQ", (char *) "Referenced Dose Sequence" },
    { 0x300c, 0x00a0, (char *) "IS", (char *) "Referenced Tolerance Table Number" },
    { 0x300c, 0x00b0, (char *) "SQ", (char *) "Referenced Bolus Sequence" },
    { 0x300c, 0x00c0, (char *) "IS", (char *) "Referenced Wedge Number" },
    { 0x300c, 0x00d0, (char *) "IS", (char *) "Referenced Compensato rNumber" },
    { 0x300c, 0x00e0, (char *) "IS", (char *) "Referenced Block Number" },
    { 0x300c, 0x00f0, (char *) "IS", (char *) "Referenced Control Point" },
    { 0x300e, 0x0002, (char *) "CS", (char *) "Approval Status" },
    { 0x300e, 0x0004, (char *) "DA", (char *) "Review Date" },
    { 0x300e, 0x0005, (char *) "TM", (char *) "Review Time" },
    { 0x300e, 0x0008, (char *) "PN", (char *) "Reviewer Name" },
    { 0x4000, 0x0000, (char *) "UL", (char *) "Text Group Length" },
    { 0x4000, 0x0010, (char *) "LT", (char *) "Text Arbitrary" },
    { 0x4000, 0x4000, (char *) "LT", (char *) "Text Comments" },
    { 0x4008, 0x0000, (char *) "UL", (char *) "Results Group Length" },
    { 0x4008, 0x0040, (char *) "SH", (char *) "Results ID" },
    { 0x4008, 0x0042, (char *) "LO", (char *) "Results ID Issuer" },
    { 0x4008, 0x0050, (char *) "SQ", (char *) "Referenced Interpretation Sequence" },
    { 0x4008, 0x00ff, (char *) "CS", (char *) "Report Production Status" },
    { 0x4008, 0x0100, (char *) "DA", (char *) "Interpretation Recorded Date" },
    { 0x4008, 0x0101, (char *) "TM", (char *) "Interpretation Recorded Time" },
    { 0x4008, 0x0102, (char *) "PN", (char *) "Interpretation Recorder" },
    { 0x4008, 0x0103, (char *) "LO", (char *) "Reference to Recorded Sound" },
    { 0x4008, 0x0108, (char *) "DA", (char *) "Interpretation Transcription Date" },
    { 0x4008, 0x0109, (char *) "TM", (char *) "Interpretation Transcription Time" },
    { 0x4008, 0x010a, (char *) "PN", (char *) "Interpretation Transcriber" },
    { 0x4008, 0x010b, (char *) "ST", (char *) "Interpretation Text" },
    { 0x4008, 0x010c, (char *) "PN", (char *) "Interpretation Author" },
    { 0x4008, 0x0111, (char *) "SQ", (char *) "Interpretation Approver Sequence" },
    { 0x4008, 0x0112, (char *) "DA", (char *) "Interpretation Approval Date" },
    { 0x4008, 0x0113, (char *) "TM", (char *) "Interpretation Approval Time" },
    { 0x4008, 0x0114, (char *) "PN", (char *) "Physician Approving Interpretation" },
    { 0x4008, 0x0115, (char *) "LT", (char *) "Interpretation Diagnosis Description" },
    { 0x4008, 0x0117, (char *) "SQ", (char *) "InterpretationDiagnosis Code Sequence" },
    { 0x4008, 0x0118, (char *) "SQ", (char *) "Results Distribution List Sequence" },
    { 0x4008, 0x0119, (char *) "PN", (char *) "Distribution Name" },
    { 0x4008, 0x011a, (char *) "LO", (char *) "Distribution Address" },
    { 0x4008, 0x0200, (char *) "SH", (char *) "Interpretation ID" },
    { 0x4008, 0x0202, (char *) "LO", (char *) "Interpretation ID Issuer" },
    { 0x4008, 0x0210, (char *) "CS", (char *) "Interpretation Type ID" },
    { 0x4008, 0x0212, (char *) "CS", (char *) "Interpretation Status ID" },
    { 0x4008, 0x0300, (char *) "ST", (char *) "Impressions" },
    { 0x4008, 0x4000, (char *) "ST", (char *) "Results Comments" },
    { 0x4009, 0x0001, (char *) "LT", (char *) "Report ID" },
    { 0x4009, 0x0020, (char *) "LT", (char *) "Report Status" },
    { 0x4009, 0x0030, (char *) "DA", (char *) "Report Creation Date" },
    { 0x4009, 0x0070, (char *) "LT", (char *) "Report Approving Physician" },
    { 0x4009, 0x00e0, (char *) "LT", (char *) "Report Text" },
    { 0x4009, 0x00e1, (char *) "LT", (char *) "Report Author" },
    { 0x4009, 0x00e3, (char *) "LT", (char *) "Reporting Radiologist" },
    { 0x5000, 0x0000, (char *) "UL", (char *) "Curve Group Length" },
    { 0x5000, 0x0005, (char *) "US", (char *) "Curve Dimensions" },
    { 0x5000, 0x0010, (char *) "US", (char *) "Number of Points" },
    { 0x5000, 0x0020, (char *) "CS", (char *) "Type of Data" },
    { 0x5000, 0x0022, (char *) "LO", (char *) "Curve Description" },
    { 0x5000, 0x0030, (char *) "SH", (char *) "Axis Units" },
    { 0x5000, 0x0040, (char *) "SH", (char *) "Axis Labels" },
    { 0x5000, 0x0103, (char *) "US", (char *) "Data Value Representation" },
    { 0x5000, 0x0104, (char *) "US", (char *) "Minimum Coordinate Value" },
    { 0x5000, 0x0105, (char *) "US", (char *) "Maximum Coordinate Value" },
    { 0x5000, 0x0106, (char *) "SH", (char *) "Curve Range" },
    { 0x5000, 0x0110, (char *) "US", (char *) "Curve Data Descriptor" },
    { 0x5000, 0x0112, (char *) "US", (char *) "Coordinate Start Value" },
    { 0x5000, 0x0114, (char *) "US", (char *) "Coordinate Step Value" },
    { 0x5000, 0x1001, (char *) "CS", (char *) "Curve Activation Layer" },
    { 0x5000, 0x2000, (char *) "US", (char *) "Audio Type" },
    { 0x5000, 0x2002, (char *) "US", (char *) "Audio Sample Format" },
    { 0x5000, 0x2004, (char *) "US", (char *) "Number of Channels" },
    { 0x5000, 0x2006, (char *) "UL", (char *) "Number of Samples" },
    { 0x5000, 0x2008, (char *) "UL", (char *) "Sample Rate" },
    { 0x5000, 0x200a, (char *) "UL", (char *) "Total Time" },
    { 0x5000, 0x200c, (char *) "xs", (char *) "Audio Sample Data" },
    { 0x5000, 0x200e, (char *) "LT", (char *) "Audio Comments" },
    { 0x5000, 0x2500, (char *) "LO", (char *) "Curve Label" },
    { 0x5000, 0x2600, (char *) "SQ", (char *) "CurveReferenced Overlay Sequence" },
    { 0x5000, 0x2610, (char *) "US", (char *) "CurveReferenced Overlay Group" },
    { 0x5000, 0x3000, (char *) "OW", (char *) "Curve Data" },
    { 0x6000, 0x0000, (char *) "UL", (char *) "Overlay Group Length" },
    { 0x6000, 0x0001, (char *) "US", (char *) "Gray Palette Color Lookup Table Descriptor" },
    { 0x6000, 0x0002, (char *) "US", (char *) "Gray Palette Color Lookup Table Data" },
    { 0x6000, 0x0010, (char *) "US", (char *) "Overlay Rows" },
    { 0x6000, 0x0011, (char *) "US", (char *) "Overlay Columns" },
    { 0x6000, 0x0012, (char *) "US", (char *) "Overlay Planes" },
    { 0x6000, 0x0015, (char *) "IS", (char *) "Number of Frames in Overlay" },
    { 0x6000, 0x0022, (char *) "LO", (char *) "Overlay Description" },
    { 0x6000, 0x0040, (char *) "CS", (char *) "Overlay Type" },
    { 0x6000, 0x0045, (char *) "CS", (char *) "Overlay Subtype" },
    { 0x6000, 0x0050, (char *) "SS", (char *) "Overlay Origin" },
    { 0x6000, 0x0051, (char *) "US", (char *) "Image Frame Origin" },
    { 0x6000, 0x0052, (char *) "US", (char *) "Plane Origin" },
    { 0x6000, 0x0060, (char *) "LO", (char *) "Overlay Compression Code" },
    { 0x6000, 0x0061, (char *) "SH", (char *) "Overlay Compression Originator" },
    { 0x6000, 0x0062, (char *) "SH", (char *) "Overlay Compression Label" },
    { 0x6000, 0x0063, (char *) "SH", (char *) "Overlay Compression Description" },
    { 0x6000, 0x0066, (char *) "AT", (char *) "Overlay Compression Step Pointers" },
    { 0x6000, 0x0068, (char *) "US", (char *) "Overlay Repeat Interval" },
    { 0x6000, 0x0069, (char *) "US", (char *) "Overlay Bits Grouped" },
    { 0x6000, 0x0100, (char *) "US", (char *) "Overlay Bits Allocated" },
    { 0x6000, 0x0102, (char *) "US", (char *) "Overlay Bit Position" },
    { 0x6000, 0x0110, (char *) "LO", (char *) "Overlay Format" },
    { 0x6000, 0x0200, (char *) "xs", (char *) "Overlay Location" },
    { 0x6000, 0x0800, (char *) "LO", (char *) "Overlay Code Label" },
    { 0x6000, 0x0802, (char *) "US", (char *) "Overlay Number of Tables" },
    { 0x6000, 0x0803, (char *) "AT", (char *) "Overlay Code Table Location" },
    { 0x6000, 0x0804, (char *) "US", (char *) "Overlay Bits For Code Word" },
    { 0x6000, 0x1001, (char *) "CS", (char *) "Overlay Activation Layer" },
    { 0x6000, 0x1100, (char *) "US", (char *) "Overlay Descriptor - Gray" },
    { 0x6000, 0x1101, (char *) "US", (char *) "Overlay Descriptor - Red" },
    { 0x6000, 0x1102, (char *) "US", (char *) "Overlay Descriptor - Green" },
    { 0x6000, 0x1103, (char *) "US", (char *) "Overlay Descriptor - Blue" },
    { 0x6000, 0x1200, (char *) "US", (char *) "Overlays - Gray" },
    { 0x6000, 0x1201, (char *) "US", (char *) "Overlays - Red" },
    { 0x6000, 0x1202, (char *) "US", (char *) "Overlays - Green" },
    { 0x6000, 0x1203, (char *) "US", (char *) "Overlays - Blue" },
    { 0x6000, 0x1301, (char *) "IS", (char *) "ROI Area" },
    { 0x6000, 0x1302, (char *) "DS", (char *) "ROI Mean" },
    { 0x6000, 0x1303, (char *) "DS", (char *) "ROI Standard Deviation" },
    { 0x6000, 0x1500, (char *) "LO", (char *) "Overlay Label" },
    { 0x6000, 0x3000, (char *) "OW", (char *) "Overlay Data" },
    { 0x6000, 0x4000, (char *) "LT", (char *) "Overlay Comments" },
    { 0x6001, 0x0000, (char *) "UN", (char *) "?" },
    { 0x6001, 0x0010, (char *) "LO", (char *) "?" },
    { 0x6001, 0x1010, (char *) "xs", (char *) "?" },
    { 0x6001, 0x1030, (char *) "xs", (char *) "?" },
    { 0x6021, 0x0000, (char *) "xs", (char *) "?" },
    { 0x6021, 0x0010, (char *) "xs", (char *) "?" },
    { 0x7001, 0x0010, (char *) "LT", (char *) "Dummy" },
    { 0x7003, 0x0010, (char *) "LT", (char *) "Info" },
    { 0x7005, 0x0010, (char *) "LT", (char *) "Dummy" },
    { 0x7000, 0x0004, (char *) "ST", (char *) "TextAnnotation" },
    { 0x7000, 0x0005, (char *) "IS", (char *) "Box" },
    { 0x7000, 0x0007, (char *) "IS", (char *) "ArrowEnd" },
    { 0x7fe0, 0x0000, (char *) "UL", (char *) "Pixel Data Group Length" },
    { 0x7fe0, 0x0010, (char *) "xs", (char *) "Pixel Data" },
    { 0x7fe0, 0x0020, (char *) "OW", (char *) "Coefficients SDVN" },
    { 0x7fe0, 0x0030, (char *) "OW", (char *) "Coefficients SDHN" },
    { 0x7fe0, 0x0040, (char *) "OW", (char *) "Coefficients SDDN" },
    { 0x7fe1, 0x0010, (char *) "xs", (char *) "Pixel Data" },
    { 0x7f00, 0x0000, (char *) "UL", (char *) "Variable Pixel Data Group Length" },
    { 0x7f00, 0x0010, (char *) "xs", (char *) "Variable Pixel Data" },
    { 0x7f00, 0x0011, (char *) "US", (char *) "Variable Next Data Group" },
    { 0x7f00, 0x0020, (char *) "OW", (char *) "Variable Coefficients SDVN" },
    { 0x7f00, 0x0030, (char *) "OW", (char *) "Variable Coefficients SDHN" },
    { 0x7f00, 0x0040, (char *) "OW", (char *) "Variable Coefficients SDDN" },
    { 0x7fe1, 0x0000, (char *) "OB", (char *) "Binary Data" },
    { 0x7fe3, 0x0000, (char *) "LT", (char *) "Image Graphics Format Code" },
    { 0x7fe3, 0x0010, (char *) "OB", (char *) "Image Graphics" },
    { 0x7fe3, 0x0020, (char *) "OB", (char *) "Image Graphics Dummy" },
    { 0x7ff1, 0x0001, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x0002, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x0003, (char *) "xs", (char *) "?" },
    { 0x7ff1, 0x0004, (char *) "IS", (char *) "?" },
    { 0x7ff1, 0x0005, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x0007, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x0008, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x0009, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x000a, (char *) "LT", (char *) "?" },
    { 0x7ff1, 0x000b, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x000c, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x000d, (char *) "US", (char *) "?" },
    { 0x7ff1, 0x0010, (char *) "US", (char *) "?" },
    { 0xfffc, 0xfffc, (char *) "OB", (char *) "Data Set Trailing Padding" },
    { 0xfffe, 0xe000, (char *) "!!", (char *) "Item" },
    { 0xfffe, 0xe00d, (char *) "!!", (char *) "Item Delimitation Item" },
    { 0xfffe, 0xe0dd, (char *) "!!", (char *) "Sequence Delimitation Item" },
    { 0xffff, 0xffff, (char *) "xs", (char *) "" }
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
static unsigned int IsDCM(const unsigned char *magick,const size_t length)
{
  if (length < 132)
    return(False);
  if (LocaleNCompare((char *) (magick+128),"DICM",4) == 0)
    return(True);
  return(False);
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
    explicit_vr[MaxTextExtent],
    implicit_vr[MaxTextExtent],
    magick[MaxTextExtent],
    photometric[MaxTextExtent],
    transfer_syntax[MaxTextExtent];

  Image
    *image;

  int
    blue,
    element,
    green,
    group,
    red,
    scene;

  long
    datum,
    y;

  Quantum
    *scale;

  register IndexPacket
    *indexes;

  register long
    x;

  register PixelPacket
    *q;

  register long
    i;

  register unsigned char
    *p;

  size_t
    count,
    length;

  unsigned char
    *data;

  unsigned int
    mask,
    explicit_file,
    use_explicit;

  unsigned long
    bits_allocated,
    bytes_per_pixel,
    height,
    max_value,
    msb_first,
    number_scenes,
    quantum,
    samples_per_pixel,
    significant_bits,
    status,
    width;

  unsigned short
    *graymap,
    index;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  /*
    Read DCM preamble.
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
    Read DCM Medical image.
  */
  (void) strcpy(photometric,"MONOCHROME1 ");
  bits_allocated=8;
  bytes_per_pixel=1;
  data=NULL;
  element=0;
  explicit_vr[2]='\0';
  explicit_file=False;
  graymap=(unsigned short *) NULL;
  group=0;
  height=0;
  max_value=255;
  mask=0xffff;
  number_scenes=1;
  samples_per_pixel=1;
  significant_bits=0;
  *transfer_syntax='\0';
  use_explicit=False;
  width=0;
  while ((group != 0x7FE0) || (element != 0x0010))
  {
    /*
      Read a group.
    */
    image->offset=(long) TellBlob(image);
    group=ReadBlobLSBShort(image);
    element=ReadBlobLSBShort(image);
    quantum=0;
    if (EOFBlob(image))
       ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    /*
      Find corresponding VR for this group and element.
    */
    for (i=0; dicom_info[i].group < 0xffffU; i++)
      if ((group == dicom_info[i].group) &&
          (element == dicom_info[i].element))
        break;
    (void) strlcpy(implicit_vr,dicom_info[i].vr,MaxTextExtent);
    if ((count=ReadBlob(image,2,(char *) explicit_vr)) != 2)
       ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
    /*
      Check for "explicitness", but meta-file headers always explicit.
    */
    if (!explicit_file && (group != 0x0002))
      explicit_file=isupper((int) *explicit_vr) &&
        isupper((int) *(explicit_vr+1));
    use_explicit=((group == 0x0002) || explicit_file);
    if (use_explicit && (strcmp(implicit_vr,"xs") == 0))
      (void) strlcpy(implicit_vr,explicit_vr,MaxTextExtent);
    if (!use_explicit || (strcmp(implicit_vr,"!!") == 0))
      {
        (void) SeekBlob(image,(ExtendedSignedIntegralType) -2,SEEK_CUR);
        quantum=4;
      }
    else
      {
        /*
          Assume explicit type.
        */
        quantum=2;
        if ((strcmp(explicit_vr,"OB") == 0) ||
            (strcmp(explicit_vr,"UN") == 0) ||
            (strcmp(explicit_vr,"OW") == 0) || (strcmp(explicit_vr,"SQ") == 0))
          {
            (void) ReadBlobLSBShort(image);
            if (EOFBlob(image))
              ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
            quantum=4;
          }
      }
    datum=0;
    if (quantum == 4)
      {
        datum=(long) ReadBlobLSBLong(image);
        if (EOFBlob(image))
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    else if (quantum == 2)
      {
        datum=(long) ReadBlobLSBShort(image);
        if (EOFBlob(image))
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    quantum=0;
    length=1;
    if (datum != 0)
      {
        if ((strcmp(implicit_vr,"SS") == 0) ||
            (strcmp(implicit_vr,"US") == 0))
          quantum=2;
        else
          if ((strcmp(implicit_vr,"UL") == 0) ||
              (strcmp(implicit_vr,"SL") == 0) ||
              (strcmp(implicit_vr,"FL") == 0))
            quantum=4;
          else
      if (strcmp(implicit_vr,"FD") != 0)
        quantum=1;
      else
        quantum=8;
      if (datum != -1)
        length=(size_t) datum/quantum;
      else
        {
          /*
            Sequence and item of undefined length.
          */
          quantum=0;
          length=0;
        }
      }
    if (image_info->verbose)
      {
        /*
          Display Dicom info.
        */
        if (!use_explicit)
          explicit_vr[0]='\0';
        for (i=0; dicom_info[i].description != (char *) NULL; i++)
          if ((group == dicom_info[i].group) &&
              (element == dicom_info[i].element))
            break;
        (void) fprintf(stdout,"0x%04lX %4lu %.1024s-%.1024s (0x%04x,0x%04x)",
          image->offset,(unsigned long) length,implicit_vr,explicit_vr,group,
          element);
        if (dicom_info[i].description != (char *) NULL)
          (void) fprintf(stdout," %.1024s",dicom_info[i].description);
        (void) fprintf(stdout,": ");
      }
    if ((group == 0x7FE0) && (element == 0x0010))
      {
        if (image_info->verbose)
          (void) fprintf(stdout,"\n");
        break;
      }
    /*
      Allocate space and read an array.
    */
    data=(unsigned char *) NULL;
    if ((length == 1) && (quantum == 1))
      {
        if ((datum=ReadBlobByte(image)) == EOF)
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    else if ((length == 1) && (quantum == 2))
      {
        datum=ReadBlobLSBShort(image);
        if (EOFBlob(image))
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    else if ((length == 1) && (quantum == 4))
      {
        datum=(long) ReadBlobLSBLong(image);
        if (EOFBlob(image))
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
      }
    else if ((quantum != 0) && (length != 0))
      {
        size_t
          size;

        if (length > ((~0UL)/quantum))
          ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
        data=MagickAllocateArray(unsigned char *,(length+1),quantum);
        if (data == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
        size=quantum*length;
        if (ReadBlob(image,size,(char *) data) != size)
          ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
        data[size]=0;
      }
    switch (group)
    {
      case 0x0002:
      {
        switch (element)
        {
          case 0x0010:
          {
            /*
              Transfer Syntax.
            */
            (void) strlcpy(transfer_syntax,(char *) data,MaxTextExtent);
            if (strcmp(transfer_syntax,"1.2.840.10008.1.2.2") == 0)
              ThrowReaderException(CoderError,MSBByteOrderNotSupported,
                image);
            if (strcmp(transfer_syntax,"1.2.840.10008.1.2.5") == 0)
              ThrowReaderException(CoderError,RLECompressionNotSupported,image);
            break;
          }
          default:
            break;
        }
        break;
      }
      case 0x0008:
      {
        switch (element)
        {
          case 0x0020:
          {
            (void) SetImageAttribute(image,"StudyDate",(char *) data);
            break;
          }
          default:
            break;
        }
        break;
      }
      case 0x0010:
      {
        switch (element)
        {
          case 0x0010:
          {
            (void) SetImageAttribute(image,"PatientName",(char *) data);
            break;
          }
          default:
            break;
        }
        break;
      }
      case 0x0018:
      {
        switch (element)
        {
          case 0x1060:
          {
            (void) SetImageAttribute(image,"TriggerTime",(char *) data);
            break;
          }
          default:
            break;
        }
        break;
      }
      case 0x0019:
      {
        switch (element)
        {
          case 0x101e:
          {
            (void) SetImageAttribute(image,"FieldOfView",(char *) data);
            break;
          }
          default:
            break;
        }
        break;
      }
      case 0x0020:
      {
        switch (element)
        {
          case 0x0011:
          {
            (void) SetImageAttribute(image,"SeriesNumber",(char *) data);
            break;
          }
          case 0x0032:
          {
            (void) SetImageAttribute(image,"ImagePosition",(char *) data);
            break;
          }
          case 0x0037:
          {
            (void) SetImageAttribute(image,"ImageOrientation",(char *) data);
            break;
          }
          case 0x1041:
          {
            (void) SetImageAttribute(image,"SliceLocation",(char *) data);
            break;
          }
          default:
            break;
        }
        break;
      }
      case 0x0028:
      {
        switch (element)
        {
          case 0x0002:
          {
            /*
              Samples per pixel.
            */
            samples_per_pixel=datum;
            break;
          }
          case 0x0004:
          {
            /*
              Photometric interpretation.
            */
            for (i=0; i < (long) Min(length, MaxTextExtent-1); i++)
              photometric[i]=data[i];
            photometric[i]='\0';
            break;
          }
          case 0x0006:
          {
            /*
              Planar configuration.
            */
            if (datum == 1)
              image->interlace=PlaneInterlace;
            break;
          }
          case 0x0008:
          {
            /*
              Number of frames.
            */
            number_scenes=atoi((char *) data);
            break;
          }
          case 0x0010:
          {
            /*
              Image rows.
            */
            height=datum;
            break;
          }
          case 0x0011:
          {
            /*
              Image columns.
            */
            width=datum;
            break;
          }
          case 0x0100:
          {
            /*
              Bits allocated.
            */
            bits_allocated=datum;
            bytes_per_pixel=1;
            if (datum > 8)
              bytes_per_pixel=2;
            max_value=(1 << bits_allocated)-1;
            break;
          }
          case 0x0101:
          {
            /*
              Bits stored.
            */
            significant_bits=datum;
            bytes_per_pixel=1;
            if (significant_bits > 8)
              bytes_per_pixel=2;
            mask=(1 << significant_bits)-1;
            image->depth=Min(significant_bits,QuantumDepth);
            break;
          }
          case 0x0102:
          {
            /*
              High bit.
            */
            break;
          }
          case 0x0103:
          {
            /*
              Pixel representation.
            */
            break;
          }
          case 0x1200:
          case 0x3006:
          {
            unsigned long
              colors;

            /*
              Populate image colormap.
            */
            colors=length/bytes_per_pixel;
            datum=(long) colors;
            graymap=MagickAllocateArray(unsigned short *,
                                                 colors,sizeof(unsigned short));
            if (graymap == (unsigned short *) NULL)
              ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,
                image);
            for (i=0; i < (long) colors; i++)
              if (bytes_per_pixel == 1)
                graymap[i]=data[i];
              else
                graymap[i]=(unsigned short) ((short *) data)[i];
            break;
          }
          case 0x1201:
          case 0x1202:
          case 0x1203:
          {
            unsigned short
              index;

            /*
              Initialize colormap.
            */
            if (!AllocateImageColormap(image,length/2))
              ThrowReaderException(ResourceLimitError,UnableToCreateColormap,
                image);
            p=data;
            for (i=0; i < (long) image->colors; i++)
            {
              index=(*p | *(p+1) << 8);
              if (element == 0x1201)
                image->colormap[i].red=ScaleShortToQuantum(index);
              if (element == 0x1202)
                image->colormap[i].green=ScaleShortToQuantum(index);
              if (element == 0x1203)
                image->colormap[i].blue=ScaleShortToQuantum(index);
              p+=2;
            }
            break;
          }
        }
        break;
      }
      default:
        break;
    }
    if (image_info->verbose)
      {
        if (data == (unsigned char *) NULL)
          (void) fprintf(stdout,"%ld\n",datum);
        else
          {
            /*
              Display group data.
            */
            for (i=0; i < (long) Max(length,4); i++)
              if (!isprint(data[i]))
                break;
            if ((i != (long) length) && (length <= 4))
              {
                long
                  j;

                datum=0;
                for (j=(long) length-1; j >= 0; j--)
                  datum=256*datum+data[j];
                (void) fprintf(stdout,"%lu",datum);
              }
            else
              for (i=0; i < (long) length; i++)
                if (isprint(data[i]))
                  (void) fprintf(stdout,"%c",data[i]);
                else
                  (void) fprintf(stdout,"%c",'.');
            (void) fprintf(stdout,"\n");
          }
      }
    MagickFreeMemory(data);
  }
  if ((width == 0) || (height == 0))
    ThrowReaderException(CorruptImageError,ImproperImageHeader,image);
  if ((strcmp(transfer_syntax,"1.2.840.10008.1.2.4.50") == 0) ||
      (strcmp(transfer_syntax,"1.2.840.10008.1.2.4.70") == 0))
    {
      char
        filename[MaxTextExtent];

      FILE
        *file;

      int
        c;

      ImageInfo
        *clone_info;

      unsigned char
        magick[MaxTextExtent];

      /*
        Handle 2.4.50 lossy JPEG and 2.4.70 lossless JPEG.
      */
      file=AcquireTemporaryFileStream(filename,BinaryFileIOMode);
      if (file == (FILE *) NULL)
        ThrowReaderTemporaryFileException(filename);
      (void) memset(magick,0,sizeof(magick));
      while ((c=ReadBlobByte(image)) != EOF)
      {
        magick[0]=magick[1];
        magick[1]=magick[2];
        magick[2]=(unsigned char) c;
        if (memcmp(magick,"\377\330\377",3) == 0)
          break;
      }
      (void) fwrite(magick,1,3,file);
      c=ReadBlobByte(image);
      while (c != EOF)
      {
        (void) fputc(c,file);
        c=ReadBlobByte(image);
      }
      (void) fclose(file);
      DestroyImage(image);
      clone_info=CloneImageInfo(image_info);
      clone_info->blob=(void *) NULL;
      clone_info->length=0;
      FormatString(clone_info->filename,"jpeg:%.1024s",filename);
      image=ReadImage(clone_info,exception);
      (void) LiberateTemporaryFile(filename);
      DestroyImageInfo(clone_info);
      return(image);
    }
  scale=(Quantum *) NULL;
  if (max_value > MaxRGB)
    {
      /*
        Compute pixel scaling table.
      */
      scale=MagickAllocateArray(Quantum *,(max_value+1),sizeof(Quantum));
      if (scale == (Quantum *) NULL)
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
      for (i=0; i <= (long) max_value; i++)
        scale[i]=(Quantum) (((double) MaxRGB*i)/max_value+0.5);
    }
  msb_first=strcmp(transfer_syntax,"1.2.840.10008.1.2.2") == 0;
  for (scene=0; scene < (long) number_scenes; scene++)
  {
    /*
      Initialize image structure.
    */
    image->columns=width;
    image->rows=height;
    if ((image->colormap == (PixelPacket *) NULL) && (samples_per_pixel == 1))
      if (!AllocateImageColormap(image,max_value+1))
        ThrowReaderException(ResourceLimitError,MemoryAllocationFailed,image);
    if (image_info->ping)
      break;
    if ((samples_per_pixel > 1) && (image->interlace == PlaneInterlace))
      {
        /*
          Convert Planar RGB DCM Medical image to pixel packets.
        */
        for (i=0; i < (long) samples_per_pixel; i++)
        {
          for (y=0; y < (long) image->rows; y++)
          {
            q=GetImagePixels(image,0,y,image->columns,1);
            if (q == (PixelPacket *) NULL)
              {
                status=MagickFail;
                break;
              }
            for (x=0; x < (long) image->columns; x++)
            {
              switch ((int) i)
              {
                case 0: q->red=ScaleCharToQuantum(ReadBlobByte(image)); break;
                case 1: q->green=ScaleCharToQuantum(ReadBlobByte(image)); break;
                case 2: q->blue=ScaleCharToQuantum(ReadBlobByte(image)); break;
                case 3: q->opacity=(Quantum)
                  (MaxRGB-ScaleCharToQuantum(ReadBlobByte(image))); break;
                default: break;
              }
              if (EOFBlob(image))
                {
                  ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
                  status=MagickFail;
                  break;
                }
              q++;
            }
            if (!SyncImagePixels(image))
              {
                status=MagickFail;
                break;
              }
            if (image->previous == (Image *) NULL)
              if (QuantumTick(y,image->rows))
                if (!MagickMonitorFormatted(y,image->rows,exception,
                                            LoadImageText,image->filename))
                  {
                    status=MagickFail;
                    break;
                  }
            if (status == MagickFail)
              break;
          }
          if (status == MagickFail)
            break;
        }
      }
    else
      {
        unsigned char
          byte;

        /*
          Convert DCM Medical image to pixel packets.
        */
        red=0;
        green=0;
        blue=0;
        i=0;
        byte=0;
        for (y=0; y < (long) image->rows; y++)
        {
          q=SetImagePixels(image,0,y,image->columns,1);
          if (q == (PixelPacket *) NULL)
            {
              status=MagickFail;
              break;
            }
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
          {
            if (samples_per_pixel == 1)
              {
                if (bytes_per_pixel == 1)
                  index=ReadBlobByte(image);
                else
                  if (bits_allocated != 12)
                    {
                      if (msb_first)
                        index=ReadBlobMSBShort(image);
                      else
                        index=ReadBlobLSBShort(image);
                    }
                  else
                    {
                      if (i & 0x01)
                        index=(ReadBlobByte(image) << 8) | byte;
                      else
                        {
                          if (msb_first)
                            index=ReadBlobMSBShort(image);
                          else
                            index=ReadBlobLSBShort(image);
                          byte=index & 0x0f;
                          index>>=4;
                        }
                      i++;
                    }
                index&=mask;
                if (index > max_value)
                  index=(unsigned short) max_value;
                if (graymap != (unsigned short *) NULL)
                  index=graymap[index];
                index=(IndexPacket) (index);
                VerifyColormapIndex(image,index);
                indexes[x]=index;
                red=image->colormap[index].red;
                green=image->colormap[index].green;
                blue=image->colormap[index].blue;
              }
            else
              {
                if (bytes_per_pixel == 1)
                  {
                    red=ReadBlobByte(image);
                    green=ReadBlobByte(image);
                    blue=ReadBlobByte(image);
                  }
                else
                  {
                    if (msb_first)
                      {
                        red=ReadBlobMSBShort(image);
                        green=ReadBlobMSBShort(image);
                        blue=ReadBlobMSBShort(image);
                      }
                    else
                      {
                        red=ReadBlobLSBShort(image);
                        green=ReadBlobLSBShort(image);
                        blue=ReadBlobLSBShort(image);
                      }
                  }
                red&=mask;
                green&=mask;
                blue&=mask;
                if (scale != (Quantum *) NULL)
                  {
                    red=scale[red];
                    green=scale[green];
                    blue=scale[blue];
                  }
              }
            q->red=(Quantum) red;
            q->green=(Quantum) green;
            q->blue=(Quantum) blue;
            q++;
            if (EOFBlob(image))
              {
                ThrowReaderException(CorruptImageError,UnexpectedEndOfFile,image);
                status=MagickFail;
              }
          }
          if (!SyncImagePixels(image))
            {
              status=MagickFail;
              break;
            }
          if (image->previous == (Image *) NULL)
            if (QuantumTick(y,image->rows))
              if (!MagickMonitorFormatted(y,image->rows,exception,
                                          LoadImageText,image->filename))
                {
                  status=MagickFail;
                  break;
                }
        }
        if (status == MagickPass)
          if (image->storage_class == PseudoClass)
            {
              if (bytes_per_pixel == 2)
                (void) NormalizeImage(image);
            }
      }
    if (EOFBlob(image))
      {
        ThrowException(exception,CorruptImageError,UnexpectedEndOfFile,
          image->filename);
        status=MagickFail;
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->subrange != 0)
      if (image->scene >= (image_info->subimage+image_info->subrange-1))
        break;
    if (scene < (long) (number_scenes-1))
      {
        /*
          Allocate next image structure.
        */
        AllocateNextImage(image_info,image);
        if (image->next == (Image *) NULL)
          {
            DestroyImageList(image);
            return((Image *) NULL);
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
    Free scale resource.
  */
  if (scale != (Quantum *) NULL)
    MagickFreeMemory(scale);
  while (image->previous != (Image *) NULL)
    image=image->previous;
  CloseBlob(image);
  return(image);
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
