#include "..\include\neon_quad.h"

namespace neon {
	quad::quad()
	{
	}

	bool quad::create(shader_program* program)
	{
		program_ = program;
		vertex vertices_[] = {
         {glm::vec2(-1.0f,  1.0f),  glm::vec2(0.0f, 1.0f)},
         {glm::vec2(-1.0f, -1.0f),  glm::vec2(0.0f, 0.0f)},
         {glm::vec2( 1.0f, -1.0f),  glm::vec2(1.0f, 0.0f)},

         {glm::vec2(-1.0f,  1.0f),  glm::vec2(0.0f, 1.0f)},
         {glm::vec2( 1.0f, -1.0f),  glm::vec2(1.0f, 0.0f)},
         {glm::vec2( 1.0f,  1.0f),  glm::vec2(1.0f, 1.0f)},
		};

		if (!buffer_.create(sizeof(vertices_), &vertices_))
		{
			return false;
		}

      program_->bind();

		GLint position_location_ = program_->get_attrib_location("position");
		GLint texcoord_location_ = program_->get_attrib_location("texcoord");
		format_.add_attribute(position_location_, 2, GL_FLOAT, false);
		format_.add_attribute(texcoord_location_, 2, GL_FLOAT, false);

      return true;
	}

	void quad::render() {
      buffer_.bind();
      format_.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
   }
}
