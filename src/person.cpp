#include <cmath>

#include "person.h"

double person::massRadiusRatio = 1; //init'ing static member

person::person(){
    //dummy ctor, sets default values
    glued = true;
    radius = 1;
    position = mathVector();
    velocity = mathVector();
    polAngle = 0;
    polVelocity = 0;
    id = 0;

    netForce = mathVector();
    netTorque = 0;
    noiseForce = mathVector();
    noiseTorque = 0;

    mass = calculateMass();
    inertia = calculateIntertia();
}

person::person(bool Glued, int ID, double Radius, double avRadius, mathVector pos, mathVector vel, double PolAngle)
{
    // initialise the standard variables
    glued = Glued;
    radius = Radius;
    position = pos;
    velocity = vel;
    polAngle = PolAngle;
    polVelocity = 0;
    id = ID;

    //set dynamics related variables to zero to start with
    netForce = mathVector();
    netTorque = 0;
    noiseForce = mathVector();
    noiseTorque = 0;

    //get mass and inertia of the particle
    mass = calculateMass();
    inertia = calculateIntertia();

    //set up neighbour list
    nListManager = new neighbourList(ID, avRadius);

    //fixing glued particles so they don't move, even by accident
    if(glued){
        polAngle = 0;
        velocity = mathVector(); // set to default (zero) vector
        return; //stop here, we need not do any more for a glued particle
    }
}

person::~person()
{
    //dtor
}

void person::killPointers(){
    if(nListManager!=NULL){ //do a safe deallocation of nListManager, to be called only at the end of the parent System class' life
        delete nListManager;
        nListManager = NULL;
    }
}

void person::update(double dt, boundaryManager *bManager){
    if(!glued){
        double sqrtDT = std::sqrt(dt); //getting sqrt of dt to deal with stochastic errors

        //do position first
        mathVector acceleration = netForce.scale(1.0/mass); // get acceleration from force by dividing through
        mathVector noiseAccel = noiseForce.scale(1.0/mass); //done separately to deal with stochastic errors
        velocity = velocity + acceleration*dt + noiseAccel*sqrtDT; // *sqrtDT done to deal with stochastic errors
        position = position + velocity*dt;
        //now handle polarity in the exact same way
        double polAccel = netTorque/inertia;
        double noisePolAccel = noiseTorque/inertia;
        polVelocity = polVelocity + polAccel*dt + noisePolAccel*sqrtDT;
        polAngle = polAngle + polVelocity*dt;

        //apply periodic boundary conditions if necessary
        position = bManager->getPeriodicPosition(position);
    }
}

void person::setID(int newID){
    id = newID;
    nListManager->updateParentID(newID);
}

int person::getID(){
    return id;
}

mathVector person::getPolarity(){ // returns polarity based on polAngle
    return mathVector::getPolarVector(1, polAngle);
}

mathVector person::getPosition(){
    return position;
}

mathVector person::getVelocity(){
    return velocity;
}

double person::getPolAngle(){
    return polAngle;
}

double person::getPolVelocity(){
    return polVelocity;
}

double person::getVelAngle(){
    return velocity.angle();
}

double person::getRadius(){
    return radius;
}

double person::getMass(){
    return mass;
}

double person::getInertia(){
    return inertia;
}

mathVector person::getForce(){
    return netForce;
}

void person::addForce(mathVector F_1){
    netForce = netForce+F_1;
}

void person::setForce(mathVector F_1){
    netForce = F_1;
}

void person::setNoisyForce(mathVector F){
    noiseForce = F;
}

double person::getPlanarTorque(){
    return netTorque;
}

void person::addTorque(double T_1){
    netTorque += T_1;
}

void person::setTorque(double T_1){
    netTorque = T_1;
}

void person::setNoisyTorque(double T){
    noiseTorque = T;
}

void person::setMassRadiusRatio(double Ratio){
    person::massRadiusRatio = Ratio;
}

double person::getDistTo(person p_j){
    return getDistTo(p_j.getPosition());
}

double person::getDistTo(mathVector v_j){
    return position.getDistTo(v_j); //get the distance to v_j....
}

std::vector<int> person::getCurrNList(){
    return nListManager->getNeighbours();
}

void person::recalculateNList(std::vector<person> *pList, boundaryManager *bManager){
    nListManager->calculateList(pList, bManager);
}

person person::getRandPerson(bool glued, int ID, double meanR, double sigmaR, double v_0, double sigmaV, RNG *randGen, boundaryManager *bManager){
    //generate a position
    mathVector pos = bManager->getRandPos(randGen);

    return getRandPerson(glued, ID, meanR, sigmaR, v_0, sigmaV, randGen, pos); //call the below function to get the actual person
}

person person::getRandPerson(bool glued, int ID, double meanR, double sigmaR, double v_0, double sigmaV, RNG *randGen, mathVector pos){
    double radius = randGen->getGauss(meanR, sigmaR); //get a radius based on the appropriate parameters
    double polAngle = randGen->getUniformDouble(0, 6.28318530); //generate a random number between 0 and 2*PI
    double velMagnitude = randGen->getGauss(v_0, sigmaV); //getting a gaussian velocity magnitude
    mathVector vel = mathVector::getPolarVector(velMagnitude, polAngle); //generate a random velocity vector by using the previous magnitude and the angle of polarity

    person newPerson = person(glued, ID, radius, meanR, pos, vel, polAngle);
    return newPerson;
}

double person::calculateMass(){
    //assume that mass is proportional to radius^2 (as per a cylinder)
    return massRadiusRatio*(radius*radius);
}

double person::calculateIntertia(){
    mass = calculateMass(); //make sure mass is updated
    //Here we assume that a person is in fact a solid cylinder of radius r
    return 0.5*mass*(radius*radius); //just a formula found off Wikipedia
}
