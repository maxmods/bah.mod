/*
% Copyright (C) 2008 - 2009 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to convert to and from the string form of enumeration values.
%
% Written by Bob Friesenhahn, June 2008.
%
*/

#include "magick/studio.h"
#include "magick/enum_strings.h"
#include "magick/utility.h"

/*
  BlobMode
*/
MagickExport const char *BlobModeToString(BlobMode blob_mode)
{
  const char
    *mode_string="?";

  switch (blob_mode)
  {
  case UndefinedBlobMode:
    mode_string="Undefined";
    break;
  case ReadBlobMode:
    mode_string="Read";
    break;
  case ReadBinaryBlobMode:
    mode_string="ReadBinary";
    break;
  case WriteBlobMode:
    mode_string="Write";
    break;
  case WriteBinaryBlobMode:
    mode_string="WriteBinary";
    break;
  }
  return mode_string;
}

/*
  ChannelType
*/
MagickExport const char *ChannelTypeToString(const ChannelType channel)
{
  const char
    *channel_type="?";

  switch (channel)
    {
    case UndefinedChannel:
      channel_type="undefined";
      break;
    case RedChannel:
      channel_type="red";
      break;
    case CyanChannel: 
      channel_type="cyan";
      break;
    case GreenChannel:
      channel_type="green";
      break;
    case MagentaChannel:
      channel_type="magenta";
      break;
    case BlueChannel:
      channel_type="blue";
      break;
    case YellowChannel:
      channel_type="yellow";
      break;
    case OpacityChannel:
      channel_type="opacity";
      break;
    case BlackChannel:
      channel_type="black";
      break;
    case MatteChannel:
      channel_type="matte";
      break;
    case AllChannels:
      channel_type="all";
      break;
    case GrayChannel:
      channel_type="gray";
      break;
    }

  return channel_type;
}
MagickExport ChannelType StringToChannelType(const char *option)
{
  ChannelType
    channel;

  channel=UndefinedChannel;
  if (LocaleCompare("Red",option) == 0)
    channel=RedChannel;
  else if (LocaleCompare("Cyan",option) == 0)
    channel=CyanChannel;
  else if (LocaleCompare("Green",option) == 0)
    channel=GreenChannel;
  else if (LocaleCompare("Magenta",option) == 0)
    channel=MagentaChannel;
  else if (LocaleCompare("Blue",option) == 0)
    channel=BlueChannel;
  else if (LocaleCompare("Yellow",option) == 0)
    channel=YellowChannel;
  else if (LocaleCompare("Opacity",option) == 0)
    channel=OpacityChannel;
  else if (LocaleCompare("Black",option) == 0)
    channel=BlackChannel;
  else if (LocaleCompare("Matte",option) == 0)
    channel=MatteChannel;
  else if (LocaleCompare("All",option) == 0)
    channel=AllChannels;
  else if ((LocaleCompare("Gray",option) == 0) ||
           (LocaleCompare("Intensity",option) == 0))
    channel=GrayChannel;

  return channel;
}

/*
  ClassType
*/
MagickExport const char* ClassTypeToString(const ClassType class_type)
{
  const char
    *log_class_type="?";

  switch (class_type)
    {
    case UndefinedClass:
      log_class_type="Undefined";
      break;
    case DirectClass:
      log_class_type="DirectClass";
      break;
    case PseudoClass:
      log_class_type="PseudoClass";
      break;
    }
  return log_class_type;
}

/*
  ColorspaceType
*/
MagickExport const char *ColorspaceTypeToString(const ColorspaceType colorspace)
{
  const char
    *colorspace_type = "?";
  
  switch (colorspace)
    {
    case UndefinedColorspace:
      colorspace_type="Undefined";
      break;
    case CineonLogRGBColorspace:
      colorspace_type="CineonLogRGB";
      break;
    case RGBColorspace:
      colorspace_type="RGB";
      break;
    case GRAYColorspace:
      colorspace_type="Gray";
      break;
    case Rec601LumaColorspace:
      colorspace_type="Rec601Luma";
      break;
    case Rec709LumaColorspace:
      colorspace_type="Rec709Luma";
      break;
    case TransparentColorspace:
      colorspace_type="Transparent";
      break;
    case OHTAColorspace:
      colorspace_type="OHTA";
      break;
    case XYZColorspace:
      colorspace_type="XYZ";
      break;
    case Rec601YCbCrColorspace:
      colorspace_type="Rec601YCbCr";
      break;
    case Rec709YCbCrColorspace:
      colorspace_type="Rec709YCbCr";
      break;
    case YCCColorspace:
      colorspace_type="PhotoCD YCC";
      break;
    case YIQColorspace:
      colorspace_type="YIQ";
      break;
    case YPbPrColorspace:
      colorspace_type="YPbPr";
      break;
    case YUVColorspace:
      colorspace_type="YUV";
      break;
    case CMYKColorspace:
      colorspace_type="CMYK";
      break;
    case sRGBColorspace:
      colorspace_type="PhotoCD sRGB";
      break;
    case HSLColorspace:
      colorspace_type="HSL";
      break;
    case HWBColorspace:
      colorspace_type="HWB";
      break;
    case LABColorspace:
      colorspace_type="LAB";
      break;
    }
  return colorspace_type;
}
MagickExport ColorspaceType StringToColorspaceType(const char *option)
{
  ColorspaceType
    colorspace;

  colorspace=UndefinedColorspace;

  if (LocaleCompare("cineonlog",option) == 0)
    colorspace=CineonLogRGBColorspace;
  else if (LocaleCompare("cmyk",option) == 0)
    colorspace=CMYKColorspace;
  else if (LocaleCompare("gray",option) == 0)
    colorspace=GRAYColorspace;
  else if (LocaleCompare("hsl",option) == 0)
    colorspace=HSLColorspace;
  else if (LocaleCompare("hwb",option) == 0)
    colorspace=HWBColorspace;
  else if (LocaleCompare("ohta",option) == 0)
    colorspace=OHTAColorspace;
  else if (LocaleCompare("rec601luma",option) == 0)
    colorspace=Rec601LumaColorspace;
  else if (LocaleCompare("rec709luma",option) == 0)
    colorspace=Rec709LumaColorspace;
  else if (LocaleCompare("rgb",option) == 0)
    colorspace=RGBColorspace;
  else if (LocaleCompare("srgb",option) == 0)
    colorspace=sRGBColorspace;
  else if (LocaleCompare("transparent",option) == 0)
    colorspace=TransparentColorspace;
  else if (LocaleCompare("xyz",option) == 0)
    colorspace=XYZColorspace;
  else if (LocaleCompare("ycbcr",option) == 0)
    colorspace=YCbCrColorspace;
  else if (LocaleCompare("rec601ycbcr",option) == 0)
    colorspace=Rec601YCbCrColorspace;
  else if (LocaleCompare("rec709ycbcr",option) == 0)
    colorspace=Rec709YCbCrColorspace;
  else if (LocaleCompare("ycc",option) == 0)
    colorspace=YCCColorspace;
  else if (LocaleCompare("yiq",option) == 0)
    colorspace=YIQColorspace;
  else if (LocaleCompare("ypbpr",option) == 0)
    colorspace=YPbPrColorspace;
  else if (LocaleCompare("yuv",option) == 0)
    colorspace=YUVColorspace;

  return colorspace;
}

