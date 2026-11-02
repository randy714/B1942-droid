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

GLRenderer::GLRenderer() : m_program(0) {}

void GLRenderer::init(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.05f, 0.04f, 0.04f, 1.0f); // Warna Background Gelap

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
    
    LOGI("ENGINE CORE: Visual Pipeline Active");
}

void GLRenderer::renderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_program);

    // Membuat Matrix Simulasi 3D agar tidak hitam polos
    float mvp[16] = {
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.8f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, -1.0f,
        0.0f, 0.0f, -0.2f, 0.0f
    };
    glUniformMatrix4fv(m_uMVPMatrixHandle, 1, GL_FALSE, mvp);

    // MENGGAMBAR GRID LANTAI 3D (BUKTI ENGINE JALAN)
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
