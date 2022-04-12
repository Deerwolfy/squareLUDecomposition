#include<stdio.h>
#include<stdlib.h>
#include"mat_func.h"

void decompose(double** source, double*** l, double*** u, unsigned int n);
void print_verbose(double** matrix, double** source, double** l, double** u, double** result, unsigned int n);

void decompose(double** source, double*** l, double*** u, unsigned int n)
{
        for (int k = 0; k < n-1; ++k) {
                for (int a = k+1; a < n; ++a) {
                    source[a][k] = source[a][k] / source[k][k];
                    for (int i = k+1; i < n; ++i) {
                        source[a][i] = source[a][i] - source[a][k] * source[k][i];
                    }
                }
        }
        mat_LU_split(source,l,u,n);
}

void print_verbose(double** matrix, double** source, double** l, double** u, double** result, unsigned int n)
{
        mat_print(matrix, n, "Source matrix:");
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
                double** source = mat_copy(matrix,n);
                double** l = mat_square_alloc(n);
                double** u = mat_square_alloc(n);
                decompose(matrix,&l,&u,n);
                double** test_result = mat_mult(l,u,n);

                if(num_of_tests < 5){
                        print_verbose(matrix,source,l,u,test_result,n);
                }
                else {
                        if(mat_cmp(source,test_result,n) != 0){
                              putchar('!');
                              print_verbose(matrix,source,l,u,test_result,n);
                              return 1;
                        }
                        else {
                              putchar('.');
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