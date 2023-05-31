#include<iostream>
#include<engine/src/marzanna.h>

class TestApplication : public mz::Application {
public:
	TestApplication() {
	}
	~TestApplication() {

	}
};

mz::Application* mz::CreateApplication() {
	return new TestApplication;
}