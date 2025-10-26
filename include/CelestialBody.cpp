#include "CelestialBody.h"
#include <include/controls.hpp>
#include <iostream>

vector<CelestialBody*> CelestialBody::bodies;
int CelestialBody::follow = -1;


float CelestialBody::G = 1;
int CelestialBody::MAX_TRAIL_POINTS = 1000;

CelestialBody::CelestialBody(vec3 p, vec3 v, vec3 c, float m, vector<GLuint*> CBShaderHandleArray, vector<GLuint*> CBBufferArray, vector<GLuint*> trailBufferArray, int size)
: position(p), velocity(v), color(c), mass(m), numVertices(size)
{
  // Set body number
  bodyNum = CelestialBody::bodies.size();

  // Maybe add a conditional so you can make a static object like a light source?

  // Add to list containing all instances
  CelestialBody::bodies.push_back(this);

  CelestialBodyID = *CBShaderHandleArray[0];  
  MatrixIDCelestialBody = *CBShaderHandleArray[1];  
  ViewMatrixIDCelestialBody = *CBShaderHandleArray[2];  
  ModelMatrixIDCelestialBody = *CBShaderHandleArray[3];
  colorIDCelestialBody = *CBShaderHandleArray[4];  
  LightIDCelestialBody = *CBShaderHandleArray[5];
  
  vertexBufferCelestialBody = *CBBufferArray[0]; 
  uvBufferCelestialBody = *CBBufferArray[1]; 
  normalBufferCelestialBody = *CBBufferArray[2]; 
  elementBufferCelestialBody = *CBBufferArray[3];

  trailingTailID = *trailBufferArray[0]; 
  MatrixIDTrailingTail = *trailBufferArray[1]; 
  trailingTrailColorID = *trailBufferArray[2];

  glGenBuffers(1, &trailingTailBufferData);
}

void CelestialBody::addPosition(vec3 pos)
{
  position += pos;
}

void CelestialBody::addVelocity(vec3 vel)
{
  velocity += vel;
}

// // Getters
vec3 CelestialBody::getPosition()
{
  return position;
}

vec3 CelestialBody::getVelocity()
{
  return velocity;
}

vec3 CelestialBody::getColor()
{
  return color;
}

float CelestialBody::getMass()
{
  return mass;
}

float CelestialBody::getRadius()
{
  return radius;
}

int CelestialBody::getBodyNum()
{
  return bodyNum;
}

// // Setters
void CelestialBody::setPosition(vec3 pos)
{
  position = pos;
}

void CelestialBody::setVelocity(vec3 vel)
{
  velocity = vel;
}

void CelestialBody::setColor(vec3 col)
{
  color = col;
}

void CelestialBody::setMass(float m)
{
  mass = m;
}

void CelestialBody::setRadius(float r)
{
  radius = r;
}

void CelestialBody::enableTrail()
{
  displayTrail = true;
}

void CelestialBody::disableTrail()
{
  displayTrail = false;
}

void CelestialBody::enableAllTrails()
{
  for(auto& el : CelestialBody::bodies)
  {
    el->displayTrail = true;
  }
}

void CelestialBody::disableAllTrails()
{
  for(auto& el : CelestialBody::bodies)
  {
    el->displayTrail = false;
  }
}

void CelestialBody::getK(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<vec3> &KRcurr, vector<vec3> &KVcurr)
{
  vector<vec3> possCurr(poss.size(), vec3(0, 0, 0));
  vector<float> massCurr(mass.size());

  // auto startTime1 = chrono::high_resolution_clock::now();
  // auto endTime1 = chrono::high_resolution_clock::now();
  // cout << "Time 1 taken: " << (endTime1 - startTime1).count() << endl;
  
  // #pragma omp parallel for collapse(2)
  for(int i = 0 ; i < CelestialBody::bodies.size() ; i++)
  {
    // Copy and remove contribution from itself
    possCurr = poss;
    possCurr.erase(possCurr.begin() + i);

    massCurr = mass;
    massCurr.erase(massCurr.begin() + i);

    vec3 p = poss[i];
    vec3 a = vec3(0,0,0);
    // cout << "here" << endl;
    for(int j = 0 ; j < possCurr.size() ; j++)
    {
      float mag = length(p - possCurr[j]);
      a -= CelestialBody::G * massCurr[j] * ((p - possCurr[j]) / (mag * mag * mag));
    }
    
    KRcurr[i] = vels[i];
    KVcurr[i] = a;
  }
}

