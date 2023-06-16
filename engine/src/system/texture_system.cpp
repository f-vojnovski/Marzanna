#include "texture_system.h"
#include "engine/src/core/log.h"
#include "engine/src/renderer/vulkan/vulkan_texture.h"

namespace mz {

	Texture::~Texture()
	{
	}

	Texture* Texture::CreateTexture(stbi_uc* pixels, int32_t width, int32_t height, int32_t channels)
	{
		switch (Application::Get().GetRenderApiType()) {
		case RenderApiType::Vulkan:
			return new VulkanTexture(pixels, width, height, channels);
		default:
			throw std::runtime_error("No render API type specified for texture creation!");
		}
	}

	std::shared_ptr<Texture> TextureSystem::Acquire(std::string name)
	{
		LoadTexture(name);
		return nullptr;
	}

	bool TextureSystem::Destroy(std::string name)
	{
		auto it = m_textures.find(name);

		if (it != m_textures.end())
		{
			Texture* texture = it->second;

			delete texture;

			m_textures.erase(it);
			return true;
		}

		MZ_CORE_WARN("Tried to delete texture {0}, which does not exist in the texture system.");
		return false;
	}

	void TextureSystem::Shutdown()
	{
		for (auto& pair : m_textures)
		{
			delete pair.second;
		}

		m_textures.clear();
	}

	bool TextureSystem::LoadTexture(std::string name)
	{
		int32_t width, height, channels;
		std::string filePath = "assets/textures/" + name;
		stbi_uc* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!pixels) {
			MZ_CORE_WARN("Failed to load texture {0}!", name);
			return false;
		}

		MZ_CORE_TRACE("Loaded texture {0}.", name);


		Texture* newTexture = Texture::CreateTexture(pixels, width, height, channels);

		m_textures.emplace(name, newTexture);

		stbi_image_free(pixels);
		
		return true;
	}
}
