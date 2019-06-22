#include "hdRenderContext.hpp"
#include "../Core/hdStringUtils.hpp"
#include "../Math/hdMathUtils.hpp"
#include "../IO/hdImage.hpp"
#include "../IO/hdFileStream.hpp"
#include "../../3rd/include/GLEW/glew.h"
#include <vector>

namespace hd {

static const uint32_t MAX_VERTEX_BUFFERS = 8;
static const uint32_t MAX_CONSTANT_BUFFERS = 8;
static const uint32_t MAX_TEXTURES = 8;
static const uint32_t MAX_SAMPLER_STATES = MAX_TEXTURES;

static const GLenum gVertexElementSizes[] = {
    1,
    2,
    3,
    4,
};

static const GLenum gVertexElementTypes[] = {
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT
};

static const GLenum gBufferUsages[] = {
    GL_STATIC_DRAW,
    GL_DYNAMIC_DRAW,
    GL_STREAM_DRAW
};

static const GLenum gBufferAccesses[] = {
    GL_READ_ONLY,
    GL_WRITE_ONLY,
    GL_READ_WRITE
};

static const int gTextureInternalFormats[] = {
    GL_R8,
    GL_RG8,
    GL_RGB8,
    GL_RGBA8,
    GL_ALPHA,
    GL_R16,
    GL_RG16,
    GL_RGB16,
    GL_RGBA16,
    GL_R16F,
    GL_RG16F,
    GL_RGB16F,
    GL_RGBA16F,
    GL_R32F,
    GL_RG32F,
    GL_RGB32F,
    GL_RGBA32F,
    GL_DEPTH_COMPONENT16,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH_COMPONENT32,
    GL_DEPTH24_STENCIL8,
    //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    //GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    //GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
};
static const GLenum gTextureExternalFormats[] = {
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,
    GL_ALPHA,
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,
    GL_RED,
    GL_RG,
    GL_RGB,
    GL_RGBA,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_STENCIL,
    //GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    //GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    //GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
};
static const GLenum gTextureDataTypes[] = {
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_SHORT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_FLOAT,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT_24_8,
};

static const GLenum gSamplerMinFilter[] = {
    GL_NEAREST,
    GL_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};

static const GLenum gSamplerMagFilter[] = {
    GL_NEAREST,
    GL_LINEAR,
    GL_LINEAR,
    GL_LINEAR,
    GL_LINEAR
};

static const GLint gSamplerAddressModes[] = {
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER,
    GL_MIRROR_CLAMP_EXT
};

constexpr GLenum gPrimitiveTypes[] = {
    GL_POINTS,
    GL_LINE_STRIP,
    GL_LINES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLES,
};

constexpr GLenum gCompareFuncs[] = {
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

constexpr GLenum gStencilOps[] =
{
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INCR,
    GL_DECR,
    GL_INVERT,
    GL_INCR_WRAP,
    GL_DECR_WRAP,
};

constexpr GLenum gCullFaces[] = {
    0,
    GL_FRONT,
    GL_BACK,
    GL_FRONT_AND_BACK
};

constexpr GLenum gFillModes[] = {
    GL_FILL,
    GL_LINE
};

constexpr GLenum gFrontFaces[] = {
    GL_CCW,
    GL_CW
};

constexpr int gBlendFactors[] = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA_SATURATE,
};

constexpr int gBlendOps[] = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT,
    GL_FUNC_REVERSE_SUBTRACT,
    GL_MIN,
    GL_MAX,
};

const BlendModeDesc gBlendModeDescs[] = {
    BlendModeDesc(false, BlendFactor::One         , BlendFactor::Zero       , BlendOp::Add        , BlendFactor::One         , BlendFactor::Zero       , BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::One         , BlendFactor::One        , BlendOp::Add        , BlendFactor::One         , BlendFactor::One        , BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::DestColor   , BlendFactor::Zero       , BlendOp::Add        , BlendFactor::One         , BlendFactor::Zero       , BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::SrcAlpha    , BlendFactor::InvSrcAlpha, BlendOp::Add        , BlendFactor::SrcAlpha    , BlendFactor::InvSrcAlpha, BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::SrcAlpha    , BlendFactor::One        , BlendOp::Add        , BlendFactor::SrcAlpha    , BlendFactor::One        , BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::One         , BlendFactor::InvSrcAlpha, BlendOp::Add        , BlendFactor::One         , BlendFactor::InvSrcAlpha, BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::InvDestAlpha, BlendFactor::DestAlpha  , BlendOp::Add        , BlendFactor::InvDestAlpha, BlendFactor::DestAlpha  , BlendOp::Add),
    BlendModeDesc(true,  BlendFactor::One         , BlendFactor::One        , BlendOp::RevSubtract, BlendFactor::One         , BlendFactor::One        , BlendOp::RevSubtract),
    BlendModeDesc(true,  BlendFactor::SrcAlpha    , BlendFactor::One        , BlendOp::RevSubtract, BlendFactor::SrcAlpha    , BlendFactor::One        , BlendOp::RevSubtract)
};

void debugCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int length, const char *message, const void *userParam) {
    const char *sourceStr = nullptr, *typeStr = nullptr, *severityStr = nullptr;
    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB: {
            sourceStr = "API";
            break;
        }
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: {
            sourceStr = "Window system";
            break;
        }
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: {
            sourceStr = "Shader compiler";
            break;
        }
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: {
            sourceStr = "Third party";
            break;
        }
        case GL_DEBUG_SOURCE_APPLICATION_ARB: {
            sourceStr = "Application";
            break;
        }
        case GL_DEBUG_SOURCE_OTHER_ARB: {
            sourceStr = "Other";
            break;
        }
        default: {
            sourceStr = "Unknown";
            break;
        }
    }
    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB: {
            typeStr = "Error";
            break;
        }
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: {
            typeStr = "Deprecated behaviour";
            break;
        }
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: {
            typeStr = "Undefined behaviour";
            break;
        }
        case GL_DEBUG_TYPE_PORTABILITY_ARB: {
            typeStr = "Portability";
            break;
        }
        case GL_DEBUG_TYPE_PERFORMANCE_ARB: {
            typeStr = "Performance";
            break;
        }
        case GL_DEBUG_TYPE_OTHER_ARB: {
            typeStr = "Other";
            break;
        }
        default: {
            typeStr = "Unknown";
            break;
        }
    }
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB: {
            severityStr = "High";
            break;
        }
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: {
            severityStr = "Medium";
            break;
        }
        case GL_DEBUG_SEVERITY_LOW_ARB: {
            severityStr = "Low";
            break;
        }
        default: {
            severityStr = "Unknown";
            break;
        }
    }
    if (type == GL_DEBUG_TYPE_ERROR_ARB) {
        HD_LOG_ERROR("OpenGL Debug:\n\tSource: %s\n\tType: %s\n\tId: %d\n\tSeverity: %s\n\tMessage: %s", sourceStr, typeStr, id, severityStr, message);
    }
    else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB || type == GL_DEBUG_TYPE_PORTABILITY_ARB || type == GL_DEBUG_TYPE_PERFORMANCE_ARB) {
        HD_LOG_WARNING("OpenGL Debug:\n\tSource: %s\n\tType: %s\n\tId: %d\n\tSeverity: %s\n\tMessage: %s", sourceStr, typeStr, id, severityStr, message);
    }
    else {
        HD_LOG_INFO("OpenGL Debug:\n\tSource: %s\n\tType: %s\n\tId: %d\n\tSeverity: %s\n\tMessage: %s", sourceStr, typeStr, id, severityStr, message);
    }
}

VertexElement::VertexElement() {
    this->type = VertexElementType::Float;
    this->index = 0;
    this->slot = 0;
    this->offset = 0;
    this->normalized = false;
    this->perInstance = false;
}

VertexElement::VertexElement(VertexElementType type, uint32_t index, uint32_t slot, uint32_t offset, bool normalized, bool perInstance) {
    this->type = type;
    this->index = index;
    this->slot = slot;
    this->offset = offset;
    this->normalized = normalized;
    this->perInstance = perInstance;
}

BlendModeDesc::BlendModeDesc() {
    this->blendEnable = false;
    this->srcBlend = BlendFactor::One;
    this->dstBlend = BlendFactor::One;
    this->blendOp = BlendOp::Add;
    this->srcBlendAlpha = BlendFactor::One;
    this->dstBlendAlpha = BlendFactor::One;
    this->blendOpAlpha = BlendOp::Add;
}

BlendModeDesc::BlendModeDesc(bool blendEnable, BlendFactor srcBlend, BlendFactor dstBlend, BlendOp blendOp, BlendFactor srcBlendAlpha, BlendFactor dstBlendAlpha, BlendOp blendOpAlpha) {
    this->blendEnable = blendEnable;
    this->srcBlend = srcBlend;
    this->dstBlend = dstBlend;
    this->blendOp = blendOp;
    this->srcBlendAlpha = srcBlendAlpha;
    this->dstBlendAlpha = dstBlendAlpha;
    this->blendOpAlpha = blendOpAlpha;
}

StencilTestDesc::StencilTestDesc() {
    this->frontFunc = CompareFunc::Always;
    this->frontFail = StencilOp::Keep;
    this->frontDepthFail = StencilOp::Keep;
    this->frontPass = StencilOp::Keep;
    this->backFunc = CompareFunc::Always;
    this->backFail = StencilOp::Keep;
    this->backDepthFail = StencilOp::Keep;
    this->backPass = StencilOp::Keep;
    this->readMask = 0xffffffff;
    this->writeMask = 0xffffffff;
}

StencilTestDesc::StencilTestDesc(CompareFunc frontFunc, StencilOp frontFail, StencilOp frontDepthFail, StencilOp frontPass,
                                 CompareFunc backFunc, StencilOp backFail, StencilOp backDepthFail, StencilOp backPass,
                                 uint32_t stencilReadMask, uint32_t stencilWriteMask){
    this->frontFunc = frontFunc;
    this->frontFail = frontFail;
    this->frontDepthFail = frontDepthFail;
    this->frontPass = frontPass;
    this->backFunc = backFunc;
    this->backFail = backFail;
    this->backDepthFail = backDepthFail;
    this->backPass = backPass;
    this->readMask = readMask;
    this->writeMask = writeMask;
}

ColorMask::ColorMask() {
    this->r = true;
    this->g = true;
    this->b = true;
    this->a = true;
}

ColorMask::ColorMask(bool rgba) {
    this->r = rgba;
    this->g = rgba;
    this->b = rgba;
    this->a = rgba;
}

