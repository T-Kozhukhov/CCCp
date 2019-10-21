#include <iostream>

#include "System.h"
#include "csv.h"

System::System(physParam param)
{
    //ctor
    sysParam = param;
    simulationBegun = false;
    personList = std::vector<person>(0);

    person::setMassRadiusRatio(sysParam.massRadiusRatio); //set the mass-radius ratio of particles now before any are generated

    //handling boundary stuff here
    bManager = boundaryManager(sysParam.periodic, sysParam.L_x, sysParam.L_y);

    //preparing fManager and tManager by getting the necessary parameters from sysParam
    fManager = force(&bManager, sysParam.zetaActive, sysParam.zetaGround, sysParam.zetaPerson,
        sysParam.v_0, sysParam.kHarmonic, sysParam.kHertzian);
    tManager = planarTorque(sysParam.xiAngular, sysParam.xiPair, sysParam.zetaPolar,
        sysParam.zetaVelocity);

    //setup of output manager
    vtpDumper = output(sysParam.outFileName, sysParam.outDirPath, sysParam.outFileType, &personList);

    //preparing thManager
    thManager = new threadManager(&personList, &sysParam);
}

System::~System()
{
    //dtor
    //kill off all pointers that need to exist for the entire system duration
    for(unsigned int i = 0; i< personList.size(); i++){
        personList.at(i).killPointers(); //kill off person pointers
    }
	
	delete thManager;
}

void System::runSimulation(){
    std::cout<< "Preparing simulation\n";

    initParticles(); //initialise particles
    csv::exportPList(personList, sysParam.pathToLoadingCSV+"initPartData.csv", sysParam.meanR); //exports the IC data for potential later usage

    std::cout<< "Executing simulation\n";
    simulationBegun = true; //set the simulation as begun to catch errors

    for(int t = 0; t<sysParam.stepMax; t++){ // perform appropriate number of steps
        step(t);
    }

    cout<< "\n\nSimulation completed. Output at: " << vtpDumper.getPathInfo() << '\n';
}

void System::initParticles(){
    if(sysParam.debugType!=physParam::None){ //If we are in a debug state, then override accordingly
        generateDebugParticles();
    } else if(sysParam.loadParticles){ //otherwise, if we are to load particles
        std::vector<person> tempList = csv::importPList(sysParam.pathToParticles); //load particles into a temp vector
        mergePList(tempList); // merge the temporary list into the main lsit
    } else { //if no loading has been specified, just generate random particles within the domain
        std::vector<person> tempList; //create a temp list

        for(int i = 0; i<sysParam.N; i++){ //create N random particles as follows:
            if(bManager.getPeriodic()){ //if we're using periodic boundary conditions, then...
                tempList.push_back(person::getRandPerson(false, i, sysParam.meanR, sysParam.sigmaR, sysParam.v_0, sysParam.sigmaV, &randGen, &bManager)); //make a normal person
            }else{ //if we're not using a periodic boundary, thus using a fixed "particle wall" around the domain...
                //in this case, we need to stop particle deployments directly on the wall, so...
                mathVector pos = bManager.getRandPos(&randGen, sysParam.meanR*2); //get a position within the domain that is further than 2 average radius' from the boundary
                person tempPerson = person::getRandPerson(false, i, sysParam.meanR, sysParam.sigmaR, sysParam.v_0, sysParam.sigmaV, &randGen, pos);
                tempList.push_back(tempPerson); //create an otherwise random particle at that position
            }
        }

        mergePList(tempList);// merge the temp list into the main pList
    }
}

