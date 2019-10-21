#include "csv.h"

#include <iostream>
#include <fstream>
#include <sstream>

//defining static members here
std::fstream csv::singleFile; //file for storing single particle information used only when dumping data of a single particle
bool csv::enableSinglePartFile; //bool to enable the above

csv::csv()
{
    //ctor
    csv::enableSinglePartFile = false; //set to false by default
}

csv::~csv()
{
    //dtor
    csv::singleFile.close(); //close the file
}

void csv::setupSingleFile(std::string filePath){ //set single particle dump to be enabled and prepare the file for usage
    csv::singleFile.open(filePath+"ParticleData.csv", std::ios::out);
    csv::enableSinglePartFile = true;
}

std::vector<person> csv::importPList(std::string path){
    std::vector<std::string> lines = getLines(path); //get and store the lines from the file

    std::vector<person> toReturn; // the vector we aim to return

    //as each line corresponds to a particle, loop through the lines to extract information
    for(unsigned int i = 0; i<lines.size(); i++){
        //split the line
        std::vector<std::string> words = splitLine(lines.at(i));

        //now extract information
        int ID = std::stoi(words.at(0));
        double xPos = std::stod(words.at(1));
        double yPos = std::stod(words.at(2));
        double polAngle = std::stod(words.at(3));
        double xVel = std::stod(words.at(4));
        double yVel = std::stod(words.at(5));
        double rad = std::stod(words.at(6));
        double meanRad = std::stod(words.at(7));
        bool glued = false;
        if(std::stoi(words.at(8))!=0){ //dealing with bool variable at end
            glued = true;
        }

        //create a few intermediary structures for readability
        mathVector pos = mathVector(xPos, yPos);
        mathVector vel = mathVector(xVel, yVel);

        //finally, create a person based on the above details and push it into the vector
        person currPerson = person(glued, ID, rad, meanRad, pos, vel, polAngle);
        toReturn.push_back(currPerson);
    }

    return toReturn; //return our constructed pList
}

physParam csv::importPhysParam(std::string path){
    //first, get lines from the specified file
    std::vector<std::string> lines = getLines(path);
    //now, extract version
    std::vector<std::string> firstLineWords = splitLine(lines.at(0));
    int version = std::stoi(firstLineWords.at(1)); //version is stored in the second cell on the first row

    //remove the version line from the lines vector for simplicity
    lines.erase(lines.begin());

    //making physParam to return:
    physParam toReturn;

    //do some output
    std::cout << "Beginning loading of parameters from "<< path << '\n';
    //check version and call the appropriate function as necessary
    if(version==1){ //each block will process the file and return the appropriate physParam
        toReturn = extractPhysParamV1(lines);
    } else if (version == 2){
        toReturn = extractPhysParamV2(lines);
    } else {
        std::cout << "Error: Cannot get version from physParam .csv at "<<path<<'\n'
                  << "Program believes we are on version " << version << '\n'
                  << "Returning a blank physParam object. Expect the program to fail.\n";
        return physParam(); //return a blank physParam
    }

    //give an output notifying depreciation
    if(version!=currPhysParamVersion){
        std::cout << "Warning - Parameter data file is of a depreciated version. The following parameters have been set to default values: \n";
    }
    //Output new parameters as necessary and init them. If things were added in version N, put them into case N-1 below. Do not add breaks except in last case
    switch(version){
    case 1: //The following were added as of version 2, thus for version 1 physParams we need to deal with the following
        std::cout << " (bool) dumpSingleParticle = false\n"; //output what is being defaulted and to what it is being defaulted
        toReturn.dumpSingleParticle = false; //set the variable to the default value
        std::cout << " (double) massRadiusRatio = 1\n";
        toReturn.massRadiusRatio = 1;
    case 2:
        break;
    }

    std::cout << "Loading of parameter data completed.\n";
    return toReturn;
}