/*
  CompositeOperator
*/
MagickExport const char *CompositeOperatorToString(const CompositeOperator composite_op)
{
  const char
    *composite_op_text = "?";

  switch (composite_op)
    {
    case UndefinedCompositeOp:
      composite_op_text = "Undefined";
      break;
    case OverCompositeOp:
      composite_op_text = "Over";
      break;
    case InCompositeOp:
      composite_op_text = "In";
      break;
    case OutCompositeOp:
      composite_op_text = "Out";
      break;
    case AtopCompositeOp:
      composite_op_text = "Atop";
      break;
    case XorCompositeOp:
      composite_op_text = "Xor";
      break;
    case PlusCompositeOp:
      composite_op_text = "Plus";
      break;
    case MinusCompositeOp:
      composite_op_text = "Minus";
      break;
    case AddCompositeOp:
      composite_op_text = "Add";
      break;
    case SubtractCompositeOp:
      composite_op_text = "Subtract";
      break;
    case DifferenceCompositeOp:
      composite_op_text = "Difference";
      break;
    case MultiplyCompositeOp:
      composite_op_text = "Multiply";
      break;
    case BumpmapCompositeOp:
      composite_op_text = "Bumpmap";
      break;
    case CopyCompositeOp:
      composite_op_text = "Copy";
      break;
    case CopyRedCompositeOp:
      composite_op_text = "CopyRed";
      break;
    case CopyGreenCompositeOp:
      composite_op_text = "CopyGreen";
      break;
    case CopyBlueCompositeOp:
      composite_op_text = "CopyBlue";
      break;
    case CopyOpacityCompositeOp:
      composite_op_text = "CopyOpacity";
      break;
    case ClearCompositeOp:
      composite_op_text = "Clear";
      break;
    case DissolveCompositeOp:
      composite_op_text = "Dissolve";
      break;
    case DisplaceCompositeOp:
      composite_op_text = "Displace";
      break;
    case ModulateCompositeOp:
      composite_op_text = "Modulate";
      break;
    case ThresholdCompositeOp:
      composite_op_text = "Threshold";
      break;
    case NoCompositeOp:
      composite_op_text = "No";
      break;
    case DarkenCompositeOp:
      composite_op_text = "Darken";
      break;
    case LightenCompositeOp:
      composite_op_text = "Lighten";
      break;
    case HueCompositeOp:
      composite_op_text = "Hue";
      break;
    case SaturateCompositeOp:
      composite_op_text = "Saturate";
      break;
    case ColorizeCompositeOp:
      composite_op_text = "Colorize";
      break;
    case LuminizeCompositeOp:
      composite_op_text = "Luminize";
      break;
    case ScreenCompositeOp:
      composite_op_text = "Screen";
      break;
    case OverlayCompositeOp:
      composite_op_text = "Overlay";
      break;
    case CopyCyanCompositeOp:
      composite_op_text = "CopyCyan";
      break;
    case CopyMagentaCompositeOp:
      composite_op_text = "CopyMagenta";
      break;
    case CopyYellowCompositeOp:
      composite_op_text = "CopyYellow";
      break;
    case CopyBlackCompositeOp:
      composite_op_text = "CopyBlack";
      break;
    case DivideCompositeOp:
      composite_op_text = "Divide";
      break;
    }

  return composite_op_text;
}
MagickExport CompositeOperator StringToCompositeOperator(const char *option)
{
  CompositeOperator
    composite_op;

  composite_op=UndefinedCompositeOp;
  if (LocaleCompare("Undefined",option) == 0)
    composite_op=UndefinedCompositeOp;
  else if (LocaleCompare("Over",option) == 0)
    composite_op=OverCompositeOp;
  else if (LocaleCompare("In",option) == 0)
    composite_op=InCompositeOp;
  else if (LocaleCompare("Out",option) == 0)
    composite_op=OutCompositeOp;
  else if (LocaleCompare("Atop",option) == 0)
    composite_op=AtopCompositeOp;
  else if (LocaleCompare("Xor",option) == 0)
    composite_op=XorCompositeOp;
  else if (LocaleCompare("Plus",option) == 0)
    composite_op=PlusCompositeOp;
  else if (LocaleCompare("Minus",option) == 0)
    composite_op=MinusCompositeOp;
  else if (LocaleCompare("Add",option) == 0)
    composite_op=AddCompositeOp;
  else if (LocaleCompare("Subtract",option) == 0)
    composite_op=SubtractCompositeOp;
  else if (LocaleCompare("Difference",option) == 0)
    composite_op=DifferenceCompositeOp;
  else if (LocaleCompare("Multiply",option) == 0)
    composite_op=MultiplyCompositeOp;
  else if (LocaleCompare("Bumpmap",option) == 0)
    composite_op=BumpmapCompositeOp;
  else if (LocaleCompare("Copy",option) == 0)
    composite_op=CopyCompositeOp;
  else if (LocaleCompare("CopyRed",option) == 0)
    composite_op=CopyRedCompositeOp;
  else if (LocaleCompare("CopyGreen",option) == 0)
    composite_op=CopyGreenCompositeOp;
  else if (LocaleCompare("CopyBlue",option) == 0)
    composite_op=CopyBlueCompositeOp;
  else if (LocaleCompare("CopyOpacity",option) == 0)
    composite_op=CopyOpacityCompositeOp;
  else if (LocaleCompare("Clear",option) == 0)
    composite_op=ClearCompositeOp;
  else if (LocaleCompare("Dissolve",option) == 0)
    composite_op=DissolveCompositeOp;
  else if (LocaleCompare("Displace",option) == 0)
    composite_op=DisplaceCompositeOp;
  else if (LocaleCompare("Modulate",option) == 0)
    composite_op=ModulateCompositeOp;
  else if (LocaleCompare("Threshold",option) == 0)
    composite_op=ThresholdCompositeOp;
  else if (LocaleCompare("No",option) == 0)
    composite_op=NoCompositeOp;
  else if (LocaleCompare("Darken",option) == 0)
    composite_op=DarkenCompositeOp;
  else if (LocaleCompare("Lighten",option) == 0)
    composite_op=LightenCompositeOp;
  else if (LocaleCompare("Hue",option) == 0)
    composite_op=HueCompositeOp;
  else if (LocaleCompare("Saturate",option) == 0)
    composite_op=SaturateCompositeOp;
  else if (LocaleCompare("Colorize",option) == 0)
    composite_op=ColorizeCompositeOp;
  else if (LocaleCompare("Luminize",option) == 0)
    composite_op=LuminizeCompositeOp;
  else if (LocaleCompare("Screen",option) == 0)
    composite_op=ScreenCompositeOp;
  else if (LocaleCompare("Overlay",option) == 0)
    composite_op=OverlayCompositeOp;
  else if (LocaleCompare("CopyCyan",option) == 0)
    composite_op=CopyCyanCompositeOp;
  else if (LocaleCompare("CopyMagenta",option) == 0)
    composite_op=CopyMagentaCompositeOp;
  else if (LocaleCompare("CopyYellow",option) == 0)
    composite_op=CopyYellowCompositeOp;
  else if (LocaleCompare("CopyBlack",option) == 0)
    composite_op=CopyBlackCompositeOp;
  else if (LocaleCompare("Divide",option) == 0)
    composite_op=DivideCompositeOp;

  return composite_op;
}

