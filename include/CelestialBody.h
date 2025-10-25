#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

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

  int bodyNum; // need to make a getter

  vector<vec3> trailPoints; // Buffer for trail
  GLuint trailingTailBufferData;

private:
  vec3 position;
  vec3 color;
};