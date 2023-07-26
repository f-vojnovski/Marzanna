#pragma once

#include "engine/src/mzpch.h"
#include "engine/src/renderer/render_api.h"

namespace mz {
	// forward declaration
	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void OnGraphicsUpdate();
	private:
		entt::registry m_registry;
		std::shared_ptr<RenderAPI> m_renderApi;
		std::unordered_map<UUID, Entity> m_entityMap;

		friend class Entity;
	};
}