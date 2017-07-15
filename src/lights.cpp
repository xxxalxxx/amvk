#include "lights.h"



PointLight::PointLight(const glm::vec3& color, const glm::vec3& position, float radius)
{
	uniform.color = color;
	uniform.position = position;
	uniform.specPower = 1.0f;
	setRadius(radius);
}

void PointLight::setRadius(float radius) 
{
	this->radius = radius;
	float r2 = radius * radius;
    float maxColor = fmax(fmax(uniform.color.x, uniform.color.y), uniform.color.z);

    uniform.quadratic = (256.0f * maxColor - 1.0f) / r2;
	uniform.linear = 0.0f;
	uniform.constant = 1.0f;
}

float PointLight::getRadius() const
{
	return radius;
}
