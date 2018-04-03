#ifndef AGENT_INCLUDED
#define AGENT_INCLUDED

#include "WorldObject.h"
#include "Vec3D.h"
#include "Node.h"
#include "PRM.h"

#include <vector>

class Agent : public WorldObject
{
  private:
    std::vector<Node*> path;

  public:
    //PUBLIC VARIABLES
    Vec3D vel;
    float speed = 10;
    Node* start_node;
    Node* goal_node;

    //CONSTRUCTORS + DESTRUCTORS
    Agent();
    Agent(Node* s, Node* g);
    ~Agent();

    //OTHERS
    void update(double dt);
    void calcPath(PRM* myPRM, CSpace* cs);
    void drawPath(GLuint nodeShader);

};

#endif
