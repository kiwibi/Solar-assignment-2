// neon_dithering.cpp

#include "neon_dithering.h"

namespace neon{
   bool Dithering::Create() {
      if (!program_.create("assets/Dithering/dithering_vertex_shader.txt",
                           "assets/Dithering/dithering_fragment_shader.txt")) {
         return false;
      }

      format_.add_attribute(0, 2, GL_FLOAT, GL_FALSE); // NOTE: I think pixels in ints are easier to work with


      if (!buffer_.create(512, nullptr)) { // NOTE: NOT DONE JUST COPIED FROM FONT
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
      // here I want to send the texture data from the framebuffer to the shader_program
      // Does that mean I need to create the buffer here?
   }
}