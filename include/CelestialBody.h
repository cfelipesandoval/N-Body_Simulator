#pragma once

#include <vector>
#include <algorithm>
#include <omp.h>
#include <iostream>
#include <chrono>



#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

class CelestialBody
{
public:
  static void RK4_step(float dt);
  static void cameraFollow(int body);
  static void cameraStopFollowing();
  static void display(vec3 lightPos);

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

private:
  static void getK(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  // Variables for displaying
  static int follow;

  void enableTrail();
  void disableTrail();

  static void enableAllTrails();
  static void disableAllTrails();

  vec3 position = vec3(0,0,0);
  vec3 velocity = vec3(0,0,0);
  vec3 color = vec3(1,1,1);
  float mass = 1;
  float radius = 0.5;
  int numVertices = 0;
  int bodyNum;
  
  // Trail Variables
  bool displayTrail = true;
  vector<vec3> trailPoints; // Buffer for trail
  GLuint trailingTailBufferData;
  
  // Handles
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