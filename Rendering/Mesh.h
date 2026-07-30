#pragma once
#include<vector>

class Mesh
{
private:
	unsigned int VAO, VBO, IBO;
	int indexCount;
public:
	std::vector <float> vertices;
	std::vector <unsigned int> indices;
	Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
	int getIndex();
	void Bind();
	void UnBind();
	void Render();
	void RenderOutline();
	~Mesh();
};