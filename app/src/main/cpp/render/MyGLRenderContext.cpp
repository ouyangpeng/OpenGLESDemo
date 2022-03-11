//
// Created by OuyangPeng on 2021/11/26.
//
#include "MyGLRenderContext.h"
#include "FaceSlenderSample.h"
#include <exception>
#include <SimpleTexture2D.h>
#include <NativeTriangle.h>
#include <NativeTriangle2.h>
#include <NativeTriangle3.h>
#include <NativeTriangleMapBuffers.h>
#include <NativeTriangleVertextArrayObject.h>
#include <NativeTriangleVertextBufferObject.h>
#include <NativeCubeSimpleVertexShader.h>
#include <SimpleTextureCubeMap.h>
#include <MipMap2D.h>
#include <TextureWrap.h>
#include <MultiTexture.h>
#include <ParticleSystem.h>
#include <ParticleSystemTransformFeedBack.h>
#include <Noise3DRender.h>
#include <TerrainRender.h>
#include <MRT.h>
#include <Shadows.h>

#include <Native1Lesson.h>
#include <Native2Lesson.h>
#include <Native3Lesson.h>
#include <Native4Lesson.h>
#include <Native5Lesson.h>
#include <Native6Lesson.h>

#include <GLUtils.h>
#include <ImageDef.h>
#include <TextureMapSample.h>
#include <NV21TextureMapSample.h>
#include <FBOSample.h>
#include <FBOLegLengthenSample.h>
#include <CoordSystemSample.h>
#include <BasicLightingSample.h>
#include <MultiLightingsSample.h>
#include <Instancing3DSample.h>
#include <StencilTestingSample.h>
#include <BlendingSample.h>
#include <ParticlesSample2.h>
#include <SkyBoxSample.h>
#include <PBOSample.h>
#include <TimeTunnelSample.h>
#include <BaseShaderToySimpleSample.h>
#include <BezierCurveSample.h>
#include <BigEyesSample.h>
#include <FaceSlenderSample.h>
#include <BigHeadSample.h>
#include <RotaryHeadSample.h>
#include <VisualizeAudioSample.h>
#include <ScratchCardSample.h>
#include <AvatarSample.h>
#include <ShockWaveSample.h>
#include <MRTSample.h>
#include <FBOBlitSample.h>
#include <UniformBufferSample.h>
#include <RGB2YUVSample.h>
#include <SharedEGLContextSample.h>
#include <TextRenderSample.h>
#include <PortraitStayColorExample.h>
#include <GLTransitionExample.h>
#include <Model3DSample.h>
#include <AirHockeySample.h>
#include <NativeRectangle.h>
#include <StickerSample.h>
#include <TimeWatermarkStickerSample.h>
#include <Model3DSample2.h>
#include <GreenScreenMatting.h>
#include <GreenScreenMattingMix.h>

// 自定义异常
// 参考 https://wiki.sei.cmu.edu/confluence/display/cplusplus/ERR60-CPP.+Exception+objects+must+be+nothrow+copy+constructible
struct MyGLException : public std::exception {
    std::runtime_error m;

public:
    explicit MyGLException(const std::string &msg) : m(msg) {}

    const char *what() const noexcept override {
        return m.what();
    }
};

MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    LOGD("MyGLRenderContext::MyGLRenderContext")
    m_pCurSample = nullptr;
    m_pBeforeSample = nullptr;
}

MyGLRenderContext::~MyGLRenderContext() {
    LOGD("MyGLRenderContext::~MyGLRenderContext")

    if (m_pCurSample) {
        m_pCurSample->Shutdown();
        delete m_pCurSample;
        m_pCurSample = nullptr;
    }

    if (m_pBeforeSample) {
        m_pBeforeSample->Shutdown();
        delete m_pBeforeSample;
        m_pBeforeSample = nullptr;
    }

}


