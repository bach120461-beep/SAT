#pragma once
#include "included.h"

// settings
const unsigned int SCR_WIDTH = 1980;
const unsigned int SCR_HEIGHT = 1080;

class Application
{
private:
	GLFWwindow* m_window;
	Camera m_camera;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Renderer> m_renderer;
	std::vector<SceneObject> objectList;;
	Boundary track;
	Track builder;
	TrackConstraint m_constraint;
	bool isRunning = true;
public:
	Application();
	//~Application();
	void Run();
private:
	void Init();
	void ProcessInput(GLFWwindow* window);
	void Update(float dt);
	void Render();
	//Section of the application - Modifying track
	void InclinePlane();
	void CircularMotion();
	void Collision();
	void ConstantAcceleration();	
};