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

#include <include/shader.hpp>
#include <include/texture.hpp>
#include <include/controls.hpp>
#include <include/objloader.hpp>
#include <include/vboindexer.hpp>
#include <include/CelestialBody.h>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <ctime>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#include <glm/gtc/random.hpp>


/**
 * @brief Initialize OGL window
 * 
 * @return int 
 */
int initWindow(int width, int height)
{
  // Initialize GLFW
  if(!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n" );
    getchar();
    return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow(width, height, "N-Body Simulator: ", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);
    
  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return 1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  // Hide the mouse and enable unlimited movement
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  
  // Set the mouse at the center of the screen
  glfwPollEvents();
  glfwSetCursorPos(window, 1024/2, 768/2);

  // Dark blue background
  // glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
  // Black Background
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it is closer to the camera than the former one
  glDepthFunc(GL_LESS); 

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  return 0;
}

/**
 * @brief Initialize Shaders
 * 
 * @return int 
 */
int initShaders()
{
  // // Initializations for CelestialBody models
  // Create and compile our GLSL program from the shaders
  CelestialBodyID = LoadShaders("resources/shaders/CelestialBodyShading.vertexshader", "resources/shaders/CelestialBodyShading.fragmentshader");

  // Maybe add these to a larger array that holds their values so you can pass them into ech object?
  MatrixIDCelestialBody = glGetUniformLocation(CelestialBodyID, "MVP");
  ViewMatrixIDCelestialBody = glGetUniformLocation(CelestialBodyID, "V");
  ModelMatrixIDCelestialBody = glGetUniformLocation(CelestialBodyID, "M");
  colorIDCelestialBody = glGetUniformLocation(CelestialBodyID, "COLOR");
  isLightSourceIDCelestialBody = glGetUniformLocation(CelestialBodyID, "isLight");
  LightIDCelestialBody = glGetUniformLocation(CelestialBodyID, "LightPosition_worldspace");

  // // Initialize buffers for trailing tail
  trailingTailID = LoadShaders("resources/shaders/TrailingTailShading.vertexshader", "resources/shaders/TrailingTailShading.fragmentshader");
  MatrixIDTrailingTail = glGetUniformLocation(trailingTailID, "MVP");
  trailingTrailColorID = glGetUniformLocation(trailingTailID, "trailColor");
 
  // Read our .obj file
  vector<vec3> vertices;
  vector<vec2> uvs;
  vector<vec3> normals;

  // Indices for VBO
  vector<unsigned short> indices;
  vector<vec3> indexed_vertices;
  vector<vec2> indexed_uvs;
  vector<vec3> indexed_normals;
  
  loadOBJ("resources/objectFiles/Sphere.obj", vertices, uvs, normals);
  
  indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
  
  glGenBuffers(1, &vertexBufferCelestialBody);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCelestialBody);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &uvBufferCelestialBody);
  glBindBuffer(GL_ARRAY_BUFFER, uvBufferCelestialBody);
  glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(vec2), &indexed_uvs[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &normalBufferCelestialBody);
  glBindBuffer(GL_ARRAY_BUFFER, normalBufferCelestialBody);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(vec3), &indexed_normals[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &elementBufferCelestialBody);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferCelestialBody);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

  CBShaderHandleArray.push_back(&CelestialBodyID);  
  CBShaderHandleArray.push_back(&MatrixIDCelestialBody);  
  CBShaderHandleArray.push_back(&ViewMatrixIDCelestialBody);  
  CBShaderHandleArray.push_back(&ModelMatrixIDCelestialBody);
  CBShaderHandleArray.push_back(&colorIDCelestialBody);
  CBShaderHandleArray.push_back(&isLightSourceIDCelestialBody);
  CBShaderHandleArray.push_back(&LightIDCelestialBody);

  CBBufferArray.push_back(&vertexBufferCelestialBody); 
  CBBufferArray.push_back(&uvBufferCelestialBody); 
  CBBufferArray.push_back(&normalBufferCelestialBody); 
  CBBufferArray.push_back(&elementBufferCelestialBody);
  
  CBTrailBufferArray.push_back(&trailingTailID); 
  CBTrailBufferArray.push_back(&MatrixIDTrailingTail); 
  CBTrailBufferArray.push_back(&trailingTrailColorID);

  vertNum = indices.size();

  return 0;
}

/**
 * @brief Delete everything related to shaders
 * 
 */
