#pragma once

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

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
using namespace glm;

#include <iostream>
using namespace std;

static GLuint VertexArrayID;

static vector<GLuint*> CBShaderHandleArray;
static vector<GLuint*> CBBufferArray;
static vector<GLuint*> CBTrailBufferArray;
static int vertNum;

static GLuint CelestialBodyID;
static GLuint MatrixIDCelestialBody;
static GLuint ViewMatrixIDCelestialBody;
static GLuint ModelMatrixIDCelestialBody;
static GLuint colorIDCelestialBody;
static GLuint isLightSourceIDCelestialBody;
static GLuint LightIDCelestialBody;

static GLuint vertexBufferCelestialBody;
static GLuint uvBufferCelestialBody;
static GLuint normalBufferCelestialBody;
static GLuint elementBufferCelestialBody;

static GLuint trailingTailID;
static GLuint MatrixIDTrailingTail;
static GLuint trailingTrailColorID;


static bool showTrail = true;
static int follow;
static bool darkMode = true;

int initWindow(int width = 1331, int height = 998);
int initShaders();
void cleanUp();

void solarSystem();
void orbitingFig8();
void fig8();
void twoBody();
void randomBodies(int numBodies);

void handleSpawnRandom(int bodies = 5, float maxRad = 80.0f, float maxVel = 2.0f, float meanMass = 0.1f, float stdMass = 0.01f);
void handleUserInput();