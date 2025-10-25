#include "CelestialBody.h"

int CelestialBody::bodyCount;

CelestialBody::CelestialBody(vec3 p, vec3 c)
{
  setPosition(p);
  setColor(c);

  bodyNum = bodyCount;

  bodyCount++;

  // is this ok?
  
	glGenBuffers(1, &trailingTailBufferData);
	glBindBuffer(GL_ARRAY_BUFFER, trailingTailBufferData);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trailPoints), &trailPoints[0] , GL_DYNAMIC_DRAW);
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