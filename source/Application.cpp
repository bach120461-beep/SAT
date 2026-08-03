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
        ProcessInput(m_window);
        Update(dt);
        Render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
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
void Application::CircularMotion()
{
    builder.reset(glm::vec3(-500.0f, -100.0f, 0.0f), 0.0f);
    objectList.clear();
}
void Application::Collision()
{
    builder.reset(glm::vec3(-500.0f, -100.0f, 0.0f), 0.0f);
    objectList.clear();

    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    SceneObject car(carMesh, glm::vec3(-400.0f, -80.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), 1.0f, Color::Black);
    SceneObject car2(carMesh, glm::vec3(400.0f, -80.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, Color::Blue);
    objectList.push_back(car);
    objectList.push_back(car2);

    builder.addGround(1000.0f, 5.0f,0.5f);
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

    // Presets switching
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) InclinePlane();
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) CircularMotion();
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) Collision();
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) ConstantAcceleration();
}
