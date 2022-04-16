#ifndef MAT_FLAT_FUNC
#define MAT_FLAT_FUNC

#include<stdlib.h>

double* mat_flat_generate(unsigned int n);
double** get_square_view(double* flat, unsigned int n);
double* mat_flat_copy(double* mat, unsigned int n);

#endif
