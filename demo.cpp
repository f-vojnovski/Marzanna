#include<iostream>
#include<engine/src/marzanna.h>

class ExampleLayer : public mz::Layer{
public:
	ExampleLayer() : Layer("example") {}

	void OnUpdate() override {
		//MZ_INFO("Example Layer OnUpdate()");
	}

	void OnEvent(mz::Event& e) override {
		//MZ_INFO("Example Layer OnEvent(e)");
	}
};

class TestApplication : public mz::Application {
public:
	TestApplication() : Application() {
		PushLayer(new ExampleLayer());

		auto entity = m_activeScene->CreateEntity("entity 1");
		
		mz::GeometryRendererComponent entityGeometryComponent;
		entityGeometryComponent.geometry = m_geometrySystem->Acquire("tower2.fbx");
		entity.AddComponent<mz::GeometryRendererComponent>(entityGeometryComponent);

		mz::Transform3dComponent entityTransformComponent;
		entityTransformComponent.Scale = glm::vec3(0.12f, 0.12f, 0.12f);
		entityTransformComponent.Translation = glm::vec3(0.0f, 0.2f, 0.1f);
		entity.AddComponent<mz::Transform3dComponent>(entityTransformComponent);

		//auto entity2 = m_activeScene->CreateEntity("entity 2");

		//mz::GeometryRendererComponent entity2GeometryComponent;
		//entity2GeometryComponent.geometry = m_geometrySystem->Acquire("handgun.obj");
		//entity2.AddComponent<mz::GeometryRendererComponent>(entity2GeometryComponent);

		//mz::Transform3dComponent entity2TransformComponent;
		//entity2TransformComponent.Translation = glm::vec3(0.2f, 1.7f, 0.3f);
		//entity2.AddComponent<mz::Transform3dComponent>(entity2TransformComponent);
	}

	~TestApplication() {

	}
};

mz::Application* mz::CreateApplication() {
	return new TestApplication();
}