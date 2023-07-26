#pragma once

#include "engine/src/mzpch.h"
#include "scene.h"

namespace mz {
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
	
		template<typename T>
		bool HasComponent()
		{
			return m_scene->m_registry.all_of<T>(m_entityHandle);
		}

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args) {
			MZ_ASSERT(!HasComponent<T>(), "Entity already contains component!");
			return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent() {
			MZ_ASSERT(HasComponent<T>(), "Entity does not contain component!");
			return m_scene->m_registry.get<T>(m_entityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			MZ_ASSERT(HasComponent<T>(), "Entity does not contain component!");
			m_scene->m_registry.remove<T>(m_entityHandle);
		}

		UUID GetUUID() { return GetComponent<IDComponent>().id; }

		operator bool() const { return m_entityHandle != entt::null; }
		operator entt::entity() const { return m_entityHandle; }
		operator uint32_t() const { return (uint32_t)m_entityHandle; }

		bool operator==(const Entity& other) const
		{
			return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_entityHandle{ entt::null };
		Scene* m_scene;
	};
}