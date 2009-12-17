/*
% Copyright (C) 2003 - 2009 GraphicsMagick Group
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
%               SSSSS  EEEEE   GGGG  M   M  EEEEE  N   N  TTTTT               %
%               SS     E      G      MM MM  E      NN  N    T                 %
%                SSS   EEE    G GGG  M M M  EEE    N N N    T                 %
%                  SS  E      G   G  M   M  E      N  NN    T                 %
%               SSSSS  EEEEE   GGGG  M   M  EEEEE  N   N    T                 %
%                                                                             %
%                                                                             %
%        Methods to Segment an Image with Thresholding Fuzzy c-Means          %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                April 1993                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Segment segments an image by analyzing the histograms of the color
%  components and identifying units that are homogeneous with the fuzzy
%  c-means technique.  The scale-space filter analyzes the histograms of
%  the three color components of the image and identifies a set of classes.
%  The extents of each class is used to coarsely segment the image with
%  thresholding.  The color associated with each class is determined by
%  the mean color of all pixels within the extents of a particular class.
%  Finally, any unclassified pixels are assigned to the closest class with
%  the fuzzy c-means technique.
%
%  The fuzzy c-Means algorithm can be summarized as follows:
%
%    o Build a histogram, one for each color component of the image.
%
%    o For each histogram, successively apply the scale-space
%      filter and build an interval tree of zero crossings in
%      the second derivative at each scale.  Analyze this
%      scale-space ``fingerprint'' to determine which peaks and
%      valleys in the histogram are most predominant.
%
%    o The fingerprint defines intervals on the axis of the
%      histogram.  Each interval contains either a minima or a
%      maxima in the original signal.  If each color component
%      lies within the maxima interval, that pixel is considered
%      ``classified'' and is assigned an unique class number.
%
%    o Any pixel that fails to be classified in the above
%      thresholding pass is classified using the fuzzy
%      c-Means technique.  It is assigned to one of the classes
%      discovered in the histogram analysis phase.
%
%  The fuzzy c-Means technique attempts to cluster a pixel by finding
%  the local minima of the generalized within group sum of squared error
%  objective function.  A pixel is assigned to the closest class of which
%  the fuzzy membership has a maximum value.
%
%  Segment is strongly based on software written by Andy Gallo, University
%  of Delaware.
%
%  The following reference was used in creating this program:
%
%    Young Won Lim, Sang Uk Lee, "On The Color Image Segmentation Algorithm
%    Based on the Thresholding and the Fuzzy c-Means Techniques", Pattern
%    Recognition, Volume 23, Number 9, pages 935-952, 1990.
%
%
*/

#include "magick/studio.h"
#include "magick/color.h"
#include "magick/log.h"
#include "magick/monitor.h"
#include "magick/pixel_cache.h"
#include "magick/quantize.h"
#include "magick/utility.h"

/*
  Define declarations.
*/
#define MaxDimension  3
#define DeltaTau  0.5f
#define Tau  5.2f

/*
  Set SquaredClassify to 1 to shortcut use of expensive pow() in the
  classification code.
*/
#define SquaredClassify 1

/* 2 is optimum performance, 2.5 may be better quality */
#if SquaredClassify
#  define WeightingExponent  2
#else
#  define WeightingExponent  2.5
#endif


/*
  Typedef declarations.
*/
typedef struct _ExtentPacket
{
  double
    center;

  int
    index,
    left,
    right;
} ExtentPacket;

typedef struct _IntervalTree
{
  double
    tau;

  int
    left,
    right;

  double
    mean_stability,
    stability;

  struct _IntervalTree
    *sibling,
    *child;
} IntervalTree;

typedef struct _ZeroCrossing
{
  double
    tau,
    histogram[256];

  short
    crossings[256];
} ZeroCrossing;

/*
  Constant declarations.
*/
static const int
  Blue = 2,
  Green = 1,
  Red = 0,
  SafeMargin = 3,
  TreeLength = 600;

/*
  Method prototypes.
*/
static int
  DefineRegion(const short *,ExtentPacket *);

static void
  ScaleSpace(const long *,const double,double *),
  ZeroCrossHistogram(double *,const double,short *);

