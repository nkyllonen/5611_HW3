#include "World.h"

using namespace std;


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
World::World()
{
	width = 0;
	height = 0;

	myPRM = new PRM();
}

World::World(int w, int h)
{
	width = w;
	height = h;

	myPRM = new PRM(w, h, w*2);
}

World::~World()
{
	delete[] modelData;
	//delete[] lineData;
	delete floor;
	delete myPRM;
}

/*----------------------------*/
// SETTERS
/*----------------------------*/

/*----------------------------*/
// GETTERS
/*----------------------------*/
int World::getWidth()
{
	return width;
}

int World::getHeight()
{
	return height;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
//load in all models and store data into the modelData array
bool World::loadModelData()
{
	/////////////////////////////////
	//LOAD IN MODELS
	/////////////////////////////////
	//CUBE
	CUBE_VERTS = 0;
	float* cubeData = util::loadModel("models/cube.txt", CUBE_VERTS);
	cout << "Number of vertices in cube model : " << CUBE_VERTS << endl;
	total_model_verts += CUBE_VERTS;

	//SPHERE
	SPHERE_START = CUBE_VERTS;
	float* sphereData = util::loadModel("models/sphere.txt", SPHERE_VERTS);
	cout << "Number of vertices in sphere model : " << SPHERE_VERTS << endl << endl;
	total_model_verts += SPHERE_VERTS;

	/////////////////////////////////
	//BUILD MODELDATA ARRAY
	/////////////////////////////////
	if (!(cubeData != nullptr && sphereData != nullptr))
	{
		cout << "ERROR. Failed to load model(s)" << endl;
		delete[] cubeData;
		delete[] sphereData;
		return false;
	}

	modelData = new float[total_model_verts * 8];
	cout << "Allocated modelData : " << total_model_verts * 8 << endl << endl;

	copy(cubeData, cubeData + CUBE_VERTS * 8, modelData);
	copy(sphereData, sphereData + SPHERE_VERTS * 8, modelData + (CUBE_VERTS * 8));

	delete[] cubeData;
	delete[] sphereData;
	return true;
}

//
bool World::setupGraphics()
{
	/////////////////////////////////
	//BUILD CUBE&SPHERE VAO + VBOs
	/////////////////////////////////
	//This stores the VBO and attribute mappings in one object
	glGenVertexArrays(1, &model_vao); //Create a VAO
	glBindVertexArray(model_vao); //Bind the model_vao to the current context

	//Allocate memory on the graphics card to store geometry (vertex buffer object)
	glGenBuffers(1, model_vbo);  //Create 1 buffer called model_vbo
	glBindBuffer(GL_ARRAY_BUFFER, model_vbo[0]); //Set the model_vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, total_model_verts * 8 * sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to model_vbo

	/////////////////////////////////
	//SETUP CUBE SHADERS (model_vao attributes --> bound to model_vbo[0])
	/////////////////////////////////
	phongProgram = util::LoadShader("Shaders/phongTex.vert", "Shaders/phongTex.frag");

	//load in textures
	tex0 = util::LoadTexture("textures/checker2.bmp");
	tex1 = util::LoadTexture("textures/grey_stones.bmp");

	if (tex0 == -1 || tex1 == -1 || phongProgram == -1)
	{
		cout << "\nCan't load texture(s)" << endl;
		printf(strerror(errno));
		return false;
	}

	//Tell OpenGL how to set fragment shader input
	GLint posAttrib = glGetAttribLocation(phongProgram, "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0); //positions first
	//Attribute, vals/attrib., type, normalized?, stride, offset
	//Binds to VBO current GL_ARRAY_BUFFER
	glEnableVertexAttribArray(posAttrib);

	GLint texAttrib = glGetAttribLocation(phongProgram, "inTexcoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //texcoords second
	glEnableVertexAttribArray(texAttrib);

	GLint normAttrib = glGetAttribLocation(phongProgram, "inNormal");
	glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float))); //normals last
	glEnableVertexAttribArray(normAttrib);

	/////////////////////////////////
	//BUILD LINE VAO + VBO
	/////////////////////////////////
	glGenVertexArrays(1, &line_vao);
	glBindVertexArray(line_vao); //Bind the line_vao to the current context
	glGenBuffers(1, line_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, total_lines * 6 * sizeof(float), lineData, GL_STATIC_DRAW);

	/////////////////////////////////
	//SETUP LINE SHADERS
	/////////////////////////////////
	flatProgram = util::LoadShader("Shaders/flat.vert", "Shaders/flat.frag");

	if (flatProgram == -1)
	{
		cout << "\nCan't load flat shaders(s)" << endl;
		printf(strerror(errno));
		return false;
	}

	//only passing in a position vec3 value into shader
	GLint line_posAttrib = glGetAttribLocation(flatProgram, "position");
	glVertexAttribPointer(line_posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(line_posAttrib);

	glBindVertexArray(0); //Unbind the VAO in case we want to create a new one

	glEnable(GL_DEPTH_TEST);
	return true;
}

//loops through WObj array and draws each
//also draws floor
void World::draw(Camera * cam)
{
	glClearColor(.2f, 0.4f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(phongProgram); //Set the active shader (only one can be used at a time)

	//vertex shader uniforms
	GLint uniView = glGetUniformLocation(phongProgram, "view");
	GLint uniProj = glGetUniformLocation(phongProgram, "proj");
	GLint uniTexID = glGetUniformLocation(phongProgram, "texID");

	//build view matrix from Camera
	glm::mat4 view = glm::lookAt(
		util::vec3DtoGLM(cam->getPos()),
		util::vec3DtoGLM(cam->getPos() + cam->getDir()),  //Look at point
		util::vec3DtoGLM(cam->getUp()));

	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(3.14f / 4, 800.0f / 600.0f, 0.1f, 100.0f); //FOV, aspect, near, far
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	glUniform1i(glGetUniformLocation(phongProgram, "tex0"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glUniform1i(glGetUniformLocation(phongProgram, "tex1"), 1);

	glBindVertexArray(model_vao);

	glUniform1i(uniTexID, 0); //Set texture ID to use for floor
	floor->draw(phongProgram);

	//draw PRM
	glUniform1i(uniTexID, -1);

	myPRM->drawNodes(phongProgram);

	glUseProgram(flatProgram);
	glBindVertexArray(line_vao);
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo[0]); //Set the line_vbo as the active

	//new uniforms for the flat shading program
	GLint uniLineModel = glGetUniformLocation(flatProgram, "model");
	GLint uniLineView = glGetUniformLocation(flatProgram, "view");
	GLint uniLineProj = glGetUniformLocation(flatProgram, "proj");

	glm::mat4 model;
	glUniformMatrix4fv(uniLineModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniLineView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniLineProj, 1, GL_FALSE, glm::value_ptr(proj));

	myPRM->drawConnections(flatProgram);
}

/*--------------------------------------------------------------*/
// init : initializes floor and random nodes
/*--------------------------------------------------------------*/
void World::init()
{
	//1. initialize floor
	floor = new WorldObject(Vec3D(0,0,-0.1));				//set floor slightly lower so there's no issues displaying lines on top
	floor->setSize(Vec3D(width, height, 0.1));			//width and height are x and y
	floor->setVertexInfo(CUBE_START, CUBE_VERTS);

	Material mat = Material();
	mat.setAmbient(glm::vec3(0.7, 0.7, 0.7));
	mat.setDiffuse(glm::vec3(0.7, 0.7, 0.7));
	mat.setSpecular(glm::vec3(0, 0, 0));

	floor->setMaterial(mat);

	//2. initialize all nodes with random positions along floor
	num_nodes = myPRM->generateNodes(CUBE_START, CUBE_VERTS);
	cout << "Generated " << num_nodes << " nodes" << endl;
	total_lines = myPRM->connectNodes();
	cout << "Generated " << total_lines << " connections between nodes" << endl;

	lineData = new float[total_lines * 6]; //3 coords per endpts of each spring (3 x 2)
	cout << "\nAllocated lineData : " << total_lines * 6 << endl;

	myPRM->loadLineVertices(lineData);
}

/*----------------------------*/
// PRIVATE FUNCTIONS
/*----------------------------*/
