#ifndef AMVK_LIGHTS_H
#define AMVK_LIGHTS_H

#include <math.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>



struct PointLight {
	PointLight(const glm::vec3& color, const glm::vec3& position, float radius);
	void setRadius(float radius);
	float getRadius() const;
	struct Uniform {
		glm::vec3 color;
		glm::vec3 position;
		float specPower;
		float quadratic, linear, constant; 
	};
	Uniform uniform;
private:
	float radius;
};

#endif
