//
//  main.cpp
//  OpenGL - Project (camera + scene + skybox)
//  Shadow mapping removed (no depth FBO / no shadowMap / no lightSpaceTrMatrix)
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

int glWindowWidth = 1920;
int glWindowHeight = 1080;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

glm::mat4 model;
GLuint modelLoc;

glm::mat4 view;
GLuint viewLoc;

glm::mat4 projection;
GLuint projectionLoc;

glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::mat4 lightRotation;
glm::vec3 lightDir;
GLuint lightDirLoc;

glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
    glm::vec3(0.0f, 2.0f, 5.5f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
);

float cameraSpeed = 0.01f;
bool pressedKeys[1024];

float angleY = 0.0f;
GLfloat lightAngle = 0.0f;

gps::Model3D scena;
gps::Model3D gloveA;
gps::Model3D gloveB;

float gloveAngleA = 0.0f;
float gloveAngleB = 0.0f;

bool glovesAnimEnabled = true;

gps::Shader myCustomShader;
gps::Shader skyboxShader;

gps::SkyBox mySkyBox_zi;
gps::SkyBox mySkyBox_n;
bool isNight = false;

bool firstMouse = true;
float lastX = 400.0f;
float lastY = 300.0f;

float yaw = -90.0f;
float pitch = 0.0f;
float mouseSensitivity = 0.1f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cameraSpeedBase = 8.0f;

// animatie camera
bool introPlaying = true;
float introStartTime = 0.0f;
float introDuration = 12.0f;

glm::vec3 introCenter(0.0f, 10.325f, -0.255f);

float introRadiusStart = 95.0f;
float introRadiusEnd = 35.0f;
float introHeightStart = 55.0f;
float introHeightEnd = 8.0f;
float introRotations = 1.15f;
float introEasePow = 2.0f;

glm::vec3 finalCamPos(28.0f, 7.0f, 38.0f);
glm::vec3 finalTarget = introCenter;

glm::vec3 savedFinalPos(0.0f);
bool hasSavedFinal = false;

// render modes
enum RenderMode { RM_SOLID = 0, RM_WIREFRAME = 1, RM_POINTS = 2, RM_SMOOTH = 3 };
RenderMode renderMode = RM_SMOOTH;
GLuint renderModeLoc;

GLuint ambientStrengthLoc;

bool fogEnabled = false;
GLuint fogEnabledLoc;
GLuint fogColorLoc;
GLuint fogDensityLoc;

// coliziuni plan
float groundY = 0.0f;
float minX = -50.0f, maxX = 50.0f;
float minZ = -50.0f, maxZ = 50.0f;
float cameraHeight = 1.7f;

// snow
bool snowEnabled = false;

struct SnowParticle {
    glm::vec3 pos;
    float speed;
};

std::vector<SnowParticle> snowParticles;

GLuint snowVAO = 0;
GLuint snowVBO = 0;

gps::Shader snowShader;

int snowCount = 6000;
float snowAreaHalfSize = 50.0f;
float snowTopY = 45.0f;
float snowBottomY = -2.0f;

// umbre
gps::Shader depthMapShader;

unsigned int SHADOW_WIDTH = 2048;
unsigned int SHADOW_HEIGHT = 2048;

GLuint shadowMapFBO = 0;
GLuint depthMapTexture = 0;

// lumina spot
glm::vec3 lanternPosWorld[3] = {
    glm::vec3(23.700060f, 7.065710f,  6.688493f),
    glm::vec3(-22.067512f, 6.471938f, 5.183189f),
    glm::vec3(8.862154f,  7.537826f, -26.788820f)
};

glm::vec3 lanternDirWorld = glm::vec3(0.0f, -1.0f, 0.0f);

// translatie + scalare scena
glm::vec3 sceneTranslate(0.0f);
float sceneScale = 1.0f;

// forward declarations
static glm::mat4 getSceneModelMatrix();
static float rand01();
static void resetParticle(int i);

GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")\n";
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);

    glfwGetFramebufferSize(window, &retina_width, &retina_height);
    glViewport(0, 0, retina_width, retina_height);

    projection = glm::perspective(glm::radians(45.0f),
        (float)retina_width / (float)retina_height, 0.1f, 1000.0f);

    myCustomShader.useShaderProgram();
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_O) renderMode = RM_SOLID;
        if (key == GLFW_KEY_F) renderMode = RM_WIREFRAME;
        if (key == GLFW_KEY_P) renderMode = RM_POINTS;
        if (key == GLFW_KEY_M) renderMode = RM_SMOOTH;
        if (key == GLFW_KEY_N) isNight = !isNight;
        if (key == GLFW_KEY_C) fogEnabled = !fogEnabled;
        if (key == GLFW_KEY_Z) snowEnabled = !snowEnabled;

        // reset scena
        if (key == GLFW_KEY_R) { sceneTranslate = glm::vec3(0.0f); sceneScale = 1.0f; }
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            pressedKeys[key] = true;
        else if (action == GLFW_RELEASE)
            pressedKeys[key] = false;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (introPlaying) {
        return;
    }

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;

    lastX = (float)xpos;
    lastY = (float)ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    myCamera.rotate(pitch, yaw);
}

void processMovement()
{
    if (pressedKeys[GLFW_KEY_Q]) angleY -= 60.0f * deltaTime;
    if (pressedKeys[GLFW_KEY_E]) angleY += 60.0f * deltaTime;

    if (pressedKeys[GLFW_KEY_J]) lightAngle -= 60.0f * deltaTime;
    if (pressedKeys[GLFW_KEY_L]) lightAngle += 60.0f * deltaTime;

    if (pressedKeys[GLFW_KEY_W]) myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
    if (pressedKeys[GLFW_KEY_S]) myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
    if (pressedKeys[GLFW_KEY_A]) myCamera.move(gps::MOVE_LEFT, cameraSpeed);
    if (pressedKeys[GLFW_KEY_D]) myCamera.move(gps::MOVE_RIGHT, cameraSpeed);

    // TRANSLATIE scena (sagetile)
    float tSpeed = 12.0f * deltaTime;
    if (pressedKeys[GLFW_KEY_UP])    sceneTranslate.z += tSpeed;
    if (pressedKeys[GLFW_KEY_DOWN])  sceneTranslate.z -= tSpeed;
    if (pressedKeys[GLFW_KEY_LEFT])  sceneTranslate.x += tSpeed;
    if (pressedKeys[GLFW_KEY_RIGHT]) sceneTranslate.x -= tSpeed;

    // SCALARE scena (+ / -)
    float sSpeed = 0.9f * deltaTime;
    if (pressedKeys[GLFW_KEY_KP_ADD] || pressedKeys[GLFW_KEY_EQUAL])  sceneScale += sSpeed;
    if (pressedKeys[GLFW_KEY_KP_SUBTRACT] || pressedKeys[GLFW_KEY_MINUS]) sceneScale -= sSpeed;

    if (sceneScale < 0.2f) sceneScale = 0.2f;
    if (sceneScale > 5.0f) sceneScale = 5.0f;
}

bool initOpenGLWindow()
{
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Project", NULL, NULL);
    if (!glWindow) {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);

    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);
    glfwSwapInterval(1);

#if not defined (__APPLE__)
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    lastX = glWindowWidth / 2.0f;
    lastY = glWindowHeight / 2.0f;

    return true;
}

void initOpenGLState()
{
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
    scena.LoadModel("objects/scenaProiect.obj", "textures/");
    gloveA.LoadModel("objects/left_glove.obj", "textures/");
    gloveB.LoadModel("objects/right_glove.obj", "textures/");
}

