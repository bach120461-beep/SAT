#pragma once
#include "included.h"
#include "Track.h"       


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
	const int m_width;
	const int m_height;
public:
	Application(const int SCR_WIDTH, const int SCR_HEIGHT);
	//~Application();
	void Run();
	void Init();
private:
	void ProcessInput(GLFWwindow* window);
	void Update(float dt);
	void Render();
	//Section of the application - Modifying track
	void InclinePlane();
	void CircularMotion();
	void Collision();
	void ConstantAcceleration();	
};