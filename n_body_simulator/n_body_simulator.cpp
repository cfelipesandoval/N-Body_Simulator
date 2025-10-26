#include <include/definitions.h> // Need to figure out how to define in a .cpp file instead of in the .h

int main(void)
{
  // // Initialize Window
  if(initWindow()) return -1;

  // Initialize Shaders
  if(initShaders()) return -1;

  // // Initialize bodies
  vector<CelestialBody*> bodies;

  float G = 1;
  CelestialBody::G = G;
  vec3 v = vec3(0.3471128135672417, 0, 0.532726851767674);

  vector<vec3> positions = {(float)3 * vec3(1,0,0), (float)3 * vec3(-1,0,0), (float)3 * vec3(0,0,0)};
  vector<vec3> velocities = {v, v, (float)-2 * v};
  vector<vec3> colors = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};
  vector<float> masses = {3, 3, 3};


  for(int i = 0 ; i < masses.size() ; i++)
  {
    bodies.push_back(new CelestialBody(positions[i], velocities[i], colors[i], masses[i], CBShaderHandleArray, CBBufferArray, trailBufferArray, vertNum));
    bodies[i]->setRadius(0.125);
  }


  // Position of light
  vec3 lightPos = vec3(1, 1, 1); // maybe add to the shader so the "sun" glows
  double lastTime = glfwGetTime();
	int nbFrames = 0;
  do
  {
    double currentTime = glfwGetTime();

    nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
      printf("%f frames/s\n", double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
      CelestialBody::MAX_TRAIL_POINTS = 500;
      for(int i = 0 ; i < 5 ; i++) CelestialBody::RK4_step(0.001);
    }
    else 
    {
      CelestialBody::MAX_TRAIL_POINTS = 50;
      for(int i = 0 ; i < 50 ; i++) CelestialBody::RK4_step(0.001);
    }
  
    computeMatricesFromInputs();
    mat4 ProjectionMatrix = getProjectionMatrix();
    mat4 ViewMatrix = getViewMatrix();

    lastTime += 1.0;
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute the MVP matrix from keyboard and mouse input

    // Iterate through bodies
    for(auto& el : bodies)
    {
      el->display(ProjectionMatrix, ViewMatrix, lightPos);
    }

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // Check if the ESC key was pressed or the window was closed

  cleanUp();

  return 0;
}

