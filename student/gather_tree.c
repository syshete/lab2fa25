#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int GT_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
              void *recvbuf, int recvcount, MPI_Datatype recvtype,
              int root, MPI_Comm comm)
{
    assert(sendtype == MPI_INT && recvtype == MPI_INT);
    assert(root == 0);
	
	/* Your code here (Do not just call MPI_Gather) */

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MPI_Request req;
    if (rank == root) {
        // Root copies its own data into recvbuf
        memcpy((int *)recvbuf + rank * recvcount, sendbuf, sendcount * sizeof(int));
        // Receive from all other ranks
        for (int i = 0; i < size; i++) {
            if(i != root){
                MPI_Irecv((int *)recvbuf + i * recvcount, recvcount, recvtype, i, 0, comm, &req);
                MPI_Wait(&req, MPI_STATUS_IGNORE);
            }
        }
    } else {
        // Non-root sends its data to root
        MPI_Isend(sendbuf, sendcount, sendtype, root, 0, comm, &req);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
