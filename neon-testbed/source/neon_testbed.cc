// neon_testbed.cc

#include "neon_testbed.h"
#include <cassert>

namespace neon {
	// note: application factory
	application* application::create(int& width, int& height, string& title) {
		width = 1280; height = 720;
		title = "neon-testbed";
		return new testbed;
	}

	namespace opengl {
		GLuint create_shader(GLenum type, const char* source) {
			GLuint id = glCreateShader(type);
			glShaderSource(id, 1, &source, nullptr);
			glCompileShader(id);
			return id;
		}

		GLuint create_program(GLuint vid, GLuint fid) {
			GLuint id = glCreateProgram();
			glAttachShader(id, vid);
			glAttachShader(id, fid);
			glLinkProgram(id);

			GLint status = 0;
			glGetProgramiv(id, GL_LINK_STATUS, &status);
			if (status == GL_FALSE) {
				char vsh_err[1024] = {};
				char fsh_err[1024] = {};
				char sh_err[1024] = {};

				glGetShaderInfoLog(vid, sizeof(vsh_err), nullptr, vsh_err);
				glGetShaderInfoLog(fid, sizeof(fsh_err), nullptr, fsh_err);
				glGetProgramInfoLog(id, sizeof(sh_err), nullptr, sh_err);

				assert(false);

				return 0;
			}

			glDeleteShader(vid);
			glDeleteShader(fid);

			return id;
		}
	} // !opengl

	// note: derived application class
	testbed::testbed()
		: rotation_(0.0f)
		, controller_(camera_, keyboard_, mouse_) // controller demands to have these, so we have to send it
		, orbit_(0.0f)
		, teapotLength_(false)
	{
	}

	bool testbed::enter() {
		GLuint vao = 0; // vertex array object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		camera_.set_perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f); //Just changing it so that our perspective is controlled by the camera instead.

      if (!frameCounter_.create()) {
         return false;
      }

		if (!skybox_.create()) {
			return false;
		}

		if(!object1_.create(glm::mat4(1.0f), "assets/model/teapot_diffuse.png", "assets/model/teapot.glb")){
			return false;
		}
		if (!object2_.create(glm::mat4(1.0f), "assets/model/teapot_diffuse.png", "assets/model/teapot.glb")) {
			return false;
		}
		if (!object3_.create(glm::mat4(1.0f), "assets/model/teapot_diffuse.png", "assets/model/teapot.glb")) {
			return false;
		}
		if (!object4_.create(glm::mat4(1.0f), "assets/model/teapot_diffuse.png", "assets/model/teapot.glb")) {
			return false;
		}

		object1_.world_ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f));
		object2_.world_ = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, -20.0f));
		object2_.world_ = glm::rotate(glm::mat4(1.0f),
									 90.0f,
									 glm::vec3(0.0f, 1.0f, 0.0f));
		object3_.world_ = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, -20.0f));
		object4_.world_ = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.0f, -20.0f));


		framebuffer_format formats[] = { FRAMEBUFFER_FORMAT_RGBA8 };
		if(!framebuffer_.create(240, 135, _countof(formats), formats, FRAMEBUFFER_FORMAT_D32)) {
			return false;
		}
      
      return true;
   }

   void testbed::exit() {
   }

   bool testbed::tick(const time &dt) {
      if (keyboard_.is_pressed(KEYCODE_ESCAPE)) {
         return false;
      }

      controller_.update(dt); // run controller update

		//spin teapot nr 1
		object1_.world_ = glm::rotate(object1_.world_,
								  rotation_,
								  glm::vec3(0.0f, 1.0f, 0.0f));

		rotation_ = dt.as_seconds();
		//orbit teapot nr 2
		glm::vec3 pos_;
		orbit_ += dt.as_seconds();
		pos_.x = (float)object1_.world_[3][0] + glm::cos(orbit_) * 10;
		pos_.y = (float)object1_.world_[3][1] + glm::sin(orbit_) * 10;
		pos_.z = -20;
		object2_.world_ = glm::translate(glm::mat4(1.0f), pos_);
		//scale teapot nr 3 up & down
		if (teapotLength_ == false)
		{
			object3_.world_ = glm::scale(object3_.world_, glm::vec3(1, 1.001f, 1));
			if (object3_.world_[1][1] >= 4)
				teapotLength_ = true;
		}
		else
		{
			object3_.world_ = glm::scale(object3_.world_, glm::vec3(1, 0.999f, 1));
			if (object3_.world_[1][1] <= 1)
				teapotLength_ = false;
		}
		//	FPS claculations
		int time = (int)(1.0f / dt.as_seconds());
		string fps = std::to_string(time);
      //render fps counter
      frameCounter_.render(0.0f, 1.0f, "FPS: " + fps);

		framebuffer_.bind();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//	render objects
		skybox_.render(camera_);

		object1_.model_.render(camera_, object1_.world_);
		object2_.model_.render(camera_, object2_.world_);
		object3_.model_.render(camera_, object3_.world_);
		object4_.model_.render(camera_, object4_.world_);

		framebuffer::unbind(1280, 720);

		framebuffer_.blit(0, 0, 1280, 720);

      // flush fps counter to actually draw it.
      frameCounter_.flush();

      return true;
   }
} // !neon
