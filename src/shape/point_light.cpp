#include "point_light.h"

PointLight::PointLight(UBO& ubo, LightUBO& lightUbo):
	ubo(&ubo),
	lightUbo(&lightUbo)
{

}

void PointLight::init(VulkanState& state, const glm::vec3& color, const glm::vec3& position, float radius, float specPower /* = 1.0f */)
{
	mState = &state;
	ubo->model = glm::scale(glm::vec3(radius, radius, radius));
	ubo->model = glm::translate(position) * ubo->model;
	lightUbo->color = color;
	lightUbo->position = position;
	lightUbo->specPower = specPower;
	setRadius(radius);
}

void PointLight::setRadius(float radius) 
{
	float r2 = radius * radius;
    float maxColor = fmax(fmax(lightUbo->color.x, lightUbo->color.y), lightUbo->color.z);

    lightUbo->quadratic = (256.0f * maxColor - 1.0f) / r2;
	lightUbo->linear = 0.0f;
	lightUbo->constant = 1.0f;
}



