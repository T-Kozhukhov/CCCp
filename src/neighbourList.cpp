#include "neighbourList.h"
#include <iostream>

//init of static members
double neighbourList::r_m = 3; //default values, assuming average radius is 1
double neighbourList::r_c = 2.4;
std::vector<mathVector> neighbourList::posList;

neighbourList::neighbourList(){
    //dummy ctor
}

neighbourList::neighbourList(int ParentID, float avRadius)
{
    //ctor
    parentID = ParentID;
    neighbourList::r_m = 3*avRadius;
    neighbourList::r_c = 2.4*avRadius;

    nList = std::vector<int>(0); //init nList to size 0
}

neighbourList::~neighbourList()
{
    //dtor
}

bool neighbourList::checkUpdate(std::vector<person> *pList, boundaryManager *bManager){
    //go through full pList and check against positions

    double maxDisp = 0;
    double maxDisp2 = 0; // current highest and second highest displacement
    // get two highest displacements
    for(unsigned int i = 0; i<pList->size(); i++){
        //get current displacement through the bManager
        double currDisp = bManager->getPeriodicDist(pList->at(i).getPosition(), neighbourList::posList.at(i));
        if(currDisp > maxDisp){
            maxDisp2 = maxDisp;
            maxDisp = currDisp;
        } else if (currDisp > maxDisp2){
            maxDisp2 = currDisp;
        }
    }

    //performs check to see if we should recalculate
    if ( (maxDisp + maxDisp2) > r_m-r_c){
        return true; // if the above is true we need to recalculate.
    }

    //if you get to this point, all good
    return false; // no need to recalculate the list
}

void neighbourList::updateLists(std::vector<person> *pList, boundaryManager *bManager){
    //update the nLists for each individual particle.
    for(unsigned int i = 0; i<pList->size(); i++){
        pList->at(i).recalculateNList(pList, bManager);
    }

    //update global posList
    posList.clear(); //clear list
    for(unsigned int i = 0; i<pList->size(); i++){
        posList.push_back(pList->at(i).getPosition()); //write positions of particles in order to the posList
    }
}

void neighbourList::calculateList(std::vector<person> *pList, boundaryManager *bManager){
    nList.clear(); //purge current neighbour list
    mathVector parentPos = pList->at(parentID).getPosition(); //get parentPos here for readability

    for (unsigned int i = 0; i<pList->size(); i++){ // cycle through all particles
        //get dist between parent and other particle through bManager
        double dist = bManager->getPeriodicDist(pList->at(i).getPosition(), parentPos);

        if(dist <= r_m){ //if cycled particle is within r_m then...
            nList.push_back(i); //add index to neighbour list
        }
    }
}

void neighbourList::updateParentID(int newID){
    parentID = newID;
}

std::vector<int> neighbourList::getNeighbours(){
    return nList; // then return the most up to date list
}
