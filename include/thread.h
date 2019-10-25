#ifndef THREAD_H
#define THREAD_H

#include <vector>
#include <mutex>

#include "person.h"
#include "physParam.h"
#include "boundaryManager.h"
#include "force.h"
#include "planarTorque.h"
#include "RNG.h"

class thread
{
    public:
        thread();
		thread(std::vector<person>* pList, int Start, int Last, physParam SysParam, long int seed);
        ~thread();
		void killPointers(); //sub to kill pointers used in the thread

		void beginThread(); //starts the thread's actions
		
		// public members for handling the FSM
		bool getWaitStatus(); //can only find if thread is waiting publically!
		void switchThreadStatus(int Status);
		enum enumStatus {WAITING, COMPUTING, UPDATING, SHUTDOWN};

    private:
		//resources which may be used by an individual thread
        physParam sysParam;
        boundaryManager* bManager;
        force* fManager;
        planarTorque* tManager;
        RNG* randGen;
		
		//FSM status vars
		int status; //describes the state of the internal FSM
		std::mutex* status_mutex; //mutex for the status variable
		int getThreadStatus(); //private only method to get status
		bool waiting; //whether or not the thread is waiting
		std::mutex* waiting_mutex; //mutex for waiting bool
		
		//person list and partitioning variables
		std::vector<person*> personList; //vector of pointers to all people in the simulation
		int start; //start point of this thread's partition
		int last; //the end of this thread's partition
		
		//workload functions
		void calculateForcesTorques(int i); //calculates force and torques for person i
        void update(int i); //uses buffer to calculate actual values for index i
};

#endif // THREAD_H
