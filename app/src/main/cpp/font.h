#pragma once
#include <GLES3/gl3.h>
#include <string>

class Font {
public:
    static Font& get();
    bool init();
    void drawText(const std::string& text, float x, float y, float scale, float r, float g, float b, float alpha = 1.0f);
    void shutdown();
private:
    Font();
    GLuint mTexture;
    GLuint mShaderProg;
    GLint mMVPLoc, mColorLoc, mTexLoc;
    GLuint mVBO, mVAO;
    int mCharWidth, mCharHeight;
    void setupBuffers();
};
