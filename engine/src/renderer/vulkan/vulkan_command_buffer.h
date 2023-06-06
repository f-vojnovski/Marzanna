#pragma once

#include "engine/src/mzpch.h"

namespace mz {
    class VulkanCommandBuffer {
    private:
        VkCommandPool m_commandPool;
        VkDevice m_device;

        VkCommandBuffer m_handle;
    public:
        VulkanCommandBuffer(VkCommandPool commandPool, VkDevice device);
        bool Create();
        void Begin();
        void End();
        inline VkCommandBuffer const GetHandle() { return m_handle; }
    };
}