void cleanUp()
{
  // Cleanup VBO and shader
  glDeleteProgram(CelestialBodyID);
  glDeleteProgram(MatrixIDCelestialBody);
  glDeleteProgram(ViewMatrixIDCelestialBody);
  glDeleteProgram(ModelMatrixIDCelestialBody);
  glDeleteProgram(colorIDCelestialBody);
  glDeleteProgram(isLightSourceIDCelestialBody);
  glDeleteProgram(LightIDCelestialBody);

  glDeleteBuffers(1, &vertexBufferCelestialBody);
  glDeleteBuffers(1, &uvBufferCelestialBody);
  glDeleteBuffers(1, &normalBufferCelestialBody);
  glDeleteBuffers(1, &elementBufferCelestialBody);

  glDeleteProgram(trailingTailID);
  glDeleteProgram(MatrixIDTrailingTail);
  glDeleteProgram(trailingTrailColorID);

  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

/**
 * @brief Handle user input to spawn random bodies with Enter Key
 * 
 * @param bodies Number of bodies to spawn
 * @param maxRad Max distance to spawn
 * @param maxVel Maximum velocity of bodies
 * @param meanMass Gaussian average mass
 * @param stdMass Gaussian standard deviation of mass
 */
void handleSpawnRandom(int bodies, float maxRad, float maxVel, float meanMass, float stdMass)
{
  // Spawn random bodies
  if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
  {
    vec3 v = CelestialBody::getBodyFollowVel();
    
    for(int i = 0 ; i < bodies ; i++)
    {
      float m = abs(gaussRand(meanMass, stdMass));
      vec3 pos = CelestialBody::getBodyFollow();

      CelestialBody::newBody(ballRand(maxRad) + pos, ballRand(maxVel) + v, ballRand(1.0f), m);
      CelestialBody::bodies[CelestialBody::bodies.size() - 1]->setRadius(m * 0.2f);    
    }
  }
}

/**
 * @brief Handle user input
 * 
 * B: Follow body with largest mass
 * C: Follow Center of Mass
 * N: Stop following body and center at (0,0,0)
 * U: Follow next body
 * Y: Follow previous body
 * T: Toggle trail display
 * RARROW: Increase frames to skip
 * LARROW: Decrease frames to skip
 * V: Toggle background color
 * P: Pause
 * 
 */
void handleUserInput()
{
  // Camera following input
  if(glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) // Follow body with biggest masss
  {
    CelestialBody::followBiggest();
  }
  else if(glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // Follow center of mass
  {
    CelestialBody::followCOM();
  }
  else if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) // Go back to origin
  {
    CelestialBody::cameraStopFollowing();
  }
  
  if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE) // Follow body with biggest masss
    {
      if(follow < CelestialBody::bodies.size() - 1) follow++;
      CelestialBody::cameraFollow(follow);
    }
    if(glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
    {
      if(follow > 0) follow--;
      CelestialBody::cameraFollow(follow);
    }
  
  // Toggle trail display
  if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
  {
    showTrail ? CelestialBody::disableAllTrails() : CelestialBody::enableAllTrails();
    showTrail = !showTrail;
  }
  
  // Update frames skipped
  if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) // Increase skips
  {
    CelestialBody::setSkipFrames(CelestialBody::getSkipFrames() + 5);
  }
  if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // Decrease skips
  {
    CelestialBody::setSkipFrames(CelestialBody::getSkipFrames() - 5);
    if(CelestialBody::getSkipFrames() <= 0) 
    {
      CelestialBody::setSkipFrames(5);
    }
  }

  // Change background color
  if(glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) 
  {
    if(darkMode)
    {
      darkMode = false;
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
      darkMode = true;
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    }
  }
  
  // Pause (Don't update)
  if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) 
  {
    CelestialBody::setSkipFrames(0);
  }
}


// // Premade scenarios

/**
 * @brief Solar system
 * 
 */
