#pragma once
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

class Shader
{
private:
	unsigned int ID;
	std::unordered_map<std::string, glm::mat4> matrixCache;
	std::unordered_map<std::string, glm::vec4> vec4Cache;
	std::unordered_map<std::string, float> floatCache;
public:
	Shader(const std::string& vertPath, const std::string& fragPath);
	void Use();
	//Uniforms
	void setUniformMatrix4(const std::string& name, const glm::mat4& value);
	void setUniform(const std::string& name, const glm::vec4& value);
	void setUniform(const std::string& name, float value);
	~Shader();
private:
	std::string readFile(const std::string& filepath);
	void compile(const std::string& vertSource, const std::string& fragSource);
	void checkCompileErrors(unsigned int shader, std::string type);
};