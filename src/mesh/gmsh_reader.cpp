

#include "gmsh_reader.h"
#include <iostream>
#include <string>
#include <list>
#include <cstdlib>
using namespace std;

GmshReader::GmshReader()
{
    this->n_nodes = 0;
    this->n_elements  = 0;
}


int GmshReader::readFile(const char* filename)
{
    string beginSection;
    this->gmsh_file.open(filename, ios::in | ios::binary);
    if(!this->gmsh_file.is_open())
    {
       cerr<<"ERROR: GMSH file, "<< string(filename)
           <<", cannot be opened. Does it exist? Have you read permission?\n";
       exit(-1);
    }

    read_header();
    this->gmsh_file >> beginSection;
    // Skip newline character
    char newLineChar[2];
    this->gmsh_file.read( newLineChar, 1 );

    if (beginSection == string("$PhysicalNames"))
    {
       read_physical_names();
       
       this->gmsh_file >> beginSection;

    }
    else if( beginSection != "$Nodes" ) {
       cerr << "Can't find $Nodes tag\n";
       exit(1);
    }
    
    read_nodes_bin();
    read_elements_bin();

    this->gmsh_file.close();
            
}

void GmshReader::print_info()
{
           //01234567890123456789012345678901234567890123456789
    cout << "--------------------------------------------------" << endl
         << "  GmshReader: Information Board                   " << endl
         << "--------------------------------------------------" << endl
         << "n-dim      = " << this->n_dim        << endl
         << "n-elements = " << this->n_elements   << endl
         << "n-faces    = " << this->n_faces      << endl
         << "n-nodes    = " << this->n_nodes      << endl
         << "--------------------------------------------------" << endl;
     cout<< " Physical Parameters: " << endl;
     for(int i = 0; i < this->PhysicalNames.size(); i++) {
       cout <<  " dim = " <<  this->PhysicalNames[i].physical_dim
            <<  "  number =  "   << this->PhysicalNames[i].physical_number
            <<  "  " <<  this->PhysicalNames[i].physical_name << endl; 
     } 
}





void GmshReader::getElementMarkedByPhysicalID(PhysicalParameter &id, std::vector<int> &elem)
{
    int start;
    int num_elem = 0;
	if( id.physical_number < 0 || id.physical_number > this->PhysicalNames.size() )
    {
        cout << "Invalid Physical parameter " << endl;
        return;
    }

	if (id.physical_dim == 2 ) return;

    for(int i = this->n_faces; i < this->n_total_elements; i++)
    {
		if(this->elements[i].getPhysicalID() == id.physical_number)
        {
            elem.push_back(i);
        }
    }

}

void GmshReader::getFacesMarkedByPhysicalID(PhysicalParameter &id, std::vector<int> &elem)
{

    int start;
    if(  id.physical_number < 0 || id.physical_number > this->PhysicalNames.size() )
    {
        cout << "Invalid Physical parameter " << endl;
        return;
    }

	if (id.physical_dim == 3 ) return;

    for(int i = 0; i < this->n_faces; i++)
    {
        if(this->elements[i].getPhysicalID() == id.physical_number)
        {
            elem.push_back(i);
        }
    }

}

void GmshReader::getNodesMarkedByPhysicalID(PhysicalParameter &id, std::list<int>& nodeSet)
{
    if(id.physical_number < 0 || id.physical_number > this->PhysicalNames.size())
    {
        cout << "Invalid Physical parameter " << endl;
        return;
    }

	if( id.physical_dim == 2 )
	{

		for(int i = 0; i < this->n_faces; i++)
		   if(this->elements[i].getPhysicalID() == id.physical_number) 
               for(int j=0; j < this->elements[i].getNumNodes(); j++)
                 nodeSet.push_back(this->elements[i].node(j));
	}
    else
	{

		for(int i = this->n_faces; i < this->n_total_elements; i++)
			if(this->elements[i].getPhysicalID() == id.physical_number) 
				for(int j=0; j < this->elements[i].getNumNodes(); j++)
					nodeSet.push_back(this->elements[i].node(j));
    }

    nodeSet.sort();
    nodeSet.unique();

}


