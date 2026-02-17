# Week 3: Intro to MPI

**hello_world_mpi:**
- input number of processors
- output "hello I am x of y"
- random order everytime
- will compile but not run without MPI_Init
- without MPI_Finalize code will hang and possibly abort

**hello_mpi_serial**
- added error checking
- separate function for checking input
- uses a single loop

As hello_world_mpi is parallel it will be faster than a for loop

 **Vector_parallel:**
- takes input: mpirun -np **4** ./vector_parallel **10**
- takes vector **(10)**size as input and returns i x (i+1)
- so it splits the task to **4** processes with each do part of the sum then each partial sum is combined at the end
- also prints the CPU time

 **vector_serial**
- takes similar input as above 
- is much slower than the parallel version with large datasets
- also goes negtive with values above 1800??
