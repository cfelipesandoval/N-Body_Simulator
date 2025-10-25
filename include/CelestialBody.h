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
  CelestialBody(vec3 p, vec3 c, vector<GLuint*> CBShaderHandleArray, vector<GLuint*> CBBufferArray, vector<GLuint*> trailBufferArray, int size);
  
  vec3 getPosition();
  void addPosition(vec3);
  vec3 getColor();
  void setPosition(vec3);
  void setColor(vec3);
  void draw(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 lightPos);

  static int bodyCount;
  static int MAX_TRAIL_POINTS;

  int bodyNum; // need to make a getter

  vector<vec3> trailPoints; // Buffer for trail
  GLuint trailingTailBufferData;

private:
  vec3 position;
  vec3 color;
  int vertNum;

  GLuint CelestialBodyID;
  GLuint MatrixIDCelestialBody;
  GLuint ViewMatrixIDCelestialBody;
  GLuint ModelMatrixIDCelestialBody;
  GLuint colorIDCelestialBody;
  GLuint LightIDCelestialBody;
  GLuint vertexBufferCelestialBody;
  GLuint uvBufferCelestialBody;
  GLuint normalBufferCelestialBody;
  GLuint elementBufferCelestialBody;
  GLuint trailingTailID;
  GLuint MatrixIDTrailingTail;
  GLuint trailingTrailColorID;
};