void initSkybox() {
    std::vector<const GLchar*> faces1;
    faces1.push_back("skybox_zi/right.jpg");
    faces1.push_back("skybox_zi/left.jpg");
    faces1.push_back("skybox_zi/top.jpg");
    faces1.push_back("skybox_zi/bottom.jpg");
    faces1.push_back("skybox_zi/back.jpg");
    faces1.push_back("skybox_zi/front.jpg");
    mySkyBox_zi.Load(faces1);

    std::vector<const GLchar*> faces2;
    faces2.push_back("skybox_noapte/right.jpg");
    faces2.push_back("skybox_noapte/left.jpg");
    faces2.push_back("skybox_noapte/top.jpg");
    faces2.push_back("skybox_noapte/bottom.jpg");
    faces2.push_back("skybox_noapte/back.jpg");
    faces2.push_back("skybox_noapte/front.jpg");
    mySkyBox_n.Load(faces2);
}

void initShaders() {
    myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
    myCustomShader.useShaderProgram();

    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();

    snowShader.loadShader("shaders/snow.vert", "shaders/snow.frag");
    snowShader.useShaderProgram();

    depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
    depthMapShader.useShaderProgram();
}

void initUniforms() {
    myCustomShader.useShaderProgram();

    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotCutOff"),
        cos(glm::radians(15.0f)));
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotOuterCutOff"),
        cos(glm::radians(25.0f)));

    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotConstant"), 1.0f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotLinear"), 0.09f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotQuadratic"), 0.032f);

    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotIntensity"), 2.2f);

    glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotColor[0]"), 1.0f, 0.85f, 0.55f);
    glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotColor[1]"), 1.0f, 0.85f, 0.55f);
    glUniform3f(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotColor[2]"), 1.0f, 0.85f, 0.55f);

    fogEnabledLoc = glGetUniformLocation(myCustomShader.shaderProgram, "uFogEnabled");
    glUniform1i(fogEnabledLoc, (int)fogEnabled);

    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uEmissiveStrength"), 2.8f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uEmissiveRadius"), 1.2f);

    fogColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogColor");
    fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
    glUniform1f(fogDensityLoc, 0.012f);

    ambientStrengthLoc = glGetUniformLocation(myCustomShader.shaderProgram, "ambientStrength");
    glUniform1f(ambientStrengthLoc, 0.2f);

    renderModeLoc = glGetUniformLocation(myCustomShader.shaderProgram, "uRenderMode");
    glUniform1i(renderModeLoc, (int)renderMode);

    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    projection = glm::perspective(glm::radians(45.0f),
        (float)retina_width / (float)retina_height, 0.1f, 1000.0f);

    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    lightDir = glm::normalize(glm::vec3(-0.3f, 1.0f, -0.2f));
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1,
        glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    lightColor = glm::vec3(1.0f, 0.98f, 0.92f);
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

// umbre
void initFBO() {
    glGenFramebuffers(1, &shadowMapFBO);

    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
    glm::mat4 lightRot = glm::rotate(glm::mat4(1.0f),
        glm::radians(lightAngle),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 lightDirWorld = glm::normalize(glm::mat3(lightRot) * lightDir);

    glm::vec3 center = glm::vec3(0.0f, 10.325f, -0.255f);

    float lightDistance = 160.0f;
    glm::vec3 lightPos = center + lightDirWorld * lightDistance;

    glm::mat4 lightView = glm::lookAt(lightPos, center, glm::vec3(0, 1, 0));

    float orthoHalf = 90.0f;
    float near_plane = 1.0f;
    float far_plane = 350.0f;

    glm::mat4 lightProjection = glm::ortho(-orthoHalf, orthoHalf,
        -orthoHalf, orthoHalf,
        near_plane, far_plane);

    return lightProjection * lightView;
}

void drawObjects(gps::Shader shader) {
    shader.useShaderProgram();

    model = getSceneModelMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"),
        1, GL_FALSE, glm::value_ptr(model));

    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    scena.Draw(shader);
}

// ninsoare
static float rand01() {
    return (float)rand() / (float)RAND_MAX;
}