/*
  CompressionType
*/
MagickExport const char* CompressionTypeToString(const CompressionType compression_type)
{
  const char
    *compression_string="?";

  switch (compression_type)
    {
    case UndefinedCompression:
      compression_string="Undefined";
      break;
    case NoCompression:
      compression_string="No";
      break;
    case BZipCompression:
      compression_string="BZip";
      break;
    case FaxCompression:
      compression_string="Fax";
      break;
    case Group4Compression:
      compression_string="Group4";
      break;
    case JPEGCompression:
      compression_string="JPEG";
      break;
    case LosslessJPEGCompression:
      compression_string="Lossless JPEG";
      break;
    case LZWCompression:
      compression_string="LZW";
      break;
    case RLECompression:
      compression_string="RLE";
      break;
    case ZipCompression:
      compression_string="Zip";
      break;
    }
  return compression_string;
}
MagickExport CompressionType StringToCompressionType(const char *option)
{
  CompressionType
    compression_type = UndefinedCompression;

  if (LocaleCompare("None",option) == 0)
    compression_type=NoCompression;
  else if (LocaleCompare("BZip",option) == 0)
    compression_type=BZipCompression;
  else if (LocaleCompare("BZip2",option) == 0)
    compression_type=BZipCompression;
  else if (LocaleCompare("BZ2",option) == 0)
    compression_type=BZipCompression;
  else if (LocaleCompare("Fax",option) == 0)
    compression_type=FaxCompression;
  else if (LocaleCompare("Group4",option) == 0)
    compression_type=Group4Compression;
  else if (LocaleCompare("JPEG",option) == 0)
    compression_type=JPEGCompression;
  else if (LocaleCompare("Lossless",option) == 0)
    compression_type=LosslessJPEGCompression;
  else if (LocaleCompare("LZW",option) == 0)
    compression_type=LZWCompression;
  else if (LocaleCompare("RLE",option) == 0)
    compression_type=RLECompression;
  else if (LocaleCompare("Zip",option) == 0)
    compression_type=ZipCompression;
  else if (LocaleCompare("GZip",option) == 0)
    compression_type=ZipCompression;

  return compression_type;
}

