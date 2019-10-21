#ifndef PLANARTORQUE_H
#define PLANARTORQUE_H

#include "vector.h"
#include "person.h"

class planarTorque //Gives torque in the plane, ie z component
{
    public:
        planarTorque();
        planarTorque(double XiAngular, double XiPair, double ZetaPolar, double ZetaVelocity);
        virtual ~planarTorque();

        double computePolarAlignmentTorque(person* p_i, person* p_j); //computes torque by forcing alignment with polarity
        double computeVelocityAlignmentTorque(person* p_i); //computes torque by forcing alignment with neighbouring particle's velocity

        double computeAngularFrictionTorque(person* p_i); //dumb friction to angular velocity
        double computePairDissipationTorque(person* p_i, person* p_j); //dissipation friction with neighbouring angular velocities

        double randNoisyTorque(RNG *sysRNG, double sigma);// returns a random torque centered on zero with SD sigma

        //auxiliary functions
        double generateZTorque(mathVector F, mathVector pos); // performs cross product, sets zTorque and also returns it.

    private:
        double xiAngular; //angular friction coeff
        double xiPair; //pair dissipation coeff

        double zetaPolar; //polar alignment coeff
        double zetaVelocity; //velocity alignment coeff
};

#endif // PLANARTORQUE_H
