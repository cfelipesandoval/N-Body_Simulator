// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

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




int main(void)
{
	// Initialize GLFW
	if(!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
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
		return -1;
	}
	glfwMakeContextCurrent(window);
    
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited movement
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it is closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// // Initializations for CelestialBody models
	// Create and compile our GLSL program from the shaders
	GLuint CelestialBodyID = LoadShaders("resources/shaders/CelestialBodyShading.vertexshader", "resources/shaders/CelestialBodyShading.fragmentshader");

	// Get a handle for our MVP, View, and Model uniforms
	GLuint MatrixIDCelestialBody = glGetUniformLocation(CelestialBodyID, "MVP");
	GLuint ViewMatrixIDCelestialBody = glGetUniformLocation(CelestialBodyID, "V");
	GLuint ModelMatrixIDCelestialBody = glGetUniformLocation(CelestialBodyID, "M");
	GLuint colorIDCelestialBody = glGetUniformLocation(CelestialBodyID, "COLOR");

	// Read our .obj file
	std::vector<vec3> vertices;
	std::vector<vec2> uvs;
	std::vector<vec3> normals;
	
	bool res = loadOBJ("resources/objectFiles/Sphere.obj", vertices, uvs, normals);

	// Indices for VBO
	std::vector<unsigned short> indices;
	std::vector<vec3> indexed_vertices;
	std::vector<vec2> indexed_uvs;
	std::vector<vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Buffers
	GLuint vertexBufferCelestialBody;
	glGenBuffers(1, &vertexBufferCelestialBody);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCelestialBody);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvBufferCelestialBody;
	glGenBuffers(1, &uvBufferCelestialBody);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferCelestialBody);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalBufferCelestialBody;
	glGenBuffers(1, &normalBufferCelestialBody);
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferCelestialBody);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(vec3), &indexed_normals[0], GL_STATIC_DRAW);

	GLuint elementBufferCelestialBody;
	glGenBuffers(1, &elementBufferCelestialBody);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferCelestialBody);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(CelestialBodyID);
	GLuint LightIDCelestialBody = glGetUniformLocation(CelestialBodyID, "LightPosition_worldspace");
	
	// Get handle for toggling diffusion and specular components
	GLuint diffSpecIDCelestialBody = glGetUniformLocation(CelestialBodyID, "diffSpec");
  glUniform1f(diffSpecIDCelestialBody, 1); // Initialize to on by default

  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferCelestialBody);

	// // Initialize buffers for trailing tail
	GLuint trailingTailID = LoadShaders("resources/shaders/TrailingTailShading.vertexshader", "resources/shaders/TrailingTailShading.fragmentshader");
	GLuint MatrixIDTrailingTail = glGetUniformLocation(trailingTailID, "MVP");
	GLuint trailingTrailColorID = glGetUniformLocation(trailingTailID, "trailColor");
	
	vector<vec3> trailPoints;

	// Bind your VAO and VBO for the trail
	GLuint trailingTailBufferData;

	glGenBuffers(1, &trailingTailBufferData);
	glBindBuffer(GL_ARRAY_BUFFER, trailingTailBufferData);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trailPoints), &trailPoints[0] , GL_DYNAMIC_DRAW);

	// Boolean to check if diffusion and specular components have been toggled
	bool diffSpecOn = true;
	// Position of light
	vec3 lightPos = vec3(0, 5, 0);

	vec3 positions[3] = {vec3(1,0,1), vec3(-1,0,-1), vec3(0,0,5)};
	vec3 colors[3] = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};

	int mult[3] = {1, 2, 4};

	vector<CelestialBody*> bodies;

	CelestialBody::bodyCount = 0;

	for(int i = 0 ; i < 3 ; i++)
	{
		bodies.push_back(new CelestialBody(positions[i], colors[i]));
	}


	double lastTime = glfwGetTime();

	int MAX_TRAIL_POINTS = 1000;

  do
  {	
		double currentTime = glfwGetTime();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		mat4 ProjectionMatrix = getProjectionMatrix();
		mat4 ViewMatrix = getViewMatrix();
		
		// Check if L key is pressed and toggle diffusion and specular components for both the square and CelestialBody models
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
    {
			if(diffSpecOn)
			{
				glUseProgram(CelestialBodyID);
				glUniform1f(diffSpecIDCelestialBody, 0);
				diffSpecOn = false; 
			}
			else
			{
				glUseProgram(CelestialBodyID);
				glUniform1f(diffSpecIDCelestialBody, 1);
				diffSpecOn = true;
			} 
    }

		

		// Iterate through bodies
		for(auto& el : bodies)
		{
			// // // Draw the Bodie
			glUseProgram(CelestialBodyID);
			// Update the uniform
			glUniform3f(LightIDCelestialBody, lightPos.x, lightPos.y, lightPos.z);
			glUniformMatrix4fv(ViewMatrixIDCelestialBody, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "CelestialBodyID"

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCelestialBody);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, uvBufferCelestialBody);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindBuffer(GL_ARRAY_BUFFER, normalBufferCelestialBody);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			// Determine position
			mat4 ModelMatrix = mat4(1.0);

			el->setPosition(vec3(el->bodyNum * 5 * cos(2 * currentTime), 0, el->bodyNum * 5 * sin(2 * currentTime)));
			
			ModelMatrix = translate(ModelMatrix, el->getPosition());
			cout << el->getPosition().x << ", " << el->getPosition().y << ", " << el->getPosition().z  <<endl;
			// ModelMatrix = scale(ModelMatrix, vec3(5.0f, 5.0f, 5.0f));  // or different scale values
			// ModelMatrix = rotate(ModelMatrix, (angleSeparation * i), vec3(0,1,0));
			mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix;
			
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixIDCelestialBody, 1, GL_FALSE, &MVP1[0][0]);
			glUniformMatrix4fv(ModelMatrixIDCelestialBody, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniform3f(colorIDCelestialBody, el->getColor().x, el->getColor().y, el->getColor().z);

			// Draw the triangles !
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
			
			
			// I think I need to add this to each body?
			glUseProgram(trailingTailID);

			mat4 thing = ProjectionMatrix * ViewMatrix;
			
			glUniformMatrix4fv(MatrixIDTrailingTail, 1, GL_FALSE, &thing[0][0]);

			trailPoints.push_back(el->getPosition());

			if (trailPoints.size() > MAX_TRAIL_POINTS) 
			{
					trailPoints.erase(trailPoints.begin());
			}

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, trailingTailBufferData);
			glBufferData(GL_ARRAY_BUFFER, trailPoints.size() * sizeof(vec3), trailPoints.data(), GL_DYNAMIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

			// Set line width if desired
			glLineWidth(2.0f);
			// Set color
			glUniform4f(trailingTrailColorID, 1.0f, 0.0f, 0.0f, 1.0f); // Red trail

			glDrawArrays(GL_LINE_STRIP, 0, trailPoints.size());
			glDisableVertexAttribArray(0);
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);




		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexBufferCelestialBody);
	glDeleteBuffers(1, &uvBufferCelestialBody);
	glDeleteBuffers(1, &normalBufferCelestialBody);
	glDeleteBuffers(1, &elementBufferCelestialBody);
	glDeleteProgram(CelestialBodyID);
	// glDeleteTextures(1, &TextureCelestialBody);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

