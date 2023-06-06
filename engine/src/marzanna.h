#pragma once

#include "defines.h"
#include "core/log.h"
#include "core/log.cpp"
#include "core/window.h"
#include "platform/windows_window.h"
#include "platform/windows_window.cpp"
#include "core/entry.h"
#include "core/application.h"
#include "core/application.cpp"
#include "core/layer.h"
#include "core/layer.cpp"
#include "core/layer_stack.h"
#include "core/layer_stack.cpp"
#include "core/input.h"
#include "platform/windows_input.h"
#include "platform/windows_input.cpp"
#include "platform/windows_vulkan.h"
#include "platform/windows_vulkan.cpp"
#include "core/input_keycodes.h"
#include "renderer/renderer_backend.h"
#include "renderer/render_api.h"
#include "renderer/render_api.cpp"
#include "renderer/vulkan/vulkan_context.h"
#include "renderer/vulkan/vulkan_renderer_backend.h"
#include "renderer/vulkan/vulkan_renderer_backend.cpp"
#include "renderer/vulkan/vulkan_utils.h"
#include "renderer/vulkan/vulkan_utils.cpp"
#include "renderer/vulkan/vulkan_platform.h"
#include "renderer/vulkan/vulkan_device.h"
#include "renderer/vulkan/vulkan_device.cpp"
#include "renderer/vulkan/vulkan_swap_chain.h"
#include "renderer/vulkan/vulkan_swap_chain.cpp"
#include "renderer/vulkan/vulkan_pipeline.h"
#include "renderer/vulkan/vulkan_pipeline.cpp"
#include "renderer/vulkan/vulkan_render_pass.h"
#include "renderer/vulkan/vulkan_render_pass.cpp"
#include "renderer/vulkan/shaders/vulkan_shader_utils.h"
#include "renderer/vulkan/shaders/vulkan_shader_utils.cpp"
#include "renderer/vulkan/vulkan_command_buffer.h"
#include "renderer/vulkan/vulkan_command_buffer.cpp"
#include "system/file_reader.h"
#include "system/file_reader.cpp"