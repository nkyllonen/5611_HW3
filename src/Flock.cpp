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
  myCSpace = cs;

  leaders = new Agent*[num_leaders];
  cout << "--------------------------------------------------" << endl;
  cout << "Allocated array of " << num_leaders << " leaders" << endl;
  followers = new Agent*[num_followers];
  cout << "Allocated array of " << num_followers << " followers" << endl;

  Material mat = Material();
  mat.setAmbient(glm::vec3(0.5, 0, 0.9));
	mat.setDiffuse(glm::vec3(0.5, 0, 0.9));
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
    //loop while checking if generated position is not valid
    do
    {
      //subtract node_size so that they won't go over the edge of the area
      p = myPRM->getRandPosition();
      p.z = 0;
    } while(cs->isValidPosition(p, size) != -1); //-1 = no collision

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
  float dist = 1;
  int num_neighbors = 0;
  float size = followers[0]->getSize().x;

  //2. update followers using Boid forces
  for (int i = 0; i < num_followers; i++)
  {
    Vec3D pos_i = followers[i]->getPos();
    Vec3D vel_i = followers[i]->vel;

    //2.1 find neighboring agents of follower i
    for (int j = 0; j < num_followers; j++)
    {
      if (i != j)
      {
        diff = followers[j]->getPos() - pos_i;
        dist = diff.getMagnitude();

        if (dist < neighborhood_r)
        {
          diff = (1.0/dist)*diff;       //find diff unit vector
          //diff = (dist - 2*size)*diff;  //rescale diff to include extent of both agents

          //sum up values in here instead of later
          total_diff = total_diff + (1.0/(dist - 2*size))*diff;
          total_pos = total_pos + followers[j]->getPos();

          heading = followers[j]->vel;
          //heading.normalize();
          total_heading = total_heading + heading;

          num_neighbors ++;
        }
      }
    }//END for followers j

    //2.2 loops through leaders as well so they can lead
    for (int i = 0; i < num_leaders; i++)
    {
      diff = leaders[i]->getPos() - pos_i;
      dist = diff.getMagnitude();

      diff = (1.0/dist)*diff;       //find diff unit vector

      //add the leader values as well
      total_diff = total_diff + (1.0/(dist - 2*size))*diff;
      total_pos = total_pos + leaders[i]->getPos();

      //only find average heading of leaders since they are leading the Flock
      heading = leaders[i]->vel;
      //heading.normalize();
      total_heading = total_heading + heading;

      num_neighbors ++;
    }//END for leaders i

    //2.3 Separation force
    net_force = net_force + (-1.0*k_sep)*total_diff;

    //2.4 Alignment force
    //net_force = net_force + k_all*((1.0/num_neighbors)*total_heading - followers[i]->vel);
    net_force = net_force + k_all*((1.0/num_neighbors)*total_heading - vel_i);

    //2.5 Cohesion force
    //net_force = net_force + k_coh*((1.0/(num_followers-1+num_leaders))*total_pos - pos_i);
    net_force = net_force + k_coh*((1.0/(num_neighbors))*total_pos - pos_i);

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
      mag = max_vel;
    }

    //2.6 Obstacle avoidance force
    //check an offset of where we're trying to go to check ahead of the agent
    Vec3D temp_p = pos_i + 3*dt*temp_v;
    int ob_i = myCSpace->isValidPosition(temp_p, size);
    if (ob_i != -1) //-1 = no collision
    {
      //cout << "\nFollower-obstacle collision detected" << endl;
      /*cout << "--pos_i : ";
      pos_i.print();
      cout << "--temp_p : ";
      temp_p.print();*/
      WorldObject* ob = myCSpace->getObstacle(ob_i);
      Vec3D radial = temp_p - ob->getPos();
      radial.normalize();

      //add force away from center of obstacle
      temp_v = temp_v + k_obs*radial;
    }

    //2.7 apply force to follower i
    followers[i]->vel = temp_v;
    followers[i]->setPos(pos_i + dt*temp_v);

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
