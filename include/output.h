#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <vector>

//VTK includes
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>

#include "person.h"

class output
{
    public:
        output();
        output(std::string FileName, std::string Path, std::string FileType, std::vector<person> *PList);
        virtual ~output();

        void dump(int tStep); //perform a vtp dump

        std::string getPathInfo(); //returns information on the file path

    protected:

    private:
        std::string fileName;
        std::string path; //path for where to store file data
        std::string fileType; //file type

        std::vector<person> *pList; //pointer to the main person list

        void addParticlesToVTKDataset(vtkSmartPointer<vtkPoints> &pts, vtkSmartPointer<vtkPolyData> &polyData); //handles adding metadata to particles
};

#endif // OUTPUT_H