#if 0
static void
DumpDerivativeArray(FILE *stream,const unsigned int entries,
                    const double *derivative)
{
  unsigned int
    i;

  for (i=0; i < entries; i++)
    fprintf(stream,"  %03u: %g\n", i, derivative[i]);
}
#endif
static void
DumpExtremaArray(FILE *stream,const unsigned int entries,
                 const short *extrema)
{
  unsigned int
    i;

  for (i=0; i < entries; i++)
    fprintf(stream,"  %03u: %d\n", i, (int) extrema[i]);
}
static void
DumpHistogramArray(FILE *stream,const unsigned int entries,
                   const long *histogram)
{
  unsigned int
    i;

  for (i=0; i < entries; i++)
    fprintf(stream,"  %03u: %ld\n", i, histogram[i]);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C l a s s i f y                                                           %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method Classify defines on ore more classes.  Each pixel is thresholded
%  to determine which class it belongs to.  If not class is identified it
%  is assigned to the closest class based on the fuzzy c-Means technique.
%
%  The format of the Classify method is:
%
%      MagickPassFail Classify(Image *image,short **extrema,
%                              const double cluster_threshold,
%                              const double weighting_exponent,
%                              const unsigned int verbose)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o extrema:  Specifies a pointer to an array of integers.  They
%      represent the peaks and valleys of the histogram for each color
%      component.
%
%    o cluster_threshold:  The minimum number of total pixels contained
%      in a hexahedra before it can be considered valid (expressed as a
%      percentage of total pixels).  This is used to eliminate seldom
%      used colors.
%
%    o weighting_exponent: Specifies the membership weighting exponent.
%
%    o verbose:  A value greater than zero prints detailed information about
%      the identified classes.
%
%
*/
#define SegmentImageText "[%s] Segment..."

typedef struct _Cluster
{
  struct _Cluster
    *next;
  
  ExtentPacket
    red,
    green,
    blue;
  
  magick_int64_t
    count;
  
  short
    id;
} Cluster;

static MagickPassFail
Classify(Image *image,short **extrema,
         const double cluster_threshold,
         const double weighting_exponent,
         const unsigned int verbose)
{
  Cluster
    *cluster,
    **cluster_array,
    *head,
    *last_cluster,
    *next_cluster;

  double
    *free_squares,
    threshold,
    total_vectors;

  ExtentPacket
    blue = { 0.0, 0, 0, 0},
    green = { 0.0, 0, 0, 0},
    red = { 0.0, 0, 0, 0};

  unsigned long
    count;

  long
    y;

  PixelPacket
    *colormap;

  register const PixelPacket
    *p;

  register double
    *squares;

  register IndexPacket
    *indexes;

  register long
    i,
    x;

  register PixelPacket
    *q;

  unsigned long
    number_clusters;

  unsigned long
    row_count=0;

  MagickPassFail
    status=MagickPass;

  /*
    Form clusters.
  */
  cluster=(Cluster *) NULL;
  head=(Cluster *) NULL;
  red.index=0;
  while (DefineRegion(extrema[Red],&red))
    {
      green.index=0;
      while (DefineRegion(extrema[Green],&green))
        {
          blue.index=0;
          while (DefineRegion(extrema[Blue],&blue))
            {
              /*
                Allocate a new class.
              */
              if (head != (Cluster *) NULL)
                {
                  cluster->next=MagickAllocateMemory(Cluster *,sizeof(Cluster));
                  cluster=cluster->next;
                }
              else
                {
                  cluster=MagickAllocateMemory(Cluster *,sizeof(Cluster));
                  head=cluster;
                }
              if (cluster == (Cluster *) NULL)
                ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                                     image->filename);
              /*
                Initialize a new class.
              */
              (void) memset(cluster,0,sizeof(Cluster));
              cluster->count=0;
              cluster->red=red;
              cluster->green=green;
              cluster->blue=blue;
              cluster->next=(Cluster *) NULL;
            }
        }
    }
  if (head == (Cluster *) NULL)
    {
      /*
        No classes were identified-- create one.
      */
      cluster=MagickAllocateMemory(Cluster *,sizeof(Cluster));
      if (cluster == (Cluster *) NULL)
        ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                             image->filename);
      /*
        Initialize a new class.
      */
      (void) memset(cluster,0,sizeof(Cluster));
      cluster->count=0;
      cluster->red=red;
      cluster->green=green;
      cluster->blue=blue;
      cluster->next=(Cluster *) NULL;
      head=cluster;
    }
  /*
    Build an array representation of the clusters.
  */
  number_clusters=0;
  for (cluster=head; cluster != (Cluster *) NULL; cluster=cluster->next)
    number_clusters++;
  cluster_array=MagickAllocateArray(Cluster **,number_clusters,sizeof(Cluster *));
  number_clusters=0;
  for (cluster=head; cluster != (Cluster *) NULL; cluster=cluster->next)
    cluster_array[number_clusters++]=cluster;
  /*
    Count the pixels for each cluster.
  */
  for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        {
          status=MagickFail;
          break;
        }
      for (x=(long) image->columns; x != 0; x--)
        {
          double
            r,
            g,
            b;

          r=(double) ScaleQuantumToChar(p->red);
          g=(double) ScaleQuantumToChar(p->green);
          b=(double) ScaleQuantumToChar(p->blue);

          for (count=0 ; count < number_clusters; count++)
            {
              if ((r >= (cluster_array[count]->red.left-SafeMargin)) &&
                  (r <= (cluster_array[count]->red.right+SafeMargin)) &&
                  (g >= (cluster_array[count]->green.left-SafeMargin)) &&
                  (g <= (cluster_array[count]->green.right+SafeMargin)) &&
                  (b >= (cluster_array[count]->blue.left-SafeMargin)) &&
                  (b <= (cluster_array[count]->blue.right+SafeMargin)))
                {
                  /*
                    Count this pixel.
                  */
                  cluster_array[count]->count++;
                  cluster_array[count]->red.center+=r;
                  cluster_array[count]->green.center+=g;
                  cluster_array[count]->blue.center+=b;

                  if ((count > 0) &&
                      (cluster_array[count]->count > cluster_array[count-1]->count))
                    {
                      Cluster
                        *tmp_cluster;

                      tmp_cluster=cluster_array[count-1];
                      cluster_array[count-1]=cluster_array[count];
                      cluster_array[count]=tmp_cluster;
                    }
                  break;
                }
            }
          p++;
        }
      if (QuantumTick(y,image->rows))
        if (!MagickMonitorFormatted(y,image->rows << 1,&image->exception,
                                    SegmentImageText,image->filename))
          {
            status=MagickFail;
            break;
          }
    }

  /*
    Remove clusters that do not meet minimum cluster threshold.
  */
  total_vectors=0.0;
  for (cluster=head; cluster != (Cluster *) NULL; cluster=cluster->next)
    total_vectors+=(double) cluster->count;
  threshold=cluster_threshold*0.01*total_vectors;
  count=0;
  last_cluster=head;
  next_cluster=head;
  for (cluster=head; cluster != (Cluster *) NULL; cluster=next_cluster)
    {
      next_cluster=cluster->next;
      if ((cluster->count > 0) &&
          ((double) cluster->count >= threshold))
        {
          /*
            Initialize cluster.
          */
          cluster->id=count;
          cluster->red.center=(cluster->red.center/((double) cluster->count));
          cluster->green.center=(cluster->green.center/((double) cluster->count));
          cluster->blue.center=(cluster->blue.center/((double) cluster->count));
          count++;
          last_cluster=cluster;
        }
      else
        {
          /*
            Delete cluster.
          */
          if (cluster == head)
            head=next_cluster;
          else
            last_cluster->next=next_cluster;

          if (image->logging)
            (void) LogMagickEvent
              (TransformEvent,GetMagickModule(),
               "Removing Cluster (usage count %lu, %.5f%%) %d-%d  %d-%d  %d-%d",
               (unsigned long) cluster->count,
               (((double) cluster->count/total_vectors) * 100.0),
               cluster->red.left,cluster->red.right,
               cluster->green.left,cluster->green.right,
               cluster->blue.left,cluster->blue.right);
          MagickFreeMemory(cluster);
        }
    }
  number_clusters=count;
  if (verbose && (head != (Cluster *) NULL))
    {
      /*
        Print cluster statistics.
      */
      (void) fprintf(stdout,"===============================================\n");
      (void) fprintf(stdout,"           Fuzzy c-Means Statistics\n");
      (void) fprintf(stdout,"===============================================\n");
      (void) fprintf(stdout,"Cluster Threshold        = %g%%\n", cluster_threshold);
      (void) fprintf(stdout,"Weighting Exponent       = %g\n", weighting_exponent);
      (void) fprintf(stdout,"Total Number of Clusters = %lu\n",
                     number_clusters);
      (void) fprintf(stdout,"Total Number of Vectors  = %g\n",
                     total_vectors);
      (void) fprintf(stdout,"Cluster Threshold        = %g vectors\n\n",
                     threshold);
      /*
        Print the total number of points per cluster.
      */
      (void) fprintf(stdout,"Cluster          Usage                 Extents                  Center\n");
      (void) fprintf(stdout,"=======  ====================  =======================  =====================\n");
      for (cluster=head; cluster != (Cluster *) NULL; cluster=cluster->next)
        {
          PixelPacket
            color;

          char
            tuple[MaxTextExtent];

          color.red=ScaleCharToQuantum((unsigned int) (cluster->red.center + 0.5));
          color.green=ScaleCharToQuantum((unsigned int) (cluster->green.center + 0.5));
          color.blue=ScaleCharToQuantum((unsigned int) (cluster->blue.center + 0.5));
          color.opacity=OpaqueOpacity;
          /* (void) QueryColorname(image,&color,X11Compliance,colorname,&image->exception); */
          GetColorTuple(&color,8,MagickFalse,MagickTrue,tuple);
          (void) fprintf(stdout,"  %3d    %10lu (%6.3f%%)  %03d-%03d %03d-%03d %03d-%03d  %03.0f %03.0f %03.0f (%s)\n",
                         cluster->id,
                         (unsigned long) cluster->count,
                         (((double) cluster->count/total_vectors) * 100.0),
                         cluster->red.left,cluster->red.right,
                         cluster->green.left,cluster->green.right,
                         cluster->blue.left,cluster->blue.right,
                         cluster->red.center,
                         cluster->green.center,
                         cluster->blue.center,
                         tuple);
        }
    }
  if ((number_clusters > 256) || (number_clusters == 0))
    ThrowBinaryException3(ImageError,UnableToSegmentImage,TooManyClusters);
  /*
    Speed up distance calculations.
  */
  squares=MagickAllocateMemory(double *,513*sizeof(double));
  if (squares == (double *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                         image->filename);
  squares+=255;
#if defined(HAVE_OPENMP)
#  pragma omp parallel for
#endif
  for (i=(-255); i <= 255; i++)
    squares[i]=i*i;
  /*
    Allocate image colormap.
  */
  colormap=MagickAllocateMemory(PixelPacket *,number_clusters*sizeof(PixelPacket));
  if (colormap == (PixelPacket *) NULL)
    ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                         image->filename);
  image->matte=False;
  image->storage_class=PseudoClass;
  if (image->colormap != (PixelPacket *) NULL)
    MagickFreeMemory(image->colormap);
  image->colormap=colormap;
  image->colors=number_clusters;
  i=0;
  for (cluster=head; cluster != (Cluster *) NULL; cluster=cluster->next)
    {
      image->colormap[i].red=ScaleCharToQuantum((unsigned int) (cluster->red.center + 0.5));
      image->colormap[i].green=ScaleCharToQuantum((unsigned int) (cluster->green.center + 0.5));
      image->colormap[i].blue=ScaleCharToQuantum((unsigned int) (cluster->blue.center + 0.5));
      image->colormap[i].opacity=OpaqueOpacity;
      i++;
    }
  /*
    Rebuild cluster array.
  */
  number_clusters=0;
  for (cluster=head; cluster != (Cluster *) NULL; cluster=cluster->next)
    cluster_array[number_clusters++]=cluster;
  /*
    Do course grain storage_class.
  */
  row_count=0;
