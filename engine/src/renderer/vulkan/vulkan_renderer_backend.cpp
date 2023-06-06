#include "vulkan_renderer_backend.h"
#include "vulkan_utils.h"
#include "vulkan_command_buffer.h"

namespace mz {
	VulkanRendererBackend::VulkanRendererBackend(const RendererBackendArgs args) 
	{
		m_name = args.name;
		m_window = args.window;
		m_allocator = nullptr;
		m_window = args.window;
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
		m_validationLayers.push_back("VK_LAYER_KHRONOS_validation");

		uint32_t availableLayerCount = 0;
		VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr));
		std::vector<VkLayerProperties> availableLayers(availableLayerCount);
		VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()))
			for (const char* layerName : m_validationLayers) {
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
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();

		VK_CHECK(vkCreateInstance(&createInfo, m_allocator, &m_instance));

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
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		MZ_ASSERT_MSG(func, "Failed to create debug messenger!");
		VK_CHECK(func(m_instance, &debugCreateInfo, m_allocator, &m_debugMessegner));
		MZ_CORE_INFO("Vulkan debugger created.");

		// Surface creation
		MZ_CORE_TRACE("Creating Vulkan surface...");
		if (!PlatformCreateVulkanSurface(m_instance, m_window, m_allocator, &m_surface)) {
			MZ_CORE_CRITICAL("Surface creation failed!");
			return false;
		}
		MZ_CORE_INFO("Vulkan surface created successfully!");

		// Device creation
		m_device = std::make_unique<VulkanDevice>(m_surface);
		
		if (!m_device->SelectPhysicalDevice(m_instance)) {
			MZ_CORE_CRITICAL("Failed to select physical device!");
			return false;
		}

		if (!m_device->CreateLogicalDevice(m_validationLayers, m_allocator)) {
			MZ_CORE_CRITICAL("Failed to create Vulkan logical device!");
			return false;
		}

		// Swap chain creation
		m_swapChain = std::make_shared<VulkanSwapChain>(m_surface, m_device->GetLogicalDevice(), m_device->GetSwapChainSupportDetails(), m_device->GetQueueFamilyIndices());
		m_swapChain->Create();

		// Main render pass
		m_mainRenderPass = std::make_unique<VulkanRenderPass>(m_device->GetLogicalDevice(), m_swapChain);
		if (!m_mainRenderPass->Create()) {
			MZ_CORE_CRITICAL("Failed to create main render pass");
			return false;
		}

		// Pipeline creation
		m_pipeline = std::make_unique<VulkanPipeline>(m_device->GetLogicalDevice(), m_swapChain);
		if (!m_pipeline->Create(m_mainRenderPass->GetRenderPass())) {
			MZ_CORE_CRITICAL("Failed to create Vulkan graphics pipeline!");
			return false;
		}

		// Framebuffers
		m_swapChain->CreateFramebuffers(m_mainRenderPass->GetRenderPass());

		// Command pool
		if (!m_device->CreateGraphicsCommandPool()) {
			MZ_CORE_CRITICAL("Failed to create graphics command pool!");
			return false;
		}

		// Command buffer {IMPL LATER}
		m_buffer = std::make_unique<VulkanCommandBuffer>(m_device->GetGraphicsCommandPool(), m_device->GetLogicalDevice());
		m_buffer->Create();

		// Sync objects
		if (!m_swapChain->CreateSyncObjects()) {
			MZ_CORE_CRITICAL("Failed to create swap chain sync objects!");
			return false;
		}

		return true;
	}
	
	void VulkanRendererBackend::Shutdown()
	{
		vkDeviceWaitIdle(m_device->GetLogicalDevice());

		m_swapChain->DestroySyncObjects();

		m_device->DestroyGraphicsCommandPool();

		m_swapChain->DestroyFramebuffers();

		m_pipeline->Destroy(m_allocator);

		m_mainRenderPass->Destroy();

		m_swapChain->Destroy(m_allocator);

		m_device->Shutdown(m_allocator);

		MZ_CORE_TRACE("Destroying surface...");
		vkDestroySurfaceKHR(m_instance, m_surface, m_allocator);

		MZ_CORE_TRACE("Destroying Vulkan debugger...");
		if (m_debugMessegner) {
			PFN_vkDestroyDebugUtilsMessengerEXT func =
				(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
			func(m_instance, m_debugMessegner, m_allocator);
		}

		MZ_CORE_TRACE("Destroying Vulkan instance...");
		vkDestroyInstance(m_instance, m_allocator);
	}
	
	bool VulkanRendererBackend::BeginFrame()
	{
		vkWaitForFences(m_device->GetLogicalDevice(), 1, &m_swapChain->GetInFlightFence(), VK_TRUE, UINT64_MAX);
		vkResetFences(m_device->GetLogicalDevice(), 1, &m_swapChain->GetInFlightFence());

		m_swapChain->AcquireNextImageIndex();
		uint32_t imageIndex = m_swapChain->GetNextImageIndex();

		auto commandBuffer = m_buffer->GetHandle();

		vkResetCommandBuffer(commandBuffer, 0);

		RecordCommandBuffer(commandBuffer, imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_swapChain->GetImageAvailableSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { m_swapChain->GetRenderFinishedSemaphore()};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(m_device->GetGraphicsQueue(), 1, &submitInfo, m_swapChain->GetInFlightFence()));

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_swapChain->GetHandle()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(m_device->GetPresentQueue(), &presentInfo);

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
		renderPassInfo.renderPass = m_mainRenderPass->GetRenderPass();
		renderPassInfo.framebuffer = m_swapChain->GetFramebuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapChain->GetExtent();

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->GetHandle());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_swapChain->GetExtentHeight();
		viewport.height = (float)m_swapChain->GetExtentWidth();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapChain->GetExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));
	}
}