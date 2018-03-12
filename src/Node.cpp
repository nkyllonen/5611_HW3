#include "Node.h"

/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Node::Node()
{
  pos = Vec3D();
  size = Vec3D(0.5, 0.5, 0.5);
}

Node::Node(Vec3D init_pos)
{
  pos = init_pos;
  size = Vec3D(0.5, 0.5, 0.5);
}

Node::~Node()
{

}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Node::setVertexInfo(int start, int total)
{
  start_vertex_index = start;
  total_vertices = total;
}

/*----------------------------*/
// OTHERS
/*----------------------------*/
//assumes that the models have already been loaded into the VBO before this call
void Node::draw(GLuint shaderProgram)
{
  GLint uniModel = glGetUniformLocation(shaderProgram, "model");

	glm::mat4 model;
	glm::vec3 size_v = util::vec3DtoGLM(size);
	glm::vec3 pos_v = util::vec3DtoGLM(pos);

	//build model mat specific to this WObj
	model = glm::translate(model, pos_v);
	model = glm::scale(model, size_v);
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	//fragment shader uniforms (from Material)
	GLint uniform_ka = glGetUniformLocation(shaderProgram, "ka");
	GLint uniform_kd = glGetUniformLocation(shaderProgram, "kd");
	GLint uniform_ks = glGetUniformLocation(shaderProgram, "ks");
	GLint uniform_s = glGetUniformLocation(shaderProgram, "s");

	glm::vec3 mat_AMB = mat.getAmbient();
	glUniform3f(uniform_ka, mat_AMB[0], mat_AMB[1], mat_AMB[2]);

	glm::vec3 mat_DIF = mat.getDiffuse();
	glUniform3f(uniform_kd, mat_DIF[0], mat_DIF[1], mat_DIF[2]);

	glm::vec3 mat_SPEC = mat.getSpecular();
	glUniform3f(uniform_ks, mat_SPEC[0], mat_SPEC[1], mat_SPEC[2]);

	glUniform1f(uniform_s, mat.getNS());

	//starts at an offset of start_vertex_index
	//(Primitive Type, Start Vertex, End Vertex)
	glDrawArrays(GL_TRIANGLES, start_vertex_index, total_vertices);
}
