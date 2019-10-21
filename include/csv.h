#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <string>

#include "physParam.h"
#include "person.h"

class csv
{
    public:
        csv();
        ~csv();

        static void setupSingleFile(std::string filePath); //set a flag for a file to hold single particle information using a full filepath

        //import data from a csv
        static std::vector<person> importPList(std::string path);// imports a pList from the full filepath specified
        static physParam importPhysParam(std::string path);//imports a physParam from the full filepath specified

        //export data into a csv for later use
        static void exportPList(std::vector<person> pList, std::string pathOut, double avRadius);//export a pList into the specified full file path
        static void exportPhysParam(physParam param, std::string path);//exports physParam to the directory specified as "PhysParamData.csv", also making a readable format to

        //dump particle data
        static void dumpParticleData(std::vector<person> pList, std::string pathOut, double currTime); //dumps important particle data to the specified path for analysis
        static void dumpSingleParticleData(std::vector<person> pList, double currTime, int id); //dumps important particle data for the particle with id 0 ONLY to the singleFile below

    private:
        static std::vector<std::string> getLines(std::string path); //gets lines from the file at the specified full file path
        static std::vector<std::string> splitLine(std::string line);//splits a given line into it's component strings

        static void makeCSV(std::vector<std::string> lines, std::string path);//make a CSV based on the lines at the specified full file path
        static void appendCSV(std::vector<std::string> lines); //append information to the singleFile csv

        //importing physParam versions
        static const int currPhysParamVersion = 2; //the current version of physParam
        static physParam extractPhysParamV1(std::vector<std::string> lines);
        static physParam extractPhysParamV2(std::vector<std::string> lines);

        //exporting physParam
        static std::string getPhysParamCSVLine(physParam param); //generates the actual line for the csv containing physParam data
        static void makeReadablePhysParam(physParam param, std::string path); //does the same as above but in a .txt file with readable parameters

        //single particle data related vars
        static std::fstream singleFile; //file for storing single particle information
        static bool enableSinglePartFile;
};

#endif // INPUT_H
