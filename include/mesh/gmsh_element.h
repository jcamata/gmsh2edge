/* 
 * File:   gmsh_element.h
 * Author: camata
 *
 * Created on April 17, 2012, 9:52 AM
 */

#ifndef _GMSH_ELEMENT_H
#define	_GMSH_ELEMENT_H

#include <vector>
#include <fstream>
using namespace std;


static int elemNumNodes[] = {2,2,3,4,4,8,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

class GmshElement
{
  public:
      GmshElement();
      void read(fstream *efile, int elemtype, int nnodes, int ntags);
      int getNumTags();
      int getNumNodes();
      std::vector<int>& getConnectivity();
      int node(int ith);
      int tag(int ith);
      int type();
      int getPhysicalID();
      GmshElement& operator=(const GmshElement& gelem);

  private:
      int element_id;
      int element_type;
      int num_tags;
      int num_nodes;
      int physicalID;
      int elementary;
      vector<int>   nodeIDs;
      vector<int>   tags;

};


#endif	/* _GMSH_ELEMENT_H */

