/* 
 * File:   gmsh_reader.h
 * Author: camata
 *
 * Created on April 17, 2012, 9:50 AM
 */

#ifndef _GMSH_READER_H
#define	_GMSH_READER_H

#include <list>

#include "gmsh_element.h"

typedef struct 
{
    int    physical_dim;
    int    physical_number;
    string physical_name;
} PhysicalParameter;


class GmshReader 
{
    public:
       GmshReader();
       int  readFile(const char* filename);
       void print_info();
       void getFacesMarkedByPhysicalID  (PhysicalParameter &id, std::vector<int> &FaceSet);
       void getElementMarkedByPhysicalID(PhysicalParameter &id, std::vector<int> &ElementSet);
       void getNodesMarkedByPhysicalID  (PhysicalParameter &id, std::list<int>   &nodeSet);
       int  getNumberofNodes();
       int  getNumberofElements();
       int  getNumberofFaces();
       bool  getPhysicalIDByName (const char* PhysicalName, PhysicalParameter& id);
       std::vector<int>& getElementConnectivity(int iel);
	   std::vector<int>& getFaceConnectivity(int iel);
       double           getX(int node_id);
       double           getY(int node_id);
       double           getZ(int node_id);

    
    private:
        // private methods
        void read_header();
        void read_nodes_bin();
        void read_elements_bin();
        void read_physical_names();

        // private attributes
        fstream              gmsh_file;
        float                gmsh_version;
        int                  gmsh_format;
        int                  n_nodes;
        int                  n_elements;
        vector<int>          nodeIDs;
        vector<double>       coords;
        vector<GmshElement>  elements;
        vector<PhysicalParameter> PhysicalNames;
        int                  face_type;
        int                  elem_type;
        int                  n_faces;
        int                  n_total_elements;
        int                  n_dim;
        int                  n_noel;
        int                  n_nodes_per_face;

};



#endif	/* _GMSH_READER_H */

