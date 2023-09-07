#include "vulkan_renderer_backend.h"
#include "vulkan_utils.h"
#include "vulkan_functions.h"
#include "vulkan_geometry.h"

namespace mz {
	VulkanRendererBackend::VulkanRendererBackend(const RendererBackendArgs args)
	{
		contextPtr = std::make_shared<VulkanContext>();
		m_name = args.name;
		contextPtr->window = args.window;

		VulkanTexture::SetContextPointer(contextPtr);
		VulkanFunctions::SetContextPointer(contextPtr);
		VulkanDevice::SetContextPointer(contextPtr);
		VulkanSwapChain::SetContextPointer(contextPtr);
		VulkanPipeline::SetContextPointer(contextPtr);
		VulkanRenderPass::SetContextPointer(contextPtr);
		VulkanGeometry::SetContextPointer(contextPtr);
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
		m_device = std::make_unique<VulkanDevice>();

		if (!m_device->SelectPhysicalDevice()) {
			MZ_CORE_CRITICAL("Failed to select physical device!");
			return false;
		}

		if (!m_device->CreateLogicalDevice()) {
			MZ_CORE_CRITICAL("Failed to create Vulkan logical device!");
			return false;
		}

		// Swap chain creation
		m_swapChain = std::make_shared<VulkanSwapChain>();
		m_swapChain->Create();

		// Depth resources
		if (!VulkanSwapChain::CreateDepthResources()) {
			return false;
		}

		// Main render pass
		if (!m_mainRenderPass->Create()) {
			MZ_CORE_CRITICAL("Failed to create main render pass");
			return false;
		}

		// Descriptor set layout
		if (!CreateDescriptorSetLayout()) {
			MZ_CORE_CRITICAL("Failed to create descriptor set layout!");
			return false;
		}

		// Pipeline creation
		if (!m_pipeline->Create(contextPtr->mainRenderPass.handle)) {
			MZ_CORE_CRITICAL("Failed to create Vulkan graphics pipeline!");
			return false;
		}

		// Framebuffers
		if (!m_swapChain->CreateFramebuffers()) {
			return false;
		}

		// Command pool
		if (!m_device->CreateGraphicsCommandPool()) {
			MZ_CORE_CRITICAL("Failed to create graphics command pool!");
			return false;
		}

		// Texture sampler
		if (!CreateTextureSampler()) {
			MZ_CORE_CRITICAL("Failed to create texture sampler!");
			return false;
		}

		// Uniform buffer
		if (!CreateUniformBuffer()) {
			MZ_CORE_CRITICAL("Failed to create uniform buffer!");
			return false;
		}

		// Descriptor pool
		if (!CreateDescriptorPool()) {
			MZ_CORE_CRITICAL("Failed to create descriptor pool!");
			return false;
		}

		// Command buffer
		if (!CreateCommandBuffers()) {
			MZ_CORE_CRITICAL("Failed to create command buffers!");
			return false;
		}

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
	
		// Sync objects
		m_swapChain->DestroySyncObjects();

		// Uniform buffers
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyBuffer(contextPtr->device.logicalDevice, contextPtr->uniformBuffers[i].handle, contextPtr->allocator);
			vkFreeMemory(contextPtr->device.logicalDevice, contextPtr->uniformBuffers[i].memory, contextPtr->allocator);
		}

		vkDestroySampler(contextPtr->device.logicalDevice, contextPtr->textureSampler, contextPtr->allocator);

		// Command pool
		m_device->DestroyGraphicsCommandPool();
		
		// Framebuffers
		m_swapChain->DestroyFramebuffers();
		
		// Pipeline
		m_pipeline->Destroy();

		// Descriptor set layout
		vkDestroyDescriptorSetLayout(contextPtr->device.logicalDevice, contextPtr->graphicsRenderingPipeline.descriptorSetLayout, contextPtr->allocator);

		vkDestroyDescriptorPool(contextPtr->device.logicalDevice, contextPtr->graphicsRenderingPipeline.descriptorPool, contextPtr->allocator);
		
		// Main renderpass
		m_mainRenderPass->Destroy();

		// Depth resources
		m_swapChain->DestroyDepthResources();
		
		// Swap chain
		m_swapChain->Destroy();

		// Device
		m_device->Shutdown();

		// Surface
		MZ_CORE_TRACE("Destroying surface...");
		vkDestroySurfaceKHR(contextPtr->instance, contextPtr->surface, contextPtr->allocator);

		// Debugger
		MZ_CORE_TRACE("Destroying Vulkan debugger...");
		if (m_debugMessegner) {
			PFN_vkDestroyDebugUtilsMessengerEXT func =
				(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(contextPtr->instance, "vkDestroyDebugUtilsMessengerEXT");
			func(contextPtr->instance, m_debugMessegner, contextPtr->allocator);
		}

		// Instance
		MZ_CORE_TRACE("Destroying Vulkan instance...");
		vkDestroyInstance(contextPtr->instance, contextPtr->allocator);
	}

