#include "geometry.h"
#include "engine/src/renderer/vulkan/vulkan_geometry.h"

namespace mz {
	Geometry::~Geometry()
	{
	}
	Geometry* Geometry::Create(std::vector<Vertex3d> vertices, std::vector<uint32_t> indices, std::string textureName)
	{
		switch (Application::Get().GetRenderApiType()) {
			case RenderApiType::Vulkan:
				return new VulkanGeometry(vertices, indices, textureName);
			default:
				throw std::runtime_error("No render API type specified for geometry creation!");
		}

		return nullptr;
	}
}