#pragma once

#include "renderer_backend.h"
#include "engine/src/mzpch.h"

namespace mz {
	struct RenderApiArgs {
		std::string name = "";
		const Window* window;
	};

	class RenderAPI {
	private:
		std::unique_ptr<RendererBackend> m_rendererBackend;
		bool BeginFrame();
		bool EndFrame();
	public:
		RenderAPI(const RenderApiArgs args);
		bool Initialize();
		void Shutdown();
		void OnResized(const uint16_t width, const uint16_t height);
		bool DrawFrame();
	};
}