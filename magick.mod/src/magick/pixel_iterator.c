/*
% Copyright (C) 2004-2012 GraphicsMagick Group
%
% This program is covered by multiple licenses, which are described in
% Copyright.txt. You should have received a copy of Copyright.txt with this
% package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
%
% Interfaces to support simple iterative pixel read/update access within
% an image or between two images.
%
% Written by Bob Friesenhahn, March 2004, Updated for rows 2008.
%
*/

#include "magick/studio.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/pixel_iterator.h"
#include "magick/utility.h"

/*
  Obtain the maximum numer of threads to use based on image storage
  type, region size, and user options.  The maximum number of threads
  which may be used is that reported by omp_get_max_threads().  The
  number of threads to be used is returned.
*/
#if defined(HAVE_OPENMP)
static int
GetRegionThreads(const PixelIteratorOptions *options,
		 const MagickBool in_core,
                 const unsigned long columns,
                 const unsigned long rows)
{
  static const unsigned long
    tiny_region_metric = 64;

  int
    max_threads,
    region_threads;

  max_threads=omp_get_max_threads();
  region_threads=max_threads;

  if (((columns <= tiny_region_metric && rows <= tiny_region_metric)) ||
      ((magick_uint64_t) columns*rows <= tiny_region_metric*tiny_region_metric) ||
      (MagickFalse == in_core))
    {
      /*
        Run tiny or non-memory-based regions single threaded.
      */
      region_threads=1;
    }
  else if ((options) && (options->max_threads > 0))
    {
      /*
        Allow the user to specify the number of threads, up to the
        current limit.
      */
      region_threads=Min(max_threads,options->max_threads);
    }

  return region_threads;
}
#endif


