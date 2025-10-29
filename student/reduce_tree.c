#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

int GT_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype,
           MPI_Op op, int root, MPI_Comm comm)
{
    assert(datatype == MPI_INT);
    assert(op == MPI_SUM);
    assert(root == 0);

    /* Your code here (Do not just call MPI_Reduce) */
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int *tempbuf = (int *)malloc(count * sizeof(int));
    int *localbuf = (int *)malloc(count * sizeof(int));
    for (int i = 0; i < count; i++) {
        localbuf[i] = ((int *)sendbuf)[i];
    }

    int mask = 1;
    while (mask < size) {
        int partner = rank ^ mask;
        MPI_Request req;
        if (rank & mask) {
            //Non-Blocking Send
            MPI_Isend(localbuf, count, datatype, partner, 0, comm, &req);
            MPI_Wait(&req, MPI_STATUS_IGNORE);
            break;
        } else {
            //Non-Blocking Receive
            MPI_Irecv(tempbuf, count, datatype, partner, 0, comm, &req);
            MPI_Wait(&req, MPI_STATUS_IGNORE);
            for (int i = 0; i < count; i++) {
                localbuf[i] += tempbuf[i];
            }
        }
        mask <<= 1;
    }

    if (rank == root) {
        for (int i = 0; i < count; i++) {
            ((int *)recvbuf)[i] = localbuf[i];
        }
    }

    free(tempbuf);
    free(localbuf);

    return MPI_SUCCESS;
}

