//neon_object_h

#ifndef NEON_OBJECT_H
#define NEON_OBJECT_H

#include "neon_model.h"

namespace neon {
	struct object {
		object();
		bool create(glm::mat4 world, const string &texturePath, const string &modelPath);

		glm::mat4 world_;
		model model_;
	};
}

#endif // !NEON_OBJECT_H
