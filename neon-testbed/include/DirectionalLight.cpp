// DirectionalLight.cpp

#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
   direction_ = glm::vec3(0.0f);
   colour_ = glm::vec3(0.0f);
   power_ = 0.0f;
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 colour, float power)
{
   direction_ = direction;
   colour_ = colour;
   power_ = power;
}