/*
  ConfirmAccessMode
*/
MagickExport const char *ConfirmAccessModeToString(const ConfirmAccessMode access_mode)
{
  const char
    *mode_string="?";

  switch(access_mode)
    {
    case UndefinedConfirmAccessMode:
      mode_string="Undefined";
      break;
    case FileExecuteConfirmAccessMode:
      mode_string="Execute";
      break;
    case FileReadConfirmAccessMode:
      mode_string="Read";
      break;
    case FileWriteConfirmAccessMode:
      mode_string="Write";
      break;
    case URLGetFTPConfirmAccessMode:
      mode_string="FTP Get";
      break;
    case URLGetFileConfirmAccessMode:
      mode_string="FILE Get";
      break;
    case URLGetHTTPConfirmAccessMode:
      mode_string="HTTP Get";
      break;
    }

  return mode_string;
}

/*
  EndianType
*/
MagickExport const char *EndianTypeToString(const EndianType endian_type)
{
  const char
    *endian_string="?";

  switch (endian_type)
    {
    case UndefinedEndian:
      endian_string="Undefined";
      break;
    case LSBEndian:
      endian_string="LSB";
      break;
    case MSBEndian:
      endian_string="MSB";
      break;
    case NativeEndian:
      endian_string="Native";
      break;
    }

  return endian_string;
}
MagickExport EndianType StringToEndianType(const char *option)
{
  EndianType
    endian_type = UndefinedEndian;

  if (LocaleCompare("LSB",option) == 0)
    endian_type=LSBEndian;
  else if (LocaleCompare("MSB",option) == 0)
    endian_type=MSBEndian;
  else if (LocaleCompare("NATIVE",option) == 0)
    {
#if defined(WORDS_BIGENDIAN)
      endian_type=MSBEndian;
#else
      endian_type=LSBEndian;
#endif
    }

  return endian_type;
}

/*
  FilterTypes
*/
MagickExport FilterTypes StringToFilterTypes(const char *option)
{
  FilterTypes
    filter = UndefinedFilter;

  if (LocaleCompare("Point",option) == 0)
    filter=PointFilter;
  else if (LocaleCompare("Box",option) == 0)
    filter=BoxFilter;
  else if (LocaleCompare("Triangle",option) == 0)
    filter=TriangleFilter;
  else if (LocaleCompare("Hermite",option) == 0)
    filter=HermiteFilter;
  else if (LocaleCompare("Hanning",option) == 0)
    filter=HanningFilter;
  else if (LocaleCompare("Hamming",option) == 0)
    filter=HammingFilter;
  else if (LocaleCompare("Blackman",option) == 0)
    filter=BlackmanFilter;
  else if (LocaleCompare("Gaussian",option) == 0)
    filter=GaussianFilter;
  else if (LocaleCompare("Quadratic",option) == 0)
    filter=QuadraticFilter;
  else if (LocaleCompare("Cubic",option) == 0)
    filter=CubicFilter;
  else if (LocaleCompare("Catrom",option) == 0)
    filter=CatromFilter;
  else if (LocaleCompare("Mitchell",option) == 0)
    filter=MitchellFilter;
  else if (LocaleCompare("Lanczos",option) == 0)
    filter=LanczosFilter;
  else if (LocaleCompare("Bessel",option) == 0)
    filter=BesselFilter;
  else if (LocaleCompare("Sinc",option) == 0)
    filter=SincFilter;

  return filter;
}

/*
  GravityType
*/
MagickExport GravityType StringToGravityType(const char *option)
{
  GravityType
    gravity_type=ForgetGravity;

  if (LocaleCompare("Forget",option) == 0)
    gravity_type=ForgetGravity;
  else if (LocaleCompare("NorthWest",option) == 0)
    gravity_type=NorthWestGravity;
  else if (LocaleCompare("North",option) == 0)
    gravity_type=NorthGravity;
  else if (LocaleCompare("NorthEast",option) == 0)
    gravity_type=NorthEastGravity;
  else if (LocaleCompare("West",option) == 0)
    gravity_type=WestGravity;
  else if (LocaleCompare("Center",option) == 0)
    gravity_type=CenterGravity;
  else if (LocaleCompare("East",option) == 0)
    gravity_type=EastGravity;
  else if (LocaleCompare("SouthWest",option) == 0)
    gravity_type=SouthWestGravity;
  else if (LocaleCompare("South",option) == 0)
    gravity_type=SouthGravity;
  else if (LocaleCompare("SouthEast",option) == 0)
    gravity_type=SouthEastGravity;
  else if (LocaleCompare("Static",option) == 0)
    gravity_type=StaticGravity;

  return gravity_type;
}

/*
  HighlightStyle
*/
MagickExport const char *
HighlightStyleToString(const HighlightStyle difference_style)
{
  const char
    *style="?";

  switch(difference_style)
    {
    case UndefinedHighlightStyle:
      style="Undefined";
      break;
    case AssignHighlightStyle:
      style="Assign";
      break;
    case ThresholdHighlightStyle:
      style="Threshold";
      break;
    case TintHighlightStyle:
      style="Tint";
      break;
    case XorHighlightStyle:
      style="XOR";
    }

  return style;
}
MagickExport HighlightStyle
StringToHighlightStyle(const char *option)
{
  HighlightStyle
    style=UndefinedHighlightStyle;

  if (LocaleCompare("Assign",option) == 0)
    style=AssignHighlightStyle;
  else if (LocaleCompare("Threshold",option) == 0)
    style=ThresholdHighlightStyle;
  else if (LocaleCompare("Tint",option) == 0)
    style=TintHighlightStyle;
  else if (LocaleCompare("XOR",option) == 0)
    style=XorHighlightStyle;

  return style;
}

