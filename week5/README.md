# **Week 5**

**string_wave.c**

- modified to remove hardcode values.

- Now takes input in the form "file_name, points on string, cycles, samples per cycle, output_file"

**animation_file_line.py**

- removed hardcoding.

- updated argument checks.

- output location also specified in command line.

**string_wave_parallel.c**

- the string is split into "equal" chunks, and each given to an MPI process.

- each rank communicates with neighbours. gives first and last points.

- rank updates postion for all the times

- MPIGather gets all chunks at rank 0 to recombine string

- only rank 0 writes to memory, avoiding any errors
