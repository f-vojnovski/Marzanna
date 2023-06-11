#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	struct Vertex2d {
		glm::vec2 pos;
		glm::vec3 color;
	};

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}