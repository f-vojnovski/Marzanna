#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/system/texture_system.h"
#include "engine/src/renderer/vulkan/vulkan_context.h"

namespace mz {
	class VulkanTexture : public Texture {
	public:
		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
		VulkanTexture(stbi_uc* pixels, int32_t width, int32_t height, int32_t channels);
		~VulkanTexture();
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;

		VkImage image;
		VkDeviceMemory imageMemory;
	};
}