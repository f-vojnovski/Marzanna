#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/events/event.h"

namespace mz {
	struct WindowProps {
		std::string title;
		uint16_t width;
		uint16_t height;
		WindowProps(
			const std::string& title = "Marzanna", 
			uint16_t width = 1280, 
			uint16_t height = 720) 
			: title(title), width(width), height(height) {}
	};

	class Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());

		virtual void* GetNativeWindow() const = 0;
	};
}