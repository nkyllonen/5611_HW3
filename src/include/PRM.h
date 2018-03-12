#ifndef PRM_INCLUDED
#define PRM_INCLUDED

#include "WorldObject.h"
#include "Node.h"
#include "Material.h"
#include "Vec3D.h"

class PRM
{
  private:
    int width = 1;
    int height = 1;
    float connection_radius_sq = 4;

    int num_nodes = 0;
  	WorldObject** node_arr;
    float node_size = 0.5;
    int num_connections = 0;

  public:
    //PUBLIC VARIABLES

    //CONSTRUCTORS AND DESTRUCTORS
    PRM();
    PRM(int w, int h, int num);
    ~PRM();

    //SETTERS
    void setNumNodes(int n);

    //GETTERS
    int getNumNodes();

    //OTHERS
    int generateNodes(int model_start, int model_verts);    //later pass in ptr to CSpace obj?
    int connectNodes();                                     //later pass in ptr to CSpace obj?
    void drawNodes(GLuint nodeShader);
    void drawConnections();

};

#endif