std::vector<int>& GmshReader::getElementConnectivity(int iel)
{
	return this->elements[iel+this->n_faces].getConnectivity();
}

std::vector<int>& GmshReader::getFaceConnectivity(int iel)
{
	return this->elements[iel].getConnectivity();
}


double GmshReader::getX(int node_id)
{
    return this->coords[node_id*3];
}

double GmshReader::getY(int node_id)
{
    return this->coords[node_id*3+1];
}

double GmshReader::getZ(int node_id)
{
    return this->coords[node_id*3+2];
}


void GmshReader::read_physical_names()
{
    int num_physical;
    char sectionTag[80];
    char newLineChar[2]="\n";
    this->gmsh_file >> num_physical ;
    this->gmsh_file.read( newLineChar, 1 );
    this->PhysicalNames.resize(num_physical);
    for(int i = 0; i < num_physical; i++) {
         this->gmsh_file >> this->PhysicalNames[i].physical_dim
                         >> this->PhysicalNames[i].physical_number
                         >> this->PhysicalNames[i].physical_name ;
         this->gmsh_file.read( newLineChar, 1 );
    }

    this->gmsh_file.getline( sectionTag, sizeof("$EndPhysicalNames") );
    this->gmsh_file.read( newLineChar, 1 );

}


void GmshReader::read_header()
{
    int errorCode=1;
    string beginTag;
    int doubleFloatLen;

    this->gmsh_file >> beginTag >> this->gmsh_version >> this->gmsh_format >> doubleFloatLen;

     if( beginTag != "$MeshFormat" ) {
        cerr << "Can't find $MeshFormat tag\n";
        exit(errorCode);
     }

    if( this->gmsh_version < 2 || this->gmsh_version >= 3 ) {
        cerr << "Currently only GMSH format v2.x is supported\n";
        exit(errorCode);
    }

    if( doubleFloatLen != sizeof(double) ) {
        cerr << "Double float size in GMSH file differs from system double\n";
        exit(errorCode);
    }

    // For performance reasons only binary GMSH files are permitted with fldecomp
    if( this->gmsh_version == 0 ) {
       cerr << "** GMSH ASCII files are verboten:\n"
            << "** please use 'gmsh -bin' to generate binary format.\n";
       exit(errorCode);
    }

    // Skip newline character
    char newLineChar[2];
    this->gmsh_file.read( newLineChar, 1 );

     // Read in integer '1' written in binary format: check for endianness
     int oneInt;
     this->gmsh_file.read( (char *)&oneInt, sizeof(int) );
     this->gmsh_file.read( newLineChar, 1 );


     /*if( oneInt != 1 ){
        cerr << "** Oh dear: internal and file binary formats for integers differ (endianness?)\n";
        exit(errorCode);
     }*/


     char sectionTag[80];
     this->gmsh_file.getline( sectionTag, sizeof("$EndMeshFormat") );

     // Check for end of formatting section
     if( string(sectionTag) != "$EndMeshFormat" ) {
         cerr << "Can't find $EndMeshFormat tag\n";
         exit(errorCode);
     }
     
}

void GmshReader::read_nodes_bin()
{
    int errorCode=1;
    char sectionTag[80];
    char newLineChar[2]="\n";

    // Start of nodes section
    //

    this->gmsh_file >> this->n_nodes;
    this->gmsh_file.read( newLineChar, 1 );

    // Read in binary node data
    this->nodeIDs.resize(this->n_nodes);
    this->coords.resize(this->n_nodes*3);

    for(int n=0; n<this->n_nodes; n++)
    {
      this->gmsh_file.read( (char *)&this->nodeIDs[n], sizeof(int) );
      this->gmsh_file.read( (char *)&this->coords[n*3], sizeof(double) );
      this->gmsh_file.read( (char *)&this->coords[n*3+1], sizeof(double) );
      this->gmsh_file.read( (char *)&this->coords[n*3+2], sizeof(double) );
    }

    this->gmsh_file.read( newLineChar, 1 );


    // End of nodes section
    this->gmsh_file.getline( sectionTag, sizeof("$EndNodes") );

    if( string(sectionTag) != "$EndNodes" )
    {
        cerr << "Can't find $EndNodes tag\n";
        exit(errorCode);
    }

}


