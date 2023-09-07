#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/render_types.h"

namespace mz {
#define MAX_FRAMES_IN_FLIGHT 2

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
	
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct VulkanDeviceInfo {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		QueueFamilyIndices queueFamalies;
		SwapChainSupportDetails swapChainDetails;

		VkCommandPool graphicsCommandPool;

		VkPhysicalDeviceProperties physicalDeviceProperties;

		VkFormat depthFormat;
	};

	struct VulkanSwapChainInfo {
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
		VkExtent2D extent;
		uint32_t imageCount;

		VkSwapchainKHR handle;

		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		uint32_t nextImageIndex;

		std::vector<VkFramebuffer> framebuffers;

		VkImage depthImage;
		VkDeviceMemory depthImageMemory;
		VkImageView depthImageView;

		bool recreating = false;
	};

	struct VulkanRenderPassInfo {
		VkRenderPass handle;
	};

	struct VulkanPipelineInfo {
		VkPipelineLayout layout;
		VkPipeline handle;
		VkDescriptorSetLayout descriptorSetLayout;
		VkDescriptorPool descriptorPool;
	};

	struct UniformBuffer {
		VkBuffer handle;
		VkDeviceMemory memory;
		void* mapped;
	};

	struct VulkanContext {
		VkInstance instance;
		VkSurfaceKHR surface;
		VkAllocationCallbacks* allocator;
		const Window* window;
		std::vector<const char*> validationLayers;
		
		VulkanDeviceInfo device;
		
		VulkanSwapChainInfo swapChain;

		VulkanRenderPassInfo mainRenderPass;
		VulkanPipelineInfo graphicsRenderingPipeline;

		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentFrame = 0;
		bool framebufferResized = false;

		std::vector<UniformBuffer> uniformBuffers;

		VkSampler textureSampler;

		uint64_t vertexBufferOffset;
		uint64_t indexBufferOffset;
	};
}