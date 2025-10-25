#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class CelestialBody
{
public:
  CelestialBody(vec3, vec3);
  
  vec3 getPosition();
  void addPosition(vec3);
  vec3 getColor();
  void setPosition(vec3);
  void setColor(vec3);

  static int bodyCount;

  int bodyNum;

private:
  vec3 position;
  vec3 color;
};