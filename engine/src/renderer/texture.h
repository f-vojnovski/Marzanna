#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	class Texture {
	public:
		virtual ~Texture();
		static Texture* CreateTexture(stbi_uc* pixels, int32_t width, int32_t height, int32_t channels);
		static bool LoadTexture(std::string filepath, Texture** outTexture);
	};
}