void solarSystem()
{
  CelestialBody::MAX_TRAIL_POINTS = 1000;
  CelestialBody::setTimeStep(0.00003);

  //// Initializing Initial Conditions
  float G = 4 * pi<float>() * pi<float>();
  CelestialBody::G = G;
  // Sun
  vec3 pos0 = vec3(0, 0, 0); 
  vec3 vel0 = vec3(0, 0, 0);
  float M0 = 1;
  
  // Mercury
  vec3 pos1 = vec3(0.3877005348 , 0, 0); // In AU
  float v = sqrt(G * M0 / 0.3877005348);
  vec3 vel1 = vec3(0, v , 0); // in AU/year
  float M1 = 1.652e-7; // In solar masses
  
  // Venus
  vec3 pos2 = vec3(0.7279411765 , 0, 0) ;// In AU
  v = sqrt(G * M0 / 0.7279411765);
  vec3 vel2 = vec3(0, v , 0); // in AU/year
  float M2 = 2.447e-6; // In solar masses
  
  // Earth
  vec3 pos3 = vec3(1, 0, 0); // In AU
  v = sqrt(G * M0 / pos3[0]);
  // vec3 vel3 = vec3(0, 6.27777651918 , 0]) // in AU/year
  vec3 vel3 = vec3(0,v,0);
  float M3 = 0.000003; // In solar masses
  
  // Mars
  vec3 pos4 = vec3(1.524064171 , 0, 0); // In AU
  v = sqrt(G * M0 / pos4[0]);
  vec3 vel4 = vec3(0, v , 0); // in AU/year
  float M4 = 3.213e-7; // In solar masses
  
  // Jupiter
  vec3 pos5 = vec3(5.063770053 , 0, 0); // In AU
  v = sqrt(G * M0 / pos5[0]);
  vec3 vel5 = vec3(0, v , 0); // in AU/year
  float M5 = 9.543e-4; // In solar masses

  // Saturn
  vec3 pos6 = vec3(9.639037433 , 0, 0); // In AU
  v = sqrt(G * M0 / pos6[0]);
  vec3 vel6 = vec3(0, v , 0); // in AU/year
  float M6 = 2.857e-4; // In solar masses
  
  // Uranus
  vec3 pos7 = vec3(19.1909893 , 0, 0); // In AU
  v = sqrt(G * M0 / pos7[0]);
  vec3 vel7 = vec3(0, v , 0); // in AU/year
  float M7 = 4.365e-5; // In solar masses
  
  // Neptune
  vec3 pos8 = vec3(29.88970588 , 0, 0); // In AU
  v = sqrt(G * M0 / pos8[0]);
  vec3 vel8 = vec3(0, v , 0); // in AU/year
  float M8 = 5.149e-5; // In solar masses
  
  // Adding all to arrays

  vector<float> masses = {M0,M1,M2,
                          M3,M4,M5,
                          M6,M7,M8};

  vector<vec3> positions = {pos0,pos1,pos2,
                            pos3,pos4,pos5,
                            pos6,pos7,pos8};
  vector<vec3> velocities = {vel0,vel1,vel2,
                            vel3,vel4,vel5,
                            vel6,vel7,vel8};

  vector<vec3> colors = {vec3(1,1,0), vec3(0.5333333333333333,0.5333333333333333,0.5333333333333333), vec3(1,0.5254901960784314,0.1843137254901961),
                        vec3(0.34509803921568627,0.7686274509803922,0.8666666666666667), vec3(0.9882352941176471,0.3843137254901961,0.3333333333333333), vec3(0.5450980392156862,0.27058823529411763,0.07450980392156863), 
                        vec3(0.803921568627451,0.5215686274509804,0.24705882352941178), vec3(0.611764705882353,0.8627450980392157,0.9215686274509803), vec3(0.10980392156862745,0.4588235294117647,0.5411764705882353)};
  
  // vector<float> rads = {0.05 / 4, 0.001 / 4, 0.001 / 4, 0.001 / 4, 0.0001 / 4, 0.001 / 4, 0.001 / 4, 0.001 / 4, 0.001 / 4, 0.001 / 4};
  vector<float> rads = {0.004654747, 1.63104e-05, 4.04551e-05, 4.26343e-05, 2.25672e-05, 0.000477895, 0.000402867, 0.000170851, 0.000164588};
  
  // Switch y and z coordinates
  for(int i = 0 ; i < positions.size() ; i++)
  {
    positions[i] = vec3(positions[i].x, positions[i].z, positions[i].y);
  }

  for(int i = 0 ; i < velocities.size() ; i++)
  {
    velocities[i] = vec3(velocities[i].x, velocities[i].z, velocities[i].y);
  }

  for(int i = 0 ; i < masses.size() ; i++)
  {
    CelestialBody* body = CelestialBody::newBody(positions[i], velocities[i], colors[i], masses[i]);
    body->setRadius(rads[i]);
  }

  // Earth's moon
  v = sqrt(G * M3 / (0.0026));
  CelestialBody* body = CelestialBody::newBody(positions[3] + vec3(0.0026,0,0), velocities[3] + vec3(0,0,v), vec3(0.5,0.5,0.5), 3.69e-8);
  body->setRadius(1.16138e-05);

  // Spawn 15 Random Bodies
  // for(int i = 0 ; i < 15 ; i++)
  // {
  //   float m = abs(gaussRand(1e-9, 1e-10));
  //   vec3 pos = CelestialBody::getBodyFollow();
  //   CelestialBody* body = CelestialBody::newBody(ballRand(5.0f) + pos, ballRand(3.0f), ballRand(1.0f), m);
  //   body->setRadius(m * 0.2f);
  // }

  follow = 0;
  CelestialBody::cameraFollow(follow);
}

