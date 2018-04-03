#include "Flock.h"

using namespace std;

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Flock::Flock()
{

}

Flock::Flock(int nf, int nl, PRM* myPRM, CSpace* cs, int model_start, int model_verts)
{
  num_followers = nf;
  num_leaders = nl;

  leaders = new Agent*[num_leaders];
  cout << "--------------------------------------------------" << endl;
  cout << "Allocated array of " << num_leaders << " leaders" << endl;
  followers = new Agent*[num_followers];
  cout << "Allocated array of " << num_followers << " followers" << endl;

  Material mat = Material();
  mat.setAmbient(glm::vec3(0.7, 0.7, 0.7));
  mat.setDiffuse(glm::vec3(0.7, 0.7, 0.7));
  mat.setSpecular(glm::vec3(0, 0, 0));

  Node* start;
  Node* goal;
  float size = myPRM->agent_size;

  //initialize leaders using PRM
  for (int i = 0; i < num_leaders; i++)
  {
    start = myPRM->generateStart(model_start, model_verts);
    goal = myPRM->generateGoal(model_start, model_verts);
    leaders[i] = new Agent(start, goal);
    leaders[i]->calcPath(myPRM, cs);
    leaders[i]->setVertexInfo(model_start, model_verts);
    leaders[i]->setMaterial(mat);
    leaders[i]->setSize(Vec3D(size, size, size));
  }

  Vec3D p;

  //initialize followers
  for (int i = 0; i < num_followers; i++)
  {
    p = myPRM->getRandPosition();
    p.z = 0;
    p.print();
    followers[i] = new Agent();
    followers[i]->setPos(p);
    followers[i]->setVertexInfo(model_start, model_verts);
    followers[i]->setMaterial(mat);
    followers[i]->setSize(Vec3D(size/2.0, size/2.0, size/2.0));
  }
}

Flock::~Flock()
{
  for (int i = 0; i < num_leaders; i++)
  {
    delete leaders[i];
  }
  delete leaders;

  for (int i = 0; i < num_followers; i++)
  {
    delete followers[i];
  }
  delete followers;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
void Flock::update(float dt)
{
  //1. update leaders so they follow their own paths
  for (int i = 0; i < num_leaders; i++)
  {
    leaders[i]->update(dt);
  }

  Vec3D net_force;
  Vec3D total_pos;
  Vec3D total_heading;
  Vec3D total_diff;
  Vec3D diff;
  Vec3D heading;
  float dist_sq = 1;
  int num_neighbors = 0;

  //2. update followers using Boid forces
  for (int i = 0; i < num_followers; i++)
  {
    Vec3D pos_i = followers[i]->getPos();
    //cout << "Follower " << i << " : " << endl;
    //pos_i.print();
    Vec3D vel_i = followers[i]->vel;

    //2.1 find neighboring agents of follower i
    for (int j = 0; j < num_followers; j++)
    {
      if (i != j)
      {
        diff = followers[j]->getPos() - pos_i;
        dist_sq = dotProduct(diff, diff);

        if (dist_sq < neighborhood_r_sq)
        {
          //sum up values in here instead of later
          total_diff = total_diff + (1.0/dist_sq)*diff;
          total_pos = total_pos + followers[j]->getPos();

          /*heading = followers[j]->vel;
          heading.normalize();
          total_heading = total_heading + heading;*/

          num_neighbors ++;
        }
      }
    }//END for followers j

    //2.2 loops through leaders as well so they can lead
    for (int i = 0; i < num_leaders; i++)
    {
      diff = leaders[i]->getPos() - pos_i;
      dist_sq = dotProduct(diff, diff);

      //add the leader values as well
      total_diff = total_diff + (1.0/dist_sq)*diff;
      total_pos = total_pos + leaders[i]->getPos();

      //only find average heading of leaders since they are leading the Flock
      heading = leaders[i]->vel;
      heading.normalize();
      total_heading = total_heading + heading;

      num_neighbors ++;
    }//END for leaders i

    //2.3 Separation force
    net_force = net_force + (-1.0*k_sep)*total_diff;

    //2.4 Alignment force
    //net_force = net_force + k_all*((1.0/num_neighbors)*total_heading - followers[i]->vel);
    net_force = net_force + k_all*((1.0/num_leaders)*total_heading - vel_i);

    //2.5 Cohesion force
    net_force = net_force + k_coh*((1.0/num_neighbors)*total_pos - pos_i);

    //2.6 apply force to follower i
    //cap acceleration
    float mag = net_force.getMagnitude();
    if (mag > max_acc)
    {
      //normalize and resize so it's the right length
      net_force = (max_acc/mag)*net_force;
    }

    //cap velocity
    Vec3D temp_v = vel_i + net_force;
    mag = temp_v.getMagnitude();
    if (mag > max_vel)
    {
      //normalize and resize so it's the right length
      temp_v = (max_vel/mag)*temp_v;
    }

    followers[i]->vel = temp_v;

    //reset for the next follower
    net_force = Vec3D();
    total_diff = Vec3D();
    total_pos = Vec3D();
    total_heading = Vec3D();
    num_neighbors = 0;
  }//END for i
}//END update

void Flock::draw(GLuint shader)
{
  for (int i = 0; i < num_leaders; i++)
  {
    leaders[i]->draw(shader);
  }

  for (int i = 0; i < num_followers; i++)
  {
    followers[i]->draw(shader);
  }
}
