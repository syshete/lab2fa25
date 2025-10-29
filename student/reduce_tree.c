#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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
    memcpy(localbuf, sendbuf, count * sizeof(int));

    int mask = 1;
    while (mask < size) {
        int partner = rank ^ mask;

        //Handles when # of processes are not powers of 2
        if (partner >= size) {
            mask <<= 1;
            continue;  
        }
        if (rank & mask) {
            //Blocking Send
            MPI_Request send_req;
            MPI_Isend(localbuf, count, datatype, partner, 0, comm, &send_req);
            MPI_Wait(&send_req, MPI_STATUS_IGNORE);
            break;
        } else {
            //Blocking Receive
            MPI_Request recv_req;
            MPI_Irecv(tempbuf, count, datatype, partner, 0, comm, &recv_req);
            MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
            for (int i = 0; i < count; i++) {
                localbuf[i] += tempbuf[i];
            }
        }
        mask <<= 1;
    }

    if (rank == root) {
        memcpy(recvbuf, localbuf, count * sizeof(int));
    }

    free(tempbuf);
    free(localbuf);

    return MPI_SUCCESS;
}