void System::step(int t){
    //We do all dumping before we do maths.
    //check to see if we should output on this step or not
    if (t%sysParam.outputSteps == 0){
        vtpDumper.dump(t);
    }

    //check to see if we should dump particle data
    if(sysParam.particleDumpSteps!=0){ //if we're supposed to dump particle data....
        if(t%sysParam.particleDumpSteps == 0){ //and it's the correct step to dump particle data
            if(sysParam.dumpSingleParticle){
                csv::dumpSingleParticleData(personList, t*sysParam.stepSize, 0); //dump a single particle if specified
            } else { //otherwise dump all particles
                //make file path
                std::stringstream ss;
                ss << sysParam.pathToParticleData << "ParticleData" << t <<".csv";

                csv::dumpParticleData(personList, ss.str(), t*sysParam.stepSize); //dump all particles to file path
            }
        }
    }

    //Now do mathematics related stuff
    //first, check to see if neighbour lists need to be updated
    if(neighbourList::checkUpdate(&personList, &bManager)){
        neighbourList::updateLists(&personList, &bManager); //update the neighbour lists if necessary
    }

    //loop through all non-glued particles...
//    for(unsigned int i = 0; i < personList.size(); i++){
//        if(!personList[i].getGlued()){
//            calculateForcesTorques(i); //calculate the forces acting on each non-glued particle
//        }
//    }
//
//    // perform dynamics for all non-glued particles particles
//    for(unsigned int i = 0; i<personList.size(); i++){
//        if(!personList[i].getGlued()) {
//            personList[i].update(sysParam.stepSize, &bManager); //if not glued, update each particle
//        }
//    }

    thManager->performStep();
}

void System::mergePList(std::vector<person> newList){
    mergePList(&personList, newList);
}

void System::mergePList(std::vector<person> *parentList, std::vector<person> newList){
    if(simulationBegun){ //if the simulation has already begun, throw some output and escape
        std::cout << "Error: The program attempted to merge the pList after the simulation had already started.\n"
                  << "Do all merging before starting the simulation 'step loop' begins. Merging was skipped.\n";
        return;
    }

    //otherwise, if the simulation hasn't begun move the list
    for(unsigned int i =0; i<newList.size();i++){
        parentList->push_back(newList.at(i));
    }

    //now loop through pList and update IDs for particles
    for(unsigned int i = 0; i < parentList->size(); i++){
        parentList->at(i).setID(i);
    }

    neighbourList::updateLists(&personList, &bManager); //update the neighbour lists to make the program ready for use
}

/* void System::calculateForcesTorques(int i){ //calculate the force and torques acting on particle i here
    //get p_i for readability here
    person *p_i = &personList[i];

    //obtain the neighbourlist for this particle
    std::vector<int> nList = p_i->getCurrNList();

    p_i->setForce(mathVector()); //reset force on particle to a 0 vector
    p_i->setTorque(0); //reset torque on particle to 0
    p_i->setNoisyForce(mathVector()); //reset noise force
    p_i->setNoisyTorque(0); //reset noise torque

    for(unsigned int j = 0; j<nList.size(); j++){ //for each of the neighbours...
        if(nList[j]!=i){ //only run the following if the particle isn't using itself as a neighbour
            //for readability, get p_j
            person *p_j = &personList[nList[j]]; // p_j is actually the person at the j'th entry in the nList, not the j'th entry in the main pList!!

            //paired forces, only calculate and add if sysParam specified to do so
            if(sysParam.enableHarmonicInterForce) { p_i->addForce(fManager.computeHarmonicInterForce(p_i, p_j)); }
            if(sysParam.enableHertzianInterForce) { p_i->addForce(fManager.computeHertzianInterForce(p_i, p_j)); }
            if(sysParam.enablePersonFrictionForce) { p_i->addForce(fManager.computePersonFrictionForce(p_i, p_j)); }

            //paired torques, only calculate and add if sysParam specified to do so
            if(sysParam.enablePolarAlignmentTorque) { p_i->addTorque(tManager.computePolarAlignmentTorque(p_i, p_j)); }
            if(sysParam.enablePairDissipationTorque) { p_i->addTorque(tManager.computePairDissipationTorque(p_i, p_j)); }
        }
    }

    //single forces, only calculate and add if sysParam specified to do so
    if(sysParam.enableActiveForce) { p_i->addForce(fManager.computeActiveForce(p_i)); }
    if(sysParam.enableGroundFrictionForce) { p_i->addForce(fManager.computeGroundFrictionForce(p_i)); }
    if(sysParam.enableRandNoisyForce) { p_i->setNoisyForce(fManager.randNoisyForce(&randGen, sysParam.sigmaForceX, sysParam.sigmaForceY)); }

    //single torques, only calculate and add if sysParam specified to do so
    if(sysParam.enableVelocityAlignmentTorque) { p_i->addTorque(tManager.computeVelocityAlignmentTorque(p_i)); }
    if(sysParam.enableAngularFrictionTorque) { p_i->addTorque(tManager.computeAngularFrictionTorque(p_i)); }
    if(sysParam.enableRandNoisyTorque) { p_i->setNoisyTorque(tManager.randNoisyTorque(&randGen, sysParam.sigmaTorque)); }
} */

