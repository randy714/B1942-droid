#include <jni.h>
#include <GLES2/gl2.h>
#include <android/log.h>

#define LOG_TAG "B1942_RENDERER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static const char* vShaderCode =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char* fShaderCode =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.545, 0.102, 0.102, 1.0);\n" // Crosshair Red (#8B1A1A)
    "}\n";

class CleanGles2Renderer {
private:
    GLuint program = 0;
    GLuint posAttr = 0;
    int screenW = 1280;
    int screenH = 720;
    bool isReady = false;

    GLuint compileShader(GLenum type, const char* src) {
        GLuint shader = glCreateShader(type);
        if (!shader) return 0;
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

public:
    void init() {
        // Dark Espresso Background (#0D0B0A)
        glClearColor(0.05f, 0.04f, 0.03f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        GLuint vShader = compileShader(GL_VERTEX_SHADER, vShaderCode);
        GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fShaderCode);

        if (vShader && fShader) {
            program = glCreateProgram();
            glAttachShader(program, vShader);
            glAttachShader(program, fShader);
            glLinkProgram(program);

            GLint linked = 0;
            glGetProgramiv(program, GL_LINK_STATUS, &linked);
            if (linked) {
                posAttr = glGetAttribLocation(program, "vPosition");
                isReady = true;
            }
        }
        LOGI("Clean Renderer GLES2 Ready. Zero Grid.");
    }

    void resize(int w, int h) {
        screenW = w;
        screenH = (h <= 0) ? 1 : h;
        glViewport(0, 0, screenW, screenH);
    }

    void render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!isReady || program == 0) return;

        glUseProgram(program);

        // Render Crosshair Merah Tipis Presisi (Tanpa Grid)
        float aspect = (float)screenW / (float)screenH;
        float sizeX = 0.025f / aspect;
        float sizeY = 0.045f;

        GLfloat crosshairVerts[] = {
            -sizeX,  0.0f,    sizeX,  0.0f,   // Horizontal
             0.0f,  -sizeY,   0.0f,   sizeY   // Vertical
        };

        glLineWidth(3.0f);
        glVertexAttribPointer(posAttr, 2, GL_FLOAT, GL_FALSE, 0, crosshairVerts);
        glEnableVertexAttribArray(posAttr);

        glDrawArrays(GL_LINES, 0, 4);

        glDisableVertexAttribArray(posAttr);
    }
};

static CleanGles2Renderer g_renderer;

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
