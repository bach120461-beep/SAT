#pragma once
#include "included.h"
#include "Track.h"  
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

enum class ViewState 
{
	Home,
	Simulation
};

enum class ScenarioType
{
	None,
	InclinePlane,
	CircularMotion,
	Collision,
	ConstantAcceleration
};

class Application
{
private:
	GLFWwindow* m_window = nullptr;
	Camera m_camera;
	std::unique_ptr<Shader> m_shader;
	std::unique_ptr<Renderer> m_renderer;
	std::vector<SceneObject> objectList;;
	Boundary track;
	Track builder;
	TrackConstraint m_constraint;
	const int m_width;
	const int m_height;

	//Play state
	ViewState currentView = ViewState::Simulation;
	ScenarioType currentScenario = ScenarioType::InclinePlane;
	bool isPaused = true;
	float timeScale = 1.0f;

	//Environment parameter
	float frictionCoefficient = 0.0f;
	float g = 9.8f;
	bool directionVec = true;
	bool speedVec = true;

	//for text font
	ImFont* m_fontRegular = nullptr;
	ImFont* m_fontBold = nullptr;
	float m_panelWidth = 380.0f;
	float m_panelHeight = 500.0f;

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
	void HomePage();
	void InclinePlane();
	void CircularMotion();
	void Collision();
	void ConstantAcceleration();
	//Control 
	void ResetSimulation();
	void PlaySimulation();
	void PauseSimulation();

	//UI
	void InitImGui();
	void RenderUI();
	void ShutDownImGui();
	void RenderTopBar();
	void RenderSimulationControls();
	void RenderSidePanel();
	void LoadScenario(ScenarioType type);
};