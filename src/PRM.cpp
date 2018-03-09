#include "PRM.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
PRM::PRM()
{
  num_nodes = 0;
}

PRM::PRM(int w, int h, int num)
{
  width = w;
  height = h;
  num_nodes = num;
  node_arr = new WorldObject*[num_nodes];
  cout << "Allocated node_arr to length " << num_nodes << endl;
}

PRM::~PRM()
{
  //delete each ptr in
	for (int i = 0; i < num_nodes; i++)
	{
		delete node_arr[i];
	}

	delete[] node_arr;
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void PRM::setNumber(int n)
{
  num_nodes = n;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
int PRM::getNumber()
{
  return num_nodes;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void PRM::generateNodes(int model_start, int model_verts)
{
  Vec3D size = Vec3D(node_size, node_size, node_size);
	Vec3D pos = Vec3D();
	Vec3D start_color = Vec3D(0,1,0);
	Vec3D goal_color = Vec3D(1,0,0);
	Vec3D other_color = Vec3D(0,0,0.5);

  Material mat = Material();
	mat.setAmbient(glm::vec3(0.7, 0.7, 0.7));
	mat.setDiffuse(glm::vec3(0.7, 0.7, 0.7));
	mat.setSpecular(glm::vec3(0, 0, 0));

	WorldObject* temp;

	float x = 0, y = 0, z = 0;

	//place start and goal across from each other
	pos = Vec3D(-width/2 + (double)rand()/RAND_MAX + node_size, -height/2 + (double)rand()/RAND_MAX + node_size, z);
	temp = new WorldObject(pos);
	temp->setSize(size);
	temp->setVertexInfo(model_start, model_verts);
	temp->setMaterial(mat);//use same Material as floor
	temp->setColor(start_color);
	node_arr[0] = temp;

	pos = Vec3D(width/2 - (double)rand()/RAND_MAX - node_size, height/2 - (double)rand()/RAND_MAX - node_size, z);
	temp = new WorldObject(pos);
	temp->setSize(size);
	temp->setVertexInfo(model_start, model_verts);
	temp->setMaterial(mat);//use same Material as floor
	temp->setColor(goal_color);
	node_arr[num_nodes-1] = temp;

	for (int i = 1; i < num_nodes-1; i++)
	{
    //subtract node_size so that they won't go over the edge of the area
		x = ((double)rand()/RAND_MAX)*(width - node_size) - (width/2.0);
		y = ((double)rand()/RAND_MAX)*(height - node_size) - (height/2.0);
		pos = Vec3D(x,y,z);

		temp = new WorldObject(pos);

		temp->setSize(size);
		temp->setVertexInfo(model_start, model_verts);
		temp->setMaterial(mat);
		temp->setColor(other_color);

		node_arr[i] = temp;
	}
}

void PRM::connectNodes()
{

}

void PRM::draw(GLuint shaderProgram)
{
  for (int i = 0; i < num_nodes; i++)
	{
		node_arr[i]->draw(shaderProgram);
	}
}
