#include<iostream>
#include<engine/src/marzanna.h>

class TestApplication : public mz::Application {
public:
	TestApplication() {
		//MZ_INFO("Application class inherited properly!");
	}
	~TestApplication() {

	}
};

mz::Application* mz::CreateApplication() {
	return new TestApplication;
}