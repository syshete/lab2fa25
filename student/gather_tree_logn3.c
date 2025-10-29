#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int GT_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                   void *recvbuf, int recvcount, MPI_Datatype recvtype,
                   int root, MPI_Comm comm)
{
    assert(sendtype == MPI_INT && recvtype == MPI_INT);

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Temporary buffer to hold received data
    int *tmpbuf = malloc(size * sendcount * sizeof(int));
    memcpy(tmpbuf + rank * sendcount, sendbuf, sendcount * sizeof(int));

    // Relative ranks so root is treated as 0
    int relative_rank = (rank - root + size) % size;

    int step = 1;
    while (step < size) {
        if ((relative_rank % (2 * step)) == 0) {
            // Parent: receive data from right child if it exists
            int right_child = relative_rank + step;
            if (right_child < size) {
                int child_rank = (right_child + root) % size;
                int count = ((right_child + step) < size ? step : (size - right_child)) * sendcount;
                MPI_Recv(tmpbuf + right_child * sendcount, count, sendtype, child_rank, 0, comm, MPI_STATUS_IGNORE);
            }
        } else {
            // Child: send accumulated data to parent and stop
            int parent = relative_rank - step;
            int parent_rank = (parent + root) % size;
            int count = ((relative_rank + step) < size ? step : (size - relative_rank)) * sendcount;
            MPI_Send(tmpbuf + relative_rank * sendcount, count, sendtype, parent_rank, 0, comm);
            break;
        }
        step <<= 1;
    }

    if (rank == root) {
        memcpy(recvbuf, tmpbuf, size * sendcount * sizeof(int));
    }

    free(tmpbuf);
    return MPI_SUCCESS;
}
