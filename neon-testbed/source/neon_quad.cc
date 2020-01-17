#include "..\include\neon_quad.h"

namespace neon {
	quad::quad()
	{
	}

	bool quad::create(shader_program* program)
	{
		program_ = program;
		float vertices_[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
		  -1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

		  -1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
		};

		if (!buffer_.create(sizeof(vertices_), vertices_))
		{
			return false;
		}

		GLint position_location_ = program_->get_attrib_location("position");
		GLint texcoord_location_ = program_->get_attrib_location("texcoord");
		format_.add_attribute(position_location_, 2, GL_FLOAT, false);
		format_.add_attribute(position_location_, 2, GL_FLOAT, false);
	}

	void quad::render() {
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
