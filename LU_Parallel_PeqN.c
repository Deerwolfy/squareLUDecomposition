#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include"mat_func.h"

int main(int argc, char *argv[]) {
  int n = 3;
  if (argc == 2) {
    n = atoi(argv[1]);
  }
  MPI_Init(&argc,&argv);
  int p;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&p);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double* local_row = (double*) malloc(n*sizeof(double));
  double* remote_row = (double*)malloc(n * sizeof(double));
  if (local_row == NULL || remote_row == NULL) {
    return 1;
  }
  if (rank == 0) {
    if (n != p) {
      printf("Number of processes shoudl be equal to number of rows");
      return 1;
    }
    srand(time(NULL));
    double** matrix = mat_generate(n);
    mat_print(matrix,n);
    putchar('\n');
    /* Assign rows to processes */
    for (int i = 1; i < p; ++i) {
      MPI_Send(matrix[i],n,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
    }
    /* Send first row to processes */
    for (int i = 1; i < p; ++i) {
      MPI_Send(matrix[0], n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
    }
    /* Wait for results and merge them */
    for (int i = 1; i < p; ++i) {
      MPI_Recv(remote_row, n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      double* temp = matrix[i];
      matrix[i] = remote_row;
      remote_row = temp;
    }
    mat_print(matrix, n);
    putchar('\n');
    mat_free(matrix,n);
  }
  else {
    if (n != p) {
      return 1;
    }
    /* Get row from main process */
    MPI_Recv(local_row,n,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    for (int k = 0; k <= rank; ++k) {
      /* If this process is responsible for row k, send it to other processes and process 0*/
      if (k == rank) {
        for (int i = rank+1; i < p; ++i) {
          MPI_Send(local_row, n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        MPI_Send(local_row, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
      }
      /* If row k belongs to other process recive it and recompute own row */
      else {
        MPI_Recv(remote_row, n, MPI_DOUBLE, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_row[k] = local_row[k] / remote_row[k];
        for (int i = k + 1; i < n; ++i) {
          local_row[i] = local_row[i] - local_row[k] * remote_row[i];
        }
      }
    }
  }
  MPI_Finalize();
  free(local_row);
  free(remote_row);
  return 0;
}
