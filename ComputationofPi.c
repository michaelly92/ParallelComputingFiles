#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
double actual_pi = 3.141592653589793238462643;
//for comparison later
int n, rank, proc, i;
double temp_pi, calc_pi, int_size, part_sum, x;
MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &proc);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
n = 10;
MPI_Bcast(&n, 1, MPI_INT, 0,MPI_COMM_WORLD); //broadcast n
  int_size = 1.0 / (double)n; //calcs interval size
  part_sum = 0.0;
  for (i = rank + 1; i <= n; i += proc)
    { //partial sums
      x = int_size * (((double)i) - 0.5);
      part_sum = part_sum + (4.0 / (1.0 + (x*x)));
    }
   temp_pi = int_size * part_sum;
  //compute pi
  MPI_Reduce(&temp_pi, &calc_pi, 1, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);
  double result = calc_pi - actual_pi;
  if (rank == 0) { // Main process
    printf("pi is approximately %.4lf . Error is %.4lf\n " , calc_pi , fabs(result));
  }
MPI_Finalize();
return 0;
}                