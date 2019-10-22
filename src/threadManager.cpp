#include "threadManager.h"
#include <cmath>
#include <iostream>
#include <ctime>

threadManager::threadManager(std::vector<person>* PersonList, physParam* SysParam)
{
    //ctor
    std::vector<int> partitions = partitionWorkload(PersonList->size()); //create partitions for each workload

    //setup tList
    for(unsigned int i = 0; i < coreCount; i++){
        tList.push_back(std::thread());
    }
	
	//now setup containers
	long int currTimeSeed = static_cast<int>(time(NULL)); //get seed for the jenny's
	for(unsigned int i = 0; i < tList.size(); i++){ //perform actual setup
		containerList.push_back(thread(PersonList, partitions[i], partitions[i+1], *SysParam, currTimeSeed+i));
	}
	
	//finally get the threads to begin their main loops
	for(int i = 0; i < tList.size(); i++){
		tList[i] = std::thread(&thread::beginThread, &containerList[i]);
	}
	std::cout << "Running using " << coreCount << " cores!\n";
}

threadManager::~threadManager()
{
    //dtor
	//need to tell each thread it's time to stop
	for(int i = 0; i < tList.size(); i++){
		containerList[i].switchThreadStatus(thread::SHUTDOWN); //switch all threads to the shutdown status
	}
}

void threadManager::performStep(){
    for(int i = 0; i < tList.size(); i++){
		containerList[i].switchThreadStatus(thread::COMPUTING); //switch all threads to the computing status
	}
	
	waitForThreads();
	
	for(int i = 0; i < tList.size(); i++){
		containerList[i].switchThreadStatus(thread::UPDATING); //switch all threads to the updating status
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

void threadManager::waitForThreads(){
	//needs to check to make sure that all threads are in the waiting status
	
    for(unsigned int i = 0; i < tList.size(); i++){
		while(containerList[i].getThreadStatus()!=thread::WAITING);
    }
}