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
  num_nodes = num;
  //node_vec = new Node*[num_nodes];
  //cout << "Allocated node_vec to length " << num_nodes << endl;
  connection_radius_sq = pow(w, 1.5); //so we get everything connecting to each other
}

PRM::~PRM()
{
  //delete each ptr in
	for (int i = 0; i < node_vec.size(); i++)
	{
		delete node_vec[i];
	}
}

/*----------------------------*/
// SETTERS
/*----------------------------*/

/*----------------------------*/
// GETTERS
/*----------------------------*/
Vec3D PRM::getRandPosition()
{
  float x = 0, y = 0, z = 0;
  x = ((double)rand()/RAND_MAX)*(width - agent_size) - (width/2.0);
  y = ((double)rand()/RAND_MAX)*(height - agent_size) - (height/2.0);
  z = ((double)rand()/RAND_MAX)*(height - agent_size) - (height/2.0);

  return Vec3D(x,y,z);
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
int PRM::generateNodes(int model_start, int model_verts, CSpace* cs)
{
  Vec3D size = Vec3D(node_size, node_size, node_size);
	Vec3D pos = Vec3D();
	Vec3D other_color = Vec3D(0,0,0.5);

  Material mat = Material();
	mat.setSpecular(glm::vec3(0, 0, 0));

  Node* temp;

  //set color for in between nodes
  mat.setAmbient(util::vec3DtoGLM(other_color));
  mat.setDiffuse(util::vec3DtoGLM(other_color));

	for (int i = 0; i < num_nodes; i++)
	{
    //loop while checking if generated position is not valid
    do
    {
      //subtract node_size so that they won't go over the edge of the area
      pos = getRandPosition();
      pos.z = 0;
    } while(cs->isValidPosition(pos, agent_size) != -1); //-1 = no collision

		temp = new Node(pos);

    temp->size = size;
  	temp->setVertexInfo(model_start, model_verts);
  	temp->mat = mat;

		node_vec.push_back(temp);
	}

  return num_nodes;
}

int PRM::connectNodes(CSpace* cs)
{
  Vec3D dist;
  float len_sq = 0;

  for (int i = 0; i < num_nodes; i++)
  {
    //compare node_vec[i] against other nodes
    for (int k = 0; k < num_nodes; k++)
    {
      if (k != i) //don't check against itself
      {
        dist = node_vec[k]->pos - node_vec[i]->pos; //dist: i --> k
        len_sq = dotProduct(dist, dist);

        if (len_sq <= connection_radius_sq) //close enough to connect
        {
          //figure out if path connecting nodes is valid in CSpace
          if (cs->isValidSegment(dist, node_vec[i]->pos, agent_size))
          {
            link_t l = link_t(len_sq, node_vec[k]); //create new link_t holding length sq'd and Node*
            node_vec[i]->neighbor_nodes.push_back(l);
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
    for (int i = 0; i < node_vec.size(); i++)
    {
      node_vec[i]->draw(nodeShader);
    }
  }
  else if (draw_state == DRAW_PATH)
  {

  }
}

void PRM::drawConnections(GLuint shaderProgram)
{
  glm::vec3 color = glm::vec3(0.5,0.5,0.5);

  //fragment shader uniform -> color
	GLint uniform_color = glGetUniformLocation(shaderProgram, "color");
  glUniform3f(uniform_color, color[0], color[1], color[2]);

  glLineWidth(2);
	if (draw_state == DRAW_ALL) glDrawArrays(GL_LINES, 0, num_connections * 2); //*2 since 2 vertices per connection
  //else if (draw_state == DRAW_PATH) glDrawArrays(GL_LINES, 0, shortest_path.size() * 2);
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

  	for (int i = 0; i < node_vec.size(); i++)
  	{
  		//go through each node's list of connections
  		neighbors = node_vec[i]->neighbor_nodes;
  		i_connections = neighbors.size();
  		pi = node_vec[i]->pos;

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
    /*int num = shortest_path.size();

  	for (int i = 0; i < num-1; i++)
  	{
  		pi = shortest_path[i]->pos;
      pii = shortest_path[i+1]->pos;

      util::loadVecValues(lineData, pi, count);
      util::loadVecValues(lineData, pii, count);
  	}*/
  }

  cout << "LineData values loaded\n" << endl;
}//END loadLineVertices

vector<Node*> PRM::buildShortest(Node* start, Node* goal)
{
  if (alg_state == UCS)  return UniformCost(start, goal);
  else if (alg_state == ASTAR) return Astar(alg_weight, start, goal);
  else return vector<Node*>();
}

int PRM::changeDrawState()
{
  /*int num_lines = 0;

  if (draw_state == DRAW_ALL)
  {
    cout << "\n--->Drawing path only" << endl;
    draw_state = DRAW_PATH;
    num_lines = num_connections;
  }
  else if (draw_state == DRAW_PATH)
  {
    cout << "\n--->Drawing all connections" << endl;
    draw_state = DRAW_ALL;
    num_lines = num_connections;
  }*/

  cout << "---Reloading connections---" << endl;

  return num_connections;
}

Node* PRM::generateStart(int model_start, int model_verts)
{
  Vec3D size = Vec3D(node_size, node_size, node_size);
  Vec3D start_color = Vec3D(0,1,0);

  Material mat = Material();
	mat.setSpecular(glm::vec3(0, 0, 0));

  Node* temp;

	//place start at bottom left corner
	Vec3D pos = Vec3D(-width/2 + (double)rand()/RAND_MAX + node_size, -height/2 + (double)rand()/RAND_MAX + node_size, 0);
	temp = new Node(pos);
	temp->size = 2*size;
	temp->setVertexInfo(model_start, model_verts);

  mat.setAmbient(util::vec3DtoGLM(start_color));
  mat.setDiffuse(util::vec3DtoGLM(start_color));
	temp->mat = mat;
	return temp;
}

Node* PRM::generateGoal(int model_start, int model_verts)
{
  Vec3D size = Vec3D(node_size, node_size, node_size);
  Vec3D goal_color = Vec3D(1,0,0);

  Material mat = Material();
	mat.setSpecular(glm::vec3(0, 0, 0));

  Node* temp;

	//place goal at top right corner
	Vec3D pos = Vec3D(width/2 - (double)rand()/RAND_MAX - node_size, height/2 - (double)rand()/RAND_MAX - node_size, 0);
	temp = new Node(pos);
	temp->size = 2*size;
	temp->setVertexInfo(model_start, model_verts);

  mat.setAmbient(util::vec3DtoGLM(goal_color));
  mat.setDiffuse(util::vec3DtoGLM(goal_color));
	temp->mat = mat;
	return temp;
}

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/
//this implementation is largely inspired/copied from:
//  https://www.snip2code.com/Snippet/1017813/Uniform-Cost-Search-Algorithm-C---Implem
vector<Node*> PRM::UniformCost(Node* start_node, Node* goal_node)
{
  cout << "--Running a Uniform Cost Search--" << endl;

  Node* cur_node;
  Node* cur_neighbor;
  int pos = 0, num = 0;
  priority_queue <q_element, vector<q_element>, q_element_comparison> PQ;
  vector<Node*> shortest_path;

  cout << "start node at : ";
  start_node->pos.print();
  cout << "goal node at : ";
  goal_node->pos.print();

  //mark all nodes as having been visited
  for (int i = 0; i < node_vec.size(); i++)
  {
    node_vec[i]->visited = false;
  }

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
    //cout << "Expanding ";
    //cur_node->pos.print();

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

      return shortest_path;
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

          //cout << "Visiting: ";

          //extend the paths in the PQ to include cur_node's neighbors
          temp_el.path.push_back(cur_neighbor);
          temp_el.cost += sqrt(cur_node->neighbor_nodes[i].length_sq);

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
  return vector<Node*>();
}

//A*: heuristic = distance to goal
vector<Node*> PRM::Astar(float weight, Node* start_node, Node* goal_node)
{
  cout << "--Running A* with weight " << weight << "--" << endl;

  Node* cur_node;
  Node* cur_neighbor;
  int pos = 0, num = 0;
  priority_queue <q_element, vector<q_element>, q_element_comparison> PQ;
  Vec3D goal_pos = goal_node->pos;
  vector<Node*> shortest_path;

  cout << "start node at : ";
  start_node->pos.print();
  cout << "goal node at : ";
  goal_pos.print();

  //mark all nodes as having been visited
  for (int i = 0; i < node_vec.size(); i++)
  {
    node_vec[i]->visited = false;
  }

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

      return shortest_path;
    }
    else
    {
      //loop through neighbors of cur_el
      //--> "insert all the children of the dequeued element, with the cumulative costs as priority"
      num = cur_node->neighbor_nodes.size();

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

          //use heuristic and weight to calculate cost
          Vec3D to_goal = goal_pos - cur_node->neighbor_nodes[i].node->pos;
          float dist_to_goal_sq = dotProduct(to_goal, to_goal);

          temp_el.cost += sqrt(cur_node->neighbor_nodes[i].length_sq) + sqrt(weight*dist_to_goal_sq);

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
  return vector<Node*>();
}
