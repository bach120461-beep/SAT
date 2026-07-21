#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer(Shader& shader, Camera& camera)
    : shader(shader), camera(camera)
{}

void Renderer::clear(glm::vec4 clearedColor)
{
    glClearColor(clearedColor.r, clearedColor.g, clearedColor.b, clearedColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawMesh(Mesh& mesh, const glm::mat4 model, const glm::vec4& color)
{
    shader.Use();
    // Camera matrices (cached — only resent if camera moved)
    shader.setUniformMatrix4("view", camera.getViewMat());
    shader.setUniformMatrix4("projection", camera.OrthoProjectionMat());

    // Per-object uniforms
    shader.setUniformMatrix4("model", model);
    shader.setUniform("color", color);

    mesh.Bind();
    mesh.Render();
}

void Renderer::drawLine(glm::vec2 start, glm::vec2 end, const glm::vec4& color)
{
    std::vector<float> verts = {
        start.x, start.y, 0.0f,
        end.x, end.y, 0.0f
    };
    std::vector<unsigned int> index{ 0, 1 };

    Mesh lineMesh(verts, index);

    shader.Use();
    shader.setUniformMatrix4("view", camera.getViewMat());
    shader.setUniformMatrix4("projection", camera.OrthoProjectionMat());
    shader.setUniformMatrix4("model", glm::mat4(1.0f));
    shader.setUniform("color", color);

    lineMesh.Bind();
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
}