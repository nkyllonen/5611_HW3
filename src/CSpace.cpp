#include "CSpace.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
CSpace::CSpace()
{

}

CSpace::~CSpace()
{
  for (int i = 0; i < obstacles.size(); i++)
  {
    delete obstacles[i];
  }
}

/*----------------------------*/
// SETTERS
/*----------------------------*/

/*----------------------------*/
// GETTERS
/*----------------------------*/

/*----------------------------*/
// OTHERS
/*----------------------------*/
void CSpace::addObstacle(WorldObject* o)
{
  obstacles.push_back(o);
}

bool CSpace::isValidPosition(Vec3D p, float agent_radius)
{
  Vec3D dist_v;
  float dist_sq = 0, radius_sq = 0;

  //check against each obstacle
  for (int i = 0; i < obstacles.size(); i++)
  {
    radius_sq = pow(obstacles[i]->getSize().getX(), 2) + pow(agent_radius, 2); //extend by extent of agent
    dist_v = obstacles[i]->getPos() - p;
    dist_sq = dotProduct(dist_v, dist_v);

    if (dist_sq < radius_sq) return false;
  }

  return true;
}

//determine is vector connecting 2 nodes intersects with any obstacles
bool CSpace::isValidSegment(Vec3D AtoB, float agent_radius)
{
  Vec3D AtoC, ptD;

  //check segment against each obstacle
  for (int i = 0; i < obstacles.size(); i++)
  {
    
  }
}

void CSpace::draw(GLuint shaderProgram)
{
  for (int i = 0; i < obstacles.size(); i++)
  {
    obstacles[i]->draw(shaderProgram);
  }
}
