#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<time.h>
#include"defs.h"
#include"mat_func.h"
#include"mat_flat_func.h"

void recompute_row(double* pivot, double* row, unsigned int pivot_index, unsigned int size);
void map_pivots_to_rows(double* pivots, double* rows, unsigned int pivot_index,
    unsigned int size, unsigned int num_of_rows, unsigned int num_of_pivots);
void self_recompute(double* rows, unsigned int index, unsigned int size, unsigned int num_of_rows);

void recompute_row(double* pivot, double* row, unsigned int pivot_index, unsigned int size)
{
  if(fabs(pivot[pivot_index]) > MY_EPSILON){
    row[pivot_index] = row[pivot_index] / pivot[pivot_index];
    for(unsigned int i = pivot_index+1; i < size; ++i){
      row[i] = row[i] - row[pivot_index] * pivot[i];
    }
  }
}

void map_pivots_to_rows(double* pivots, double* rows, unsigned int pivot_index,
    unsigned int size, unsigned int num_of_rows, unsigned int num_of_pivots)
{
  fprintf(stderr,"map_to_pivot: %d %d\n", pivot_index, num_of_rows);
  for(unsigned int p = 0; p < num_of_pivots; ++p){
    for(unsigned int r = 0; r < num_of_rows; ++r){
      recompute_row(pivots+p*size,rows+r*size,pivot_index+p,size);
    }
  }
}

void self_recompute(double* rows, unsigned int index, unsigned int size, unsigned int num_of_rows)
{
  fprintf(stderr,"self_recompute: %d %d\n", index, num_of_rows);
  for(unsigned int p = 0; p < num_of_rows-1; ++p){
    for(unsigned int i = p+1; i < num_of_rows; ++i){
      map_pivots_to_rows(rows+p*size,rows+i*size,index,size,num_of_rows-p,1);
    }
  }
}

int main(int argc, char* argv[]) {
  int dims = 3;
  if(argc >= 2){
    dims = atoi(argv[1]);
  }
  MPI_Init(&argc, &argv);
  int num_of_processes = 0;
  int my_rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &num_of_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  unsigned int* rows_per_process = (unsigned int*) calloc(num_of_processes,sizeof(unsigned int));
  unsigned int target_rows_per_process = (unsigned int) ceil(dims/(double)num_of_processes);
  int left_out_rows = dims%target_rows_per_process;
  if(left_out_rows > 0){
    rows_per_process[0] = left_out_rows;
  } else {
    rows_per_process[0] = target_rows_per_process;
  }
  int assigned = rows_per_process[0];
  for(int p = 1; p < num_of_processes; ++p){
    if(assigned != dims){
      rows_per_process[num_of_processes-p] = target_rows_per_process;
      assigned += target_rows_per_process;
    }
  }
  if(my_rank == 0) {
    srand(time(NULL));
    unsigned int* shifts = (unsigned int*) calloc(num_of_processes,sizeof(unsigned int));
    unsigned int shift_sum = 0;
    for(unsigned int i = num_of_processes-1; i != 0; --i){
      shifts[i] = shift_sum;
      shift_sum += rows_per_process[i]*dims;
    }
    shifts[0] = shift_sum;
    double* matrix = mat_flat_generate(dims);
    double** square = get_square_view(matrix,dims);
    fprintf(stderr,"%d %d\n", target_rows_per_process, left_out_rows);
    for(int i = 0; i < num_of_processes; ++i)
      fprintf(stderr,"%d %d %d\n", i, rows_per_process[i], shifts[i]);
    mat_print(square,dims,"Source:");
    for(unsigned int i = num_of_processes-1; i > 0; --i){
      if(rows_per_process[i] == 0){
        continue;
      }
      MPI_Send(matrix+shifts[i],rows_per_process[i]*dims,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
    }
    unsigned int row_index = 0;
    for(unsigned int i = num_of_processes-1; i > 0; --i){
      if(rows_per_process[i] == 0){
        continue;
      }
      fprintf(stderr,"Waiting fro %d, %d\n", i, my_rank);
      MPI_Recv(matrix+shifts[i],rows_per_process[i]*dims,MPI_DOUBLE,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      fprintf(stderr,"Got from %d, %d\n", i, my_rank);
      map_pivots_to_rows(matrix+shifts[0],matrix+shifts[0],row_index,dims,rows_per_process[i],rows_per_process[0]);
      row_index += rows_per_process[i];
    }
    fprintf(stderr,"recomputing rows %d\n", my_rank);
    self_recompute(matrix+shifts[0],row_index,dims,rows_per_process[0]);
    fprintf(stderr,"My rows recomputed %d\n", my_rank);
    mat_print(square,dims,"LU matrix:");
    free(shifts);
    free(matrix);
    free(square);
  } else {
    if(rows_per_process[my_rank] != 0){
      double* rows = (double*) malloc(dims*rows_per_process[my_rank]*sizeof(double));
      MPI_Recv(rows,rows_per_process[my_rank]*dims,MPI_DOUBLE,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      fprintf(stderr,"Recieved %d from %d\n", my_rank, 0);
      unsigned int row_index = 0;
      for(int p = num_of_processes-1; p > my_rank; --p){
        if(rows_per_process[p] == 0){
          continue;
        }
        double* pivots = (double*) malloc(dims*rows_per_process[p]*sizeof(double));
        fprintf(stderr,"%d waiting for %d\n", my_rank, p);
        MPI_Recv(pivots,rows_per_process[p]*dims,MPI_DOUBLE,p,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        fprintf(stderr,"Got from %d, %d\n", p, my_rank);
        map_pivots_to_rows(pivots,rows,row_index,dims,rows_per_process[p],rows_per_process[my_rank]);
        row_index += rows_per_process[p];
        free(pivots);
      }
      self_recompute(rows,row_index,dims,rows_per_process[my_rank]);
      fprintf(stderr,"My rows recomputed %d\n", my_rank);
      for(int p = my_rank-1; p >= 0; --p){
        if(rows_per_process[p] == 0){
          continue;
        }
        fprintf(stderr,"Sending to %d, %d\n", p, my_rank);
        MPI_Send(rows,rows_per_process[my_rank]*dims,MPI_DOUBLE,p,0,MPI_COMM_WORLD);
        fprintf(stderr,"Sent to %d, %d\n", p, my_rank);
      }
      free(rows);
    }
  }
  MPI_Finalize();
  free(rows_per_process);
  return 0;
}
