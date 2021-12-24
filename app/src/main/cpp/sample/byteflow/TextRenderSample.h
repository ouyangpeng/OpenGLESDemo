//
// Created by OuyangPeng on 2021/12/24 0024.
//

#ifndef OPENGLESDEMO_TEXTRENDERSAMPLE_H
#define OPENGLESDEMO_TEXTRENDERSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>
#include <ft2build.h>
#include <freetype/ftglyph.h>
#include <string>
#include <map>

// Holds all state information relevant to a character as loaded using FreeType
// 按照前面的思路，使用 FreeType 加载字形的位图然后生成纹理，然后进行纹理贴图。
// 然而每次渲染的时候都去重新加载位图显然不是高效的，
// 我们应该将这些生成的数据储存在应用程序中，在渲染过程中再去取，重复利用。

struct Character {
    GLuint textureID;   // ID handle of the glyph texture
    glm::ivec2 size;    // Size of glyph
    glm::ivec2 bearing;  // Offset from baseline to left/top of glyph
    GLuint advance;    // Horizontal offset to advance to next glyph
};

class TextRenderSample : public GLBaseSample {

public:
    TextRenderSample();

    virtual ~TextRenderSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    /**
     * (x,y)为屏幕坐标系的位置，即原点位于屏幕中心，x(-1.0,1.0), y(-1.0,1.0)
     * */
    void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color,
                    glm::vec2 viewport);

    void RenderText(const wchar_t *text, int textLen, GLfloat x, GLfloat y, GLfloat scale,
                    glm::vec3 color, glm::vec2 viewport);

    void LoadFacesByASCII();

    void LoadFacesByUnicode(const wchar_t *text, int size);

    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboId;
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    std::map<GLint, Character> m_Characters;
};


#endif //OPENGLESDEMO_TEXTRENDERSAMPLE_H
