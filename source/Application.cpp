#include "Application.h"


Application::Application() :
    m_window(nullptr), 
    m_camera(SCR_WIDTH, SCR_HEIGHT), 
    m_shader("Resources/Shader.vert", "Resources/Shader.frag"),
    m_renderer(m_shader, m_camera)
{}
void Application::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Motion Simulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
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
        m_renderer.clear(Color::White);
        Update(dt);
    }
}

void Application::Update(float dt)
{
    for (auto& object : objectList)
    {
        object.physics.update(dt);
    }
}

void Application::Render()
{
    Track.draw(m_renderer);
    for (auto& object : objectList)
    {
        m_renderer.drawMesh(*object.mesh, object.getModelMat(), object.color);
    }
}

void Application::ProcessInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/*
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
*/

void Application::InclinePlane()
{
    Track.segments.clear();
    Track.addGround(400.0f, 5.0f, glm::vec3(-400.0f, 35.0f, 0.0f))
        .addWall(100.0f, 5.0f, glm::vec3(-600.0f, 35.0f, 0.0f))
        .addSlope(220.0f, 5.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::radians(-10.0f))
        .addGround(400.0f, 5.0f, glm::vec3(400.0f, -35.0f, 0.0f))
        .addWall(100.0f, 5.0f, glm::vec3(600.0f, -17.5f, 0.0f));
}
void Application::CircularMotion()
{
    Track.segments.clear();
}
void Application::Collision()
{
    Track.segments.clear();
}
void Application::ConstantAcceleration()
{
    Track.segments.clear();
}