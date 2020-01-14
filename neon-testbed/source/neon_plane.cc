//NEON_PLANE_CC
#include "neon_plane.h"

namespace neon {
	wall::wall()
		: size_(0)
		, world_(1.0f)
	{
	};

	bool wall::create(const string& texturePath, const string& normalMapPath){
		if (!program_.create("assets/model/vertex_shader.txt",
									"assets/model/fragment_shader.txt")) {
			return false;
		}

		if (!texture_.create(texturePath)) {
			return false;
		}

		if (!sampler_.create(GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE)) {
			return false;
		}

		if (!texture_.create(normalMapPath)) {
			return false;
		}
		sampler_.bind(1);

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

		normal_ = glm::vec3(0.0f, 0.0f, 1.0f);

		GLint position_location_ = program_.get_attrib_location("position");
		GLint texcoords_location_ = program_.get_attrib_location("texcoords");
		format_.add_attribute(position_location_, 3, GL_FLOAT, false);
		format_.add_attribute(texcoords_location_, 2, GL_FLOAT, false);

		program_.set_uniform_vec3("normal", normal_);
		program_.set_uniform_mat4("projection", camera_->projection_);
		program_.set_uniform_mat4("view", camera_->view_);
		program_.set_uniform_mat4("world", world_);


		return true;
	};
}