/**
 * @brief Random bodies
 * 
 * @param numBodies Number of random bodies to spawn
 */
void randomBodies(int numBodies)
{
  CelestialBody::MAX_TRAIL_POINTS = 1000;
  CelestialBody::cameraFollow(-1);
  CelestialBody::setTimeStep(0.001);

  float m = abs(gaussRand(5e-2, 1e-2));
  float radMax = 3;
  float vMax = 0.5;
  float separation = 100;

  // Spawn random bodies
  // for(int i = 0 ; i < 4 ; i++)
  // {
    vec3 initPos = ballRand(separation);
    for(int j = 0 ; j < numBodies ; j++)
    {
      CelestialBody* body = CelestialBody::newBody(ballRand(radMax), ballRand(vMax), ballRand(1.0f), m);
      body->setRadius(m * 0.2f);
    }
    
  // }
}

/**
 * @brief System of Figure 8 orbiting 
 * 
 */
void orbitingFig8()
{
  CelestialBody::MAX_TRAIL_POINTS = 1000;
  CelestialBody::setTimeStep(0.001);

  float G = 1;
  CelestialBody::G = G;
  
  float r = 25;
  
  vec3 v = vec3(0.3471128135672417, 0.532726851767674, 0);

  vec3 pos0 = vec3(0,0,0);
  vec3 vel0 = vec3(0,0,0) + vec3(0,0,1);
  
  vec3 pos1 = vec3(1, 0, 0);
  vec3 pos2 = vec3(-1, 0, 0);
  vec3 pos3 = vec3(0, 0, 0);
  
  vec3 pos4 = vec3(1, 0, 0) + vec3(1,0,0) * r;
  vec3 pos5 = vec3(-1, 0, 0) + vec3(1,0,0) * r;
  vec3 pos6 = vec3(0, 0, 0) + vec3(1,0,0) * r;
  
  vec3 pos7 = vec3(1, 0, 0) - vec3(1,0,0) * r;
  vec3 pos8 = vec3(-1, 0, 0) - vec3(1,0,0) * r;
  vec3 pos9 = vec3(0, 0, 0) - vec3(1,0,0) * r;
  
  vec3 pos10 = vec3(1, 0, 0) + vec3(0,1,0) * r;
  vec3 pos11 = vec3(-1, 0, 0) + vec3(0,1,0) * r;
  vec3 pos12 = vec3(0, 0, 0) + vec3(0,1,0) * r;
  
  vec3 pos13 = vec3(1, 0, 0) - vec3(0,1,0) * r;
  vec3 pos14 = vec3(-1, 0, 0) - vec3(0,1,0) * r;
  vec3 pos15 = vec3(0, 0, 0) - vec3(0,1,0) * r;

  vec3 vel1 = v;
  vec3 vel2 = v;
  vec3 vel3 = (float)-2 * v;
  
  float v2 = sqrt(G * (float)(6)  / r);
    
  vec3 vel4 = v + v2 * vec3(0,1,0);
  vec3 vel5 = v + v2 * vec3(0,1,0);
  vec3 vel6 = (float)-2 * v + v2 * vec3(0,1,0);
  
  vec3 vel7 = v - v2 * vec3(0,1,0);
  vec3 vel8 = v - v2 * vec3(0,1,0);
  vec3 vel9 = (float)-2 * v - v2 * vec3(0,1,0);
  
  vec3 vel10 = v - v2 * vec3(1,0,0);
  vec3 vel11 = v - v2 * vec3(1,0,0);
  vec3 vel12 = (float)-2 * v - v2 * vec3(1,0,0);
  
  vec3 vel13 = v + v2 * vec3(1,0,0);
  vec3 vel14 = v + v2 * vec3(1,0,0);
  vec3 vel15 = (float)-2 * v + v2 * vec3(1,0,0);

  vector<float> masses = {1,1,1,
                          1,1,1,
                          1,1,1,
                          1,1,1,
                          1,1,1};

  vector<vec3> positions = {pos1,pos2,pos3,
                            pos4,pos5,pos6,
                            pos7,pos8,pos9, 
                            pos10, pos11, pos12, 
                            pos13, pos14, pos15};
  vector<vec3> velocities = {vel1,vel2,vel3,
                            vel4,vel5,vel6,
                            vel7,vel8,vel9, 
                            vel10, vel11, vel12, 
                            vel13, vel14, vel15};
  vector<vec3> colors = {vec3(0,1,0), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333),
                        vec3(0,1,0), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), 
                        vec3(0,1,0), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), 
                        vec3(0,1,0), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), 
                        vec3(0,1,0), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), };
  

  // Switch y and z coordinates
  for(int i = 0 ; i < positions.size() ; i++)
  {
    positions[i] = vec3(positions[i].x, positions[i].z, positions[i].y);
  }

  for(int i = 0 ; i < velocities.size() ; i++)
  {
    velocities[i] = vec3(velocities[i].x, velocities[i].z, velocities[i].y);
  }

  for(int i = 0 ; i < masses.size() ; i++)
  {
    CelestialBody::newBody(positions[i], velocities[i], colors[i], masses[i]);
    CelestialBody::bodies[i]->setRadius(0.125);
  }
}

