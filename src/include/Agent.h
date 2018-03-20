#ifndef AGENT_INCLUDED
#define AGENT_INCLUDED

#include "WorldObject.h"
#include "Vec3D.h"
#include "Node.h"

#include <vector>

class Agent : public WorldObject
{
  private:
    Vec3D vel;
    std::vector<Node*> path;

  public:
    //PUBLIC VARIABLES
    float speed = 10;

    //CONSTRUCTORS + DESTRUCTORS
    Agent();
    Agent(std::vector<Node*> p);
    ~Agent();

    //OTHERS
    void update(double dt);

};

#endif
