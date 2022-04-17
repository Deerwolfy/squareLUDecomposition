#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include"mat_func.h"
#include"defs.h"

char decompose(double** source, double*** l, double*** u, unsigned int n);
void print_verbose(double** matrix, double** source, double** l, double** u, double** result, unsigned int n, char LU_exists);

char decompose(double** source, double*** l, double*** u, unsigned int n)
{
  for (unsigned int k = 0; k < n-1; ++k) {
    for (unsigned int a = k+1; a < n; ++a) {
      if(fabs(source[k][k]) < MY_EPSILON){
        return 0;
      } else {
        source[a][k] = source[a][k] / source[k][k];
      }
      for (unsigned int i = k+1; i < n; ++i) {
        source[a][i] = source[a][i] - source[a][k] * source[k][i];
      }
    }
  }
  mat_LU_split(source,l,u,n);
  return 1;
}

void print_verbose(double** matrix, double** source, double** l, double** u, double** result, unsigned int n, char LU_exists)
{
  mat_print(source, n, "Source matrix:");
  putchar('\n');
  if(!LU_exists){
    printf("LU decomposition does not exists for given matrix!\n");
    return;
  }
  mat_print(matrix, n, "Before split:");
  putchar('\n');
  mat_print(u, n, "U matrix");
  putchar('\n');
  mat_print(l, n, "L matrix");
  putchar('\n');
  mat_print(result, n, "Mult result matrix");
  putchar('\n');
  printf("%s", (mat_cmp(source,result,n))?"FALSE":"TRUE");
  putchar('\n');
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  int n = 3;
  unsigned int num_of_tests = 1;
  if (argc >= 2) {
    n = atoi(argv[1]);
  }
  if (argc >= 3) {
    num_of_tests = atoi(argv[2]);
  }

  for(unsigned int i = 0; i < num_of_tests; ++i) {
    double** matrix = mat_generate(n);
    if(!matrix){
      printf("Failed to allocate memory for matrix.");
    }
    double** source = mat_copy(matrix,n);
    if(!source){
      printf("Failed to allocate memory for source.");
    }
    double** l = mat_square_alloc(n);
    if(!l){
      printf("Failed to allocate memory for l.");
    }
    double** u = mat_square_alloc(n);
    if(!u){
      printf("Failed to allocate memory for u.");
    }
    char is_LU_exists = decompose(matrix,&l,&u,n);
    double** test_result = mat_mult(l,u,n);
    if(!test_result){
      printf("Failed to allocate memory for test_result.");
    }

    if(num_of_tests < 5){
      print_verbose(matrix,source,l,u,test_result,n,is_LU_exists);
    }
    else {
      if(is_LU_exists){
        if(mat_cmp(source,test_result,n) != 0){
          putchar('!');
          print_verbose(matrix,source,l,u,test_result,n,1);
          return 1;
        }
        else {
          putchar('.');
        }
      } else {
        putchar('?');
      }
    }

    mat_free(l,n);
    mat_free(u,n);
    mat_free(matrix, n);
    mat_free(source, n);
    mat_free(test_result, n);
  }
  putchar('\n');
  return 0;
}
