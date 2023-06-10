#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/render_buffer.h"

namespace mz {
    class VulkanVertexBuffer : public VertexBuffer {
        virtual ~VertexBuffer();

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        static VertexBuffer* Create(float* vertices, uint32_t size);
    };

    class VulkanIndexBuffer : public IndexBuffer {

    };
}