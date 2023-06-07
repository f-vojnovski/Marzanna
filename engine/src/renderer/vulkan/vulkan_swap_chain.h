#pragma once

#include "engine/src/mzpch.h"
#include "vulkan_device.h"

namespace mz {
	class VulkanSwapChain {
	private:
		std::shared_ptr<VulkanContext> contextPtr;

		void ChooseSurfaceFormat();
		void ChoosePresentMode();
		void ChooseExtent();
	public:
		VulkanSwapChain(std::shared_ptr<VulkanContext> contextPtr);
		void Create();
		void Destroy();
		void CreateImageViews();
		bool CreateSyncObjects();
		void DestroySyncObjects();
		bool AcquireNextImageIndex();
	};
}