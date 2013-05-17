
#include "gmsh_element.h"

GmshElement::GmshElement()
{
    this->element_id = -1;
    this->element_type = 0;
    this->elementary = -1;
    this->physicalID = -1;
}

int GmshElement::getPhysicalID()
{
    return this->physicalID;
}

void GmshElement::read(fstream* efile, int elemtype, int nnodes, int ntags)
{
    int i,j;
    this->element_type = elemtype;
    this->num_nodes  = nnodes;
    this->num_tags   = ntags;

    this->nodeIDs.resize(this->num_nodes);
    this->tags.resize(this->num_tags);

    // reading element id from gmsh file
    efile->read( (char*)&this->element_id, sizeof(int));
    for(i=0; i < this->num_tags; i++)
        efile->read((char*)&this->tags[i], sizeof(int));
    for(i=0; i < this->num_nodes; i++)
        efile->read((char*)&this->nodeIDs[i], sizeof(int));

    if(this->num_tags >= 2 )
    {
        this->physicalID = this->tags[0];
        this->elementary = this->tags[1];
    }
    
}

int GmshElement::getNumNodes()
{
    return this->num_nodes;
}

int GmshElement::getNumTags()
{
    return this->num_tags;
}

int GmshElement::node(int ith)
{
    return this->nodeIDs[ith];
}

int GmshElement::tag(int ith)
{
    return this->tags[ith];
}

int GmshElement::type(){
    return this->element_type;
}


GmshElement& GmshElement::operator=(const GmshElement& gelem)
{
    this->element_id   = gelem.element_id;
    this->element_type = gelem.element_type;
    this->elementary   = gelem.elementary;
    //this->nodeIDs     = gelem.nodeIDs;
    this->num_nodes    = gelem.num_nodes;
    this->num_tags     = gelem.num_tags;
    this->physicalID   = gelem.physicalID;
    //this->tags         = gelem.tags;
    return *this;
}


std::vector<int>& GmshElement::getConnectivity()
{
    return this->nodeIDs;
}
