#ifndef THREAD_H
#define THREAD_H

#include <vector>

#include "person.h"
#include "physParam.h"
#include "boundaryManager.h"
#include "force.h"
#include "planarTorque.h"
#include "RNG.h"

/* 
	Thread class to contain all resources in use by a single thread via the threadManager class
*/

class thread
{
    public:
        thread();
		thread(std::vector<person>* pList, int Start, int Last, physParam SysParam, long int seed);
        ~thread();

		void doCalculateWorkload(); //does the calculation workload on this particular thread's work
		void doUpdateWorkload(); //does the update workload on this particular thread's work

    private:
		//resources which may be used by a thread
        physParam sysParam;
        boundaryManager* bManager;
        force* fManager;
        planarTorque* tManager;
        RNG* randGen;
		
		std::vector<person*> personList; //vector of pointers to all people in the simulation
		int start; //start point of this thread's partition
		int last; //the end of this thread's partition
		
		void calculateForcesTorques(int i); //calculates force and torques for person i
        void update(int i); //uses buffer to calculate actual values for index i
};

#endif // THREAD_H