#if defined(HAVE_OPENMP)
#  pragma omp parallel for schedule(dynamic,8) shared(row_count, status) private(count,indexes,p,q,x)
#endif
  for (y=0; y < (long) image->rows; y++)
    {
      MagickBool
        thread_status;

      int
	num_threads;

      thread_status=status;
      if (thread_status == MagickFail)
        continue;

      num_threads=omp_get_num_threads();
      q=GetImagePixelsEx(image,0,y,image->columns,1,&image->exception);
      if (q == (PixelPacket *) NULL)
        thread_status=MagickFail;

      if (thread_status != MagickFail)
        {
          indexes=AccessMutableIndexes(image);
          for (x=0; x < (long) image->columns; x++)
            {
              MagickBool
                classified=MagickFalse;

              long
                r,
                g,
                b;

              r=(long) ScaleQuantumToChar(q[x].red);
              g=(long) ScaleQuantumToChar(q[x].green);
              b=(long) ScaleQuantumToChar(q[x].blue);

              for (count=0; count < number_clusters; count++)
                if ((r >= (cluster_array[count]->red.left-SafeMargin)) &&
                    (r <= (cluster_array[count]->red.right+SafeMargin)) &&
                    (g >= (cluster_array[count]->green.left-SafeMargin)) &&
                    (g <= (cluster_array[count]->green.right+SafeMargin)) &&
                    (b >= (cluster_array[count]->blue.left-SafeMargin)) &&
                    (b <= (cluster_array[count]->blue.right+SafeMargin)))
                  {
                    /*
                      Classify this pixel.
                    */
                    indexes[x]=(IndexPacket) cluster_array[count]->id;
                    q[x]=image->colormap[indexes[x]];
                    classified=MagickTrue;

		    /*
		      Re-sort array so that most frequent occurs first.

		      Updating cluster_array causes a multithread race
		      condition so this chunk is only enabled in the
		      case of one thread.
		    */
                    if ((num_threads == 1) && (count > 0) &&
                        (cluster_array[count]->count > cluster_array[count-1]->count))
                      {
                        Cluster
                          *tmp_cluster;

                        tmp_cluster=cluster_array[count-1];
                        cluster_array[count-1]=cluster_array[count];
                        cluster_array[count]=tmp_cluster;
                      }

                    break;
                  }
              if (classified == MagickFalse)
                {
                  double
                    local_minima,
                    sum;

                  long
                    j,
                    k;

                  /*
                    Compute fuzzy membership.
                  */
                  local_minima=0.0;
                  for (j=0; j < (long) image->colors; j++)
                    {
                      double
                        distance_squared,
                        numerator,
                        ratio_squared;

                      sum=0.0;
                      p=image->colormap+j;
                      distance_squared=
                        squares[r-(long) ScaleQuantumToChar(p->red)]+
                        squares[g-(long) ScaleQuantumToChar(p->green)]+
                        squares[b-(long) ScaleQuantumToChar(p->blue)];
                      numerator=distance_squared;
                      for (k=0; k < (long) image->colors; k++)
                        {
                          p=image->colormap+k;
                          distance_squared=
                            squares[r-(long) ScaleQuantumToChar(p->red)]+
                            squares[g-(long) ScaleQuantumToChar(p->green)]+
                            squares[b-(long) ScaleQuantumToChar(p->blue)];
                          ratio_squared=numerator/distance_squared;;
#if SquaredClassify
                          /*
                            Since SquaredClassify (using a weighting
                            exponent of 2.0) is normally defined to be
                            true, this is the normally active code.
                            Otherwise execution is even slower since
                            pow() is excruciatingly slow.
                          */
                          sum+=ratio_squared;
#else
                          sum+=pow(ratio_squared,((double) (1.0/(weighting_exponent-1.0))));
#endif
                        }
                      if ((sum != 0.0) && ((1.0/sum) > local_minima))
                        {
                          /*
                            Classify this pixel.
                          */
                          local_minima=1.0/sum;
                          indexes[x]=(IndexPacket) j;
                          q[x]=image->colormap[indexes[x]];
                        }
                    }
                }
            }
          if (!SyncImagePixelsEx(image,&image->exception))
            thread_status=MagickFail;
        }
#if defined(HAVE_OPENMP)
#  pragma omp critical (GM_Classify)
#endif
      {
        row_count++;
        if (QuantumTick(row_count,image->rows))
          if (!MagickMonitorFormatted(row_count+image->rows,image->rows << 1,
                                      &image->exception,
                                      SegmentImageText,image->filename))
            thread_status=MagickFail;

        if (thread_status == MagickFail)
          status=MagickFail;
      }
    }
  /*
    Free memory.
  */
  for (cluster=head; cluster != (Cluster *) NULL; cluster=next_cluster)
    {
      next_cluster=cluster->next;
      MagickFreeMemory(cluster);
      head=(Cluster *) NULL;
    }
  MagickFreeMemory(cluster_array);
  squares-=255;
  free_squares=squares;
  MagickFreeMemory(free_squares);
  return(status);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   C o n s o l i d a t e C r o s s i n g s                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method ConsolidateCrossings guarantees that an even number of zero
