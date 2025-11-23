# N-Body_Simulator
<p align="center">
  <a>
    <img width = 800, src ="https://github.com/cfelipesandoval/N-Body_Simulator/blob/b9806bebc83f8f4791950000dae5f802254cf59b/Example_Videos/OrbitingFig8.gif"
  </a>
</p>

This tool sets out to create a framework to easily create simulations for planetary motion.

## Building the tool
To build, first go into (or create) the build folder using the terminal

```console
cd build
```
Then you can run

```console
cmake ../ && cmake --build . && ./launch-n_body_simulator.cmd
```

On subsequent builds, you only need to run
```console
cmake --build . && ./launch-n_body_simulator.cmd
```

## Using the Tool
### Creating Instances
Each body is created as an instance of the class CelestialBody, which can be initialized as
```cpp
new CelestialBody(glm::vec3 pos, glm::vec3 vel, glm::vec3 color, float m);
```
Which automatically adds the body to the list of all bodies CelestialBody::bodies

You can look at the class definition and its parameters in CelestialBody.h

I recommend initializing by first setting all the parameters and initializing by collecting each parameter into its own array
```cpp
vector<float> M = {mass1, mass2 ... , massN}; // Masses
vector<vec3> pos = {pos1, pos2, ... , posN}; // Position Vectors
vector<vec3> vel = {vel1, vel2, ... , velN}; // Velocity Vectors
vector<vec3> col = {col1, col2, ... , colN}; // Velocity Vectors

for(int i = 0 ; i < masses.size() ; i++)
{
  new CelestialBody(pos[i], vel[i], col[i], M[i]);
}
```

### Simulating

You can set some of the simulation parameters as follows:

```cpp
CelestialBody::setTimeStep(dt); // Set time step
CelestialBody::setOrder(order); // Set Runge-Kutta order as 4, 10 or 14
CelestialBody::setSkipFrames(frames); // Set how many updates to do before drawing to screen
```

By default, the Camera is set to follow the center of mass of all bodies, but can be changed by:

```cpp
CelestialBody::cameraFollow(bodyNumber); // Follow a specific body
CelestialBody::followBiggest(); // Follow body with largest mass
CelestialBody::followCOM(); // Follow center of mass
CelestialBody::cameraStopFollowing(); // Stop following and set camera to be around (0,0,0)
```

Once your simulation is setup, you can simulate by making a while loop like this:
```cpp
do
{
  // Update Bodies
  CelestialBody::update();
  // Display Bodies
  CelestialBody::display();

} while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

```

#### User Input
You can include inside your loop the
```cpp
handleUserInput();
```
Function which has the following inputs:
W,A,S,D: Pan around center
UP/DOWN_ARROW: Zoom in/out
RIGHT/LEFT_ARROW: Increase/Decrease frames to skip
B: Follow body with largest mass
C: Follow Center of Mass
N: Stop following body and center at (0,0,0)
U: Follow next body
Y: Follow previous body
T: Toggle trail display
P: Pause

You can also include the
```cpp
handleSpawnRandom(num, maxRadius, maxVelocity, meanMass, stdMass);
```
Function, which spawns random bodies around the center when you press the Enter key

### Examples
In the main() function in n_body_simulator.cpp you can see the following examples which you can comment in and out for predefined systems:

```cpp
// // Predefined Body Initializations
solarSystem();
randomBodies(200);
twoBody();
fig8();
orbitingFig8();
```
And you can see the way I set them up in their function definitions in definitions.cpp