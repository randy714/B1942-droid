#include "gl_renderer.h"
#include <android/log.h>

#define LOG_TAG "B1942_GLRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* VERTEX_SHADER_SRC =
    "attribute vec4 aPosition;\n"
    "attribute vec4 aColor;\n"
    "varying vec4 vColor;\n"
    "uniform mat4 uMVPMatrix;\n"
    "void main() {\n"
    "    gl_Position = uMVPMatrix * aPosition;\n"
    "    vColor = aColor;\n"
    "}\n";

const char* FRAGMENT_SHADER_SRC =
    "precision mediump float;\n"
    "varying vec4 vColor;\n"
    "void main() {\n"
    "    gl_FragColor = vColor;\n"
    "}\n";

GLRenderer::GLRenderer() 
    : m_width(1280), m_height(720), m_program(0), m_camYaw(0.0f), m_camPitch(0.0f) {
    for (int i = 0; i < 10; i++) m_btnStates[i] = false;
}

GLRenderer::~GLRenderer() {
    if (m_program) glDeleteProgram(m_program);
}

GLuint GLRenderer::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint GLRenderer::createProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    return prog;
}

void GLRenderer::init(int width, int height) {
    m_width = width;
    m_height = height;

    glViewport(0, 0, width, height);
    glClearColor(0.05f, 0.04f, 0.04f, 1.0f); // Dark Espresso Tint
    glEnable(GL_DEPTH_TEST);

    m_program = createProgram(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
    m_aPositionHandle = glGetAttribLocation(m_program, "aPosition");
    m_aColorHandle = glGetAttribLocation(m_program, "aColor");
    m_uMVPMatrixHandle = glGetUniformLocation(m_program, "uMVPMatrix");
    
    LOGI("Native OpenGL ES 2.0 Engine Pipeline active (%dx%d)", width, height);
}

void GLRenderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void GLRenderer::setCameraLook(float yawDelta, float pitchDelta) {
    m_camYaw += yawDelta * 0.005f;
    m_camPitch += pitchDelta * 0.005f;
}

void GLRenderer::updateMoveState(int buttonId, bool pressed) {
    if (buttonId >= 0 && buttonId < 10) {
        m_btnStates[buttonId] = pressed;
    }
}

void GLRenderer::renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);

    float aspect = (float)m_width / (float)m_height;
    float mvp[16] = {
        1.0f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.05f, -1.0f,
        0.0f, 0.0f, -0.2f, 0.0f
    };

    glUniformMatrix4fv(m_uMVPMatrixHandle, 1, GL_FALSE, mvp);

    static const GLfloat lineVertices[] = {
        -10.0f, 0.0f, -10.0f,   10.0f, 0.0f, -10.0f,
         -10.0f, 0.0f,  10.0f,   10.0f, 0.0f,  10.0f,
         -10.0f, 0.0f, -10.0f,  -10.0f, 0.0f,  10.0f,
          10.0f, 0.0f, -10.0f,   10.0f, 0.0f,  10.0f
    };

    static const GLfloat colors[] = {
        0.83f, 0.55f, 0.27f, 1.0f,  0.83f, 0.55f, 0.27f, 1.0f,
        0.83f, 0.55f, 0.27f, 1.0f,  0.83f, 0.55f, 0.27f, 1.0f,
        0.83f, 0.55f, 0.27f, 1.0f,  0.83f, 0.55f, 0.27f, 1.0f,
        0.83f, 0.55f, 0.27f, 1.0f,  0.83f, 0.55f, 0.27f, 1.0f
    };

    glVertexAttribPointer(m_aPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, lineVertices);
    glEnableVertexAttribArray(m_aPositionHandle);

    glVertexAttribPointer(m_aColorHandle, 4, GL_FLOAT, GL_FALSE, 0, colors);
    glEnableVertexAttribArray(m_aColorHandle);

    glDrawArrays(GL_LINES, 0, 8);
}
