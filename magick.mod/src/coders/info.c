/*
% Copyright (C) 2010 - 2012 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/blob.h"
#include "magick/describe.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/tempfile.h"
#include "magick/utility.h"

/*
  Forward declaractions.
*/
static unsigned int
  WriteINFOImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r I N F O I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterINFOImage adds attributes for the INFO image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterINFOImage method is:
%
%      RegisterINFOImage(void)
%
*/
ModuleExport void RegisterINFOImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("INFO");
  entry->encoder=(EncoderHandler) WriteINFOImage;
  entry->description="Image descriptive information and statistics";
  entry->module="INFO";
  entry->extension_treatment=IgnoreExtensionTreatment;
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I N F O I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterINFOImage removes format registrations made by the
%  INFO module from the list of supported formats.
%
%  The format of the UnregisterINFOImage method is:
%
%      UnregisterINFOImage(void)
%
*/
ModuleExport void UnregisterINFOImage(void)
{
  (void) UnregisterMagickInfo("INFO");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I N F O I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteINFOImage() writes image descriptive info to stdout.
%
%  The format of the WriteINFOImage method is:
%
%      MagickPassFail WriteINFOImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o image_info: The image info.
%
%    o image:  The image.
%
%
*/
static MagickPassFail
WriteINFOImage(const ImageInfo *image_info,Image *image)
{
  MagickPassFail
    status;

  FILE
    *file;

  Image
    *list_entry;

  char
    temporary_filename[MaxTextExtent];

  const char
    *format;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);

  /*
    Obtain optional 'identify' style output specification.
  */
  format=AccessDefinition(image_info,"info","format");
  if (format != (char *) NULL)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
			  "info:format=\"%s\"",format);

  /*
    Open blob.
  */
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFail)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);

  /*
    Allocate and open a temporary file to write output to.
  */
  temporary_filename[0]='\0';
  if ((file=GetBlobFileHandle(image)) == (FILE *) NULL)
    {
      if(!AcquireTemporaryFileName(temporary_filename))
	ThrowWriterException(FileOpenError,UnableToCreateTemporaryFile,image);
      if ((file=fopen(temporary_filename,"w")) == (FILE *) NULL)
	{
	  (void) LiberateTemporaryFile(temporary_filename);
	  ThrowWriterException(FileOpenError,UnableToCreateTemporaryFile,image);
	}
      
    }
  list_entry=image;
  
  while (list_entry != (Image *) NULL)
    {
      /*
	Avoid convert style output syntax by restoring original filename.
      */
      (void) strlcpy(list_entry->filename,list_entry->magick_filename,
		     sizeof(list_entry->filename));

      /*
	Describe image.
      */
      if (format != (char *) NULL)
	{
	  char
	    *text;

	  text=TranslateText(image_info,list_entry,format);
	  if (text != (char *) NULL)
	    {
	      (void) fputs(text,file);
	      (void) fputs("\n",file);
	      MagickFreeMemory(text);
	    }
	}
      else
	{
	  if ((status=DescribeImage(list_entry,file,image_info->verbose))
	      == MagickFail)
	    break;
	}
      
      list_entry=GetNextImageInList(list_entry);
    }

  if ('\0' != temporary_filename[0])
    {
      /*
	Close temporary file.
      */
      (void) fclose(file);

      /*
	Send content of temporary file to blob stream.
      */
      if (WriteBlobFile(image,temporary_filename) == MagickFail)
	status=MagickFail;
      (void) LiberateTemporaryFile(temporary_filename);
    }

  CloseBlob(image);

  return status;
}
