#ifndef CSPACE_INCLUDED
#define CSPACE_INCLUDED

#include "WorldObject.h"
#include "Vec3D.h"

#include <cmath>

class CSpace
{
  private:
    std::vector<WorldObject*> obstacles;

  public:
    CSpace();
    ~CSpace();

    //SETTERS

    //GETTERS
    WorldObject* getObstacle(int i);

    //OTHERS
    void addObstacle(WorldObject* o);
    int isValidPosition(Vec3D p, float agent_radius);
    bool isValidSegment(Vec3D AtoB, Vec3D ptA, float agent_radius);
    void draw(GLuint shaderProgram);

};

#endif
