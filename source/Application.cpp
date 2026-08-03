#include "Application.h"


Application::Application(const int SCR_WIDTH, const int SCR_HEIGHT) :
    m_width(SCR_WIDTH), m_height(SCR_HEIGHT),
    m_camera(SCR_WIDTH, SCR_HEIGHT), builder(track), m_constraint(builder)
{}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void Application::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, "Motion Simulation", NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    m_shader = std::make_unique<Shader>("Resources/Shader.vert", "Resources/Shader.frag");
    m_renderer = std::make_unique<Renderer>(*m_shader, m_camera);

    m_constraint.isActive = true;

    //Set up ImGui
    InitImGui();

    InclinePlane();

}

void Application::Run()
{
    float lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(m_window))
    {
        double currentFrame = glfwGetTime();
        float dt = static_cast<float>(currentFrame - lastFrame);
        if (dt > 0.016f) dt = 0.016f;
        lastFrame = currentFrame;
        glfwPollEvents();
        ProcessInput(m_window);
        if (!isPaused && currentView == ViewState::Simulation)
        {
            Update(dt * timeScale);
        }

        Render();
        RenderUI();

        glfwSwapBuffers(m_window);
    }
    ShutDownImGui();
    glfwTerminate();
}

void Application::Update(float dt)
{
    // Apply Track Constraints
    for (auto& object : objectList)
    {
        m_constraint.constrain(object, dt, 25.0f);
        if (!m_constraint.isActive) {
            float g = -98.0f;
            object.physics.applyForce(glm::vec3(0.0f, g * object.physics.mass, 0.0f));
        }
        object.physics.update(dt);
    }


    for (size_t i = 0; i < objectList.size(); ++i) {
        for (size_t j = i + 1; j < objectList.size(); ++j) {
            CollisionInfo info = Collision::checkAABB(objectList[i], objectList[j]);
            Collision::resolveCollision(objectList[i], objectList[j], info);
        }
    }
}

void Application::Render()
{
    m_renderer->clear(Color::White);
    track.draw(*m_renderer);
    for (auto& object : objectList)
    {
        m_renderer->drawMesh(*object.mesh, object.getModelMat(), object.color);
    }
}

void Application::HomePage()
{
    ImGui::SetNextWindowPos(ImVec2(0, 30), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - 30), ImGuiCond_Always);

    ImGui::Begin("Homepage Catalog", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);

    ImGui::Text("Simulation Catalog");
    ImGui::Separator();

    if (ImGui::Button("Incline Plane", ImVec2(200, 40))) {
        LoadScenario(ScenarioType::InclinePlane);
        currentView = ViewState::Simulation;
    }
    if (ImGui::Button("Collision", ImVec2(200, 40))) {
        LoadScenario(ScenarioType::Collision);
        currentView = ViewState::Simulation;
    }

    ImGui::End();
}


void Application::InclinePlane()
{
    builder.reset(glm::vec3(-500.0f, -100.0f, 0.0f), 0.0f);
    objectList.clear();

    //Adding object
    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    SceneObject car(carMesh, glm::vec3(-400.0f, -90.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), 1.0f, Color::Black);
    objectList.push_back(car);

    
    builder
        .addGround(200.0f, 5.0f)
        .addSlope(200.0f, 30.0f, 5.0f)
        .addSlope(100.0f, -120.0f, 5.0f)
        .addSlope(400.0f, 90.0f, 5.0f);
}
void Application::CircularMotion()
{
    builder.reset(glm::vec3(-500.0f, -100.0f, 0.0f), 0.0f);
    objectList.clear();

    //Adding object
    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    SceneObject car(carMesh, glm::vec3(-400.0f, -90.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), 1.0f, Color::Black);
    objectList.push_back(car);


    builder
        .addGround(200.0f, 5.0f)
        .addArcLeft(40.0f, 60.0f, 5.0f)
        .addArcRight(100.0f, 120.0f, 5.0f)
        .addArcLeft(40.0f, 60.0f, 5.0f)
        .addGround(200.0f, 5.0f)
        .addArcRight(40.0f, 60.0f, 5.0f)
        .addArcLeft(100.0f, 120.0f, 5.0f)
        .addArcRight(40.0f, 60.0f, 5.0f)
        .addGround(100.0f, 5.0f)
        .addSlope(100.0f, 30.0f, 5.0f);
}
void Application::Collision()
{
    builder.reset(glm::vec3(-1000.0f, -100.0f, 0.0f), 0.0f);
    objectList.clear();

    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    SceneObject car(carMesh, glm::vec3(-400.0f, -80.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), 1.0f, Color::Black);
    SceneObject car2(carMesh, glm::vec3(400.0f, -80.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, Color::Blue);
    objectList.push_back(car);
    objectList.push_back(car2);

    builder.addGround(2000.0f, 5.0f,0.5f);
}
void Application::ConstantAcceleration()
{
    builder.segments.clear();
    objectList.clear();
}


void Application::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Application::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Enable Docking & Keyboard Controls 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    //Adjust style with viewports
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    //Binding
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    
    //ImGui::SetWindowFontScale(5.0f);
}

void Application::RenderUI()
{
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create a DockSpace over the main window 
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);

    RenderTopBar();

    if (currentView == ViewState::Simulation)
    {
        RenderSidePanel();
    }
    else if (currentView == ViewState::Home)
    {
        HomePage();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
    
}

