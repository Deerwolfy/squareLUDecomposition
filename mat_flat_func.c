#include"mat_flat_func.h"

double* mat_flat_generate(unsigned int n)
{
  unsigned int nn = n*n;
  double* matrix = (double*) calloc(nn,sizeof(double));
  for(unsigned int i = 0; i < nn; ++i){
    matrix[i] = rand() % 100;
  }
  return matrix;
}

double** get_square_view(double* flat, unsigned int n)
{
  double** square = (double**) malloc(n*sizeof(double*));
  for(unsigned int i = 0; i < n; ++i){
    square[i] = &(flat[i*n]);
  }
  return square;
}

double* mat_flat_copy(double* mat, unsigned int n)
{
  unsigned int nn = n*n;
  double* copy = (double*) calloc(nn,sizeof(double));
  for(unsigned int i = 0; i < nn; ++i){
    copy[i] = mat[i];
  }
  return copy;
}
