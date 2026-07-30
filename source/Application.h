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
	Shader m_shader;
	Renderer m_renderer;
	bool isRunning = true;
	std::vector<SceneObject> objectList;;
	Boundary Track;
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
	//Adding object - these are 2 line methods
	//void addCar();
	//void addSquare();
	//void addRectangle();
	//void addTriangle();
	//void addCircle();

};