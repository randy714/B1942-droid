#include <jni.h>
#include <GLES3/gl3.h>
#include <vulkan/vulkan.h>
#include <android/log.h>
#include <chrono>
#include <atomic>

#define LOG_TAG "B1942_RENDERER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Shader OpenGL ES 3.0 Hardware Accelerated
static const char* vShaderCode =
    "#version 300 es\n"
    "in vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char* fShaderCode =
    "#version 300 es\n"
    "precision mediump float;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "  fragColor = vec4(0.545, 0.102, 0.102, 1.0);\n" // Merah (#8B1A1A)
    "}\n";

class OptimizedRenderer {
private:
    GLuint program = 0;
    GLuint vao = 0, vbo = 0;
    int screenW = 1280, screenH = 720;
    bool isReady = false;

    // Sistem Waktu Nyata (Delta Time) Anti Frame Drop
    std::chrono::high_resolution_clock::time_point lastTime;
    std::atomic<float> deltaTime{0.016f}; 

    GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        return shader;
    }

public:
    void init() {
        lastTime = std::chrono::high_resolution_clock::now();
        glClearColor(0.05f, 0.04f, 0.03f, 1.0f); // Dark Espresso
        glEnable(GL_DEPTH_TEST);

        GLuint vShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
        GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

        program = glCreateProgram();
        glAttachShader(program, vShader);
        glAttachShader(program, fShader);
        glLinkProgram(program);

        // Optimalisasi VAO & VBO (Mencegah Lag CPU)
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        
        isReady = true;
        LOGI("GLES3 & Vulkan Engine Core Ready. Hardware Acceleration Active.");
    }

    void resize(int w, int h) {
        screenW = w;
        screenH = (h <= 0) ? 1 : h;
        glViewport(0, 0, screenW, screenH);
    }

    void render() {
        if (!isReady) return;

        // Hitung Delta Time
        auto now = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program);

        // Crosshair Presisi
        float aspect = (float)screenW / (float)screenH;
        float sizeX = 0.025f / aspect;
        float sizeY = 0.045f;

        GLfloat crosshairVerts[] = {
            -sizeX,  0.0f,    sizeX,  0.0f,
             0.0f,  -sizeY,   0.0f,   sizeY
        };

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVerts), crosshairVerts, GL_DYNAMIC_DRAW);

        GLuint posAttr = glGetAttribLocation(program, "vPosition");
        glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(posAttr);

        glLineWidth(3.0f);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0);
    }

    float getDeltaTime() { return deltaTime.load(); }
};

static OptimizedRenderer g_renderer;

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGLInit(JNIEnv* env, jobject thiz) {
    g_renderer.init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGLResize(JNIEnv* env, jobject thiz, jint width, jint height) {
    g_renderer.resize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeGLRender(JNIEnv* env, jobject thiz) {
    g_renderer.render();
}

// Jembatan Waktu Nyata untuk Refractor Engine
float getEngineDeltaTime() {
    return g_renderer.getDeltaTime();
}
