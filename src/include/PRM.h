#ifndef PRM_INCLUDED
#define PRM_INCLUDED

#include "Node.h"
#include "Material.h"
#include "Vec3D.h"
#include "Util.h"
#include "CSpace.h"

#include <list>
#include <queue>

enum state
{
  DRAW_ALL,
  DRAW_PATH
};

class PRM
{
  private:
    int width = 1;
    int height = 1;
    float connection_radius_sq = 10;

    int num_nodes = 0;
  	Node** node_arr;
    float node_size = 0.25;
    int num_connections = 0;

    state draw_state = DRAW_ALL;

    //PRIVATE FUNCTIONS
    void UCS();

  public:
    //PUBLIC VARIABLES
    vector<Node*> shortest_path;

    //CONSTRUCTORS AND DESTRUCTORS
    PRM();
    PRM(int w, int h, int num);
    ~PRM();

    //SETTERS
    void setNumNodes(int n);

    //GETTERS
    int getNumNodes();

    //OTHERS
    int generateNodes(int model_start, int model_verts, CSpace* cs);
    int connectNodes(CSpace* cs);
    void drawNodes(GLuint nodeShader);
    void drawConnections(GLuint shaderProgram);
    void loadLineVertices(float* lineData);
    void buildShortest();
    void printShortest();
    int changeDrawState();

};

//other structures necessary for PRM's Uniform Cost Search
struct q_element
{
  std::list<Node*> path;
  float cost;
  bool operator> (const q_element& rhs) const
  {
    return cost > rhs.cost;
  }
};

class q_element_comparison
{
  public:
    bool operator()(q_element e1, q_element e2) const
    {
      return e1 > e2;
    }
};

#endif
