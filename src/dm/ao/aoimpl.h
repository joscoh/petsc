/* $Id: aoimpl.h,v 1.23 2000/05/10 16:43:19 bsmith Exp bsmith $ */
/* 
   This private file should not be included in users' code.
*/

#ifndef __AOIMPL 
#define __AOIMPL
#include "petscao.h"

/*
    Defines the abstract AO operations
*/
struct _AOOps {
  int (*petsctoapplication)(AO,int,int*),  /* map a set of integers to application order */
      (*applicationtopetsc)(AO,int,int*);   
  int (*destroy)(AO);
  int (*view)(AO,PetscViewer);
};

struct _p_AO {
  PETSCHEADER(struct _AOOps)
  void          *data;                   /* implementation-specific data */
  int           N,n;                    /* global, local vector size */
};

/*
    Defines the abstract AOData operations
*/
struct _AODataOps {
  int (*segmentadd)(AOData,char *,char *,int,int,int*,void*,PetscDataType);
  int (*segmentget)(AOData,char *,char*,int,int*,void**);
  int (*segmentrestore)(AOData,char *,char *,int,int*,void**);
  int (*segmentgetlocal)(AOData,char *,char*,int,int*,void**);
  int (*segmentrestorelocal)(AOData,char *,char *,int,int*,void**);
  int (*segmentgetreduced)(AOData,char *,char*,int,int*,IS *);
  int (*segmentgetextrema)(AOData,char *,char*,void *,void *);
  int (*keyremap)(AOData,char *,AO);
  int (*keygetadjacency)(AOData,char *,Mat*);
  int (*keygetactive)(AOData,char*,char*,int,int*,int,IS*);
  int (*keygetactivelocal)(AOData,char*,char*,int,int*,int,IS*);
  int (*segmentpartition)(AOData,char*,char*);
  int (*keyremove)(AOData,char*);
  int (*segmentremove)(AOData,char*,char*);
  int (*destroy)(AOData);
  int (*view)(AOData,PetscViewer);
};

/*
      A AODate object consists of 

           - key1 
	       * name      = name of first key
               * N         = number of key entries
               * nlocal    = number of local key entries
               * nsegments = number of segments in first key  
               * ltog      = local to global mapping 
               - segment1 
                  * name      = name of first segment in first key
                  * bs        = blocksize of first segment in first key
                  * datatype  = datatype of first segment in first key

               - segment2

                  ....

            - key2

                ....
*/       
typedef struct __AODataSegment AODataSegment; 
struct __AODataSegment {
  void              *data;                   /* implementation-specific data */
  char              *name;
  int               bs;                      /* block size of basic chunk */
  PetscDataType     datatype;                /* type of data item, int, double etc */
  AODataSegment     *next;  
};

typedef struct __AODataKey AODataKey;
struct __AODataKey {
  void                   *data;                   /* implementation-specific data */
  char                   *name;
  int                    N;                       /* number of key entries */
  int                    nsegments;               /* number of segments in key */
  AODataSegment          *segments;
  ISLocalToGlobalMapping ltog;

  /* should the following be so public? */

  int                    nlocal;                  /* number of key entries owned locally */
  int                    *rowners;                /* ownership range of each processor */
  int                    rstart,rend;             /* first and 1 + last owned locally */
  AODataKey              *next;
};

typedef struct __AODataAlias AODataAlias;         /* allows a field or key to have several names */
struct __AODataAlias {
  char        *alias;
  char        *name;
  AODataAlias *next;
};

struct _p_AOData {
  PETSCHEADER(struct _AODataOps)
  int               nkeys;                   /* current number of keys */
  AODataKey         *keys;
  void              *data;
  int               datacomplete;            /* indicates all AOData object is fully set */
  AODataAlias       *aliases;
};

EXTERN int AODataKeyFind_Private(AOData,char *,PetscTruth *,AODataKey **);
EXTERN int AODataSegmentFind_Private(AOData,char *,char *,PetscTruth *,AODataKey **,AODataSegment **);


#include "petscbt.h"

struct _p_AOData2dGrid {
   int     cell_n, vertex_n, edge_n;
   int     cell_max, vertex_max, edge_max;
   int     *cell_vertex,*cell_edge,*cell_cell;
   double  *vertex;
   double  xmin,xmax,ymin,ymax;
   int     *edge_vertex,*edge_cell;
   PetscBT vertex_boundary;
};


#endif
