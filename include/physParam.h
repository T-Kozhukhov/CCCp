#ifndef PHYSPARAM_INCLUDED
#define PHYSPARAM_INCLUDED

#include <string>

///Welcome to spaghetti land
struct physParam{
    //step properties
    double stepSize; // step size in seconds
    int stepMax; // how many steps the simulation should run for

    //output information
    int outputSteps; // after how many steps should you output?
    std::string outDirPath; //path to vtp output directory
    std::string outFileName; //what file to store data into. .vtp extension should NOT be included
    std::string outFileType; //file tpye to save as (should be VTP, no dots)

    //particle properties
    int N; //if randomly generating particles, this is how many to generate
    double meanR; // mean radius
    double sigmaR; // standard deviation of the radius
    double sigmaV; // standard deviation for when generating gaussian initial velocities
    double massRadiusRatio; //ratio of particle mass to radius used for finding mass of particles

    //boundary consts
    bool periodic; // is the boundary periodic or no?
    double L_x;
    double L_y;
    double overlapRatio; //overlap ratio for when "drawing" soft particle walls

    //csv information
    bool loadParticles; // whether to load particles or not
    std::string pathToParticles; //full file path to csv containing particle information
    std::string pathToLoadingCSV; //file path to the directory where debug particles are stored
    int particleDumpSteps; //after how many steps to dump particle data. 0 for no dumping
    std::string pathToParticleData; //file path to the directory where particle data should be stored
    bool dumpSingleParticle; //whether to dump a single particle or not

    //force bools - Used to enable or disable forces within the simulation
    bool enableHarmonicInterForce;
    bool enableHertzianInterForce;
    bool enableActiveForce;
    bool enableGroundFrictionForce;
    bool enablePersonFrictionForce;
    bool enableRandNoisyForce;

    //force parameters
    double zetaActive; // friction parameter for active propulsion
    double zetaGround; // friction parameter against the ground
    double zetaPerson; // friction parameter against other people
    double v_0; // active velocity parameter
    double kHarmonic; // harmonic spring const between particles
    double kHertzian; // hertzian const between particles
    double sigmaForceX; // standard deviation for the force resulted from noise in the x axis
    double sigmaForceY; // standard deviation for the force resulted from noise in the y axis

    //torque bools - Used to enable or disable torques within the simulation
    bool enablePolarAlignmentTorque;
    bool enableVelocityAlignmentTorque;
    bool enableAngularFrictionTorque;
    bool enablePairDissipationTorque;
    bool enableRandNoisyTorque;

    //torque parameters
    double xiAngular; //angular friction coeff
    double xiPair; //pair dissipation coeff
    double zetaPolar; //polar alignment coeff
    double zetaVelocity; //velocity alignment coeff
    double sigmaTorque; //SD for the torque resulted from noise

    //debug parameters
    int debugType; //what type of debugging is occurring. Use one of the below enums only.
    enum debugTypes {None, SingleAlongXAxis, SingleAlongRandAxis, DoubleCenterCollision, DoubleEdgeCollision, Corridor, Box, Cluster}; //see below descriptions of the types
    /*
        ==Note==    All types except 'None' will override particle loading and random particle generation for the system and instead deploy the included test case.

        None =                  No debug, aka normal simulation behaviour
                                    Used for running the simulation normally
        SingleAlongXAxis =      Deploy a single particle going at const velocity along the x axis
                                    Useful for testing output and basic BC behaviour
        SingleAlongRandAxis =   Deploy a single particle going at const velocity along a random axis
                                    Useful for testing output and more complex BC behaviour
        DoubleCenterCollision = Deploy two identical particles, each travelling towards each other parallel to the x axis aiming for a collision
                                    Useful for testing contact forces and neighbour lists
        DoubleEdgeCollision =   Deploy two identical particles, each travelling towards each other such they collide on the boundary
                                    Useful for resting contact forces and neighbour lists subject to periodic BCs
        Corridor =              Deploy N particles within a horizontal corridor of length L_x. Intended to be used with periodic BCs
        Box =                   Deploy N particles within a square box along the boundaries.
        Cluster =               Deploy 4 particles close to each other within a cluster, and a singular particle below them outside the cluster, all going in the same direction
                                    Use to see if velocity alignment and person friction works as intended.
    */
};

#endif // PHYSPARAM_INCLUDED
