#pragma once
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"

class Renderer
{
public:
	Renderer(Shader& shader, Camera& camera);
	void clear(glm::vec4 clearedColor = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f));
	void drawMesh(Mesh& mesh, const glm::mat4 model, const glm::vec4& color);
	void drawLine(glm::vec2 start, glm::vec2 end, const glm::vec4& color);
	void drawMeshOutline(Mesh& mesh, const glm::mat4 model, const glm::vec4& color);
private:
	Shader& shader;
	Camera& camera;
};