/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   I n i t i a l i z e P i x e l I t e r a t o r O p t i o n s               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  InitializePixelIteratorOptions() assigns default options to a user-provided
%  PixelIteratorOptions structure.  This function should always be used
%  to initialize the PixelIteratorOptions structure prior to making any
%  changes to it.
%
%  The format of the InitializePixelIteratorOptions method is:
%
%      void InitializePixelIteratorOptions(PixelIteratorOptions *options,
%                                          ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o options: pointer to PixelIteratorOptions structure to initialize.
%
%    o exception: Return any errors or warnings in this structure.
%
*/
MagickExport void
InitializePixelIteratorOptions(PixelIteratorOptions *options,
                               ExceptionInfo *exception)
{
  ARG_NOT_USED(exception);
  assert(options != (PixelIteratorOptions *) NULL);
  options->max_threads=0;
  options->signature=MagickSignature;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e M o n o R e a d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% PixelIterateMonoRead() iterates through a region of an image and invokes a
% user-provided callback function (of type PixelRowIteratorMonoReadCallback)
% for a row of pixels. This is useful to support simple operations such as
% statistics computation.
%
%  The format of the PixelIterateMonoRead method is:
%
%      MagickPassFail PixelIterateMonoRead(
%                                 PixelIteratorMonoReadCallback call_back,
%                                 const PixelIteratorOptions *options,
%                                 const char *description,
%                                 void *mutable_data,
%                                 const void *immutable_data,
%                                 const long x,
%                                 const long y,
%                                 const unsigned long columns,
%                                 const unsigned long rows,
%                                 const Image *image,
%                                 ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of pixels from each image.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o x: The horizontal ordinate of the top left corner of the region.
%
%    o y: The vertical ordinate of the top left corner of the region.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o image: The address of the Image.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateMonoRead(PixelIteratorMonoReadCallback call_back,
                     const PixelIteratorOptions *options,
                     const char *description,
                     void *mutable_data,
                     const void *immutable_data,
                     const long x,
                     const long y,
                     const unsigned long columns,
                     const unsigned long rows,
                     const Image *image,
                     ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  unsigned long
    row_count=0;

#if defined(HAVE_OPENMP)
  int num_threads=GetRegionThreads(options,GetPixelCacheInCore(image),columns,rows);
#else
  (void) options;
#endif /* defined(HAVE_OPENMP) */

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(static,1) shared(row_count, status)
#  endif
#endif
  for (row=y; row < (long) (y+rows); row++)
    {
      MagickPassFail
        thread_status;

      const PixelPacket
        * restrict pixels;

      const IndexPacket
        * restrict indexes;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateMonoRead)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
	continue;

      pixels=AcquireImagePixels(image,x, row, columns, 1, exception);
      if (!pixels)
        thread_status=MagickFail;
      indexes=AccessImmutableIndexes(image);

      if (thread_status != MagickFail)
        thread_status=(call_back)(mutable_data,immutable_data,image,pixels,indexes,columns,exception);

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateMonoRead)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,rows))
          if (!MagickMonitorFormatted(row_count,rows,exception,
                                      description,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e M o n o M o d i f y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateMonoModify() iterates through a region of an image and invokes
%  a user-provided callback function (of type PixelIteratorMonoReadCallback)
%  for a region of pixels. This is useful to support simple operations such as
%  level shifting, colorspace translation, or thresholding.
%
%  The format of the PixelIterateMonoModify method is:
%
%      MagickPassFail PixelIterateMonoModify(
%                              PixelIteratorMonoModifyCallback call_back,
%                              const PixelIteratorOptions *options,
%                              const char *description,
%                              void *mutable_data,
%                              const void *immutable_data,
%                              const long x,
%                              const long y,
%                              const unsigned long columns,
%                              const unsigned long rows,
%                              Image *image,
%                              ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of pixels from each image.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o x: The horizontal ordinate of the top left corner of the region.
%
%    o y: The vertical ordinate of the top left corner of the region.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o image: The address of the Image.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateMonoModify(PixelIteratorMonoModifyCallback call_back,
                       const PixelIteratorOptions *options,
                       const char *description,
                       void *mutable_data,
                       const void *immutable_data,
                       const long x,
                       const long y,
                       const unsigned long columns,
                       const unsigned long rows,
                       Image *image,
                       ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  unsigned long
    row_count=0;

#if defined(HAVE_OPENMP)
  int num_threads=GetRegionThreads(options,GetPixelCacheInCore(image),columns,rows);
#else
  (void) options;
#endif /* defined(HAVE_OPENMP) */

  if (ModifyCache(image,exception) == MagickFail)
    return MagickFail;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(static,1) shared(row_count, status)
#  endif
#endif
  for (row=y; row < (long) (y+rows); row++)
    {
      MagickBool
        thread_status;

      PixelPacket
        * restrict pixels;

      IndexPacket
        * restrict indexes;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateMonoModify)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      pixels=GetImagePixelsEx(image, x, row, columns, 1, exception);
      if (!pixels)
        thread_status=MagickFail;
      indexes=AccessMutableIndexes(image);
      
      if (thread_status != MagickFail)
        thread_status=(call_back)(mutable_data,immutable_data,image,pixels,indexes,columns,exception);

      if (thread_status != MagickFail)
        if (!SyncImagePixelsEx(image,exception))
          thread_status=MagickFail;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateMonoModify)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,rows))
          if (!MagickMonitorFormatted(row_count,rows,exception,
                                      description,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l R e a d                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualRead() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualReadCallback) for each row of pixels. This is useful to
%  support operations such as image comparison.
%
%  The format of the PixelIterateDualModify method is:
%
%      MagickPassFail PixelIterateDualRead(
%                                PixelIteratorDualReadCallback call_back,
%                                const PixelIteratorOptions *options,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *first_image,
%                                const long first_x,
%                                const long first_y,
%                                const Image *second_image,
%                                const long second_x,
%                                const long second_y,
%                                ExceptionInfo *exception);
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which is passed the
%       address of pixels from each image.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o first_image: The address of the first Image.
%
%    o first_x: The horizontal ordinate of the top left corner of the first region.
%
%    o first_y: The vertical ordinate of the top left corner of the first region.
%
%    o second_image: The address of the second Image.
%
%    o second_x: The horizontal ordinate of the top left corner of the second region.
%
%    o second_y: The vertical ordinate of the top left corner of the second region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateDualRead(PixelIteratorDualReadCallback call_back,
                     const PixelIteratorOptions *options,
                     const char *description,
                     void *mutable_data,
                     const void *immutable_data,
                     const unsigned long columns,
                     const unsigned long rows,
                     const Image *first_image,
                     const long first_x,
                     const long first_y,
                     const Image *second_image,
                     const long second_x,
                     const long second_y,
                     ExceptionInfo *exception)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  unsigned long
    row_count=0;

#if defined(HAVE_OPENMP)
  int num_threads=GetRegionThreads(options,
				   (GetPixelCacheInCore(first_image) &&
				    GetPixelCacheInCore(second_image)),
				   columns,rows);
#else
  (void) options;
#endif /* defined(HAVE_OPENMP) */

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(static,1) shared(row_count, status)
#  endif
#endif
  for (row=0; row < (long) rows; row++)
    {
      MagickBool
        thread_status;

      long
        first_row,
        second_row;

      const PixelPacket
        * restrict first_pixels,
        * restrict second_pixels;

      const IndexPacket
        * restrict first_indexes,
        * restrict second_indexes;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateDualRead)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      first_row=first_y+row;
      second_row=second_y+row;

      first_pixels=AcquireImagePixels(first_image, first_x, first_row,
                                      columns, 1, exception);
      if (!first_pixels)
        thread_status=MagickFail;
      first_indexes=AccessImmutableIndexes(first_image);

      second_pixels=AcquireImagePixels(second_image, second_x, second_row,
                                       columns, 1, exception);
      if (!second_pixels)
        thread_status=MagickFail;
      second_indexes=AccessImmutableIndexes(second_image);

      if (thread_status != MagickFail)
        thread_status=(call_back)(mutable_data,immutable_data,
                                  first_image,first_pixels,first_indexes,
                                  second_image,second_pixels,second_indexes,
                                  columns, exception);

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateDualRead)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,rows))
          if (!MagickMonitorFormatted(row_count,rows,exception,
                                      description,first_image->filename,
                                      second_image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l M o d i f y                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualModify() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualModifyCallback) for each row of pixels. This is useful to
%  support operations such as composition.
%
%  The format of the PixelIterateDualModify method is:
%
%      MagickPassFail PixelIterateDualModify(
%                                PixelIteratorDualModifyCallback call_back,
%                                const PixelIteratorOptions *options,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *update_image,
%                                const long update_x,
%                                const long update_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a region of source pixels and updates a region of destination pixels.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source_image: The address of the constant source Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source region.
%
%    o source_y: The vertical ordinate of the top left corner of the source region.
%
%    o update_image: The address of the update Image.
%
%    o update_x: The horizontal ordinate of the top left corner of the update region.
%
%    o update_y: The vertical ordinate of the top left corner of the update region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
static MagickPassFail
PixelIterateDualImplementation(PixelIteratorDualModifyCallback call_back,
                               const PixelIteratorOptions *options,
                               const char *description,
                               void *mutable_data,
                               const void *immutable_data,
                               const unsigned long columns,
                               const unsigned long rows,
                               const Image *source_image,
                               const long source_x,
                               const long source_y,
                               Image *update_image,
                               const long update_x,
                               const long update_y,
                               ExceptionInfo *exception,
                               MagickBool set)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  unsigned long
    row_count=0;

#if defined(HAVE_OPENMP)
  int num_threads=GetRegionThreads(options,
				   (GetPixelCacheInCore(source_image) &&
				    GetPixelCacheInCore(update_image)),
				   columns,rows);
#else
  (void) options;
#endif /* defined(HAVE_OPENMP) */

  if (ModifyCache(update_image,exception) == MagickFail)
    return MagickFail;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(static,1) shared(row_count, status)
#  endif
#endif
  for (row=0; row < (long) rows; row++)
    {
      MagickBool
        thread_status;

      const PixelPacket
        * restrict source_pixels;

      const IndexPacket
        * restrict source_indexes;

      PixelPacket
        * restrict update_pixels;

      IndexPacket
        * restrict update_indexes;

      register long
        source_row,
        update_row;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateDualImplementation)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      source_row=source_y+row;
      update_row=update_y+row;

      source_pixels=AcquireImagePixels(source_image, source_x, source_row,
                                       columns, 1, exception);
      if (!source_pixels)
        thread_status=MagickFail;
      source_indexes=AccessImmutableIndexes(source_image);
      
      if (set)
        update_pixels=SetImagePixelsEx(update_image, update_x, update_row,
                                       columns, 1, exception);
      else
        update_pixels=GetImagePixelsEx(update_image, update_x, update_row,
                                       columns, 1, exception);
      if (!update_pixels)
        thread_status=MagickFail;
      update_indexes=AccessMutableIndexes(update_image);

      if (thread_status != MagickFail)
        thread_status=(call_back)(mutable_data,immutable_data,
                                  source_image,source_pixels,source_indexes,
                                  update_image,update_pixels,update_indexes,
                                  columns,exception);
      
      if (thread_status != MagickFail)
        if (!SyncImagePixelsEx(update_image,exception))
          thread_status=MagickFail;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateDualImplementation)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,rows))
          if (!MagickMonitorFormatted(row_count,rows,exception,
                                      description,source_image->filename,
                                      update_image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

MagickExport MagickPassFail
PixelIterateDualModify(PixelIteratorDualModifyCallback call_back,
                       const PixelIteratorOptions *options,
                       const char *description,
                       void *mutable_data,
                       const void *immutable_data,
                       const unsigned long columns,
                       const unsigned long rows,
                       const Image *source_image,
                       const long source_x,
                       const long source_y,
                       Image *update_image,
                       const long update_x,
                       const long update_y,
                       ExceptionInfo *exception)
{
  return PixelIterateDualImplementation
    (call_back,options,description,mutable_data,immutable_data,columns,rows,
     source_image,source_x,source_y,update_image,update_x,update_y,exception,
     MagickFalse);
}



/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e D u a l N e w                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateDualNew() iterates through pixel regions of two images and
%  invokes a user-provided callback function (of type
%  PixelIteratorDualNewCallback) for each row of pixels. This is used if a
%  new output image is created based on an input image.  The difference from
%  PixelIterateDualModify() is that the output pixels are not initialized so
%  it is more efficient when outputting a new image.
%
%  The format of the PixelIterateDualNew method is:
%
%      MagickPassFail PixelIterateDualNew(
%                                PixelIteratorDualNewCallback call_back,
%                                const PixelIteratorOptions *options,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *new_image,
%                                const long new_x,
%                                const long new_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a region of source pixels and initializes a region of destination pixels.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source_image: The address of the constant source Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source region.
%
%    o source_y: The vertical ordinate of the top left corner of the source region.
%
%    o new_image: The address of the new Image.
%
%    o new_x: The horizontal ordinate of the top left corner of the new region.
%
%    o new_y: The vertical ordinate of the top left corner of the new region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateDualNew(PixelIteratorDualNewCallback call_back,
                    const PixelIteratorOptions *options,
                    const char *description,
                    void *mutable_data,
                    const void *immutable_data,
                    const unsigned long columns,
                    const unsigned long rows,
                    const Image *source_image,
                    const long source_x,
                    const long source_y,
                    Image *new_image,
                    const long new_x,
                    const long new_y,
                    ExceptionInfo *exception)
{
  return PixelIterateDualImplementation
    (call_back,options,description,mutable_data,immutable_data,columns,rows,
     source_image,source_x,source_y,new_image,new_x,new_y,exception,
     MagickTrue);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e T r i p l e M o d i f y                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateTripleModify() iterates through pixel regions of three images
%  and invokes a user-provided callback function (of type
%  PixelIteratorTripleModifyCallback) for each row of pixels.  The first two
%  images are read-only, while the third image is read-write for update.
%  Access of the first two images is done lock-step using the same coordinates.
%  This is useful to support operations such as image differencing.
%
%  The format of the PixelIterateTripleModify method is:
%
%      MagickPassFail PixelIterateTripleModify(
%                                PixelIteratorTripleModifyCallback call_back,
%                                const PixelIteratorOptions *options,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source1_image,
%                                const Image *source2_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *update_image,
%                                const long update_x,
%                                const long update_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a region of source pixels and updates a region of destination pixels.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source1_image: The address of the constant source 1 Image.
%
%    o source2_image: The address of the constant source 2 Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source regions.
%
%    o source_y: The vertical ordinate of the top left corner of the source regions.
%
%    o update_image: The address of the update Image.
%
%    o update_x: The horizontal ordinate of the top left corner of the update region.
%
%    o update_y: The vertical ordinate of the top left corner of the update region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
static MagickPassFail
PixelIterateTripleImplementation(PixelIteratorTripleModifyCallback call_back,
                                 const PixelIteratorOptions *options,
                                 const char *description,
                                 void *mutable_data,
                                 const void *immutable_data,
                                 const unsigned long columns,
                                 const unsigned long rows,
                                 const Image *source1_image,
                                 const Image *source2_image,
                                 const long source_x,
                                 const long source_y,
                                 Image *update_image,
                                 const long update_x,
                                 const long update_y,
                                 ExceptionInfo *exception,
                                 MagickBool set)
{
  MagickPassFail
    status = MagickPass;

  register long
    row;

  unsigned long
    row_count=0;

#if defined(HAVE_OPENMP)
  int num_threads=GetRegionThreads(options,
				   (GetPixelCacheInCore(source1_image) &&
				    GetPixelCacheInCore(source2_image) &&
				    GetPixelCacheInCore(update_image)),
				   columns,rows);
#else
  (void) options;
#endif /* defined(HAVE_OPENMP) */

  if (ModifyCache(update_image,exception) == MagickFail)
    return MagickFail;

#if defined(HAVE_OPENMP)
#  if defined(TUNE_OPENMP)
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(runtime) shared(row_count, status)
#  else
#    pragma omp parallel for if(num_threads > 1) num_threads(num_threads) schedule(static,1) shared(row_count, status)
#endif
#endif
  for (row=0; row < (long) rows; row++)
    {
      MagickBool
        thread_status;

      const PixelPacket
        * restrict source1_pixels,
        * restrict source2_pixels;

      const IndexPacket
        * restrict source1_indexes,
        * restrict source2_indexes;

      PixelPacket
        * restrict update_pixels;

      IndexPacket
        * restrict update_indexes;

      long
        source_row,
        update_row;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateTripleImplementation)
#endif
      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      source_row=source_y+row;
      update_row=update_y+row;

      /*
        First image (read only).
      */
      source1_pixels=AcquireImagePixels(source1_image, source_x, source_row,
                                        columns, 1, exception);
      if (!source1_pixels)
        thread_status=MagickFail;
      source1_indexes=AccessImmutableIndexes(source1_image);

      /*
        Second image (read only).
      */
      source2_pixels=AcquireImagePixels(source2_image, source_x, source_row,
                                        columns, 1, exception);
      if (!source2_pixels)
        thread_status=MagickFail;
      source2_indexes=AccessImmutableIndexes(source2_image);

      /*
        Third image (read/write).
      */
      if (set)
        update_pixels=SetImagePixelsEx(update_image, update_x, update_row,
                                       columns, 1, exception);
      else
        update_pixels=GetImagePixelsEx(update_image, update_x, update_row,
                                       columns, 1, exception);
      if (!update_pixels)
        {
          thread_status=MagickFail;
          CopyException(exception,&update_image->exception);
        }
      update_indexes=AccessMutableIndexes(update_image);

      if (thread_status != MagickFail)
        thread_status=(call_back)(mutable_data,immutable_data,
				  source1_image,source1_pixels,source1_indexes,
				  source2_image,source2_pixels,source2_indexes,
				  update_image,update_pixels,update_indexes,
				  columns,exception);

      if (thread_status != MagickFail)
	if (!SyncImagePixelsEx(update_image,exception))
	  thread_status=MagickFail;

#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_PixelIterateTripleImplementation)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,rows))
          if (!MagickMonitorFormatted(row_count,rows,exception,description,
                                      source1_image->filename,
                                      source2_image->filename,
                                      update_image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }

  return (status);
}

MagickExport MagickPassFail
PixelIterateTripleModify(PixelIteratorTripleModifyCallback call_back,
                         const PixelIteratorOptions *options,
                         const char *description,
                         void *mutable_data,
                         const void *immutable_data,
                         const unsigned long columns,
                         const unsigned long rows,
                         const Image *source1_image,
                         const Image *source2_image,
                         const long source_x,
                         const long source_y,
                         Image *update_image,
                         const long update_x,
                         const long update_y,
                         ExceptionInfo *exception)
{
  return PixelIterateTripleImplementation
    (call_back,options,description,mutable_data,immutable_data,columns,rows,
     source1_image,source2_image,source_x,source_y,
     update_image,update_x,update_y,
     exception,MagickFalse);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   P i x e l I t e r a t e T r i p l e N e w                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  PixelIterateTripleNew() iterates through pixel regions of three images
%  and invokes a user-provided callback function (of type
%  PixelIteratorTripleNewCallback) for each row of pixels. The first two
%  images are read-only, while the third image is read-write for update.
%  Access of the first two images is done lock-step using the same coordinates.
%  This is used if a new output image is created based on two input images.
%  The difference from PixelIterateTripleModify() is that the output pixels
%  are not initialized so it is more efficient when outputting a new image.
%
%  The format of the PixelIterateTripleNew method is:
%
%      MagickPassFail PixelIterateTripleNew(
%                                PixelIteratorTripleNewCallback call_back,
%                                 const PixelIteratorOptions *options,
%                                const char *description,
%                                void *mutable_data,
%                                const void *immutable_data,
%                                const unsigned long columns,
%                                const unsigned long rows,
%                                const Image *source1_image,
%                                const Image *source2_image,
%                                const long source_x,
%                                const long source_y,
%                                Image *new_image,
%                                const long new_x,
%                                const long new_y,
%                                ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o call_back: A user-provided C callback function which reads from
%      a region of source pixels and initializes a region of destination pixels.
%
%    o options: Pixel iterator execution options (may be NULL).
%
%    o description: textual description of operation being performed.
%
%    o mutable_data: User-provided mutable context data.
%
%    o immutable_data: User-provided immutable context data.
%
%    o columns: Width of pixel region
%
%    o rows: Height of pixel region
%
%    o source1_image: The address of the constant source 1 Image.
%
%    o source2_image: The address of the constant source 2 Image.
%
%    o source_x: The horizontal ordinate of the top left corner of the source regions.
%
%    o source_y: The vertical ordinate of the top left corner of the source regions.
%
%    o new_image: The address of the new Image.
%
%    o new_x: The horizontal ordinate of the top left corner of the new region.
%
%    o new_y: The vertical ordinate of the top left corner of the new region.
%
%    o exception: If an error is reported, this argument is updated with the reason.
%
*/
MagickExport MagickPassFail
PixelIterateTripleNew(PixelIteratorTripleNewCallback call_back,
                      const PixelIteratorOptions *options,
                      const char *description,
                      void *mutable_data,
                      const void *immutable_data,
                      const unsigned long columns,
                      const unsigned long rows,
                      const Image *source1_image,
                      const Image *source2_image,
                      const long source_x,
                      const long source_y,
                      Image *new_image,
                      const long new_x,
                      const long new_y,
                      ExceptionInfo *exception)
{
  return PixelIterateTripleImplementation
    (call_back,options,description,mutable_data,immutable_data,columns,rows,
     source1_image,source2_image,source_x,source_y,
     new_image,new_x,new_y,
     exception,MagickTrue);
}
