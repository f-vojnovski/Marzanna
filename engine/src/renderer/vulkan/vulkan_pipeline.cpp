#include "vulkan_pipeline.h"
#include "shaders/vulkan_shader_utils.h"
#include "engine/src/system/file_reader.h"

namespace mz {
	VulkanPipeline::VulkanPipeline(VkDevice device)
	{
		m_device = device;
		m_fragShaderModule = VK_NULL_HANDLE;
		m_vertShaderModule = VK_NULL_HANDLE;
	}

	bool VulkanPipeline::Create()
	{
		MZ_CORE_TRACE("Creating Vulkan graphics rendering pipeline...");

		/* Programmable part begin */
		auto vertShaderCode = EngineReadFile(s_engineMaterialShaderFragmentFileName);
		auto fragShaderCode = EngineReadFile(s_engineMaterialShaderVertexFileName);

		m_vertShaderModule = CreateShaderModule(vertShaderCode, m_device);
		m_fragShaderModule = CreateShaderModule(fragShaderCode, m_device);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = m_vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = m_fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
		/* Programmable part end*/

		/* Fixed function stages begin */

		/* Fixed function stages end*/

		MZ_CORE_INFO("Vulkan graphics rendering pipeline created!");

		return true;
	}

	void VulkanPipeline::Destroy(VkAllocationCallbacks* allocator)
	{
		MZ_CORE_TRACE("Destroying Vulkan graphics pipeline...");
		vkDestroyShaderModule(m_device, m_vertShaderModule, allocator);
		vkDestroyShaderModule(m_device, m_fragShaderModule, allocator);
	}
}