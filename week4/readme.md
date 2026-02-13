Topic 4 

COMM_MPI
running gives more outputs than expected and in a random order, i.e. outputs are in different orders for the same input.

MPI_Rsend can hang
MPI_Bsend needs a buffer to be included
MPI_Isend means MPI_Wait must be used order differences
MPI_Ssend slower but always in order

Recommend
Ssend, predictable output and no buffers required

