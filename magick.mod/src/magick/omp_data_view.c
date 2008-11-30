/*
% Copyright (C) 2008 GraphicsMagick Group
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
%                  GraphicsMagick OpenMP Data View Methods                    %
%                                                                             %
%                                                                             %
%                             Software Design                                 %
%                             Bob Friesenhahn                                 %
%                              September 2008                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
*/

/*
  Include declarations.
*/
#include "magick/studio.h"
#include "magick/utility.h"
#include "magick/omp_data_view.h"


/*
  Declare OpenMP data view interfaces.
*/
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
/*
  Destroy a thread view data set.
*/
MagickExport void
DestroyThreadViewDataSet(ThreadViewDataSet *data_set)
{
  unsigned int
    i;
  
  if (data_set != (ThreadViewDataSet *) NULL)
    {
      if (data_set->view_data != (void *) NULL)
        {
          if (data_set->destructor != (MagickFreeFunc) NULL)
            {
              for (i=0; i < data_set->nviews; i++)
                {
                  (data_set->destructor)(data_set->view_data[i]);
                  data_set->view_data[i]=(void *) NULL;
                }
            }
          MagickFreeMemory(data_set->view_data);
        }
      data_set->nviews=0;
      MagickFreeMemory(data_set);
    }
}

/*
  Allocate an empty thread view data set.
*/
MagickExport ThreadViewDataSet *
AllocateThreadViewDataSet(const MagickFreeFunc destructor,
                          const Image *image,
                          ExceptionInfo *exception)
{
  ThreadViewDataSet
    *data_set;
  
  MagickPassFail
    status=MagickPass;
  
  data_set=MagickAllocateMemory(ThreadViewDataSet *,sizeof(ThreadViewDataSet));
  if (data_set == (ThreadViewDataSet *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                      UnableToAllocateCacheView);
  data_set->destructor=destructor;
  data_set->nviews=omp_get_max_threads();
  data_set->view_data=MagickAllocateArray(void *,data_set->nviews,sizeof(void *));
  if (data_set->view_data == (void *) NULL)
    {
      ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
                     image->filename);
      status=MagickFail;
    }

  if (data_set->view_data != (void *) NULL)
    (void) memset(data_set->view_data,0,data_set->nviews*sizeof(void *));
  
  if (status == MagickFail)
    {
      DestroyThreadViewDataSet(data_set);
      data_set=(ThreadViewDataSet *) NULL;
    }

  return data_set;
}

/*
  Allocate a thread view data set containing data elements with
  allocation size dictated by 'count' and 'size'.
  The allocated data is initialized to zero.
*/
MagickExport ThreadViewDataSet *
AllocateThreadViewDataArray(const Image *image,
                            ExceptionInfo *exception,
                            size_t count,size_t size)
{
  /*
    Allocate per-thread-view memory.
  */
  ThreadViewDataSet
    *data_set;
  
  MagickPassFail
    alloc_status=MagickFail;
          
  data_set=AllocateThreadViewDataSet(MagickFree,image,exception);
  if (data_set != (ThreadViewDataSet *) NULL)
    {
      unsigned int
        allocated_views;

      unsigned int
        i;
              
      alloc_status=MagickPass;
      allocated_views=GetThreadViewDataSetAllocatedViews(data_set);
              
      for (i=0; i < allocated_views; i++)
        {
          unsigned char
            *data;
                  
          data=MagickAllocateArray(unsigned char *,count,size);
          if (data == (unsigned char *) NULL)
            {
              ThrowException(exception,ResourceLimitError,MemoryAllocationFailed,
                             image->filename);
              alloc_status=MagickFail;
              break;
            }
          (void) memset(data,0,count*size);
          AssignThreadViewData(data_set,i,data);
        }
      if (alloc_status == MagickFail)
        {
          DestroyThreadViewDataSet(data_set);
          data_set=(ThreadViewDataSet *) NULL;
        }
    }

  return data_set;
}

/*
  Access allocated thread data.
*/
MagickExport void *
AccessThreadViewData(ThreadViewDataSet *data_set)
{
  unsigned int
    index=0;

  index=omp_get_thread_num();
  assert(index < data_set->nviews);
  return data_set->view_data[index];
}

MagickExport void *
AccessThreadViewDataById(ThreadViewDataSet *data_set,
                         unsigned int index)
{
  index=omp_get_thread_num();
  assert(index < data_set->nviews);
  return data_set->view_data[index];
}

/*
  Associate data with a thread data view.
*/
MagickExport void AssignThreadViewData
(ThreadViewDataSet *data_set, unsigned int index, void *data)
{
  assert(index < data_set->nviews);
  MagickFreeMemory(data_set->view_data[index]);
  data_set->view_data[index]=data;
}

/*
  Obtain the nuber of thread data views.
*/
MagickExport unsigned int
GetThreadViewDataSetAllocatedViews
(ThreadViewDataSet *data_set)
{
  return data_set->nviews;
}
