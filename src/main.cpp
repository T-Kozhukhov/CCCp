#include <iostream>

#include "System.h"
#include "physParam.h"
#include "csv.h"

using namespace std;

physParam createPhysParam(){ ///A hard-coded helper function to edit PhysParams
    physParam toReturn; //make the physParam that we will return

    //step properties
    toReturn.stepSize = 0.01; // step size in seconds
    toReturn.stepMax = 100000; // how many steps the simulation should run for

    //output information
    toReturn.outputSteps = 1000; // after how many steps should you output?
    toReturn.outDirPath = "./Output/MSD_Calculations/Run5/VTP"; //path to output directory
    toReturn.outFileName = "VisualiserData"; //what file to store data into. .vtp extension should NOT be included
    toReturn.outFileType = "vtp"; //file tpye to save as (should be vtp, no dots)

    //particle properties
    toReturn.N = 300; //if randomly generating particles, this is how many to generate
    toReturn.meanR = 1; // mean radius
    toReturn.v_0 = 0.05; // active velocity parameter
    toReturn.sigmaR = 0.1; // standard deviation of the radius
    toReturn.sigmaV = 0; // standard deviation for when generating gaussian velocities
    toReturn.massRadiusRatio = 1; //ratio of particle mass to radius used for finding mass of particles

    //boundary consts
    toReturn.periodic = true; // is the boundary periodic or no?
    toReturn.L_x = 40;
    toReturn.L_y = 40;
    toReturn.overlapRatio = 0; //overlap ratio if the boundary is not periodic

    //csv information
    toReturn.loadParticles = false; // whether to load particles or not
    toReturn.pathToParticles = ".CSVs/DebugPLists/SingleAlongRandAxis.csv"; //full file path to csv containing particle information
    toReturn.pathToLoadingCSV = "./Output/MSD_Calculations/Run5/"; //file path to where debug particles are stored
    toReturn.particleDumpSteps = 0; //after how many steps to dump particle data. 0 for no dumping
    toReturn.pathToParticleData = "./Output/MSD_Calculations/Run5/ParticleData/"; //file path to the directory where particle data should be stored
    toReturn.dumpSingleParticle = false; //whether to dump a single particle or not

    //force bools - Do we want to consider the following forces in our sim, yay or nay?
    toReturn.enableHarmonicInterForce = true;
    toReturn.enableHertzianInterForce = false;
    toReturn.enableActiveForce = true;
    toReturn.enableGroundFrictionForce = true;
    toReturn.enablePersonFrictionForce = true;
    toReturn.enableRandNoisyForce = false;

    //force parameters
    toReturn.zetaActive = 1; // friction parameter for active propulsion
    toReturn.zetaGround = 1; // friction parameter against the ground
    toReturn.zetaPerson = 1; // friction parameter against other people
    toReturn.kHarmonic = 1; // harmonic spring const between particles
    toReturn.kHertzian = 1; // hertzian const between particles
    toReturn.sigmaForceX = 0.2; // standard deviation for the force resulted from noise in the x axis
    toReturn.sigmaForceY = 0.2; // standard deviation for the force resulted from noise in the y axis

    //torque bools - Do we want to conside the following torques in oru sim, yay or nay?
    toReturn.enablePolarAlignmentTorque = false;
    toReturn.enableVelocityAlignmentTorque = false;
    toReturn.enableAngularFrictionTorque = true;
    toReturn.enablePairDissipationTorque = true;
    toReturn.enableRandNoisyTorque = true;

    //torque parameters
    toReturn.xiAngular = 1; //angular friction coeff
    toReturn.xiPair = 1; //pair dissipation coeff
    toReturn.zetaPolar = 1; //polar alignment coeff
    toReturn.zetaVelocity = 0.1; //velocity alignment coeff
    toReturn.sigmaTorque = 0.2; //SD for the torque resulted from noise

    //debug parameters
    toReturn.debugType = physParam::None; //see physparam header for full list of debug states

    //return the constructed physParam
    return toReturn;
}

//rewrite the input physParam using the standard "experiment" format
physParam rewriteParamForExperiment(physParam input, std::string expDirectory){
    input.outDirPath = expDirectory+"/VTP"; //path to output directory
    input.pathToParticles = expDirectory+"/initPartData.csv"; //full file path to csv containing particle information for loading
    input.pathToLoadingCSV = expDirectory+"/"; //file path to where debug particle data (ICs) are stored
    input.pathToParticleData = expDirectory+"/ParticleData/"; //file path to the directory where particle data should be stored
    return input;
}

/*
    Program actions will depend on the number of arguments you give it.
        No arguments - Create a sysParam based on the above hardcoded function
        1 Argument - Load a sysParam from the first argument and run it entirely
        2 Arguments - Load the sysParam specified in the first argument, then rewrite it using the "experiment" format with experiment directory as the second arguments
        3 Arguments - As above, but load particles from the 3rd argument
*/
int main(int argc, char* argv[])
{
    std::cout   << '\n'
                << "==========================\n"
                << "|  Custom Crowds in C++  |\n"
                << "|   13/10/2019 Release   |\n"
                << "==========================\n\n";

    //get starting data depending on number of arguments
    physParam sysParam; // the actual physParam object which we use
    if(argc>1){ //if we have some arguments, load physParam from specified file
        std::string loadingPath(argv[1]); //get loading path from the first argument
        std::cout << "Importing parameters from " << loadingPath << '\n';
        sysParam = csv::importPhysParam(loadingPath);

        if(argc>2){ //if you have two arguments or more
            std::string expDir(argv[2]);
            std::cout << "Forcing experiment format. Experiment directory set to " << expDir << '\n';
            sysParam = rewriteParamForExperiment(sysParam, expDir);

            if(argc>3){ //if you have three arguments or more, force loading particles from specified directory
                std::string particlesDir(argv[3]);
                std::cout << "Forcing loading particles from " << particlesDir << '\n';
                sysParam.loadParticles = true;
                sysParam.pathToParticles = particlesDir;
            }
        }
    } else { //if no parameters, load from hardcoded function
        sysParam = createPhysParam();
    }

    ///Uncomment below to force an experiment through hardcoding main
    //sysParam  = rewriteParamForExperiment(sysParam, "/home/tk16693/Documents/Summer19_Project/ProgramData/Output/temp");
    ///Uncomment below to force loading of system parameter data
    //sysParam = csv::importPhysParam("/home/tk16693/Documents/Summer19_Project/ProgramData/Output/Flocking/350parts/Run1/PhysParamData.csv");

    //set flag to dump particle data of a single particle if necessary
    if(sysParam.dumpSingleParticle){
        csv::setupSingleFile(sysParam.pathToParticleData);
    }

    ///Uncomment below to export the sysParam
    csv::exportPhysParam(sysParam, sysParam.pathToLoadingCSV); //export the sysParam we have at this point for later usage by default

    ///Uncomment below to run a simulation based on sysParam
    System simulation(sysParam); //init the system
    simulation.runSimulation(); //run the system

    return 0;
}
