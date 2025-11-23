#include <include/definitions.h>
#include <include/shader.hpp>
#include <include/texture.hpp>
#include <include/controls.hpp>
#include <include/objloader.hpp>
#include <include/vboindexer.hpp>
#include <include/CelestialBody.h>

GLFWwindow* window;

int main(void)
{
  // Initialize Window and Shaders
  if(initWindow()) return -1;

  // Initialize Shaders
  if(initShaders()) return -1;

  srand(time(0));

  // // Predefined Body Initializations
  // twoBody();
  // orbitingFig8();
  // fig8();
  // randomBodies(200);
  solarSystem();

  CelestialBody::setOrder(10);
  // CelestialBody::MAX_TRAIL_POINTS = 1000;

  do
  {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update Bodies
    CelestialBody::update();
    // Display Bodies
    CelestialBody::display();

    // // Handle user input
    handleUserInput();
    handleSpawnRandom(15, 10, 3, 1e-9, 1e-10); // Press Enter Key
    
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // Check if the ESC key was pressed or the window was closed

  cleanUp();

  return 0;
}

