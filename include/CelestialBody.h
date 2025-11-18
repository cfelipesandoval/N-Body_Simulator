#pragma once

#include <vector>
#include <algorithm>
#include <omp.h>
#include <iostream>
#include <chrono>
#include <set>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

class CelestialBody
{
public:
  // // Static Functions

  /**
   * @brief Take a step using the Runge-Kutta Fourth Order Algorithm
   * 
   * @param dt Time step
   */
  static vector<vector<int>> RK4_step(float dt);

  /**
   * @brief Take a step using the Runge-Kutta Tenth Order Algorithm
   * 
   * @param dt Time step
   */
  static vector<vector<int>> RK10_step(float dt);

  /**
   * @brief Take a step using the Runge-Kutta Fourteenth Order Algorithm
   * 
   * @param dt Time step
   */
  static vector<vector<int>> RK14_step(float dt);

  /**
   * @brief Set camera to follow a specific body
   * 
   * @param body Body index
   */
  static void cameraFollow(int body);

  /**
   * @brief Disable camera following
   * 
   */
  static void cameraStopFollowing();

  /**
   * @brief Display the current state of the system
   * 
   * @param lightPos 
   */
  static void display(vec3 lightPos);

  static vector<CelestialBody*> bodies; // Vector containing all active bodies
  static float G; // Gravitational constant
  static int MAX_TRAIL_POINTS; // Size of trail vector to display

  /**
   * @brief Construct a new Celestial Body object
   * 
   * @param p Initial position
   * @param v Initial velocity
   * @param c color
   * @param m mass
   * @param CBShaderHandleArray Handle Array with {Shader, MVP, V, M, COLOR, Light_position} 
   * @param CBBufferArray Buffer Array with {vertex, uv, normal, elements}
   * @param trailBufferArray Handle Array with {Shader, MVP, COLOR}
   * @param size Number of Vertices
   */
  CelestialBody(vec3 p, vec3 v, vec3 c, float m, vector<GLuint*> CBShaderHandleArray, vector<GLuint*> CBBufferArray, vector<GLuint*> trailBufferArray, int size);
  
  /**
   * @brief Destroy the Celestial Body object
   * 
   */
  ~CelestialBody();

  /**
   * @brief Add to current position
   * 
   * @param pos Position to add
   */
  void addPosition(vec3 pos);

  /**
   * @brief Add to current velocity
   * 
   * @param vel Velocity to add
   */
  void addVelocity(vec3 vel);

  // // Getters

  /**
   * @brief Get the Position 
   * 
   * @return vec3 Position
   */
  vec3 getPosition();

  /**
   * @brief Get the Velocity 
   * 
   * @return vec3 Velocity
   */
  vec3 getVelocity();

  /**
   * @brief Get the Color
   * 
   * @return vec3 Color
   */
  vec3 getColor();

  /**
   * @brief Get the Mass
   * 
   * @return float Mass
   */
  float getMass();

  /**
   * @brief Get the Radius
   * 
   * @return float Radius
   */
  float getRadius();

  /**
   * @brief Get the Index of the body
   * 
   * @return int bodyNum
   */
  int getBodyNum();

  // // Setters

  /**
   * @brief Set the Position
   * 
   * @param pos Position
   */
  void setPosition(vec3 pos);

  /**
   * @brief Set the Velocity
   * 
   * @param vel Velocity
   */
  void setVelocity(vec3 vel);

  /**
   * @brief Set the Color
   * 
   * @param col Color
   */
  void setColor(vec3 col);

  /**
   * @brief Set the Mass
   * 
   * @param m Mass
   */
  void setMass(float m);
  
  /**
   * @brief Set the Radius
   * 
   * @param r Radius
   */
  void setRadius(float r);

  static void update(float dt);

  static void setOrder(int order);

private:
  static vector<vector<int>> RK4_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<float> &rads,  vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  static vector<vector<int>> RK10_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<float> &rads,  vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  static vector<vector<int>> RK14_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<float> &rads,  vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  static void combineBodies(vector<vector<int>> combine);
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
  static vector<vector<int>> (*orderPtr)(float);
  
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