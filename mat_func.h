#ifndef MAT_FUNC
#define MAT_FUNC

#include<time.h>
#include<stdlib.h>

#define MAT_EPSILON 0.00001

void mat_print(double** mat, unsigned int n, const char* label);
double** mat_generate(unsigned int n);
void mat_free(double** mat, unsigned int n);
void mat_LU_split(double** LU, double*** L, double*** U, unsigned int n);
double** mat_square_alloc(unsigned int n);
double** mat_mult(double** l, double** r, unsigned int n);
char mat_cmp(double** r, double** l, unsigned int n);
double** mat_copy(double** mat, unsigned int n);

#endif // !MAT_FUNC