	bool VulkanRendererBackend::BeginFrame()
	{
		if (m_isMinimized) {
			return false;
		}

		VkFence inFlightFence = contextPtr->swapChain.inFlightFences[contextPtr->currentFrame];
		VkCommandBuffer commandBuffer = contextPtr->commandBuffers[contextPtr->currentFrame];

		vkWaitForFences(contextPtr->device.logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

		VkResult result = m_swapChain->AcquireNextImageIndex();

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			m_swapChain->Recreate();
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			MZ_CORE_ERROR("Failed to acquire swap chain image!");
			return false;
		}

		vkResetFences(contextPtr->device.logicalDevice, 1, &inFlightFence);

		uint32_t imageIndex = contextPtr->swapChain.nextImageIndex;

		vkResetCommandBuffer(commandBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)contextPtr->swapChain.extent.width;
		viewport.height = (float)contextPtr->swapChain.extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = contextPtr->swapChain.extent;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		m_mainRenderPass->Begin(commandBuffer, imageIndex);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, contextPtr->graphicsRenderingPipeline.handle);

		return true;
	}

	bool VulkanRendererBackend::EndFrame()
	{
		VkSemaphore imageAvailableSemaphore = contextPtr->swapChain.imageAvailableSemaphores[contextPtr->currentFrame];
		VkCommandBuffer commandBuffer = contextPtr->commandBuffers[contextPtr->currentFrame];
		VkSemaphore renderFinishedSemaphore = contextPtr->swapChain.renderFinishedSemaphores[contextPtr->currentFrame];
		VkFence inFlightFence = contextPtr->swapChain.inFlightFences[contextPtr->currentFrame];
		uint32_t imageIndex = contextPtr->swapChain.nextImageIndex;
		
		m_mainRenderPass->End(commandBuffer, imageIndex);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(contextPtr->device.graphicsQueue, 1, &submitInfo, inFlightFence));

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

		VkResult result = vkQueuePresentKHR(contextPtr->device.presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || contextPtr->framebufferResized) {
			contextPtr->framebufferResized = false;
			m_swapChain->Recreate();
		}
		else if (result != VK_SUCCESS) {
			MZ_CORE_ERROR("Failed to present swap chain image!");
		}

		contextPtr->currentFrame = (contextPtr->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

		return true;
	}

	void VulkanRendererBackend::OnResize()
	{
		if (Application::Get().GetWindow().GetFramebufferWidth() == 0 || Application::Get().GetWindow().GetFramebufferHeight() == 0) {
			m_isMinimized = true;
		}
		else {
			m_isMinimized = false;
		}
	}

	void VulkanRendererBackend::UpdateGlobalState(RendererGlobalState globalState)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.view = globalState.view;
		ubo.proj = globalState.projection;

		memcpy(contextPtr->uniformBuffers[contextPtr->currentFrame].mapped, &ubo, sizeof(ubo));
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

	bool VulkanRendererBackend::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());;
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(contextPtr->device.logicalDevice, &layoutInfo, contextPtr->allocator, &contextPtr->graphicsRenderingPipeline.descriptorSetLayout) != VK_SUCCESS) {
			return false;
		}

		return true;
	}

	bool VulkanRendererBackend::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		if (vkCreateDescriptorPool(contextPtr->device.logicalDevice, &poolInfo, contextPtr->allocator, &contextPtr->graphicsRenderingPipeline.descriptorPool) != VK_SUCCESS) {
			return false;
		}

		return true;
	}

	bool VulkanRendererBackend::CreateTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;

		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = contextPtr->device.physicalDeviceProperties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(contextPtr->device.logicalDevice, &samplerInfo, contextPtr->allocator, &contextPtr->textureSampler) != VK_SUCCESS) {
			MZ_CORE_CRITICAL("Failed to create texture sampler!");
			return false;
		}

		return true;
	}

	bool VulkanRendererBackend::CreateCommandBuffers() {
		contextPtr->commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = contextPtr->device.graphicsCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

		if (vkAllocateCommandBuffers(contextPtr->device.logicalDevice, &allocInfo, contextPtr->commandBuffers.data()) != VK_SUCCESS) {
			return false;
		}

		return true;
	}

	bool VulkanRendererBackend::CreateUniformBuffer()
	{
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);

		contextPtr->uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (!VulkanFunctions::CreateBuffer(
				bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				contextPtr->uniformBuffers[i].handle, contextPtr->uniformBuffers[i].memory)) {
				MZ_CORE_CRITICAL("Failed to create uniform buffers!");
				return false;
			}

			vkMapMemory(contextPtr->device.logicalDevice, contextPtr->uniformBuffers[i].memory, 0, bufferSize, 0, &contextPtr->uniformBuffers[i].mapped);
		}

		return true;
	}
}