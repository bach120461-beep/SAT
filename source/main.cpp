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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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


    std::vector<float> vertices =
    {
        -25.0f, -25.0f, 0.0f,  // bottom left
         25.0f, -25.0f, 0.0f,  // bottom right
         25.0f,  25.0f, 0.0f,  // top right
        -25.0f,  25.0f, 0.0f   // top left
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 2,  // first triangle
        0, 2, 3   // second triangle
    };

    Camera camera(SCR_WIDTH, SCR_HEIGHT);
    auto squareMesh = std::make_shared<Mesh>(vertices, indices);
    auto groundMesh = MeshFactory::createGround(800.0f, 2.0f);
    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    auto wallMesh = MeshFactory::createWall(300.0f, 2.0f);
    Shader basicShader("Resources/Shader.vert", "Resources/Shader.frag");
    Renderer renderer(basicShader, camera);
    SceneObject square1(carMesh, glm::vec3(-300.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, Color::Black);
    SceneObject square2(carMesh, glm::vec3(300.0f, 0.0f, 0.0f), glm::vec3(-100.0f, 0.0f, 0.0f), 1.0f, Color::Blue);
    SceneObject ground(groundMesh, glm::vec3(0.0f, -35.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, Color::Black);
    SceneObject wall(wallMesh, glm::vec3(-400.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, Color::Green);
    camera.zoom(2);
    float lastFrame = glfwGetTime();
    square1.restitution = 0.5f;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        double currentFrame = glfwGetTime();
        float dt = static_cast<float>(currentFrame - lastFrame);
        if (dt > 0.016f) dt = 0.016f;
        lastFrame = currentFrame;

        //square.applyForce(glm::vec3(50.0f,0.0f,0.0f));
        CollisionInfo info;
        square1.physics.update(dt);
        square2.physics.update(dt);
        info = Collision::checkAABB(square1, square2);
        Collision::resolveCollision(square1, square2, info);
        info = Collision::checkAABB(square1, wall);
        Collision::resolveCollision(square1, wall, info);
        info = Collision::checkAABB(square2, wall);
        Collision::resolveCollision(square2, wall, info);
        if (square1.physics.position.x > 400.0f || square1.physics.position.x < -400.0f)
            square1.physics.velocity.x = square1.physics.velocity.x * (-1);
        if (square2.physics.position.x < -400.0f || square2.physics.position.x >400.0f)
            square2.physics.velocity.x = square2.physics.velocity.x * (-1);
        renderer.clear(glm::vec4(1.0f,1.0f,1.0f,1.0f));
        
        
        renderer.drawMesh(*square1.mesh, square1.getModelMat(),square1.color);
        renderer.drawMesh(*square2.mesh, square2.getModelMat(), square2.color);
        renderer.drawMesh(*ground.mesh, ground.getModelMat(), ground.color);
        renderer.drawMesh(*wall.mesh, wall.getModelMat(), wall.color);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::cout << "dt = " << dt << '\n';
    }
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
