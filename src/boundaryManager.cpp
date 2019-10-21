#include "boundaryManager.h"

#include <cmath>
#include <iostream>

boundaryManager::boundaryManager(){
    //dummy ctor
    periodic = true;
    L_x = 10;
    L_y = 10;
    N_boundary = 0;
}

boundaryManager::boundaryManager(bool Periodic, double L_X, double L_Y)
{
    //ctor
    periodic = Periodic;
    L_x = L_X;
    L_y = L_Y;
    N_boundary = 0;
}

boundaryManager::~boundaryManager()
{
    //dtor
}

std::vector<person> boundaryManager::placeHorizontalBoundaryParticles(double radius, double overlapRatio, double yPos, double startX, double endX){ //places glued boundary particles horizontally
    std::vector<person> toReturn;

    double xPos = startX;
    do{ //make at least one vertical particle on each side. Loop through....
        toReturn.push_back(person(true, N_boundary, radius, radius, mathVector(xPos,yPos), mathVector(0,0), 0)); //make a person at specified position

        N_boundary += 1; //update count of boundary particles appropriately
        xPos += (1+overlapRatio)*radius; // increase the position as required
    }while(xPos<=endX); // ... as long as the highest particle's position hasn't exceeded the domain

    return toReturn;
}

std::vector<person> boundaryManager::placeVerticalBoundaryParticles(double radius, double overlapRatio, double xPos, double startY, double endY){ //places glued boundary particles vertically
    std::vector<person> toReturn;

    double yPos = startY;
    do{ //make at least one vertical particle on each side. Loop through....
        toReturn.push_back(person(true, N_boundary, radius, radius, mathVector(xPos,yPos), mathVector(0, 0), 1.5707)); //make a person at specified position

        N_boundary += 1; //update count of boundary particles appropriately
        yPos += (1+overlapRatio)*radius; // increase the position as required
    }while(yPos<=endY); // ... as long as the highest particle's position hasn't exceeded the domain

    return toReturn;
}

bool boundaryManager::getPeriodic(){
    return periodic;
}

mathVector boundaryManager::getPeriodicPosition(mathVector rawPos){
    if(periodic){ // if we're in a periodic system...
        // get positions out first
        double x = rawPos.getX();
        double y = rawPos.getY();

        //for x:
        while (x > L_x){ x -= L_x; } //recursively reduce x until we are within the domain
        while (x < 0){ x += L_x; } //recursively increase x until we are within the domain

        //do the same as above for y:
        while (y > L_y){ y -= L_y; }
        while (y < 0){ y += L_y; }

        return mathVector(x, y); // return a mathvector based on the above
    } else { //if we're not in a periodic system
        return rawPos; // return the rawPos
    }
}

mathVector boundaryManager::getPeriodicDifference(mathVector v_i, mathVector v_j){
    //first get the periodic positions
    v_i = getPeriodicPosition(v_i);
    v_j = getPeriodicPosition(v_j);

    if(periodic){ //if the system is periodic...
        //calculate the raw differences first
        double dx = v_i.getX() - v_j.getX();
        double dy = v_i.getY() - v_j.getY();

        //for x, we consider...
        if (dx > L_x/2){ //if the difference is more than half the length, then j should be in the left side, so...
            dx -= L_x; //subtract a length from it
        } else if (dx < -L_x/2) { //similarly for going negative
            dx += L_x;
        }

        //for y we have essentially the same as above:
        if (dy > L_y/2){
            dy -= L_y;
        } else if (dy < -L_y/2){
            dy += L_y;
        }

        //generate a new vector based on these modified dx, dy, and return it
        return mathVector(dx, dy);
    } else { // if the system isn't periodic
        return (v_i-v_j); // return the standard difference
    }
}

double boundaryManager::getPeriodicDist(mathVector v_i, mathVector v_j){
    mathVector diff = getPeriodicDifference(v_i, v_j); //gets the periodic difference
    return diff.magnitude(); //return the magnitude of the periodic difference
}

mathVector boundaryManager::getRandPos(RNG *randGen){ //get a random position within the boundary
    return getRandPos(randGen, 1, 1);
}

mathVector boundaryManager::getRandPos(RNG *randGen, double xScale, double yScale){
    if(xScale>1||xScale<0||yScale>1||yScale<0){ //error catching
        std::cout << "Error: Attempted to call boundaryManager::getRandPos(RNG*, double, double) with erronous parameters.\n"
                  << "Ensure that the scalars are betwee 0 and 1. Returning a random position within the boundary instead.\n";
        return getRandPos(randGen);
    }

    //generate bounds for the random number generators
    double xMin = L_x*(0.5 - xScale/2);
    double xMax = L_x*(0.5 + xScale/2);
    double yMin = L_y*(0.5 - yScale/2);
    double yMax = L_y*(0.5 + yScale/2);

    return getRandPos(randGen, xMin, xMax, yMin, yMax);
}

mathVector boundaryManager::getRandPos(RNG *randGen, double edgeDist){
    //get the relevent bounds for the random number generators
    double xMin = edgeDist;
    double xMax = L_x-edgeDist;
    double yMin = edgeDist;
    double yMax = L_y-edgeDist;

    //first, we'll do some simple error checking
    if(xMin>xMax){
        std::cout << "Error: Attempted to call boundaryManager::getRandPos(RNG*, double) with too large an edge distance.\n"
                  << "Either reduce the parameter, or increase L_x. Returning random position within the boundary instead.\n";
        return getRandPos(randGen);
    }
    if(yMin>yMax){
        std::cout << "Error: Attempted to call boundaryManager::getRandPos(RNG*, double) with too large an edge distance.\n"
                  << "Either reduce the parameter, or increase L_y. Returning random position within the boundary instead.\n";
        return getRandPos(randGen);
    }

    return getRandPos(randGen, xMin, xMax, yMin, yMax);
}

mathVector boundaryManager::getRandPos(RNG *randGen, double xMin, double xMax, double yMin, double yMax){
    double xPos = randGen->getUniformDouble(xMin, xMax);
    double yPos = randGen->getUniformDouble(yMin, yMax);

    return mathVector(xPos, yPos);
}

mathVector boundaryManager::getScaledPos(double xScale, double yScale){
    return mathVector(xScale*L_x, yScale*L_y);
}

mathVector boundaryManager::getScaledPeriodicPos(double xScale, double yScale){
    return getPeriodicPosition(getScaledPos(xScale, yScale));
}
