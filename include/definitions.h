#pragma once

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <include/shader.hpp>
#include <include/texture.hpp>
#include <include/controls.hpp>
#include <include/objloader.hpp>
#include <include/vboindexer.hpp>
#include <include/CelestialBody.h>

#include <iostream>
using namespace std;

GLFWwindow* window;

GLuint VertexArrayID;

vector<GLuint*> CBShaderHandleArray;
vector<GLuint*> CBBufferArray;
vector<GLuint*> trailBufferArray;
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

int initWindow()
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
  window = glfwCreateWindow(1024, 768, "Tutorial 09 - Rendering several models", NULL, NULL);
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
   // Blacl Background
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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
  CBShaderHandleArray.push_back(&LightIDCelestialBody);

  CBBufferArray.push_back(&vertexBufferCelestialBody); 
  CBBufferArray.push_back(&uvBufferCelestialBody); 
  CBBufferArray.push_back(&normalBufferCelestialBody); 
  CBBufferArray.push_back(&elementBufferCelestialBody);
  
  trailBufferArray.push_back(&trailingTailID); 
  trailBufferArray.push_back(&MatrixIDTrailingTail); 
  trailBufferArray.push_back(&trailingTrailColorID);

  vertNum = indices.size();

  return 0;
}

void cleanUp()
{
  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertexBufferCelestialBody);
  glDeleteBuffers(1, &uvBufferCelestialBody);
  glDeleteBuffers(1, &normalBufferCelestialBody);
  glDeleteBuffers(1, &elementBufferCelestialBody);
  glDeleteProgram(CelestialBodyID);
  glDeleteProgram(LightIDCelestialBody);
  glDeleteProgram(trailingTailID);
  // glDeleteTextures(1, &TextureCelestialBody);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}