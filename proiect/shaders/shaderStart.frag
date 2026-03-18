#version 410 core

in vec3 vNormalEye;
in vec3 vPosEye;
in vec2 vTexCoords;
in vec4 vPosLightSpace;

out vec4 fColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float ambientStrength;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform float shadowStrength;

uniform int uFogEnabled;
uniform vec3 fogColor;
uniform float fogDensity;

#define MAX_SPOTS 3

uniform int uNumSpots;
uniform vec3 uSpotPosEye[MAX_SPOTS];
uniform vec3 uSpotDirEye[MAX_SPOTS];
uniform vec3 uSpotColor[MAX_SPOTS];

uniform float uSpotCutOff;
uniform float uSpotOuterCutOff;

uniform float uSpotConstant;
uniform float uSpotLinear;
uniform float uSpotQuadratic;

uniform float uSpotIntensity;

uniform float uEmissiveStrength;
uniform float uEmissiveRadius;

float computeShadow(vec4 fragPosLightSpace, vec3 normalEye, vec3 lightDirEye)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    float ndotl = max(dot(normalize(normalEye), normalize(lightDirEye)), 0.0);

    float bias = max(0.0025 * (1.0 - ndotl), 0.0008);

    float shadow = 0.0;

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(float(x), float(y)) * texelSize).r;

            if (currentDepth - bias > pcfDepth)
                shadow += 1.0;
        }
    }

    shadow = shadow / 9.0;
    return shadow;
}

float computeFogFactor(vec3 posEye)
{
    float dist = length(posEye);
    float fogFactor = exp(-fogDensity * dist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    return fogFactor;
}

vec3 computeSpotContribution(vec3 posEye, vec3 normalEye, vec3 baseColor)
{
    vec3 result = vec3(0.0);

    vec3 N = normalize(normalEye);
    vec3 V = normalize(-posEye);

    for (int i = 0; i < uNumSpots; i++)
    {
        vec3 Lvec = uSpotPosEye[i] - posEye;
        float dist = length(Lvec);
        vec3 L = Lvec / max(dist, 1e-4);

        float theta = dot(L, normalize(-uSpotDirEye[i]));
        float eps = max(uSpotCutOff - uSpotOuterCutOff, 1e-4);
        float spot = clamp((theta - uSpotOuterCutOff) / eps, 0.0, 1.0);

        float att = 1.0 / (uSpotConstant + uSpotLinear * dist + uSpotQuadratic * dist * dist);

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = diff * baseColor * uSpotColor[i];

        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(V, R), 0.0), 32.0);
        vec3 specular = spec * uSpotColor[i] * 0.35;

        result += (diffuse + specular) * att * spot;
    }

    return result * uSpotIntensity;
}

float computeLanternEmissiveMask(vec3 posEye)
{
    float mask = 0.0;

    for (int i = 0; i < uNumSpots; i++)
    {
        float d = length(posEye - uSpotPosEye[i]);

        float m = 1.0 - smoothstep(uEmissiveRadius * 0.7, uEmissiveRadius, d);

        vec3 upEye = vec3(0.0, 1.0, 0.0);
        float dy = abs(dot(posEye - uSpotPosEye[i], upEye));
        float h = 1.0 - smoothstep(0.35, 0.65, dy);

        mask = max(mask, m * h);
    }

    return clamp(mask, 0.0, 1.0);
}


void main()
{
    vec3 baseColor = texture(diffuseTexture, vTexCoords).rgb;

    vec3 N = normalize(vNormalEye);
    vec3 L = normalize(lightDir);

    vec3 ambient = ambientStrength * baseColor * lightColor;

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * baseColor * lightColor;

    vec3 V = normalize(-vPosEye);
    vec3 R = reflect(-L, N);

    float spec = pow(max(dot(V, R), 0.0), 32.0);
    vec3 specular = spec * lightColor * 0.25;

    float shadow = computeShadow(vPosLightSpace, N, L);
    float s = clamp(shadow * shadowStrength, 0.0, 1.0);

    vec3 colorLit = ambient + (1.0 - s) * (diffuse + specular);

    colorLit += computeSpotContribution(vPosEye, N, baseColor);
    
    float eMask = computeLanternEmissiveMask(vPosEye);
    colorLit += baseColor * (uEmissiveStrength * eMask);

    if (uFogEnabled == 1)
    {
        float fogFactor = computeFogFactor(vPosEye);
        colorLit = mix(fogColor, colorLit, fogFactor);
    }

    fColor = vec4(colorLit, 1.0);
}
