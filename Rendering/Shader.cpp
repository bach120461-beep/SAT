#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include "Shader.h"

Shader::Shader(const std::string& vertPath, const std::string& fragPath)
{
	std::string vertSource = readFile(vertPath);
	std::string fragSource = readFile(fragPath);

	compile(vertSource, fragSource);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::Use()
{
	glUseProgram(ID);
}

void Shader::compile(const std::string& vertSource, const std::string& fragSource)
{
	ID = glCreateProgram();
	const char* vertCode = vertSource.c_str();
	const char* fragCode = fragSource.c_str();

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertCode, nullptr);
	glCompileShader(vertexShader);
	checkCompileErrors(vertexShader, "VERTEX");

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragCode, nullptr);
	glCompileShader(fragmentShader);
	checkCompileErrors(fragmentShader, "FRAGMENT");

	//Attach to shaderprogram
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	
	// Add these — free GPU memory
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

std::string Shader::readFile(const std::string& filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open())
	{
		std::cerr << "Failed to open shader: " << filepath << "\n";
		return "";
	}
	std::cerr << "Loaded shader: " << filepath << "\n";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

//Uniforms
void Shader::setUniformMatrix4(const std::string& name, const glm::mat4& value)
{
	//if Uniform already exist, skip saving it
	if (matrixCache.count(name) > 0 && matrixCache[name] == value) return;

	Use();
	//Update Uniform in GPU
	int loc = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));

	//Cache
	matrixCache[name] = value;
}
void Shader::setUniform(const std::string& name, const glm::vec4& value)
{
	//if Uniform already exist, skip saving it
	if (vec4Cache.count(name) > 0 && vec4Cache[name] == value) return;

	Use();
	//Update Uniform in GPU
	int loc = glGetUniformLocation(ID, name.c_str());
	glUniform4f(loc, value.x, value.y, value.z, value.w);

	//Cache
	vec4Cache[name] = value;
}
void Shader::setUniform(const std::string& name, float value)
{
	//if Uniform already exist, skip saving it
	if (floatCache.count(name) > 0 && floatCache[name] == value) return;

	Use();
	//Update Uniform in GPU
	int loc = glGetUniformLocation(ID, name.c_str());
	glUniform1f(loc, value);

	floatCache[name] = value;
}

