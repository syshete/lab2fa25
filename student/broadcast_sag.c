#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int GT_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    assert(datatype == MPI_INT);
    assert(root == 0);

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int chunk_size = count / size;
    int *chunk = (int *)malloc(chunk_size * sizeof(int));

    // Scatter phase
    if (rank == root) {
        memcpy(chunk, (int *)buffer + rank * chunk_size, chunk_size * sizeof(int));
        for (int i = 0; i < size; i++) {
            if(i != root) {
                MPI_Send((int *)buffer + i * chunk_size, chunk_size, datatype, i, 0, comm);
            }
        }
    } else {
        MPI_Recv(chunk, chunk_size, datatype, root, 0, comm, MPI_STATUS_IGNORE);
    }

    // Allgather phase
    for (int i = 0; i < size; i++) {
        if (i == rank) {
            memcpy((int *)buffer + i * chunk_size, chunk, chunk_size * sizeof(int));
        } else {
            MPI_Sendrecv(chunk, chunk_size, datatype, i, 1,
                        (int *)buffer + i * chunk_size, chunk_size, datatype, i, 1,
                        comm, MPI_STATUS_IGNORE);
        }
    }   


    free(chunk);
    return MPI_SUCCESS;
}