void MyGLRenderContext::SetRenderType(int sampleCategoryType, int renderSampleType) {
    LOGD("MyGLRenderContext::SetRenderType sampleCategoryType = %d, renderSampleType = %d",
         sampleCategoryType, renderSampleType)

    if (sampleCategoryType == SAMPLE_TYPE) {
        m_pBeforeSample = m_pCurSample;

        LOGD("MyGLRenderContext::SetRenderType 0 m_pBeforeSample = %p", m_pBeforeSample)

        switch (renderSampleType) {
            case SAMPLE_TYPE_KEY_TRIANGLE:
                m_pCurSample = new NativeTriangle();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE2:
                m_pCurSample = new NativeTriangle2();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE3:
                m_pCurSample = new NativeTriangle3();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE_MAP_BUFFERS:
                m_pCurSample = new NativeTriangleMapBuffers();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE_VERTEX_ARRAY_OBJECT:
                m_pCurSample = new NativeTriangleVAO();
                break;
            case SAMPLE_TYPE_KEY_TRIANGLE_VERTEX_BUFFER_OBJECT:
                m_pCurSample = new NativeTriangleVBO();
                break;
            case SAMPLE_TYPE_KEY_CUBE_SIMPLE_VERTEX_SHADER:
                m_pCurSample = new NativeCubeSimpleVertexShader();
                break;
            case SAMPLE_TYPE_KEY_SIMPLE_TEXTURE_2D:
                m_pCurSample = new SimpleTexture2D();
                break;
            case SAMPLE_TYPE_KEY_SIMPLE_TEXTURE_CUBE_MAP:
                m_pCurSample = new SimpleTextureCubeMap();
                break;
            case SAMPLE_TYPE_KEY_MIPMAP_2D:
                m_pCurSample = new MipMap2D();
                break;
            case SAMPLE_TYPE_KEY_TEXTURE_WRAP:
                m_pCurSample = new TextureWrap();
                break;
            case SAMPLE_TYPE_KEY_MULTI_TEXTURE:
                m_pCurSample = new MultiTexture();
                break;
            case SAMPLE_TYPE_KEY_PARTICLE_SYSTEM:
                m_pCurSample = new ParticleSystem();
                break;
            case SAMPLE_TYPE_KEY_PARTICLE_SYSTEM_TRANSFORM_FEEDBACK:
                m_pCurSample = new ParticleSystemTransformFeedBack();
                break;
            case SAMPLE_TYPE_KEY_NOISE3D:
                m_pCurSample = new Noise3DRender();
                break;
            case SAMPLE_TYPE_KEY_MRT:
                m_pCurSample = new MRT();
                break;
            case SAMPLE_TYPE_KEY_TERRAIN_RENDER:
                m_pCurSample = new TerrainRender();
                break;
            case SAMPLE_TYPE_KEY_SHADOWS:
                m_pCurSample = new Shadows();
                break;
            case SAMPLE_TYPE_KEY_LESSON_ONE:
                m_pCurSample = new Native1Lesson();
                break;
            case SAMPLE_TYPE_KEY_LESSON_TWO:
                m_pCurSample = new Native2Lesson();
                break;
            case SAMPLE_TYPE_KEY_LESSON_THREE:
                m_pCurSample = new Native3Lesson();
                break;
            case SAMPLE_TYPE_KEY_LESSON_FOUR:
                m_pCurSample = new Native4Lesson();
                break;
            case SAMPLE_TYPE_KEY_LESSON_FIVE:
                m_pCurSample = new Native5Lesson();
                break;
            case SAMPLE_TYPE_KEY_LESSON_SIX:
                m_pCurSample = new Native6Lesson();
                break;
            case SAMPLE_TYPE_KEY_TEXTURE_MAP:
                m_pCurSample = new TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_YUV_RENDER:
                m_pCurSample = new NV21TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_FBO:
                m_pCurSample = new FBOSample();
                break;
            case SAMPLE_TYPE_KEY_FBO_LEG:
                m_pCurSample = new FBOLegLengthenSample();
                break;
            case SAMPLE_TYPE_COORD_SYSTEM:
                m_pCurSample = new CoordSystemSample();
                break;
            case SAMPLE_TYPE_KEY_BASE_LIGHT:
                m_pCurSample = new BasicLightingSample();
                break;
            case SAMPLE_TYPE_KEY_MULTI_LIGHT:
                m_pCurSample = new MultiLightingsSample();
                break;
            case SAMPLE_TYPE_KEY_INSTANCING:
                m_pCurSample = new Instancing3DSample();
                break;
            case SAMPLE_TYPE_KEY_STENCIL_TESTING:
                m_pCurSample = new StencilTestingSample();
                break;
            case SAMPLE_TYPE_KEY_BLENDING:
                m_pCurSample = new BlendingSample();
                break;
            case SAMPLE_TYPE_KEY_PARTICLE_SYSTEM2:
                m_pCurSample = new ParticlesSample2();
                break;
            case SAMPLE_TYPE_KEY_SKYBOX:
                m_pCurSample = new SkyBoxSample();
                break;
            case SAMPLE_TYPE_KEY_PBO:
                m_pCurSample = new PBOSample();
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_BEATING_HEART:
                m_pCurSample = new BaseShaderToySimpleSample(
                        SAMPLE_TYPE_KEY_SHADER_TOY_BEATING_HEART);
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_CLOUD:
                m_pCurSample = new BaseShaderToySimpleSample(SAMPLE_TYPE_KEY_SHADER_TOY_CLOUD);
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_TIME_TUNNEL:
                m_pCurSample = new TimeTunnelSample();
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_MAIN_SEQUENCE_STAR:
                m_pCurSample = new BaseShaderToySimpleSample(
                        SAMPLE_TYPE_KEY_SHADER_TOY_MAIN_SEQUENCE_STAR);
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_SKY_PATH:
                m_pCurSample = new BaseShaderToySimpleSample(SAMPLE_TYPE_KEY_SHADER_TOY_SKY_PATH);
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_A_DAY:
                m_pCurSample = new BaseShaderToySimpleSample(SAMPLE_TYPE_KEY_SHADER_TOY_A_DAY);
                break;
            case SAMPLE_TYPE_KEY_SHADER_TOY_ATMOSPHERE_SYSTEM_TEST:
                m_pCurSample = new BaseShaderToySimpleSample(
                        SAMPLE_TYPE_KEY_SHADER_TOY_ATMOSPHERE_SYSTEM_TEST);
                break;
            case SAMPLE_TYPE_KEY_BEZIER_CURVE:
                m_pCurSample = new BezierCurveSample();
                break;
            case SAMPLE_TYPE_KEY_BIG_EYES:
                m_pCurSample = new BigEyesSample();
                break;
            case SAMPLE_TYPE_KEY_FACE_SLENDER:
                m_pCurSample = new FaceSlenderSample();
                break;
            case SAMPLE_TYPE_KEY_BIG_HEAD:
                m_pCurSample = new BigHeadSample();
                break;
            case SAMPLE_TYPE_KEY_RATARY_HEAD:
                m_pCurSample = new RotaryHeadSample();
                break;
            case SAMPLE_TYPE_KEY_VISUALIZE_AUDIO:
                m_pCurSample = new VisualizeAudioSample();
                break;
            case SAMPLE_TYPE_KEY_SCRATCH_CARD:
                m_pCurSample = new ScratchCardSample();
                break;
            case SAMPLE_TYPE_KEY_AVATAR:
                m_pCurSample = new AvatarSample();
                break;
            case SAMPLE_TYPE_KEY_SHOCK_WAVE:
                m_pCurSample = new ShockWaveSample();
                break;
            case SAMPLE_TYPE_KEY_MRT2:
                m_pCurSample = new MRTSample();
                break;
            case SAMPLE_TYPE_KEY_FBO_BLIT:
                m_pCurSample = new FBOBlitSample();
                break;
            case SAMPLE_TYPE_KEY_UBO:
                m_pCurSample = new UniformBufferSample();
                break;
            case SAMPLE_TYPE_KEY_RGB2YUV:
                m_pCurSample = new RGB2YUVSample();
                break;
            case SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER:
                m_pCurSample = new SharedEGLContextSample();
                break;
            case SAMPLE_TYPE_KEY_TEXT_RENDER:
                m_pCurSample = new TextRenderSample();
                break;
            case SAMPLE_TYPE_KEY_STAY_COLOR:
                m_pCurSample = new PortraitStayColorExample();
                break;
            case SAMPLE_TYPE_KEY_TRANSITIONS_1:
            case SAMPLE_TYPE_KEY_TRANSITIONS_2:
            case SAMPLE_TYPE_KEY_TRANSITIONS_3:
            case SAMPLE_TYPE_KEY_TRANSITIONS_4:
            case SAMPLE_TYPE_KEY_TRANSITIONS_5:
            case SAMPLE_TYPE_KEY_TRANSITIONS_6:
            case SAMPLE_TYPE_KEY_TRANSITIONS_7:
            case SAMPLE_TYPE_KEY_TRANSITIONS_8:
            case SAMPLE_TYPE_KEY_TRANSITIONS_9:
            case SAMPLE_TYPE_KEY_TRANSITIONS_10:
            case SAMPLE_TYPE_KEY_TRANSITIONS_11:
            case SAMPLE_TYPE_KEY_TRANSITIONS_12:
            case SAMPLE_TYPE_KEY_TRANSITIONS_13:
            case SAMPLE_TYPE_KEY_TRANSITIONS_14:
            case SAMPLE_TYPE_KEY_TRANSITIONS_15:
            case SAMPLE_TYPE_KEY_TRANSITIONS_16:
            case SAMPLE_TYPE_KEY_TRANSITIONS_17:
            case SAMPLE_TYPE_KEY_TRANSITIONS_18:
            case SAMPLE_TYPE_KEY_TRANSITIONS_19:
            case SAMPLE_TYPE_KEY_TRANSITIONS_20:
            case SAMPLE_TYPE_KEY_TRANSITIONS_21:
                m_pCurSample = new GLTransitionExample(renderSampleType);
                break;
            case SAMPLE_TYPE_KEY_3D_MODEL:
                m_pCurSample = new Model3DSample();
                break;
            case SAMPLE_TYPE_KEY_3D_MODEL2:
                m_pCurSample = new Model3DSample2();
                break;
            case SAMPLE_TYPE_KEY_AIR_HOCKEY:
                m_pCurSample = new AirHockeySample();
                break;

            case SAMPLE_TYPE_KEY_RECTANGLE:
                m_pCurSample = new NativeRectangle();
                break;

            case SAMPLE_TYPE_KEY_STICKER:
                m_pCurSample = new StickerSample();
                break;
            case SAMPLE_TYPE_KEY_TIME_WATERMARK_STICKER:
                m_pCurSample = new TimeWatermarkStickerSample();
                break;

            case SAMPLE_TYPE_KEY_GREEN_SCREEN_MATTING:
                m_pCurSample = new GreenScreenMatting();
                break;
            case SAMPLE_TYPE_KEY_GREEN_SCREEN_MATTING_MIX:
                m_pCurSample = new GreenScreenMattingMix();
                break;
            default:
                m_pCurSample = nullptr;
                break;
        }
        if (m_pCurSample == nullptr) {
            throw MyGLException(
                    "MyGLRenderContext::SetRenderType() 请注意：你应该忘记初始化你要展示的Sample类型 ，请补上初始化的代码，否则无法渲染");
        }
        LOGD("MyGLRenderContext::SetRenderType m_pBeforeSample = %p, m_pCurSample=%p",
             m_pBeforeSample, m_pCurSample)
    }
}

