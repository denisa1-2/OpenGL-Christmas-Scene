#version 410 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;        
uniform mat4 lightSpaceTrMatrix;   

out vec3 vNormalEye;
out vec3 vPosEye;
out vec2 vTexCoords;
out vec4 vPosLightSpace;

void main()
{
    vec4 worldPos = model * vec4(inPosition, 1.0);
    vec4 posEye4 = view * worldPos;

    vPosEye = posEye4.xyz;
    vNormalEye = normalize(normalMatrix * inNormal);
    vTexCoords = inTexCoords;

    vPosLightSpace = lightSpaceTrMatrix * worldPos;

    gl_Position = projection * posEye4;
}
