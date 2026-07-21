#version 460 core
layout(location = 0) in vec3 aPos; //local position

uniform mat4 model; // transformation (world position)
uniform mat4 view; // pose of camera
uniform mat4 projection; //perspective
uniform vec4 color;

out vec4 vertexColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = color;
}