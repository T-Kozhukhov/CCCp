#include "threadManager.h"
#include <cmath>
#include <iostream>

threadManager::threadManager(std::vector<person>* PersonList, physParam* SysParam, boundaryManager* BManager, force* FManager, planarTorque* TManager, RNG* RandGen)
{
    //ctor
    //setup matrices
    personList = PersonList;
    sysParam = SysParam;
    bManager = BManager;
    tManager = TManager;
    randGen = RandGen;

    partitionWorkload(personList->size()); //create partitions for each workload

    //setup tList
    for(unsigned int i = 0; i < coreCount; i++){
        tList.push_back(std::thread());
    }
	
	std::cout << "Running using " << coreCount << " cores!\n";
}

threadManager::~threadManager()
{
    //dtor
}

void threadManager::performStep(){
    allocateWorkload(&threadManager::calculateForcesTorques); //perform calculation step onto buffer

    allocateWorkload(&threadManager::update); //perform update step onto value matrix

}

void threadManager::partitionWorkload(int n){
    //needs to be equal number of partitions as threads, ie partitions.size()==coreCount+1 at the end
    int partSize = std::floor((int)n/(double)coreCount); //get rough size of each partition, last partition will be largest

    //now perform actual partitioning
    partitions.push_back(0); //first partition point will be 0
    for(unsigned int i = partitions.size(); i < coreCount; i++){
        partitions.push_back(partSize*i); //mark out partition points at correct spots
    }
    partitions.push_back(n); //final partition point will be the size of the vector
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

void threadManager::calculateForcesTorques(int i){
    //get p_i for readability here
    person *p_i = &personList->at(i);

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
                person *p_j = &personList->at(nList[j]); // p_j is actually the person at the j'th entry in the nList, not the j'th entry in the main pList!!

                //paired forces, only calculate and add if sysParam specified to do so
                if(sysParam->enableHarmonicInterForce) { p_i->addForce(fManager->computeHarmonicInterForce(p_i, p_j)); }
                if(sysParam->enableHertzianInterForce) { p_i->addForce(fManager->computeHertzianInterForce(p_i, p_j)); }
                if(sysParam->enablePersonFrictionForce) { p_i->addForce(fManager->computePersonFrictionForce(p_i, p_j)); }

                //paired torques, only calculate and add if sysParam specified to do so
                if(sysParam->enablePolarAlignmentTorque) { p_i->addTorque(tManager->computePolarAlignmentTorque(p_i, p_j)); }
                if(sysParam->enablePairDissipationTorque) { p_i->addTorque(tManager->computePairDissipationTorque(p_i, p_j)); }
            }
        }

        //single forces, only calculate and add if sysParam specified to do so
        if(sysParam->enableActiveForce) { p_i->addForce(fManager->computeActiveForce(p_i)); }
        if(sysParam->enableGroundFrictionForce) { p_i->addForce(fManager->computeGroundFrictionForce(p_i)); }
        if(sysParam->enableRandNoisyForce) { p_i->setNoisyForce(fManager->randNoisyForce(randGen, sysParam->sigmaForceX, sysParam->sigmaForceY)); }

        //single torques, only calculate and add if sysParam specified to do so
        if(sysParam->enableVelocityAlignmentTorque) { p_i->addTorque(tManager->computeVelocityAlignmentTorque(p_i)); }
        if(sysParam->enableAngularFrictionTorque) { p_i->addTorque(tManager->computeAngularFrictionTorque(p_i)); }
        if(sysParam->enableRandNoisyTorque) { p_i->setNoisyTorque(tManager->randNoisyTorque(randGen, sysParam->sigmaTorque)); }
    }
}

void threadManager::update(int i){
    if(!personList->at(i).getGlued()) {
        personList->at(i).update(sysParam->stepSize, bManager); //if not glued, update each particle
    }
}
