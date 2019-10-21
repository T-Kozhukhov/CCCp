#ifndef PERSON_H
#define PERSON_H

#include <vector>

#include "vector.h"
#include "neighbourList.h"
#include "physParam.h"
#include "RNG.h"

class neighbourList; //forward declaration to avoid cyclic dependency issue
class boundaryManager;

class person
{
    public:
        person(); //dummy ctor
        person(bool Glued, int ID, double Radius, double avRadius, mathVector pos, mathVector vel, double polAngle); //primary ctor
        virtual ~person(); //general destructor
        void killPointers(); //kills pointers at the end of the simulation

        void update(double dt, boundaryManager *bManager); // updates positions and velocities in time for the next time step (ie, activates the double buffer)

        //ID get sets
        void setID(int newID);
        int getID();

        // mathematical gets
        mathVector getPolarity(); // returns a unit vector representing the polarity
        mathVector getPosition();
        mathVector getVelocity();

        //angular gets
        double getPolAngle(); //gets the polarity angle
        double getPolVelocity(); //gets the rate of change of the polarity angle
        double getVelAngle(); //gets the velocity angle

        //physics gets
        double getRadius();
        double getMass(); // gets mass (based on radius)
        double getInertia(); // gets inertia (based on radius)
        bool getGlued(){ return glued; }

        //force helpers
        mathVector getForce(); // gets the net force on the particle
        void addForce(mathVector F_1);
        void setForce(mathVector F_1);
        void setNoisyForce(mathVector F);

        //torque helpers
        double getPlanarTorque(); // gets the net planar torque on the particle
        void addTorque(double T_1);
        void setTorque(double T_1);
        void setNoisyTorque(double T);

        //static sets
        static void setMassRadiusRatio(double Ratio); //sets the mass-radius^2 ratio for all particles in the simulation

        //auxiliary functions
        double getDistTo(person p_j); // gets the distance to a person p_j without a BC manager
        double getDistTo(mathVector v_j); // gets the distance to a vector v_j without a BC manager
        std::vector<int> getCurrNList(); // returns the current neighbour list
        void recalculateNList(std::vector<person> *pList, boundaryManager *bManager);//recalculate the nList for this particle

        static person getRandPerson(bool glued, int ID, double meanR, double sigmaR, double v_0, double sigmaV, RNG *randGen, boundaryManager *bManager); // return a person with appropriate random position, polarity, and velocity along polarity
        static person getRandPerson(bool glued, int ID, double meanR, double sigmaR, double v_0, double sigmaV, RNG *randGen, mathVector pos); //return a person at the specified position, but with everything else random

    private:
        int id; // the ID of this particle in it's container

        // mathematical properties
        mathVector position;
        double polAngle; // angle of polarity, represents the direction the person is pointing
        mathVector velocity;
        double polVelocity;

        //physics properties for the individual
        double radius;
        double mass;
        double inertia;
        bool glued; // is this particle glued or not?

        //physics related static variables
        static double massRadiusRatio; //the ratio of mass to radius

        //physics helpers
        double calculateMass(); //used to calculate the mass of the particle
        double calculateIntertia(); //used to calculate the inertia of the particle

        //dynamics related variables
        mathVector netForce;
        mathVector noiseForce; //specifically holds noisy forces
        double netTorque;
        double noiseTorque; //specifically holds noisy torques

        //auxiliary objects
        neighbourList *nListManager; // the neighbour list manager for this individual person

};

#endif
