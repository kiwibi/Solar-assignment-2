// DirectionalLight.h

#ifndef DIRECTIONAL_LIGHT_H_INCLUDED
#define DIRECTIONAL_LIGHT_H_INCLUDED

#include <neon_core.h>
#include <neon_opengl.h>

#pragma warning(push) // Because they have unnamned things in here
#pragma warning(disable: 4201)
#pragma warning(disable: 4127)
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma warning(pop)


struct DirectionalLight
{
   DirectionalLight();
   DirectionalLight(glm::vec3 direction, glm::vec3 colour, float power);

   void Rotate(); // maybe send in a direction to rotate in?
   void ChangeColour(); // same as above basically

   glm::vec3 direction_;
   glm::vec3 colour_;
   float power_;
};

#endif //DIRECTIONAL_LIGTH_H_INCLUDED