void csv::exportPList(std::vector<person> pList, std::string pathOut, double avRadius){
    //we aim to generate a series of lines, each representing a person, before passing them to makeCSV
    std::vector<std::string> lines; //the lines we will pass to makeCSV at the end

    for(unsigned int i = 0; i < pList.size(); i++){ //loop through particles
        person currPerson = pList.at(i); //get current person like this for readability

        //get data from the person and store it as strings
        std::string id = std::to_string(currPerson.getID());
        std::string xPos = std::to_string(currPerson.getPosition().getX());
        std::string yPos = std::to_string(currPerson.getPosition().getY());
        std::string polAngle = std::to_string(currPerson.getPolAngle());
        std::string xVel = std::to_string(currPerson.getVelocity().getX());
        std::string yVel = std::to_string(currPerson.getVelocity().getY());
        std::string rad = std::to_string(currPerson.getRadius());
        std::string avRad = std::to_string(avRadius);
        std::string glued = "0";
        if(currPerson.getGlued()){ //store the bool by storing 1 if glued, 0 otherwise
            glued = "1";
        }

        //now, create a stringsteam to put the data together in correct format
        std::stringstream ss;
        ss  << id << ',' << xPos << ',' << yPos << ',' << polAngle << ',' << xVel << ','
            << yVel << ',' << rad << ',' << avRad << ',' << glued;
        std::string currLine = ss.str(); //getting current line via the string steam

        lines.push_back(currLine); //push back the current line
    }

    makeCSV(lines, pathOut); //finally, make a csv based on those lines at the specified full file path
}

void csv::exportPhysParam(physParam param, std::string path){
    std::vector<std::string> lines; //the lines we will make our CSV out of

    //write the version details in:
    std::stringstream ss;
    ss << "version:" << ',' << currPhysParamVersion;
    lines.push_back(ss.str()); //make it the first line

    //make the line containing the actual csv data
    std::string csvLine = getPhysParamCSVLine(param);
    lines.push_back(csvLine); //make it the second line

    //finally, make a csv based on the generated lines:
    makeCSV(lines, path+"PhysParamData.csv");

    //now, save the data into a readable format
    makeReadablePhysParam(param, path+"ParameterList_Readable.txt");
}

void csv::dumpParticleData(std::vector<person> pList, std::string pathOut, double currTime){ //do a normal particle data dump
    std::vector<std::string> lines;//our lines vector

    //loop through particles
    for(unsigned int i = 0; i<pList.size();i++){
        person currPerson = pList.at(i); //get current person for readability

        //get data from the person and store it as strings
        std::string xPos = std::to_string(currPerson.getPosition().getX());
        std::string yPos = std::to_string(currPerson.getPosition().getY());
        std::string xVel = std::to_string(currPerson.getVelocity().getX());
        std::string yVel = std::to_string(currPerson.getVelocity().getY());
        std::string polAngle = std::to_string(currPerson.getPolAngle());
        std::string polVel = std::to_string(currPerson.getPolVelocity());

        //make a stringstream and put the data in the correct format
        std::stringstream ss;
        ss << currTime << ',' << xPos << ',' << yPos << ',' << xVel << ',' << yVel << ','
           << polAngle << ',' << polVel;
        std::string currLine = ss.str();

        lines.push_back(currLine);//add the current line to the vector
    }

    //finally, make a CSV
    makeCSV(lines, pathOut);
}

void csv::dumpSingleParticleData(std::vector<person> pList, double currTime, int id){ //do a single particle data dump
    if(enableSinglePartFile){
        std::vector<std::string> lines;//our lines vector

        person currPerson = pList.at(id); //get first person in list

        //get data from the person and store it as strings
        std::string xPos = std::to_string(currPerson.getPosition().getX());
        std::string yPos = std::to_string(currPerson.getPosition().getY());
        std::string xVel = std::to_string(currPerson.getVelocity().getX());
        std::string yVel = std::to_string(currPerson.getVelocity().getY());
        std::string polAngle = std::to_string(currPerson.getPolAngle());
        std::string polVel = std::to_string(currPerson.getPolVelocity());

        //make a stringstream and put the data in the correct format
        std::stringstream ss;
        ss << currTime << ',' << xPos << ',' << yPos << ',' << xVel << ',' << yVel << ','
           << polAngle << ',' << polVel;
        std::string currLine = ss.str();

        lines.push_back(currLine);//add the current line to the vector

        //finally, make a CSV
        appendCSV(lines);
    }
}

std::vector<std::string> csv::getLines(std::string path){
    std::fstream fin; // make the file pointer
    fin.open(path, std::ios::in); //open the file specified by path in input mode

    std::vector<std::string> toReturn; // the vector of strings we aim to return
    std::string currLine; //create the string for the current line

    while (fin >> currLine){ //loop through all lines
        toReturn.push_back(currLine); //add the current line to the toReturn vector
    }

    fin.close(); //close the file
    return toReturn; //return as necessary
}

