#include "vulkan_render_pass.h"

namespace mz {
	VulkanRenderPass::VulkanRenderPass(std::shared_ptr<VulkanContext> contextPtr)
	{
        this->contextPtr = contextPtr;
	}

	bool VulkanRenderPass::Create()
	{
		MZ_CORE_TRACE("Creating render pass...");

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = contextPtr->swapChain.surfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;

        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;

        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(contextPtr->device.logicalDevice, &renderPassInfo, contextPtr->allocator, &contextPtr->mainRenderPass.handle) != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to create render pass");
			return false;
		}

		MZ_CORE_INFO("Render pass created!");
	}
	void VulkanRenderPass::Destroy()
	{
		MZ_CORE_TRACE("Destroying render pass...");
		vkDestroyRenderPass(contextPtr->device.logicalDevice, contextPtr->mainRenderPass.handle, contextPtr->allocator);
	}
    
    void VulkanRenderPass::Begin(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = contextPtr->mainRenderPass.handle;
        renderPassInfo.framebuffer = contextPtr->swapChain.framebuffers[imageIndex];

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = contextPtr->swapChain.extent;

        VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }
    
    void VulkanRenderPass::End(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        vkCmdEndRenderPass(commandBuffer);
    }
}