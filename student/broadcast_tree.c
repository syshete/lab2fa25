#include <assert.h>
#include <mpi.h>

#include <mpi.h>

int GT_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {

    assert(datatype == MPI_INT);
	assert(root == 0);

    /* Your code here (Do not just call MPI_Bcast) */
	
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Convert to relative ranks so root is 0
    int relative_rank = (rank - root + size) % size;

    int parent = (relative_rank == 0) ? -1 : (relative_rank - 1) / 2;
    if (relative_rank != 0) {
        int source = (parent + root) % size;
        MPI_Recv(buffer, count, datatype, source, 0, comm, MPI_STATUS_IGNORE);
    }

    // Send to children
    int left_child = 2 * relative_rank + 1;
    int right_child = 2 * relative_rank + 2;

    if (left_child < size) {
        int dest = (left_child + root) % size;
        MPI_Send(buffer, count, datatype, dest, 0, comm);
    }
    if (right_child < size) {
        int dest = (right_child + root) % size;
        MPI_Send(buffer, count, datatype, dest, 0, comm);
    }

    return MPI_SUCCESS;
}