void GmshReader::read_elements_bin()
{
    int errorCode=1;
    char sectionTag[80];
    char newLineChar[2]="\n";
    int elemType;
    int numBlockElems;
    int numTags;

    // Elements section
    this->gmsh_file.getline( sectionTag, sizeof("$Elements") );


    if( string(sectionTag) != "$Elements" ){
        cerr << "Can't find $Elements tag\n";
        exit(errorCode);
    }

    this->gmsh_file >> this->n_total_elements;

    this->gmsh_file.read( newLineChar, 1 );

    int eIx=0;

    // Element array
    this->elements.resize(this->n_total_elements);

    int numEdges=0, numTriangles=0, numQuads=0, numTets=0, numHexes=0;

    while( eIx < this->n_total_elements )
    {
        this->gmsh_file.read( (char *)&elemType, sizeof(int) );
        this->gmsh_file.read( (char *)&numBlockElems, sizeof(int) );
        this->gmsh_file.read( (char *)&numTags, sizeof(int) );

        // Do we know about this type of element?
        if( elemNumNodes[elemType] == -1 )
        {
            cerr << "Element type not supported by fldecomp\n";
            exit(errorCode);
        }

        // Read in all the elements of this type
        for(int e=0; e<numBlockElems; e++)
        {
            this->elements[eIx].read(&this->gmsh_file, elemType, elemNumNodes[elemType], numTags);

            // Here, we count up the number of different types of elements.
            // This allows us to decide what are faces, and what are internal
            // elements.
          switch( elemType )
            {
            case 1:
              numEdges++;
              break;
            case 2:
              numTriangles++;
              break;
            case 3:
              numQuads++;
              break;
            case 4:
              numTets++;
              break;
            case 5:
              numHexes++;
              break;
            case 15:
              break;
            default:
              cerr << "Unsupported element type in GMSH mesh\n";
              cerr << "type: "<< elemType << "\n";
              exit(errorCode);
              break;
            }

          eIx++;
        }
    }

  this->gmsh_file.read( newLineChar, 1 );

  // End of Elements section
  this->gmsh_file.getline( sectionTag, sizeof("$EndElements") );


  if( string(sectionTag) != "$EndElements" ) {
      cerr << "Can't find $EndElements tag\n";
      exit(errorCode);
    }

  // Make some calculations based on the different types of elements
  // collected.

  if(numTets>0) {
    this->n_faces   = numTriangles;
    this->face_type = 2;
    this->elem_type = 4;
    this->n_dim     = 3;

  } else if(numTriangles>0) {
    this->n_faces   = numEdges;
    this->face_type = 1;
    this->elem_type = 2;
    this->n_dim  = 2;

  } else if(numHexes>0) {
    this->n_faces   = numQuads;
    this->face_type = 3;
    this->elem_type = 5;
    this->n_dim = 3;

  } else if(numQuads>0) {
    this->n_faces = numEdges;
    this->face_type = 1;
    this->elem_type = 3;
    this->n_dim = 2;

  } else {
       cerr << "Unsupported mixture of face/element types\n";
       exit(errorCode);

  }

  // Set some handy variables to be used elsewhere
  this->n_elements = this->n_total_elements - this->n_faces;
  this->n_noel = elemNumNodes[this->elem_type];
  this->n_nodes_per_face = elemNumNodes[this->face_type];

}



bool GmshReader::getPhysicalIDByName (const char* PhysicalName, PhysicalParameter& id){
    

    for(int i = 0; i < this->PhysicalNames.size(); i++) {

       if (  this->PhysicalNames[i].physical_name == ("\"" + string(PhysicalName) + "\"") ){

            id = this->PhysicalNames[i];
			return true;
                 
       }

     }
   
     return false;

}

int  GmshReader::getNumberofNodes(){
     return this->n_nodes;
}

int  GmshReader::getNumberofElements(){
     return this->n_elements;
}

int  GmshReader::getNumberofFaces(){
     return this->n_faces;
}
