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
		entityGeometryComponent.geometry = m_geometrySystem->Acquire("handgun.obj");
		entity.AddComponent<mz::GeometryRendererComponent>(entityGeometryComponent);

		mz::Transform3dComponent entityTransformComponent;
		entity.AddComponent<mz::Transform3dComponent>(entityTransformComponent);

	}
	~TestApplication() {

	}
};

mz::Application* mz::CreateApplication() {
	return new TestApplication();
}