void MyGLRenderContext::OnSurfaceCreated(JNIEnv *env, jobject assetManager) {
    LOGD("MyGLRenderContext::OnSurfaceCreated")

    // 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
    GLUtils::setEnvAndAssetManager(env, assetManager);

    if (m_pBeforeSample) {
        m_pBeforeSample->Shutdown();
        delete m_pBeforeSample;
        m_pBeforeSample = nullptr;
    }

    if (m_pCurSample) {
        m_pCurSample->Create();
    }
}

void MyGLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGD("MyGLRenderContext::OnSurfaceChanged [w, h] = [%d, %d]", width, height)
    if (m_pCurSample) {
        m_pCurSample->Change(width, height);
    }
}

void MyGLRenderContext::OnDrawFrame() {
    if (m_pCurSample) {
        m_pCurSample->Draw();
    }
}

void MyGLRenderContext::SwitchBlendingMode() {
    if (m_pCurSample) {
        m_pCurSample->SwitchBlendingMode();
    }
}

void MyGLRenderContext::SetDelta(float x, float y) {
    if (m_pCurSample) {
        m_pCurSample->SetDelta(x, y);
    }
}

void MyGLRenderContext::SetMinFilter(int filter) {
    if (m_pCurSample) {
        m_pCurSample->SetMinFilter(filter);
    }
}

