#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/geometry.h"

namespace mz {
	class GeometrySystem {
	public:
		GeometrySystem();
		const Geometry* Acquire(std::string name);
		void Destroy(std::string name);
		void Shutdown();
		void DrawGeometries();
	private:
		std::unordered_map<std::string, Geometry*> m_geometries;
		bool LoadObjGeometry(std::string name);
	};
}