/* 
 * File:   main.cpp
 * Author: Jose J. Camata 
           Adriano

 *
 * Created on July 23, 2012, 9:56 AM
 */

#include <cstdlib>
#include "gmsh_reader.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <assert.h> 


using namespace std;


int build_lock(int argc, char** argv)
{

    GmshReader gmsh;
    int nnodes, nfaces, nel, nnoel, ncar, nic;  
    int bc_type; 
    char fname[256];

    list<int> noslip;
    list<int> slipy;
    list<int> slipx;
    list<int>  slipz;
    list<int> pnull;
    list<int> fluid_heavy;
    list<int> fluid_light;

    ofstream    InFile;
    ofstream    MshFile;
    PhysicalParameter PhysicalID;
   
   
    if(argc < 2){
      cout << "Mesh filename is missing!!! \n";
      cout << "Usage: " << argv[0] << " [input gmsh file .msh (binary)] [output file] " << endl;
      return 1;
    }
    else{
      cout << "Opening file: " << argv[1] << "! \n";
      gmsh.readFile(argv[1]);
      gmsh.print_info();
    }

    if( gmsh.getPhysicalIDByName("NOSLIP", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,noslip);
      cout << "Getting " << noslip.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    } 

    if( gmsh.getPhysicalIDByName("SLIPX", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,slipz);
      cout << "Getting " << slipy.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    }

    if( gmsh.getPhysicalIDByName("SLIPY", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,slipy);
      cout << "Getting " << slipy.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    }

    if( gmsh.getPhysicalIDByName("SLIPZ", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,slipz);
      cout << "Getting " << slipy.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    }

    if( gmsh.getPhysicalIDByName("PNULL", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,slipz);
      cout << "Getting " << slipy.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    }


    if( gmsh.getPhysicalIDByName("FLUID_HEAVY", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,fluid_heavy);
      cout << "Getting " << fluid_heavy.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    }

    if( gmsh.getPhysicalIDByName("FLUID_LIGHT", PhysicalID) ) {
      gmsh.getNodesMarkedByPhysicalID(PhysicalID,fluid_light);
      cout << "Getting " << fluid_light.size() << " nodes from " << PhysicalID.physical_name.c_str() << " region" << endl;
    }




    nel    = gmsh.getNumberofElements();
    nnodes = gmsh.getNumberofNodes(); 
    ncar =  noslip.size()*3     +   // NoSlip
            slipx.size()        +
            slipy.size()        + 
            slipz.size()        + 
            pnull.size()        ;

        
    nic  = fluid_light.size() + fluid_heavy.size();


   sprintf(fname,"%s_edgecfd.in",argv[2]);

   cout << " Writting EdgeCFD "<<fname<<" file... " << endl;


   InFile.open(fname, fstream::out);
   InFile.flags(ios::right | ios::scientific);
   InFile.precision(6);
  
   //LINE 1
   InFile << setw(40) << "DADOS PARA O EDGECFD" << endl;
   
   //LINE 2
   InFile << setw(10) << "4" //nnoel
          << setw(10) << nel //nel
          << setw(10) << "2" //numat
          << setw(10) << "3" //ndim
          << setw(10) << "4" //ngl
          << setw(10) << nnodes //nnos
          << setw(10) << "8" << endl;//npar
   
   //LINE 3
   InFile << setw(10) << "20" //maxbct
          << setw(10) << "2" //nbct
          << setw(10) << "1" //imet
          << setw(10) << "3" //isolver
          << setw(10) << "35" << endl; //kmax1
   
   //LINE 4        
   InFile << setw(10) << "10" //iprint
          << setw(10) << "7" //maxit
          << setw(10) << "-200" //maxslv
          << setw(10) << "0" //nansys
          << setw(10) << "1000" << endl; //ntan
   
   //LINE 5      
   InFile << setw(10) << "1" //nfun
          << setw(10) << ncar //ncar
          << setw(10) << nic << endl; //nic
   
   
   //LINE 6        
   InFile << setw(10) << "1.000e-4" //alpha
          << setw(10) << "0.100" //sigma0
          << setw(10) << "0.500" //sigma1
          << setw(10) << "0.900" //gamma
          << setw(10) << "0.100" //eta0
          << setw(10) << "1.000e-3" << endl; //etamax
   
   
   //LINE 7      
   InFile << setw(10) << "1.000e-3" //rtol
          << setw(10) << "1.000e-3" << endl; //dutol
   
   //LINE 8      
   InFile << setw(10) << "-0.050" //dt
          << setw(10) << "3.000" << endl; //tmax
   
   
   //LINE 9      
   InFile << setw(10) << "0.500" //cflmin
          << setw(10) << "1.000" //cflmax
          << setw(10) << "0.200" << endl; //tol_v
   
   //LINE 10      
   InFile << setw(10) << "2" //nptf
          << setw(10) << "CONSTANTE" << endl; //titfun(ifun)
   
   //TIME FUNCTION     
   InFile << setw(10) << "0.000e+0" //tf
          << setw(10) << "1.000" << endl; //val
        
   InFile << setw(10) << "1.000e+3" //tf
          << setw(10) << "1.000" << endl; //val


   // Aplicando condições de contorno
   list<int>::iterator iter;
 
   int lp = 0;

   for( iter = noslip.begin(); iter != noslip.end(); iter++)
   {
     int node = *(iter);
     InFile <<setw(10)<< node << setw(7)  << -1 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
     InFile <<setw(10)<< node << setw(7)  << -2 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
     InFile <<setw(10)<< node << setw(7)  << -3 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
   }

   for( iter = slipx.begin(); iter != slipx.end(); iter++)
   {
     int node = *(iter);
     InFile <<setw(10)<< node << setw(7)  << -1 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
   }

   for( iter = slipy.begin(); iter != slipy.end(); iter++)
   {
     int node = *(iter);
     InFile <<setw(10)<< node << setw(7)  << -2 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
   }

   for( iter = slipz.begin(); iter != slipz.end(); iter++)
   {
     int node = *(iter);
     InFile <<setw(10)<< node << setw(7)  << -3 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
   }

   for( iter = pnull.begin(); iter != pnull.end(); iter++)
   {
     int node = *(iter);
     InFile <<setw(10)<< node << setw(7)  << -4 << setw(7)   << 1  << setw(7) << 0 << setw(15) << 0.0 << endl; lp++;
   }


    for( iter = fluid_heavy.begin() ; iter != fluid_heavy.end() ; iter++ )
    {
       
       int node = *(iter);
       
       InFile << setw(10) << node 
              << setw(7)  << 5 
              << setw(15) << 1.0 << endl;    
    }
   
      //SCALAR IC
   for( iter = fluid_light.begin() ; iter != fluid_light.end() ; iter++ )
   {
       
       int node = *(iter);
       
       InFile << setw(10) << node 
              << setw(7)  << 5 
              << setw(15) << 0.0 << endl;    
   }

  InFile.close();

  

  sprintf(fname,"%s_edgecfd.msh", argv[2]);

  cout << " Writting EdgeCFD "<<fname<<" file... " << endl;
  MshFile.open(fname, fstream::out);
  MshFile.flags(ios::right | ios::scientific);
  MshFile.precision(10);
    for(int node = 0; node < nnodes; node++){
       MshFile << setw(20) << gmsh.getX(node)
               << setw(20) << gmsh.getY(node)
               << setw(20) << gmsh.getZ(node) << endl;
   }
   
   for(int iel=0; iel < nel; iel++){
       
       vector<int> connectivity = gmsh.getElementConnectivity(iel);
       
       for(int i=0; i < connectivity.size() ; i++)
           MshFile << setw(10) << connectivity[i];
       
       MshFile << setw(10) << 1 << endl;
       
   }

   MshFile.close();


   return 0;

}




int main(int argc, char** argv)
{
	return build_lock(argc, argv);
}
