# CCCmt
A multithreaded version of CCC.

## TODO for Tim
- Investigate how to change threadManager such that no thread is ever re-initialised and check performance benefits
- Edit CMakeLists to allow user to input number of cores 
- Write the usage guide in readme
- Write the main changes in readme

## Performance Guide
Unfortunately due to the way that the particle simulation is run performance doesn't scale inverse to the number of cores used. As a very rough guide, only use this version if N>~500 and aim to have no more than N/250 cores.
Some statistics are below. N are the number of particles, C is the number of threads used, t is time taken, phi is the packing fraction. The below are run using a Ryzen 3700X 8 core hyper-threaded CPU running the TESTCASE sim with N modified only.
- N=250, normal CCC t=18s; C=1 => t=26s, C=2 => 22s, C=4 => 26s, C=8 => 1m16s. phi~=0.49
- N=500, normal CCC t=43s; C=1 => t=56s, C=2 => 40s, C=4 => 39s, C=8 => 1m22s. phi~=0.98
- N=5000, normal CCC t=1m46; C=16 => t=45s. phi~=0.25

## Usage Guide
TODO

## Main Changes in relation to CCC
TODO