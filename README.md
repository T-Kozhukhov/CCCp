# CCCp
A multithreaded version of CCC. 
Implemented by partitioning the particles between the threads, and having each thread work on their own partition of particles in parallel.

The code uses the main thread (that which executes the main() function) to manage a user adjustable number of compute threads which perform the actual workloads. 
Currently, the main thread still performs neighbourList calculations and IO related operations while the compute threads will calculate forces, torques and update particle data.

## TODO for Tim
- Clean up code ready for deployment
- Edit CMakeLists to allow user to input number of cores either while 
- Write the usage guide in readme
- Write the main changes in readme

## Performance Guide
Multiple runs of CCCp for relatively normal particle numbers (N<1000) imply the following:
- Running CCCp using only one compute thread (2 total threads) is a waste of performance - While it does sometimes match the performance of CCC it may sometimes be a bit slower.
- While performances isn't inversely proportional to the number of threads used, it does improve performance significantly. For example on my test machine:
	- 2 compute threads is around 30% faster than CCC
	- 4 compute threads is around 40% faster than CCC
	- 8 compute threads is around 50% faster than CCC
- The performance improvement over CCC seems to stop at a hard limit depending on your CPU. For me this was around 8-10 compute threads.
- The performance improvement over CCC scales very well with the packing fraction of the running simulation.
- If your CPU has T total threads then ensure that you use no more than T-1 compute threads or the program won't present any useful output.
- If you will be using your computer while running a simulation with CCCp, ensure that you leave enough threads free to do other tasks. As a rough idea, aim to have no more than 90% CPU utilisation while running CCCp and your normal background tasks. I found that using 15 compute threads resulted not only in slower simulations but also a near unusable computer.

The exact statistics behind the above are available on request. 
These statistics were computed by using a modified PhysParam file based off that supplied in the TESTCASE, with only 100000 total time steps, and the number of particles N modified. 
Furthermore, the domain was modified to roughly match the packing fractions while remaining square.
Tests were run for N=250, 500, 750, 1000 for each packing fraction 0.3, 0.5, 0.8 three times in total, and the execution time between the three runs were averaged.
Each run was performed on normal CCC, and then CCCp for 1, 2, 4, 8, 10, and 12 compute threads.
These tests were run on a machine using a stock Ryzen 3700X 8-core CPU with 16 threads in total. Other tasks were being done on the machine during testing.

Some further testing on lower thread count CPUs or higher particle counts should be done to fully understand the performance of CCCp over CCC.

## Usage Guide
TODO

As mentioned in the introduction, do note that if you setup CCCp with N compute threads, you will in fact use N+1 total threads taking into account the main thread.

## Main Changes in relation to CCC
TODO
