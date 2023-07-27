#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	class Texture {
	public:
		virtual ~Texture();
		static Texture* CreateTexture(stbi_uc* pixels, int32_t width, int32_t height, int32_t channels);
	};

	class TextureSystem {
	public:
		TextureSystem() = default;
		std::shared_ptr<Texture> Acquire(std::string name);
		bool Destroy(std::string name);
		void Shutdown();
	private:
		std::unordered_map<std::string, Texture*> m_textures;
		bool LoadTexture(std::string name);
	};
}