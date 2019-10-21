#include "threadManager.h"
#include <cmath>
#include <iostream>
#include <ctime>

threadManager::threadManager(std::vector<person>* PersonList, physParam* SysParam, boundaryManager* BManager, force* FManager, planarTorque* TManager, RNG* RandGen)
{
    //ctor
    std::vector<int> partitions = partitionWorkload(personList->size()); //create partitions for each workload

    //setup tList
    for(unsigned int i = 0; i < coreCount; i++){
        tList.push_back(std::thread());
    }
	
	//now setup containers
	long int currTimeSeed = static_cast<int>(time(NULL)); //get seed for the jenny's
	for(unsigned int i = 0; i < tList.size(); i++){ //perform actual setup
		containerList.push_back(thread(PersonList, partitions[i], partitions[i+1], *SysParam, currTimeSeed+i));
	}
	
	std::cout << "Running using " << coreCount << " cores!\n";
}

threadManager::~threadManager()
{
    //dtor
}

void threadManager::performStep(){
    for(int i = 0; i < tList.size(); i++){
		tList[i] = std::thread(containerList[i].doCalculateWorkload());
	}
	
	waitForThreads();
	
	for(int i = 0; i < tList.size(); i++){
		tList[i] = std::thread(containerList[i].doUpdateWorkload());
	}
	
	waitForThreads();
}

std::vector<int> threadManager::partitionWorkload(int n){
	std::vector<int> toReturn;
	
    //needs to be equal number of partitions as threads, ie partitions.size()==coreCount+1 at the end
    int partSize = std::floor((int)n/(double)coreCount); //get rough size of each partition, last partition will be largest

    //now perform actual partitioning
    toReturn.push_back(0); //first partition point will be 0
    for(unsigned int i = toReturn.size(); i < coreCount; i++){
        toReturn.push_back(partSize*i); //mark out partition points at correct spots
    }
    toReturn.push_back(n); //final partition point will be the size of the vector
	
	return toReturn;
}

void threadManager::allocateWorkload(void (threadManager::*f)(int)){
    //for each thread...
    for(unsigned int i = 0; i < coreCount; i++){
        tList[i] = std::thread(&threadManager::performWorkload, this, f, partitions[i], partitions[i+1]); //split up workload between threads
    }

    //now wait for the workload to be finished
    waitForThreads();
}

void threadManager::performWorkload(void (threadManager::*f)(int), unsigned int start, unsigned int last){
    //this is the workload that occurs on each thread, so do it
    for(unsigned int i = start; i < last; i++){
        (this->*f)(i); //perform function f with parameter i
    }
}

void threadManager::waitForThreads(){
    for(unsigned int i = 0; i < tList.size(); i++){
        tList[i].join(); //wait for each thread to finish
    }
}