#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	class Input {
	private:
		static Input* s_Instance;

	protected:
		inline virtual bool IsKeyPressedImpl(int keycode) = 0;
		inline virtual bool IsMouseButtonPressedImpl(int button) = 0;
		inline virtual std::pair<float, float> GetMousePositionImpl() = 0;

	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePosition(); }
	};
}