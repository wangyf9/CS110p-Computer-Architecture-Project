#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <omp.h>

#include <mmintrin.h> //MMX
#include <xmmintrin.h> //SSE(include mmintrin.h)
#include <emmintrin.h> //SSE2(include xmmintrin.h)
#include <pmmintrin.h> //SSE3(include emmintrin.h)
#include <tmmintrin.h>//SSSE3(include pmmintrin.h)
#include <smmintrin.h>//SSE4.1(include tmmintrin.h)
#include <nmmintrin.h>//SSE4.2(include smmintrin.h)
#include <immintrin.h>//AVX(include wmmintrin.h)
#define NSPEEDS         9
#define NUM_THREADS     28

typedef struct
{
  float speeds[NSPEEDS];
} t_speed;

typedef struct
{
  float  density;       /* density per cell */
  float  viscosity;     /* kinematic viscosity of fluid */
  float  velocity;      /* inlet velocity */
  float  omega;         /* relaxation parameter */
  int    nx;            /* no. of cells in x-direction */
  int    ny;            /* no. of cells in y-direction */
  int    maxIters;      /* no. of iterations */
  int    type;          /* inlet type */

  
} t_param;

/* struct to hold the distribution of different speeds */


#endif