/*
  ImageType
*/
MagickExport const char *ImageTypeToString(const ImageType image_type)
{
  const char
    *p = "?";

  switch (image_type)
    {
    case UndefinedType:
      p="Undefined";
      break;
    case BilevelType:
      p="Bilevel";
      break;
    case GrayscaleType:
      p="Grayscale";
      break;
    case GrayscaleMatteType:
      p="GrayscaleMatte";
      break;
    case PaletteType:
      p="Palette";
      break;
    case PaletteMatteType:
      p="PaletteMatte";
      break;
    case TrueColorType:
      p="TrueColor";
      break;
    case TrueColorMatteType:
      p="TrueColorMatte";
      break;
    case ColorSeparationType:
      p="ColorSeparation";
      break;
    case ColorSeparationMatteType:
      p="ColorSeparationMatte";
      break;
    case OptimizeType:
      p="Optimize";
      break;
    }

  return p;
}
MagickExport ImageType StringToImageType(const char *option)
{
  ImageType
    image_type = UndefinedType;

  if (LocaleCompare("Bilevel",option) == 0)
    image_type=BilevelType;
  else if (LocaleCompare("Grayscale",option) == 0)
    image_type=GrayscaleType;
  else if (LocaleCompare("GrayscaleMatte",option) == 0)
    image_type=GrayscaleMatteType;
  else if (LocaleCompare("Palette",option) == 0)
    image_type=PaletteType;
  else if (LocaleCompare("PaletteMatte",option) == 0)
    image_type=PaletteMatteType;
  else if (LocaleCompare("TrueColor",option) == 0)
    image_type=TrueColorType;
  else if (LocaleCompare("TrueColorMatte",option) == 0)
    image_type=TrueColorMatteType;
  else if (LocaleCompare("ColorSeparation",option) == 0)
    image_type=ColorSeparationType;
  else if (LocaleCompare("ColorSeparationMatte",option) == 0)
    image_type=ColorSeparationMatteType;
  else if (LocaleCompare("Optimize",option) == 0)
    image_type=OptimizeType;

  return image_type;
}

/*
  InterlaceType
*/
MagickExport const char *InterlaceTypeToString(const InterlaceType interlace_type)
{
  const char
    *interlace_string = "?";

  switch (interlace_type)
    {
    case UndefinedInterlace:
      interlace_string = "Undefined";
      break;
    case NoInterlace:
      interlace_string = "No";
      break;
    case LineInterlace:
      interlace_string = "Line";
      break;
    case PlaneInterlace:
      interlace_string = "Plane";
      break;
    case PartitionInterlace:
      interlace_string = "Partition";
      break;
    }

  return interlace_string;
}
MagickExport InterlaceType StringToInterlaceType(const char *option)
{
  InterlaceType
    interlace_type = UndefinedInterlace;

  if (LocaleCompare("None",option) == 0)
    interlace_type=NoInterlace;
  else if (LocaleCompare("Line",option) == 0)
    interlace_type=LineInterlace;
  else if (LocaleCompare("Plane",option) == 0)
    interlace_type=PlaneInterlace;
  else if (LocaleCompare("Partition",option) == 0)
    interlace_type=PartitionInterlace;

  return interlace_type;
}

/*
  MapMode
*/
MagickExport const char *MapModeToString(MapMode map_mode)
{
  char
    *mode_string="?";

  switch (map_mode)
  {
  case ReadMode:
    mode_string="Read";
    break;
  case WriteMode:
    mode_string="Write";
    break;
  case IOMode:
    mode_string="IO";
    break;
  }
  return mode_string;
}

/*
  MetricType
*/
MagickExport const char *MetricTypeToString(MetricType metric)
{
  char
    *metric_string="Undefined";

  switch (metric)
    {
    case UndefinedMetric:
      metric_string="Undefined";
      break;
    case MeanAbsoluteErrorMetric:
      metric_string="MeanAbsoluteError";
      break;
    case MeanSquaredErrorMetric:
      metric_string="MeanSquaredError";
      break;
    case PeakAbsoluteErrorMetric:
      metric_string="PeakAbsoluteError";
      break;
    case PeakSignalToNoiseRatioMetric:
      metric_string="PeakSignalToNoiseRatio";
      break;
    case RootMeanSquaredErrorMetric:
      metric_string="RootMeanSquaredError";
      break;
    }

  return metric_string;
}
MagickExport MetricType StringToMetricType(const char *option)
{
  MetricType
    metric_type=UndefinedMetric;

  if ((LocaleCompare("MAE",option) == 0) ||
      (LocaleCompare("MeanAbsoluteError",option) == 0))
    metric_type=MeanAbsoluteErrorMetric;
  else if ((LocaleCompare("MSE",option) == 0) ||
           (LocaleCompare("MeanSquaredError",option) == 0))
    metric_type=MeanSquaredErrorMetric;
  else if ((LocaleCompare("PAE",option) == 0) ||
           (LocaleCompare("PeakAbsoluteError",option) == 0))
    metric_type=PeakAbsoluteErrorMetric;
  else if ((LocaleCompare("PSNR",option) == 0) ||
           (LocaleCompare("PeakSignalToNoiseRatio",option) == 0))
    metric_type=PeakSignalToNoiseRatioMetric;
  else if ((LocaleCompare("RMSE",option) == 0) ||
           (LocaleCompare("RootMeanSquaredError",option) == 0))
    metric_type=RootMeanSquaredErrorMetric;

  return metric_type;
}

