#include "vulkan_renderer_backend.h"
#include "vulkan_utils.h"

namespace mz {
	VulkanRendererBackend::VulkanRendererBackend(const RendererBackendArgs args) 
	{
		contextPtr = std::make_shared<VulkanContext>();
		m_name = args.name;
		contextPtr->window = args.window;
	}

	bool VulkanRendererBackend::Initialize()
	{
		MZ_CORE_TRACE("Creating Vulkan instance...");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = m_name.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = m_name.c_str();
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		// Vulkan extensions 
		std::vector<const char*> requiredExtensions;
		requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

		// Platform specific extensions here
		PlatformGetRequiredExtensionNames(requiredExtensions);

		// Debug extensions
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		MZ_CORE_INFO("Required Vulkan extensions: ");
		for (auto& extension : requiredExtensions) {
			MZ_CORE_TRACE(extension);
		}

		// Vulkan validation layers
		// Non-release builds
		MZ_CORE_INFO("Validation layers enabled. Enumerating...");
		contextPtr->validationLayers.push_back("VK_LAYER_KHRONOS_validation");

		uint32_t availableLayerCount = 0;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()))
			for (const char* layerName : contextPtr->validationLayers) {
				MZ_CORE_TRACE("Searching for validation layer {0}", layerName);
				bool layerFound = false;
				for (const auto& layerProperties : availableLayers) {
					if (strcmp(layerName, layerProperties.layerName) == 0) {
						MZ_CORE_TRACE("Found");

						layerFound = true;
						break;
					}
				}

				if (!layerFound) {
					MZ_CORE_ERROR("Required validation layer is missing: {0}", layerName);
					return false;
				}
			}

		VkInstanceCreateInfo createInfo = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();
		createInfo.enabledLayerCount = static_cast<uint32_t>(contextPtr->validationLayers.size());
		createInfo.ppEnabledLayerNames = contextPtr->validationLayers.data();

		VK_CHECK(vkCreateInstance(&createInfo, contextPtr->allocator, &contextPtr->instance));

		MZ_CORE_INFO("Vulkan instance created successfully!");

		// Debugger
		MZ_CORE_TRACE("Creating Vulkan debugger...");
		unsigned int logSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  //|
		//    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
		debugCreateInfo.messageSeverity = logSeverity;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugCreateInfo.pfnUserCallback = VulkanDebugCallback;

		PFN_vkCreateDebugUtilsMessengerEXT func =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(contextPtr->instance, "vkCreateDebugUtilsMessengerEXT");
		MZ_ASSERT_MSG(func, "Failed to create debug messenger!");
		VK_CHECK(func(contextPtr->instance, &debugCreateInfo, contextPtr->allocator, &m_debugMessegner));
		MZ_CORE_INFO("Vulkan debugger created.");

		// Surface creation
		MZ_CORE_TRACE("Creating Vulkan surface...");
		if (!PlatformCreateVulkanSurface(contextPtr->instance, contextPtr->window, contextPtr->allocator, &contextPtr->surface)) {
			MZ_CORE_CRITICAL("Surface creation failed!");
			return false;
		}
		MZ_CORE_INFO("Vulkan surface created successfully!");

		// Device creation
		m_device = std::make_unique<VulkanDevice>(contextPtr);
		
		if (!m_device->SelectPhysicalDevice()) {
			MZ_CORE_CRITICAL("Failed to select physical device!");
			return false;
		}

		if (!m_device->CreateLogicalDevice()) {
			MZ_CORE_CRITICAL("Failed to create Vulkan logical device!");
			return false;
		}

		// Swap chain creation
		m_swapChain = std::make_shared<VulkanSwapChain>(contextPtr);
		m_swapChain->Create();

		// Main render pass
		m_mainRenderPass = std::make_unique<VulkanRenderPass>(contextPtr);
		if (!m_mainRenderPass->Create()) {
			MZ_CORE_CRITICAL("Failed to create main render pass");
			return false;
		}

		// Pipeline creation
		m_pipeline = std::make_unique<VulkanPipeline>(contextPtr);
		if (!m_pipeline->Create(contextPtr->mainRenderPass.handle)) {
			MZ_CORE_CRITICAL("Failed to create Vulkan graphics pipeline!");
			return false;
		}

		// Framebuffers
		m_mainRenderPass->CreateFramebuffers();

		// Command pool
		if (!m_device->CreateGraphicsCommandPool()) {
			MZ_CORE_CRITICAL("Failed to create graphics command pool!");
			return false;
		}

		// Command buffer {IMPL LATER}
		CreateCommandBuffers();

		// Sync objects
		if (!m_swapChain->CreateSyncObjects()) {
			MZ_CORE_CRITICAL("Failed to create swap chain sync objects!");
			return false;
		}

		return true;
	}
	
	void VulkanRendererBackend::Shutdown()
	{
		vkDeviceWaitIdle(contextPtr->device.logicalDevice);

		m_swapChain->DestroySyncObjects();

		m_device->DestroyGraphicsCommandPool();

		m_mainRenderPass->DestroyFramebuffers();

		m_pipeline->Destroy();

		m_mainRenderPass->Destroy();

		m_swapChain->Destroy();

		m_device->Shutdown();

		MZ_CORE_TRACE("Destroying surface...");
		vkDestroySurfaceKHR(contextPtr->instance, contextPtr->surface, contextPtr->allocator);

		MZ_CORE_TRACE("Destroying Vulkan debugger...");
		if (m_debugMessegner) {
			PFN_vkDestroyDebugUtilsMessengerEXT func =
				(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(contextPtr->instance, "vkDestroyDebugUtilsMessengerEXT");
			func(contextPtr->instance, m_debugMessegner, contextPtr->allocator);
		}

		MZ_CORE_TRACE("Destroying Vulkan instance...");
		vkDestroyInstance(contextPtr->instance, contextPtr->allocator);
	}
	
	bool VulkanRendererBackend::BeginFrame()
	{
		vkWaitForFences(contextPtr->device.logicalDevice, 1, &contextPtr->swapChain.inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(contextPtr->device.logicalDevice, 1, &contextPtr->swapChain.inFlightFence);

		m_swapChain->AcquireNextImageIndex();
		uint32_t imageIndex = contextPtr->swapChain.nextImageIndex;

		auto commandBuffer = m_buffer;

		vkResetCommandBuffer(commandBuffer, 0);

		RecordCommandBuffer(commandBuffer, imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { contextPtr->swapChain.imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { contextPtr->swapChain.renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(contextPtr->device.graphicsQueue, 1, &submitInfo, contextPtr->swapChain.inFlightFence));

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { contextPtr->swapChain.handle };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(contextPtr->device.presentQueue, &presentInfo);

		return true;
	}
	
	bool VulkanRendererBackend::EndFrame()
	{
		return false;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRendererBackend::VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_types,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void* user_data) {
		switch (message_severity) {
		default:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			MZ_CORE_ERROR(callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			MZ_CORE_WARN(callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			MZ_CORE_INFO(callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			MZ_CORE_TRACE(callback_data->pMessage);
			break;
		}
		return VK_FALSE;
	}
	
	void VulkanRendererBackend::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = contextPtr->mainRenderPass.handle;
		renderPassInfo.framebuffer = contextPtr->mainRenderPass.framebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = contextPtr->swapChain.extent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, contextPtr->graphicsRenderingPipeline.handle);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)contextPtr->swapChain.extent.width;
		viewport.height = (float)contextPtr->swapChain.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = contextPtr->swapChain.extent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));
	}

	bool VulkanRendererBackend::CreateCommandBuffers() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = contextPtr->device.graphicsCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(contextPtr->device.logicalDevice, &allocInfo, &m_buffer) != VK_SUCCESS) {
			MZ_CORE_CRITICAL("Failed to allocate command buffer!");
			return false;
		}
	}
}