#include "thread.h"

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
}

thread::~thread(){
	//kill pointers
	delete bManager;
	delete fManager;
	delete tManager;
	delete randGen;
}

void thread::doCalculateWorkload(){
	for(int i = start; i < last; i++){
		calculateForcesTorques(i); //calculate the forces and torques on person i
	}
}

void thread::doUpdateWorkload(){
	for(int i = start; i < last; i++){
		update(i); //calculate the forces and torques on person i
	}
}

void thread::calculateForcesTorques(int i){
	//get p_i for readability here
    person *p_i = personList.at(i);

    if(!p_i->getGlued()){
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

void thread::update(int i){
	if(!personList.at(i)->getGlued()) {
        personList.at(i)->update(sysParam.stepSize, bManager); //if not glued, update each particle
    }
}
