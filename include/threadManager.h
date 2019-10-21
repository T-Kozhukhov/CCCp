#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <vector>
#include <thread>

#include "person.h"
#include "physParam.h"
#include "boundaryManager.h"
#include "force.h"
#include "planarTorque.h"
#include "RNG.h"

class threadManager
{
    public:
        threadManager(std::vector<person>* PersonList, physParam* SysParam, boundaryManager* BManager, force* FManager, planarTorque* TManager, RNG* RandGen);
        ~threadManager();

        void performStep(); //performs all the multithreaded actions required in a single time step

    private:
        //tManager specific members ///////////////////////
        const unsigned int coreCount = 8;//CORE_COUNT; //gets number of threads to use via preprocessor variable
        std::vector<std::thread> tList; //the main list of threads.

        //workload partitioning members
        void partitionWorkload(int n); //partitions the n "work items" into the correct number of threads
        std::vector<int> partitions; //contains the partition values used when allocating a workload

        void allocateWorkload(void (threadManager::*f)(int)); //splits the input function between the threads, each focussing on their own partition
        void performWorkload(void (threadManager::*f)(int), unsigned int start, unsigned int last); // performs the input function on the target starting at index start, ending just before index last

        void waitForThreads(); //waits for all threads

        //job specific members ////////////////////////////
        std::vector<person>* personList;
        physParam* sysParam;
        boundaryManager* bManager;
        force* fManager;
        planarTorque* tManager;
        RNG* randGen;

        void calculateForcesTorques(int i); //calculates force and torques for person i
        void update(int i); //uses buffer to calculate actual values for index i
};

#endif // THREADMANAGER_H
