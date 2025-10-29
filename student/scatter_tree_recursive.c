#include <assert.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void scatter_tree(int *buf, int count, int recvcount,
                  int rank, int size, int base_rank, int *recvbuf, 
                  MPI_Comm comm, MPI_Datatype sendtype, 
                  MPI_Datatype recvtype)
{
    int num_chunks = count / recvcount;
    if (num_chunks == 1) {
        memcpy(recvbuf, buf, recvcount * sizeof(int));
        return;
    }

    int mid_chunks = num_chunks / 2;
    int left_chunks = mid_chunks;
    int right_chunks = num_chunks - mid_chunks;

    int left_count = left_chunks * recvcount;
    int right_count = right_chunks * recvcount;

    int left_rank = base_rank;
    int right_rank = base_rank + left_chunks;

    // If I'm the sender of the right half
    if (rank == left_rank && right_rank < size) {
        MPI_Request send_req;
        MPI_Isend(buf + left_count, right_count, sendtype, right_rank, 0, comm, &send_req);
        MPI_Wait(&send_req, MPI_STATUS_IGNORE);
    }

    // If I'm the receiver of the right half
    int *subbuf = buf;
    if (rank >= right_rank && rank < base_rank + num_chunks) {
        if (rank == right_rank) {
            subbuf = (int *)malloc(right_count * sizeof(int));
            MPI_Request recv_req;
            MPI_Irecv(subbuf, right_count, recvtype, left_rank, 0, comm, &recv_req);
            MPI_Wait(&recv_req, MPI_STATUS_IGNORE);
        } else {
            // Other ranks in right subtree do nothing until recursion
            subbuf = NULL;
        }
    }

    // Recurse into left subtree
    if (rank >= left_rank && rank < right_rank) {
        scatter_tree(buf, left_count, recvcount, rank, size, left_rank, recvbuf, comm, sendtype, recvtype);
    }
    // Recurse into right subtree
    else if (rank >= right_rank && rank < base_rank + num_chunks) {
        scatter_tree(subbuf, right_count, recvcount, rank, size, right_rank, recvbuf, comm, sendtype, recvtype);
        if (rank == right_rank) {
            free(subbuf);
        }
    }
}



int GT_Scatter(void *sendbuf, int sendcount, MPI_Datatype sendtype,
               void *recvbuf, int recvcount, MPI_Datatype recvtype,
               int root, MPI_Comm comm)
{
    assert(sendtype == MPI_INT && recvtype == MPI_INT);
    assert(root == 0);

    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    int total_count = sendcount * size;
    int *buffer = NULL;

    if (rank == root) {
        buffer = (int *)sendbuf;
    }

    scatter_tree(buffer, total_count, recvcount, rank, size, 0, (int *)recvbuf, comm, sendtype, recvtype);

    return MPI_SUCCESS;
}
