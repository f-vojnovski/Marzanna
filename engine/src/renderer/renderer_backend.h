#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/window.h"

namespace mz {
	struct RendererBackendArgs {
		std::string name;
		const Window* window;
	};

	class RendererBackend {
	protected:
		std::string m_name;
		const Window* m_window;
	public:
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual bool BeginFrame() = 0;
		virtual bool EndFrame() = 0;
		inline virtual ~RendererBackend() {}
	};
}