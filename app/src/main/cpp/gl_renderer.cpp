#include "gl_renderer.h"
#include <android/log.h>
#include <vector>

#define LOG_TAG "B1942_Engine_Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* VERTEX_SHADER_SRC =
    "attribute vec4 aPosition;\n"
    "uniform mat4 uMVPMatrix;\n"
    "void main() {\n"
    "    gl_Position = uMVPMatrix * aPosition;\n"
    "}\n";

const char* FRAGMENT_SHADER_SRC =
    "precision mediump float;\n"
    "void main() {\n"
    "    gl_FragColor = vec4(0.83, 0.55, 0.27, 1.0);\n" // Warna Perunggu Taktis
    "}\n";

GLRenderer::GLRenderer() 
    : m_width(1280), m_height(720), m_program(0), m_camYaw(0.0f), m_camPitch(0.0f) {
    for (int i = 0; i < 10; i++) m_btnStates[i] = false;
}

GLRenderer::~GLRenderer() {
    if (m_program) glDeleteProgram(m_program);
}

void GLRenderer::init(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
    glClearColor(0.05f, 0.04f, 0.04f, 1.0f); // Background Espresso
    glEnable(GL_DEPTH_TEST);

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
    
    LOGI("ENGINE CORE: Native Pipeline Fully Connected");
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

    // Matrix Perspektif agar Engine tidak Hitam (Daging Engine Visual)
    float aspect = (float)m_width / (float)m_height;
    float mvp[16] = {
        1.0f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.05f, -1.0f,
        0.0f, 0.0f, -0.2f, 0.0f
    };
    glUniformMatrix4fv(m_uMVPMatrixHandle, 1, GL_FALSE, mvp);

    // MENGGAMBAR GRID TAKTIS 3D (BUKTI ENGINE BERJALAN)
    static const GLfloat gridLines[] = {
        -1.0f, 0.0f, -1.0f,  1.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, -0.5f,  1.0f, 0.0f, -0.5f,
        -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,
        -1.0f, 0.0f,  0.5f,  1.0f, 0.0f,  0.5f,
        -1.0f, 0.0f,  1.0f,  1.0f, 0.0f,  1.0f,
        
        -1.0f, 0.0f, -1.0f, -1.0f, 0.0f,  1.0f,
        -0.5f, 0.0f, -1.0f, -0.5f, 0.0f,  1.0f,
         0.0f, 0.0f, -1.0f,  0.0f, 0.0f,  1.0f,
         0.5f, 0.0f, -1.0f,  0.5f, 0.0f,  1.0f,
         1.0f, 0.0f, -1.0f,  1.0f, 0.0f,  1.0f
    };

    glVertexAttribPointer(m_aPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, gridLines);
    glEnableVertexAttribArray(m_aPositionHandle);
    glDrawArrays(GL_LINES, 0, 20);
}
