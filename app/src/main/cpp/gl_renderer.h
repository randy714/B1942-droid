#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    void init(int width, int height);
    void resize(int width, int height);
    void renderFrame();
    void setCameraLook(float yawDelta, float pitchDelta);
    void updateMoveState(int buttonId, bool pressed);

private:
    GLuint compileShader(GLenum type, const char* source);
    GLuint createProgram(const char* vertexSource, const char* fragmentSource);

    int m_width;
    int m_height;
    GLuint m_program;
    GLint m_aPositionHandle;
    GLint m_aColorHandle;
    GLint m_uMVPMatrixHandle;

    float m_camYaw;
    float m_camPitch;
    bool m_btnStates[10];
};

#endif // GL_RENDERER_H
