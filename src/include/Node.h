#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include "Vec3D.h"
#include "WorldObject.h"

class Node : public WorldObject
{
private:

public:
  //PUBLIC VARIABLES
  std::vector<Node*> neighbor_nodes;

  //CONSTRUCTORS + DESTRUCTORS
  Node();
  Node(Vec3D init_pos);
  ~Node();

  //OTHERS
  void addNeighbor(Node* n);

};

#endif
