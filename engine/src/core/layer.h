#pragma once

#include "engine/src/mzpch.h"
#include "events/event.h"

namespace mz {
	class Layer {
	protected:
		std::string m_debugName;

	public:
		Layer(const std::string& name = "Layer");

		virtual ~Layer();

		virtual void OnAttach() {};
		virtual void OnDetach() {};
		virtual void OnUpdate() {};
		virtual void OnEvent(Event& e) {};

		inline const std::string& GetName() const { return m_debugName; }
	};
}