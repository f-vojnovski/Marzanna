#pragma once

#include "renderer_backend.h"
#include "engine/src/mzpch.h"

namespace mz {
	enum class RenderApiType {
		None, Vulkan
	};
	
	struct RenderApiArgs {
		std::string name = "";
		const Window* window;
	};

	class RenderAPI {
	public:
		RenderAPI(const RenderApiArgs args);
		bool Initialize();
		void Shutdown();
		bool DrawFrame();
		void OnResize();
		inline RenderApiType GetType() { return RenderApiType::Vulkan; }
	private:
		std::unique_ptr<RendererBackend> m_rendererBackend;
	};
}