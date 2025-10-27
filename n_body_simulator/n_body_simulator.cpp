#include <include/definitions.h> // Need to figure out how to define in a .cpp file instead of in the .h

const int FPS = 60;

int main(void)
{
  // // Initialize Window
  if(initWindow()) return -1;

  // Initialize Shaders
  if(initShaders()) return -1;

  // // Initialize bodies
  vector<CelestialBody*> bodies;

  // double G = 4 * pi<double>() * pi<double>();
  // CelestialBody::G = G;
  
  // double M0 = 1;
  // double M1 = 1;
  
  // double m = (M0 * M1) / (M0 + M1);
  
  // double loc = 1;
    
  // double v0 = sqrt(G * m / (2*loc));
  
  // // Body 0
  // vec3 pos0 = vec3(loc, 0, 0);
  // vec3 vel0 = vec3(0, v0, 0);
  
  // // Body 1
  // vec3 pos1 = vec3(-loc, 0, 0);
  // vec3 vel1 = vec3(0, -v0, 0);
  
  // vector<vec3> positions = {pos0, pos1};
  // vector<vec3> velocities = {vel0, vel1};
  // vector<float> masses = {(float)M0, (float)M1};
  // vector<vec3> colors = {vec3(1,0,0), vec3(0,1,0)};




  // float G = 1;
  // CelestialBody::G = G;
  // vec3 v = vec3(0.3471128135672417, 0, 0.532726851767674);
  
  // vec3 pos1 = (float)3 * vec3(1,0,0);
  // vec3 pos2 = (float)3 * vec3(-1,0,0);
  // vec3 pos3 = vec3(0,0,0);

  // vec3 vel1 = v;
  // vec3 vel2 = v;
  // vec3 vel3 = (float)-2 * v;

  // vector<vec3> positions = {pos1,pos2,pos3};
  // vector<vec3> velocities = {vel1,vel2,vel3};
  // vector<vec3> colors = {vec3(1,0,0), vec3(0,1,0), vec3(0,0,1)};
  // vector<float> masses = {3, 3, 3};

  

  CelestialBody::MAX_TRAIL_POINTS = 1000;

  float G = 1;
  CelestialBody::G = G;
  
  float r = 40;
  
  vec3 v = vec3(0.3471128135672417, 0.532726851767674, 0);

  vec3 pos0 = vec3(0,0,0);
  vec3 vel0 = vec3(0,0,0)- vec3(0,0,1);
  
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
  
  float v2 = sqrt(G * (float)(100)  / r);
    
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

  vector<float> masses = {100,/*1,1,1,*/
                          1,1,1,
                          1,1,1,
                          1,1,1,
                          1,1,1};

  vector<vec3> positions = {pos0,/*pos1,pos2,pos3,*/
                            pos4,pos5,pos6,
                            pos7,pos8,pos9, 
                            pos10, pos11, pos12, 
                            pos13, pos14, pos15};
  vector<vec3> velocities = {vel0,/*vel1,vel2,vel3,*/
                            vel4,vel5,vel6,
                            vel7,vel8,vel9, 
                            vel10, vel11, vel12, 
                            vel13, vel14, vel15};
  vector<vec3> colors = {vec3(1,1,1),/*vec3(1,1,1), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), */
                        vec3(1,1,1), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), 
                        vec3(1,1,1), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), 
                        vec3(1,1,1), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), 
                        vec3(1,1,1), vec3(0.34509803921568627, 0.7686274509803922, 0.8666666666666667), vec3(0.9882352941176471, 0.3843137254901961, 0.3333333333333333), };

  // // Switch y and z coordinates
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
    bodies.push_back(new CelestialBody(positions[i], velocities[i], colors[i], masses[i], CBShaderHandleArray, CBBufferArray, trailBufferArray, vertNum));
    bodies[i]->setRadius(0.125);
  }

  CelestialBody::cameraFollow(0);

  // Position of light
   // maybe add to the shader so the "sun" glows

  do
  {
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
      for(int i = 0 ; i < 5000 ; i++) CelestialBody::RK14_step(0.0001);
    }
    else 
    {
      for(int i = 0 ; i < 1000 ; i++) CelestialBody::RK14_step(0.0001);
    }

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Display bodies
    vec3 lightPos = CelestialBody::bodies[0]->getPosition() + vec3(0,10,0);
    CelestialBody::display(lightPos);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    

    // cout << CelestialBody::bodies[0]->getPosition().x << ", " << CelestialBody::bodies[0]->getPosition().y << ", " << CelestialBody::bodies[0]->getPosition().z << endl;

  } while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0); // Check if the ESC key was pressed or the window was closed

  cleanUp();

  return 0;
}

