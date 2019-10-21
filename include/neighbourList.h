#ifndef NEIGHBOURLIST_H
#define NEIGHBOURLIST_H

#include <vector>

#include "person.h"
#include "boundaryManager.h"

class person;
class boundaryManager;

class neighbourList
{
    public:
        neighbourList(); // for initialisation only
        neighbourList(int ParentID, float avRadius); //primary ctor
        ~neighbourList();

        //functions used in the system checking/ updating mechanism
        static bool checkUpdate(std::vector<person> *pList, boundaryManager *bManager); //perform a check to see if an update needs to occur
        static void updateLists(std::vector<person> *pList, boundaryManager *bManager); //update lists if necessary
        void calculateList(std::vector<person> *pList, boundaryManager *bManager); //recalculate an individual list

        //used when merging pLists
        void updateParentID(int newID); //update the ID of a singular nList object

        std::vector<int> getNeighbours(); // returns the neighbour list

    private:
        std::vector<int> nList; // contains the indexes of all neighbour particles
        static std::vector<mathVector> posList; // static var containing positions of all particles at time of last calculation
        unsigned int parentID; //ID of the parent particle of the particular nList

        // critical values
        static double r_m; //critical distance for a particle to be in the neighbour list
        static double r_c; //distance at which the potential from other particles ceases to have an effect
        ///typically, for average radius <R>; r_c = 2.4*<R>, r_m-r_c = 0.5*<R>
};

#endif // NEIGHBOURLIST_H
