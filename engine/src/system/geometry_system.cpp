#include "geometry_system.h"
#include "engine/src/core/log.h"
#include "engine/src/renderer/render_types.h"

namespace mz {
	GeometrySystem::GeometrySystem()
	{
	}
	const Geometry* GeometrySystem::Acquire(std::string name)
	{
		auto it = m_geometries.find(name);

		if (it == m_geometries.end()) {
			LoadObjGeometry(name);
			it = m_geometries.find(name);
		}

		if (it != m_geometries.end()) {
			return it->second;
		}

		return nullptr;
	}

	void GeometrySystem::Destroy(std::string name)
	{
		auto it = m_geometries.find(name);

		if (it != m_geometries.end())
		{
			Geometry* geometry = it->second;

			delete geometry;

			m_geometries.erase(it);
		}

		MZ_CORE_WARN("Tried to delete geometry {0}, which does not exist in the geometry system.", name);
	}

	void GeometrySystem::Shutdown()
	{
		for (auto& pair : m_geometries)
		{
			delete pair.second;
		}

		m_geometries.clear();
	}

	bool GeometrySystem::LoadObjGeometry(std::string name)
	{
		std::string filePath = "assets/models/" + name;

		std::vector<Vertex3d> vertices;
		std::vector<uint32_t> indices;

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			MZ_CORE_ERROR("TinyObjLoader failed to load geometry {0}, warning: {1}, error: {2}", name, warn, err);
			return false;
		}

		// Vertex de-duplication
		std::unordered_map<Vertex3d, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex3d vertex{};

				if (index.vertex_index >= 0) {
					vertex.pos = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				// Uncomment when normals implemeneted
				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.texCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}

		Geometry* newGeometry = Geometry::Create(vertices, indices, "vapor.png");
		m_geometries.emplace(name, newGeometry);
	}
}