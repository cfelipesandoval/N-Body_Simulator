#include <include/definitions.h> // Need to figure out how to define in a .cpp file instead of in the .h

int main(void)
{
  // // Initialize Window
  if(initWindow()) return -1;

  // Initialize Shaders
  if(initShaders()) return -1;

  // // Initialize bodies
  vector<CelestialBody*> bodies;

  // CelestialBody::MAX_TRAIL_POINTS = 1000;

  vec3 positions[3] = {vec3(0,0,0), vec3(-1,0,-1), vec3(0,0,5)};
  vec3 velocities[3] = {vec3(0,0,0), vec3(-1,0,-1), vec3(0,0,5)};
  vec3 colors[3] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};
  double masses[3] = {1, 1, 1};


  for(int i = 0 ; i < 3 ; i++)
  {
    bodies.push_back(new CelestialBody(positions[i], velocities[i], colors[i], masses[i], CBShaderHandleArray, CBBufferArray, trailBufferArray, vertNum));
  }

  double lastTime = glfwGetTime();

  // Position of light
  vec3 lightPos = vec3(0, 0, 0); // maybe add to the shader so the "sun" glows
  bodies[0]->RK4_step(0);
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
      el->setPosition(vec3(el->getBodyNum() * 5 * cos(2 * currentTime + el->getBodyNum() * 2), 0, el->getBodyNum() * 5 * sin(2 * currentTime + el->getBodyNum() * 2)));

      el->display(ProjectionMatrix, ViewMatrix, lightPos);
    }

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // Check if the ESC key was pressed or the window was closed

  cleanUp();

  return 0;
}

