#ifndef BOUNDARYMANAGER_H
#define BOUNDARYMANAGER_H

#include <vector>

#include "person.h"

//forward declarations here
class person;
class RNG;

class boundaryManager
{
    public:
        boundaryManager();
        boundaryManager(bool Periodic, double L_X, double L_Y);
        virtual ~boundaryManager();

        //place a horizontal or vertical wall of boundary particles
        std::vector<person> placeHorizontalBoundaryParticles(double radius, double overlapRatio, double yPos, double startX, double endX);
        std::vector<person> placeVerticalBoundaryParticles(double radius, double overlapRatio, double xPos, double startY, double endY);

        //gets
        bool getPeriodic();

        //periodic position related functions
        mathVector getPeriodicPosition(mathVector rawPos);
        mathVector getPeriodicDifference(mathVector v_i, mathVector v_j); //does v_i - v_j subject to periodic BCs
        double getPeriodicDist(mathVector v_i, mathVector v_j); // gets the distance between v_i, v_j subject to periodic BCs

        //random position functions
        mathVector getRandPos(RNG *randGen); //get a random position within the boundary and return it
        mathVector getRandPos(RNG *randGen, double xScale, double yScale); //gets a random position from a block xScale*yScale centered on the middle of the domain
        mathVector getRandPos(RNG *randGen, double edgeDist); //gets a random position within a block of the boundary such that particles are no closer than edgeDist from the boundary
        mathVector getRandPos(RNG *randGen, double xMin, double xMax, double yMin, double yMax); //gets a random position within the given bounds

        //scaling functions
        mathVector getScaledPos(double xScale, double yScale); // scales L_x, L_y and returns the appropriate vector
        mathVector getScaledPeriodicPos(double xScale, double yScale); //the above but periodic

    private:
        bool periodic; //do we have periodic boundary conditions or not?
        double L_x; // box size in x and y respectively
        double L_y;

        int N_boundary; // number of boundary particles (ie glued)
};

#endif // BOUNDARYMANAGER_H
