#pragma once

#include <vector>
#include <algorithm> 

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

class CelestialBody
{
public:
  static vector<CelestialBody*> bodies;
  static float G;
  static int MAX_TRAIL_POINTS;

  CelestialBody(vec3 p, vec3 v, vec3 c, float m, vector<GLuint*> CBShaderHandleArray, vector<GLuint*> CBBufferArray, vector<GLuint*> trailBufferArray, int size);

  void addPosition(vec3);
  void addVelocity(vec3 vel);

  // // Getters
  vec3 getPosition();
  vec3 getVelocity();
  vec3 getColor();
  float getMass();
  float getRadius();
  int getBodyNum();

  // // Setters
  void setPosition(vec3);
  void setVelocity(vec3);
  void setColor(vec3);
  void setMass(float);
  void setRadius(float);

  static void RK4_step(float dt);
  void display(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 lightPos);

private:
  static void getK(vector<vec3> &poss, vector<float> &mass, vector<vec3> &KRcurr); // Put in private
  vec3 position;
  vec3 velocity;
  vec3 color;
  float mass;
  float radius = 0.5;
  int numVertices;
  int bodyNum; // need to make a getter

  vector<vec3> trailPoints; // Buffer for trail
  GLuint trailingTailBufferData;
  
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