/*
  OrientationType
*/
MagickExport const char *OrientationTypeToString(const OrientationType orientation_type)
{
  const char *
    orientation = "?";

  switch (orientation_type)
    {
    case UndefinedOrientation:
      orientation = "Unknown";
      break;
    case TopLeftOrientation:
      orientation = "TopLeft";
      break;
    case TopRightOrientation:
      orientation = "TopRight";
      break;
    case BottomRightOrientation:
      orientation = "BottomRight";
      break;
    case BottomLeftOrientation:
      orientation = "BottomLeft";
      break;
    case LeftTopOrientation:
      orientation = "LeftTop";
      break;
    case RightTopOrientation:
      orientation = "RightTop";
      break;
    case RightBottomOrientation:
      orientation = "RightBottom";
      break;
    case LeftBottomOrientation:
      orientation = "LeftBottom";
      break;
    }

  return orientation;
}

/*
  PreviewType
*/
MagickExport PreviewType StringToPreviewType(const char *option)
{
  PreviewType
    preview_type = UndefinedPreview;

  if (LocaleCompare("Rotate",option) == 0)
    preview_type=RotatePreview;
  else if (LocaleCompare("Shear",option) == 0)
    preview_type=ShearPreview;
  else if (LocaleCompare("Roll",option) == 0)
    preview_type=RollPreview;
  else if (LocaleCompare("Hue",option) == 0)
    preview_type=HuePreview;
  else if (LocaleCompare("Saturation",option) == 0)
    preview_type=SaturationPreview;
  else if (LocaleCompare("Brightness",option) == 0)
    preview_type=BrightnessPreview;
  else if (LocaleCompare("Gamma",option) == 0)
    preview_type=GammaPreview;
  else if (LocaleCompare("Spiff",option) == 0)
    preview_type=SpiffPreview;
  else if (LocaleCompare("Dull",option) == 0)
    preview_type=DullPreview;
  else if (LocaleCompare("Grayscale",option) == 0)
    preview_type=GrayscalePreview;
  else if (LocaleCompare("Quantize",option) == 0)
    preview_type=QuantizePreview;
  else if (LocaleCompare("Despeckle",option) == 0)
    preview_type=DespecklePreview;
  else if (LocaleCompare("ReduceNoise",option) == 0)
    preview_type=ReduceNoisePreview;
  else if (LocaleCompare("AddNoise",option) == 0)
    preview_type=AddNoisePreview;
  else if (LocaleCompare("Sharpen",option) == 0)
    preview_type=SharpenPreview;
  else if (LocaleCompare("Blur",option) == 0)
    preview_type=BlurPreview;
  else if (LocaleCompare("Threshold",option) == 0)
    preview_type=ThresholdPreview;
  else if (LocaleCompare("EdgeDetect",option) == 0)
    preview_type=EdgeDetectPreview;
  else if (LocaleCompare("Spread",option) == 0)
    preview_type=SpreadPreview;
  else if (LocaleCompare("Shade",option) == 0)
    preview_type=ShadePreview;
  else if (LocaleCompare("Raise",option) == 0)
    preview_type=RaisePreview;
  else if (LocaleCompare("Segment",option) == 0)
    preview_type=SegmentPreview;
  else if (LocaleCompare("Solarize",option) == 0)
    preview_type=SolarizePreview;
  else if (LocaleCompare("Swirl",option) == 0)
    preview_type=SwirlPreview;
  else if (LocaleCompare("Implode",option) == 0)
    preview_type=ImplodePreview;
  else if (LocaleCompare("Wave",option) == 0)
    preview_type=WavePreview;
  else if (LocaleCompare("OilPaint",option) == 0)
    preview_type=OilPaintPreview;
  else if (LocaleCompare("CharcoalDrawing",option) == 0)
    preview_type=CharcoalDrawingPreview;
  else if (LocaleCompare("JPEG",option) == 0)
    preview_type=JPEGPreview;

  return preview_type;
}