ColorMask::ColorMask(bool r, bool g, bool b, bool a) {
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

bool ColorMask::operator==(const ColorMask &rhs) const {
    return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b && this->a == rhs.a;
}

bool ColorMask::operator!=(const ColorMask &rhs) const {
    return this->r != rhs.r || this->g != rhs.g || this->b != rhs.b || this->a != rhs.a;
}

PolygonOffset::PolygonOffset() {
    this->enabled = false;
    this->factor = 0.0f;
    this->units = 0.0f;
}

PolygonOffset::PolygonOffset(bool enabled, float factor, float units) {
    this->enabled = enabled;
    this->factor = factor;
    this->units = units;
}

bool PolygonOffset::operator==(const PolygonOffset &rhs) const {
    return this->enabled == rhs.enabled && MathUtils::compareFloat(this->factor, rhs.factor) && MathUtils::compareFloat(this->units, rhs.units);
}

bool PolygonOffset::operator!=(const PolygonOffset &rhs) const {
    return this->enabled != rhs.enabled || !MathUtils::compareFloat(this->factor, rhs.factor) || !MathUtils::compareFloat(this->units, rhs.units);
}

struct VertexFormatImpl {
    uint32_t id;
    std::vector<VertexElement> elements;
};

struct VertexBufferImpl {
    uint32_t id, size;
    BufferUsage usage;
};

struct IndexBufferImpl {
    uint32_t id, size;
    BufferUsage usage;
};

struct ConstantBufferImpl {
    uint32_t id, size;
    BufferUsage usage;
};

struct Texture2DImpl {
    uint32_t id;
    TextureFormat format;
    glm::ivec2 size;
};

struct SamplerStateImpl {
    uint32_t id;
    SamplerFilter filter;
    uint32_t maxAnisotropy;
    CompareFunc compareFunc;
    bool compareRefToTexture;
    SamplerAddressMode u, v, w;
    float lodBias, borderColor[4], minLod, maxLod;
};

struct ProgramImpl {
    uint32_t id;
    std::string name, vsCode, psCode, defines;
};

struct RenderContext::Impl {
    OpenGLContextSettings settings;
    std::vector<HVertexFormat> vertexFormats;
    std::vector<HVertexBuffer> vertexBuffers;
    std::vector<HIndexBuffer> indexBuffers;
    std::vector<HConstantBuffer> constantBuffers;
    std::vector<HTexture2D> textures2D;
    std::vector<HSamplerState> samplerStates;
    std::vector<HProgram> programs;
    uint32_t currentVertexFormat = 0;
    struct {
        uint32_t id = 0;
        uint32_t offset = 0, stride = 0;
    } currentVertexBuffer[MAX_VERTEX_BUFFERS];
    uint32_t currentIndexBuffer = 0;
    uint32_t currentConstantBuffer[MAX_CONSTANT_BUFFERS] = { 0 };
    uint32_t currentTexture2D[MAX_TEXTURES] = { 0 };
    uint32_t currentSamplerState[MAX_SAMPLER_STATES] = { 0 };
    uint32_t currentProgram = 0;
    uint32_t currentVBO = 0, currentUBO = 0;
    struct {
        bool isEnabled = false;
        CompareFunc compFunc = CompareFunc::Less;
        bool mask = true;
    } currentDepthState;
    struct {
        bool isEnabled = false;
        StencilTestDesc desc;
        int ref = 0;
    } currentStencilState;
    struct {
        BlendModeDesc desc;
        ColorMask colorMask;
    } currentBlendState;
    struct {
        CullFace cullFace = CullFace::None;
        FillMode fillMode = FillMode::Solid;
        FrontFace frontFace = FrontFace::CCW;
        PolygonOffset polygonOffset;
    } currentRasterizerState;
    struct {
        int x = 0, y = 0;
        int w = 0, h = 0;
    } currentViewport;

    void bindVertexFormat(uint32_t id) {
        if (currentVertexFormat != id) {
            currentVertexFormat = id;
            glBindVertexArray(id);
        }
    }

    void bindVertexBuffer(uint32_t id, uint32_t slot, uint32_t offset, uint32_t stride) {
        if (currentVertexBuffer[slot].id != id || currentVertexBuffer[slot].offset != offset ||
            currentVertexBuffer[slot].stride != stride) {
            currentVertexBuffer[slot].id = id;
            currentVertexBuffer[slot].offset = offset;
            currentVertexBuffer[slot].stride = stride;
            glBindVertexBuffer(slot, id, offset, static_cast<int>(stride));
        }
    }

    void bindIndexBuffer(uint32_t id) {
        if (currentIndexBuffer != id) {
            currentIndexBuffer = id;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        }
    }

    void bindConstantBuffer(uint32_t id, uint32_t slot) {
        if (currentConstantBuffer[slot] != id) {
            currentConstantBuffer[slot] = id;
            glBindBufferBase(GL_UNIFORM_BUFFER, slot, id);
        }
    }

    void bindTexture2D(uint32_t id, uint32_t slot) {
        if (currentTexture2D[slot] != id) {
            currentTexture2D[slot] = id;
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, id);
        }
    }

    void bindSamplerState(uint32_t id, uint32_t slot) {
        if (currentSamplerState[slot] != id) {
            currentSamplerState[slot] = id;
            glBindSampler(slot, id);
        }
    }

    void bindProgram(uint32_t id) {
        if (currentProgram != id) {
            currentProgram = id;
            glUseProgram(id);
        }
    }

    void bindVBO(uint32_t id) {
        if (currentVBO != id) {
            currentVBO = id;
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }
    }

    void bindUBO(uint32_t id) {
        if (currentUBO != id) {
            currentUBO = id;
            glBindBuffer(GL_UNIFORM_BUFFER, id);
        }
    }

    void setDepthEnabled(bool isEnabled) {
        if (currentDepthState.isEnabled != isEnabled) {
            currentDepthState.isEnabled = isEnabled;
            if (isEnabled) {
                glEnable(GL_DEPTH_TEST);
            }
            else {
                glDisable(GL_DEPTH_TEST);
            }
        }
    }

    void setDepthCompFunc(CompareFunc func) {
        if (currentDepthState.compFunc != func) {
            currentDepthState.compFunc = func;
            glDepthFunc(gCompareFuncs[static_cast<size_t>(func)]);
        }
    }

    void setDepthMask(bool mask) {
        if (currentDepthState.mask != mask) {
            currentDepthState.mask = mask;
            glDepthMask(mask);
        }
    }

    void setStencilEnabled(bool isEnabled) {
        if (currentStencilState.isEnabled != isEnabled) {
            currentStencilState.isEnabled = isEnabled;
            if (isEnabled) {
                glEnable(GL_STENCIL_TEST);
            }
            else {
                glDisable(GL_STENCIL_TEST);
            }
        }
    }

    void setStencilFunc(CompareFunc frontFunc, CompareFunc backFunc, int ref, uint32_t readMask) {
        if (currentStencilState.desc.frontFunc != frontFunc || currentStencilState.desc.backFunc != backFunc || currentStencilState.ref != ref || currentStencilState.desc.readMask != readMask) {
            currentStencilState.desc.frontFunc = frontFunc;
            currentStencilState.desc.backFunc = backFunc;
            currentStencilState.ref = ref;
            currentStencilState.desc.readMask = readMask;
            glStencilFuncSeparate(GL_FRONT, gCompareFuncs[static_cast<size_t>(frontFunc)], ref, readMask);
            glStencilFuncSeparate(GL_BACK, gCompareFuncs[static_cast<size_t>(backFunc)], ref, readMask);
        }
    }

    void setStencilOpFront(StencilOp fail, StencilOp depthFail, StencilOp pass) {
        if (currentStencilState.desc.frontFail != fail || currentStencilState.desc.frontDepthFail != depthFail || currentStencilState.desc.frontPass != pass) {
            currentStencilState.desc.frontFail = fail;
            currentStencilState.desc.frontDepthFail = depthFail;
            currentStencilState.desc.frontPass = pass;
            glStencilOpSeparate(GL_FRONT, gStencilOps[static_cast<size_t>(fail)], gStencilOps[static_cast<size_t>(depthFail)], gStencilOps[static_cast<size_t>(pass)]);
        }
    }

    void setStencilOpBack(StencilOp fail, StencilOp depthFail, StencilOp pass) {
        if (currentStencilState.desc.backFail != fail || currentStencilState.desc.backDepthFail != depthFail || currentStencilState.desc.backPass != pass) {
            currentStencilState.desc.backFail = fail;
            currentStencilState.desc.backDepthFail = depthFail;
            currentStencilState.desc.backPass = pass;
            glStencilOpSeparate(GL_BACK, gStencilOps[static_cast<size_t>(fail)], gStencilOps[static_cast<size_t>(depthFail)], gStencilOps[static_cast<size_t>(pass)]);
        }
    }

    void setStencilMask(uint32_t writeMask) {
        if (currentStencilState.desc.writeMask != writeMask) {
            currentStencilState.desc.writeMask = writeMask;
            glStencilMask(writeMask);
        }
    }

    void setBlendEnabled(bool isEnabled) {
        if (currentBlendState.desc.blendEnable != isEnabled) {
            currentBlendState.desc.blendEnable = isEnabled;
            if (isEnabled) {
                glEnable(GL_BLEND);
            }
            else {
                glDisable(GL_BLEND);
            }
        }
    }

    void setBlendFunc(BlendFactor srcBlend, BlendFactor dstBlend, BlendFactor srcBlendAlpha, BlendFactor dstBlendAlpha) {
        if (currentBlendState.desc.srcBlend != srcBlend || currentBlendState.desc.dstBlend != dstBlend || 
            currentBlendState.desc.srcBlendAlpha != srcBlendAlpha || currentBlendState.desc.dstBlendAlpha != dstBlendAlpha) {
            currentBlendState.desc.srcBlend = srcBlend;
            currentBlendState.desc.dstBlend = dstBlend; 
            currentBlendState.desc.srcBlendAlpha = srcBlendAlpha;
            currentBlendState.desc.dstBlendAlpha = dstBlendAlpha;
            glBlendFuncSeparate(gBlendFactors[static_cast<size_t>(srcBlend)], gBlendFactors[static_cast<size_t>(dstBlend)],
                gBlendFactors[static_cast<size_t>(srcBlendAlpha)], gBlendFactors[static_cast<size_t>(dstBlendAlpha)]);
        }
    }

    void setBlendOp(BlendOp blendOp, BlendOp blendOpAlpha) {
        if (currentBlendState.desc.blendOp != blendOp || currentBlendState.desc.blendOpAlpha != blendOpAlpha) {
            currentBlendState.desc.blendOp = blendOp;
            currentBlendState.desc.blendOpAlpha = blendOpAlpha;
            glBlendEquationSeparate(gBlendOps[static_cast<size_t>(blendOp)], gBlendOps[static_cast<size_t>(blendOpAlpha)]);
        }
    }

    void setBlendColorMask(const ColorMask &mask) {
        if (currentBlendState.colorMask != mask) {
            currentBlendState.colorMask = mask;
            glColorMask(mask.r, mask.g, mask.b, mask.a);
        }
    }

    void setRastCullFace(CullFace mode) {
        if (currentRasterizerState.cullFace != mode) {
            currentRasterizerState.cullFace = mode;
            if (mode == CullFace::None) {
                glDisable(GL_CULL_FACE);
            }
            else {
                glEnable(GL_CULL_FACE);
                glCullFace(gCullFaces[static_cast<size_t>(mode)]);
            }
        }
    }

    void setRastFillMode(FillMode mode) {
        if (currentRasterizerState.fillMode != mode) {
            currentRasterizerState.fillMode = mode;
            glPolygonMode(GL_FRONT_AND_BACK, gFillModes[static_cast<size_t>(mode)]);
        }
    }

    void setRastFrontFace(FrontFace mode) {
        if (currentRasterizerState.frontFace != mode) {
            currentRasterizerState.frontFace = mode;
            glFrontFace(gFrontFaces[static_cast<size_t>(mode)]);
        }
    }

    void setRastPolygonOffset(const PolygonOffset &desc) {
        if (currentRasterizerState.polygonOffset != desc) {
            currentRasterizerState.polygonOffset = desc;
            if (desc.enabled) {
                glEnable(GL_POLYGON_OFFSET_FILL);
            }
            else {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            glPolygonOffset(desc.factor, desc.units);
        }
    }

    void setViewport(int x, int y, int w, int h) {
        if (currentViewport.x != x || currentViewport.y != y || currentViewport.w != w || currentViewport.h != h) {
            currentViewport.x = x;
            currentViewport.y = y;
            currentViewport.w = w;
            currentViewport.h = h;
            glViewport(x, y, w, h);
        }
    }
};

RenderContext::RenderContext() : impl(std::make_unique<Impl>()) {
}

RenderContext::RenderContext(const Window &window) : RenderContext() {
    create(window);
}

RenderContext::~RenderContext() {
    destroy();
}

void RenderContext::create(const Window &window) {
    if (!window.hasOpenGLContext()) {
        HD_LOG_ERROR("Failed to create Context for '%s' window", window.getTitle().data());
    }

    destroy();

    impl->settings = window.getOpenGLContextSettings();
    glewExperimental = true;
    auto error = glewInit();
    if (error != GLEW_OK) {
        HD_LOG_ERROR("Failed to initialize GLEW. Errors: %s", glewGetErrorString(error));
    }

    if (window.getOpenGLContextSettings().isDebug && GLEW_ARB_debug_output) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debugCallback, nullptr);
    }

    setViewport(0, 0, window.getSizeX(), window.getSizeY());
}