void System::generateDebugParticles(){
    //This subroutine is used to hardcode particles to debug the code
    std::vector<person> debugParticles; //this is the list we're handling in this subroutine

    if(sysParam.debugType==physParam::SingleAlongXAxis){
        //SingleAlongXAxis = Deploy a single particle going at const velocity along the x axis
        mathVector pos = bManager.getScaledPeriodicPos(0.5, 0.5);
        mathVector vel = mathVector(sysParam.v_0, 0);
        double polAngle = 0; //makes the particle go in the +ve x direction
        debugParticles.push_back(person(false, 0, sysParam.meanR, sysParam.meanR, pos, vel, polAngle));
    }
    else if (sysParam.debugType==physParam::SingleAlongRandAxis){
        //SingleAlongRandAxis = Deploy a single particle going at const velocity along a random axis
        mathVector pos = bManager.getScaledPeriodicPos(0.5, 0.5); //puts the particle directly in the center
        double polAngle = randGen.getUniformDouble(0, 6.28318530); //generate a random number between 0 and 2*PI
        mathVector vel = mathVector::getPolarVector(sysParam.v_0, polAngle); //generate a random velocity vector by using the previous magnitude and angle
        debugParticles.push_back(person(false, 0, sysParam.meanR, sysParam.meanR, pos, vel, polAngle));
    }
    else if (sysParam.debugType==physParam::DoubleCenterCollision){
        //DoubleCenterCollision = Deploy two identical particles, each travelling towards each other parallel to the x axis aiming for a collision
        //particle 1, going in +ve x direction
        mathVector pos1 = bManager.getScaledPeriodicPos(0.25, 0.5);
        mathVector vel1 = mathVector(sysParam.v_0, 0);
        double polAngle1 = 0;
        debugParticles.push_back(person(false, 0, sysParam.meanR, sysParam.meanR, pos1, vel1, polAngle1));

        //particle 2, going in -ve x direction
        mathVector pos2 = bManager.getScaledPeriodicPos(0.75, 0.5);
        mathVector vel2 = mathVector(-sysParam.v_0, 0);
        double polAngle2 =  3.141592;
        debugParticles.push_back(person(false, 1, sysParam.meanR, sysParam.meanR, pos2, vel2, polAngle2));
    }
    else if (sysParam.debugType==physParam::DoubleEdgeCollision){
        //DoubleEdgeCollision = Deploy two identical particles, each travelling towards each other such they collide on the boundary
        //particle 1, going in -ve x direction
        mathVector pos1 = bManager.getScaledPeriodicPos(0.25, 0.5);
        mathVector vel1 = mathVector(-sysParam.v_0, 0);
        double polAngle1 = 3.141592;
        debugParticles.push_back(person(false, 0, sysParam.meanR, sysParam.meanR, pos1, vel1, polAngle1));

        //particle 2, going in +ve x direction
        mathVector pos2 = bManager.getScaledPeriodicPos(0.75, 0.5);
        mathVector vel2 = mathVector(sysParam.v_0, 0);
        double polAngle2 =  0;
        debugParticles.push_back(person(false, 1, sysParam.meanR, sysParam.meanR, pos2, vel2, polAngle2));
    }
    else if (sysParam.debugType==physParam::Corridor){
        //Corridor = Deploy N particles within a horizontal corridor of length L_x. Intended to be used with periodic BCs
        const double widthScale = 0.4; //how much of L_y, as a scalar, should be used as the width of the corridor

        //get position related information
        double corridorMid = bManager.getScaledPos(0.5,0.5).getY(); //middle of corridor
        double dY = bManager.getScaledPos(0,0.5*widthScale).getY(); //difference between corridor walls and midpoint
        double corridorLength = bManager.getScaledPos(1, 0).getX(); //getting the length of the corridor
        double corridorTop = corridorMid+dY;
        double corridorBot = corridorMid-dY;

        //generate the "walls"
        std::vector<person> topWall = bManager.placeHorizontalBoundaryParticles(sysParam.meanR, sysParam.overlapRatio, corridorTop, 0, corridorLength);
        std::vector<person> botWall = bManager.placeHorizontalBoundaryParticles(sysParam.meanR, sysParam.overlapRatio, corridorBot, 0, corridorLength);

        //merge the walls into our debugParticles list
        mergePList(&debugParticles, topWall);
        mergePList(&debugParticles, botWall);

        //now generate N particles within that domain, but away from the "walls"
        const double wallDist = sysParam.meanR*2; //how far away from the walls you have to be
        for(int i = 0; i<sysParam.N; i++){ //create N random particles as follows:
            mathVector pos = bManager.getRandPos(&randGen, 0, corridorLength, corridorBot+wallDist, corridorTop-wallDist); //generate a random position within the corridor but away from walls
            person tempPerson = person::getRandPerson(false, i, sysParam.meanR, sysParam.sigmaR, sysParam.v_0, sysParam.sigmaV, &randGen, pos);
            debugParticles.push_back(tempPerson); //create an otherwise random particle at that position
        }
    }
    else if (sysParam.debugType==physParam::Box){
        //Box = Deploy N particles within a square box along the boundaries.
        //get box height and length
        double height = bManager.getScaledPos(1,1).getY();
        double width = bManager.getScaledPos(1,1).getX();

        //generate the walls
        std::vector<person> topWall = bManager.placeHorizontalBoundaryParticles(sysParam.meanR, sysParam.overlapRatio, height, 0, width);
        std::vector<person> botWall = bManager.placeHorizontalBoundaryParticles(sysParam.meanR, sysParam.overlapRatio, 0, 0, width);
        std::vector<person> leftWall = bManager.placeVerticalBoundaryParticles(sysParam.meanR, sysParam.overlapRatio, 0, 0, height);
        std::vector<person> rightWall = bManager.placeVerticalBoundaryParticles(sysParam.meanR, sysParam.overlapRatio, width, 0, height);

        //merge the walls into the debug list
        mergePList(&debugParticles, topWall);
        mergePList(&debugParticles, botWall);
        mergePList(&debugParticles, leftWall);
        mergePList(&debugParticles, rightWall);

        //now generate N particles within that domain but away from the "walls"
        const double wallDist = sysParam.meanR*2; ///how far away from the walls you have to be, change as appopriate
        for(int i = 0; i<sysParam.N; i++){ //create N random particles as follows:
            mathVector pos = bManager.getRandPos(&randGen, wallDist, width-wallDist, wallDist, height-wallDist); //generate a random position within the box but away from walls
            person tempPerson = person::getRandPerson(false, i, sysParam.meanR, sysParam.sigmaR, sysParam.v_0, sysParam.sigmaV, &randGen, pos);
            debugParticles.push_back(tempPerson); //create an otherwise random particle at that position
        }
    }
    else if (sysParam.debugType==physParam::Cluster){
        //Cluster = Delpoy 4 particles close to each other within a cluster, and a singular particle below them outside the cluster, all going in the same direction

        //get a vector pointing to center and a particle average radius
        mathVector center = bManager.getScaledPos(0.5,0.5);
        double radius = sysParam.meanR;

        //generate positions based on the center and radius
        //cluster positions are 3 radii above the center in a square block s.t. they touch each other perfectly
        mathVector pos1 = mathVector(center.getX(), center.getY()+3*radius);
        mathVector pos2 = mathVector(center.getX()+2*radius, center.getY()+3*radius);
        mathVector pos3 = mathVector(center.getX()+2*radius, center.getY()+5*radius);
        mathVector pos4 = mathVector(center.getX(), center.getY()+5*radius);
        //position for singleton particle
        mathVector pos5 = mathVector(center.getX(), center.getY()-3*radius);

        //now create the 5 particles with appropriate position, all going at speed v_0 in the +ve direction
        mathVector vel = mathVector(sysParam.v_0, 0); //make a velocity vector as appropriate
        debugParticles.push_back(person(false, 0, radius, radius, pos1, vel, 0));
        debugParticles.push_back(person(false, 1, radius, radius, pos2, vel, 0));
        debugParticles.push_back(person(false, 2, radius, radius, pos3, vel, 0));
        debugParticles.push_back(person(false, 3, radius, radius, pos4, vel, 0));
        debugParticles.push_back(person(false, 4, radius, radius, pos5, vel, 0));
    }

    mergePList(debugParticles); //merge debug particles into main pList
}
