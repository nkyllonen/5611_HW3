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

/*----------------------------*/
// OTHERS
/*----------------------------*/
void Node::addNeighbor(Node* n)
{
  neighbor_nodes.push_back(n);
}
