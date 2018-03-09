#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include "Vec3D.h"
#include "WorldObject.h"

class Node : public WorldObject
{
private:
  std::vector<Node*> neighbor_nodes;

public:
  Node();
  Node(Vec3D init_pos);

  //OTHERS
  void addNeighbor(Node* n);

};

#endif
