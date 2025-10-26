// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

mat4 ViewMatrix;
mat4 ProjectionMatrix;

mat4 getViewMatrix(){
	return ViewMatrix;
}
mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

void computeMatricesFromInputs()
{
	static float initialFoV = 45.0f;

	static float speed = 3.0f; // 3 units / second
	static float mouseSpeed = 0.005f;

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Initial conditions
	static float radius = 25;
	static float theta = radians(15.0);
	static float phi = 0;


	// Move closer to the origin
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		radius -= 5 * speed * deltaTime;
	}
	// Move farther from the origin
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS)
	{
		radius += 5 * speed * deltaTime;
	}
	// Rotate left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS)
	{
		phi -= deltaTime * speed;
	}
	// Rotate right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS)
	{
		phi += deltaTime * speed;
	}
	// Rotate up
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS)
	{
		theta += deltaTime * speed;
	}
	// Rotate down
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS)
	{
		theta -= deltaTime * speed;
	}

	float x,y,z;

	// Make sure camera does not go over theta limits (-90, 90)
	if(theta > radians(90.0)) theta = radians(89.9);
	if(theta < radians(-90.0)) theta = radians(-89.9);

	// Calculation for camera movement around origin
	x = radius * cos(theta) * sin(phi);
	y = radius * sin(theta);
	z = radius * cos(theta) * cos(phi);

	vec3 position = vec3(x,y,z);

	float FoV = initialFoV;

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = perspective(radians(FoV), 4.0f / 3.0f, 0.1f, 10000.0f);
	
	// Camera matrix
	ViewMatrix = lookAt(position, vec3(0,0,0), vec3(0,1,0));

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}