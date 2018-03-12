#include "Node.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Node::Node() : WorldObject()
{

}

Node::Node(Vec3D init_pos) : WorldObject(init_pos)
{

}

Node::~Node()
{
  for (int i = 0; i < neighbor_nodes.size(); i++)
  {
    delete neighbor_nodes[i];
  }
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void Node::addNeighbor(Node* n)
{
  neighbor_nodes.push_back(n);
}
