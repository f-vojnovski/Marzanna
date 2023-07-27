#include "scene.h"
#include "entity.h"
#include "engine/src/renderer/render_api.h"

namespace mz {
	Scene::Scene()
	{
	}
	Scene::~Scene()
	{
	}
	
	Entity Scene::CreateEntity(const std::string& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}
	
	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string& name)
	{
		Entity entity = { m_registry.create(), this };
		entity.AddComponent<IDComponent>(uuid);
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		m_entityMap[uuid] = entity;

		return entity;
	}
	
	void Scene::DestroyEntity(Entity entity)
	{
		m_entityMap.erase(entity.GetUUID());
		m_registry.destroy(entity);
	}
	
	void Scene::OnGraphicsUpdate()
	{
		RenderApiDrawCallArgs drawArgs;

		auto group = m_registry.group<Transform3dComponent>(entt::get<GeometryRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, geometry] = group.get<Transform3dComponent, GeometryRendererComponent>(entity);

			GeometryWithPosition geometryInWorldSpace;
			geometryInWorldSpace.geometry = geometry.geometry;
			drawArgs.geometries.push_back(geometryInWorldSpace);
		}
		Application::Get().GetRenderApi().DrawFrame(drawArgs);
	}
}