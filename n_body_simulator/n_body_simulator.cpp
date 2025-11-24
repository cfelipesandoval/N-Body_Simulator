#include <include/definitions.h>
#include <include/shader.hpp>
#include <include/texture.hpp>
#include <include/controls.hpp>
#include <include/objloader.hpp>
#include <include/vboindexer.hpp>
#include <include/CelestialBody.h>

GLFWwindow* window;

const int windowWidth = 1024;
const int windowHeight = 768;


int main(int argc, char ** argv)
{
  
  // Initialize Window and Shaders
  if(initWindow(windowWidth, windowHeight)) return -1;

  // Initialize Shaders
  if(initShaders()) return -1;

  srand(time(0)); // Initialize random seed

  // // Predefined Body Initializations
  // solarSystem();
  randomBodies(200);
  // twoBody();
  // fig8();
  // orbitingFig8();

  CelestialBody::setOrder(10);
  // CelestialBody::MAX_TRAIL_POINTS = 1000;

  do
  {
    // Update Bodies
    CelestialBody::update();
    // Display Bodies
    CelestialBody::display();

    // // Handle user input
    handleUserInput();
    handleSpawnRandom(15, 5, 3, 1e-9, 1e-10); // Press Enter Key

  } while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // Check if the ESC key was pressed or the window was closed

  cleanUp();

  return 0;
}

