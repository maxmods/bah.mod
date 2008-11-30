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
%               V   V  EEEEE  RRRR   SSSSS  IIIII   OOO   N   N               %
%               V   V  E      R   R  SS       I    O   O  NN  N               %
%               V   V  EEE    RRRR    SSS     I    O   O  N N N               %
%                V V   E      R R       SS    I    O   O  N  NN               %
%                 V    EEEEE  R  R   SSSSS  IIIII   OOO   N   N               %
%                                                                             %
%                                                                             %
%                        GraphicsMagick Version Methods                       %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                               September 2002                                %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#include "magick/studio.h"
#include "magick/version.h"

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k C o p y r i g h t                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickCopyright() returns the GraphicsMagick API copyright as a string.
%
%  The format of the GetMagickCopyright method is:
%
%      const char *GetMagickCopyright(void)
%
%
*/
MagickExport const char *GetMagickCopyright(void)
{
  return(MagickCopyright);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k V e r s i o n                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickVersion() returns the GraphicsMagick API version as a string and
%  as a number.
%
%  The format of the GetMagickVersion method is:
%
%      const char *GetMagickVersion(unsigned long *version)
%
%  A description of each parameter follows:
%
%    o version: The GraphicsMagick version is returned as a number.
%
*/
MagickExport const char *GetMagickVersion(unsigned long *version)
{
  if (version != (unsigned long *) NULL)
    *version=MagickLibVersion;
  return(MagickVersion);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   G e t M a g i c k W e b S i t e                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  GetMagickWebSite() returns the GraphicsMagick web site URL as a string.
%
%  The format of the GetMagickWebSite method is:
%
%      const char *GetMagickWebSite(void)
%
%
*/
MagickExport const char *GetMagickWebSite(void)
{
  return(MagickWebSite);
}
