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
  node_arr = new Node*[num_nodes];
  cout << "Allocated node_arr to length " << num_nodes << endl;
  connection_radius_sq = w; //so we get everything connecting to each other
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
void PRM::setNumNodes(int n)
{
  num_nodes = n;
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
int PRM::getNumNodes()
{
  return num_nodes;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
int PRM::generateNodes(int model_start, int model_verts, CSpace* cs)
{
  Vec3D size = Vec3D(node_size, node_size, node_size);
	Vec3D pos = Vec3D();
	Vec3D start_color = Vec3D(0,1,0);
	Vec3D goal_color = Vec3D(1,0,0);
	Vec3D other_color = Vec3D(0,0,0.5);

  Material mat = Material();
	mat.setSpecular(glm::vec3(0, 0, 0));

	float x = 0, y = 0, z = 0;
  Node* temp;

	//place start and goal across from each other
	pos = Vec3D(-width/2 + (double)rand()/RAND_MAX + node_size, -height/2 + (double)rand()/RAND_MAX + node_size, z);
	temp = new Node(pos);
	temp->size = 2*size;
	temp->setVertexInfo(model_start, model_verts);

  mat.setAmbient(util::vec3DtoGLM(start_color));
  mat.setDiffuse(util::vec3DtoGLM(start_color));
	temp->mat = mat;
	node_arr[0] = temp;

	pos = Vec3D(width/2 - (double)rand()/RAND_MAX - node_size, height/2 - (double)rand()/RAND_MAX - node_size, z);
	temp = new Node(pos);
  temp->size = 2*size;
	temp->setVertexInfo(model_start, model_verts);

  mat.setAmbient(util::vec3DtoGLM(goal_color));
  mat.setDiffuse(util::vec3DtoGLM(goal_color));
	temp->mat = mat;
	node_arr[num_nodes-1] = temp;

  //set color for other nodes
  mat.setAmbient(util::vec3DtoGLM(other_color));
  mat.setDiffuse(util::vec3DtoGLM(other_color));

	for (int i = 1; i < num_nodes-1; i++)
	{
    //loop while checking if generated position is not valid
    do
    {
      //subtract node_size so that they won't go over the edge of the area
      x = ((double)rand()/RAND_MAX)*(width - node_size) - (width/2.0);
      y = ((double)rand()/RAND_MAX)*(height - node_size) - (height/2.0);
      pos = Vec3D(x,y,z);
    } while(!cs->isValidPosition(pos, node_size/2.0));

		temp = new Node(pos);

    temp->size = size;
  	temp->setVertexInfo(model_start, model_verts);
  	temp->mat = mat;

		node_arr[i] = temp;
	}

  return num_nodes;
}

int PRM::connectNodes(CSpace* cs)
{
  Vec3D dist;
  float len_sq = 0;

  for (int i = 0; i < num_nodes; i++)
  {
    //compare node_arr[i] against other nodes
    for (int k = 0; k < num_nodes; k++)
    {
      if (k != i) //don't check against itself
      {
        dist = node_arr[i]->pos - node_arr[k]->pos; //dist: k --> i
        len_sq = dotProduct(dist, dist);

        if (len_sq <= connection_radius_sq) //close enough to connect
        {
          //figure out if path connecting nodes is valid in CSpace
          if (cs->isValidSegment(dist, node_arr[k]->pos, node_size/2.0))
          {
            link_t l = link_t(len_sq, node_arr[k]); //create new link_t holding length sq'd and Node*
            node_arr[i]->neighbor_nodes.push_back(l);
            num_connections++;
          }
        }
      }
    }//END for-k
  }//END for-i

  return num_connections;
}//END connectNodes

void PRM::drawNodes(GLuint nodeShader)
{
  for (int i = 0; i < num_nodes; i++)
  {
    node_arr[i]->draw(nodeShader);
  }
}

void PRM::drawConnections(GLuint shaderProgram)
{
  glm::vec3 color = glm::vec3(0.5,0.5,0.5);

  //fragment shader uniform -> color
	GLint uniform_color = glGetUniformLocation(shaderProgram, "color");
  glUniform3f(uniform_color, color[0], color[1], color[2]);

  glLineWidth(2);
	glDrawArrays(GL_LINES, 0, num_connections * 2);
}

/*--------------------------------------------------------------*/
// loadLineVertices : loop through nodes and connections
//											and plug positions into lineData
/*--------------------------------------------------------------*/
void PRM::loadLineVertices(float* lineData)
{
	int count = 0, i_connections = 0;
	Vec3D pi;
	Vec3D pii;
	vector<link_t> neighbors;

	for (int i = 0; i < num_nodes; i++)
	{
		//go through each node's list of connections
		neighbors = node_arr[i]->neighbor_nodes;
		i_connections = neighbors.size();
		pi = node_arr[i]->pos;

		for (int c = 0; c < i_connections; c++)
		{
			pii = neighbors[c].node->pos;

			util::loadVecValues(lineData, pi, count);
			util::loadVecValues(lineData, pii, count);
		}
	}

  cout << "LineData values loaded" << endl;
}//END loadLineVertices

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/
link_t PRM::getShortest(link_t cur_link)
{
  Node* cur_node = cur_link.node;
  int num = cur_node->neighbor_nodes.size();
  int min_i = 0, min_len = width*width;

  //base case 1 : we're at the goal
  if (*cur_node == *node_arr[num_nodes-1]) return cur_link;

  //base case 2 : we only have one neighbor (i.e. the one we came from)
  if (num == 1) return link_t();

  //loop over neighbors and find shortest
  for (int i = 0; i < num; i++)
  {
    if (cur_node->neighbor_nodes[i].length_sq < min_len)
    {
      min_i = i;
      min_len = cur_node->neighbor_nodes[i].length_sq;
    }
  }

  //construct shortest link_t
  link_t min_link = link_t(min_len, node_arr[min_i]);
  link_t shortest = getShortest(min_link);
  if (shortest.node == nullptr)
  {
    //try again with a random connection
    int rand_i = -1;
    while (rand_i == min_i)
    {
      rand_i = rand()%num;
    }

    min_link = cur_node->neighbor_nodes[rand_i];
    shortest = getShortest(min_link);
  }

  path.push_back(shortest);
}
