#ifndef FLOCK_INCLUDED
#define FLOCK_INCLUDED

#include "glad.h"  //Include order can matter here

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#else
#include <SDL.h>
#include <SDL_opengl.h>
#endif

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Vec3D.h"
#include "Node.h"
#include "Agent.h"
#include "PRM.h"
#include "CSpace.h"

class Flock
{
  private:
    Agent** followers;
    Agent** leaders;
    int num_followers = 0;
    int num_leaders = 0;
    float neighborhood_r = 40.0;
    float max_vel = 50.0;
    float max_acc = 50.0;
    CSpace* myCSpace;
    float error = 0.5;

  public:
    //PUBLIC VARIABLES
    float k_sep = 0.8;
    float k_all = 1.0;
    float k_coh = 2.0;
    float k_obs = 5.0;

    Flock();
    Flock(int nf, int nl, PRM* myPRM, CSpace* cs, int model_start, int model_verts);
    ~Flock();

    //OTHERS
    void update(float dt);
    void draw(GLuint shader);

};

#endif
