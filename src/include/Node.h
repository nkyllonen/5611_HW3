#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include "Vec3D.h"
#include "Material.h"
#include "Util.h"

//forward declaration of struct
struct link_t;

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
  std::vector<link_t> neighbor_nodes;

  //CONSTRUCTORS + DESTRUCTORS
  Node();
  Node(Vec3D init_pos);
  ~Node();

  //SETTERS
  void setVertexInfo(int start, int total);

  //OTHERS
  void draw(GLuint shaderProgram);

  //OVERLOADED OPERATORS
  friend bool operator== (Node n1, Node n2);

};

struct link_t
{
  //constructors
  link_t() : length_sq(-1), node(nullptr) {};
  link_t(float len, Node* n) : length_sq(len), node(n) {};

  float length_sq;
  Node* node;
};

#endif
