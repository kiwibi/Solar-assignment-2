//INCLUDE NEON_QUAD_H
#ifndef NEON_QUAD_H_INCLUDED
#define NEON_QUAD_H_INCLUDED

#include "neon_graphics.h"

namespace neon {
	struct quad {
      struct vertex {
         glm::vec2 position_;
         glm::vec2 texcoord_;
      };

		quad();
		bool create(shader_program *program);

		void render();

		vertex_buffer buffer_;
		vertex_format format_;
		shader_program* program_;
	};
};

#endif // !NEON_QUAD_H_INCLUDED
