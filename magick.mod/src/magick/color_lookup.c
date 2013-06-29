/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Color Lookup Functions
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/color.h"
#include "magick/color_lookup.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/semaphore.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define ColorFilename  "colors.mgk"

/*
  Declare color map.
*/

static const char
  *BuiltInPath="[Built In]";

static const struct
{
  char
    *name;

  unsigned short
    compliance;

  unsigned char
    red,
    green,
    blue,
    opacity;
}
StaticColors[] =
{
#define COLOR(name,red,green,blue,opacity,compliance) {name,compliance,red,green,blue,opacity}
  COLOR("AliceBlue",240,248,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("AntiqueWhite",250,235,215,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("AntiqueWhite1",255,239,219,0,X11Compliance),
  COLOR("AntiqueWhite2",238,223,204,0,X11Compliance),
  COLOR("AntiqueWhite3",205,192,176,0,X11Compliance),
  COLOR("AntiqueWhite4",139,131,120,0,X11Compliance),
  COLOR("aqua",0,255,255,0,SVGCompliance),
  COLOR("aquamarine",127,255,212,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("aquamarine1",127,255,212,0,X11Compliance),
  COLOR("aquamarine2",118,238,198,0,X11Compliance),
  COLOR("aquamarine3",102,205,170,0,X11Compliance),
  COLOR("aquamarine4",69,139,116,0,X11Compliance),
  COLOR("azure",240,255,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("azure1",240,255,255,0,X11Compliance),
  COLOR("azure2",224,238,238,0,X11Compliance),
  COLOR("azure3",193,205,205,0,X11Compliance),
  COLOR("azure4",131,139,139,0,X11Compliance),
  COLOR("beige",245,245,220,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("bisque",255,228,196,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("bisque1",255,228,196,0,X11Compliance),
  COLOR("bisque2",238,213,183,0,X11Compliance),
  COLOR("bisque3",205,183,158,0,X11Compliance),
  COLOR("bisque4",139,125,107,0,X11Compliance),
  COLOR("black",0,0,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("BlanchedAlmond",255,235,205,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("blue",0,0,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("blue1",0,0,255,0,X11Compliance),
  COLOR("blue2",0,0,238,0,X11Compliance),
  COLOR("blue3",0,0,205,0,X11Compliance),
  COLOR("blue4",0,0,139,0,X11Compliance),
  COLOR("BlueViolet",138,43,226,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("brown",165,42,42,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("brown1",255,64,64,0,X11Compliance),
  COLOR("brown2",238,59,59,0,X11Compliance),
  COLOR("brown3",205,51,51,0,X11Compliance),
  COLOR("brown4",139,35,35,0,X11Compliance),
  COLOR("burlywood",222,184,135,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("burlywood1",255,211,155,0,X11Compliance),
  COLOR("burlywood2",238,197,145,0,X11Compliance),
  COLOR("burlywood3",205,170,125,0,X11Compliance),
  COLOR("burlywood4",139,115,85,0,X11Compliance),
  COLOR("cadet blue",95,158,160,0,X11Compliance),
  COLOR("CadetBlue",95,158,160,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("CadetBlue1",152,245,255,0,X11Compliance),
  COLOR("CadetBlue2",142,229,238,0,X11Compliance),
  COLOR("CadetBlue3",122,197,205,0,X11Compliance),
  COLOR("CadetBlue4",83,134,139,0,X11Compliance),
  COLOR("chartreuse",127,255,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("chartreuse1",127,255,0,0,X11Compliance),
  COLOR("chartreuse2",118,238,0,0,X11Compliance),
  COLOR("chartreuse3",102,205,0,0,X11Compliance),
  COLOR("chartreuse4",69,139,0,0,X11Compliance),
  COLOR("chocolate",210,105,30,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("chocolate1",255,127,36,0,X11Compliance),
  COLOR("chocolate2",238,118,33,0,X11Compliance),
  COLOR("chocolate3",205,102,29,0,X11Compliance),
  COLOR("chocolate4",139,69,19,0,X11Compliance),
  COLOR("coral",255,127,80,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("coral1",255,114,86,0,X11Compliance),
  COLOR("coral2",238,106,80,0,X11Compliance),
  COLOR("coral3",205,91,69,0,X11Compliance),
  COLOR("coral4",139,62,47,0,X11Compliance),
  COLOR("CornflowerBlue",100,149,237,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("cornsilk",255,248,220,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("cornsilk1",255,248,220,0,X11Compliance),
  COLOR("cornsilk2",238,232,205,0,X11Compliance),
  COLOR("cornsilk3",205,200,177,0,X11Compliance),
  COLOR("cornsilk4",139,136,120,0,X11Compliance),
  COLOR("crimson",220,20,60,0,SVGCompliance),
  COLOR("cyan",0,255,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("cyan1",0,255,255,0,X11Compliance),
  COLOR("cyan2",0,238,238,0,X11Compliance),
  COLOR("cyan3",0,205,205,0,X11Compliance),
  COLOR("cyan4",0,139,139,0,X11Compliance),
  COLOR("dark violet",148,0,211,0,X11Compliance),
  COLOR("DarkBlue",0,0,139,0,SVGCompliance|X11Compliance),
  COLOR("DarkCyan",0,139,139,0,SVGCompliance|X11Compliance),
  COLOR("DarkGoldenrod",184,134,11,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkGoldenrod1",255,185,15,0,X11Compliance),
  COLOR("DarkGoldenrod2",238,173,14,0,X11Compliance),
  COLOR("DarkGoldenrod3",205,149,12,0,X11Compliance),
  COLOR("DarkGoldenrod4",139,101,8,0,X11Compliance),
  COLOR("DarkGray",169,169,169,0,SVGCompliance|X11Compliance),
  COLOR("DarkGreen",0,100,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkKhaki",189,183,107,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkMagenta",139,0,139,0,SVGCompliance|X11Compliance),
  COLOR("DarkOliveGreen",85,107,47,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkOliveGreen1",202,255,112,0,X11Compliance),
  COLOR("DarkOliveGreen2",188,238,104,0,X11Compliance),
  COLOR("DarkOliveGreen3",162,205,90,0,X11Compliance),
  COLOR("DarkOliveGreen4",110,139,61,0,X11Compliance),
  COLOR("DarkOrange",255,140,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkOrange1",255,127,0,0,X11Compliance),
  COLOR("DarkOrange2",238,118,0,0,X11Compliance),
  COLOR("DarkOrange3",205,102,0,0,X11Compliance),
  COLOR("DarkOrange4",139,69,0,0,X11Compliance),
  COLOR("DarkOrchid",153,50,204,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkOrchid1",191,62,255,0,X11Compliance),
  COLOR("DarkOrchid2",178,58,238,0,X11Compliance),
  COLOR("DarkOrchid3",154,50,205,0,X11Compliance),
  COLOR("DarkOrchid4",104,34,139,0,X11Compliance),
  COLOR("DarkRed",139,0,0,0,SVGCompliance|X11Compliance),
  COLOR("DarkSalmon",233,150,122,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkSeaGreen",143,188,143,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkSeaGreen1",193,255,193,0,X11Compliance),
  COLOR("DarkSeaGreen2",180,238,180,0,X11Compliance),
  COLOR("DarkSeaGreen3",155,205,155,0,X11Compliance),
  COLOR("DarkSeaGreen4",105,139,105,0,X11Compliance),
  COLOR("DarkSlateBlue",72,61,139,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkSlateGray",47,79,79,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkSlateGray1",151,255,255,0,X11Compliance),
  COLOR("DarkSlateGray2",141,238,238,0,X11Compliance),
  COLOR("DarkSlateGray3",121,205,205,0,X11Compliance),
  COLOR("DarkSlateGray4",82,139,139,0,X11Compliance),
  COLOR("DarkTurquoise",0,206,209,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DarkViolet",148,0,211,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DeepPink",255,20,147,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DeepPink1",255,20,147,0,X11Compliance),
  COLOR("DeepPink2",238,18,137,0,X11Compliance),
  COLOR("DeepPink3",205,16,118,0,X11Compliance),
  COLOR("DeepPink4",139,10,80,0,X11Compliance),
  COLOR("DeepSkyBlue",0,191,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DeepSkyBlue1",0,191,255,0,X11Compliance),
  COLOR("DeepSkyBlue2",0,178,238,0,X11Compliance),
  COLOR("DeepSkyBlue3",0,154,205,0,X11Compliance),
  COLOR("DeepSkyBlue4",0,104,139,0,X11Compliance),
  COLOR("DimGray",105,105,105,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DodgerBlue",30,144,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("DodgerBlue1",30,144,255,0,X11Compliance),
  COLOR("DodgerBlue2",28,134,238,0,X11Compliance),
  COLOR("DodgerBlue3",24,116,205,0,X11Compliance),
  COLOR("DodgerBlue4",16,78,139,0,X11Compliance),
  COLOR("firebrick",178,34,34,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("firebrick1",255,48,48,0,X11Compliance),
  COLOR("firebrick2",238,44,44,0,X11Compliance),
  COLOR("firebrick3",205,38,38,0,X11Compliance),
  COLOR("firebrick4",139,26,26,0,X11Compliance),
  COLOR("FloralWhite",255,250,240,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("ForestGreen",34,139,34,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("fractal",128,128,128,0,SVGCompliance),
  COLOR("fuchsia",255,0,255,0,SVGCompliance),
  COLOR("gainsboro",220,220,220,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("GhostWhite",248,248,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("gold",255,215,0,0,X11Compliance|XPMCompliance),
  COLOR("gold1",255,215,0,0,SVGCompliance|X11Compliance),
  COLOR("gold2",238,201,0,0,X11Compliance),
  COLOR("gold3",205,173,0,0,X11Compliance),
  COLOR("gold4",139,117,0,0,X11Compliance),
  COLOR("goldenrod",218,165,32,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("goldenrod1",255,193,37,0,X11Compliance),
  COLOR("goldenrod2",238,180,34,0,X11Compliance),
  COLOR("goldenrod3",205,155,29,0,X11Compliance),
  COLOR("goldenrod4",139,105,20,0,X11Compliance),
  COLOR("gray",126,126,126,0,SVGCompliance),
  COLOR("gray",190,190,190,0,X11Compliance|XPMCompliance),
  COLOR("gray0",0,0,0,0,X11Compliance|XPMCompliance),
  COLOR("gray1",3,3,3,0,X11Compliance|XPMCompliance),
  COLOR("gray10",26,26,26,0,X11Compliance|XPMCompliance),
  COLOR("gray100",255,255,255,0,X11Compliance|XPMCompliance),
  COLOR("gray11",28,28,28,0,X11Compliance|XPMCompliance),
  COLOR("gray12",31,31,31,0,X11Compliance|XPMCompliance),
  COLOR("gray13",33,33,33,0,X11Compliance|XPMCompliance),
  COLOR("gray14",36,36,36,0,X11Compliance|XPMCompliance),
  COLOR("gray15",38,38,38,0,X11Compliance|XPMCompliance),
  COLOR("gray16",41,41,41,0,X11Compliance|XPMCompliance),
  COLOR("gray17",43,43,43,0,X11Compliance|XPMCompliance),
  COLOR("gray18",46,46,46,0,X11Compliance|XPMCompliance),
  COLOR("gray19",48,48,48,0,X11Compliance|XPMCompliance),
  COLOR("gray2",5,5,5,0,X11Compliance|XPMCompliance),
  COLOR("gray20",51,51,51,0,X11Compliance|XPMCompliance),
  COLOR("gray21",54,54,54,0,X11Compliance|XPMCompliance),
  COLOR("gray22",56,56,56,0,X11Compliance|XPMCompliance),
  COLOR("gray23",59,59,59,0,X11Compliance|XPMCompliance),
  COLOR("gray24",61,61,61,0,X11Compliance|XPMCompliance),
  COLOR("gray25",64,64,64,0,X11Compliance|XPMCompliance),
  COLOR("gray26",66,66,66,0,X11Compliance|XPMCompliance),
  COLOR("gray27",69,69,69,0,X11Compliance|XPMCompliance),
  COLOR("gray28",71,71,71,0,X11Compliance|XPMCompliance),
  COLOR("gray29",74,74,74,0,X11Compliance|XPMCompliance),
  COLOR("gray3",8,8,8,0,X11Compliance|XPMCompliance),
  COLOR("gray30",77,77,77,0,X11Compliance|XPMCompliance),
  COLOR("gray31",79,79,79,0,X11Compliance|XPMCompliance),
  COLOR("gray32",82,82,82,0,X11Compliance|XPMCompliance),
  COLOR("gray33",84,84,84,0,X11Compliance|XPMCompliance),
  COLOR("gray34",87,87,87,0,X11Compliance|XPMCompliance),
  COLOR("gray35",89,89,89,0,X11Compliance|XPMCompliance),
  COLOR("gray36",92,92,92,0,X11Compliance|XPMCompliance),
  COLOR("gray37",94,94,94,0,X11Compliance|XPMCompliance),
  COLOR("gray38",97,97,97,0,X11Compliance|XPMCompliance),
  COLOR("gray39",99,99,99,0,X11Compliance|XPMCompliance),
  COLOR("gray4",10,10,10,0,X11Compliance|XPMCompliance),
  COLOR("gray40",102,102,102,0,X11Compliance|XPMCompliance),
  COLOR("gray41",105,105,105,0,X11Compliance|XPMCompliance),
  COLOR("gray42",107,107,107,0,X11Compliance|XPMCompliance),
  COLOR("gray43",110,110,110,0,X11Compliance|XPMCompliance),
  COLOR("gray44",112,112,112,0,X11Compliance|XPMCompliance),
  COLOR("gray45",115,115,115,0,X11Compliance|XPMCompliance),
  COLOR("gray46",117,117,117,0,X11Compliance|XPMCompliance),
  COLOR("gray47",120,120,120,0,X11Compliance|XPMCompliance),
  COLOR("gray48",122,122,122,0,X11Compliance|XPMCompliance),
  COLOR("gray49",125,125,125,0,X11Compliance|XPMCompliance),
  COLOR("gray5",13,13,13,0,X11Compliance|XPMCompliance),
  COLOR("gray50",127,127,127,0,X11Compliance|XPMCompliance),
  COLOR("gray51",130,130,130,0,X11Compliance|XPMCompliance),
  COLOR("gray52",133,133,133,0,X11Compliance|XPMCompliance),
  COLOR("gray53",135,135,135,0,X11Compliance|XPMCompliance),
  COLOR("gray54",138,138,138,0,X11Compliance|XPMCompliance),
  COLOR("gray55",140,140,140,0,X11Compliance|XPMCompliance),
  COLOR("gray56",143,143,143,0,X11Compliance|XPMCompliance),
  COLOR("gray57",145,145,145,0,X11Compliance|XPMCompliance),
  COLOR("gray58",148,148,148,0,X11Compliance|XPMCompliance),
  COLOR("gray59",150,150,150,0,X11Compliance|XPMCompliance),
  COLOR("gray6",15,15,15,0,X11Compliance|XPMCompliance),
  COLOR("gray60",153,153,153,0,X11Compliance|XPMCompliance),
  COLOR("gray61",156,156,156,0,X11Compliance|XPMCompliance),
  COLOR("gray62",158,158,158,0,X11Compliance|XPMCompliance),
  COLOR("gray63",161,161,161,0,X11Compliance|XPMCompliance),
  COLOR("gray64",163,163,163,0,X11Compliance|XPMCompliance),
  COLOR("gray65",166,166,166,0,X11Compliance|XPMCompliance),
  COLOR("gray66",168,168,168,0,X11Compliance|XPMCompliance),
  COLOR("gray67",171,171,171,0,X11Compliance|XPMCompliance),
  COLOR("gray68",173,173,173,0,X11Compliance|XPMCompliance),
  COLOR("gray69",176,176,176,0,X11Compliance|XPMCompliance),
  COLOR("gray7",18,18,18,0,X11Compliance|XPMCompliance),
  COLOR("gray70",179,179,179,0,X11Compliance|XPMCompliance),
  COLOR("gray71",181,181,181,0,X11Compliance|XPMCompliance),
  COLOR("gray72",184,184,184,0,X11Compliance|XPMCompliance),
  COLOR("gray73",186,186,186,0,X11Compliance|XPMCompliance),
  COLOR("gray74",189,189,189,0,X11Compliance|XPMCompliance),
  COLOR("gray75",191,191,191,0,X11Compliance|XPMCompliance),
  COLOR("gray76",194,194,194,0,X11Compliance|XPMCompliance),
  COLOR("gray77",196,196,196,0,X11Compliance|XPMCompliance),
  COLOR("gray78",199,199,199,0,X11Compliance|XPMCompliance),
  COLOR("gray79",201,201,201,0,X11Compliance|XPMCompliance),
  COLOR("gray8",20,20,20,0,X11Compliance|XPMCompliance),
  COLOR("gray80",204,204,204,0,X11Compliance|XPMCompliance),
  COLOR("gray81",207,207,207,0,X11Compliance|XPMCompliance),
  COLOR("gray82",209,209,209,0,X11Compliance|XPMCompliance),
  COLOR("gray83",212,212,212,0,X11Compliance|XPMCompliance),
  COLOR("gray84",214,214,214,0,X11Compliance|XPMCompliance),
  COLOR("gray85",217,217,217,0,X11Compliance|XPMCompliance),
  COLOR("gray86",219,219,219,0,X11Compliance|XPMCompliance),
  COLOR("gray87",222,222,222,0,X11Compliance|XPMCompliance),
  COLOR("gray88",224,224,224,0,X11Compliance|XPMCompliance),
  COLOR("gray89",227,227,227,0,X11Compliance|XPMCompliance),
  COLOR("gray9",23,23,23,0,X11Compliance|XPMCompliance),
  COLOR("gray90",229,229,229,0,X11Compliance|XPMCompliance),
  COLOR("gray91",232,232,232,0,X11Compliance|XPMCompliance),
  COLOR("gray92",235,235,235,0,X11Compliance|XPMCompliance),
  COLOR("gray93",237,237,237,0,X11Compliance|XPMCompliance),
  COLOR("gray94",240,240,240,0,X11Compliance|XPMCompliance),
  COLOR("gray95",242,242,242,0,X11Compliance|XPMCompliance),
  COLOR("gray96",245,245,245,0,X11Compliance|XPMCompliance),
  COLOR("gray97",247,247,247,0,X11Compliance|XPMCompliance),
  COLOR("gray98",250,250,250,0,X11Compliance|XPMCompliance),
  COLOR("gray99",252,252,252,0,X11Compliance|XPMCompliance),
  COLOR("green",0,128,0,0,SVGCompliance),
  COLOR("green",0,255,0,0,X11Compliance|XPMCompliance),
  COLOR("green1",0,255,0,0,X11Compliance),
  COLOR("green2",0,238,0,0,X11Compliance),
  COLOR("green3",0,205,0,0,X11Compliance),
  COLOR("green4",0,139,0,0,X11Compliance),
  COLOR("GreenYellow",173,255,47,0,X11Compliance|XPMCompliance),
  COLOR("honeydew",240,255,240,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("honeydew1",240,255,240,0,X11Compliance),
  COLOR("honeydew2",224,238,224,0,X11Compliance),
  COLOR("honeydew3",193,205,193,0,X11Compliance),
  COLOR("honeydew4",131,139,131,0,X11Compliance),
  COLOR("HotPink",255,105,180,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("HotPink1",255,110,180,0,X11Compliance),
  COLOR("HotPink2",238,106,167,0,X11Compliance),
  COLOR("HotPink3",205,96,144,0,X11Compliance),
  COLOR("HotPink4",139,58,98,0,X11Compliance),
  COLOR("IndianRed",205,92,92,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("IndianRed1",255,106,106,0,X11Compliance),
  COLOR("IndianRed2",238,99,99,0,X11Compliance),
  COLOR("IndianRed3",205,85,85,0,X11Compliance),
  COLOR("IndianRed4",139,58,58,0,X11Compliance),
  COLOR("indigo",75,0,130,0,SVGCompliance),
  COLOR("ivory",255,255,240,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("ivory1",255,255,240,0,X11Compliance),
  COLOR("ivory2",238,238,224,0,X11Compliance),
  COLOR("ivory3",205,205,193,0,X11Compliance),
  COLOR("ivory4",139,139,131,0,X11Compliance),
  COLOR("khaki",240,230,140,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("khaki1",255,246,143,0,X11Compliance),
  COLOR("khaki2",238,230,133,0,X11Compliance),
  COLOR("khaki3",205,198,115,0,X11Compliance),
  COLOR("khaki4",139,134,78,0,X11Compliance),
  COLOR("lavender",230,230,250,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LavenderBlush",255,240,245,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LavenderBlush1",255,240,245,0,X11Compliance),
  COLOR("LavenderBlush2",238,224,229,0,X11Compliance),
  COLOR("LavenderBlush3",205,193,197,0,X11Compliance),
  COLOR("LavenderBlush4",139,131,134,0,X11Compliance),
  COLOR("LawnGreen",124,252,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LemonChiffon",255,250,205,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LemonChiffon1",255,250,205,0,X11Compliance),
  COLOR("LemonChiffon2",238,233,191,0,X11Compliance),
  COLOR("LemonChiffon3",205,201,165,0,X11Compliance),
  COLOR("LemonChiffon4",139,137,112,0,X11Compliance),
  COLOR("LightBlue",173,216,230,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightBlue1",191,239,255,0,X11Compliance),
  COLOR("LightBlue2",178,223,238,0,X11Compliance),
  COLOR("LightBlue3",154,192,205,0,X11Compliance),
  COLOR("LightBlue4",104,131,139,0,X11Compliance),
  COLOR("LightCoral",240,128,128,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightCyan",224,255,255,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightCyan1",224,255,255,0,X11Compliance),
  COLOR("LightCyan2",209,238,238,0,X11Compliance),
  COLOR("LightCyan3",180,205,205,0,X11Compliance),
  COLOR("LightCyan4",122,139,139,0,X11Compliance),
  COLOR("LightGoldenrod",238,221,130,0,X11Compliance|XPMCompliance),
  COLOR("LightGoldenrod1",255,236,139,0,X11Compliance),
  COLOR("LightGoldenrod2",238,220,130,0,X11Compliance),
  COLOR("LightGoldenrod3",205,190,112,0,X11Compliance),
  COLOR("LightGoldenrod4",139,129,76,0,X11Compliance),
  COLOR("LightGoldenrodYellow",250,250,210,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightGray",211,211,211,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightGreen",144,238,144,0,SVGCompliance|X11Compliance),
  COLOR("LightPink",255,182,193,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightPink1",255,174,185,0,X11Compliance),
  COLOR("LightPink2",238,162,173,0,X11Compliance),
  COLOR("LightPink3",205,140,149,0,X11Compliance),
  COLOR("LightPink4",139,95,101,0,X11Compliance),
  COLOR("LightSalmon",255,160,122,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightSalmon1",255,160,122,0,X11Compliance),
  COLOR("LightSalmon2",238,149,114,0,X11Compliance),
  COLOR("LightSalmon3",205,129,98,0,X11Compliance),
  COLOR("LightSalmon4",139,87,66,0,X11Compliance),
  COLOR("LightSeaGreen",32,178,170,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightSkyBlue",135,206,250,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightSkyBlue1",176,226,255,0,X11Compliance),
  COLOR("LightSkyBlue2",164,211,238,0,X11Compliance),
  COLOR("LightSkyBlue3",141,182,205,0,X11Compliance),
  COLOR("LightSkyBlue4",96,123,139,0,X11Compliance),
  COLOR("LightSlateBlue",132,112,255,0,X11Compliance|XPMCompliance),
  COLOR("LightSlateGray",119,136,153,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightSteelBlue",176,196,222,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightSteelBlue1",202,225,255,0,X11Compliance),
  COLOR("LightSteelBlue2",188,210,238,0,X11Compliance),
  COLOR("LightSteelBlue3",162,181,205,0,X11Compliance),
  COLOR("LightSteelBlue4",110,123,139,0,X11Compliance),
  COLOR("LightYellow",255,255,224,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("LightYellow1",255,255,224,0,X11Compliance),
  COLOR("LightYellow2",238,238,209,0,X11Compliance),
  COLOR("LightYellow3",205,205,180,0,X11Compliance),
  COLOR("LightYellow4",139,139,122,0,X11Compliance),
  COLOR("lime",0,255,0,0,SVGCompliance),
  COLOR("LimeGreen",50,205,50,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("linen",250,240,230,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("magenta",255,0,255,0,SVGCompliance),
  COLOR("magenta",255,0,255,0,X11Compliance|XPMCompliance),
  COLOR("magenta1",255,0,255,0,X11Compliance),
  COLOR("magenta2",238,0,238,0,X11Compliance),
  COLOR("magenta3",205,0,205,0,X11Compliance),
  COLOR("magenta4",139,0,139,0,X11Compliance),
  COLOR("maroon",128,0,0,0,SVGCompliance),
  COLOR("maroon",176,48,96,0,X11Compliance|XPMCompliance),
  COLOR("maroon1",255,52,179,0,X11Compliance),
  COLOR("maroon2",238,48,167,0,X11Compliance),
  COLOR("maroon3",205,41,144,0,X11Compliance),
  COLOR("maroon4",139,28,98,0,X11Compliance),
  COLOR("MediumAquamarine",102,205,170,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumBlue",0,0,205,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumForestGreen",50,129,75,0,X11Compliance|XPMCompliance),
  COLOR("MediumGoldenRod",209,193,102,0,X11Compliance|XPMCompliance),
  COLOR("MediumOrchid",186,85,211,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumOrchid1",224,102,255,0,X11Compliance),
  COLOR("MediumOrchid2",209,95,238,0,X11Compliance),
  COLOR("MediumOrchid3",180,82,205,0,X11Compliance),
  COLOR("MediumOrchid4",122,55,139,0,X11Compliance),
  COLOR("MediumPurple",147,112,219,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumPurple1",171,130,255,0,X11Compliance),
  COLOR("MediumPurple2",159,121,238,0,X11Compliance),
  COLOR("MediumPurple3",137,104,205,0,X11Compliance),
  COLOR("MediumPurple4",93,71,139,0,X11Compliance),
  COLOR("MediumSeaGreen",60,179,113,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumSlateBlue",123,104,238,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumSpringGreen",0,250,154,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumTurquoise",72,209,204,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MediumVioletRed",199,21,133,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MidnightBlue",25,25,112,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MintCream",245,255,250,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MistyRose",255,228,225,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("MistyRose1",255,228,225,0,X11Compliance),
  COLOR("MistyRose2",238,213,210,0,X11Compliance),
  COLOR("MistyRose3",205,183,181,0,X11Compliance),
  COLOR("MistyRose4",139,125,123,0,X11Compliance),
  COLOR("moccasin",255,228,181,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("NavajoWhite",255,222,173,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("NavajoWhite1",255,222,173,0,X11Compliance),
  COLOR("NavajoWhite2",238,207,161,0,X11Compliance),
  COLOR("NavajoWhite3",205,179,139,0,X11Compliance),
  COLOR("NavajoWhite4",139,121,94,0,X11Compliance),
  COLOR("navy",0,0,128,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("NavyBlue",0,0,128,0,X11Compliance|XPMCompliance),
  COLOR("none",0,0,0,255,SVGCompliance),
  COLOR("OldLace",253,245,230,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("olive",128,128,0,0,SVGCompliance),
  COLOR("OliveDrab",107,142,35,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("OliveDrab1",192,255,62,0,X11Compliance),
  COLOR("OliveDrab2",179,238,58,0,X11Compliance),
  COLOR("OliveDrab3",154,205,50,0,X11Compliance),
  COLOR("OliveDrab4",105,139,34,0,X11Compliance),
  COLOR("opaque",0,0,0,0,SVGCompliance),
  COLOR("orange",255,165,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("orange1",255,165,0,0,X11Compliance),
  COLOR("orange2",238,154,0,0,X11Compliance),
  COLOR("orange3",205,133,0,0,X11Compliance),
  COLOR("orange4",139,90,0,0,X11Compliance),
  COLOR("OrangeRed",255,69,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("OrangeRed1",255,69,0,0,X11Compliance),
  COLOR("OrangeRed2",238,64,0,0,X11Compliance),
  COLOR("OrangeRed3",205,55,0,0,X11Compliance),
  COLOR("OrangeRed4",139,37,0,0,X11Compliance),
  COLOR("orchid",218,112,214,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("orchid1",255,131,250,0,X11Compliance),
  COLOR("orchid2",238,122,233,0,X11Compliance),
  COLOR("orchid3",205,105,201,0,X11Compliance),
  COLOR("orchid4",139,71,137,0,X11Compliance),
  COLOR("PaleGoldenrod",238,232,170,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("PaleGreen",152,251,152,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("PaleGreen1",154,255,154,0,X11Compliance),
  COLOR("PaleGreen2",144,238,144,0,X11Compliance),
  COLOR("PaleGreen3",124,205,124,0,X11Compliance),
  COLOR("PaleGreen4",84,139,84,0,X11Compliance),
  COLOR("PaleTurquoise",175,238,238,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("PaleTurquoise1",187,255,255,0,X11Compliance),
  COLOR("PaleTurquoise2",174,238,238,0,X11Compliance),
  COLOR("PaleTurquoise3",150,205,205,0,X11Compliance),
  COLOR("PaleTurquoise4",102,139,139,0,X11Compliance),
  COLOR("PaleVioletRed",219,112,147,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("PaleVioletRed1",255,130,171,0,X11Compliance),
  COLOR("PaleVioletRed2",238,121,159,0,X11Compliance),
  COLOR("PaleVioletRed3",205,104,137,0,X11Compliance),
  COLOR("PaleVioletRed4",139,71,93,0,X11Compliance),
  COLOR("PapayaWhip",255,239,213,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("PeachPuff",255,218,185,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("PeachPuff1",255,218,185,0,X11Compliance),
  COLOR("PeachPuff2",238,203,173,0,X11Compliance),
  COLOR("PeachPuff3",205,175,149,0,X11Compliance),
  COLOR("PeachPuff4",139,119,101,0,X11Compliance),
  COLOR("peru",205,133,63,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("pink",255,192,203,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("pink1",255,181,197,0,X11Compliance),
  COLOR("pink2",238,169,184,0,X11Compliance),
  COLOR("pink3",205,145,158,0,X11Compliance),
  COLOR("pink4",139,99,108,0,X11Compliance),
  COLOR("plum",221,160,221,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("plum1",255,187,255,0,X11Compliance),
  COLOR("plum2",238,174,238,0,X11Compliance),
  COLOR("plum3",205,150,205,0,X11Compliance),
  COLOR("plum4",139,102,139,0,X11Compliance),
  COLOR("PowderBlue",176,224,230,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("purple",128,0,128,0,SVGCompliance),
  COLOR("purple",160,32,240,0,X11Compliance|XPMCompliance),
  COLOR("purple1",155,48,255,0,X11Compliance),
  COLOR("purple2",145,44,238,0,X11Compliance),
  COLOR("purple3",125,38,205,0,X11Compliance),
  COLOR("purple4",85,26,139,0,X11Compliance),
  COLOR("red",255,0,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("red1",255,0,0,0,X11Compliance),
  COLOR("red2",238,0,0,0,X11Compliance),
  COLOR("red3",205,0,0,0,X11Compliance),
  COLOR("red4",139,0,0,0,X11Compliance),
  COLOR("RosyBrown",188,143,143,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("RosyBrown1",255,193,193,0,X11Compliance),
  COLOR("RosyBrown2",238,180,180,0,X11Compliance),
  COLOR("RosyBrown3",205,155,155,0,X11Compliance),
  COLOR("RosyBrown4",139,105,105,0,X11Compliance),
  COLOR("RoyalBlue",65,105,225,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("RoyalBlue1",72,118,255,0,X11Compliance),
  COLOR("RoyalBlue2",67,110,238,0,X11Compliance),
  COLOR("RoyalBlue3",58,95,205,0,X11Compliance),
  COLOR("RoyalBlue4",39,64,139,0,X11Compliance),
  COLOR("SaddleBrown",139,69,19,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("salmon",250,128,114,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("salmon1",255,140,105,0,X11Compliance),
  COLOR("salmon2",238,130,98,0,X11Compliance),
  COLOR("salmon3",205,112,84,0,X11Compliance),
  COLOR("salmon4",139,76,57,0,X11Compliance),
  COLOR("SandyBrown",244,164,96,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SeaGreen",46,139,87,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SeaGreen1",84,255,159,0,X11Compliance),
  COLOR("SeaGreen2",78,238,148,0,X11Compliance),
  COLOR("SeaGreen3",67,205,128,0,X11Compliance),
  COLOR("SeaGreen4",46,139,87,0,X11Compliance),
  COLOR("seashell",255,245,238,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("seashell1",255,245,238,0,X11Compliance),
  COLOR("seashell2",238,229,222,0,X11Compliance),
  COLOR("seashell3",205,197,191,0,X11Compliance),
  COLOR("seashell4",139,134,130,0,X11Compliance),
  COLOR("sienna",160,82,45,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("sienna1",255,130,71,0,X11Compliance),
  COLOR("sienna2",238,121,66,0,X11Compliance),
  COLOR("sienna3",205,104,57,0,X11Compliance),
  COLOR("sienna4",139,71,38,0,X11Compliance),
  COLOR("silver",192,192,192,0,SVGCompliance),
  COLOR("SkyBlue",135,206,235,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SkyBlue1",135,206,255,0,X11Compliance),
  COLOR("SkyBlue2",126,192,238,0,X11Compliance),
  COLOR("SkyBlue3",108,166,205,0,X11Compliance),
  COLOR("SkyBlue4",74,112,139,0,X11Compliance),
  COLOR("SlateBlue",106,90,205,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SlateBlue1",131,111,255,0,X11Compliance),
  COLOR("SlateBlue2",122,103,238,0,X11Compliance),
  COLOR("SlateBlue3",105,89,205,0,X11Compliance),
  COLOR("SlateBlue4",71,60,139,0,X11Compliance),
  COLOR("SlateGray",112,128,144,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SlateGray1",198,226,255,0,X11Compliance),
  COLOR("SlateGray2",185,211,238,0,X11Compliance),
  COLOR("SlateGray3",159,182,205,0,X11Compliance),
  COLOR("SlateGray4",108,123,139,0,X11Compliance),
  COLOR("snow",255,250,250,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("snow1",255,250,250,0,X11Compliance),
  COLOR("snow2",238,233,233,0,X11Compliance),
  COLOR("snow3",205,201,201,0,X11Compliance),
  COLOR("snow4",139,137,137,0,X11Compliance),
  COLOR("SpringGreen",0,255,127,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SpringGreen1",0,255,127,0,X11Compliance),
  COLOR("SpringGreen2",0,238,118,0,X11Compliance),
  COLOR("SpringGreen3",0,205,102,0,X11Compliance),
  COLOR("SpringGreen4",0,139,69,0,X11Compliance),
  COLOR("SteelBlue",70,130,180,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("SteelBlue1",99,184,255,0,X11Compliance),
  COLOR("SteelBlue2",92,172,238,0,X11Compliance),
  COLOR("SteelBlue3",79,148,205,0,X11Compliance),
  COLOR("SteelBlue4",54,100,139,0,X11Compliance),
  COLOR("tan",210,180,140,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("tan1",255,165,79,0,X11Compliance),
  COLOR("tan2",238,154,73,0,X11Compliance),
  COLOR("tan3",205,133,63,0,X11Compliance),
  COLOR("tan4",139,90,43,0,X11Compliance),
  COLOR("teal",0,128,128,0,SVGCompliance),
  COLOR("thistle",216,191,216,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("thistle1",255,225,255,0,X11Compliance),
  COLOR("thistle2",238,210,238,0,X11Compliance),
  COLOR("thistle3",205,181,205,0,X11Compliance),
  COLOR("thistle4",139,123,139,0,X11Compliance),
  COLOR("tomato",255,99,71,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("tomato1",255,99,71,0,X11Compliance),
  COLOR("tomato2",238,92,66,0,X11Compliance),
  COLOR("tomato3",205,79,57,0,X11Compliance),
  COLOR("tomato4",139,54,38,0,X11Compliance),
  COLOR("transparent",0,0,0,255,SVGCompliance),
  COLOR("turquoise",64,224,208,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("turquoise1",0,245,255,0,X11Compliance),
  COLOR("turquoise2",0,229,238,0,X11Compliance),
  COLOR("turquoise3",0,197,205,0,X11Compliance),
  COLOR("turquoise4",0,134,139,0,X11Compliance),
  COLOR("violet",238,130,238,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("VioletRed",208,32,144,0,X11Compliance|XPMCompliance),
  COLOR("VioletRed1",255,62,150,0,X11Compliance),
  COLOR("VioletRed2",238,58,140,0,X11Compliance),
  COLOR("VioletRed3",205,50,120,0,X11Compliance),
  COLOR("VioletRed4",139,34,82,0,X11Compliance),
  COLOR("wheat",245,222,179,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("wheat1",255,231,186,0,X11Compliance),
  COLOR("wheat2",238,216,174,0,X11Compliance),
  COLOR("wheat3",205,186,150,0,X11Compliance),
  COLOR("wheat4",139,126,102,0,X11Compliance),
  COLOR("white",255,255,255,0,SVGCompliance|X11Compliance),
  COLOR("WhiteSmoke",245,245,245,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("yellow",255,255,0,0,SVGCompliance|X11Compliance|XPMCompliance),
  COLOR("yellow1",255,255,0,0,X11Compliance),
  COLOR("yellow2",238,238,0,0,X11Compliance),
  COLOR("yellow3",205,205,0,0,X11Compliance),
  COLOR("yellow4",139,139,0,0,X11Compliance),
  COLOR("YellowGreen",154,205,50,0,SVGCompliance|X11Compliance|XPMCompliance)
};

/*
  Static declarations.
*/
static SemaphoreInfo
  *color_semaphore = (SemaphoreInfo *) NULL;

static ColorInfo
  *color_list = (ColorInfo *) NULL;

/*
  Forward declarations.
*/
static unsigned int
  ReadColorConfigureFile(const char *,const unsigned int,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y C o l o r I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyColorInfo deallocates memory associated with the color list.
%
%  The format of the DestroyColorInfo method is:
%
%      DestroyColorInfo(void)
%
%
*/
static void
DestroyColorInfoEntry(ColorInfo *entry)
{
  if (entry->previous)
    entry->previous->next=entry->next;
  if (entry->next)
    entry->next->previous=entry->previous;
  if (entry == color_list)
    color_list=entry->next;
  if ((entry->path[0] != BuiltInPath[0]) &&
      (LocaleCompare(entry->path,BuiltInPath) != 0))
    {
      MagickFreeMemory(entry->path);
      MagickFreeMemory(entry->name);
    }
  MagickFreeMemory(entry);    
}
MagickExport void
DestroyColorInfo(void)
{
  ColorInfo
    *color_info;

  register ColorInfo
    *p;

  for (p=color_list; p != (const ColorInfo *) NULL; )
  {
    color_info=p;
    p=p->next;
    DestroyColorInfoEntry(color_info);
  }
  color_list=(ColorInfo *) NULL;
  DestroySemaphoreInfo(&color_semaphore);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t C o l o r I n f o                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorInfo searches the color list for the specified name and if
%  found returns attributes for that color.
%
%  The format of the GetColorInfo method is:
%
%      const PixelPacket *GetColorInfo(const char *name,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o color_info: Method GetColorInfo searches the color list for the
%      specified name and if found returns attributes for that color.
%
%    o name: The color name.
%
%    o compliance: Define the required color standard.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport const ColorInfo *
GetColorInfo(const char *name,ExceptionInfo *exception)
{
  char
    colorname[MaxTextExtent];

  register char
    *q;

  register ColorInfo
    *p;

  LockSemaphoreInfo(color_semaphore);
  if (color_list == (ColorInfo *) NULL)
    (void) ReadColorConfigureFile(ColorFilename,0,exception);
  UnlockSemaphoreInfo(color_semaphore);
  if ((name == (const char *) NULL) || (LocaleCompare(name,"*") == 0))
    return((const ColorInfo *) color_list);
  /*
    Search for named color.
  */
  (void) strlcpy(colorname,name,MaxTextExtent);
  for (q=colorname; *q != '\0'; q++)
  {
    if (*q != ' ')
      continue;
    (void) strcpy(q,q+1);
    q--;
  }
  LockSemaphoreInfo(color_semaphore);
  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
    if (LocaleCompare(colorname,p->name) == 0)
      break;
  if (p == (ColorInfo *) NULL)
    {
      /* Check common synonyms */
      const char
	*pos;

      LocaleUpper(colorname);
      if ((pos = strstr(colorname,"GREY")) != (const char *) NULL)
	{
	  colorname[pos-colorname+2]='A';
	  for (p=color_list; p != (ColorInfo *) NULL; p=p->next)
	    if (LocaleCompare(colorname,p->name) == 0)
	      break;
	}
    }
  if (p == (ColorInfo *) NULL)
    ThrowException(exception,OptionWarning,UnrecognizedColor,name);
  else
    if (p != color_list)
      {
        /*
          Self-adjusting list.
        */
        if (p->previous != (ColorInfo *) NULL)
          p->previous->next=p->next;
        if (p->next != (ColorInfo *) NULL)
          p->next->previous=p->previous;
        p->previous=(ColorInfo *) NULL;
        p->next=color_list;
        color_list->previous=p;
        color_list=p;
      }
  UnlockSemaphoreInfo(color_semaphore);
  return((const ColorInfo *) p);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r I n f o A r r a y                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetColorInfoArray() returns a sorted null-terminated array of ColorInfo
%  pointers corresponding to the available color definitions. This function
%  should be used to access the entire list rather than GetColorInfo since
%  the list returned by GetColorInfo may be re-ordered every time it is
%  invoked. GetColorList may be used if only a list of color names is desired.
%  The array should be deallocated by the user once it is no longer needed.
%  Do not attempt to deallocate members of the array.
%
%  The format of the GetMagickList method is:
%
%      ColorInfo **GetColorInfoArray(ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
/*
  Compare two ColorInfo structures based on their name
*/
static int
ColorInfoCompare(const void *x, const void *y)
{
  const ColorInfo
    *xx=*((const ColorInfo **) x),
    *yy=*((const ColorInfo **) y);

  return strcmp(xx->name, yy->name);
}
MagickExport ColorInfo **
GetColorInfoArray(ExceptionInfo *exception)
{
  ColorInfo
    **array;

  ColorInfo
    *p;

  ColorInfo
    *list;

  size_t
    entries=0;

  int
    i;

  /*
    Load color list
  */
  (void) GetColorInfo("*",exception);
  if ((!color_list) || (exception->severity > UndefinedException))
    return 0;

  LockSemaphoreInfo(color_semaphore);

  list=color_list;

  /*
    Count number of list entries
  */
  for (p=list; p != 0; p=p->next)
    entries++;

  /*
    Allocate array memory
  */
  array=MagickAllocateMemory(ColorInfo **,sizeof(ColorInfo *)*(entries+1));
  if (!array)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,0);
      return False;
    }
  (void) memset((void **)array,0,sizeof(ColorInfo *)*(entries+1));

  /*
    Add entries to array
  */
  i=0;
  for (p=list; p != 0; p=p->next)
    array[i++]=p;

  UnlockSemaphoreInfo(color_semaphore);

  /*
    Sort array entries
  */
  qsort((void *) array, entries, sizeof(ColorInfo *), ColorInfoCompare);

  return (array);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t C o l o r L i s t                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetColorList returns any colors that match the specified pattern.
%
%  The format of the GetColorList function is:
%
%      filelist=GetColorList(const char *pattern,unsigned long *number_colors)
%
%  A description of each parameter follows:
%
%    o filelist: Method GetColorList returns a list of colors that match the
%      specified pattern.
%
%    o pattern: Specifies a pointer to a text string containing a pattern.
%
%    o number_colors:  This integer returns the number of colors in the list.
%
%
*/
MagickExport char **
GetColorList(const char *pattern,unsigned long *number_colors)
{
  char
    **colorlist;

  ExceptionInfo
    exception;

  register long
    i;

  register const ColorInfo
    *p;


  assert(pattern != (char *) NULL);
  assert(number_colors != (unsigned long *) NULL);
  *number_colors=0;
  GetExceptionInfo(&exception);
  p=GetColorInfo("*",&exception);
  DestroyExceptionInfo(&exception);
  if (p == (const ColorInfo *) NULL)
    return((char **) NULL);

  /*
    Determine color list size
  */
  LockSemaphoreInfo(color_semaphore);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
    i++;
  UnlockSemaphoreInfo(color_semaphore);

  /*
    Allocate color list.
  */
  colorlist=MagickAllocateMemory(char **,i*sizeof(char *));
  if (colorlist == (char **) NULL)
    return((char **) NULL);

  /*
    Add colors matching glob specification to list
  */
  LockSemaphoreInfo(color_semaphore);
  i=0;
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if (p->stealth)
      continue;
    if (GlobExpression(p->name,pattern))
      colorlist[i++]=AcquireString(p->name);
  }
  UnlockSemaphoreInfo(color_semaphore);

  *number_colors=i;
  return(colorlist);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t C o l o r I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ListColorInfo() lists color names to the specified file.  Color names
%  are a convenience.  Rather than defining a color by its red, green, and
%  blue intensities just use a color name such as white, blue, or yellow.
%
%  The format of the ListColorInfo method is:
%
%      unsigned int ListColorInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  List color names to this file handle.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int
ListColorInfo(FILE *file,ExceptionInfo *exception)
{
  register long
    i;

  register const ColorInfo
    *p;

  if (file == (const FILE *) NULL)
    file=stdout;
  (void) GetColorInfo("*",exception);
  LockSemaphoreInfo(color_semaphore);
  for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
  {
    if ((p->previous == (ColorInfo *) NULL) ||
        (LocaleCompare(p->path,p->previous->path) != 0))
      {
        if (p->previous != (ColorInfo *) NULL)
          (void) fprintf(file,"\n");
        if (p->path != (char *) NULL)
          (void) fprintf(file,"Path: %.1024s\n\n",p->path);
        (void) fprintf(file,
          "Name                   Color                   Compliance\n");
        (void) fprintf(file,"-------------------------------------------------"
          "------------------------------\n");
      }
    if (p->stealth)
      continue;

    (void) fprintf(file,"%.1024s",p->name);
    for (i=(long) strlen(p->name); i <= 22; i++)
      (void) fprintf(file," ");

    if (p->color.opacity == OpaqueOpacity)
      (void) fprintf(file,"%5d,%5d,%5d       ",
		     ScaleQuantumToChar(p->color.red),
		     ScaleQuantumToChar(p->color.green),
		     ScaleQuantumToChar(p->color.blue));
    else
      (void) fprintf(file,"%5d,%5d,%5d,%5d ",
		     ScaleQuantumToChar(p->color.red),
		     ScaleQuantumToChar(p->color.green),
		     ScaleQuantumToChar(p->color.blue),
		     ScaleQuantumToChar(p->color.opacity));
    if ((unsigned int) p->compliance & (unsigned int) SVGCompliance)
      (void) fprintf(file,"SVG ");
    if ((unsigned int) p->compliance & (unsigned int) X11Compliance)
      (void) fprintf(file,"X11 ");
    if ((unsigned int) p->compliance & (unsigned int) XPMCompliance)
      (void) fprintf(file,"XPM ");
    (void) fprintf(file,"\n");
  }
  (void) fflush(file);
  UnlockSemaphoreInfo(color_semaphore);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e C o l o r I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeColorInfo initializes the color look-up facility
%
%  The format of the InitializeColorInfo method is:
%
%      MagickPassFail InitializeColorInfo(void)
%
%
*/
MagickPassFail
InitializeColorInfo(void)
{
  assert(color_semaphore == (SemaphoreInfo *) NULL);
  color_semaphore=AllocateSemaphoreInfo();
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   Q u e r y C o l o r D a t a b a s e                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorDatabase() returns the red, green, blue, and opacity intensities
%  for a given color name.
%
%  The format of the QueryColorDatabase method is:
%
%      unsigned int QueryColorDatabase(const char *name,PixelPacket *color,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o name: The color name (e.g. white, blue, yellow).
%
%    o color: The red, green, blue, and opacity intensities values of the
%      named color in this structure.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int
QueryColorDatabase(const char *name,PixelPacket *color,
		   ExceptionInfo *exception)
{
  double
    scale;

  register const ColorInfo
    *p;

  register int
    i;

  /*
    Initialize color return value.
  */
  assert(color != (PixelPacket *) NULL);
  (void) memset(color,0,sizeof(PixelPacket));
  color->opacity=TransparentOpacity;
  if ((name == (char *) NULL) || (*name == '\0'))
    name=BackgroundColor;
  while (isspace((int) (*name)))
    name++;
  if (*name == '#')
    {
      char
        c;

      int
        n;

      LongPixelPacket
        pixel;

      (void) memset(&pixel,0,sizeof(pixel));
      name++;
      for (n=0; isxdigit((int) name[n]); n++);
      if ((n == 3) || (n == 6) || (n == 9) || (n == 12) || (n == 24))
        {
          /*
            Parse RGB specification.
          */
          n/=3;
          do
          {
            pixel.red=pixel.green;
            pixel.green=pixel.blue;
            pixel.blue=0;
            for (i=n-1; i >= 0; i--)
            {
              c=(*name++);
              pixel.blue<<=4;
              if ((c >= '0') && (c <= '9'))
                pixel.blue|=c-'0';
              else
                if ((c >= 'A') && (c <= 'F'))
                  pixel.blue|=c-('A'-10);
                else
                  if ((c >= 'a') && (c <= 'f'))
                    pixel.blue|=c-('a'-10);
                  else
                    {
                      ThrowException(exception,OptionWarning,UnrecognizedColor,name);
                      return(False);
                    }
            }
          } while (isxdigit((int) *name));
        }
      else
        if ((n != 4) && (n != 8) && (n != 16) && (n != 32))
          {
            ThrowException(exception,OptionWarning,UnrecognizedColor,name);
            return(False);
          }
        else
          {
            /*
              Parse RGBA specification.
            */
            n/=4;
            do
            {
              pixel.red=pixel.green;
              pixel.green=pixel.blue;
              pixel.blue=pixel.opacity;
              pixel.opacity=0;
              for (i=n-1; i >= 0; i--)
              {
                c=(*name++);
                pixel.opacity<<=4;
                if ((c >= '0') && (c <= '9'))
                  pixel.opacity|=c-'0';
                else
                  if ((c >= 'A') && (c <= 'F'))
                    pixel.opacity|=c-('A'-10);
                  else
                    if ((c >= 'a') && (c <= 'f'))
                      pixel.opacity|=c-('a'-10);
                    else
                      {
                        ThrowException(exception,OptionWarning,UnrecognizedColor,name);
                        return(False);
                      }
              }
            } while (isxdigit((int) *name));
          }
      {
        unsigned int
          divisor=1;
        
        n<<=2;
        for( i=n-1; i; i--)
          {
            divisor <<= 1;
            divisor |=1;
          }
        color->red=(Quantum)
          (((double) MaxRGB*pixel.red)/divisor+0.5);
        color->green=(Quantum)
          (((double) MaxRGB*pixel.green)/divisor+0.5);
        color->blue=(Quantum)
          (((double) MaxRGB*pixel.blue)/divisor+0.5);
        color->opacity=OpaqueOpacity;
        if ((n != 3) && (n != 6) && (n != 9) && (n != 12) && (n != 24))
          color->opacity=(Quantum)
            (((double) MaxRGB*pixel.opacity)/divisor+0.5);
      }
      return(True);
    }
  if (LocaleNCompare(name,"rgb(",4) == 0)
    {
      DoublePixelPacket
	pixel;

      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf",
        &pixel.red,&pixel.green,&pixel.blue);
      color->red=ScaleCharToQuantum(scale*pixel.red);
      color->green=ScaleCharToQuantum(scale*pixel.green);
      color->blue=ScaleCharToQuantum(scale*pixel.blue);
      color->opacity=OpaqueOpacity;
      return(True);
    }
  if (LocaleNCompare(name,"rgba(",5) == 0)
    {
      DoublePixelPacket
	pixel;

      scale=strchr(name,'%') == (char *) NULL ? 1.0 :
        ScaleQuantumToChar(MaxRGB)/100.0;
      (void) sscanf(name,"%*[^(](%lf%*[%,]%lf%*[%,]%lf%*[%,]%lf",
        &pixel.red,&pixel.green,&pixel.blue,&pixel.opacity);
      color->red=ScaleCharToQuantum(scale*pixel.red);
      color->green=ScaleCharToQuantum(scale*pixel.green);
      color->blue=ScaleCharToQuantum(scale*pixel.blue);
      color->opacity=ScaleCharToQuantum(scale*pixel.opacity);
      return(True);
    }
  p=GetColorInfo(name,exception);
  if (p == (const ColorInfo *) NULL)
    return(False);
  if ((LocaleCompare(p->name,"opaque") == 0) ||
      (LocaleCompare(p->name,"transparent") == 0))
    {
      color->opacity=p->color.opacity;
      return(True);
    }
  *color=p->color;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  Q u e r y C o l o r n a m e                                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  QueryColorname() returns a named color for the given color intensity.  If
%  an exact match is not found, a hex value is return instead.  For example
%  an intensity of rgb:(0,0,0) returns black whereas rgb:(223,223,223)
%  returns #dfdfdf.
%
%  The format of the QueryColorname method is:
%
%      unsigned int QueryColorname(const Image *image,const PixelPacket *color,
%        const ComplianceType compliance,char *name,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image: The image.
%
%    o color: The color intensities.
%
%    o Compliance: Adhere to this color standard: NoCompliance, SVGCompliance,
%                    X11Compliance, XPMCompliance, AllCompliance.
%
%    o name: Update with color name or hex value.  Buffer should be be at
%            least MaxTextExtent long.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport unsigned int
QueryColorname(const Image *image,const PixelPacket *color,
	       const ComplianceType compliance,char *name,
	       ExceptionInfo *exception)
{
  register const ColorInfo
    *p;

  MagickBool
    matte;

  *name='\0';
  matte=image->matte;
  if (compliance == XPMCompliance)
    matte=MagickFalse;
  p=GetColorInfo("*",exception);
  if (p != (const ColorInfo *) NULL)
    {
      for (p=color_list; p != (const ColorInfo *) NULL; p=p->next)
      {
        if (!((unsigned int) p->compliance & (unsigned int) compliance))
          continue;
        if ((p->color.red != color->red) || (p->color.green != color->green) ||
            (p->color.blue != color->blue) ||
            (p->color.opacity != color->opacity))
          continue;
        (void) strlcpy(name,p->name,MaxTextExtent);
        return(True);
      }
    }
  GetColorTuple(color,image->depth,matte,True,name);
  return(False);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o l o r C o n f i g u r e F i l e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadColorConfigureFile reads the color configuration file which maps
%  color strings with a particular image format.
%
%  The format of the ReadColorConfigureFile method is:
%
%      MagickPassFail ReadColorConfigureFile(const char *basename,
%        const unsigned int depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadColorConfigureFile returns True if at least one color
%      is defined otherwise False.
%
%    o basename:  The color configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
static MagickPassFail
ReadColorConfigureFile(const char *basename,
		       const unsigned int depth,
		       ExceptionInfo *exception)
{
  size_t
    length;

  char
    path[MaxTextExtent],
    *xml;

  if (depth == 0)
    {
      size_t
	i;

      /*
	Load default set of colors from the static color table.
      */
      for (i=0 ; i < sizeof(StaticColors)/sizeof(StaticColors[0]); i++)
	{
	  ColorInfo
	    *color_info;

	  color_info=MagickAllocateMemory(ColorInfo *,sizeof(ColorInfo));
	  if (color_info == (ColorInfo *) NULL)
	    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
			      UnableToAllocateColorInfo);
	  color_info->path=(char *) BuiltInPath;
	  color_info->name=(char *) StaticColors[i].name;
	  color_info->compliance=(ComplianceType) StaticColors[i].compliance;
	  color_info->color.red=ScaleCharToQuantum(StaticColors[i].red);
	  color_info->color.green=ScaleCharToQuantum(StaticColors[i].green);
	  color_info->color.blue=ScaleCharToQuantum(StaticColors[i].blue);
	  color_info->color.opacity=ScaleCharToQuantum(StaticColors[i].opacity);
	  color_info->stealth=MagickFalse;
	  color_info->signature=MagickSignature;
	  color_info->previous=(ColorInfo *) NULL;
	  color_info->next=(ColorInfo *) NULL;
	  if (color_list == (ColorInfo *) NULL)
	    {
	      color_list=color_info;
	      continue;
	    }
	  color_list->next=color_info;
	  color_info->previous=color_list;
	  color_list=color_list->next;
	}
    }

  /*
    Read the color configure file (if any).
  */
  (void) strlcpy(path,basename,sizeof(path));
  if (depth == 0)
    {
      ExceptionInfo
	exception_local;

      GetExceptionInfo(&exception_local);
      xml=(char *) GetConfigureBlob(basename,path,&length,&exception_local);
      if (exception_local.severity != ConfigureError)
	CopyException(exception,&exception_local);
      DestroyExceptionInfo(&exception_local);
    }
  else
    {
      xml=(char *) FileToBlob(basename,&length,exception);
    }
  if (xml != (char *) NULL)
    {
      char
	keyword[MaxTextExtent],
	*q,
	*token;

      MagickBool
	in_entry;

      token=AcquireString(xml);
      in_entry=MagickFalse;
      for (q=xml; *q != '\0'; )
	{
	  /*
	    Interpret XML.
	  */
	  GetToken(q,&q,token);
	  if (*token == '\0')
	    break;
	  (void) strlcpy(keyword,token,sizeof(keyword));
	  if (LocaleNCompare(keyword,"<!--",4) == 0)
	    {
	      /*
		Comment element.
	      */
	      while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
		GetToken(q,&q,token);
	      continue;
	    }
	  if (LocaleCompare(keyword,"<include") == 0)
	    {
	      /*
		Include element.
	      */
	      while ((*token != '>') && (*q != '\0'))
		{
		  (void) strlcpy(keyword,token,MaxTextExtent);
		  GetToken(q,&q,token);
		  if (*token != '=')
		    continue;
		  GetToken(q,&q,token);
		  if (LocaleCompare(keyword,"file") == 0)
		    {
		      if (depth > 200)
			ThrowException(exception,ConfigureError,
				       IncludeElementNestedTooDeeply,path);
		      else
			{
			  char
			    filename[MaxTextExtent];

			  GetPathComponent(path,HeadPath,filename);
			  if (*filename != '\0')
			    (void) strlcat(filename,DirectorySeparator,sizeof(filename));
			  (void) strlcat(filename,token,sizeof(filename));
			  (void) ReadColorConfigureFile(filename,depth+1,exception);
			}
		      if (color_list != (ColorInfo *) NULL)
			while (color_list->next != (ColorInfo *) NULL)
			  color_list=color_list->next;
		    }
		}
	      continue;
	    }
	  if (LocaleCompare(keyword,"<color") == 0)
	    {
	      ColorInfo
		*color_info;

	      /*
		Allocate memory for the color list.
	      */
	      in_entry=MagickTrue;
	      color_info=MagickAllocateMemory(ColorInfo *,sizeof(ColorInfo));
	      if (color_info == (ColorInfo *) NULL)
		MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
				  UnableToAllocateColorInfo);
	      (void) memset(color_info,0,sizeof(ColorInfo));
	      color_info->path=AcquireString(path);
	      color_info->signature=MagickSignature;
	      if (color_list == (ColorInfo *) NULL)
		{
		  color_list=color_info;
		  continue;
		}
	      color_list->next=color_info;
	      color_info->previous=color_list;
	      color_list=color_list->next;
	      continue;
	    }
	  if (LocaleCompare(keyword,"/>") == 0)
	    {
	      /*
		Closing a color specification.
	      */
	      if (in_entry)
		{
		  /*
		    Remove any existing entry with same name (last one wins).
		  */
		  {
		    ColorInfo
		      *color_info;
	      
		    for (color_info=color_list->previous;
			 color_info != (ColorInfo *) NULL;
			 color_info=color_info->previous)
		      {
			if ((LocaleCompare(color_list->name,color_info->name) == 0) &&
			    (color_list->compliance == color_info->compliance))
			  {
			    DestroyColorInfoEntry(color_info);
			    break;
			  }
		      }
		  }
		  in_entry=MagickFalse;
		}
	    }
	  if (color_list == (ColorInfo *) NULL)
	    continue;
	  GetToken(q,(char **) NULL,token);
	  if (*token != '=')
	    continue;
	  GetToken(q,&q,token);
	  GetToken(q,&q,token);
	  switch (*keyword)
	    {
	    case 'B':
	    case 'b':
	      {
		if (LocaleCompare((char *) keyword,"blue") == 0)
		  {
		    color_list->color.blue=ScaleCharToQuantum(MagickAtoL(token));
		    break;
		  }
		break;
	      }
	    case 'C':
	    case 'c':
	      {
		if (LocaleCompare((char *) keyword,"compliance") == 0)
		  {
		    long
		      compliance;

		    compliance=color_list->compliance;
		    if (GlobExpression(token,"*SVG*"))
		      compliance|=SVGCompliance;
		    if (GlobExpression(token,"*X11*"))
		      compliance|=X11Compliance;
		    if (GlobExpression(token,"*XPM*"))
		      compliance|=XPMCompliance;
		    color_list->compliance=(ComplianceType) compliance;
		    break;
		  }
		break;
	      }
	    case 'G':
	    case 'g':
	      {
		if (LocaleCompare((char *) keyword,"green") == 0)
		  {
		    color_list->color.green=ScaleCharToQuantum(MagickAtoL(token));
		    break;
		  }
		break;
	      }
	    case 'N':
	    case 'n':
	      {
		if (LocaleCompare((char *) keyword,"name") == 0)
		  {
		    color_list->name=AcquireString(token);
		    break;
		  }
		break;
	      }
	    case 'O':
	    case 'o':
	      {
		if (LocaleCompare((char *) keyword,"opacity") == 0)
		  {
		    color_list->color.opacity=ScaleCharToQuantum(MagickAtoL(token));
		    break;
		  }
		break;
	      }
	    case 'R':
	    case 'r':
	      {
		if (LocaleCompare((char *) keyword,"red") == 0)
		  {
		    color_list->color.red=ScaleCharToQuantum(MagickAtoL(token));
		    break;
		  }
		break;
	      }
	    case 'S':
	    case 's':
	      {
		if (LocaleCompare((char *) keyword,"stealth") == 0)
		  {
		    color_list->stealth=LocaleCompare(token,"True") == 0;
		    break;
		  }
		break;
	      }
	    default:
	      break;
	    }
	}
      MagickFreeMemory(token);
      MagickFreeMemory(xml);
    }
  if (color_list == (ColorInfo *) NULL)
    return(MagickFail);
  while (color_list->previous != (ColorInfo *) NULL)
    color_list=color_list->previous;
  return(MagickPass);
}
