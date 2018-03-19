#include "PRM.h"

using namespace std;

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
  num_nodes = num + 2;                //+2 for strart and goal
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
        dist = node_arr[k]->pos - node_arr[i]->pos; //dist: i --> k
        len_sq = dotProduct(dist, dist);

        if (len_sq <= connection_radius_sq) //close enough to connect
        {
          //figure out if path connecting nodes is valid in CSpace
          if (cs->isValidSegment(dist, node_arr[i]->pos, node_size/2.0))
          {
            link_t l = link_t(len_sq, node_arr[k]); //create new link_t holding length sq'd and Node*
            node_arr[i]->neighbor_nodes.push_back(l);
            num_connections++;
          }
        }
      }//END if k != i
    }//END for-k
  }//END for-i

  return num_connections;
}//END connectNodes

void PRM::drawNodes(GLuint nodeShader)
{
  if (draw_state == DRAW_ALL)
  {
    for (int i = 0; i < num_nodes; i++)
    {
      node_arr[i]->draw(nodeShader);
    }
  }
  else if (draw_state == DRAW_PATH)
  {
    int num = shortest_path.size();

    for (int i = 0; i < num; i++)
    {
      shortest_path[i]->draw(nodeShader);
    }
  }
}

void PRM::drawConnections(GLuint shaderProgram)
{
  glm::vec3 color = glm::vec3(0.5,0.5,0.5);

  //fragment shader uniform -> color
	GLint uniform_color = glGetUniformLocation(shaderProgram, "color");
  glUniform3f(uniform_color, color[0], color[1], color[2]);

  glLineWidth(2);
	if (draw_state == DRAW_ALL) glDrawArrays(GL_LINES, 0, num_connections * 2); //*2 since 2 vertices per connection?
  else if (draw_state == DRAW_PATH) glDrawArrays(GL_LINES, 0, shortest_path.size() * 2);
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

  if (draw_state == DRAW_ALL)
  {
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
  }
  else if (draw_state == DRAW_PATH)
  {
    int num = shortest_path.size();

  	for (int i = 0; i < num-1; i++)
  	{
  		pi = shortest_path[i]->pos;
      pii = shortest_path[i+1]->pos;

      util::loadVecValues(lineData, pi, count);
      util::loadVecValues(lineData, pii, count);
  	}
  }

  cout << "LineData values loaded\n" << endl;
}//END loadLineVertices

void PRM::buildShortest()
{
  //leave room for other path finding algorithms?
  UCS();
}

void PRM::printShortest()
{
  int num = shortest_path.size();

  for (int i = 0; i < num; i++)
  {
    printf("path[%i] at ", i);
    shortest_path[i]->pos.print();
  }
}

int PRM::changeDrawState()
{
  int num_lines = 0;

  if (draw_state == DRAW_ALL)
  {
    cout << "\n--->Drawing path only" << endl;
    draw_state = DRAW_PATH;
    num_lines = shortest_path.size();
  }
  else if (draw_state == DRAW_PATH)
  {
    cout << "\n--->Drawing all connections" << endl;
    draw_state = DRAW_ALL;
    num_lines = num_connections;

  }

  return num_lines;
}

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/
//this implementation is largely inspired/copied from:
//  https://www.snip2code.com/Snippet/1017813/Uniform-Cost-Search-Algorithm-C---Implem
void PRM::UCS()
{
  Node* cur_node;
  Node* cur_neighbor;
  int pos = 0, num = 0;
  priority_queue <q_element, vector<q_element>, q_element_comparison> PQ;

  Node* start_node = node_arr[0];
  Node* goal_node = node_arr[num_nodes-1];

  cout << "start node at : ";
  start_node->pos.print();
  cout << "goal node at : ";
  goal_node->pos.print();

  start_node->visited = true;            //mark start has having been visited

  //1. add starting node to PQ
  q_element vstart;
  vstart.path.push_back(start_node);
  vstart.cost = 0;
  PQ.push(vstart);

  //2. loop until we finish
  while (!PQ.empty())
  {
    q_element cur_el, temp_el;
    cur_el = PQ.top();                  //hold onto maximum priority element
    cur_node = cur_el.path.back();      //access last element in cur_el's list of Nodes

    PQ.pop();                           //"dequeue the maximum priority element from the queue"

    cur_node->visited = true;

    //check if we're at the goal
    if (cur_node == goal_node)
    {
      cout << "\n+++++Goal reached!+++++" << endl;

      //need to add path to shortest_path
      list<Node*> cur_el_path = cur_el.path;
      num = cur_el_path.size();
      Node* temp_node;

      for (int i = 0; i < num; i++)
      {
        temp_node = cur_el_path.front();   //grab very first element
        shortest_path.push_back(temp_node);
        cur_el_path.pop_front();           //remove first element from list
      }

      return;
    }
    else
    {
      //loop through neighbors of cur_el
      //--> "insert all the children of the dequeued element, with the cumulative costs as priority"
      num = cur_node->neighbor_nodes.size();

      //cout << "current cost: " << cur_el.cost << " num neighbors: " << num << endl;

      for (int i = 0; i < num; i++)
      {
        cur_neighbor = cur_node->neighbor_nodes[i].node;

        //check if we've visit this node before
        if (!cur_neighbor->visited)
        {
          //cur_neighbor->visited = true;
          temp_el = cur_el;

          //extend the paths in the PQ to include cur_node's neighbors
          temp_el.path.push_back(cur_neighbor);
          temp_el.cost += cur_node->neighbor_nodes[i].length_sq;

          //push extended element onto PQ
          PQ.push(temp_el);
        }
        else
        {
          //cout << "already visited this neighbor" << endl;
        }
      }
    }
  }//END while !empty

  cout << "++PQ determined to be empty++" << endl;
}
