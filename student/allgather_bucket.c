#include <assert.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int GT_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
    assert(sendtype == MPI_INT && recvtype == MPI_INT);

    /* Your code here (Do not just call MPI_Allgather) */

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Copy own bucket into correct slot
    memcpy((int *)recvbuf + rank * recvcount, sendbuf, sendcount * sizeof(int));

    int *bucket = (int *)malloc(recvcount * sizeof(int));

    // Track which bucket we're sending
    int current_index = rank;

    for (int step = 1; step < size; step++) {
        int send_to = (rank + 1) % size;
        int recv_from = (rank - 1 + size) % size;

        // Send the bucket we received in the previous step
        MPI_Sendrecv((int *)recvbuf + current_index * recvcount, recvcount, sendtype, send_to, 0,
                     bucket, recvcount, recvtype, recv_from, 0,
                     comm, MPI_STATUS_IGNORE);

        // Determine where the received bucket originated
        int recv_index = (rank - step + size) % size;
        memcpy((int *)recvbuf + recv_index * recvcount, bucket, recvcount * sizeof(int));

        // Update current_index to the newly received bucket
        current_index = recv_index;
    }

    free(bucket);
    return MPI_SUCCESS;
}
