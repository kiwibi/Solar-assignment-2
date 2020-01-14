//neon_object_h

#ifndef NEON_OBJECT_H
#define NEON_OBJECT_H

#include "neon_model.h"
#include "neon_graphics.h"

struct DirectionalLight;

namespace neon {
	struct object {
		object();
		bool create(glm::mat4 world, const string &texturePath, const string &modelPath);
      void render(fps_camera camera_);

		glm::mat4 world_;
		model model_;

      DirectionalLight* light_;
	};
}

#endif // !NEON_OBJECT_H
