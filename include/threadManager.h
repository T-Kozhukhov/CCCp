#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <vector>
#include <thread>

#include "thread.h"
#include "physParam.h"

class threadManager
{
    public:
        threadManager(std::vector<person>* PersonList, physParam* SysParam);
        ~threadManager();

        void performStep(); //performs all the multithreaded actions required in a single time step

    private:
        //tManager specific members ///////////////////////
        const unsigned int coreCount = 10;//CORE_COUNT; //gets number of threads to use via preprocessor variable
        std::vector<std::thread> tList; //the main list of threads.
		std::vector<thread> containerList; //the list of containers for each thread

        std::vector<int> partitionWorkload(int n); //partitions the n "work items" into the correct number of threads and returns the partition values

        void waitForThreads(); //waits for all threads
};

#endif // THREADMANAGER_H
