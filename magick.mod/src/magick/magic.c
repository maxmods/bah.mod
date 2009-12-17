/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                    M   M   AAA    GGGG  IIIII   CCCC                        %
%                    MM MM  A   A  G        I    C                            %
%                    M M M  AAAAA  G GGG    I    C                            %
%                    M   M  A   A  G   G    I    C                            %
%                    M   M  A   A   GGGG  IIIII   CCCC                        %
%                                                                             %
%                                                                             %
%                    GraphicsMagick Image Magic Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                              Bob Friesenhahn                                %
%                                 July 2000                                   %
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
#include "magick/magic.h"
#include "magick/utility.h"

/*
  Define declarations.
*/

/*
  Static declarations.
*/
static const struct
{
  char
    *name;

  unsigned char
    *magic;

  unsigned int
    length,
    offset;
}
StaticMagic[] =
{
#define MAGIC(name,offset,magic) {name,(unsigned char *)magic,sizeof(magic)-1,offset}
  MAGIC("AVI", 0, "RIFF"),
  MAGIC("8BIMWTEXT", 0, "8\000B\000I\000M\000#"),
  MAGIC("8BIMTEXT", 0, "8BIM#"),
  MAGIC("8BIM", 0, "8BIM"),
  MAGIC("BMP", 0, "BA"),
  MAGIC("BMP", 0, "BM"),
  MAGIC("BMP", 0, "CI"),
  MAGIC("BMP", 0, "CP"),
  MAGIC("BMP", 0, "IC"),
  MAGIC("BMP", 0, "PI"),
  MAGIC("CALS", 21, "version: MIL-STD-1840"),
  MAGIC("CALS", 0, "srcdocid:"),
  MAGIC("CALS", 9, "srcdocid:"),
  MAGIC("CALS", 8, "rorient:"),
  MAGIC("CGM", 0, "BEGMF"),
  MAGIC("CIN", 0, "\200\052\137\327"),
  MAGIC("DCM", 128, "DICM"),
  MAGIC("DCX", 0, "\261\150\336\72"),
  MAGIC("DIB", 0, "\050\000"),
  MAGIC("DOT", 0, "digraph"),
  MAGIC("DPX", 0, "SDPX"),
  MAGIC("DPX", 0, "XPDS"),
  MAGIC("EMF", 40, "\040\105\115\106\000\000\001\000"),
  MAGIC("EPT", 0, "\305\320\323\306"),
  MAGIC("FAX", 0, "DFAX"),
  MAGIC("FIG", 0, "#FIG"),
  MAGIC("FITS", 0, "IT0"),
  MAGIC("FITS", 0, "SIMPLE"),
  MAGIC("FPX", 0, "\320\317\021\340"),
  MAGIC("GIF", 0, "GIF8"),
  MAGIC("GPLT", 0, "#!/usr/local/bin/gnuplot"),
  MAGIC("HDF", 1, "HDF"),
  MAGIC("HPGL", 0, "IN;"),
  MAGIC("HPGL", 0, "\033E\033"),
  MAGIC("HTML", 1, "HTML"),
  MAGIC("HTML", 1, "html"),
  MAGIC("ILBM", 8, "ILBM"),
  MAGIC("IPTCWTEXT", 0, "\062\000#\000\060\000=\000\042\000&\000#\000\060\000;\000&\000#\000\062\000;\000\042\000"),
  MAGIC("IPTCTEXT", 0, "2#0=\042&#0;&#2;\042"),
  MAGIC("IPTC", 0, "\034\002"),
  MAGIC("JNG", 0, "\213JNG\r\n\032\n"),
  MAGIC("JPEG", 0, "\377\330\377"),
  MAGIC("JPC", 0, "\377\117"),
  MAGIC("JP2", 4, "\152\120\040\040\015"),
  MAGIC("MIFF", 0, "Id=ImageMagick"),
  MAGIC("MIFF", 0, "id=ImageMagick"),
  MAGIC("MNG", 0, "\212MNG\r\n\032\n"),
  MAGIC("MPC", 0, "id=MagickCache"),
  MAGIC("MPEG", 0, "\000\000\001\263"),
  MAGIC("PCD", 2048, "PCD_"),
  MAGIC("PCL", 0, "\033E\033"),
  MAGIC("PCX", 0, "\012\002"),
  MAGIC("PCX", 0, "\012\005"),
  MAGIC("PDB", 60, "vIMGView"),
  MAGIC("PDF", 0, "%PDF-"),
  MAGIC("PFA", 0, "%!PS-AdobeFont-1.0"),
  MAGIC("PFB", 6, "%!PS-AdobeFont-1.0"),
  MAGIC("PGX", 0, "\050\107\020\115\046"),
  MAGIC("PICT", 522, "\000\021\002\377\014\000"),
  MAGIC("PNG", 0, "\211PNG\r\n\032\n"),
  MAGIC("PNM", 0, "P1"),
  MAGIC("PNM", 0, "P2"),
  MAGIC("PNM", 0, "P3"),
  MAGIC("PNM", 0, "P4"),
  MAGIC("PNM", 0, "P5"),
  MAGIC("PNM", 0, "P6"),
  MAGIC("PNM", 0, "P7"),
  MAGIC("PS", 0, "%!"),
  MAGIC("PS", 0, "\004%!"),
  MAGIC("PS", 0, "\305\320\323\306"),
  MAGIC("PSD", 0, "8BPS"),
  MAGIC("PWP", 0, "SFW95"),
  MAGIC("RAD", 0, "#?RADIANCE"),
  MAGIC("RAD", 0, "VIEW= "),
  MAGIC("RLE", 0, "\122\314"),
  MAGIC("SCT", 0, "CT"),
  MAGIC("SFW", 0, "SFW94"),
  MAGIC("SGI", 0, "\001\332"),
  MAGIC("SUN", 0, "\131\246\152\225"),
  MAGIC("SVG", 1, "?XML"),
  MAGIC("SVG", 1, "?xml"),
  MAGIC("TIFF", 0, "\115\115\000\052"),
  MAGIC("TIFF", 0, "\111\111\052\000"),
  MAGIC("BIGTIFF", 0, "\115\115\000\053\000\010\000\000"),
  MAGIC("BIGTIFF", 0, "\111\111\053\000\010\000\000\000"),
  MAGIC("VICAR", 0, "LBLSIZE"),
  MAGIC("VICAR", 0, "NJPL1I"),
  MAGIC("VIFF", 0, "\253\001"),
  MAGIC("WMF", 0, "\327\315\306\232"),
  MAGIC("WMF", 0, "\001\000\011\000"),
  MAGIC("WPG", 0, "\377WPC"),
  MAGIC("XBM", 0, "#define"),
  MAGIC("XCF", 0, "gimp xcf"),
  MAGIC("XPM", 1, "* XPM *"),
  MAGIC("XWD", 4, "\007\000\000"),
  MAGIC("XWD", 5, "\000\000\007")
};

