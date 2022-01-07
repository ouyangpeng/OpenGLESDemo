//
// Created by OuyangPeng on 2021/12/24 0024.
// 参考博客：
// OpenGL ES 文字渲染  https://www.jianshu.com/p/93b059a9d84f
//

#include "TextRenderSample.h"

static const wchar_t BYTE_ROLLING[] = L"微信公众号 【字节卷动】，欢迎关注交流学习。";
static const int MAX_SHORT_VALUE = 65536;

TextRenderSample::TextRenderSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;
    m_VboId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

TextRenderSample::~TextRenderSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TextRenderSample::Create() {
    LoadFacesByASCII();

    LoadFacesByUnicode(BYTE_ROLLING, sizeof(BYTE_ROLLING)/sizeof(BYTE_ROLLING[0]) - 1);

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_texrender.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texrender.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (m_ProgramObj)
    {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_textTexture");
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    }
    else
    {
        LOGD("TextRenderSample::Init create program fail")
    }

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, &m_VboId);

    glBindVertexArray(m_VaoId);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
    // 每个 2D 方块需要 6 个顶点，
    // 每个顶点又是由一个 4 维向量（一个纹理坐标和一个顶点坐标）组成，
    // 因此我们将VBO 的内存分配为 6*4 个 float 的大小。
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //upload RGBA image data
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void TextRenderSample::Draw() {
    LOGD("TextRenderSample::Draw()")
    if(m_ProgramObj == GL_NONE) return;

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    // 禁用byte-alignment限制
    // OpenGL 纹理对应的图像默认要求 4 字节对齐，这里需要设置为 1 ，
    // 确保宽度不是 4 倍数的位图（灰度图）能够正常渲染。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 开启混合，去掉文字背景。
    glEnable(GL_BLEND);
    //glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec2 viewport(m_Width, m_Height);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, viewport.x / viewport.y);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // (x,y)为屏幕坐标系的位置，即原点位于屏幕中心，x(-1.0,1.0), y(-1.0,1.0)
    RenderText("Welcome to my blog and WeChat official account.", -0.9f, 0.2f, 1.0f, glm::vec3(0.8, 0.1f, 0.1f), viewport);
    RenderText("https://ouyangpeng.blog.csdn.net", -0.9f, 0.0f, 1.5f, glm::vec3(0.2, 0.4f, 0.7f), viewport);

    RenderText(BYTE_ROLLING, sizeof(BYTE_ROLLING)/sizeof(BYTE_ROLLING[0]) - 1, -0.9f, -0.2f, 1.0f, glm::vec3(0.7, 0.4f, 0.2f), viewport);
}

void TextRenderSample::Shutdown() {
    GLBaseSample::Shutdown();

    glDeleteBuffers(1, &m_VboId);
    glDeleteVertexArrays(1, &m_VaoId);
    glDeleteTextures(1, &m_TextureId);

    std::map<GLint, Character>::const_iterator iter;
    for (iter = m_Characters.begin(); iter != m_Characters.end(); iter++)
    {
        glDeleteTextures(1, &m_Characters[iter->first].textureID);
    }
}