void MyGLRenderContext::SetMagFilter(int filter) {
    if (m_pCurSample) {
        m_pCurSample->SetMagFilter(filter);
    }
}


void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGD("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format, width,
         height, pData)
    NativeImage nativeImage = getImage(format, width, height, pData);

    if (m_pCurSample) {
        m_pCurSample->LoadImage(&nativeImage);
    }
}

void MyGLRenderContext::SetImageDataWithIndex(int index, int format, int width, int height,
                                              uint8_t *pData) {
    LOGD("MyGLRenderContext::SetImageDataWithIndex index=%d, format=%d, width=%d, height=%d, pData=%p",
         index, format, width, height, pData)

    NativeImage nativeImage = getImage(format, width, height, pData);

    if (m_pCurSample) {
        m_pCurSample->LoadMultiImageWithIndex(index, &nativeImage);
    }
}

NativeImage MyGLRenderContext::getImage(int format, int width, int height, uint8_t *pData) {
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }
    return nativeImage;
}


MyGLRenderContext *MyGLRenderContext::GetInstance() {
//    LOGD("MyGLRenderContext::GetInstance")
    if (m_pContext == nullptr) {
        m_pContext = new MyGLRenderContext();
    }
    return m_pContext;
}

void MyGLRenderContext::DestroyInstance() {
    LOGD("MyGLRenderContext::DestroyInstance")
    if (m_pContext) {
        delete m_pContext;
        m_pContext = nullptr;
    }

}

void
MyGLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    LOGD("MyGLRenderContext::UpdateTransformMatrix [rotateX, rotateY, scaleX, scaleY] = [%f, %f, %f, %f]",
         rotateX, rotateY, scaleX, scaleY)
    if (m_pCurSample) {
        m_pCurSample->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    }
}

void MyGLRenderContext::SetAudioData(short *buffer, int len) {
    LOGD("MyGLRenderContext::SetAudioData buffer=%p, len=%d, buffer[0]=%d", buffer, len, buffer[0]);
    if (m_pCurSample) {
        m_pCurSample->LoadAudioData(buffer, len);
    }
}

void MyGLRenderContext::SetTouchLocation(jfloat x, jfloat y) {
    LOGD("MyGLRenderContext::SetTouchLocation x=%f, y=%f", x, y);
    if (m_pCurSample) {
        m_pCurSample->SetTouchLocation(x, y);
    }
}

void MyGLRenderContext::SetGravityXY(jfloat x, jfloat y) {
    LOGD("MyGLRenderContext::SetGravityXY x=%f, y=%f", x, y);
    if (m_pCurSample) {
        m_pCurSample->SetGravityXY(x, y);
    }
}




