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

void DirectionalLight::update()
{
}

void DirectionalLight::rotate(float amount)
{
   direction_.x = ( ( direction_.x * glm::cos(amount) ) - ( direction_.z * glm::sin(amount) ) );
   direction_.z = ( ( direction_.x * glm::sin(amount) ) + ( direction_.z * glm::cos(amount) ) );

   glm::vec2 tmp(direction_.x, direction_.z);
   tmp = glm::normalize(tmp);
   direction_.x = tmp.x;
   direction_.z = tmp.y;
}

void DirectionalLight::ChangeColour(float amount)
{
   colour_.g += amount;
   colour_.b += amount;

   colour_ = glm::clamp(colour_, glm::vec3(0.0f), glm::vec3(1.0f));
}
