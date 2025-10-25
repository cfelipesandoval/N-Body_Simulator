#include "CelestialBody.h"
#include <iostream>

int CelestialBody::bodyCount;
int CelestialBody::MAX_TRAIL_POINTS;

CelestialBody::CelestialBody(vec3 p, vec3 c, vector<GLuint*> CBShaderHandleArray, vector<GLuint*> CBBufferArray, vector<GLuint*> trailBufferArray, int size)
{
  // cout << ", " << &CBShaderHandleArray << endl;
  // cout << ", " << &CBShaderHandleArray[0] << endl;
  // cout << ", " << CBShaderHandleArray[0] << endl;
  // cout << ", " << *CBShaderHandleArray[0] << endl;
  // cout << ", " << &CBShaderHandleArray[1] << endl;
  // cout << ", " << CBShaderHandleArray[1] << endl;
  // cout << ", " << *(CBShaderHandleArray[1]) << endl;


  setPosition(p);
  setColor(c);

  bodyNum = bodyCount;
  bodyCount++;

  vertNum = size;

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
  // glBindBuffer(GL_ARRAY_BUFFER, trailingTailBufferData);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(trailPoints), &trailPoints[0] , GL_DYNAMIC_DRAW);
}

vec3 CelestialBody::getPosition()
{
  return position;
}

vec3 CelestialBody::getColor()
{
  return color;
}

void CelestialBody::addPosition(vec3 pos)
{
  position += pos;
}

void CelestialBody::setPosition(vec3 pos)
{
  position = pos;
}

void CelestialBody::setColor(vec3 col)
{
  color = col;
}

void CelestialBody::draw(mat4 ProjectionMatrix, mat4 ViewMatrix, vec3 lightPos)
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
  ModelMatrix = scale(ModelMatrix, vec3(0.5f, 0.5f, 0.5f));  // or different scale values
  // ModelMatrix = rotate(ModelMatrix, (angleSeparation * i), vec3(0,1,0));
  mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix;
  
  // Send our transformation to the currently bound shader, 
  // in the "MVP" uniform
  glUniformMatrix4fv(MatrixIDCelestialBody, 1, GL_FALSE, &MVP1[0][0]);
  glUniformMatrix4fv(ModelMatrixIDCelestialBody, 1, GL_FALSE, &ModelMatrix[0][0]);
  glUniform3f(colorIDCelestialBody, getColor().x, getColor().y, getColor().z);

  // Draw the triangles !
  glDrawElements(GL_TRIANGLES, vertNum, GL_UNSIGNED_SHORT, (void*)0);
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