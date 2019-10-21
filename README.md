# CCCmt
A multithreaded version of CCC.

## TODO for Tim
- Investigate how to change threadManager such that no thread is ever re-initialised and check performance benefits
- Edit CMakeLists to allow user to input number of cores 
- Make further statistics on performance information. Make a batch script to gather them. 
- Write the usage guide in readme
- Write the main changes in readme

## Performance Guide
Unfortunately due to the way that the particle simulation is run performance doesn't scale inverse to the number of cores used. This is due to how each thread needs to be re-initialised twice per time step. Further investigation should be done on how to avoid re-initialising threads - Perhaps having threads never properly stop, instead forcing them through a FSM, may be effective?

Some statistics are below. N are the number of particles, C is the number of threads used, t is time taken, phi is the packing fraction. The below are run using a Ryzen 3700X 8 core hyper-threaded CPU running the TESTCASE sim with N modified only.
- N=250, phi~=0.49, normal CCC t=18s; C=1 t=26s, C=2 22s, C=4 26s, C=8 1m16s. 
- N=500, phi~=0.98, normal CCC t=43s; C=1 t=56s, C=2 40s, C=4 39s, C=8 1m22s. 
- N=5000, phi~=0.25, normal CCC t=1m46; C=16 t=45s. 

While these are by no means solid statistics, they do show that scaling with the number of cores is absolutely pathetic - Without using a modern enthusiast-level CPU you won't have any significant benefits. As a rough guide they imply that you should use the original version of CCC for N<500. For N>500 using CCCmt seems more effective, and that using roughly N/250 threads is efficient.

## Usage Guide
TODO

## Main Changes in relation to CCC
TODO
