# Week 2 Performance and Parallelism
# How to run 
C scripts: gcc time_write.c -O2 -o bin/time_write  

Python scripts:  python3 time_write.py 
both followed by numbers of operations to be timed i.e 100000


#Conclusions
C is much faster at reading to from disk to memory than python.  
It is also faster at writing to the terminal.  
The slowwest operation is printing to the terminal with the time linearly dependent on integer size for both C and Python however with python the time increases more rapidly