std::vector<std::string> csv::splitLine(std::string line){
    std::stringstream s(line); //making the stream which does the splitting based on the line

    std::vector<std::string> toReturn; //the vector of strings we aim to return
    std::string currWord; //the current word

    while(std::getline(s, currWord, ',')){ //loop through every instance of ","
        toReturn.push_back(currWord); //add each "word" to the toReturn vector
    }

    return toReturn;
}

void csv::makeCSV(std::vector<std::string> lines, std::string path){ //make a csv at the specified path with the included lines
    std::fstream fout; //file pointer
    fout.open(path, std::ios::out); //opens the file specified by the path in output mode

    for(unsigned int i = 0; i < lines.size(); i++){ //loop through the lines
        fout << lines.at(i) << "\n"; //add the lines to the file in this format
    }

    fout.close(); //close the file
}

void csv::appendCSV(std::vector<std::string> lines){ //append the specified lines to the singleFile
    for(unsigned int i = 0; i < lines.size(); i++){ //loop through the lines
        singleFile << lines.at(i) << "\n"; //add the lines to the file in this format
    }
}

physParam csv::extractPhysParamV1(std::vector<std::string> lines){ //extract a V1 physParam file
    //split the line into words
    std::vector<std::string> words = splitLine(lines.at(0));

    physParam toReturn; //creating a toReturn object

    ///Welcome to spaghetti land
    toReturn.stepSize = std::stod(words.at(0));
    toReturn.stepMax = std::stoi(words.at(1));

    toReturn.outputSteps = std::stoi(words.at(2));
    toReturn.outDirPath = words.at(3);
    toReturn.outFileName = words.at(4);
    toReturn.outFileType = words.at(5);

    toReturn.N = std::stoi(words.at(6));
    toReturn.meanR = std::stod(words.at(7));
    toReturn.sigmaR = std::stod(words.at(8));
    toReturn.sigmaV = std::stod(words.at(9));

    toReturn.periodic = std::stoi(words.at(10));
    toReturn.L_x = std::stod(words.at(11));
    toReturn.L_y = std::stod(words.at(12));
    toReturn.overlapRatio = std::stod(words.at(13));

    toReturn.loadParticles = std::stoi(words.at(14));
    toReturn.pathToParticles = words.at(15);
    toReturn.pathToLoadingCSV = words.at(16);
    toReturn.particleDumpSteps = std::stoi(words.at(17));
    toReturn.pathToParticleData = words.at(18);

    toReturn.enableHarmonicInterForce = std::stoi(words.at(19));
    toReturn.enableHertzianInterForce = std::stoi(words.at(20));
    toReturn.enableActiveForce = std::stoi(words.at(21));
    toReturn.enableGroundFrictionForce = std::stoi(words.at(22));
    toReturn.enablePersonFrictionForce = std::stoi(words.at(23));
    toReturn.enableRandNoisyForce = std::stoi(words.at(24));

    toReturn.zetaActive = std::stod(words.at(25));
    toReturn.zetaGround = std::stod(words.at(26));
    toReturn.zetaPerson = std::stod(words.at(27));
    toReturn.v_0 = std::stod(words.at(28));
    toReturn.kHarmonic = std::stod(words.at(29));
    toReturn.kHertzian = std::stod(words.at(30));
    toReturn.sigmaForceX = std::stod(words.at(31));
    toReturn.sigmaForceY = std::stod(words.at(32));

    toReturn.enablePolarAlignmentTorque = std::stoi(words.at(33));
    toReturn.enableVelocityAlignmentTorque = std::stoi(words.at(34));
    toReturn.enableAngularFrictionTorque = std::stoi(words.at(35));
    toReturn.enablePairDissipationTorque = std::stoi(words.at(36));
    toReturn.enableRandNoisyTorque = std::stoi(words.at(37));

    toReturn.xiAngular = std::stod(words.at(38));
    toReturn.xiPair = std::stod(words.at(39));
    toReturn.zetaPolar = std::stod(words.at(40));
    toReturn.zetaVelocity = std::stod(words.at(41));
    toReturn.sigmaTorque = std::stod(words.at(42));

    toReturn.debugType = std::stoi(words.at(43));

    return toReturn;
}

