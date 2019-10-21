#include "force.h"

#include <cmath>
#include <iostream>

force::force()
{
    //ctor
}

force::force(boundaryManager *BManager, double ZetaActive, double ZetaGround,
    double ZetaPerson, double V_0, double KHarmonic, double KHertzian){
    //simply set the values of member variables equal to the passed parameters
    bManager = BManager;
    zetaActive = ZetaActive;
    zetaGround = ZetaGround;
    zetaPerson = ZetaPerson;
    v_0 = V_0;
    kHarmonic = KHarmonic;
    kHertzian = KHertzian;
}

force::~force()
{
    //dtor
}

mathVector force::computeHarmonicInterForce(person* p_i, person* p_j){
    //do gets to make code more readable
    double radius_j = p_j->getRadius();
    mathVector pos_j = p_j->getPosition(); //no need to get periodic positions as that's done in getPeriodicDifference
    double radius_i = p_i->getRadius();
    mathVector pos_i = p_i->getPosition();

    // first, get a vector pointing from p_i to p_j taking into account periodic BCs
    mathVector vecDiff = bManager->getPeriodicDifference(pos_j, pos_i); //does p_j - p_i subject to BCs
    //get the unit vector
    mathVector unitVecDiff = vecDiff.unit();

    //get displacement using vecDiff
    double vecMagn = vecDiff.magnitude();

    //now, calculate the scalar at the front of the interaction force
    double interScalar = (-kHarmonic)*(radius_i + radius_j - vecMagn);
    //perform check on interScalar, as it MUST non-positive
    if (interScalar > 0){
        interScalar = 0;
    }

    mathVector forceVec = unitVecDiff*interScalar; // set the vector of the force as required
    return forceVec; // return a force based on the above force vector
}

mathVector force::computeHertzianInterForce(person* p_i, person* p_j){
    //very similar to the above, except around the calculation of the interscalar double
    //do gets to make code more readable
    double radius_j = p_j->getRadius();
    mathVector pos_j = p_j->getPosition(); //no need to get periodic positions as they're done in getPeriodicDifference
    double radius_i = p_i->getRadius();
    mathVector pos_i = p_i->getPosition();

    // first, get a vector pointing from p_i to p_j taking into account periodic BCs
    mathVector vecDiff = bManager->getPeriodicDifference(pos_j, pos_i); //does p_j - p_i subject to BCs
    //get the unit vector
    mathVector unitVecDiff = vecDiff.unit();

    //get displacement using vecDiff
    double vecMagn = vecDiff.magnitude();

    //changes occur here..
    //first, we calculate the compression factor
    double compFactor = (radius_i + radius_j - vecMagn);
    //perform check on compFactor, as it MUST be non-negative
    if (compFactor < 0){
        compFactor = 0;
    }
    //do the same as before, but now the compFactor has power 3/2 due to Hertzian things....
    double interScalar = (-kHertzian)*std::sqrt(compFactor*compFactor*compFactor); //using this instead of pow(... , 3/2) is twice as fast (typically)
    //perform check on interScalar, as it MUST be non-positive

    mathVector forceVec = unitVecDiff*interScalar; // set the vector of the force as required
    return forceVec; // return a force based on the above force vector
}

mathVector force::computeActiveForce(person* p_i){
    mathVector unitPolarity = p_i->getPolarity(); // gets the unit vector of polarity
    double activeScalar = zetaActive*v_0; // get the scalar of the unit vector

    mathVector forceVec = unitPolarity*activeScalar; // the vector for the force as required
    return forceVec; // return a force based on the above force vector
}

mathVector force::computeGroundFrictionForce(person* p_i){
    mathVector vel = p_i->getVelocity();
    return (vel*((-1)*zetaGround));
}

mathVector force::computePersonFrictionForce(person* p_i, person* p_j){
    mathVector v_i = p_i->getVelocity();
    mathVector v_j = p_j->getVelocity();
    mathVector diff = v_i-v_j; //gets a difference in velocities

    return (diff*((-1)*zetaPerson)); //returns -1 * zeta * diff
}

mathVector force::randNoisyForce(RNG *sysRNG, double sigma_x, double sigma_y){
    //get force components
    double force_x = sysRNG->getGauss(sigma_x);
    double force_y = sysRNG->getGauss(sigma_y);

    //create a vector based on those components
    mathVector forceVec = mathVector(force_x, force_y);
    return forceVec; // return a force based on the above force vector
}