void CelestialBody::RK4_step(float dt)
{
  vector<vector<vec3>> KR(4, vector<vec3>(CelestialBody::bodies.size(), vec3(0,0,0)));
  vector<vector<vec3>> KV(4, vector<vec3>(CelestialBody::bodies.size(), vec3(0,0,0)));
  
  vector<vec3> KRcurr(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> KVcurr(CelestialBody::bodies.size(), vec3(0,0,0));

  vector<float> div = {1, 2, 2, 1};
  // vec<4,float> div = {1, 2, 2, 1};

  vector<vec3> poss(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> vels(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<float> mass(CelestialBody::bodies.size());

  for(int i = 0 ; i < 4 ; i++)
  {
    for(int j = 0 ; j < CelestialBody::bodies.size() ; j++)
    {
      poss[j] = (CelestialBody::bodies[j]->getPosition() + KRcurr[j] * dt / div[i]);
      vels[j] = (CelestialBody::bodies[j]->getVelocity() + KVcurr[j] * dt / div[i]);
      mass[j] = (CelestialBody::bodies[j]->getMass());
    }

    CelestialBody::getK(poss, vels, mass, KRcurr, KVcurr);

    KR[i] = KRcurr;
    KV[i] = KVcurr;
  }

  for(int i = 0 ; i < CelestialBody::bodies.size() ; i++)
  {
    vec3 tempR = vec3(0,0,0);
    vec3 tempV = vec3(0,0,0);
    
    for(int j = 0 ; j < 4 ; j++)
    {
      tempR += KR[j][i] * div[j];
      tempV += KV[j][i] * div[j];
    }

    CelestialBody::bodies[i]->addPosition(-((float)1 / (float)6) * tempR * dt);
    CelestialBody::bodies[i]->addVelocity(-((float)1 / (float)6) * tempV * dt);
  }
}

void CelestialBody::cameraFollow(int body)
{
  if(body < CelestialBody::bodies.size()) CelestialBody::follow = body;
  else CelestialBody::follow = -1;
}

void CelestialBody::cameraStopFollowing()
{
  CelestialBody::follow = -1;
}

void CelestialBody::display(vec3 lightPos)
{
  // Compute the MVP matrix from keyboard and mouse input
  computeMatricesFromInputs();
  mat4 ProjectionMatrix = getProjectionMatrix();
  mat4 ViewMatrix = getViewMatrix();

  if(CelestialBody::follow > -1)
  {
    ViewMatrix = translate(ViewMatrix, -(CelestialBody::bodies[CelestialBody::follow]->getPosition()));
  }

  for(auto& el : CelestialBody::bodies)
  {
    glUseProgram(el->CelestialBodyID);
    // Update the uniforms
    glUniform3f(el->LightIDCelestialBody, lightPos.x, lightPos.y, lightPos.z);
    glUniformMatrix4fv(el->ViewMatrixIDCelestialBody, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "el->el->CelestialBodyID"

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, el->vertexBufferCelestialBody);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, el->uvBufferCelestialBody);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, el->normalBufferCelestialBody);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Determine position
    mat4 ModelMatrix = mat4(1.0);
    ModelMatrix = translate(ModelMatrix, el->getPosition());
    ModelMatrix = scale(ModelMatrix, vec3(el->radius, el->radius, el->radius));
    // ModelMatrix = rotate(ModelMatrix, (angleSeparation * i), vec3(0,1,0));
    mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix;
    
    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(el->MatrixIDCelestialBody, 1, GL_FALSE, &MVP1[0][0]);
    glUniformMatrix4fv(el->ModelMatrixIDCelestialBody, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniform3f(el->colorIDCelestialBody, el->getColor().x, el->getColor().y, el->getColor().z);

    // Draw the triangles !
    glDrawElements(GL_TRIANGLES, el->numVertices, GL_UNSIGNED_SHORT, (void*)0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    if(el->displayTrail)
    {
      // // I think I need to add this to each body?
      glUseProgram(el->trailingTailID);

      // glBindBuffer(GL_ARRAY_BUFFER, trailingTailBufferData);

      mat4 thing = ProjectionMatrix * ViewMatrix;
      
      glUniformMatrix4fv(el->MatrixIDTrailingTail, 1, GL_FALSE, &thing[0][0]);

      el->trailPoints.push_back(el->getPosition());

      if (el->trailPoints.size() > CelestialBody::MAX_TRAIL_POINTS) 
      {
        el->trailPoints.erase(el->trailPoints.begin());
      }

      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, el->trailingTailBufferData);
      glBufferData(GL_ARRAY_BUFFER, el->trailPoints.size() * sizeof(vec3), el->trailPoints.data(), GL_DYNAMIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

      // Set line width if desired
      glLineWidth(2.0f);
      // Set color
      glUniform4f(el->trailingTrailColorID, el->getColor().x, el->getColor().y, el->getColor().z, 1.0f); // Red trail

      glDrawArrays(GL_LINE_STRIP, 0, el->trailPoints.size());
      glDisableVertexAttribArray(0);
    }
  }
}