/*
  QuantumOperator
*/
MagickExport QuantumOperator StringToQuantumOperator(const char *option)
{
  QuantumOperator
    quantum_operator;

  quantum_operator=UndefinedQuantumOp;
  if (LocaleCompare("add",option) == 0)
    quantum_operator=AddQuantumOp;
  else if (LocaleCompare("and",option) == 0)
    quantum_operator=AndQuantumOp;
  else if ((LocaleCompare("assign",option) == 0) ||
           (LocaleCompare("Set",option) == 0))
    quantum_operator=AssignQuantumOp;
  else if (LocaleCompare("divide",option) == 0)
    quantum_operator=DivideQuantumOp;
  else if ((LocaleCompare("lshift",option) == 0) ||
           (LocaleCompare("LeftShift",option) == 0))
    quantum_operator=LShiftQuantumOp;
  else if (LocaleCompare("multiply",option) == 0)
    quantum_operator=MultiplyQuantumOp;
  else if (LocaleCompare("or",option) == 0)
    quantum_operator=OrQuantumOp;
  else if ((LocaleCompare("rshift",option) == 0) ||
           (LocaleCompare("RightShift",option) == 0))
    quantum_operator=RShiftQuantumOp;
  else if (LocaleCompare("subtract",option) == 0)
    quantum_operator=SubtractQuantumOp;
  else if (LocaleCompare("threshold",option) == 0)
    quantum_operator=ThresholdQuantumOp;
  else if ((LocaleCompare("threshold-black",option) == 0) ||
           (LocaleCompare("ThresholdBlack",option) == 0))
    quantum_operator=ThresholdBlackQuantumOp;
  else if ((LocaleCompare("threshold-white",option) == 0) ||
           (LocaleCompare("ThresholdWhite",option) == 0))
    quantum_operator=ThresholdWhiteQuantumOp;
  else if (LocaleCompare("xor",option) == 0)
    quantum_operator=XorQuantumOp;
  else if ((LocaleCompare("noise-gaussian",option) == 0) ||
           (LocaleCompare("GaussianNoise",option) == 0))
    quantum_operator=NoiseGaussianQuantumOp;
  else if ((LocaleCompare("noise-impulse",option) == 0) ||
           (LocaleCompare("ImpulseNoise",option) == 0))
    quantum_operator=NoiseImpulseQuantumOp;
  else if ((LocaleCompare("noise-laplacian",option) == 0) ||
           (LocaleCompare("LaplacianNoise",option) == 0))
    quantum_operator=NoiseLaplacianQuantumOp;
  else if ((LocaleCompare("noise-multiplicative",option) == 0) ||
           (LocaleCompare("MultiplicativeNoise",option) == 0))
    quantum_operator=NoiseMultiplicativeQuantumOp;
  else if ((LocaleCompare("noise-poisson",option) == 0) ||
           (LocaleCompare("PoissonNoise",option) == 0))
    quantum_operator=NoisePoissonQuantumOp;
  else if ((LocaleCompare("noise-uniform",option) == 0) ||
           (LocaleCompare("UniformNoise",option) == 0))
    quantum_operator=NoiseUniformQuantumOp;
  else if (LocaleCompare("negate",option) == 0)
    quantum_operator=NegateQuantumOp;
  else if (LocaleCompare("gamma",option) == 0)
    quantum_operator=GammaQuantumOp;
  else if (LocaleCompare("depth",option) == 0)
    quantum_operator=DepthQuantumOp;
  else if (LocaleCompare("log",option) == 0)
    quantum_operator=LogQuantumOp;
  else if (LocaleCompare("max",option) == 0)
    quantum_operator=MaxQuantumOp;
  else if (LocaleCompare("min",option) == 0)
    quantum_operator=MinQuantumOp;
  else if (LocaleCompare("pow",option) == 0)
    quantum_operator=PowQuantumOp;

  return quantum_operator;
}
MagickExport const char *QuantumOperatorToString(const QuantumOperator quantum_operator)
{
  const char
    *operator_text = "?";

  switch (quantum_operator)
    {
    case UndefinedQuantumOp:
      operator_text="undefined";
      break;
    case AddQuantumOp:
      operator_text="add";
      break;
    case AndQuantumOp:
      operator_text="and";
      break;
    case AssignQuantumOp:
      operator_text="assign";
      break;
    case DivideQuantumOp:
      operator_text="divide";
      break;
    case LShiftQuantumOp:
      operator_text="lshift";
      break;
    case MultiplyQuantumOp:
      operator_text="multiply";
      break;
    case OrQuantumOp:
      operator_text="or";
      break;
    case RShiftQuantumOp:
      operator_text="rshift";
      break;
    case SubtractQuantumOp:
      operator_text="subtract";
      break;
    case ThresholdQuantumOp:
      operator_text="threshold";
      break;
    case ThresholdBlackQuantumOp:
      operator_text="threshold-black";
      break;
    case ThresholdWhiteQuantumOp:
      operator_text="threshold-white";
      break;
    case XorQuantumOp:
      operator_text="xor";
      break;
    case NoiseGaussianQuantumOp:
      operator_text="noise-gaussian";
      break;
    case NoiseImpulseQuantumOp:
      operator_text="noise-impulse";
      break;
    case NoiseLaplacianQuantumOp:
      operator_text="noise-laplacian";
      break;
    case NoiseMultiplicativeQuantumOp:
      operator_text="noise-multiplicative";
      break;
    case NoisePoissonQuantumOp:
      operator_text="noise-poisson";
      break;
    case NoiseUniformQuantumOp:
      operator_text="noise-uniform";
      break;
    case NegateQuantumOp:
      operator_text="negate";
      break;
    case GammaQuantumOp:
      operator_text="gamma";
      break;
    case DepthQuantumOp:
      operator_text="depth";
      break;
    case LogQuantumOp:
      operator_text="log";
      break;
    case MaxQuantumOp:
      operator_text="max";
      break;
    case MinQuantumOp:
      operator_text="min";
      break;
    case PowQuantumOp:
      operator_text="pow";
      break;
    }

  return operator_text;
}

/*
  FilterTypes
*/
MagickExport const char *ResizeFilterToString(const FilterTypes filter)
{
  const char *
    filter_string = "?";

  switch (filter)
    {
    case UndefinedFilter:
      filter_string="Undefined";
      break;
    case PointFilter:
      filter_string="Point";
      break;
    case BoxFilter:
      filter_string="Box";
      break;
    case TriangleFilter:
      filter_string="Triangle";
      break;
    case HermiteFilter:
      filter_string="Hermite";
      break;
    case HanningFilter:
      filter_string="Hanning";
      break;
    case HammingFilter:
      filter_string="Hamming";
      break;
    case BlackmanFilter:
      filter_string="Blackman";
      break;
    case GaussianFilter:
      filter_string="Gaussian";
      break;
    case QuadraticFilter:
      filter_string="Quadratic";
      break;
    case CubicFilter:
      filter_string="Cubi";
      break;
    case CatromFilter:
      filter_string="Catrom";
      break;
    case MitchellFilter:
      filter_string="Mitchell";
      break;
    case LanczosFilter:
      filter_string="Lanczos";
      break;
    case BesselFilter:
      filter_string="Bessel";
      break;
    case SincFilter:
      filter_string="Sinc";
      break;
    }

  return filter_string;
}

