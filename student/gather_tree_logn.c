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

    int *tempbuf = malloc(size * recvcount * sizeof(int));
    memcpy(tempbuf + rank * recvcount, sendbuf, sendcount * sizeof(int));

    int step = 1;
    while (step < size) {
        if (rank % (2 * step) == 0) {
            int partner = rank + step;
            if (partner < size) {
                MPI_Recv(tempbuf + partner * recvcount, step * recvcount, sendtype,
                         partner, 0, comm, MPI_STATUS_IGNORE);
            }
        } else {
            int partner = rank - step;
            MPI_Send(tempbuf + rank * recvcount, step * recvcount, sendtype,
                     partner, 0, comm);
            break;
        }
        step *=2;
    }

   if (rank == root){
      memcpy(recvbuf, tempbuf, size * recvcount * sizeof(int));
   }
   free(tempbuf);

    return MPI_SUCCESS;
}