void TextRenderSample::LoadImage(NativeImage *pImage) {
    LOGD("TextRenderSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage)
    {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
TextRenderSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void TextRenderSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("TextRenderSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;
}

void
TextRenderSample::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color,
                             glm::vec2 viewport) {
    // 激活合适的渲染状态
    glUseProgram(m_ProgramObj);
    glUniform3f(glGetUniformLocation(m_ProgramObj, "u_textColor"), color.x, color.y, color.z);
    glBindVertexArray(m_VaoId);
//    GO_CHECK_GL_ERROR()
    // 对文本中的所有字符迭代
    std::string::const_iterator c;
    x *= viewport.x;
    y *= viewport.y;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = m_Characters[*c];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        xpos /= viewport.x;
        ypos /= viewport.y;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        w /= viewport.x;
        h /= viewport.y;

//        LOGD("TextRenderSample::RenderText [xpos,ypos,w,h]=[%f, %f, %f, %f], ch.advance >> 6 = %d", xpos, ypos, w, h, ch.advance >> 6)

        // 当前字符的VBO
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // 在方块上绘制字形纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glUniform1i(m_SamplerLoc, 0);
//        GO_CHECK_GL_ERROR()
        
        // 更新当前字符的VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        GO_CHECK_GL_ERROR()
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // 绘制方块
        glDrawArrays(GL_TRIANGLES, 0, 6);
//        GO_CHECK_GL_ERROR()
        
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.advance >> 6) * scale; //(2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void
TextRenderSample::RenderText(const wchar_t *text, int textLen, GLfloat x, GLfloat y, GLfloat scale,
                             glm::vec3 color, glm::vec2 viewport) {
    // 激活合适的渲染状态
    glUseProgram(m_ProgramObj);
    glUniform3f(glGetUniformLocation(m_ProgramObj, "u_textColor"), color.x, color.y, color.z);
    glBindVertexArray(m_VaoId);
    GO_CHECK_GL_ERROR()
    x *= viewport.x;
    y *= viewport.y;
    for (int i = 0; i < textLen; ++i)
    {
        Character ch = m_Characters[text[i]];

        GLfloat xpos = x + ch.bearing.x * scale;
        GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

        xpos /= viewport.x;
        ypos /= viewport.y;

        GLfloat w = ch.size.x * scale;
        GLfloat h = ch.size.y * scale;

        w /= viewport.x;
        h /= viewport.y;

//        LOGD("TextRenderSample::RenderText [xpos,ypos,w,h]=[%f, %f, %f, %f]", xpos, ypos, w, h)

        // 当前字符的VBO
        GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.0, 0.0 }
        };

        // 在方块上绘制字形纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glUniform1i(m_SamplerLoc, 0);
//        GO_CHECK_GL_ERROR()
        // 更新当前字符的VBO
        glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//        GO_CHECK_GL_ERROR()
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制方块
        glDrawArrays(GL_TRIANGLES, 0, 6);
//        GO_CHECK_GL_ERROR()
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.advance >> 6) * scale; //(2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderSample::LoadFacesByASCII() {
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        LOGD("TextRenderSample::LoadFacesByASCII FREETYPE: Could not init FreeType Library")

    // Load font as face
    FT_Face face;
    std::string path(DEFAULT_OGL_ASSETS_DIR);
    // 加载 Antonio-Regular.ttf
    if (FT_New_Face(ft, (path + "/fonts/Antonio-Regular.ttf").c_str(), 0, &face))
        LOGD("TextRenderSample::LoadFacesByASCII FREETYPE: Failed to load font")

    // Set size to load glyphs as
    // FT_Set_Pixel_Sizes 用于设置文字的大小，此函数设置了字体面的宽度和高度，
    // 将宽度值设为0表示我们要从字体面通过给出的高度中动态计算出字形的宽度。
    FT_Set_Pixel_Sizes(face, 0, 96);

    // Disable byte-alignment restriction
    // OpenGL 纹理对应的图像默认要求 4 字节对齐，
    // 这里需要设置为 1 ，确保宽度不是 4 倍数的位图（灰度图）能够正常渲染。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    // 简单起见，我们只生成表示 128 个 ASCII 字符的字符表，
    // 并为每一个字符储存纹理和一些度量值。
    // 这样，所有需要的字符就被存下来备用了。
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        // 一个字体面中 Face 包含了所有字形的集合，
        // 我们可以通过调用 FT_Load_Char 函数来激活当前要表示的字形。
        // 通过将 FT_LOAD_RENDER 设为一个加载标识，
        // 我们告诉 FreeType 去创建一个 8 位的灰度位图，
        // 我们可以通过face->glyph->bitmap 来取得这个位图。
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            LOGD("TextRenderSample::LoadFacesByASCII FREETYTPE: Failed to load Glyph")
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // 针对 OpenGL ES 灰度图要使用的纹理格式是 GL_LUMINANCE 而不是 GL_RED 。
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_LUMINANCE,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_LUMINANCE,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        LOGD("TextRenderSample::LoadFacesByASCII [w,h,buffer]=[%d, %d, %p], ch.advance >> 6 = %ld", face->glyph->bitmap.width,face->glyph->bitmap.rows, face->glyph->bitmap.buffer,face->glyph->advance.x >> 6)

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>(face->glyph->advance.x)
        };
        m_Characters.insert(std::pair<GLint, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);


    // Destroy FreeType once we're finished
    // 使用完 FreeType 记得释放相关资源
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderSample::LoadFacesByUnicode(const wchar_t *text, int size) {
// FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        LOGD("TextRenderSample::LoadFacesByUnicode FREETYPE: Could not init FreeType Library")

    // Load font as face
    FT_Face face;
    std::string path(DEFAULT_OGL_ASSETS_DIR);
    if (FT_New_Face(ft, (path + "/fonts/msyh.ttc").c_str(), 0, &face))
        LOGD("TextRenderSample::LoadFacesByUnicode FREETYPE: Failed to load font")

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 96, 96);
    FT_Select_Charmap(face, ft_encoding_unicode);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (int i = 0; i < size; ++i) {
        //int index =  FT_Get_Char_Index(face,unicodeArr[i]);
        if (FT_Load_Glyph(face, FT_Get_Char_Index(face, text[i]), FT_LOAD_DEFAULT))
        {
            LOGD("TextRenderSample::LoadFacesByUnicode FREETYTPE: Failed to load Glyph")
            continue;
        }

        FT_Glyph glyph;
        FT_Get_Glyph(face->glyph, &glyph );

        //Convert the glyph to a bitmap.
        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1 );
        FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

        //This reference will make accessing the bitmap easier
        FT_Bitmap& bitmap = bitmap_glyph->bitmap;

        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_LUMINANCE,
                bitmap.width,
                bitmap.rows,
                0,
                GL_LUMINANCE,
                GL_UNSIGNED_BYTE,
                bitmap.buffer
        );

        LOGD("TextRenderSample::LoadFacesByUnicode text[i]=%d [w,h,buffer]=[%d, %d, %p], advance.x=%ld", text[i], bitmap.width, bitmap.rows, bitmap.buffer, glyph->advance.x / MAX_SHORT_VALUE)
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLuint>((glyph->advance.x / MAX_SHORT_VALUE) << 6)
        };
        m_Characters.insert(std::pair<GLint, Character>(text[i], character));

    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
