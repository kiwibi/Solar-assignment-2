// neon_dithering.h

#ifndef NEON_DITHERING_H_INCLUDED
#define NEON_DITHERING_H_INCLUDED

#include "neon_graphics.h"
#include "neon_framebuffer.h"

#include <neon_core.h>
#include <neon_opengl.h>

namespace neon {
   struct Dithering {
      struct vertex {
         glm::vec2 position_;
      };
      
      bool Create(); // create the shader_program and formats.

      bool IsValid() const;
      void Bind() const;
      void Dither (framebuffer *image); // This should run the image trough the shader_program to apply the effect

      shader_program program_;
      sampler_state sampler_;

      GLuint id_;
   };
}

#endif //NEON_DITHERING_H_INCLUDED
// !neon_dithering.h