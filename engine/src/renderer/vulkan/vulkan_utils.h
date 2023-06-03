#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/asserts.h"

namespace mz {
    const std::string VulkanResultString(VkResult result, bool getExtended);
    bool IsVulkanResultSuccess(VkResult result);
}

#define VK_CHECK(expr)                      \
    {                                       \
        MZ_ASSERT(expr == VK_SUCCESS);      \
    }