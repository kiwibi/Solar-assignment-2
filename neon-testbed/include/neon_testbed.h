// neon_testbed.h

#ifndef NEON_TESTBED_H_INCLUDED
#define NEON_TESTBED_H_INCLUDED

#include <neon_core.h>
#include <neon_opengl.h>

#include "neon_graphics.h"
#include "neon_object.h"
#include "neon_framebuffer.h"
#include "DirectionalLight.h"
#include "neon_plane.h"

namespace neon {
   struct vertex {
      float x_, y_, z_;
      uint32 color_;
      float u_, v_;
   };
   struct testbed : application {
      testbed();
      virtual bool enter() final;
      virtual void exit() final;
      virtual bool tick(const time &dt) final;

      shader_program program_;
      vertex_buffer vbo_;
      vertex_format format_;
      texture texture_;
      sampler_state sampler_;

      float rotation_;
		float orbit_;
		bool teapotLength_;
      
      bitmap_font mouseX_; // we want fonts
      bitmap_font mouseY_; // Made another and named both to mouse because I am testing shit
      bitmap_font frameCounter_;
      
      fps_camera camera_; // adding our camera
      controller controller_; // and controller
      skybox skybox_;
      terrain terrain_;

		object object1_;
		object object2_;
		object object3_;
		object object4_;

		framebuffer framebuffer_;

      DirectionalLight light_;

		wall wall_;
   };
} // !neon

#endif // !NEON_TESTBED_H_INCLUDED
