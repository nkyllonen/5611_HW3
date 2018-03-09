#ifndef PRM_INCLUDED
#define PRM_INCLUDED

#include "WorldObject.h"
#include "Node.h"
#include "Material.h"

class PRM
{
  private:
    int width = 1;
    int height = 1;

    int num_nodes = 0;
  	WorldObject** node_arr;
    float node_size = 0.5;

  public:
    PRM();
    PRM(int w, int h, int num);
    ~PRM();

    //SETTERS
    void setNumber(int n);

    //GETTERS
    int getNumber();

    //OTHERS
    void generateNodes(int model_start, int model_verts);   //later pass in ptr to CSpace obj?
    void connectNodes();    //later pass in ptr to CSpace obj?
    void draw(GLuint shaderProgram);

};

#endif
