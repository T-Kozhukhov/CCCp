#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>

#include "person.h"
#include "neighbourList.h"
#include "boundaryManager.h"
#include "force.h"
#include "planarTorque.h"
#include "output.h"

#include "threadManager.h"

class System
{
    public:
        System(physParam param);
        virtual ~System();

        void runSimulation(); // begins the simulation according to sysParam

    private:
        //system parameters
        physParam sysParam; // all global parameters particular system

        //Jenny
        RNG randGen; // the random number generator for the system

        //set up subroutines
        void initParticles(); // initialises the particles for the simulation

        std::vector<person> personList; // main list of particles, ie the pList

        //dynamics functions
        void step(int t); // performs a single time step
        void calculateForcesTorques(int i); //calculates all the forces for the person at index i in the list
        void generateDebugParticles(); //generate hardcoded particles in order to debug

        //auxiliary functions
        void mergePList(std::vector<person> newList); //merges newList into the main personList
        void mergePList(std::vector<person> *parentList, std::vector<person> newList); //merges newList into parentList

        //helper objects/ managers
        boundaryManager bManager;
        force fManager;
        planarTorque tManager;
        output vtpDumper;

        //thread handler
        threadManager* thManager;

        //assorted variables
        bool simulationBegun; //whether the simulation has begun or not, used for error catching
};

#endif // SYSTEM_H
