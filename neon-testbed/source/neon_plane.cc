//NEON_PLANE_CC
#include "neon_plane.h"

namespace neon {
	wall::wall()
		: size_(1)
		, world_(1.0f)
	{
      world_ = glm::translate(world_, glm::vec3(0.0f, 0.0f, -10.0f));
	};

	bool wall::create(const string& texturePath, const string& normalMapPath){
		if (!program_.create("assets/Wall/wall_vertex_shader.txt",
									"assets/Wall/wall_fragment_shader.txt")) {
			return false;
		}

		if (!texture_.create(texturePath)) {
			return false;
		}

		if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		if (!normalMap_.create(normalMapPath)) {
			return false;
		}
      
		vertex vertices[] = {
			//side 1, triangle 1
			{ glm::vec3(-size_,   size_,   size_),   glm::vec2(0.0f,1.0f)},
			{ glm::vec3(-size_,  -size_,   size_),   glm::vec2(0.0f,0.0f)},
			{ glm::vec3( size_,   size_,   size_),   glm::vec2(1.0f,1.0f)},
			//side 1, triangle 2
			{ glm::vec3(-size_,  -size_,   size_),   glm::vec2(0.0f,0.0f)},
			{ glm::vec3( size_,   size_,   size_),   glm::vec2(1.0f,1.0f)},
			{ glm::vec3( size_,  -size_,   size_),   glm::vec2(1.0f,0.0f)},
		};

		if (!vbo_.create(sizeof(vertices), &vertices))
		{
			return false;
		}

		normal_ = glm::vec3(0.0f, 0.0f, -1.0f);

		GLint position_location_ = program_.get_attrib_location("position");
		GLint texcoords_location_ = program_.get_attrib_location("texcoord");
		format_.add_attribute(position_location_, 3, GL_FLOAT, false);
		format_.add_attribute(texcoords_location_, 2, GL_FLOAT, false);

		return true;
	}
   void wall::render(fps_camera camera)
   {
      GLenum err = GL_NO_ERROR;

      glEnable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);
      glFrontFace(GL_CW);

      program_.bind();
      program_.set_uniform_mat4("projection", camera.projection_);
      program_.set_uniform_mat4("view", camera.view_);
      program_.set_uniform_mat4("world", world_);
      program_.set_uniform_vec3("normal", normal_);

      program_.set_uniform_vec3("lightDir", light_->direction_);
      program_.set_uniform_vec3("lightColor", light_->colour_);
      program_.set_uniform_float("lightPower", light_->power_);

      program_.set_uniform_int("diffuse", 0);
      program_.set_uniform_int("normalMap", 1);

      vbo_.bind();
      format_.bind();

      texture_.bind(0);
      normalMap_.bind(1);
      sampler_.bind(0);
      sampler_.bind(1);

      vbo_.render(GL_TRIANGLES, 0, 6);
   }
}