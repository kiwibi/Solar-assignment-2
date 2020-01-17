// neon_dithering.cpp

#include "neon_dithering.h"

namespace neon{
   bool Dithering::Create() {
      if (!program_.create("assets/Dithering/dithering_vertex_shader.txt",
                           "assets/Dithering/dithering_fragment_shader.txt")) {
         return false;
      }

      if (!sampler_.create(GL_NEAREST,
                           GL_CLAMP_TO_EDGE,
                           GL_CLAMP_TO_EDGE))
      {
         return false;
      }

      return true;
   }

   bool Dithering::IsValid() const {
      return id_ != 0;
   }

   void Dithering::Bind() const {
      // Might not be neccessary 
   }

   void Dithering::Dither(framebuffer* image) {
      sampler_.bind();
      program_.bind();

      // here I want to send the texture data from the framebuffer to the shader_program
      // Does that mean I need to create the buffer here?
   }
}