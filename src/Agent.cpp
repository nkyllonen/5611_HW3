#include "Agent.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Agent::Agent() : WorldObject()
{
  vel = Vec3D();
}

Agent::Agent(Node* s, Node* g) : WorldObject()
{
  vel = Vec3D();
  start_node = s;
  goal_node = g;
}

Agent::~Agent()
{
  //don't delete these here because PRM deletes them
  //delete start_node;
  //delete goal_node;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void Agent::update(double dt)
{
  //0. FIRST check if we're moving at all
  if (vel == Vec3D())
  {
    return;
  }

  //1. check to see if we're near a node
  float collision_radius_sq = pow(0.1, 2);
  int num_nodes = path.size();
  Vec3D dist;


  for (int i = 0; i < num_nodes; i++)
  {
    //distance vector from this position to node[i]
    dist = path[i]->pos - pos;

    if (dotProduct(dist, dist) <= collision_radius_sq)
    {
      //we are at node[i]!
      if (i == num_nodes - 1) //at the goal!!
      {
        std::cout << "+++++++GOAL REACHED!!!+++++++" << std::endl;
        vel = Vec3D();        //stop moving
      }
      else
      {
        //change direction and head towards the next node
        vel = path[i+1]->pos - path[i]->pos;
        vel.normalize();
      }
    }
  }

  //2. update posiiton using velocity
  pos = pos + speed*dt*vel;
}

void Agent::calcPath(PRM* myPRM, CSpace* cs)
{
  //1. connect start_node and goal_node to myPRM's nodes
  //loop through all between nodes and connect if close enough to s/g
  Vec3D sToi;
  Vec3D gToi;
  int num = myPRM->num_nodes;
  float len_sq = 1;
  link_t l;

  //push start and goal onto PRM's graph
  myPRM->node_vec.push_back(start_node);
  myPRM->node_vec.push_back(goal_node);

  for (int i = 0; i < num; i++)
  {
    sToi = myPRM->node_vec[i]->pos - start_node->pos; //start --> i
    gToi = myPRM->node_vec[i]->pos - goal_node->pos; //goal --> i

    //check for start to i
    len_sq = dotProduct(sToi, sToi);
    if (len_sq <= myPRM->connection_radius_sq) //close enough to connect
    {
      //printf("Connected node %i to start\n", i);
      //figure out if path connecting nodes is valid in CSpace
      if (cs->isValidSegment(sToi, start_node->pos, myPRM->agent_size))
      {
        //connect start to i
        l = link_t(len_sq, myPRM->node_vec[i]);
        start_node->neighbor_nodes.push_back(l);

        //connect i to start
        l = link_t(len_sq, start_node);
        myPRM->node_vec[i]->neighbor_nodes.push_back(l);
        myPRM->num_connections += 2;
      }
    }

    //check for goal to i
    len_sq = dotProduct(gToi, gToi);
    if (len_sq <= myPRM->connection_radius_sq) //close enough to connect
    {
      //printf("Connected node %i to goal\n", i);
      //figure out if path connecting nodes is valid in CSpace
      if (cs->isValidSegment(gToi, goal_node->pos, myPRM->agent_size))
      {
        //connect goal to i
        l = link_t(len_sq, myPRM->node_vec[i]);
        goal_node->neighbor_nodes.push_back(l);

        //connect i to goal
        l = link_t(len_sq, goal_node);
        myPRM->node_vec[i]->neighbor_nodes.push_back(l);
        myPRM->num_connections += 2;
      }
    }
  }

  //update number of nodes AFTER connecting start and goal
  myPRM->num_nodes += 2;

  //2. use PRM to calc shortest path
  cout << "\nCalculating shortest_path...." << endl;
  path = myPRM->buildShortest(start_node, goal_node);

  //3. determine initial pos and vel from calculated path
  if (path.size() > 0)
  {
    pos = start_node->pos;                   //start at start
    vel = path[1]->pos - path[0]->pos; //from start to first Node
    vel.normalize();                   //make sure it's a unit vector!

    cout << "Agent starting at: ";
    pos.print();
    cout << "\t--> with velocity: ";
    vel.print();
  }
  else
  {
    cout << "ERROR: EMPTY PATH VECTOR GIVEN TO AGENT" << endl;
  }
}

void Agent::drawPath(GLuint nodeShader)
{
  int num = path.size();

  for (int i = 0; i < num; i++)
  {
    path[i]->draw(nodeShader);
  }
}
