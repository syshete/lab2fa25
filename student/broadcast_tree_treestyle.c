#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int GT_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    assert(datatype == MPI_INT);

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int mask = 1;
    while (mask < size) {
        int partner = rank ^ mask;

        //Handles when # of processes are not powers of 2
        if (partner >= size) {
            mask <<= 1;
            continue;
        }

        if (rank & mask) {
            MPI_Request recv_req;
            MPI_Irecv(buffer, count, datatype, partner, 0, comm, &recv_req);
            MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
        } else {
            MPI_Request send_req;
            MPI_Isend(buffer, count, datatype, partner, 0, comm, &send_req);
            MPI_Wait(&send_req, MPI_STATUS_IGNORE);
        } 
    
        mask <<= 1;
    }

    return MPI_SUCCESS;
}
