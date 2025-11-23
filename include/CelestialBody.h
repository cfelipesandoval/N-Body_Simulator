#pragma once

#include <include/definitions.h>

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

static float MAX_DISTANCE = 2000.0f; // Max distance of body before deleting

/**
 * @brief Enum for camera follow type
 * 
 */
enum CameraFollow
{
  BODY = 1,
  BIGGEST = 0,
  COM = -1,
  NONE = -2
};

class CelestialBody
{
public:
  static vector<CelestialBody*> bodies; // Vector containing all active bodies
  static float G; // Gravitational constant
  static int MAX_TRAIL_POINTS; // Size of trail vector to display

  // // Static Functions

  /**
   * @brief Set the Time step for simulation
   * 
   * @param dt Time step
   */
  static void setTimeStep(float dt);

  /**
   * @brief Set the how many frames to skip before drawing
   * 
   * @param frames Number of frames
   */
  static void setSkipFrames(int frames);

  /**
   * @brief Get the Time Step 
   * 
   * @return float Time step
   */
  static float getTimeStep();

  /**
   * @brief Get the number of frames skipped
   * 
   * @return int Frames skipped
   */
  static int getSkipFrames();

  /**
   * @brief Get Position of Center of Mass
   * 
   * @return vec3 position
   */
  static vec3 getCOM();
  
  /**
   * @brief Get position of most massive body
   * 
   * @return vec3 position
   */
  static vec3 getBiggestPos();
  

  /**
   * @brief Set the RK order
   * 
   * @param order 4, 10, or 14
   */
  static void setOrder(int order);
  
  /**
   * @brief Get position of body currently being followed
   * 
   * @return vec3 position
   */
  static vec3 getBodyFollow();
  
  /**
   * @brief Set camera to follow most massive body
   * 
   */
  static void followBiggest();
  
  /**
   * @brief Set camera to follow center of mass
   * 
   */
  static void followCOM();
  
  /**
   * @brief Enable display of trails
   * 
   */
  static void enableAllTrails();
  
  /**
   * @brief Disable display of trails
   * 
   */
  static void disableAllTrails();
  
  /**
   * @brief Update position of bodies
   * 
   * @param dt time step
   */
  static void update(float dt = timeStep);

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
  static void display(vec3 lightPos = vec3(0,0,0));
  
  // // Class Functions
  
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
  CelestialBody(vec3 p, vec3 v, vec3 c, float m, vector<GLuint*> shaderHandleArray = CBShaderHandleArray, vector<GLuint*> bufferArray = CBBufferArray, vector<GLuint*> trailBufferArray = CBTrailBufferArray, int size = vertNum);
  
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

  /**
   * @brief Set body as light source
   * 
   */
  void setAsLightSource();

private:
  // // Static functions and variables
  static CelestialBody* followBody; // Pointer to body to follow
  static CameraFollow follow; // follow body type
  static vector<vector<int>> (*orderPtr)(float); // Function pointer for which RK order to use
  static float timeStep; // Time step size
  static int skipFrames; // Frames to skip before drawing to screen
  static bool displayAllTrails; // Bool to display trails

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
   * @brief Helper for RK4
   * 
   * @param poss Positions
   * @param vels Velocities
   * @param mass Masses
   * @param rads Radii
   * @param KRcurr K matrix for position
   * @param KVcurr K matrix for velocity 
   * @return vector<vector<int>> Vector of k values for RK algorithm
   */
  static vector<vector<int>> RK4_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<float> &rads,  vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  
  /**
   * @brief Helper for RK10
   * 
   * @param poss Positions
   * @param vels Velocities
   * @param mass Masses
   * @param rads Radii
   * @param KRcurr K matrix for position
   * @param KVcurr K matrix for velocity 
   * @return vector<vector<int>> Vector of k values for RK algorithm
   */
  static vector<vector<int>> RK10_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<float> &rads,  vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  
  /**
   * @brief Helper for RK14
   * 
   * @param poss Positions
   * @param vels Velocities
   * @param mass Masses
   * @param rads Radii
   * @param KRcurr K matrix for position
   * @param KVcurr K matrix for velocity 
   * @return vector<vector<int>> Vector of k values for RK algorithm
   */
  static vector<vector<int>> RK14_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<float> &rads,  vector<vec3> &KRcurr, vector<vec3> &KVcurr); // Put in private
  
  /**
   * @brief Combine bodies if they touch
   * 
   * @param combine Vector of bodies to combine
   */
  static void combineBodies(vector<vector<int>> combine);

  // // Class variables and functions
  vec3 position = vec3(0,0,0); // Position vector
  vec3 velocity = vec3(0,0,0); // Velocity vector
  vec3 color = vec3(1,1,1); // Color vector
  float mass = 1; // Mass
  float radius = 0.5; // Radius
  int numVertices = 0; // Number of vertices for drawing
  int bodyNum; // Number in list of existing bodies
  float isLightSource = 0.0; // is light source
  
  
  // Trail Variables
  bool displayTrail = true; // Display trail bool
  vector<vec3> trailPoints; // Buffer for trail
  GLuint trailingTailBufferData; // Buffer containing trail for shader
  
  // Handles
  GLuint CelestialBodyID;
  GLuint MatrixIDCelestialBody;
  GLuint ViewMatrixIDCelestialBody;
  GLuint ModelMatrixIDCelestialBody;
  GLuint colorIDCelestialBody;
  GLuint isLightSourceIDCelestialBody;
  GLuint LightIDCelestialBody;
  GLuint vertexBufferCelestialBody;
  GLuint uvBufferCelestialBody;
  GLuint normalBufferCelestialBody;
  GLuint elementBufferCelestialBody;
  GLuint trailingTailID;
  GLuint MatrixIDTrailingTail;
  GLuint trailingTrailColorID;

  /**
   * @brief Enable the trail for body
   * 
   */
  void enableTrail();

  /**
   * @brief Disable trail for body
   * 
   */
  void disableTrail();
};