//NEON_PLANE_H_INCLUDED
#ifndef NEON_PLANE_H_INCLUDED
#define NEON_PLANE_H_INCLUDED

#include <neon_core.h>
#include "neon_graphics.h"
#include "DirectionalLight.h"

namespace neon {
	struct wall {
		struct vertex {
			glm::vec3 position_;
			glm::vec2 texcoords_;
		};
		wall();
		bool create(const string& texturePath, const string& normalMapPath);

      void render(fps_camera camera);

		shader_program program_;
		vertex_buffer vbo_;
		vertex_format format_;
		texture texture_;
      texture normalMap_;
		sampler_state sampler_;

		float size_;
		glm::vec3 normal_;
		glm::mat4 world_;

      DirectionalLight* light_;
	};
}

#endif //NEON_PLANE_H_INCLUDED