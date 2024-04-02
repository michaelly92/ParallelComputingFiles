#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define num_tasks 100
#define request_tag 0
#define task_tag 1

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int empty = -1 , full = 1, task_processed=0;
    int task, buffer = empty, busy = 0, request = 0;
    MPI_Request req[2];
    MPI_Status status[2];
//Master
    if (rank == 0) {
        for (int i = 0; i < num_tasks; i++) {
            MPI_Send(&request, 1, MPI_INT, 1, request_tag, MPI_COMM_WORLD);
            MPI_Send(&i, 1, MPI_INT, 1, task_tag, MPI_COMM_WORLD);

        }
        MPI_Send(&request, 1, MPI_INT, 1, request_tag, MPI_COMM_WORLD);
        MPI_Send(&empty, 1, MPI_INT, 1, task_tag, MPI_COMM_WORLD);
    } else {
//Slave
        while (1) {
            MPI_Irecv(&buffer, 1, MPI_INT, (rank - 1 + size) % size, task_tag, MPI_COMM_WORLD, &req[0]);
            MPI_Irecv(&request, 1, MPI_INT, (rank + 1) % size, request_tag, MPI_COMM_WORLD, &req[1]);
//Nonblocking communication
            MPI_Waitall(2, req, status);

            if (buffer == empty) {
                MPI_Send(&request, 1, MPI_INT, (rank - 1 + size) % size, request_tag, MPI_COMM_WORLD);
            }

            if (buffer == full && !busy) {
                task = buffer;
                buffer = empty;
                busy = 1;

            }

            MPI_Send(&request, 1, MPI_INT, (rank + 1) % size, request_tag, MPI_COMM_WORLD);

            if (busy) {
                sleep(1);
                busy = 0;
                task_processed++;
            }
//leave loop when buffer is empty
            if (buffer == empty) {
                break;
            }
        }
    }
    printf("Process %d has processed %d tasks\n", rank, task_processed);

    MPI_Finalize();
    return 0;
}
