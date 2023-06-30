#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_device.h"

namespace mz {
	class VulkanSwapChain {
	public:
		void Create();
		void Destroy();
		void CreateImageViews();
		bool CreateSyncObjects();
		void DestroySyncObjects();
		bool CreateFramebuffers();
		void DestroyFramebuffers();
		VkResult AcquireNextImageIndex();
		void Cleanup();
		bool Recreate();
		static bool CreateDepthResources();
		static void DestroyDepthResources();
		inline static void SetContextPointer(std::shared_ptr<VulkanContext> contextPtr) { s_contextPtr = contextPtr; }
	private:
		inline static std::shared_ptr<VulkanContext> s_contextPtr = nullptr;

		void ChooseSurfaceFormat();
		void ChoosePresentMode();
		void ChooseExtent();

	};
}