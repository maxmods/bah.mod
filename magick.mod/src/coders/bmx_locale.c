/*
% Copyright (C) 2003 GraphicsMagick Group
% Copyright (C) 2002 ImageMagick Studio
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                  L       OOO    CCCC   AAA   L      EEEEE                   %
%                  L      O   O  C      A   A  L      E                       %
%                  L      O   O  C      AAAAA  L      EEE                     %
%                  L      O   O  C      A   A  L      E                       %
%                  LLLLL   OOO    CCCC  A   A  LLLLL  EEEEE                   %
%                                                                             %
%                   Read/Write GraphicsMagick Locale Files                    %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               Kyle Shorter                                  %
%                               September 2002                                %
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
#include "magick/list.h"
#include "magick/log.h"
#include "magick/magick.h"
#include "magick/utility.h"

/*
  Forward declarations.
*/
static unsigned int
  WriteLOCALEImage(const ImageInfo *,Image *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   R e a d C o n f i g u r e F i l e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadConfigureFile() reads the locale configuration file which maps text
%  strings to localized forms.
%
%  The format of the ReadConfigureFile method is:
%
%      unsigned int ReadConfigureFile(Image *image,const char *basename,
%        const unsigned long depth,ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o status: Method ReadConfigureFile returns True if at least one color
%      is defined otherwise False.
%
%    o image: The image.
%
%    o basename:  The color configuration filename.
%
%    o depth: depth of <include /> statements.
%
%    o exception: Return any errors or warnings in this structure.
%
%
*/

static void ChopLocaleComponents(char *path,const unsigned long components)
{
  long
    count;

  register char
    *p;

  if (*path == '\0')
    return;
  p=path+strlen(path)-1;
  if (*p == '/')
    *p='\0';
  for (count=0; (count < (long) components) && (p > path); p--)
    if (*p == '/')
      {
        *p='\0';
        count++;
      }
}

static unsigned int ReadConfigureFile(Image *image,const char *basename,
  const unsigned long depth,ExceptionInfo *exception)
{
  char
    keyword[MaxTextExtent],
    locale[MaxTextExtent],
    message[MaxTextExtent],
    path[MaxTextExtent],
    *q,
    *token,
    *xml;

  register char
    *p;

  size_t
    length;

  /*
    Read the locale configure file.
  */
  (void) strlcpy(path,basename,sizeof(path));
  xml=(char *) FileToBlob(basename,&length,exception);
  if (xml == (char *) NULL)
    return(False);
  (void) strlcpy(locale,"/",sizeof(locale));
  token=AllocateString(xml);
  for (q=xml; *q != '\0'; )
  {
    /*
      Interpret XML.
    */
    GetToken(q,&q,token);
    if (*token == '\0')
      break;
    (void) strlcpy(keyword,token,MaxTextExtent);
    if (LocaleNCompare(keyword,"<!--",4) == 0)
      {
        char
          comment[MaxTextExtent];

        /*
          Comment element.
        */
        p=q;
        while ((LocaleNCompare(q,"->",2) != 0) && (*q != '\0'))
          GetToken(q,&q,token);
        length=Min(q-p-2,MaxTextExtent-1);
        (void) strncpy(comment,p+1,length);
        comment[length]='\0';
        (void) SetImageAttribute(image,"[LocaleComment]",comment);
        (void) SetImageAttribute(image,"[LocaleComment]","\n");
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
                ThrowException(exception,ConfigureError,IncludeElementNestedTooDeeply,path);
              else
                {
                  char
                    filename[MaxTextExtent];

                  filename[0]='\0';
                  GetPathComponent(path,HeadPath,filename);
                  if (filename[0] != '\0')
                    (void) strlcat(filename,DirectorySeparator,sizeof(filename));
                  (void) strlcat(filename,token,sizeof(filename));
                  (void) ReadConfigureFile(image,filename,depth+1,exception);
                }
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"<locale") == 0)
      {
        /*
          Locale element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strlcpy(keyword,token,MaxTextExtent);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"name") == 0)
            {
              (void) strlcpy(locale,token,MaxTextExtent);
              (void) strlcat(locale,"/",MaxTextExtent);
            }
        }
        continue;
      }
    if (LocaleCompare(keyword,"</locale>") == 0)
      {
        ChopLocaleComponents(locale,1);
        (void) strcat(locale,"/");
        continue;
      }
    if (LocaleCompare(keyword,"<localemap>") == 0)
      continue;
    if (LocaleCompare(keyword,"</localemap>") == 0)
      continue;
    if (LocaleCompare(keyword,"<message") == 0)
      {
        /*
          Message element.
        */
        while ((*token != '>') && (*q != '\0'))
        {
          (void) strlcpy(keyword,token,MaxTextExtent);
          GetToken(q,&q,token);
          if (*token != '=')
            continue;
          GetToken(q,&q,token);
          if (LocaleCompare(keyword,"name") == 0)
            {
              (void) strlcat(locale,token,sizeof(locale));
              (void) strlcat(locale,"/",sizeof(locale));
            }
        }
        for (p=q; (*q != '<') && (*q != '\0'); q++);
        {
          (void) strncpy(message,p,(size_t)(q-p));
          message[q-p]='\0';
          Strip(message);
          (void) strlcat(locale,message,sizeof(locale));
          (void) strlcat(locale,"\n",sizeof(locale));
          (void) SetImageAttribute(image,"[Locale]",locale);
        }
        continue;
      }
    if (LocaleCompare(keyword,"</message>") == 0)
      {
        ChopLocaleComponents(locale,2);
        (void) strcat(locale,"/");
        continue;
      }
    if (*keyword == '<')
      {
        /*
          Subpath element.
        */
        if (*(keyword+1) == '?')
          continue;
        if (*(keyword+1) == '/')
          {
            ChopLocaleComponents(locale,1);
            (void) strcat(locale,"/");
            continue;
          }
        token[strlen(token)-1]='\0';
        (void) strcpy(token,token+1);
        (void) strlcat(locale,token,sizeof(locale));
        (void) strlcat(locale,"/",sizeof(locale));
        continue;
      }
    GetToken(q,(char **) NULL,token);
    if (*token != '=')
      continue;
  }
  MagickFreeMemory(token);
  MagickFreeMemory(xml);
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d L O C A L E I m a g e                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method ReadLOCALEImage reads a Magick Configure File as a blob an attaches
%  it as an image attribute to a proxy image.  It allocates the memory
%  necessary for the new Image structure and returns a pointer to the new
%  image.
%
%  The format of the ReadLOCALEImage method is:
%
%      Image *ReadLOCALEImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image:  Method ReadLOCALEImage returns a pointer to the image after
%      reading.  A null image is returned if there is a memory shortage or
%      if the image cannot be read.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o exception: return any errors or warnings in this structure.
%
%
*/
static Image *ReadLOCALEImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  Image
    *image;

  unsigned int
    status;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickSignature);
  image=AllocateImage(image_info);
  status=OpenBlob(image_info,image,ReadBlobMode,exception);
  if (status == False)
    ThrowReaderException(FileOpenError,UnableToOpenFile,image);
  if (status == False)
    {
      DestroyImage(image);
      return((Image *) NULL);
    }
  image->columns=1;
  image->rows=1;
  (void) SetImage(image,OpaqueOpacity);
  status=ReadConfigureFile(image,image->filename,0,exception);
  return(image);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r L O C A L E I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method RegisterLOCALEImage adds attributes for the LOCALE image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterLOCALEImage method is:
%
%      RegisterLOCALEImage(void)
%
*/
ModuleExport void RegisterLOCALEImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("LOCALE");
  entry->decoder=(DecoderHandler) ReadLOCALEImage;
  entry->encoder=(EncoderHandler) WriteLOCALEImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Locale Message File";
  entry->module="LOCALE";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("LOCALEMC");
  entry->encoder=(EncoderHandler) WriteLOCALEImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Microsoft Message File";
  entry->module="LOCALE";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("LOCALEC");
  entry->encoder=(EncoderHandler) WriteLOCALEImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Locale Message File - C code";
  entry->module="LOCALE";
  (void) RegisterMagickInfo(entry);

  entry=SetMagickInfo("LOCALEH");
  entry->encoder=(EncoderHandler) WriteLOCALEImage;
  entry->adjoin=False;
  entry->stealth=True;
  entry->description="Locale Message File - C header file";
  entry->module="LOCALE";
  (void) RegisterMagickInfo(entry);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r L O C A L E I m a g e                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method UnregisterLOCALEImage removes format registrations made by the
%  LOCALE module from the list of supported formats.
%
%  The format of the UnregisterLOCALEImage method is:
%
%      UnregisterLOCALEImage(void)
%
*/
ModuleExport void UnregisterLOCALEImage(void)
{
  (void) UnregisterMagickInfo("LOCALE");
  (void) UnregisterMagickInfo("LOCALEMC");
  (void) UnregisterMagickInfo("LOCALEC");
  (void) UnregisterMagickInfo("LOCALEH");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e L O C A L E I m a g e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteLOCALEImage() writes a Magick Configure File as C source.
%
%  The format of the WriteLOCALEImage method is:
%
%      unsigned int WriteLOCALEImage(const ImageInfo *image_info,Image *image)
%
%  A description of each parameter follows.
%
%    o status: Method WriteLOCALEImage return True if the image is written.
%      False is returned is there is a memory shortage or if the image file
%      fails to write.
%
%    o image_info: Specifies a pointer to a ImageInfo structure.
%
%    o image:  A pointer to an Image structure.
%
%
*/

struct locale_str {
    struct locale_str *next;  /* link list of subfield names at this level */
    struct locale_str *lower; /* link list of subfield names below this level */
    char *name;               /* the subfield name or the message */
};

static void accumulate(const char **, int, struct locale_str **);
static void output_switches(Image *image,struct locale_str *, int, int);

#define INDENT 2         /* # of spaces to indent each line of the output */

static const char prologue[] = "/* This method is autogenerated-- do not edit */\
\nconst char *GetLocaleMessage(const char *tag)\
\n{\
\n#define NEXT_FIELD ((p = (np = strchr((tp = np), '/')) ? np++ : (np = tp + strlen(tp))), tp)\n\
\n\
\n   static const char *locale = 0;\
\n   register const char *p, *tp, *np;\
\n   if (!tag || *tag == '\\0')\
\n      return \"\";\n\
\n   if ( (!locale &&\
\n         ( (!(locale = setlocale(LC_CTYPE, 0)) || *locale == '\\0') &&\
\n           (!(locale = getenv(\"LC_ALL\"))       || *locale == '\\0') &&\
\n           (!(locale = getenv(\"LC_MESSAGES\"))     || *locale == '\\0') &&\
\n           (!(locale = getenv(\"LC_CTYPE\"))     || *locale == '\\0') &&\
\n           (!(locale = getenv(\"LANG\"))         || *locale == '\\0') ) )\
\n         || !LocaleCompare(locale, \"C\"))\
\n      locale = \"C\";\n\
\n   if (!LocaleNCompare(locale,\"en_US\",5))\
\n      locale = \"C\";\n\
\n   locale = \"C\";\n\
\n   tp = locale;\n   p = locale + strlen(locale);\n   np = tag;\n";

static const char epilogue[] = "\n   return tag;\n}\n";

static char *EscapeLocaleString(const char *str)
{
    const char *p;
    char *strput, *s;
    size_t n;

    for (p = str, n = 0; *p; p++, n++)
        if (*p == '"' || *p == '\\')
            ++n;

    if (!(strput = MagickAllocateMemory(char *,n + 1)))
    {
        (void) fprintf(stderr, "out of memory!\n");
        exit(1);
    }

    for (p = str, s = strput; *p; p++)
    {
        if (*str == '\\' || *str == '"')
            *s++ = '\\';
        *s++ = *p;
    }

    *s = '\0';
    return strput;
}

static void FreeAccumulatedStrings(void *handle)
{
    struct locale_str *xl = (struct locale_str *)handle;
    if (!handle)
        return;
    FreeAccumulatedStrings((void *)xl->next);
    FreeAccumulatedStrings((void *)xl->lower);
    MagickFreeMemory(xl->name);
    MagickFreeMemory(handle);
}

/*  accumulate -- read a line from the file, break it up at the '/'s into
 *                individual subfields and build a tree structure that has
 *                a string message (the last subfield) as its leaf node.
 */
static void accumulate(const char **buf, int siz, struct locale_str **locstr)
{
    const char *p, *np, *tp;
    char *xp, *xn, *xt;
    struct locale_str *xl, **xloc;
    int n;

    for (n = 0; (siz == 0 || n < siz) && buf[n]; n++)
    {
        xloc = locstr;

        /* break the line into separate fields, setting up as follows: */
        /* tp points at the first char of the current subfield */
        /* np points at the first char of the next subfield */
        /* p points one after the last char of the current subfield */
        for (p = tp = buf[n]; p && *p; p = np)
        {
            if (!(np = strchr(p, '/')))    /* last field is the message */
            {
                if (!(xp = (char *) AllocateString(tp)))
                {
                    (void) fprintf(stderr, " out of memory!\n");
                    exit(1);
                }
                xt = xp;
                for (xn = xp; (*xn = *xt); xt++)      /* eliminate '\'s */
                    if (*xn != '\\')
                        ++xn;

                if (*xloc && !(*xloc)->lower)   /* see if already there */
                {
                    if (LocaleCompare((*xloc)->name, xp))
                        (void) fprintf(stderr, "ignoring dup message for `%s'\n",
                                       buf[n]);
                    MagickFreeMemory(xp);
                    break;
                }
                /* fall through to create the node */
            }
            else if (np == tp)      /* skip multiple /'s */
            {
                ++np, ++tp;
                continue;
            }
            else if (np[-1] == '\\')  /* this '/' has been escaped */
            {
                ++np;
                continue;
            }
            else    /* subfield name */
            {
                int x;

                if (!(xp = MagickAllocateMemory(char *,(size_t) (np - tp + 1))))
                {
                    (void) fprintf(stderr, "out of memory!\n" );
                    exit(1);
                }
                (void) strlcpy(xp, tp, (size_t) (np - tp + 1));
                tp = ++np;

                if (*xloc && !(*xloc)->lower) /* skip leaf node if it's there */
                    xloc = &(*xloc)->next;
                for (x = -1; (xl = *xloc); xloc = &xl->next)
                    if ((x = LocaleCompare(xp, xl->name)) <= 0)
                        break;

                if (x == 0)   /* subfield exists */
                {
                    MagickFreeMemory(xp);
                    xloc = &xl->lower;
                    continue;
                }
            }

            /* node doesn't exist; create it */

            if (*xp == '\0')
                (void) fprintf(stderr, "warning: message is null for '%s'\n", buf[n]);

            if (!(xl = MagickAllocateMemory(struct locale_str *,sizeof *xl)))
            {
                (void) fprintf(stderr, "out of memory!\n");
                exit(1);
            }
            xl->name = xp;
            xl->lower = 0;
            xl->next = *xloc;
            *xloc = xl;
            xloc = &xl->lower;
        }
    }
}

/*  output_switches -- generate a single C statement from the list of names
 *                     in the locstr tree. This is a recursive procedure.
 *                     `indent' is the output line indentation to make the
 *                     generated code readable.  `elseflag' is < 0 the first
 *                     time this routine is called, otherwise it's just used
 *                     to line up "else if" in the output a little better.
 */
static void output_switches(Image *image,struct locale_str *locstr, int indent, int elseflag)
{
   char
     message[10*MaxTextExtent];

    int flag;
    struct locale_str *xl;
    char *p, *field = (char *) "NEXT_FIELD";

    if (!locstr)
    {
        (void) fprintf(stderr, "NULL locstr in output_switches\n");
        return;
    }

    if (elseflag < 0)
        field = (char *) "locale", elseflag = 0;

    if (!locstr->next)     /* output simpler code for a single case */
    {
        p = EscapeLocaleString(locstr->name);
        if (!locstr->lower)
          {
            FormatString(message, "%*sreturn *np ? tag : \"%s\";\n", indent, "", p);
            (void) WriteBlobString(image,message);
          }
        else
        {
            if (elseflag)
                indent -= INDENT;

            FormatString(message, "%*sif (LocaleNCompare(%s, \"%s\", %ld) || p - tp != %ld)\n%*sreturn tag;\n%*selse\n",
                    indent, "", field, p, (long) strlen(locstr->name),
                    (long) strlen(locstr->name), indent+INDENT, "", indent, "");
            (void) WriteBlobString(image,message);

            output_switches(image, locstr->lower, indent+INDENT, 1);
        }
        MagickFreeMemory(p);
        return;
    }

    FormatString(message, "%*sswitch (*%s)\n%*s{\n%*sdefault:\n%*sreturn tag;\n",
                indent, "", field, indent, "", indent, "", indent+INDENT, "");

    (void) WriteBlobString(image,message);
    xl = locstr;
    if (!xl->lower)
    {
        p = EscapeLocaleString(xl->name);
        FormatString(message, "\n%*scase '\\0':\n%*sreturn \"%s\";\n",
                indent, "", indent+INDENT, "", p);
        (void) WriteBlobString(image,message);
        MagickFreeMemory(p);
        xl = xl->next;
    }

    for (flag = 1; xl; xl = xl->next)
    {
        if (flag)
          {
            FormatString(message, "\n%*scase '%c':  case '%c':\n",
                    indent, "", tolower(*xl->name), toupper(*xl->name));
            (void) WriteBlobString(image,message);
          }

        p = EscapeLocaleString(xl->name);
        FormatString(message, "%*sif (p - tp == %ld && !LocaleNCompare(tp, \"%s\", %ld))\n",
                indent+INDENT, "", (long) strlen(xl->name), p, (long) strlen(xl->name));
        (void) WriteBlobString(image,message);
        MagickFreeMemory(p);

        output_switches(image,xl->lower, indent+INDENT+INDENT, 0);
        FormatString(message, "%*selse\n", indent+INDENT, "");
        (void) WriteBlobString(image,message);

        flag = xl->next == 0 || tolower(*xl->name) != tolower(*xl->next->name);
        if (flag)
          {
            FormatString(message, "%*sreturn tag;\n", indent+INDENT+INDENT, "");
            (void) WriteBlobString(image,message);
          }
    }

    FormatString(message, "%*s}\n", indent, "");
    (void) WriteBlobString(image,message);
}

void WriteBlobStringEOL(Image *image)
{
#if defined(MSWINDOWS)
  (void) WriteBlobString(image,"\r\n");
#else
  (void) WriteBlobString(image,"\n");
#endif
}

void WriteBlobStringWithEOL(Image *image,const char *s)
{
  (void) WriteBlobString(image,s);
#if defined(MSWINDOWS)
  (void) WriteBlobString(image,"\r\n");
#else
  (void) WriteBlobString(image,"\n");
#endif
}

#define TREE_LEVELS_SUPPORTED 5

static unsigned int WriteLOCALEImage(const ImageInfo *image_info,Image *image)
{
  char
    **locale;

  const ImageAttribute
    *attribute;

  register int
    i,
    j;

  struct locale_str
    *locales;

  unsigned int
    status;

  int
    count;

  /*
    Open output locale file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == False)
    ThrowWriterException(FileOpenError,UnableToOpenFile,image);
  attribute=GetImageAttribute(image,"[Locale]");
  if (attribute == (const ImageAttribute *) NULL)
    ThrowWriterException(ImageError,NoLocaleImageAttribute,image);
  locale=StringToList(attribute->value);
  if (locale == (char **) NULL)
    ThrowWriterException(ResourceLimitError,MemoryAllocationFailed,image);
  /*
    Sort locale messages.
  */
  for (i=0; locale[i] != (char *) NULL; i++);
  count=i-1;
  for (i=0; i < count; i++)
    for (j=(i+1); j < count; j++)
      if (LocaleCompare(locale[i],locale[j]) > 0)
        {
          char
            *swap;

          swap=locale[i];
          locale[i]=locale[j];
          locale[j]=swap;
        }
  if (IsEventLogging())
    for (i=0; i < count; i++)
      (void) LogMagickEvent(LocaleEvent,GetMagickModule(),"%.1024s",locale[i]);
  if (LocaleCompare(image_info->magick,"LOCALEMC") == 0)
    {
      /*
        Write microsoft message compiler message file format.
      */
      for (i=0; i < count; i++)
      {
        char
          *fields[4],
          text[MaxTextExtent],
          path[MaxTextExtent];

        int
          index;

        register char
          *p;

        (void) strlcpy(path,locale[i],sizeof(path));
        if (*path != '\0')
          {
            p=path+strlen(path)-1;
            if (*p == '/')
              *p='\0';
            for (index=0; (index < 4) && (p > path); p--)
            {
              if (*p == '/')
                {
                  *p='\0';
                  fields[3-index]=p+1;
                  index++;
                }
            }
            (void) WriteBlobString(image,"\r\n");
            if (i == 0)
              {
               (void) WriteBlobString(image,"MessageId       = 0\r\nSymbolicName    = GENERIC_MESSAGE\r\nLanguage        = English\r\n%1\r\n.\r\n");
               (void) WriteBlobString(image,"MessageId       = 1\r\n");
              }
            else
              (void) WriteBlobString(image,"MessageId       = +1\r\n");
            FormatString(text, "SymbolicName    = %s%s%s\r\n", fields[0],fields[1],fields[2]);
            (void) WriteBlobString(image,text);
            (void) WriteBlobString(image,"Language        = English\r\n");
            FormatString(text, "%s\r\n", fields[3]);
            (void) WriteBlobString(image,text);
            (void) WriteBlobString(image,".\r\n");
          }
      }
    }
  else if (LocaleCompare(image_info->magick,"LOCALEH") == 0)
    {
      const char
        *fields[TREE_LEVELS_SUPPORTED];

      char
        last[MaxTextExtent],
        last2[MaxTextExtent],
        category[MaxTextExtent],
        severity[MaxTextExtent],
        text[MaxTextExtent],
        path[MaxTextExtent];

      int
        index,
        severityindex;

      register char
        *p;

      WriteBlobStringWithEOL(image,"#ifndef _LOCAL_C_H");
      WriteBlobStringWithEOL(image,"#define _LOCAL_C_H");
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"extern MagickExport const char *GetLocaleMessageFromID(const int);");
      WriteBlobStringEOL(image);
      FormatString(text, "#define MAX_LOCALE_MSGS %d",count);
      WriteBlobStringWithEOL(image,text);
      WriteBlobStringEOL(image);
      /*
        Write series of #defines for all the messages in the system.
      */
      for (i=0; i < count; i++)
      {
        (void) strlcpy(path,locale[i],sizeof(path));
        if (*path != '\0')
          {
            p=path+strlen(path)-1;
            if (*p == '/')
              *p='\0';
            fields[0]=""; /* this one may not exist */
            for (index=0; (index < TREE_LEVELS_SUPPORTED) && (p > path); p--)
            {
              if (*p == '/')
                {
                  *p='\0';
                  fields[4-index]=p+1;
                  index++;
                }
            }
            FormatString(text, "#define MGK_%s%s%s%s %d",fields[0],fields[1],
              fields[2],fields[3],i+1);
            WriteBlobStringWithEOL(image,text);
          }
      }
      /*
        Write a table that maps category strings over to severity id's
      */
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"#endif");
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"#if defined(_INCLUDE_CATEGORYMAP_TABLE_)");
      WriteBlobStringWithEOL(image,"typedef struct _CategoryInfo{");
      WriteBlobStringWithEOL(image,"  const char *name;");
      WriteBlobStringWithEOL(image,"  int offset;");
      WriteBlobStringWithEOL(image,"} CategoryInfo;");
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"static const CategoryInfo category_map[] =");
      WriteBlobStringWithEOL(image,"  {");
      last[0]='\0';
      last2[0]='\0';
      severityindex=0;
      for (i=0; i < count; i++)
      {
        (void) strlcpy(path,locale[i],sizeof(path));
        if (*path != '\0')
          {
            p=path+strlen(path)-1;
            if (*p == '/')
              *p='\0';
            fields[0]=""; /* this one may not exist */
            for (index=0; (index < TREE_LEVELS_SUPPORTED) && (p > path); p--)
            {
              if (*p == '/')
                {
                  *p='\0';
                  fields[4-index]=p+1;
                  index++;
                }
            }
            FormatString(category, "%s%s",fields[0], fields[1]);
            FormatString(severity, "%s%s%s",fields[0], fields[1], fields[2]);
            if (LocaleCompare(severity,last2) != 0)
              {
                severityindex++;
                (void) strlcpy(last2,severity,MaxTextExtent);
              }
           if (LocaleCompare(category,last) != 0)
              {
                FormatString(text, "    { \"%s%s%s\", %d },", fields[0],
                  strlen(fields[0]) ? "/" : "", fields[1], severityindex-1);
                WriteBlobStringWithEOL(image,text);
                (void) strlcpy(last,category,MaxTextExtent);
              }
          }
      }
      FormatString(text, "    { 0, %d }",severityindex-1);
      WriteBlobStringWithEOL(image,text);
      WriteBlobStringWithEOL(image,"  };");
      WriteBlobStringWithEOL(image,"#endif");
      /*
        Write a table that maps severity strings over to severity id's
      */
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"#if defined(_INCLUDE_SEVERITYMAP_TABLE_)");
      WriteBlobStringWithEOL(image,"typedef struct _SeverityInfo{");
      WriteBlobStringWithEOL(image,"  const char *name;");
      WriteBlobStringWithEOL(image,"  int offset;");
      WriteBlobStringWithEOL(image,"  ExceptionType severityid;");
      WriteBlobStringWithEOL(image,"} SeverityInfo;");
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"static const SeverityInfo severity_map[] =");
      WriteBlobStringWithEOL(image,"  {");
      last[0]='\0';
      for (i=0; i < count; i++)
      {
        (void) strlcpy(path,locale[i],sizeof(path));
        if (*path != '\0')
          {
            p=path+strlen(path)-1;
            if (*p == '/')
              *p='\0';
            fields[0]=""; /* this one may not exist */
            for (index=0; (index < TREE_LEVELS_SUPPORTED) && (p > path); p--)
            {
              if (*p == '/')
                {
                  *p='\0';
                  fields[4-index]=p+1;
                  index++;
                }
            }
            FormatString(severity, "%s%s%s",fields[0], fields[1], fields[2]);
            if (LocaleCompare(severity,last) != 0)
              {
                FormatString(text, "    { \"%s%s%s/%s\", %d, %s },", fields[0],
                  strlen(fields[0]) ? "/" : "", fields[1], fields[2], i,
                  severity);
                WriteBlobStringWithEOL(image,text);
                (void) strlcpy(last,severity,MaxTextExtent);
              }
          }
      }
      FormatString(text, "    { 0, %d, UndefinedException }",count);
      WriteBlobStringWithEOL(image,text);
      WriteBlobStringWithEOL(image,"  };");
      WriteBlobStringWithEOL(image,"#endif");
      /*
        Write a table that maps tag strings over to messages id's
      */
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"#if defined(_INCLUDE_TAGMAP_TABLE_)");
      WriteBlobStringWithEOL(image,"typedef struct _MessageInfo");
      WriteBlobStringWithEOL(image,"{");
      WriteBlobStringWithEOL(image,"  const char *name;");
      WriteBlobStringWithEOL(image,"  int messageid;");
      WriteBlobStringWithEOL(image,"} MessageInfo;");
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"static const MessageInfo message_map[] =");
      WriteBlobStringWithEOL(image,"  {");
      for (i=0; i < count; i++)
      {
        (void) strlcpy(path,locale[i],sizeof(path));
        if (*path != '\0')
          {
            p=path+strlen(path)-1;
            if (*p == '/')
              *p='\0';
            fields[0]=""; /* this one may not exist */
            for (index=0; (index < TREE_LEVELS_SUPPORTED) && (p > path); p--)
            {
              if (*p == '/')
                {
                  *p='\0';
                  fields[4-index]=p+1;
                  index++;
                }
            }
            FormatString(text, "    { \"%s\", %d },", fields[3], i+1);
            WriteBlobStringWithEOL(image,text);
          }
      }
      WriteBlobStringWithEOL(image,"    { 0, 0 }");
      WriteBlobStringWithEOL(image,"  };");
      WriteBlobStringWithEOL(image,"#endif");
      /*
        Write a table of all the messages indexed by the message id.
      */
      WriteBlobStringEOL(image);
      WriteBlobStringWithEOL(image,"#if defined(_INCLUDE_MESSAGE_TABLE_)");
      WriteBlobStringWithEOL(image,"static const char *message_dat[] =");
      WriteBlobStringWithEOL(image,"  {");
      /* message 0 is reserved as the generic windows event message */
      WriteBlobStringWithEOL(image,"    \"%1\",");
      for (i=0; i < count; i++)
      {
        (void) strlcpy(path,locale[i],sizeof(path));
        if (*path != '\0')
          {
            p=path+strlen(path)-1;
            if (*p == '/')
              *p='\0';
            fields[0]=""; /* this one may not exist */
            for (index=0; (index < TREE_LEVELS_SUPPORTED) && (p > path); p--)
            {
              if (*p == '/')
                {
                  *p='\0';
                  fields[4-index]=p+1;
                  index++;
                }
            }
            FormatString(text, "    \"%s\",",fields[4]);
            WriteBlobStringWithEOL(image,text);
          }
      }
      WriteBlobStringWithEOL(image,"    0");
      WriteBlobStringWithEOL(image,"  };");
      WriteBlobStringWithEOL(image,"#endif");
   }
  else if ((LocaleCompare(image_info->magick,"LOCALE") == 0) ||
      (LocaleCompare(image_info->magick,"LOCALEC") == 0))
    {
      /*
        Write locale comments.
      */
      attribute=GetImageAttribute(image,"[LocaleComment]");
      if (attribute != (const ImageAttribute *) NULL)
        (void) WriteBlobString(image,attribute->value);
      /*
        Write finite-state-machine.
      */
      locales=(struct locale_str *) NULL;
      accumulate((const char **) locale,count,&locales);
      (void) WriteBlobString(image,prologue);
      output_switches(image,locales, INDENT, -1);
      (void) WriteBlobString(image,epilogue);
    }
  /*
    Free resources.
  */
  for (i=0; i <= count; i++)
    MagickFreeMemory(locale[i]);
  MagickFreeMemory(locale);
  CloseBlob(image);
  return(True);
}

