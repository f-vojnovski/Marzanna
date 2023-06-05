#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	class VulkanPipeline {
	private:
		inline static const std::string s_engineMaterialShaderFragmentFileName = "assets/shaders/engine-material-shader.frag.spv";
		inline static const std::string s_engineMaterialShaderVertexFileName = "assets/shaders/engine-material-shader.frag.spv";

		VkDevice m_device;

		VkShaderModule m_fragShaderModule;
		VkShaderModule m_vertShaderModule;
	public:
		VulkanPipeline(VkDevice device);
		bool Create();
		void Destroy(VkAllocationCallbacks* allocator);
	};
}