physParam csv::extractPhysParamV2(std::vector<std::string> lines){ //extract a v2 physParam
    // standard setup
    physParam toReturn;
    std::vector<std::string> words = splitLine(lines.at(0));

    // extract all version 1 values here
    toReturn = extractPhysParamV1(lines);

    //Then extract the newly added values
    toReturn.dumpSingleParticle = std::stoi(words.at(44));
    toReturn.massRadiusRatio = std::stod(words.at(45));

    return toReturn;
}

std::string csv::getPhysParamCSVLine(physParam param){ //makes a physParam csv line for the current version of physParam
    std::stringstream ss; //make a string stream

    ///Welcome to spaghetti land
    //do actual writing to the string stream in the necessesary csv format
    ss  << std::to_string(param.stepSize)<<','<<std::to_string(param.stepMax)
        <<','<<std::to_string(param.outputSteps)<<','<<param.outDirPath
        <<','<<param.outFileName<<','<<param.outFileType
        <<','<<std::to_string(param.N)<<','<<std::to_string(param.meanR)
        <<','<<std::to_string(param.sigmaR)<<','<<std::to_string(param.sigmaV)
        <<','<<std::to_string(param.periodic)<<','<<std::to_string(param.L_x)
        <<','<<std::to_string(param.L_y)<<','<<std::to_string(param.overlapRatio)
        <<','<<std::to_string(param.loadParticles)
        <<','<<param.pathToParticles
        <<','<<param.pathToLoadingCSV
        <<','<<std::to_string(param.particleDumpSteps)
        <<','<<param.pathToParticleData
        <<','<<std::to_string(param.enableHarmonicInterForce)
        <<','<<std::to_string(param.enableHertzianInterForce)
        <<','<<std::to_string(param.enableActiveForce)<<','
        <<std::to_string(param.enableGroundFrictionForce)<<','
        <<std::to_string(param.enablePersonFrictionForce)<<','
        <<std::to_string(param.enableRandNoisyForce)<<','
        <<std::to_string(param.zetaActive)<<','<<std::to_string(param.zetaGround)<<','
        <<std::to_string(param.zetaPerson)<<','<<std::to_string(param.v_0)<<','
        <<std::to_string(param.kHarmonic)<<','<<std::to_string(param.kHertzian)<<','
        <<std::to_string(param.sigmaForceX)<<','<<std::to_string(param.sigmaForceY)<<','
        <<std::to_string(param.enablePolarAlignmentTorque)<<','
        <<std::to_string(param.enableVelocityAlignmentTorque)<<','
        <<std::to_string(param.enableAngularFrictionTorque)<<','
        <<std::to_string(param.enablePairDissipationTorque)<<','
        <<std::to_string(param.enableRandNoisyTorque)<<','
        <<std::to_string(param.xiAngular)<<','<<std::to_string(param.xiPair)<<','
        <<std::to_string(param.zetaPolar)<<','<<std::to_string(param.zetaVelocity)<<','
        <<std::to_string(param.sigmaTorque)<<','<<std::to_string(param.debugType)<<','
        <<std::to_string(param.dumpSingleParticle)<<','<<std::to_string(param.massRadiusRatio)<<',';
    return ss.str(); //finally return the necessary string
}

 void csv::makeReadablePhysParam(physParam param, std::string path){
    //open the specified file
    std::fstream fout; //file pointer
    fout.open(path, std::ios::out); //opens the file specified by the path in output mode

    ///Welcome to spaghetti land
    //now write to the file
    fout    <<"[B] indicates this is a boolean variable. A boolean has value 0 if false and 1 if true.\n"
            <<"The current physParam version is version " << currPhysParamVersion << '\n'
            <<'\n'
            <<"Step size: "<<std::to_string(param.stepSize)<<'\n'
            <<"Maximum number of steps: "<<std::to_string(param.stepMax)<<'\n'
            <<"Dump .vtp files every n steps. n: "<<std::to_string(param.outputSteps)<<'\n'
            <<"The directory to store .vtp files in: "<<param.outDirPath<<'\n'
            <<"Prefix to the timestamp for .vtp files: "<<param.outFileName<<'\n'
            <<"Output file type (should typically be vtp): "<<param.outFileType<<'\n'
            <<"If randomly generating N particles (or debug type 0), how many particles to generate: "<<std::to_string(param.N)<<'\n'
            <<"Mean radius of particles: "<<std::to_string(param.meanR)<<'\n'
            <<"Standard deviation of radius of particles: "<<std::to_string(param.sigmaR)<<'\n'
            <<"Standard deviation of initial velocity of particles: "<<std::to_string(param.sigmaV)<<'\n'
            <<"[B] Does the system have periodic BCs: "<<std::to_string(param.periodic)<<'\n'
            <<"Size of the system box in the x direction: "<<std::to_string(param.L_x)<<'\n'
            <<"Size of the system box in the y direction: "<<std::to_string(param.L_y)<<'\n'
            <<"If generating 'boundary particles', the 'overlap ratio' between generated boundary particles: "<<std::to_string(param.overlapRatio)<<'\n'
            <<"[B] Whether to load particle ICs from a file: "<<std::to_string(param.loadParticles)<<'\n'
            <<"The full file path leading to the .csv file containing particle ICs: "<<param.pathToParticles<<'\n'
            <<"File path where CSVs for loading will be stored: "<<param.pathToLoadingCSV<<'\n'
            <<"Dump particle information every n steps (0 means no dumps). n: "<<std::to_string(param.particleDumpSteps)<<'\n'
            <<"File path where particle information will be stored: "<<param.pathToParticleData<<'\n'
            <<"[B] Are harmonic interactional forces enabled: "<<std::to_string(param.enableHarmonicInterForce)<<'\n'
            <<"[B] Are hertzian interactional forces enabled: "<<std::to_string(param.enableHertzianInterForce)<<'\n'
            <<"[B] Are particle active forces enabled: "<<std::to_string(param.enableActiveForce)<<'\n'
            <<"[B] Are particle ground friction forces enabled: "<<std::to_string(param.enableGroundFrictionForce)<<'\n'
            <<"[B] Are particle person friction forces enabled: "<<std::to_string(param.enablePersonFrictionForce)<<'\n'
            <<"[B] Are particle random noisy forces enabled: "<<std::to_string(param.enableRandNoisyForce)<<'\n'
            <<"Friction parameter included only in the active force: "<<std::to_string(param.zetaActive)<<'\n'
            <<"Friction parameter against the ground, included only in the ground friction force: "<<std::to_string(param.zetaGround)<<'\n'
            <<"Friction parameter against other people, used only in person friction force: "<<std::to_string(param.zetaPerson)<<'\n'
            <<"V_0 parameter. Affects both initial velocities and the active force: "<<std::to_string(param.v_0)<<'\n'
            <<"Spring constant used in harmonic interactional forces: "<<std::to_string(param.kHarmonic)<<'\n'
            <<"Spring constant used in hertzian interactional forces: "<<std::to_string(param.kHertzian)<<'\n'
            <<"Standard deviation in the x direction for random noisy forces: "<<std::to_string(param.sigmaForceX)<<'\n'
            <<"Standard deviation in the y direction for random noisy forces: "<<std::to_string(param.sigmaForceY)<<'\n'
            <<"[B] Are polar alignment torques enabled: "<<std::to_string(param.enablePolarAlignmentTorque)<<'\n'
            <<"[B] Are velocity alignment torques enabled: "<<std::to_string(param.enableVelocityAlignmentTorque)<<'\n'
            <<"[B] Are angular friction torques enabled: "<<std::to_string(param.enableAngularFrictionTorque)<<'\n'
            <<"[B] Are pair dissipation torques enabled: "<<std::to_string(param.enablePairDissipationTorque)<<'\n'
            <<"[B] Are random noisy torques enabled: "<<std::to_string(param.enableRandNoisyTorque)<<'\n'
            <<"Friction parameter for angular friction torques: "<<std::to_string(param.xiAngular)<<'\n'
            <<"Friction parameter for pair dissipation torques: "<<std::to_string(param.xiPair)<<'\n'
            <<"Coefficient of polar alignment torques: "<<std::to_string(param.zetaPolar)<<'\n'
            <<"Coefficient of velocity alignment torques: "<<std::to_string(param.zetaVelocity)<<'\n'
            <<"Standard deviation for random noisy torques: "<<std::to_string(param.sigmaTorque)<<'\n'
            <<"Debug override. IF NOT 0, uses hardcoded overrides for initial conditions. See docs for more details: "<<std::to_string(param.debugType)<<'\n'
            <<"[B] If dumping particle data, do we dump only a single particle to a single file: "<<std::to_string(param.dumpSingleParticle)<<'\n'
            <<"Ratio of mass to radius^2, used for calculating mass of particles: "<<std::to_string(param.massRadiusRatio);

    fout.close(); //close the file at end
 }
