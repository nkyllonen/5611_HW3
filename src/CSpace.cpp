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
WorldObject* CSpace::getObstacle(int i)
{
  return obstacles[i];
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void CSpace::addObstacle(WorldObject* o)
{
  obstacles.push_back(o);
}

//returns if a given position is valid
// -1 = valid position
// otherwise returns index of obstacle of intersection
int CSpace::isValidPosition(Vec3D p, float agent_radius)
{
  Vec3D dist_v;
  float dist_sq = 0, radius_sq = 0;

  //check against each obstacle
  for (int i = 0; i < obstacles.size(); i++)
  {
    radius_sq = obstacles[i]->getSize().getX() + agent_radius/2.0; //extend by extent of agent
    radius_sq = pow(radius_sq, 2);
    dist_v = obstacles[i]->getPos() - p;
    dist_sq = dotProduct(dist_v, dist_v);

    if (dist_sq <= radius_sq) return i;
  }

  return -1;
}

//determine is vector connecting 2 nodes intersects with any obstacles
bool CSpace::isValidSegment(Vec3D AtoB, Vec3D ptA, float agent_radius)
{
  //immediately normalize AtoB so projection is correct
  AtoB.normalize();

  Vec3D AtoC;
  float projAC = 1, CtoD_len = 1, radius = 0;

  //check segment against each obstacle
  for (int i = 0; i < obstacles.size(); i++)
  {
    AtoC = obstacles[i]->getPos() - ptA;
    projAC = dotProduct(AtoC, AtoB);

    //some pythag --> triangle ACD
    CtoD_len = sqrt(dotProduct(AtoC, AtoC) - pow(projAC, 2));
    radius = obstacles[i]->getSize().getX()/2.0 + agent_radius/2.0; //extend by extent of agent

    if (CtoD_len <= radius) return false;
  }

  return true;
}

void CSpace::draw(GLuint shaderProgram)
{
  for (int i = 0; i < obstacles.size(); i++)
  {
    obstacles[i]->draw(shaderProgram);
  }
}
