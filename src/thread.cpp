#include "thread.h"
#include <iostream>

thread::thread(){
	//dummy ctor
}

thread::thread(std::vector<person>* pList, int Start, int Last, physParam SysParam, long int seed){
	sysParam = SysParam; //store a copy of the sysParam variables here
	start = Start;
	last = Last;
	
	//setup person list
	for(int i = 0; i < pList->size(); i++){ //loop through list and make a pointer to each individual person
		personList.push_back(&pList->at(i));
	}
	
	//set up jenny
	randGen = new RNG(seed);
	
	//handling boundary stuff here
    bManager = new boundaryManager(sysParam.periodic, sysParam.L_x, sysParam.L_y);

    //preparing fManager and tManager by getting the necessary parameters from sysParam
    fManager = new force(bManager, sysParam.zetaActive, sysParam.zetaGround, sysParam.zetaPerson,
        sysParam.v_0, sysParam.kHarmonic, sysParam.kHertzian);
    tManager = new planarTorque(sysParam.xiAngular, sysParam.xiPair, sysParam.zetaPolar,
        sysParam.zetaVelocity);
		
	//prepare status variables
	status_mutex = new std::mutex(); //dynamically create mutexs for FSM vars
	waiting_mutex = new std::mutex();
	switchThreadStatus(WAITING); //wait for instructions by default
}

thread::~thread(){
	//dtor - empty to avoid segfaults - pointers are killed upon instruction by parent threadManager class
}

void thread::killPointers(){
	//kill pointers
	delete bManager;
	delete fManager;
	delete tManager;
	delete randGen;
	
	//kill mutexs
	delete status_mutex;
	delete waiting_mutex;
}

void thread::beginThread(){
	//this is the "main" function for this particular thread
	while(true){ 
		int currStatus = getThreadStatus(); //get status only through function to ensure mutex is used appropriately
	
		if(currStatus==COMPUTING){ //pivot based off earlier init'd var to avoid overuse of mutex'd get function
			for(int i = start; i < last; i++){
				calculateForcesTorques(i); //calculate the forces and torques on person i
			}
			switchThreadStatus(WAITING); //switch back to waiting status
		} else if (currStatus==UPDATING){
			for(int i = start; i < last; i++){
				update(i); // update person i
			}
			switchThreadStatus(WAITING); //switch back to waiting status
		} else if (currStatus==SHUTDOWN){
			return; //leave the function if we're told to shutdown, allowing parent std::thread object to be joinable
		}
	}
}

bool thread::getWaitStatus(){ //mutex'd wait get
	std::lock_guard<std::mutex> guard(*waiting_mutex); 
	return waiting;
}

void thread::switchThreadStatus(int Status){
	std::lock_guard<std::mutex> guard(*status_mutex); //set mutex now so other threads can't use status
	status = Status;
	
	if(Status == WAITING){ //if we're being told to wait, switch the wait bool appropriately using the appropriate mutex
		std::lock_guard<std::mutex> guard(*waiting_mutex);
		waiting = true;
	} else {
		std::lock_guard<std::mutex> guard(*waiting_mutex);
		waiting = false;
	}
}

int thread::getThreadStatus(){
	std::lock_guard<std::mutex> guard(*status_mutex);
	return status; //returns the thread's status
}

void thread::calculateForcesTorques(int i){ //nearly identical forces/ torques function as would normally be in the System class
	//get p_i for readability here
    person *p_i = personList.at(i);

    if(!p_i->getGlued()){ //only perform action if pointed particle is't glued
        //obtain the neighbourlist for this particle
        std::vector<int> nList = p_i->getCurrNList();

        p_i->setForce(mathVector()); //reset force on particle to a 0 vector
        p_i->setTorque(0); //reset torque on particle to 0
        p_i->setNoisyForce(mathVector()); //reset noise force
        p_i->setNoisyTorque(0); //reset noise torque

        for(unsigned int j = 0; j<nList.size(); j++){ //for each of the neighbours...
            if(nList[j]!=i){ //only run the following if the particle isn't using itself as a neighbour
                //for readability, get p_j
                person *p_j = personList.at(nList[j]); // p_j is actually the person at the j'th entry in the nList, not the j'th entry in the main pList!!

                //paired forces, only calculate and add if sysParam specified to do so
                if(sysParam.enableHarmonicInterForce) { p_i->addForce(fManager->computeHarmonicInterForce(p_i, p_j)); }
                if(sysParam.enableHertzianInterForce) { p_i->addForce(fManager->computeHertzianInterForce(p_i, p_j)); }
                if(sysParam.enablePersonFrictionForce) { p_i->addForce(fManager->computePersonFrictionForce(p_i, p_j)); }

                //paired torques, only calculate and add if sysParam specified to do so
                if(sysParam.enablePolarAlignmentTorque) { p_i->addTorque(tManager->computePolarAlignmentTorque(p_i, p_j)); }
                if(sysParam.enablePairDissipationTorque) { p_i->addTorque(tManager->computePairDissipationTorque(p_i, p_j)); }
            }
        }

        //single forces, only calculate and add if sysParam specified to do so
        if(sysParam.enableActiveForce) { p_i->addForce(fManager->computeActiveForce(p_i)); }
        if(sysParam.enableGroundFrictionForce) { p_i->addForce(fManager->computeGroundFrictionForce(p_i)); }
        if(sysParam.enableRandNoisyForce) { p_i->setNoisyForce(fManager->randNoisyForce(randGen, sysParam.sigmaForceX, sysParam.sigmaForceY)); }

        //single torques, only calculate and add if sysParam specified to do so
        if(sysParam.enableVelocityAlignmentTorque) { p_i->addTorque(tManager->computeVelocityAlignmentTorque(p_i)); }
        if(sysParam.enableAngularFrictionTorque) { p_i->addTorque(tManager->computeAngularFrictionTorque(p_i)); }
        if(sysParam.enableRandNoisyTorque) { p_i->setNoisyTorque(tManager->randNoisyTorque(randGen, sysParam.sigmaTorque)); }
    }
}

void thread::update(int i){ //function to update non-glued particles
	if(!personList.at(i)->getGlued()) {
        personList.at(i)->update(sysParam.stepSize, bManager); //if not glued, update each particle
    }
}