static void resetParticle(int i) {
    float x = (rand01() * 2.0f - 1.0f) * snowAreaHalfSize;
    float z = (rand01() * 2.0f - 1.0f) * snowAreaHalfSize;
    float y = snowTopY + rand01() * 5.0f;

    snowParticles[i].pos = glm::vec3(x, y, z);
    snowParticles[i].speed = 2.5f + rand01() * 3.5f;
}

void initSnow() {
    snowParticles.clear();
    snowParticles.resize(snowCount);

    for (int i = 0; i < snowCount; i++) {
        resetParticle(i);
        snowParticles[i].pos.y = snowBottomY + rand01() * (snowTopY - snowBottomY);
    }

    glGenVertexArrays(1, &snowVAO);
    glGenBuffers(1, &snowVBO);

    glBindVertexArray(snowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, snowVBO);

    glBufferData(GL_ARRAY_BUFFER, snowCount * sizeof(glm::vec3), NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void updateSnow(float dt) {
    if (!snowEnabled) return;

    for (int i = 0; i < snowCount; i++) {
        snowParticles[i].pos.y -= snowParticles[i].speed * dt;

        snowParticles[i].pos.x += (rand01() - 0.5f) * 0.12f;
        snowParticles[i].pos.z += (rand01() - 0.5f) * 0.12f;

        if (snowParticles[i].pos.y <= snowBottomY) {
            resetParticle(i);
        }
    }

    std::vector<glm::vec3> positions(snowCount);
    for (int i = 0; i < snowCount; i++) positions[i] = snowParticles[i].pos;

    glBindBuffer(GL_ARRAY_BUFFER, snowVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, snowCount * sizeof(glm::vec3), positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void drawSnow() {
    if (!snowEnabled) return;

    snowShader.useShaderProgram();

    GLuint viewLocSnow = glGetUniformLocation(snowShader.shaderProgram, "view");
    GLuint projLocSnow = glGetUniformLocation(snowShader.shaderProgram, "projection");

    glUniformMatrix4fv(viewLocSnow, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLocSnow, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(snowVAO);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, snowCount);

    glBindVertexArray(0);
}

// coliziune plan
void clampCameraToGroundAndBounds() {
    glm::vec3 camWorld = myCamera.getPosition();

    glm::mat4 invScene = glm::inverse(getSceneModelMatrix());
    glm::vec3 camScene = glm::vec3(invScene * glm::vec4(camWorld, 1.0f));

    float minY = groundY + cameraHeight;
    if (camScene.y < minY) camScene.y = minY;

    if (camScene.x < minX) camScene.x = minX;
    if (camScene.x > maxX) camScene.x = maxX;
    if (camScene.z < minZ) camScene.z = minZ;
    if (camScene.z > maxZ) camScene.z = maxZ;

    glm::vec3 camWorldClamped = glm::vec3(getSceneModelMatrix() * glm::vec4(camScene, 1.0f));
    myCamera.setPosition(camWorldClamped);
}


// render modes
void applyRenderMode() {
    switch (renderMode) {
    case RM_WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
        break;
    case RM_POINTS:
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glDisable(GL_CULL_FACE);
        glPointSize(3.0f);
        break;
    case RM_SOLID:
    case RM_SMOOTH:
    default:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        break;
    }

    myCustomShader.useShaderProgram();
    glUniform1i(renderModeLoc, (int)renderMode);
}

// animatie obiect
static void drawModelWithMatrix(gps::Model3D& model3D, gps::Shader& shader, glm::mat4 modelMatrix) {
    shader.useShaderProgram();

    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"),
        1, GL_FALSE, glm::value_ptr(modelMatrix));

    if (shader.shaderProgram == myCustomShader.shaderProgram) {
        glm::mat3 nm = glm::mat3(glm::inverseTranspose(view * modelMatrix));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(nm));
    }

    model3D.Draw(shader);
}

static glm::mat4 getSceneModelMatrix()
{
    glm::mat4 m(1.0f);
    m = glm::translate(m, sceneTranslate);
    m = glm::rotate(m, glm::radians(angleY), glm::vec3(0, 1, 0));
    m = glm::scale(m, glm::vec3(sceneScale));
    return m;
}

void renderScene()
{
    glm::mat4 lightSpaceTrMatrix = computeLightSpaceTrMatrix();

    // PASS 1: depth map
    glViewport(0, 0, (int)SHADOW_WIDTH, (int)SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    glDisable(GL_CULL_FACE);

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(
        glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix)
    );

    {
        glm::mat4 modelDepth = getSceneModelMatrix();
        glUniformMatrix4fv(
            glGetUniformLocation(depthMapShader.shaderProgram, "model"),
            1, GL_FALSE, glm::value_ptr(modelDepth)
        );
        scena.Draw(depthMapShader);
    }

    {
        glm::vec3 waveAxis(0.0f, 0.0f, 1.0f);

        glm::mat4 mA = getSceneModelMatrix();
        mA = glm::rotate(mA, glm::radians(gloveAngleA), waveAxis);
        drawModelWithMatrix(gloveA, depthMapShader, mA);

        glm::mat4 mB = getSceneModelMatrix();
        mB = glm::rotate(mB, glm::radians(gloveAngleB), waveAxis);
        drawModelWithMatrix(gloveB, depthMapShader, mB);
    }

    glDisable(GL_POLYGON_OFFSET_FILL);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // PASS 2: scena finala
    glViewport(0, 0, retina_width, retina_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myCustomShader.useShaderProgram();

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(
        glGetUniformLocation(myCustomShader.shaderProgram, "view"),
        1, GL_FALSE, glm::value_ptr(view)
    );
    glUniformMatrix4fv(
        glGetUniformLocation(myCustomShader.shaderProgram, "projection"),
        1, GL_FALSE, glm::value_ptr(projection)
    );

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

    glUniformMatrix4fv(
        glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
        1, GL_FALSE, glm::value_ptr(lightSpaceTrMatrix)
    );

    float sh = isNight ? 0.0f : 1.35f;
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "shadowStrength"), sh);

    lightRotation = glm::rotate(glm::mat4(1.0f),
        glm::radians(lightAngle),
        glm::vec3(0.0f, 1.0f, 0.0f));

    glUniform3fv(
        lightDirLoc,
        1,
        glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir)
    );

    float amb;
    glm::vec3 fogCol;
    float fogD;

    if (isNight) {
        lightColor = glm::vec3(0.28f, 0.30f, 0.34f);
        amb = 0.08f;
        fogCol = glm::vec3(0.35f, 0.38f, 0.42f);
        fogD = 0.010f;
    }
    else {
        lightColor = glm::vec3(1.0f, 0.98f, 0.92f);
        amb = 0.25f;
        fogCol = glm::vec3(0.70f, 0.72f, 0.75f);
        fogD = 0.010f;
    }

    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform1f(ambientStrengthLoc, amb);

    glUniform1i(fogEnabledLoc, (int)fogEnabled);
    glUniform3fv(fogColorLoc, 1, glm::value_ptr(fogCol));
    glUniform1f(fogDensityLoc, fogD);

    int numSpots = isNight ? 3 : 0;
    glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "uNumSpots"), numSpots);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "uEmissiveStrength"), isNight ? 2.8f : 0.0f);

    if (numSpots > 0) {
        glm::mat4 sceneM = getSceneModelMatrix();
        glm::mat4 rotOnly = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0, 1, 0));

        glm::vec3 dirWorldRot = glm::normalize(glm::mat3(rotOnly) * lanternDirWorld);
        glm::vec3 dirEye = glm::normalize(glm::mat3(view) * dirWorldRot);

        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotDirEye[0]"), 1, glm::value_ptr(dirEye));
        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotDirEye[1]"), 1, glm::value_ptr(dirEye));
        glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "uSpotDirEye[2]"), 1, glm::value_ptr(dirEye));

        for (int i = 0; i < 3; i++) {
            glm::vec3 posWorldTransformed = glm::vec3(sceneM * glm::vec4(lanternPosWorld[i], 1.0f));
            glm::vec3 posEye = glm::vec3(view * glm::vec4(posWorldTransformed, 1.0f));
            std::string uname = "uSpotPosEye[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, uname.c_str()), 1, glm::value_ptr(posEye));
        }
    }

    applyRenderMode();

    drawObjects(myCustomShader);

    {
        glm::vec3 waveAxis(0.0f, 0.0f, 1.0f);

        glm::mat4 mA = getSceneModelMatrix();
        mA = glm::rotate(mA, glm::radians(gloveAngleA), waveAxis);
        drawModelWithMatrix(gloveA, myCustomShader, mA);

        glm::mat4 mB = getSceneModelMatrix();
        mB = glm::rotate(mB, glm::radians(gloveAngleB), waveAxis);
        drawModelWithMatrix(gloveB, myCustomShader, mB);
    }

    drawSnow();

    skyboxShader.useShaderProgram();
    if (isNight)
        mySkyBox_n.Draw(skyboxShader, view, projection);
    else
        mySkyBox_zi.Draw(skyboxShader, view, projection);
}

