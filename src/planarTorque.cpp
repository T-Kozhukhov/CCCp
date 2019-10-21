#include "planarTorque.h"

#include <cmath>

planarTorque::planarTorque()
{
    //ctor
}

planarTorque::~planarTorque()
{
    //dtorS
}

planarTorque::planarTorque(double XiAngular, double XiPair, double ZetaPolar, double ZetaVelocity){
    // set member variables accordingly
    xiAngular = XiAngular;
    xiPair = XiPair;
    zetaPolar = ZetaPolar;
    zetaVelocity = ZetaVelocity;
}


double planarTorque::computePolarAlignmentTorque(person* p_i, person* p_j){
    double pol_i = p_i->getPolAngle();
    double pol_j = p_j->getPolAngle();
    double polDiff = pol_i-pol_j; //getting difference in polar angles

    return -1*zetaPolar*std::sin(polDiff); //returns the torque for this pair as necessary
}

double planarTorque::computeVelocityAlignmentTorque(person* p_i){
    double pol_i = p_i->getPolAngle();
    double velAngle_i = p_i->getVelAngle(); //getting the angle of VELOCITY here, rather than polarity
    double angleDiff = pol_i-velAngle_i;

    return -1*zetaVelocity*std::sin(angleDiff);
}

double planarTorque::computeAngularFrictionTorque(person* p_i){
    double polVel_i = p_i->getPolVelocity();
    return -1*xiAngular*polVel_i; //returns the necessary angualr velocity friction
}

double planarTorque::computePairDissipationTorque(person* p_i, person* p_j){
    double polVel_i = p_i->getPolVelocity();
    double polVel_j = p_j->getPolVelocity();
    double polVelDiff = polVel_i-polVel_j; //gets the difference in polar velocities between the two particles

    return -1*xiPair*polVelDiff;
}

double planarTorque::randNoisyTorque(RNG *sysRNG, double sigma){
    return sysRNG->getGauss(sigma);
}


double planarTorque::generateZTorque(mathVector F, mathVector pos){
    return F.zCross(pos); // perform the cross product of F x pos, set z component to zTorque
}
