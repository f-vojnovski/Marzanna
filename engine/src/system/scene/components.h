#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/core/uuid.h"
#include "engine/src/renderer/geometry.h"

namespace mz {
	struct IDComponent {
		UUID id;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct Transform3dComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		Transform3dComponent() = default;
		Transform3dComponent(const Transform3dComponent&) = default;
		Transform3dComponent(const glm::vec3& translation) : Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct GeometryRendererComponent
	{
		std::shared_ptr<Geometry> geometry;

		GeometryRendererComponent() = default;
		GeometryRendererComponent(const GeometryRendererComponent&) = default;
	};
}