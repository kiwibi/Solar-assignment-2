// neon_dithering.h

#ifndef NEON_DITHERING_H_INCLUDED
#define NEON_DITHERING_H_INCLUDED

#include "neon_graphics.h"

#include <neon_core.h>
#include <neon_opengl.h>

namespace neon {
   struct Dithering {
      
      //void Dither (framebuffer *image); 

      vertex_format format_;
      vertex_buffer buffer_;
      shader_program program_;
      sampler_state state_;
   };
}

#endif //NEON_DITHERING_H_INCLUDED
// !neon_dithering.h