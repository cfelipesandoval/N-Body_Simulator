#include "CelestialBody.h"
#include <iostream>

vector<CelestialBody*> CelestialBody::bodies;
// int CelestialBody::bodyCount;
int CelestialBody::MAX_TRAIL_POINTS = 1000;

CelestialBody::CelestialBody(vec3 p, vec3 v, vec3 c, float m, vector<GLuint*> CBShaderHandleArray, vector<GLuint*> CBBufferArray, vector<GLuint*> trailBufferArray, int size)
: position(p), velocity(v), color(c), mass(m), numVertices(size)
{
  // Set body number
  bodyNum = CelestialBody::bodies.size();

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


// for(int i = 0 ; i < 4 ; i++)
//   {
//     for(int j = 0 ; j < CelestialBody::bodies.size() ; j++)
//     {
//       for(int k = 0 ; k < 3 ; k++)
//       {
//         KR[i][j][k] = 0;
//       }
//     }
//   }

//   for(int i = 0 ; i < 4 ; i++)
//   {
//     for(int j = 0 ; j < CelestialBody::bodies.size() ; j++)
//     {
//       for(int k = 0 ; k < 3 ; k++)
//       {
//         cout << ", " << KR[i][j][k];
//       }
//       cout << endl;
//     }
//     cout << endl;
//   }


void CelestialBody::getK(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<vec3> &KRcurr, vector<vec3> &KVcurr)
{
  vector<vector<float>> accs(CelestialBody::bodies.size(), vector<float>(3,0));

  for(int i = 0 ; i < CelestialBody::bodies.size() ; i++)
  {

  }
}

void CelestialBody::RK4_step(float dt)
{
  vector<vector<vec3>> KR(4, vector<vec3>(CelestialBody::bodies.size(), vec3(0, 0, 0)));
  vector<vector<vec3>> KV(4, vector<vec3>(CelestialBody::bodies.size(), vec3(0, 0, 0)));
  
  vector<vec3> KRcurr(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> KVcurr(CelestialBody::bodies.size(), vec3(0,0,0));
  
  vector<float> div = {1, 2, 2, 1};

  for(int i = 0 ; i < 4 ; i++)
  {
    vector<vec3> poss, vels;
    vector<float> mass;

    for(int j = 0 ; j < CelestialBody::bodies.size() ; j++)
    {
      poss.push_back(CelestialBody::bodies[j]->getPosition() + KRcurr[j] * dt / div[i]);
      vels.push_back(CelestialBody::bodies[j]->getVelocity() + KVcurr[j] * dt / div[i]);
      mass.push_back(CelestialBody::bodies[j]->getMass());
    }

    CelestialBody::getK(poss, vels, mass, KRcurr, KVcurr);
    KR[i] = KRcurr;
    KV[i] = KRcurr;
  }

  for(int i = 0 ; i < CelestialBody::bodies.size() ; i++)
  {
    CelestialBody::bodies[i]->addPosition((1 / 6) * div * KV[i]); // need to figure out how to do withouth this matrix multiplication
  }
  
  
}

void CelestialBody::display(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 lightPos)
{
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

  // should make a matrix for each element maybe
  ModelMatrix = translate(ModelMatrix, getPosition());
  ModelMatrix = scale(ModelMatrix, vec3(radius, radius, radius));  // or different scale values
  // ModelMatrix = rotate(ModelMatrix, (angleSeparation * i), vec3(0,1,0));
  mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix;
  
  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform
  glUniformMatrix4fv(MatrixIDCelestialBody, 1, GL_FALSE, &MVP1[0][0]);
  glUniformMatrix4fv(ModelMatrixIDCelestialBody, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniform3f(colorIDCelestialBody, getColor().x, getColor().y, getColor().z);

  // Draw the triangles !
  glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_SHORT, (void*)0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
  
  if(true)
  {
    // // I think I need to add this to each body?
    glUseProgram(trailingTailID);

    // glBindBuffer(GL_ARRAY_BUFFER, trailingTailBufferData);

    mat4 thing = ProjectionMatrix * ViewMatrix;
    
    glUniformMatrix4fv(MatrixIDTrailingTail, 1, GL_FALSE, &thing[0][0]);

    trailPoints.push_back(getPosition());

    if (trailPoints.size() > CelestialBody::MAX_TRAIL_POINTS) 
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
    glUniform4f(trailingTrailColorID, getColor().x, getColor().y, getColor().z, 1.0f); // Red trail

    glDrawArrays(GL_LINE_STRIP, 0, trailPoints.size());
    glDisableVertexAttribArray(0);
  }
}