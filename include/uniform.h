#pragma once

#include <glm/glm.hpp>

struct UniformData {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 tint;
	glm::vec4 padding[3];
};
