#include "..\include\neon_object.h"

namespace neon {
	object::object()
	{

	}

	bool object::create(glm::mat4 world, const string &texturePath, const string &modelPath)
	{
		world_ = world;
		if (!model_.create_from_file(modelPath, texturePath))
		{
			return false;
		}
		return true;
	}

   void object::render(fps_camera camera_)
   {
      model_.render(camera_, world_, light_);
   }
}