/**
 * @brief Figure 8 3-body system
 * 
 */
void fig8()
{
  CelestialBody::MAX_TRAIL_POINTS = 1000;
  CelestialBody::setTimeStep(0.001);

  float G = 1;
  CelestialBody::G = G;
  vec3 v = vec3(0.3471128135672417, 0, 0.532726851767674);
  
  vec3 pos1 = (float)3 * vec3(1,0,0);
  vec3 pos2 = (float)3 * vec3(-1,0,0);
  vec3 pos3 = vec3(0,0,0);

  vec3 vel1 = v;
  vec3 vel2 = v;
  vec3 vel3 = (float)-2 * v;

  vector<vec3> positions = {pos1,pos2,pos3};
  vector<vec3> velocities = {vel1,vel2,vel3};
  vector<vec3> colors = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};
  vector<float> masses = {3, 3, 3};

  // Initialize Bodies
  for(int i = 0 ; i < masses.size() ; i++)
  {
    CelestialBody::newBody(positions[i], velocities[i], colors[i], masses[i]);
    CelestialBody::bodies[i]->setRadius(0.125);
  }
}

/**
 * @brief Two body system
 * 
 */
void twoBody()
{
  CelestialBody::MAX_TRAIL_POINTS = 1000;
  CelestialBody::setTimeStep(0.0001);
  double G = 4 * pi<double>() * pi<double>();
  CelestialBody::G = G;
  
  double M0 = 1;
  double M1 = 1;
  
  double m = (M0 * M1) / (M0 + M1);
  
  double loc = 1;
    
  double v0 = sqrt(G * m / (2*loc));
  
  // Body 0
  vec3 pos0 = vec3(loc, 0, 0);
  vec3 vel0 = vec3(0, v0, 0);
  
  // Body 1
  vec3 pos1 = vec3(-loc, 0, 0);
  vec3 vel1 = vec3(0, -v0, 0);
  
  vector<vec3> positions = {pos0, pos1};
  vector<vec3> velocities = {vel0, vel1};
  vector<float> masses = {(float)M0, (float)M1};
  vector<vec3> colors = {vec3(1,0,0), vec3(0,1,0)};

  // Switch y and z coordinates
  for(int i = 0 ; i < positions.size() ; i++)
  {
    positions[i] = vec3(positions[i].x, positions[i].z, positions[i].y);
  }

  for(int i = 0 ; i < velocities.size() ; i++)
  {
    velocities[i] = vec3(velocities[i].x, velocities[i].z, velocities[i].y);
  }

  // Initialize Bodies
  for(int i = 0 ; i < masses.size() ; i++)
  {
    CelestialBody::newBody(positions[i], velocities[i], colors[i], masses[i]);
    CelestialBody::bodies[i]->setRadius(0.125);
  }
}
