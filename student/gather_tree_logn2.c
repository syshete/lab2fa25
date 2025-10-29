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

    int *tmpbuf = malloc(size * sendcount * sizeof(int));
    memcpy(tmpbuf + rank * sendcount, sendbuf, sendcount * sizeof(int));

    int step;
    for (step = 1; step < size; step <<= 1) {
        int partner;

        if ((rank / step) % 2 == 0) {
            // Parent: receive data from partner
            partner = rank + step;
            if (partner < size) {
                int count = ((partner + step) < size ? step : (size - partner)) * sendcount;
                MPI_Recv(tmpbuf + partner * sendcount, count, MPI_INT, partner, 0, comm, MPI_STATUS_IGNORE);
            }
        } else {
            // Child: send data to partner and stop
            partner = rank - step;
            int count = ((rank + step) < size ? step : (size - rank)) * sendcount;
            MPI_Send(tmpbuf + rank * sendcount, count, MPI_INT, partner, 0, comm);
            break;
        }
    }

    if (rank == root) {
        memcpy(recvbuf, tmpbuf, size * sendcount * sizeof(int));
    }

    free(tmpbuf);
    return MPI_SUCCESS;
}
