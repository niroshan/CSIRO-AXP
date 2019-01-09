#include "stdafx.h"

/********************** #defines ********************************/

#define PI            3.141592654
#define NN              513 


/* (C) Copr. 1986-92 Numerical Recipes Software #p21E6W)1.1&iE10(9p#. */
/* ---------------------- end-of-file (h source) ---------------------- */

#define NR_END          1
#define FREE_ARG        char*
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#define SQR(a) (a == 0.0 ? 0.0 : a * a)
/* End num rec.....*/

/********************* Function Declarations *******************/

//void fft_print_array (const char *msg, unsigned long nn, float data[]);
void four1(float data[], unsigned long nn, int isign);
void realft(float data[], unsigned long n, int isign);
float * vector(long nl, long nh);
void free_vector(float *v, long nl, long nh);
void paintblock(HWND hWnd);
