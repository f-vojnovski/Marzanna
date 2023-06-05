#pragma once

#include "engine/src/mzpch.h"

namespace mz {
	VkShaderModule CreateShaderModule(const std::vector<char>& code, VkDevice device, VkAllocationCallbacks* allocator = nullptr);
}