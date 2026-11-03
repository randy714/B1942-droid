#include <jni.h>
#include <GLES2/gl2.h>
#include <vector>

static GLuint gProgram;
static GLuint gTexId;

const char* vSrc = "attribute vec4 p; attribute vec2 t; varying vec2 v; void main(){ gl_Position=p; v=t; }";
const char* fSrc = "precision mediump float; uniform sampler2D s; varying vec2 v; void main(){ gl_FragColor=texture2D(s, v); }";

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeInitGL(JNIEnv* env, jobject thiz) {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vSrc, NULL); glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fSrc, NULL); glCompileShader(fs);

    gProgram = glCreateProgram();
    glAttachShader(gProgram, vs); glAttachShader(gProgram, fs);
    glLinkProgram(gProgram);

    glGenTextures(1, &gTexId);
    glBindTexture(GL_TEXTURE_2D, gTexId);
    // Placeholder agar tidak hitam total saat loading
    unsigned char color[] = { 60, 50, 40, 255 }; 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, color);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

extern "C" JNIEXPORT void JNICALL
Java_com_b1942_engine_GameActivity_nativeRenderGL(JNIEnv* env, jobject thiz) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(gProgram);

    float v[] = { -1,1, -1,-1, 1,1, 1,-1 };
    float t[] = { 0,0, 0,1, 1,0, 1,1 };

    int pLoc = glGetAttribLocation(gProgram, "p");
    int tLoc = glGetAttribLocation(gProgram, "t");

    glVertexAttribPointer(pLoc, 2, GL_FLOAT, GL_FALSE, 0, v);
    glEnableVertexAttribArray(pLoc);
    glVertexAttribPointer(tLoc, 2, GL_FLOAT, GL_FALSE, 0, t);
    glEnableVertexAttribArray(tLoc);

    glBindTexture(GL_TEXTURE_2D, gTexId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
