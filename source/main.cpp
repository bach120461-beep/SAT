#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Rendering/Mesh.h"
#include "Rendering/Shader.h"
#include "Rendering/Renderer.h"
#include "Rendering/Camera.h"
#include "Physic.h"
#include "Shape.h"
#include <vector>

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
    auto carMesh = MeshFactory::createCar(80.0f, 50.0f, 10.0f);
    Shader basicShader("Resources/Shader.vert", "Resources/Shader.frag");
    Renderer renderer(basicShader, camera);
    SceneObject square1(carMesh, glm::vec3(-300.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec4(0.0f, 0.0f, 0.0f,1.0f));
    SceneObject square2(carMesh, glm::vec3(300.0f, 0.0f, 0.0f), glm::vec3(-100.0f, 0.0f, 0.0f), 1.0f, glm::vec4(0.2f, 0.3f, 0.1f, 1.0f));

    camera.zoom(2);
    std::cout << square1.calculateBoundingRadius() << '\n';
    std::cout << square2.calculateBoundingRadius() << '\n';
    std::cout
        << "Before collision\n"
        << "A vel = " << square1.physics.velocity.x
        << "\nB vel = " << square2.physics.velocity.x
        << '\n';
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
            std::cout
                << "Normal = "
                << info.normal.x << ", "
                << info.normal.y << '\n';
        if (info.hit) {
            std::cout
                << "After collision\n"
                << "A vel = " << square1.physics.velocity.x
                << "\nB vel = " << square2.physics.velocity.x
                << '\n';
        }
        if (square1.physics.position.x > 400.0f || square1.physics.position.x < -400.0f)
            square1.physics.velocity.x = square1.physics.velocity.x * (-1);
        if (square2.physics.position.x < -400.0f || square2.physics.position.x >400.0f)
            square2.physics.velocity.x = square2.physics.velocity.x * (-1);
        renderer.clear(glm::vec4(1.0f,1.0f,1.0f,1.0f));
        
        
        renderer.drawMesh(*square1.mesh, square1.getModelMat(),square1.color);
        renderer.drawMesh(*square2.mesh, square2.getModelMat(), square2.color);
        
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
