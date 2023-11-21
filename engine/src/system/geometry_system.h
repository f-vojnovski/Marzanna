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
	private:
		std::unordered_map<std::string, Geometry*> m_geometries;
		bool LoadObjGeometry(std::string name);
		bool LoadGeometryAssimp(std::string name);

		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Vertex3d>& vertices, std::vector<uint32_t>& indices);
		void ProcessMesh(aiMesh* mesh, const aiScene* scene, std::vector<Vertex3d>& vertices, std::vector<uint32_t>& indices);
	};
}