void cleanup() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (snowVBO) glDeleteBuffers(1, &snowVBO);
    if (snowVAO) glDeleteVertexArrays(1, &snowVAO);
    glfwDestroyWindow(glWindow);
    glfwTerminate();
}

// functie animatie camera
static void startIntro() {
    introStartTime = (float)glfwGetTime();
    introPlaying = true;
    firstMouse = true;

    glm::vec3 initialPos = introCenter + glm::vec3(0.0f, introHeightStart, introRadiusStart);
    myCamera.setPosition(initialPos);
    myCamera.lookAt(introCenter);
}

static void updateIntro()
{
    float now = (float)glfwGetTime();
    float t = (now - introStartTime) / introDuration;

    t = glm::clamp(t, 0.0f, 1.0f);
    float s = t * t * (3.0f - 2.0f * t);

    float angle = glm::radians(360.0f * introRotations * s);

    float radius = glm::mix(introRadiusStart, introRadiusEnd, s);
    float height = glm::mix(introHeightStart, introHeightEnd, s);

    glm::vec3 camPos;
    camPos.x = introCenter.x + radius * cosf(angle);
    camPos.z = introCenter.z + radius * sinf(angle);
    camPos.y = height;

    myCamera.setPosition(camPos);
    myCamera.lookAt(introCenter);

    if (t >= 1.0f)
    {
        introPlaying = false;

        glm::vec3 finalPos = hasSavedFinal ? savedFinalPos : finalCamPos;

        myCamera.setPosition(finalPos);
        myCamera.lookAt(introCenter);

        glm::vec3 dir = glm::normalize(introCenter - finalPos);

        pitch = glm::degrees(asinf(dir.y));
        yaw = glm::degrees(atan2f(dir.z, dir.x));

        myCamera.rotate(pitch, yaw);

        lastX = glWindowWidth / 2.0f;
        lastY = glWindowHeight / 2.0f;
        firstMouse = true;
    }
}

int main(int argc, const char* argv[]) {

    if (!initOpenGLWindow()) {
        glfwTerminate();
        return 1;
    }

    initOpenGLState();
    initObjects();
    initShaders();
    initUniforms();
    initFBO();
    initSkybox();

    srand((unsigned int)time(NULL));
    initSnow();

    startIntro();

    while (!glfwWindowShouldClose(glWindow)) {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        cameraSpeed = cameraSpeedBase * deltaTime;

        if (introPlaying) {
            updateIntro();
        }
        else {
            processMovement();
            clampCameraToGroundAndBounds();
        }

        updateSnow(deltaTime);

        if (glovesAnimEnabled) {
            float t = (float)glfwGetTime();
            gloveAngleA = 0.5f * sinf(t * 2.0f);
            gloveAngleB = 0.5f * sinf(t * 2.0f + 1.0f);
        }

        view = myCamera.getViewMatrix();
        renderScene();

        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }

    cleanup();
    return 0;
}
