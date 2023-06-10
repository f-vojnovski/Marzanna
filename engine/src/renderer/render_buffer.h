#pragma once

namespace mz {
	class VertexBuffer {
	public:
		virtual ~VertexBuffer();

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer {
	public:
		virtual ~IndexBuffer();

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static IndexBuffer* Create(int* indices, uint32_t size);
	};
}