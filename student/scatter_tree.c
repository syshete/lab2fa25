#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int GT_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype,
               int root, MPI_Comm comm)
{
    assert(sendtype == MPI_INT && recvtype == MPI_INT);
    assert(root == 0);

    /* Your code here (Do not just call MPI_Scatter) */

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MPI_Request req;
    if (rank == root) {
        memcpy(recvbuf, (int *)sendbuf + rank * sendcount, sendcount * sizeof(int));
        for (int i = 0; i < size; i++) {
            if (i !=root){
                MPI_Isend((int *)sendbuf + i * sendcount, sendcount, sendtype, i, 0, comm, &req);
                MPI_Wait(&req, MPI_STATUS_IGNORE);
            }
        }
    } else {
        MPI_Irecv(recvbuf, recvcount, recvtype, root, 0, comm, &req);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
