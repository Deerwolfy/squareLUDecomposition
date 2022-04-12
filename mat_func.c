#include"mat_func.h"
#include<stdio.h>
#include<math.h>

void mat_print(double** mat, unsigned int n, const char* label)
{
  if (mat != NULL) {
    if (label)
      printf("%s\n", label);
    for (unsigned int i = 0; i < n; ++i) {
      for (unsigned int j = 0; j < n; ++j) {
        if (mat[i] != NULL) {
          printf("%8.4f ", mat[i][j]);
        }
      }
      putchar('\n');
    }
  }
}

double** mat_generate(unsigned int n)
{
  double** mat = (double**) malloc(n*sizeof(double*));
  if (mat != NULL) {
    for (unsigned int i = 0; i < n; ++i) {
      mat[i] = (double*) malloc(n * sizeof(double));
      if (mat[i] != NULL) {
        for (unsigned int j = 0; j < n; ++j) {
          mat[i][j] = (((int)((rand()%100)*1000))/1000);
        }
      }
    }
  }
  return mat;
}

double** mat_square_alloc(unsigned int n)
{
  double** mat = (double**)malloc(n * sizeof(double*));
  if (mat != NULL) {
    for (unsigned int i = 0; i < n; ++i) {
      mat[i] = (double*)malloc(n * sizeof(double));
      if (mat[i] != NULL) {
        for (unsigned int j = 0; j < n; ++j) {
          mat[i][j] = 0;
        }
      }
    }
  }
  return mat;
}

void mat_free(double **mat, unsigned int n)
{
  if (mat != NULL) {
    for (unsigned int i = 0; i < n; ++i) {
      free(mat[i]);
    }
  }
  free(mat);
}

void mat_LU_split(double **LU, double ***L, double ***U, unsigned int n)
{
  if (!U || !L || !LU) return;
  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int j = 0; j < n; ++j) {
      if (j < i) {
        (*L)[i][j] = LU[i][j];
      }
      else if (j >= i) {
        (*U)[i][j] = LU[i][j];
        if (j == i) {
          (*L)[i][j] = 1;
        }
      }
    }
  }
}

double** mat_mult(double** l, double** r, unsigned int n)
{
  double** result = mat_square_alloc(n);
  for (unsigned int i = 0; i < n; ++i) {
    for (unsigned int j = 0; j < n; ++j) {
      for (unsigned int k = 0; k < n; ++k) {
        result[i][j] += l[i][k] * r[k][j];
      }
    }
  }
  return result;
}

int sign(double num)
{
  return (num<0)?-1:1;
}

char mat_cmp(double** r, double** l, unsigned int n)
{
  for(unsigned int i = 0; i < n; ++i){
    for(unsigned int j = 0; j < n; ++j){
      if(fabs(r[i][j] - l[i][j]) > MAT_EPSILON){
        return sign(r[i][j] - l[i][j]);
      }
    }
  }
  return 0;
}

double** mat_copy(double** mat, unsigned int n)
{
  double** copy = mat_square_alloc(n);
  for(unsigned int i = 0; i < n; ++i){
    for(unsigned int j = 0; j < n; ++j){
      copy[i][j] = mat[i][j];
    }
  }
  return copy;
}
