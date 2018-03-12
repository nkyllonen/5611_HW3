#ifndef WORLDOBJ_INCLUDED
#define WORLDOBJ_INCLUDED

#include "Vec3D.h"
#include "Util.h"
#include "Camera.h"
#include "Material.h"

class WorldObject
{
protected:
	Vec3D pos;
	Material mat;
	Vec3D size;
	int start_vertex_index;	//index where vertices start in modelData array
	int total_vertices;	//total num of vertices within modelData array

public:
	//CONSTRUCTORS AND DESTRUCTORS
	WorldObject();
	WorldObject(Vec3D init_pos);
	~WorldObject();

	//SETTERS
	void setPos(Vec3D p);
	void setVertexInfo(int start, int total);
	void setMaterial(Material m);
	void setSize(Vec3D s);
	void setColor(Vec3D color); //sets ambient and diffuse to 'color'

	//GETTERS
	Vec3D getPos();
	Material getMaterial();
	Vec3D getSize();

	//VIRTUAL

	//OTHER
	void draw(GLuint shaderProgram); //shared draw function among WObjs

};

#endif
