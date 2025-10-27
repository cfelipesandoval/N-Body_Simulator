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

// need to figure out how to get better precision for calculations
void CelestialBody::RK14_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<vec3> &KRcurr, vector<vec3> &KVcurr)
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

void CelestialBody::RK14_step(float dt)
{
  vector<vector<vec3>> KR(35, vector<vec3>(CelestialBody::bodies.size(), vec3(0,0,0)));
  vector<vector<vec3>> KV(35, vector<vec3>(CelestialBody::bodies.size(), vec3(0,0,0)));
  
  vector<vec3> KRcurr(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> KVcurr(CelestialBody::bodies.size(), vec3(0,0,0));

  vector<float> ak = {0.000000000000000000000000000000000000000000000000000000000000,
                      0.111111111111111111111111111111111111111111111111111111111111,
                      0.555555555555555555555555555555555555555555555555555555555556,
                      0.833333333333333333333333333333333333333333333333333333333333,
                      0.333333333333333333333333333333333333333333333333333333333333,
                      1.00000000000000000000000000000000000000000000000000000000000,
                      0.669986979272772921764683785505998513938845229638460353285142,
                      0.297068384213818357389584716808219413223332094698915687379168,
                      0.727272727272727272727272727272727272727272727272727272727273,
                      0.140152799042188765276187487966946717629806463082532936287323,
                      0.700701039770150737151099854830749337941407049265546408969222,
                      0.363636363636363636363636363636363636363636363636363636363636,
                      0.263157894736842105263157894736842105263157894736842105263158,
                      0.0392172246650270859125196642501208648863714315266128052078483,
                      0.812917502928376762983393159278036506189612372617238550774312,
                      0.166666666666666666666666666666666666666666666666666666666667,
                      0.900000000000000000000000000000000000000000000000000000000000,
                      0.0641299257451966923312771193896682809481096651615083225402924,
                      0.204149909283428848927744634301023405027149505241333751628870,
                      0.395350391048760565615671369827324372352227297456659450554577,
                      0.604649608951239434384328630172675627647772702543340549445423,
                      0.795850090716571151072255365698976594972850494758666248371130,
                      0.935870074254803307668722880610331719051890334838491677459708,
                      0.166666666666666666666666666666666666666666666666666666666667,
                      0.812917502928376762983393159278036506189612372617238550774312,
                      0.0392172246650270859125196642501208648863714315266128052078483,
                      0.363636363636363636363636363636363636363636363636363636363636,
                      0.700701039770150737151099854830749337941407049265546408969222,
                      0.140152799042188765276187487966946717629806463082532936287323,
                      0.297068384213818357389584716808219413223332094698915687379168,
                      0.669986979272772921764683785505998513938845229638460353285142,
                      0.333333333333333333333333333333333333333333333333333333333333,
                      0.555555555555555555555555555555555555555555555555555555555556,
                      0.111111111111111111111111111111111111111111111111111111111111,
                      1.00000000000000000000000000000000000000000000000000000000000};

  vector<float> ck = {0.0178571428571428571428571428571428571428571428571428571428571,
                      0.00585937500000000000000000000000000000000000000000000000000000,
                      0.0117187500000000000000000000000000000000000000000000000000000,
                      0,
                      0.0175781250000000000000000000000000000000000000000000000000000,
                      0,
                      0.0234375000000000000000000000000000000000000000000000000000000,
                      0.0292968750000000000000000000000000000000000000000000000000000,
                      0,
                      0.0351562500000000000000000000000000000000000000000000000000000,
                      0.0410156250000000000000000000000000000000000000000000000000000,
                      0.0468750000000000000000000000000000000000000000000000000000000,
                      0,
                      0.0527343750000000000000000000000000000000000000000000000000000,
                      0.0585937500000000000000000000000000000000000000000000000000000,
                      0.0644531250000000000000000000000000000000000000000000000000000,
                      0,
                      0.105352113571753019691496032887878162227673083080523884041670,
                      0.170561346241752182382120338553874085887555487802790804737501,
                      0.206229397329351940783526485701104894741914286259542454077972,
                      0.206229397329351940783526485701104894741914286259542454077972,
                      0.170561346241752182382120338553874085887555487802790804737501,
                      0.105352113571753019691496032887878162227673083080523884041670,
                    -0.0644531250000000000000000000000000000000000000000000000000000,
                    -0.0585937500000000000000000000000000000000000000000000000000000,
                    -0.0527343750000000000000000000000000000000000000000000000000000,
                    -0.0468750000000000000000000000000000000000000000000000000000000,
                    -0.0410156250000000000000000000000000000000000000000000000000000,
                    -0.0351562500000000000000000000000000000000000000000000000000000,
                    -0.0292968750000000000000000000000000000000000000000000000000000,
                    -0.0234375000000000000000000000000000000000000000000000000000000,
                    -0.0175781250000000000000000000000000000000000000000000000000000,
                    -0.0117187500000000000000000000000000000000000000000000000000000,
                    -0.00585937500000000000000000000000000000000000000000000000000000,
                      0.0178571428571428571428571428571428571428571428571428571428571};

  vector<vec3> poss(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> vels(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<float> mass(CelestialBody::bodies.size());

  for(int i = 0 ; i < 35 ; i++)
  {
    for(int j = 0 ; j < CelestialBody::bodies.size() ; j++)
    {
      poss[j] = (CelestialBody::bodies[j]->getPosition() + KRcurr[j] * dt * ak[i]);
      vels[j] = (CelestialBody::bodies[j]->getVelocity() + KVcurr[j] * dt * ak[i]);
      mass[j] = (CelestialBody::bodies[j]->getMass());
    }

    CelestialBody::RK14_helper(poss, vels, mass, KRcurr, KVcurr);

    KR[i] = KRcurr;
    KV[i] = KVcurr;
  }

  for(int i = 0 ; i < CelestialBody::bodies.size() ; i++)
  {
    vec3 tempR = vec3(0,0,0);
    vec3 tempV = vec3(0,0,0);
    
    for(int j = 0 ; j < 35 ; j++)
    {
      tempR += KR[j][i] * ck[j];
      tempV += KV[j][i] * ck[j];
    }

    CelestialBody::bodies[i]->addPosition(-tempR * dt);
    CelestialBody::bodies[i]->addVelocity(-tempV * dt);
  }
}

void CelestialBody::RK10_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<vec3> &KRcurr, vector<vec3> &KVcurr)
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

void CelestialBody::RK10_step(float dt)
{
  vector<vector<vec3>> KR(17, vector<vec3>(CelestialBody::bodies.size(), vec3(0,0,0)));
  vector<vector<vec3>> KV(17, vector<vec3>(CelestialBody::bodies.size(), vec3(0,0,0)));
  
  vector<vec3> KRcurr(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> KVcurr(CelestialBody::bodies.size(), vec3(0,0,0));

  vector<float> ak = {0.000000000000000000000000000000000000000000000000000000000000,
                      0.100000000000000000000000000000000000000000000000000000000000,
                      0.539357840802981787532485197881302436857273449701009015505500,
                      0.809036761204472681298727796821953655285910174551513523258250,
                      0.309036761204472681298727796821953655285910174551513523258250,
                      0.981074190219795268254879548310562080489056746118724882027805,
                      0.833333333333333333333333333333333333333333333333333333333333,
                      0.354017365856802376329264185948796742115824053807373968324184,
                      0.882527661964732346425501486979669075182867844268052119663791,
                      0.642615758240322548157075497020439535959501736363212695909875,
                      0.357384241759677451842924502979560464040498263636787304090125,
                      0.117472338035267653574498513020330924817132155731947880336209,
                      0.833333333333333333333333333333333333333333333333333333333333,
                      0.309036761204472681298727796821953655285910174551513523258250,
                      0.539357840802981787532485197881302436857273449701009015505500,
                      0.100000000000000000000000000000000000000000000000000000000000,
                      1.00000000000000000000000000000000000000000000000000000000000};

  vector<float> ck = {0.0333333333333333333333333333333333333333333333333333333333333,
                        0.0250000000000000000000000000000000000000000000000000000000000,
                        0.0333333333333333333333333333333333333333333333333333333333333,
                        0.000000000000000000000000000000000000000000000000000000000000,
                        0.0500000000000000000000000000000000000000000000000000000000000,
                        0.000000000000000000000000000000000000000000000000000000000000,
                        0.0400000000000000000000000000000000000000000000000000000000000,
                        0.000000000000000000000000000000000000000000000000000000000000,
                        0.189237478148923490158306404106012326238162346948625830327194,
                        0.277429188517743176508360262560654340428504319718040836339472,
                        0.277429188517743176508360262560654340428504319718040836339472,
                        0.189237478148923490158306404106012326238162346948625830327194,
                        -0.0400000000000000000000000000000000000000000000000000000000000,
                        -0.0500000000000000000000000000000000000000000000000000000000000,
                        -0.0333333333333333333333333333333333333333333333333333333333333,
                        -0.0250000000000000000000000000000000000000000000000000000000000,
                        0.0333333333333333333333333333333333333333333333333333333333333};

  vector<vec3> poss(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<vec3> vels(CelestialBody::bodies.size(), vec3(0,0,0));
  vector<float> mass(CelestialBody::bodies.size());

  for(int i = 0 ; i < 17 ; i++)
  {
    for(int j = 0 ; j < CelestialBody::bodies.size() ; j++)
    {
      poss[j] = (CelestialBody::bodies[j]->getPosition() + KRcurr[j] * dt * ak[i]);
      vels[j] = (CelestialBody::bodies[j]->getVelocity() + KVcurr[j] * dt * ak[i]);
      mass[j] = (CelestialBody::bodies[j]->getMass());
    }

    CelestialBody::RK10_helper(poss, vels, mass, KRcurr, KVcurr);

    KR[i] = KRcurr;
    KV[i] = KVcurr;
  }

  for(int i = 0 ; i < CelestialBody::bodies.size() ; i++)
  {
    vec3 tempR = vec3(0,0,0);
    vec3 tempV = vec3(0,0,0);
    
    for(int j = 0 ; j < 17 ; j++)
    {
      tempR += KR[j][i] * ck[j];
      tempV += KV[j][i] * ck[j];
    }

    CelestialBody::bodies[i]->addPosition(-tempR * dt);
    CelestialBody::bodies[i]->addVelocity(-tempV * dt);
  }
}

void CelestialBody::RK4_helper(vector<vec3> &poss, vector<vec3> &vels, vector<float> &mass, vector<vec3> &KRcurr, vector<vec3> &KVcurr)
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

    CelestialBody::RK4_helper(poss, vels, mass, KRcurr, KVcurr);

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
