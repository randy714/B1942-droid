#include "gl_renderer.h"
#include <android/log.h>

#define LOG_TAG "B1942_GLRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* VERTEX_SHADER_SRC =
    "attribute vec4 aPosition;\n"
    "attribute vec2 aTexCoord;\n"
    "varying vec2 vTexCoord;\n"
    "uniform mat4 uMVPMatrix;\n"
    "void main() {\n"
    "    gl_Position = uMVPMatrix * aPosition;\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

const char* FRAGMENT_SHADER_SRC =
    "precision mediump float;\n"
    "varying vec2 vTexCoord;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(0.08, 0.07, 0.06, 1.0);\n" // Dark Espresso Clean Viewport Background
    "}\n";

GLRenderer::GLRenderer() 
    : m_width(1280), m_height(720), m_program(0), m_camYaw(0.0f), m_camPitch(0.0f) {
    for (int i = 0; i < 10; i++) m_btnStates[i] = false;
}

GLRenderer::~GLRenderer() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

void GLRenderer::init(int width, int height) {
    m_width = width;
    m_height = height;

    glViewport(0, 0, width, height);
    glClearColor(0.08f, 0.07f, 0.06f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &VERTEX_SHADER_SRC, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &FRAGMENT_SHADER_SRC, NULL);
    glCompileShader(fs);

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);

    m_aPositionHandle = glGetAttribLocation(m_program, "aPosition");
    m_uMVPMatrixHandle = glGetUniformLocation(m_program, "uMVPMatrix");

    LOGI("Clean Viewport Engine Pipeline Active (%dx%d)", width, height);
}

void GLRenderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void GLRenderer::setCameraLook(float yawDelta, float pitchDelta) {
    m_camYaw += yawDelta * 0.005f;
    m_camPitch += pitchDelta * 0.005f;
    if (m_camPitch > 1.57f) m_camPitch = 1.57f;
    if (m_camPitch < -1.57f) m_camPitch = -1.57f;
}

void GLRenderer::updateMoveState(int buttonId, bool pressed) {
    if (buttonId >= 0 && buttonId < 10) {
        m_btnStates[buttonId] = pressed;
    }
}

void GLRenderer::renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (m_program) {
        glUseProgram(m_program);
    }
}