void Application::RenderTopBar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 10.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 0.0f));

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::Selectable("Browse", currentView == ViewState::Home, 0, ImVec2(100, 0))) {
            currentView = ViewState::Home;
        }
        if (ImGui::Selectable("Filter", currentView == ViewState::Home, 0, ImVec2(100, 0))) {
            currentView = ViewState::Home;
        }
        if (ImGui::Selectable("Custom", currentView == ViewState::Simulation, 0, ImVec2(100, 0))) {
            currentView = ViewState::Simulation;
        }

        ImGui::EndMainMenuBar();
    }
    ImGui::PopStyleVar(2);
}
void Application::RenderSimulationControls()
{
    if (isPaused)
    {
        if (ImGui::Button("PLAY ", ImVec2(120, 35))) PlaySimulation();
    }
    else
    {
        if (ImGui::Button("PAUSE ", ImVec2(120, 35)))  PauseSimulation();
    }

    //Reset button
    ImGui::SameLine();
    if (ImGui::Button("RESET", ImVec2(120, 35)))
    {
        ResetSimulation();
        PauseSimulation();
    }
}

void Application::RenderSidePanel()
{
    ImGuiIO& io = ImGui::GetIO();
    float topBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowSizeConstraints(
        ImVec2(250.0f, 150.0f),                        
        ImVec2(600.0f, io.DisplaySize.y - topBarHeight)  
    );

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - m_panelWidth, topBarHeight), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(m_panelWidth, m_panelHeight), ImGuiCond_FirstUseEver);

    ImGui::Begin("Simulation Parameters", nullptr,  ImGuiWindowFlags_NoCollapse);

    ImVec2 currentSize = ImGui::GetWindowSize();
    m_panelWidth = currentSize.x;
    m_panelHeight = currentSize.y;

    ImGui::Text("Performance: %.1f FPS", io.Framerate);

    //Play - Control Simulation
    ImGui::Spacing();
    RenderSimulationControls();

    ImGui::Separator();
    ImGui::Spacing();

    // Vector Visual Toggles
    ImGui::Checkbox("Direction Vector", &directionVec);
    ImGui::Checkbox("Speed Vector", &speedVec);

    ImGui::Separator();

    //Adjust environment parameter
    ImGui::Text("Environment");
    ImGui::PushItemWidth(180.0f);
    ImGui::SliderFloat("Friction", &frictionCoefficient, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Gravity", &g, 0.0f, 25.0f, "%.2f m/s²");

    ImGui::Separator();

    //Object observer + editing parameter
    for (size_t i = 0; i < objectList.size(); ++i)
    {
        std::string title = "Car " + std::to_string(i + 1);
        ImGui::Text("%s", title.c_str());

        ImGui::PushID(static_cast<int>(i));
        ImGui::InputFloat("Mass (kg)", &objectList[i].physics.mass, 0.5f, 5.0f, "%.1f");

        // Position (reads current physics position, lets ImGui modify pos[], then writes back every frame)
        float pos[3] = {
        objectList[i].physics.position.x,
        objectList[i].physics.position.y,
        objectList[i].physics.position.z
        };

        // 2. ImGui::InputFloat3 returns TRUE while modified, BUT we guard against NaN / empty inputs!
        if (ImGui::InputFloat3("Position (m)", pos, "%.1f"))
        {
            // Safety Check: Ensure none of the values are NaN or Infinity (happens when backspacing/selecting)
            if (!std::isnan(pos[0]) && !std::isnan(pos[1]) && !std::isnan(pos[2]) &&
                !std::isinf(pos[0]) && !std::isinf(pos[1]) && !std::isinf(pos[2]))
            {
                objectList[i].physics.position.x = pos[0];
                objectList[i].physics.position.y = pos[1];
                objectList[i].physics.position.z = pos[2];
            }
        }

        // Copy back directly (just like your current speed logic)
        objectList[i].physics.position.x = pos[0];
        objectList[i].physics.position.y = pos[1];
        objectList[i].physics.position.z = pos[2];
        // Show current velocity 
        float currentSpeed = glm::length(objectList[i].physics.velocity);
        ImGui::InputFloat("Speed (m/s)", &currentSpeed, 1.0f, 5.0f, "%.1f");
        

        if (isPaused) {
            objectList[i].physics.velocity.x = currentSpeed;
        }

        ImGui::PopID();
        ImGui::Spacing();
    }
    ImGui::PopItemWidth();
    ImGui::End();
}

void Application::ShutDownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
void Application::LoadScenario(ScenarioType type)
{
    currentScenario = type;
    isPaused = true;

    switch (currentScenario)
    {
    case ScenarioType::InclinePlane:
        InclinePlane();
        break;
    case ScenarioType::CircularMotion:
        CircularMotion();
        break;
    case ScenarioType::Collision:
        Collision();
        break;
    case ScenarioType::ConstantAcceleration:
        ConstantAcceleration();
        break;
    }
}


void Application::PlaySimulation()
{
    isPaused = false;
}

void Application::PauseSimulation()
{
    isPaused = true;
}
void Application::ResetSimulation()
{
    LoadScenario(currentScenario);
}
