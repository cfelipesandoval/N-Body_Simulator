#include "include/definitions.h"


int main(void)
{
  // // Initialize Window
  if(initWindow()) return -1;

  // Initialize Shaders
  if(initShaders()) return -1;

  // // Initialize bodies
  vector<CelestialBody*> bodies;

  CelestialBody::bodyCount = 0;
  CelestialBody::MAX_TRAIL_POINTS = 1000;

  vec3 positions[3] = {vec3(0,0,0), vec3(-1,0,-1), vec3(0,0,5)};
  vec3 colors[3] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};

  for(int i = 0 ; i < 3 ; i++)
  {
    bodies.push_back(new CelestialBody(positions[i], colors[i], CBShaderHandleArray, CBBufferArray, trailBufferArray, vertNum));
  }

  double lastTime = glfwGetTime();

  // Position of light
  vec3 lightPos = vec3(0, 0, 0); // maybe add to the shader so the "sun" glows
  
  do
  {
    double currentTime = glfwGetTime();

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute the MVP matrix from keyboard and mouse input
    computeMatricesFromInputs();
    mat4 ProjectionMatrix = getProjectionMatrix();
    mat4 ViewMatrix = getViewMatrix();
    
    // Iterate through bodies
    for(auto& el : bodies)
    {
      el->setPosition(vec3(el->bodyNum * 5 * cos(2 * currentTime + el->bodyNum * 2), 0, el->bodyNum * 5 * sin(2 * currentTime + el->bodyNum * 2)));

      el->draw(ProjectionMatrix, ViewMatrix, lightPos);
    }

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

  cleanUp();

  return 0;
}

