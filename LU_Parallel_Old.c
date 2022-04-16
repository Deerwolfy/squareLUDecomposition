#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include"mat_func.h"

/* TODO:
 * Refactor code
 */

int main(int argc, char* argv[]) {
  int n = 3;
  if (argc == 2) {
    n = atoi(argv[1]);
  }
  MPI_Init(&argc, &argv);
  int p;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double* local_row = (double*)malloc(n * sizeof(double));
  double* remote_row = (double*)malloc(n * sizeof(double));
  if (local_row == NULL || remote_row == NULL) {
    return 1;
  }
  int rows_per_process = (int) ceil(n / ((double)p-1));
  if (rank == 0) {
    srand(time(NULL));
    double** matrix = mat_generate(n);
    mat_print(matrix, n,"Source matrix:");
    putchar('\n');
    /* Assign rows to processes */
    for (int i = 1; i < p; ++i) {
      /* Current row index */
      int row_shift = rows_per_process * (i-1);
      int available_rows = n - row_shift;
      /* Send number of rows based on number of available rows */
      if (available_rows < rows_per_process) {
        MPI_Send(&available_rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
      else {
        MPI_Send(&rows_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      }
      /* Send rows */
      for (int j = 0; j < rows_per_process && j < available_rows; ++j) {
        MPI_Send(matrix[row_shift+j], n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
      }
    }
    /* Wait for results and merge them */
    int expected_rows = 0;
    for (int i = 1; i < p; ++i) {
      /* Recieve number of transfered rows */
      MPI_Recv(&expected_rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      /* Recieve rows and compose result matrix */
      for (int j = 0; j < expected_rows; ++j) {
        MPI_Recv(remote_row, n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /* Compute current row shift */
        int row_shift = rows_per_process * (i - 1);
        /* Swap pointers between matrix and buffer */
        double* temp = matrix[row_shift + j];
        matrix[row_shift + j] = remote_row;
        remote_row = temp;
      }
    }
    double** L = mat_square_alloc(n);
    double** U = mat_square_alloc(n);
    mat_LU_split(matrix,&L,&U,n);
    mat_print(L, n, "L matrix:");
    putchar('\n');
    mat_print(U, n, "U matrix:");
    putchar('\n');
    double** mult = mat_mult(L,U,n);
    mat_print(mult, n, "Multiplication result:");
    mat_free(matrix, n);
    mat_free(L, n);
    mat_free(U, n);
    mat_free(mult, n);
  }
  else {
    /* Get number of rows from main process */
    int expected_rows = 0;
    MPI_Recv(&expected_rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    double** partial_mat = (double**)malloc(expected_rows*sizeof(double*));
    /* Get rows from main process */
    for (int i = 0; i < expected_rows; ++i) {
      MPI_Recv(local_row, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      partial_mat[i] = local_row;
      local_row = (double*)malloc(n * sizeof(double));
    }
    int local_row_start = (rank - 1) * rows_per_process;
    /* Start recieving updated rows from other processes */
    for (int k = 1; k < rank; ++k) {
      double** remote_mat = (double**)malloc(rows_per_process * sizeof(double*));
      int remote_expected_rows = 0;
      MPI_Recv(&remote_expected_rows, 1, MPI_INT, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      for (int i = 0; i < remote_expected_rows; ++i) {
        MPI_Recv(local_row, n, MPI_DOUBLE, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        remote_mat[i] = local_row;
        local_row = (double*)malloc(n * sizeof(double));
      }
      /* Recompute own rows */
      /* Find number of recivied row in global matrix */
      int remote_row_start = (k-1) * rows_per_process;
      for (int i = 0; i < rows_per_process; ++i) {
        int remote_row_pos = remote_row_start + i;
        for (int j = 0; j < expected_rows; ++j) {
          partial_mat[j][remote_row_pos] = partial_mat[j][remote_row_pos] / remote_mat[i][remote_row_pos];
          for (int h = remote_row_pos + 1; h < n; ++h) {
            partial_mat[j][h] = partial_mat[j][h] - partial_mat[j][remote_row_pos] * remote_mat[i][h];
          }
        }
      }
      mat_free(remote_mat, rows_per_process);
    }
    /* Update own rows */
    for (int i = 0; i < expected_rows; ++i) {
      int local_row_pos = local_row_start + i;
      for (int j = i+1; j < expected_rows; ++j) {
        partial_mat[j][local_row_pos] = partial_mat[j][local_row_pos] / partial_mat[i][local_row_pos];
        for (int h = local_row_pos + 1; h < n; ++h) {
          partial_mat[j][h] = partial_mat[j][h] - partial_mat[j][local_row_pos] * partial_mat[i][h];
        }
      }
    }
    /* Send own computed rows to other processes */
    for (int i = rank + 1; i < p; ++i) {
      MPI_Send(&expected_rows, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      for (int j = 0; j < expected_rows; ++j) {
        MPI_Send(partial_mat[j], n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
      }
    }
    /* Send to main process */
    MPI_Send(&expected_rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    for (int j = 0; j < expected_rows; ++j) {
      MPI_Send(partial_mat[j], n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    mat_free(partial_mat,expected_rows);
  }
  MPI_Finalize();
  free(local_row);
  free(remote_row);
  return 0;
}
