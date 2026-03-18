#version 410 core

layout(location = 0) in vec3 inPosition;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(inPosition, 1.0);

    gl_PointSize = 3.0
        + fract(sin(inPosition.x * 12.9898 + inPosition.z * 78.233) * 43758.5453) * 4.0;
}
