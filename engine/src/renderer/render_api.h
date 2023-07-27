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
		// position
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

		Geometry* testGeometry1;
		const std::vector<Vertex3d> vertices1 = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};

		const std::vector<uint32_t> indices1 = {
			0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
			12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21
		};

		Geometry* testGeometry2;
		const std::vector<Vertex3d> vertices2 = {
			// left face (white)
			{{-.2f, -.2f, -.2f}, {.9f, .9f, .9f}},
			{{-.2f, .2f, .2f}, {.9f, .9f, .9f}},
			{{-.2f, -.2f, .2f}, {.9f, .9f, .9f}},
			{{-.2f, .2f, -.2f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.2f, -.2f, -.2f}, {.8f, .8f, .1f}},
			{{.2f, .2f, .2f}, {.8f, .8f, .1f}},
			{{.2f, -.2f, .2f}, {.8f, .8f, .1f}},
			{{.2f, .2f, -.2f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.2f, -.2f, -.2f}, {.9f, .6f, .1f}},
			{{.2f, -.2f, .2f}, {.9f, .6f, .1f}},
			{{-.2f, -.2f, .2f}, {.9f, .6f, .1f}},
			{{.2f, -.2f, -.2f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.2f, .2f, -.2f}, {.8f, .1f, .1f}},
			{{.2f, .2f, .2f}, {.8f, .1f, .1f}},
			{{-.2f, .2f, .2f}, {.8f, .1f, .1f}},
			{{.2f, .2f, -.2f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.2f, -.2f, 0.2f}, {.1f, .1f, .8f}},
			{{.2f, .2f, 0.2f}, {.1f, .1f, .8f}},
			{{-.2f, .2f, 0.2f}, {.1f, .1f, .8f}},
			{{.2f, -.2f, 0.2f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.2f, -.2f, -0.2f}, {.1f, .8f, .1f}},
			{{.2f, .2f, -0.2f}, {.1f, .8f, .1f}},
			{{-.2f, .2f, -0.2f}, {.1f, .8f, .1f}},
			{{.2f, -.2f, -0.2f}, {.1f, .8f, .1f}},
		};

		const std::vector<uint32_t> indices2 = {
			0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
			12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21
		};

		PerspectiveCamera* testCamera;
	};
}