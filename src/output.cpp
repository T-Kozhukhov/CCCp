#include "output.h"
#include <sstream>
#include <iostream>

output::output(){
    //empty ctor
}

output::output(std::string FileName, std::string Path, std::string FileType, std::vector<person> *PList)
{
    //ctor
    fileName = FileName;
    pList = PList;
    fileType = FileType;
    path = Path;
}

output::~output()
{
    //dtor
}

void output::dump(int tStep){
    //set up some pointers
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New(); //polydata container
    vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New(); //points

    addParticlesToVTKDataset(pts, polyData); //add particle data to the unstructured grid

    polyData->Squeeze(); //remove unused memory

    //generate the full filenpath properly
    std::string fullFilePath; //init'ing the full file path
    std::stringstream ss;
    ss << path << "/" << fileName << tStep << "." << fileType; //generate file name in this format and save to stream
    ss >> fullFilePath; //output stream to full file name

    //create a writer and set filename accordingly
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    writer->SetFileName(fullFilePath.c_str());

    //write data
    writer->SetInputData(polyData);
    writer->SetDataModeToAscii();
    writer->Write();

    std::cout << "Stored data to: " << fullFilePath << std::endl;
}

std::string output::getPathInfo(){
    std::stringstream ss;
    std::string toReturn;
    ss << path << "/" << fileName << "{STEP}" << "." << fileType;
    ss >> toReturn;
    return toReturn;
}

void output::addParticlesToVTKDataset(vtkSmartPointer<vtkPoints> &pts, vtkSmartPointer<vtkPolyData> &polyData){
    //create arrays with a tuple for each particle containing the particle data
    vtkSmartPointer<vtkIntArray> IDs = vtkSmartPointer<vtkIntArray>::New();
    IDs->SetNumberOfComponents(1);
    IDs->SetName("Particle ID");

    vtkSmartPointer<vtkDoubleArray> velocity = vtkSmartPointer<vtkDoubleArray>::New();
    velocity->SetNumberOfComponents(3);
    velocity->SetName("Velocity");

    vtkSmartPointer<vtkDoubleArray> force = vtkSmartPointer<vtkDoubleArray>::New();
    force->SetNumberOfComponents(3);
    force->SetName("Force");

    vtkSmartPointer<vtkDoubleArray> polarity = vtkSmartPointer<vtkDoubleArray>::New();
    polarity->SetNumberOfComponents(1);
    polarity->SetName("Polarity");

    vtkSmartPointer<vtkDoubleArray> polVelocity = vtkSmartPointer<vtkDoubleArray>::New();
    polVelocity->SetNumberOfComponents(1);
    polVelocity->SetName("Polar Velocity");

    vtkSmartPointer<vtkDoubleArray> radius = vtkSmartPointer<vtkDoubleArray>::New();
    radius->SetNumberOfComponents(1);
    radius->SetName("Radius");

    //now loop through particles and add their data to the appropriate arrays:
    for(unsigned int i=0; i<pList->size(); i++){
        //IDs
        int id = pList->at(i).getID();
        IDs->InsertNextValue(id);

        //positions
        double pos[3] = {pList->at(i).getPosition().getX(), pList->at(i).getPosition().getY(),0};
        pts->InsertNextPoint(pos[0], pos[1], 0); //insert a data point at the next point

        //velocities
        double vel[3] = {pList->at(i).getVelocity().getX(), pList->at(i).getVelocity().getY(),0};
        velocity->InsertNextTuple(vel);

        //force
        double forces[3] = {pList->at(i).getForce().getX(), pList->at(i).getForce().getY(),0};
        force->InsertNextTuple(forces);

        //polarity
        double pol[1] = {pList->at(i).getPolAngle()};
        polarity->InsertNextTuple(pol);

        //polar velocity
        double polVel[1] = {pList->at(i).getPolVelocity()};
        polVelocity->InsertNextTuple(polVel);

        //radius
        double rad[1] = {pList->at(i).getRadius()};
        radius->InsertNextTuple(rad);
    }

    //finally add the program data to the dataset
    polyData->SetPoints(pts); //add points to the container
    //attach arrays to the data points
    polyData->GetPointData()->AddArray(IDs);
    polyData->GetPointData()->AddArray(velocity);
    polyData->GetPointData()->AddArray(force);
    polyData->GetPointData()->AddArray(polarity);
    polyData->GetPointData()->AddArray(polVelocity);
    polyData->GetPointData()->AddArray(radius);
}