/*
  Forward declarations.
*/

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e s t r o y M a g i c I n f o                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method DestroyMagicInfo deallocates memory associated with the magic list.
%
%  The format of the DestroyMagicInfo method is:
%
%      DestroyMagicInfo(void)
%
%
*/
MagickExport void
DestroyMagicInfo(void)
{
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   G e t M a g i c k F i l e F o r m a t                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method GetMagickFileFormat inspects the provided image file/blob header
%  bytes and sets/updates the provided file format string buffer. The value
%  MagickPass is returned if the format was successfully identified.  The
%  value MagickFail is returned if the format was not identified or an
%  exception occured.
%  
%
%  The format of the GetMagickFileFormat method is:
%
%      MagickPassFail GetMagickFileFormat(
%        const unsigned char *header, const size_t header_length,
%        char *postulate, const size_t postulate_length,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o header: A binary string representing the first several
%        thousand bytes of the file/blob header to test.
%
%    o header_length: The length of the binary signature.  Currently
%        2*MaxTextExtent (4106 bytes).
%
%    o format: Format buffer. Updated with actual format if it can be
%        determined.
%
%    o format_length: Format buffer length.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail
GetMagickFileFormat(const unsigned char *header, const size_t header_length,
                    char *format, const size_t format_length,
                    ExceptionInfo *exception)
{
  register unsigned int
    i;

  MagickPassFail
    status=MagickFail;

  ARG_NOT_USED(exception);

  if (!((header == (const unsigned char *) NULL) || (header_length == 0) ||
	(format_length < 2)))
    {
      /*
	Search for requested magic.
      */
      for (i=0; i < sizeof(StaticMagic)/sizeof(StaticMagic[0]); i++)
	{
	  if (StaticMagic[i].offset+StaticMagic[i].length <= header_length)
	    {
	      if ((header[StaticMagic[i].offset] == StaticMagic[i].magic[0]) &&
		  (memcmp(header+StaticMagic[i].offset,StaticMagic[i].magic,
			  StaticMagic[i].length) == 0))
		{
		  if (strlcpy(format,StaticMagic[i].name,format_length) < format_length)
		    status=MagickPass;
		  break;
		}
	    }
	}
    }
  return status;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e M a g i c I n f o                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeMagicInfo initializes the file header magic detection
%  facility.
%
%  The format of the InitializeMagicInfo method is:
%
%      MagickPassFail InitializeMagicInfo(void)
%
%
*/
MagickExport MagickPassFail
InitializeMagicInfo(void)
{
  return MagickPass;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  L i s t M a g i c I n f o                                                  %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ListMagicInfo lists the magic info to a file.
%
%  The format of the ListMagicInfo method is:
%
%      unsigned int ListMagicInfo(FILE *file,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o file:  An pointer to a FILE.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/
MagickExport MagickPassFail
ListMagicInfo(FILE *file,ExceptionInfo *exception)
{
  register unsigned int
    i,
    j;

  ARG_NOT_USED(exception);

  if (file == (const FILE *) NULL)
    file=stdout;

  (void) fprintf(file,"Name      Offset Target\n");
  (void) fprintf(file,"-------------------------------------------------"
		 "------------------------------\n");
  for (i=0; i < sizeof(StaticMagic)/sizeof(StaticMagic[0]); i++)
    {
      register const unsigned char
	*c;

      (void) fprintf(file,"%.1024s",StaticMagic[i].name);
      for (j= (unsigned int) strlen(StaticMagic[i].name); j <= 9; j++)
	(void) fprintf(file," ");
      (void) fprintf(file,"%6u ",StaticMagic[i].offset);

      (void) fprintf(file,"\"");
      c=StaticMagic[i].magic;
      for (j=0; j < StaticMagic[i].length; j++)
	{
/* 	    else if ('\b' == c[j]) */
/* 	      (void) fprintf(file,"\\b"); */
/* 	    else if ('\f' == c[j]) */
/* 	      (void) fprintf(file,"\\f"); */
	  if ('\n' == c[j])
	    (void) fprintf(file,"\\n");
	  else if ('\r' == c[j])
	    (void) fprintf(file,"\\r");
	  else if ('\t' == c[j])
	      (void) fprintf(file,"\\t");
/* 	    else if ('\v' == c[j]) */
/* 	      (void) fprintf(file,"\\v"); */
/* 	    else if ('\a' == c[j]) */
/* 	      (void) fprintf(file,"\\a"); */
	  else if ('\\' == c[j])
	    (void) fprintf(file,"\\");
	  else if ('\?' == c[j])
	    (void) fprintf(file,"\\?");
	  else if ('"' == c[j])
	    (void) fprintf(file,"\\\"");
	  else if (isprint((int) c[j]))
	    (void) fprintf(file,"%c",(int) c[j]);
	  else
	    {
	      (void) fprintf(file,"\\%03o",c[j]);
	    }
	}
      (void) fprintf(file,"\"\n");
    }
  (void) fflush(file);
  return(MagickPass);
}
