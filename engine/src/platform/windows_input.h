#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/input.h"

namespace mz {
	class WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;
	};
}