/*
  ResourceType
*/
MagickExport ResourceType StringToResourceType(const char *option)
{
  ResourceType
    resource_type = UndefinedResource;

  if (LocaleCompare("Disk",option) == 0)
    resource_type=DiskResource;
  else if (LocaleCompare("File",option) == 0)
    resource_type=FileResource;
  else if (LocaleCompare("Files",option) == 0)
    resource_type=FileResource;
  else if (LocaleCompare("Map",option) == 0)
    resource_type=MapResource;
  else if (LocaleCompare("Memory",option) == 0)
    resource_type=MemoryResource;
  else if (LocaleCompare("Pixels",option) == 0)
    resource_type=PixelsResource;
  else if (LocaleCompare("Threads",option) == 0)
    resource_type=ThreadsResource;
  return resource_type;
}

/*
  StorageType
*/
MagickExport const char *StorageTypeToString(const StorageType storage_type)
{
    const char
    *p = "?";

    switch (storage_type)
      {
      case CharPixel:
        p="CharPixel";
        break;
      case ShortPixel:
        p="ShortPixel";
        break;
      case IntegerPixel:
        p="IntegerPixel";
        break;
      case LongPixel:
        p="LongPixel";
        break;
      case FloatPixel:
        p="FloatPixel";
        break;
      case DoublePixel:
        p="DoublePixel";
        break;
      }

    return p;
}

/*
  QuantumSampleType
*/
MagickExport const char *QuantumSampleTypeToString(const QuantumSampleType sample_type)
{
    const char
    *p = "?";

    switch (sample_type)
      {
      case UndefinedQuantumSampleType:
        p="UndefinedQuantumSampleType";
        break;
      case UnsignedQuantumSampleType:
        p="UnsignedQuantumSampleType";
        break;
      case FloatQuantumSampleType:
        p="FloatQuantumSampleType";
        break;
      }

    return p;
}

/*
  StretchType
*/
MagickExport const char *StretchTypeToString(StretchType stretch)
{
  switch(stretch)
  {
    case NormalStretch: return("normal");
    case UltraCondensedStretch: return("ultra-condensed");
    case ExtraCondensedStretch: return("extra-condensed");
    case CondensedStretch: return("condensed");
    case SemiCondensedStretch: return("semi-condensed");
    case SemiExpandedStretch: return("semi-expanded");
    case ExpandedStretch: return("expanded");
    case ExtraExpandedStretch: return("extra-expanded");
    case UltraExpandedStretch: return("ultra-expanded");
    case AnyStretch: return("any");
    default: break;
  }
  return("unknown");
}

/*
  StyleType
*/
MagickExport const char *StyleTypeToString(StyleType style)
{
  switch(style)
  {
    case NormalStyle: return("normal");
    case ItalicStyle: return("italic");
    case ObliqueStyle: return("oblique");
    case AnyStyle: return("any");
    default:  break;
  }
  return("unknown");
}

/*
  QuantumType
*/
MagickExport const char *QuantumTypeToString(const QuantumType quantum_type)
{
  const char
    *p = "?";

  switch (quantum_type)
    {
    case UndefinedQuantum:
      p="UndefinedQuantum";
      break;
    case IndexQuantum:
      p="IndexQuantum";
      break;
    case GrayQuantum:
      p="GrayQuantum";
      break;
    case IndexAlphaQuantum:
      p="IndexAlphaQuantum";
      break;
    case GrayAlphaQuantum:
      p="GrayAlphaQuantum";
      break;
    case RedQuantum:
      p="RedQuantum";
      break;
    case CyanQuantum:
      p="CyanQuantum";
      break;
    case GreenQuantum:
      p="GreenQuantum";
      break;
    case YellowQuantum:
      p="YellowQuantum";
      break;
    case BlueQuantum:
      p="BlueQuantum";
      break;
    case MagentaQuantum:
      p="MagentaQuantum";
      break;
    case AlphaQuantum:
      p="AlphaQuantum";
      break;
    case BlackQuantum:
      p="BlackQuantum";
      break;
    case RGBQuantum:
      p="RGBQuantum";
      break;
    case RGBAQuantum:
      p="RGBAQuantum";
      break;
    case CMYKQuantum:
      p="CMYKQuantum";
      break;
    case CMYKAQuantum:
      p="CMYKAQuantum";
      break;
    case CIEYQuantum:
      p="CIEYQuantum";
      break;
    case CIEXYZQuantum:
      p="CIEXYZQuantum";
      break;
    }

  return p;
}

/*
  VirtualPixelMethod
*/
MagickExport VirtualPixelMethod StringToVirtualPixelMethod(const char *option)
{
  VirtualPixelMethod
    virtual_pixel_method = UndefinedVirtualPixelMethod;
  
  if (LocaleCompare("Constant",option) == 0)
    virtual_pixel_method=ConstantVirtualPixelMethod;
  else if (LocaleCompare("Edge",option) == 0)
    virtual_pixel_method=EdgeVirtualPixelMethod;
  else if (LocaleCompare("Mirror",option) == 0)
    virtual_pixel_method=MirrorVirtualPixelMethod;
  else if (LocaleCompare("Tile",option) == 0)
    virtual_pixel_method=TileVirtualPixelMethod;

  return virtual_pixel_method;
}
