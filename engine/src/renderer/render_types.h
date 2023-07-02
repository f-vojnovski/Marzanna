#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/utils.h"

namespace mz {
	struct Vertex2d {
		glm::vec2 pos;
		glm::vec3 color;
	};

	struct Vertex3d {
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		bool operator==(const Vertex3d& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}

namespace std {
	template<> struct hash<mz::Vertex3d> {
		size_t operator()(mz::Vertex3d const& vertex) const {
			size_t seed = 0;
			mz::HashCombine(seed, vertex.pos, vertex.color, vertex.texCoord);
			return seed;
		}
	};
}