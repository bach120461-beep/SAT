#include "included.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1980;
const unsigned int SCR_HEIGHT = 1080;

int main()
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
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    Camera camera(SCR_WIDTH, SCR_HEIGHT);
    auto groundMesh = MeshFactory::createGround(800.0f, 2.0f);
    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    Shader basicShader("Resources/Shader.vert", "Resources/Shader.frag");
    Renderer renderer(basicShader, camera);
    // Circular motion track
    Boundary linearTrack;
    // Initialize builder at start position (-500, -200) facing Right (0 degrees)
    Track builder(linearTrack, glm::vec3(-500.0f, -200.0f, 0.0f), 0.0f);

    SceneObject square1(carMesh, glm::vec3(-20.0f, -200.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), 1.0f, Color::Black);

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

    TrackConstraint constraint(builder);
    constraint.isActive = true;

    float lastFrame = glfwGetTime();
    square1.restitution = 0.5f;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        double currentFrame = glfwGetTime();
        float dt = static_cast<float>(currentFrame - lastFrame);
        if (dt > 0.016f) dt = 0.016f;
        lastFrame = currentFrame;
        renderer.clear(Color::White);
        CollisionInfo info;
        constraint.constrain(square1, dt, 25.0f);
        if (!constraint.isActive) {
            float g = -98.0f;
            square1.physics.applyForce(glm::vec3(0.0f, g * square1.physics.mass, 0.0f));
        }
        //square1.turnToVelocity();
        square1.physics.update(dt);
        
        std::cout << "Normal force: " << constraint.normalForce << " N\n";
        linearTrack.draw(renderer);
        renderer.drawMesh(*square1.mesh, square1.getModelMat(), square1.color);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
