#include "texture.h"

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

	bool Texture::LoadTexture(std::string name, Texture** outTexture)
	{
		int32_t width, height, channels;
		std::string filePath = "assets/textures/" + name;
		stbi_uc* pixels = stbi_load(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		if (!pixels) {
			MZ_CORE_WARN("Failed to load texture {0}!", name);
			return false;
		}

		MZ_CORE_TRACE("Loaded texture {0}.", name);

		Texture* vulkanTexture = Texture::CreateTexture(pixels, width, height, channels);
		*outTexture = vulkanTexture;

		stbi_image_free(pixels);

		return true;
	}
}