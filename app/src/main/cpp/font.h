#pragma once
#ifdef DESKTOP_BUILD
#include <GL/glew.h>
#else
#include <GLES3/gl3.h>
#endif
#include <string>

class Font {
public:
    static Font& get();
    bool init();
    void drawText(const std::string& text, float x, float y, float scale, float r, float g, float b, float alpha = 1.0f);
    void drawRect(float x, float y, float w, float h, float r, float g, float b, float alpha = 1.0f);
    void shutdown();
private:
    Font();
    GLuint mTexture;
    GLuint mShaderProg;
    GLuint mRectProg;
    GLint mMVPLoc, mColorLoc, mTexLoc;
    GLint mRectMVPLoc, mRectColorLoc;
    GLuint mVBO, mVAO;
    GLuint mRectVAO, mRectVBO;
    int mCharWidth, mCharHeight;
    void setupBuffers();
};
