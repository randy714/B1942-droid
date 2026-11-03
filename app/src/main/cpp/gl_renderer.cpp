#include <jni.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <chrono>

#define LOG_TAG "B1942_GL"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static GLuint gShaderProgram = 0;
static GLint gPositionHandle = -1;
static GLint gColorHandle = -1;

// Universal Vertex Shader
static const char* VERTEX_SHADER =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

// Universal Fragment Shader
static const char* FRAGMENT_SHADER =
    "precision mediump float;\n"
    "uniform vec4 vColor;\n"
    "void main() {\n"
    "  gl_FragColor = vColor;\n"
    "}\n";

static GLuint loadShader(GLenum type, const char* shaderCode) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    return shader;
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInitGL(JNIEnv* env, jobject thiz) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);

    gShaderProgram = glCreateProgram();
    glAttachShader(gShaderProgram, vertexShader);
    glAttachShader(gShaderProgram, fragmentShader);
    glLinkProgram(gShaderProgram);

    gPositionHandle = glGetAttribLocation(gShaderProgram, "vPosition");
    gColorHandle = glGetUniformLocation(gShaderProgram, "vColor");

    // Warna Latar Belakang: Dark Espresso (#0D0B0A)
    glClearColor(0.05f, 0.04f, 0.04f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    LOGI("GL Renderer: Universal OpenGL ES Engine Siap.");
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeResizeGL(JNIEnv* env, jobject thiz, jint width, jint height) {
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeRenderGL(JNIEnv* env, jobject thiz) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gShaderProgram);

    // Visual Crosshair Merah (#8B1A1A) di Tengah Layar
    GLfloat crosshairVertices[] = {
        -0.03f,  0.00f, 0.0f,
         0.03f,  0.00f, 0.0f,
         0.00f, -0.05f, 0.0f,
         0.00f,  0.05f, 0.0f
    };

    glVertexAttribPointer(gPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, crosshairVertices);
    glEnableVertexAttribArray(gPositionHandle);

    // Warna Merah Crosshair (#8B1A1A -> R:0.54, G:0.10, B:0.10)
    glUniform4f(gColorHandle, 0.54f, 0.10f, 0.10f, 1.0f);
    glLineWidth(4.0f);
    glDrawArrays(GL_LINES, 0, 4);

    glDisableVertexAttribArray(gPositionHandle);
}