void RenderContext::destroy() {
    for (auto &it : impl->vertexFormats) {
        destroyVertexFormat(it);
    }
    for (auto &it : impl->vertexBuffers) {
        destroyVertexBuffer(it);
    }
    for (auto &it : impl->indexBuffers) {
        destroyIndexBuffer(it);
    }
    for (auto &it : impl->constantBuffers) {
        destroyConstantBuffer(it);
    }
    for (auto &it : impl->textures2D) {
        destroyTexture2D(it);
    }
    for (auto &it : impl->samplerStates) {
        destroySamplerState(it);
    }
    for (auto &it : impl->programs) {
        destroyProgram(it);
    }
}

void RenderContext::clearRenderTarget(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RenderContext::clearRenderTarget(const glm::vec4 &rgba) {
    clearRenderTarget(rgba.r, rgba.g, rgba.b, rgba.a);
}

void RenderContext::clearDepthStencil(float depth) {
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderContext::draw(PrimitiveType primType, uint32_t vertexCount, uint32_t firstVertex) {
    glDrawArrays(gPrimitiveTypes[static_cast<uint32_t>(primType)], firstVertex, vertexCount);
}

void RenderContext::drawInstanced(PrimitiveType primType, uint32_t vertexCountPerInstance, uint32_t firstVertex, uint32_t instanceCount) {
    glDrawArraysInstanced(gPrimitiveTypes[static_cast<uint32_t>(primType)], firstVertex, vertexCountPerInstance, instanceCount);
}

void RenderContext::drawIndexed(PrimitiveType primType, uint32_t indexCount, uint32_t firstIndex, IndexType indexType) {
    switch (indexType) {
        case IndexType::Uint: {
            glDrawElements(gPrimitiveTypes[static_cast<uint32_t>(primType)], indexCount, GL_UNSIGNED_INT, static_cast<uint32_t*>(nullptr) + firstIndex);
            break;
        }
        case IndexType::Ushort: {
            glDrawElements(gPrimitiveTypes[static_cast<uint32_t>(primType)], indexCount, GL_UNSIGNED_SHORT, static_cast<uint16_t*>(nullptr) + firstIndex);
            break;
        }
        case IndexType::Ubyte: {
            glDrawElements(gPrimitiveTypes[static_cast<uint32_t>(primType)], indexCount, GL_UNSIGNED_BYTE, static_cast<uint8_t*>(nullptr) + firstIndex);
            break;
        }
    }
}

void RenderContext::drawIndexedInstanced(PrimitiveType primType, uint32_t indexCountPerInstance, uint32_t firstIndex, IndexType indexType, uint32_t instanceCount) {
    switch (indexType) {
        case IndexType::Uint: {
            glDrawElementsInstanced(gPrimitiveTypes[static_cast<uint32_t>(primType)], indexCountPerInstance, GL_UNSIGNED_INT, static_cast<uint32_t*>(nullptr) + firstIndex, instanceCount);
            break;
        }
        case IndexType::Ushort: {
            glDrawElementsInstanced(gPrimitiveTypes[static_cast<uint32_t>(primType)], indexCountPerInstance, GL_UNSIGNED_SHORT, static_cast<uint16_t*>(nullptr) + firstIndex, instanceCount);
            break;
        }
        case IndexType::Ubyte: {
            glDrawElementsInstanced(gPrimitiveTypes[static_cast<uint32_t>(primType)], indexCountPerInstance, GL_UNSIGNED_BYTE, static_cast<uint8_t*>(nullptr) + firstIndex, instanceCount);
            break;
        }
    }
}

void RenderContext::setDepthState(bool depthTestEnabled, CompareFunc compareFunc, bool depthMask) {
    impl->setDepthEnabled(depthTestEnabled);
    impl->setDepthCompFunc(compareFunc);
    impl->setDepthMask(depthMask);
}

void RenderContext::setStencilState(bool enabled, const StencilTestDesc &desc, int ref) {
    impl->setStencilEnabled(enabled);
    impl->setStencilFunc(desc.frontFunc, desc.backFunc, ref, desc.readMask);
    impl->setStencilOpFront(desc.frontFail, desc.frontDepthFail, desc.frontPass);
    impl->setStencilOpBack(desc.backFail, desc.backDepthFail, desc.backPass);
    impl->setStencilMask(desc.writeMask);
}

void RenderContext::setBlendState(const BlendModeDesc &blendModeDesc, const ColorMask &colorMask) {
    impl->setBlendEnabled(blendModeDesc.blendEnable);
    impl->setBlendFunc(blendModeDesc.srcBlend, blendModeDesc.srcBlendAlpha, blendModeDesc.dstBlend, blendModeDesc.dstBlendAlpha);
    impl->setBlendOp(blendModeDesc.blendOp, blendModeDesc.blendOpAlpha);
    impl->setBlendColorMask(colorMask);
}

void RenderContext::setBlendState(BlendMode blendMode, const ColorMask &colorMask) {
    setBlendState(gBlendModeDescs[static_cast<uint32_t>(blendMode)], colorMask);
}

void RenderContext::setRasterizerState(CullFace cullFace, FillMode fillMode, FrontFace frontFace, const PolygonOffset &polygonOffset) {
    impl->setRastCullFace(cullFace);
    impl->setRastFillMode(fillMode);
    impl->setRastFrontFace(frontFace);
    impl->setRastPolygonOffset(polygonOffset);
}

void RenderContext::setViewport(int x, int y, int w, int h) {
    impl->setViewport(x, y, w, h);
}

void RenderContext::setViewport(const glm::ivec2 &pos, const glm::ivec2 &size) {
    setViewport(pos.x, pos.y, size.x, size.y);
}

HVertexFormat RenderContext::createVertexFormat(const std::vector<VertexElement> &desc) {
    HD_ASSERT(!desc.empty());
    auto obj = new VertexFormatImpl();
    impl->vertexFormats.push_back(HVertexFormat(obj));
    obj->elements = desc;
    glGenVertexArrays(1, &obj->id);
    impl->bindVertexFormat(obj->id);
    for (const auto &it : desc) {
        glEnableVertexAttribArray(it.index);
        glVertexAttribBinding(it.index, it.slot);
        glVertexAttribFormat(it.index, gVertexElementSizes[static_cast<size_t>(it.type)], gVertexElementTypes[static_cast<size_t>(it.type)], it.normalized, it.offset);
        glVertexBindingDivisor(it.slot, it.perInstance);
    }
    return impl->vertexFormats.back();
}

void RenderContext::destroyVertexFormat(HVertexFormat &handle) {
    if (handle) {
        if (impl->currentVertexFormat == handle->id) {
            impl->currentVertexFormat = 0;
        }
        glDeleteVertexArrays(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindVertexFormat(const HVertexFormat &handle) {
    HD_ASSERT(handle);
    impl->bindVertexFormat(handle->id);
}

HVertexBuffer RenderContext::createVertexBuffer(const void *data, uint32_t size, BufferUsage usage) {
    HD_ASSERT(size != 0);
    auto obj = new VertexBufferImpl();
    impl->vertexBuffers.push_back(HVertexBuffer(obj));
    obj->size = size;
    obj->usage = usage;
    glGenBuffers(1, &obj->id);
    setVertexBufferData(impl->vertexBuffers.back(), data, size);
    return impl->vertexBuffers.back();
}

void RenderContext::destroyVertexBuffer(HVertexBuffer &handle) {
    if (handle) {
        for (auto &it : impl->currentVertexBuffer) {
            if (it.id == handle->id) {
                it.id = it.offset = it.stride = 0;
            }
        }
        if (impl->currentVBO == handle->id) {
            impl->currentVBO = 0;
        }
        glDeleteBuffers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::setVertexBufferData(HVertexBuffer &handle, const void *data, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    handle->size = size;
    impl->bindVBO(handle->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, gBufferUsages[static_cast<size_t>(handle->usage)]);
}

void RenderContext::updateVertexBufferData(HVertexBuffer &handle, const void *data) {
    HD_ASSERT(handle);
    impl->bindVBO(handle->id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, handle->size, data);
}

void RenderContext::updateVertexBufferData(HVertexBuffer &handle, const void *data, uint32_t offset, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    HD_ASSERT((offset + size) < handle->size);
    impl->bindVBO(handle->id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void *RenderContext::mapVertexBuffer(HVertexBuffer &handle, BufferAccess access) {
    HD_ASSERT(handle);
    impl->bindVBO(handle->id);
    return glMapBuffer(GL_ARRAY_BUFFER, gBufferAccesses[static_cast<size_t>(access)]);
}

void RenderContext::unmapVertexBuffer(HVertexBuffer &handle) {
    HD_ASSERT(handle);
    impl->bindVBO(handle->id);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void RenderContext::bindVertexBuffer(const HVertexBuffer &handle, uint32_t slot, uint32_t offset, uint32_t stride) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_VERTEX_BUFFERS);
    HD_ASSERT(stride != 0);
    impl->bindVertexBuffer(handle->id, slot, offset, stride);
}

HIndexBuffer RenderContext::createIndexBuffer(const void *data, uint32_t size, BufferUsage usage) {
    HD_ASSERT(size != 0);
    auto obj = new IndexBufferImpl();
    impl->indexBuffers.push_back(HIndexBuffer(obj));
    obj->size = size;
    obj->usage = usage;
    glGenBuffers(1, &obj->id);
    setIndexBufferData(impl->indexBuffers.back(), data, size);
    return impl->indexBuffers.back();
}

void RenderContext::destroyIndexBuffer(HIndexBuffer &handle) {
    if (handle) {
        if (impl->currentIndexBuffer == handle->id) {
            impl->currentIndexBuffer = 0;
        }
        glDeleteBuffers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::setIndexBufferData(HIndexBuffer &handle, const void *data, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    handle->size = size;
    impl->bindIndexBuffer(handle->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, gBufferUsages[static_cast<size_t>(handle->usage)]);
}

void RenderContext::updateIndexBufferData(HIndexBuffer &handle, const void *data) {
    HD_ASSERT(handle);
    impl->bindIndexBuffer(handle->id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, handle->size, data);
}

void RenderContext::updateIndexBufferData(HIndexBuffer &handle, const void *data, uint32_t offset, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    HD_ASSERT((offset + size) < handle->size);
    impl->bindIndexBuffer(handle->id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void *RenderContext::mapIndexBuffer(HIndexBuffer &handle, BufferAccess access) {
    HD_ASSERT(handle);
    impl->bindIndexBuffer(handle->id);
    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, gBufferAccesses[static_cast<size_t>(access)]);
}

void RenderContext::unmapIndexBuffer(HIndexBuffer &handle) {
    HD_ASSERT(handle);
    impl->bindIndexBuffer(handle->id);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void RenderContext::bindIndexBuffer(const HIndexBuffer &handle) {
    HD_ASSERT(handle);
    impl->bindIndexBuffer(handle->id);
}

HConstantBuffer RenderContext::createConstantBuffer(const void *data, uint32_t size, BufferUsage usage) {
    HD_ASSERT(size != 0);
    auto obj = new ConstantBufferImpl();
    impl->constantBuffers.push_back(HConstantBuffer(obj));
    obj->size = size;
    obj->usage = usage;
    glGenBuffers(1, &obj->id);
    setConstantBufferData(impl->constantBuffers.back(), data, size);
    return impl->constantBuffers.back();
}

void RenderContext::destroyConstantBuffer(HConstantBuffer &handle) {
    if (handle) {
        for (auto &it : impl->currentConstantBuffer) {
            if (it == handle->id) {
                it = 0;
            }
        }
        if (impl->currentUBO == handle->id) {
            impl->currentUBO = 0;
        }
        glDeleteBuffers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::setConstantBufferData(HConstantBuffer &handle, const void *data, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    handle->size = size;
    impl->bindUBO(handle->id);
    glBufferData(GL_UNIFORM_BUFFER, size, data, gBufferUsages[static_cast<size_t>(handle->usage)]);
}

void RenderContext::updateConstantBufferData(HConstantBuffer &handle, const void *data) {
    HD_ASSERT(handle);
    impl->bindUBO(handle->id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, handle->size, data);
}

void RenderContext::updateConstantBufferData(HConstantBuffer &handle, const void *data, uint32_t offset, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    HD_ASSERT((offset + size) < handle->size);
    impl->bindUBO(handle->id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void *RenderContext::mapConstantBuffer(HConstantBuffer &handle, BufferAccess access) {
    HD_ASSERT(handle);
    impl->bindUBO(handle->id);
    return glMapBuffer(GL_UNIFORM_BUFFER, gBufferAccesses[static_cast<size_t>(access)]);
}

void RenderContext::unmapConstantBuffer(HConstantBuffer &handle) {
    HD_ASSERT(handle);
    impl->bindUBO(handle->id);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void RenderContext::bindConstantBuffer(const HConstantBuffer &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_CONSTANT_BUFFERS);
    impl->bindConstantBuffer(handle->id, slot);
}

HTexture2D RenderContext::createTexture2D(const void *data, uint32_t w, uint32_t h, TextureFormat format) {
    HD_ASSERT(w != 0 && h != 0);
    auto obj = new Texture2DImpl();
    impl->textures2D.push_back(HTexture2D(obj));
    obj->format = format;
    obj->size = glm::ivec2(w, h);
    glGenTextures(1, &obj->id);
    impl->bindTexture2D(obj->id, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, gTextureInternalFormats[static_cast<size_t>(format)], w, h, 0, gTextureExternalFormats[static_cast<size_t>(format)], gTextureDataTypes[static_cast<size_t>(format)], data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return impl->textures2D.back();
}

HTexture2D RenderContext::createTexture2D(const void *data, const glm::ivec2 &size, TextureFormat format) {
    return createTexture2D(data, size.x, size.y, format);
}

HTexture2D RenderContext::createTexture2DFromStream(StreamReader &stream) {
    auto img = Image(stream);
    return createTexture2D(static_cast<const void*>(img.getPixels()), img.getWidth(), img.getHeight(), TextureFormat::RGBA8);
}

HTexture2D RenderContext::createTexture2DFromFile(const std::string &filename) {
    auto fs = FileReader(filename);
    fs.setName(filename);
    return createTexture2DFromStream(fs);
}

void RenderContext::destroyTexture2D(HTexture2D &handle) {
    if (handle) {
        for (auto &it : impl->currentTexture2D) {
            if (it == handle->id) {
                it = 0;
            }
        }
        glDeleteTextures(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindTexture2D(const HTexture2D &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_TEXTURES);
    impl->bindTexture2D(handle->id, slot);
}

HSamplerState RenderContext::createSamplerState(SamplerFilter filter, uint32_t maxAnisotropy) {
    return createSamplerState(filter, maxAnisotropy, CompareFunc::Less, false, SamplerAddressMode::Repeat, SamplerAddressMode::Repeat, SamplerAddressMode::Repeat, 0, nullptr, -1000, 1000);
}

HSamplerState RenderContext::createSamplerState(SamplerFilter filter, uint32_t maxAnisotropy, CompareFunc compareFunc, bool compareRefToTex) {
    return createSamplerState(filter, maxAnisotropy, compareFunc, compareRefToTex, SamplerAddressMode::Repeat, SamplerAddressMode::Repeat, SamplerAddressMode::Repeat, 0, nullptr, -1000, 1000);
}

HSamplerState RenderContext::createSamplerState(SamplerFilter filter, uint32_t maxAnisotropy, CompareFunc compareFunc, bool compareRefToTex, SamplerAddressMode uvw) {
    return createSamplerState(filter, maxAnisotropy, compareFunc, compareRefToTex, uvw, uvw, uvw, 0, nullptr, -1000, 1000);
}

HSamplerState RenderContext::createSamplerState(SamplerFilter filter, uint32_t maxAnisotropy, CompareFunc compareFunc, bool compareRefToTex, SamplerAddressMode uvw, float lodBias, const float *borderColor, float minLod, float maxLod) {
    return createSamplerState(filter, maxAnisotropy, compareFunc, compareRefToTex, uvw, uvw, uvw, lodBias, borderColor, minLod, maxLod);
}

HSamplerState RenderContext::createSamplerState(SamplerFilter filter, uint32_t maxAnisotropy, CompareFunc compareFunc, bool compareRefToTex, SamplerAddressMode u, SamplerAddressMode v, SamplerAddressMode w, float lodBias, const float *borderColor, float minLod, float maxLod) {
    auto obj = new SamplerStateImpl();
    impl->samplerStates.push_back(HSamplerState(obj));
    obj->filter = filter;
    obj->maxAnisotropy = maxAnisotropy;
    obj->compareFunc = compareFunc;
    obj->compareRefToTexture = compareRefToTex;
    obj->u = u;
    obj->v = v;
    obj->w = w;
    obj->lodBias = lodBias;
    if (borderColor) {
        memcpy(obj->borderColor, borderColor, sizeof(obj->borderColor));
    }
    else {
        memset(obj->borderColor, 0, sizeof(obj->borderColor));
    }
    obj->minLod = minLod;
    obj->maxLod = maxLod;
    glGenSamplers(1, &obj->id);
    glSamplerParameteri(obj->id, GL_TEXTURE_MIN_FILTER, gSamplerMinFilter[static_cast<size_t>(filter)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_MAG_FILTER, gSamplerMagFilter[static_cast<size_t>(filter)]);
    if (filter == SamplerFilter::Anisotropic) {
        glSamplerParameteri(obj->id, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
    }
    glSamplerParameteri(obj->id, GL_TEXTURE_COMPARE_FUNC, gCompareFuncs[static_cast<GLenum>(compareFunc)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_COMPARE_MODE, compareRefToTex ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
    glSamplerParameteri(obj->id, GL_TEXTURE_WRAP_S, gSamplerAddressModes[static_cast<size_t>(u)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_WRAP_T, gSamplerAddressModes[static_cast<size_t>(v)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_WRAP_R, gSamplerAddressModes[static_cast<size_t>(w)]);
    glSamplerParameterf(obj->id, GL_TEXTURE_LOD_BIAS, lodBias);
    glSamplerParameterfv(obj->id, GL_TEXTURE_BORDER_COLOR, borderColor);
    glSamplerParameterf(obj->id, GL_TEXTURE_MIN_LOD, minLod);
    glSamplerParameterf(obj->id, GL_TEXTURE_MAX_LOD, maxLod);
    return impl->samplerStates.back();
}

void RenderContext::destroySamplerState(HSamplerState &handle) {
    if (handle) {
        for (auto &it : impl->currentSamplerState) {
            if (it == handle->id) {
                it = 0;
            }
        }
        glDeleteSamplers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindSamplerState(const HSamplerState &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_SAMPLER_STATES);
    impl->bindSamplerState(handle->id, slot);
}

HProgram RenderContext::createProgram(const std::string &name, const std::string &vsCode, const std::string &psCode, const std::string &defines) {
    HD_ASSERT(!vsCode.empty());
    HD_ASSERT(!psCode.empty());
    auto obj = new ProgramImpl();
    impl->programs.push_back(HProgram(obj));
    obj->name = name;
    obj->vsCode = vsCode;
    obj->psCode = psCode;
    obj->defines = defines;
    obj->id = glCreateProgram();

    auto result = 0, logLength = 0;
    auto version = impl->settings.majorVersion*100 + impl->settings.minorVersion*10;
    if (version < 300) {
        version -= 90;
    }
    else if (version < 330) {
        version -= 170;
    }
    auto versionStr = StringUtils::format("#version %d%s\n", version, impl->settings.isCoreProfile ? " core" : "");
    std::string definesStr;
    if (!defines.empty()) {
        for (const auto &it : StringUtils::split(defines, ";", false)) {
            definesStr += "#define" + it + "\n";
        }
    }

    auto vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vsSources[] = { versionStr.data(), definesStr.data(), "#line 1\n", vsCode.data() };
    int vsSourcesLengths[] = {  static_cast<int>(versionStr.length()), static_cast<int>(definesStr.length()), static_cast<int>(strlen("#line 1\n")), static_cast<int>(vsCode.length()) };
    glShaderSource(vs, HD_ARRAYSIZE(vsSourcesLengths), vsSources, vsSourcesLengths);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLength);
        auto errorStr = std::vector<char>(static_cast<size_t>(logLength));
        glGetShaderInfoLog(vs, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to compile vertex shader for program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    uint32_t ps = glCreateShader(GL_FRAGMENT_SHADER);
    const char *psSources[] = { versionStr.data(), definesStr.data(), "#line 1\n", psCode.data() };
    int psSourcesLengths[] = {  static_cast<int>(versionStr.length()), static_cast<int>(definesStr.length()), static_cast<int>(strlen("#line 1\n")), static_cast<int>(psCode.length()) };
    glShaderSource(ps, HD_ARRAYSIZE(psSourcesLengths), psSources, psSourcesLengths);
    glCompileShader(ps);
    glGetShaderiv(ps, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &logLength);
        auto errorStr = std::vector<char>(static_cast<size_t>(logLength));
        glGetShaderInfoLog(ps, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to compile pixel shader for program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    glAttachShader(obj->id, vs);
    glAttachShader(obj->id, ps);
    glLinkProgram(obj->id);
    glGetProgramiv(obj->id, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramiv(obj->id, GL_INFO_LOG_LENGTH, &logLength);
        auto errorStr = std::vector<char>(static_cast<size_t>(logLength));
        glGetProgramInfoLog(obj->id, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to link program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    glValidateProgram(obj->id);
    glGetProgramiv(obj->id, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramiv(obj->id, GL_INFO_LOG_LENGTH, &logLength);
        auto errorStr = std::vector<char>(static_cast<size_t>(logLength));
        glGetProgramInfoLog(obj->id, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to link program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    glDetachShader(obj->id, vs);
    glDeleteShader(vs);
    glDetachShader(obj->id, ps);
    glDeleteShader(ps);
    return impl->programs.back();
}

HProgram RenderContext::createProgramFromStream(StreamReader &stream, const std::string &defines) {
    auto text = stream.readAllLines();
    std::string code[2];
    auto shaderType = 0; // 0 - vs, 1 - ps;
    for (const auto &it : text) {
        if (StringUtils::contains(it, "@VertexShader", false)) {
            shaderType = 0;
        }
        else if (StringUtils::contains(it, "@PixelShader", false)) {
            shaderType = 1;
        }
        else {
            code[shaderType] += it + "\n";
        }
    }
    return createProgram(stream.getName(), code[0], code[1], defines);
}

HProgram RenderContext::createProgramFromFile(const std::string &filename, const std::string &defines) {
    auto fs = FileReader(filename);
    fs.setName(filename);
    return createProgramFromStream(fs, defines);
}

void RenderContext::destroyProgram(HProgram &handle) {
    if (handle) {
        if (impl->currentProgram == handle->id) {
            impl->currentProgram = 0;
        }
        glDeleteProgram(handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

HProgramConstant RenderContext::getProgramConstantID(const HProgram &handle, const std::string &name) {
    HD_ASSERT(handle);
    HD_ASSERT(!name.empty());
    return HProgramConstant(glGetUniformLocation(handle->id, name.data()));
}

void RenderContext::bindProgram(const HProgram &handle) {
    HD_ASSERT(handle);
    impl->bindProgram(handle->id);
}

void RenderContext::setProgramConstant(HProgramConstant &id, int value) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform1i(id.value, value);
}

void RenderContext::setProgramConstant(HProgramConstant &id, float value) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform1f(id.value, value);
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::vec2 &value) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform2f(id.value, value.x, value.y);
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::vec3 &value) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform3f(id.value, value.x, value.y, value.z);
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::vec4 &value) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform4f(id.value, value.x, value.y, value.z, value.w);
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::mat4 &value) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniformMatrix4fv(id.value, 1, false, reinterpret_cast<const float*>(&value));
}

void RenderContext::setProgramConstant(HProgramConstant &id, const int *value, uint32_t count) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform1iv(id.value, count, value);
}

void RenderContext::setProgramConstant(HProgramConstant &id, const float *value, uint32_t count) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform1fv(id.value, count, value);
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::vec2 *value, uint32_t count) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform2fv(id.value, count, reinterpret_cast<const float*>(value));
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::vec3 *value, uint32_t count) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform3fv(id.value, count, reinterpret_cast<const float*>(value));
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::vec4 *value, uint32_t count) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniform4fv(id.value, count, reinterpret_cast<const float*>(value));
}

void RenderContext::setProgramConstant(HProgramConstant &id, const glm::mat4 *value, uint32_t count) {
    HD_ASSERT(id);
    HD_ASSERT(impl->currentProgram != 0);
    glUniformMatrix4fv(id.value, count, false, reinterpret_cast<const float*>(value));
}

}
