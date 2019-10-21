#pragma once

#include "vector.h"
#include "RNG.h"
#include "person.h"
#include "boundaryManager.h"

class force
{
    public:
        force();
        force(boundaryManager *BManager, double ZetaActive, double ZetaGround,
            double ZetaPerson, double V_0, double KHarmonic, double KHertzian);
        virtual ~force();

        mathVector computeHarmonicInterForce(person* p_i, person* p_j); //computes a harmonic spring interactional force ON p_i
        mathVector computeHertzianInterForce(person* p_i, person* p_j); //computes a hertzian interactional force ON p_i

        mathVector computeActiveForce(person* p_i); //computes the active force on p_i

        mathVector computeGroundFrictionForce(person* p_i);//computes friction with the ground ON p_i
        mathVector computePersonFrictionForce(person* p_i, person* p_j);//computes friction with neighbouring person ON p_i

        mathVector randNoisyForce(RNG *sysRNG, double sigma_x, double sigma_y); // generate a noisy, (gauss) random force with mean vector (0,0)

    private:
        //pointer to the bManager for this system
        boundaryManager *bManager;

        //parameters
        double zetaActive; // friction parameter for active propulsion
        double zetaGround; // friction parameter against the ground
        double zetaPerson; // friction parameter against other people

        double v_0; // active velocity parameter

        double kHarmonic; // harmonic spring const between particles
        double kHertzian; // hertzian const between particles
};

