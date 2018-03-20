#include "Agent.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Agent::Agent() : WorldObject()
{
  vel = Vec3D();
}

Agent::Agent(std::vector<Node*> p) : WorldObject()
{
  path = p;

  //determine initial pos and vel from given path
  if (path.size() > 0)
  {
    pos = p[0]->pos;                   //start at start
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

Agent::~Agent()
{
  //don't need to delete vector since it holds Node*
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
