#pragma once

#include "renderer_backend.h"
#include "engine/src/mzpch.h"
#include "geometry.h"
#include "engine/src/system/geometry_system.h"
#include "perspective_camera.h"

namespace mz {
	enum class RenderApiType {
		None, Vulkan
	};
	
	struct RenderApiArgs {
		std::string name = "";
		const Window* window;
	};

	struct GeometryWithPosition {
		const Geometry* geometry;
		glm::mat4 model;
	};

	struct RenderApiDrawCallArgs {
		std::vector<GeometryWithPosition> geometries;
	};

	class RenderAPI {
	public:
		RenderAPI(const RenderApiArgs args);
		bool Initialize();
		void Shutdown();
		bool DrawFrame(RenderApiDrawCallArgs args);
		void OnResize();
		inline RenderApiType GetType() { return RenderApiType::Vulkan; }
	private:
		std::unique_ptr<RendererBackend> m_rendererBackend;

		PerspectiveCamera* testCamera;
	};
}