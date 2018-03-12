#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include "Vec3D.h"
#include "Material.h"
#include "Util.h"

class Node
{
private:
  int start_vertex_index;	//index where vertices start in modelData array
	int total_vertices;	//total num of vertices within modelData array

public:
  //PUBLIC VARIABLES
  Vec3D pos;
  Vec3D size;
  Material mat;
  std::vector<Node*> neighbor_nodes;

  //CONSTRUCTORS + DESTRUCTORS
  Node();
  Node(Vec3D init_pos);
  ~Node();

  //SETTERS
  void setVertexInfo(int start, int total);

  //OTHERS
  void draw(GLuint shaderProgram);

};

#endif
