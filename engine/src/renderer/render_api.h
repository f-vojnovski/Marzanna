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
	public:
		RenderAPI(const RenderApiArgs args);
		bool Initialize();
		void Shutdown();
		bool DrawFrame();
	};
}