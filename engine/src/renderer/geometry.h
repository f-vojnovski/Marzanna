#pragma once

#include "engine/src/mzpch.h"
#include "render_types.h"

namespace mz {
	class Geometry {
	public:
		virtual ~Geometry();
		virtual void Draw(glm::mat4 model) const = 0;
		static Geometry* Create(std::vector<Vertex3d> vertices, std::vector<uint32_t> indices, std::string textureName);
	};
}