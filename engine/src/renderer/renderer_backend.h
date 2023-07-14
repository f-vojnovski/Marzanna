#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/window.h"
#include "render_types.h"

namespace mz {
	struct RendererBackendArgs {
		std::string name;
		const Window* window;
	};

	struct RendererGlobalState {
		glm::mat4 view;
		glm::mat4 projection;
	};

	struct RendererGeometryData {
	};

	class RendererBackend {
	protected:
		std::string m_name;
	public:
		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual bool BeginFrame() = 0;
		virtual bool EndFrame() = 0;
		virtual void OnResize() = 0;
		virtual void UpdateGlobalState(RendererGlobalState globalState) = 0;
		inline virtual ~RendererBackend() {}
	};
}