%  crossings always lie between two crossings.
%
%  The format of the ConsolidateCrossings method is:
%
%      ConsolidateCrossings(zero_crossing,number_crossings)
%
%  A description of each parameter follows.
%
%    o zero_crossing: Specifies an array of structures of type ZeroCrossing.
%
%    o number_crossings: This unsigned int specifies the number of elements
%      in the zero_crossing array.
%
%
*/
static void
ConsolidateCrossings(ZeroCrossing *zero_crossing,
                     const unsigned int number_crossings)
{
  int
    center,
    correct,
    count,
    left,
    right;

  register long
    i,
    j,
    k,
    l;

  /*
    Consolidate zero crossings.
  */
  for (i=(long) number_crossings-1; i >= 0; i--)
    for (j=0; j <= 255; j++)
      {
        if (zero_crossing[i].crossings[j] == 0)
          continue;
        /*
          Find the entry that is closest to j and still preserves the
          property that there are an even number of crossings between
          intervals.
        */
        for (k=j-1; k > 0; k--)
          if (zero_crossing[i+1].crossings[k] != 0)
            break;
        left=Max(k,0);
        center=j;
        for (k=j+1; k < 255; k++)
          if (zero_crossing[i+1].crossings[k] != 0)
            break;
        right=Min(k,255);
        /*
          K is the zero crossing just left of j.
        */
        for (k=j-1; k > 0; k--)
          if (zero_crossing[i].crossings[k] != 0)
            break;
        if (k < 0)
          k=0;
        /*
          Check center for an even number of crossings between k and j.
        */
        correct=(-1);
        if (zero_crossing[i+1].crossings[j] != 0)
          {
            count=0;
            for (l=k+1; l < center; l++)
              if (zero_crossing[i+1].crossings[l] != 0)
                count++;
            if ((count % 2) == 0)
              if (center != k)
                correct=center;
          }
        /*
          Check left for an even number of crossings between k and j.
        */
        if (correct == -1)
          {
            count=0;
            for (l=k+1; l < left; l++)
              if (zero_crossing[i+1].crossings[l] != 0)
                count++;
            if ((count % 2) == 0)
              if (left != k)
                correct=left;
          }
        /*
          Check right for an even number of crossings between k and j.
        */
        if (correct == -1)
          {
            count=0;
            for (l=k+1; l < right; l++)
              if (zero_crossing[i+1].crossings[l] != 0)
                count++;
            if ((count % 2) == 0)
              if (right != k)
                correct=right;
          }
        l=zero_crossing[i].crossings[j];
        zero_crossing[i].crossings[j]=0;
        if (correct != -1)
          zero_crossing[i].crossings[correct]=(short) l;
      }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e f i n e R e g i o n                                                   %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method DefineRegion defines the left and right boundaries of a peak
%  region.
%
%  The format of the DefineRegion method is:
%
%      int DefineRegion(const short *extrema,ExtentPacket *extents)
%
%  A description of each parameter follows.
%
%    o extrema:  Specifies a pointer to an array of integers.  They
%      represent the peaks and valleys of the histogram for each color
%      component.
%
%    o extents:  This pointer to an ExtentPacket represent the extends
%      of a particular peak or valley of a color component.
%
%
*/
static int
DefineRegion(const short *extrema,ExtentPacket *extents)
{
  /*
    Initialize to default values.
  */
  extents->left=0;
  extents->center=0;
  extents->right=255;
  /*
    Find the left side (maxima).
  */
  for ( ; extents->index <= 255; extents->index++)
    if (extrema[extents->index] > 0)
      break;
  if (extents->index > 255)
    return(False);  /* no left side - no region exists */
  extents->left=extents->index;
  /*
    Find the right side (minima).
  */
  for ( ; extents->index <= 255; extents->index++)
    if (extrema[extents->index] < 0)
      break;
  extents->right=extents->index-1;
  return(True);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   D e r i v a t i v e H i s t o g r a m                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method DerivativeHistogram determines the derivative of the histogram
%  using central differencing.
%
%  The format of the DerivativeHistogram method is:
%
%      DerivativeHistogram(const double *histogram,double *derivative
%
%  A description of each parameter follows.
%
%    o histogram: Specifies an array of doubles representing the number of
%      pixels for each intensity of a particular color component.
%
%    o derivative: This array of doubles is initialized by DerivativeHistogram
%      to the derivative of the histogram using central differencing.
%
%
*/
static void
DerivativeHistogram(const double *histogram,double *derivative)
{
  register long
    i,
    n;

  /*
    Compute endpoints using second order polynomial interpolation.
  */
  n=255;
  derivative[0]=(-1.5*histogram[0]+2.0*histogram[1]-0.5*histogram[2]);
  derivative[n]=(0.5*histogram[n-2]-2.0*histogram[n-1]+1.5*histogram[n]);
  /*
    Compute derivative using central differencing.
  */
  for (i=1; i < n; i++)
    derivative[i]=((double) histogram[i+1]-histogram[i-1])/2.0;
  return;
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+  I n i t i a l i z e H i s t o g r a m                                      %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method InitializeHistogram computes the histogram for an image.
%
%  The format of the InitializeHistogram method is:
%
%      InitializeHistogram(Image *image,long **histogram)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o histogram: Specifies an array of integers representing the number
%      of pixels for each intensity of a particular color component.
%
%
*/
static void
InitializeHistogram(Image *image,long **histogram)
{
  long
    y;

  register const PixelPacket
    *p;

  register long
    i,
    x;

  /*
    Initialize histogram.
  */
  for (i=0; i <= 255; i++)
    {
      histogram[Red][i]=0;
      histogram[Green][i]=0;
      histogram[Blue][i]=0;
    }
  for (y=0; y < (long) image->rows; y++)
    {
      p=AcquireImagePixels(image,0,y,image->columns,1,&image->exception);
      if (p == (const PixelPacket *) NULL)
        break;
      for (x=0; x < (long) image->columns; x++)
        {
          histogram[Red][ScaleQuantumToChar(p->red)]++;
          histogram[Green][ScaleQuantumToChar(p->green)]++;
          histogram[Blue][ScaleQuantumToChar(p->blue)]++;
          p++;
        }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   I n i t i a l i z e I n t e r v a l T r e e                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method InitializeIntervalTree initializes an interval tree from the
%  lists of zero crossings.
%
%  The format of the InitializeIntervalTree method is:
%
%      InitializeIntervalTree(const ZeroCrossing *zero_crossing,
%                             const unsigned int number_crossings)
%
%  A description of each parameter follows.
%
%    o zero_crossing: Specifies an array of structures of type ZeroCrossing.
%
%    o number_crossings: This unsigned int specifies the number of elements
%      in the zero_crossing array.
%
%
*/

static void
InitializeList(IntervalTree **list,int *number_nodes,IntervalTree *node)
{
  if (node == (IntervalTree *) NULL)
    return;
  if (node->child == (IntervalTree *) NULL)
    list[(*number_nodes)++]=node;
  InitializeList(list,number_nodes,node->sibling);
  InitializeList(list,number_nodes,node->child);
}

static void
MeanStability(register IntervalTree *node)
{
  register IntervalTree
    *child;

  if (node == (IntervalTree *) NULL)
    return;
  node->mean_stability=0.0;
  child=node->child;
  if (child != (IntervalTree *) NULL)
    {
      register double
        sum;

      register long
        count;

      sum=0.0;
      count=0;
      for ( ; child != (IntervalTree *) NULL; child=child->sibling)
        {
          sum+=child->stability;
          count++;
        }
      node->mean_stability=sum/count;
    }
  MeanStability(node->sibling);
  MeanStability(node->child);
}

static void Stability(register IntervalTree *node)
{
  if (node == (IntervalTree *) NULL)
    return;
  if (node->child == (IntervalTree *) NULL)
    node->stability=0.0;
  else
    node->stability=node->tau-(node->child)->tau;
  Stability(node->sibling);
  Stability(node->child);
}

static IntervalTree *
InitializeIntervalTree(const ZeroCrossing *zero_crossing,
                       const unsigned int number_crossings)
{
  int
    left,
    number_nodes;

  IntervalTree
    *head,
    **list,
    *node,
    *root;

  register long
    i,
    j,
    k;

  /*
    Allocate interval tree.
  */
  list=MagickAllocateMemory(IntervalTree **,TreeLength*sizeof(IntervalTree *));
  if (list == (IntervalTree **) NULL)
    return((IntervalTree *) NULL);
  /*
    The root is the entire histogram.
  */
  root=MagickAllocateMemory(IntervalTree *,sizeof(IntervalTree));
  root->child=(IntervalTree *) NULL;
  root->sibling=(IntervalTree *) NULL;
  root->tau=0.0;
  root->left=0;
  root->right=255;
  for (i=(-1); i < (long) number_crossings; i++)
    {
      /*
        Initialize list with all nodes with no children.
      */
      number_nodes=0;
      InitializeList(list,&number_nodes,root);
      /*
        Split list.
      */
      for (j=0; j < number_nodes; j++)
        {
          head=list[j];
          left=head->left;
          node=head;
          for (k=head->left+1; k < head->right; k++)
            {
              if (zero_crossing[i+1].crossings[k] != 0)
                {
                  if (node == head)
                    {
                      node->child=MagickAllocateMemory(IntervalTree *,
                                                       sizeof(IntervalTree));
                      node=node->child;
                    }
                  else
                    {
                      node->sibling=MagickAllocateMemory(IntervalTree *,
                                                         sizeof(IntervalTree));
                      node=node->sibling;
                    }
                  node->tau=zero_crossing[i+1].tau;
                  node->child=(IntervalTree *) NULL;
                  node->sibling=(IntervalTree *) NULL;
                  node->left=left;
                  node->right=k;
                  left=k;
                }
            }
          if (left != head->left)
            {
              node->sibling=MagickAllocateMemory(IntervalTree *,sizeof(IntervalTree));
              node=node->sibling;
              node->tau=zero_crossing[i+1].tau;
              node->child=(IntervalTree *) NULL;
              node->sibling=(IntervalTree *) NULL;
              node->left=left;
              node->right=head->right;
            }
        }
    }
  /*
    Determine the stability: difference between a nodes tau and its child.
  */
  Stability(root->child);
  MeanStability(root->child);
  MagickFreeMemory(list);
  return(root);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   O p t i m a l T a u                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method OptimalTau finds the optimal tau for each band of the histogram.
%
%  The format of the OptimalTau method is:
%
%    double OptimalTau(const long *histogram,const double max_tau,
%                      const double min_tau,const double delta_tau,
%                      const double smoothing_threshold,
%                      short *extrema)
%
%  A description of each parameter follows.
%
%    o histogram: Specifies an array of integers representing the number
%      of pixels for each intensity of a particular color component.
%
%    o max_tau: (current 5.2f)
%
%    o min_tau: (current 0.2)
%
%    o delta_tau: (current 0.5f)
%
%    o smoothing_threshold: If the absolute value of a second derivative
%       point is less than smoothing_threshold then that derivative point
%       is ignored (i.e. set to 0) while evaluating zero crossings.  This
%       causes small variations (could be noise) to be ignored.
%
%    o extrema:  Specifies a pointer to an array of integers.  They
%      represent the peaks and valleys of the histogram for each color
%      component.
%
%
*/

static void
ActiveNodes(IntervalTree **list,int *number_nodes,IntervalTree *node)
{
  if (node == (IntervalTree *) NULL)
    return;
  if (node->stability >= node->mean_stability)
    {
      list[(*number_nodes)++]=node;
      ActiveNodes(list,number_nodes,node->sibling);
    }
  else
    {
      ActiveNodes(list,number_nodes,node->sibling);
      ActiveNodes(list,number_nodes,node->child);
    }
}

static void
FreeNodes(IntervalTree *node)
{
  if (node == (IntervalTree *) NULL)
    return;
  FreeNodes(node->sibling);
  FreeNodes(node->child);
  MagickFreeMemory(node);
}

static double
OptimalTau(const long *histogram,const double max_tau,
           const double min_tau,const double delta_tau,
           const double smoothing_threshold,
           short *extrema)
{
  double
    average_tau,
    *derivative,
    *second_derivative,
    tau,
    value;

  int
    index,
    number_nodes,
    peak,
    x;

  IntervalTree
    **list,
    *node,
    *root;

  register long
    i,
    j,
    k;

  unsigned long
    count,
    number_crossings;

  ZeroCrossing
    *zero_crossing;

  /*
    Allocate interval tree.
  */
  list=MagickAllocateMemory(IntervalTree **,TreeLength*sizeof(IntervalTree *));
  if (list == (IntervalTree **) NULL)
    return(0.0);
  /*
    Allocate zero crossing list.
  */
  count=(unsigned long) ((max_tau-min_tau)/delta_tau)+2;
  zero_crossing=MagickAllocateMemory(ZeroCrossing *,count*sizeof(ZeroCrossing));
  if (zero_crossing == (ZeroCrossing *) NULL)
    return(0.0);
  for (i=0; i < (long) count; i++)
    zero_crossing[i].tau=(-1);
  /*
    Initialize zero crossing list.
  */
  derivative=MagickAllocateMemory(double *,256*sizeof(double));
  second_derivative=MagickAllocateMemory(double *,256*sizeof(double));
  if ((derivative == (double *) NULL) || (second_derivative == (double *) NULL))
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                      UnableToAllocateDerivatives);
  i=0;
  for (tau=max_tau; tau >= min_tau; tau-=delta_tau)
    {
      zero_crossing[i].tau=tau;
      ScaleSpace(histogram,tau,zero_crossing[i].histogram);
      DerivativeHistogram(zero_crossing[i].histogram,derivative);
      DerivativeHistogram(derivative,second_derivative);
      ZeroCrossHistogram(second_derivative,smoothing_threshold,
                         zero_crossing[i].crossings);
      i++;
    }
  /*
    Add an entry for the original histogram.
  */
  zero_crossing[i].tau=0.0;
  for (j=0; j <= 255; j++)
    zero_crossing[i].histogram[j]=(double) histogram[j];
  DerivativeHistogram(zero_crossing[i].histogram,derivative);
  DerivativeHistogram(derivative,second_derivative);
  ZeroCrossHistogram(second_derivative,smoothing_threshold,
                     zero_crossing[i].crossings);
  number_crossings=i;
  MagickFreeMemory(derivative);
  MagickFreeMemory(second_derivative);
  /*
    Ensure the scale-space fingerprints form lines in scale-space, not loops.
  */
  ConsolidateCrossings(zero_crossing,number_crossings);
  /*
    Force endpoints to be included in the interval.
  */
  for (i=0; i <= (long) number_crossings; i++)
    {
      for (j=0; j < 255; j++)
        if (zero_crossing[i].crossings[j] != 0)
          break;
      zero_crossing[i].crossings[0]=(-zero_crossing[i].crossings[j]);
      for (j=255; j > 0; j--)
        if (zero_crossing[i].crossings[j] != 0)
          break;
      zero_crossing[i].crossings[255]=
        (-zero_crossing[i].crossings[j]);
    }
  /*
    Initialize interval tree.
  */
  root=InitializeIntervalTree(zero_crossing,number_crossings);
  if (root == (IntervalTree *) NULL)
    return(0.0);
  /*
    Find active nodes:  stability is greater (or equal) to the mean stability of
    its children.
  */
  number_nodes=0;
  ActiveNodes(list,&number_nodes,root->child);
  /*
    Initialize extrema.
  */
  for (i=0; i <= 255; i++)
    extrema[i]=0;
  for (i=0; i < number_nodes; i++)
    {
      /*
        Find this tau in zero crossings list.
      */
      k=0;
      node=list[i];
      for (j=0; j <= (long) number_crossings; j++)
        if (zero_crossing[j].tau == node->tau)
          k=j;
      /*
        Find the value of the peak.
      */
      peak=zero_crossing[k].crossings[node->right] == -1;
      index=node->left;
      value=zero_crossing[k].histogram[index];
      for (x=node->left; x <= node->right; x++)
        {
          if (peak)
            {
              if (zero_crossing[k].histogram[x] > value)
                {
                  value=zero_crossing[k].histogram[x];
                  index=x;
                }
            }
          else
            if (zero_crossing[k].histogram[x] < value)
              {
                value=zero_crossing[k].histogram[x];
                index=x;
              }
        }
      for (x=node->left; x <= node->right; x++)
        {
          if (index == 0)
            index=256;
          if (peak)
            extrema[x]=index;
          else
            extrema[x]=(-index);
        }
    }
  /*
    Determine the average tau.
  */
  average_tau=0.0;
  for (i=0; i < number_nodes; i++)
    average_tau+=list[i]->tau;
  average_tau/=(double) number_nodes;
  /*
    Free memory.
  */
  FreeNodes(root);
  MagickFreeMemory(zero_crossing);
  MagickFreeMemory(list);
  return(average_tau);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   S c a l e S p a c e                                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method ScaleSpace performs a scale-space filter on the 1D histogram.
%
%  The format of the ScaleSpace method is:
%
%      ScaleSpace(const long *histogram,const double tau,
%                 double *scaled_histogram)
%
%  A description of each parameter follows.
%
%    o histogram: Specifies an array of doubles representing the number of
%      pixels for each intensity of a particular color component.
%
%    o tau:
%
%    o scaled_histogram:
%
*/
static void
ScaleSpace(const long *histogram,const double tau,double *scaled_histogram)
{
  double
    alpha,
    beta,
    *gamma,
    sum;

  register long
    u,
    x;

  gamma=MagickAllocateMemory(double *,256*sizeof(double));
  if (gamma == (double *) NULL)
    MagickFatalError3(ResourceLimitFatalError,MemoryAllocationFailed,
                      UnableToAllocateGammaMap);
  alpha=1.0/(tau*sqrt(2.0*MagickPI));
  beta=(-1.0/(2.0*tau*tau));
  for (x=0; x <= 255; x++)
    gamma[x]=0.0;
  for (x=0; x <= 255; x++)
    {
      gamma[x]=exp(beta*x*x);
      if (gamma[x] < MagickEpsilon)
        break;
    }
  for (x=0; x <= 255; x++)
    {
      sum=0.0;
      for (u=0; u <= 255; u++)
        sum+=(double) histogram[u]*gamma[AbsoluteValue(x-u)];
      scaled_histogram[x]=alpha*sum;
    }
  MagickFreeMemory(gamma);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
+   Z e r o C r o s s H i s t o g r a m                                       %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%  Method ZeroCrossHistogram find the zero crossings in a histogram and
%  marks directions as:  1 is negative to positive; 0 is zero crossing; and
%  -1 is positive to negative.
%
%  The format of the ZeroCrossHistogram method is:
%
%      ZeroCrossHistogram(double *second_derivative,
%        const double smoothing_threshold,short *crossings)
%
%  A description of each parameter follows.
%
%    o second_derivative: Specifies an array of doubles representing the
%      second derivative of the histogram of a particular color component.
%
%    o smoothing_threshold: If the absolute value of a second derivative
%       point is less than smoothing_threshold then that derivative point
%       is ignored (i.e. set to 0) while evaluating zero crossings.  This
%       causes small variations (could be noise) to be ignored.
%
%    o crossings:  This array of integers is initialized with
%      -1, 0, or 1 representing the slope of the first derivative of the
%      of a particular color component.
%
%
*/
static void
ZeroCrossHistogram(double *second_derivative,const double smoothing_threshold,
                   short *crossings)
{
  int
    parity;

  register long
    i;

  /*
    Merge low numbers to zero to help prevent noise.
  */
  for (i=0; i <= 255; i++)
    if ((second_derivative[i] < smoothing_threshold) &&
        (second_derivative[i] >= -smoothing_threshold))
      second_derivative[i]=0.0;
  /*
    Mark zero crossings.
  */
  parity=0;
  for (i=0; i <= 255; i++)
    {
      crossings[i]=0;
      if (second_derivative[i] < 0.0)
        {
          if (parity > 0)
            crossings[i]=(-1);
          parity=1;
        }
      else
        if (second_derivative[i] > 0.0)
          {
            if (parity < 0)
              crossings[i]=1;
            parity=(-1);
          }
    }
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  S e g m e n t I m a g e                                                    %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Method SegmentImage segment an image by analyzing the histograms of the
%  color components and identifying units that are homogeneous with the fuzzy
%  c-means technique.
%
%  Specify cluster threshold as the number of pixels in each cluster must
%  exceed the the cluster threshold to be considered valid.  Smoothing
%  threshold eliminates noise in the second derivative of the histogram.
%  As the value is increased, you can expect a smoother second derivative.
%  The default is 1.5.
%
%  The format of the SegmentImage method is:
%
%      unsigned int SegmentImage(Image *image,const ColorspaceType colorspace,
%        const unsigned int verbose,const double cluster_threshold,
%        const double smoothing_threshold)
%
%  A description of each parameter follows.
%
%    o image: Specifies a pointer to an Image structure;  returned from
%      ReadImage.
%
%    o colorspace: An unsigned integer value that indicates the colorspace.
%      Empirical evidence suggests that distances in YUV or YIQ correspond to
%      perceptual color differences more closely than do distances in RGB
%      space.  The image is then returned to RGB colorspace after color
%      reduction.
%
%    o verbose:  A value greater than zero prints detailed information about
%      the identified classes.
%
%    o cluster_threshold: The minimum number of total pixels contained
%      in a hexahedra before it can be considered valid (expressed as a
%      percentage of total pixels).  This is used to eliminate seldom
%      used colors.
%
%    o smoothing_threshold: If the absolute value of a second derivative
%       point is less than smoothing_threshold then that derivative point
%       is ignored (i.e. set to 0) while evaluating zero crossings.  This
%       causes small variations (could be noise) to be ignored.
%
*/
MagickExport MagickPassFail
SegmentImage(Image *image,
             const ColorspaceType colorspace,
             const unsigned int verbose,
             const double cluster_threshold,
             const double smoothing_threshold)
{

  long
    *histogram[MaxDimension];

  register long
    i;

  short
    *extrema[MaxDimension];

  MagickPassFail
    status;

  /*
    Allocate histogram and extrema.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  for (i=0; i < MaxDimension; i++)
    {
      histogram[i]=MagickAllocateMemory(long *,256*sizeof(long));
      extrema[i]=MagickAllocateMemory(short *,256*sizeof(short));
      if ((histogram[i] == (long *) NULL) || (extrema[i] == (short *) NULL))
        {
          for (i-- ; i >= 0; i--)
            {
              MagickFreeMemory(extrema[i]);
              MagickFreeMemory(histogram[i]);
            }
          ThrowBinaryException(ResourceLimitError,MemoryAllocationFailed,
                               image->filename);
        }
    }
  (void) TransformColorspace(image,colorspace);
  /*
    Initialize histogram.
  */
  InitializeHistogram(image,histogram);
  (void) OptimalTau(histogram[Red],Tau,0.2,DeltaTau,smoothing_threshold,
                    extrema[Red]);
  (void) OptimalTau(histogram[Green],Tau,0.2,DeltaTau,smoothing_threshold,
                    extrema[Green]);
  (void) OptimalTau(histogram[Blue],Tau,0.2,DeltaTau,smoothing_threshold,
                    extrema[Blue]);
  if (verbose > 1)
    {
      FILE
        *stream;

      stream=stdout;

      fprintf(stream,"Red Histogram:\n");
      DumpHistogramArray(stream,256,histogram[Red]);
      fprintf(stream,"Green Histogram:\n");
      DumpHistogramArray(stream,256,histogram[Green]);
      fprintf(stream,"Blue Histogram:\n");
      DumpHistogramArray(stream,256,histogram[Blue]);

      fprintf(stream,"Red Extrema:\n");
      DumpExtremaArray(stream,256,extrema[Red]);
      fprintf(stream,"Green Extrema:\n");
      DumpExtremaArray(stream,256,extrema[Green]);
      fprintf(stream,"Blue Extrema:\n");
      DumpExtremaArray(stream,256,extrema[Blue]);
    }
  /*
    Classify using the fuzzy c-Means technique.
  */
  status=Classify(image,extrema,cluster_threshold,(double)WeightingExponent,verbose);
  (void) TransformColorspace(image,RGBColorspace);
  /*
    Free memory.
  */
  for (i=0; i < MaxDimension; i++)
    {
      MagickFreeMemory(extrema[i]);
      MagickFreeMemory(histogram[i]);
    }
  return(status);
}
