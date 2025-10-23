#include <assert.h>
#include <mpi.h>

int GT_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
  assert(datatype == MPI_INT);
  assert(root == 0);
  
  /* Your code here (Do not just call MPI_Bcast) */
  
  return MPI_SUCCESS;
}
