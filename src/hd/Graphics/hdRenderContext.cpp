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

void samplerFilterToGL(SamplerFilter filter, GLenum &glFilter, bool &glIsAnisotropic, bool &glIsComparison) {
    switch(filter) {
        case SamplerFilter::Point: {
            glIsAnisotropic = false;
            glIsComparison = false;
            glFilter = GL_NEAREST;
            break;
        }
        case SamplerFilter::Linear: {
            glIsAnisotropic = false;
            glIsComparison = false;
            glFilter = GL_LINEAR;
            break;
        }
        case SamplerFilter::Anisotropic: {
            glIsAnisotropic = true;
            glIsComparison = false;
            glFilter = GL_LINEAR;
            break;
        }
        case SamplerFilter::ComparisonPoint: {
            glIsAnisotropic = false;
            glIsComparison = true;
            glFilter = GL_NEAREST;
            break;
        }
        case SamplerFilter::ComparisonLinear: {
            glIsAnisotropic = false;
            glIsComparison = true;
            glFilter = GL_LINEAR;
            break;
        }
        case SamplerFilter::ComparisonAnisotropic: {
            glIsAnisotropic = true;
            glIsComparison = true;
            glFilter = GL_LINEAR;
            break;
        }
    }
}

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

SamplerStateDesc::SamplerStateDesc() : borderColor{0.0f, 0.0f, 0.0f, 0.0f} {
    this->minFilter = SamplerFilter::Linear;
    this->magFilter = SamplerFilter::Linear;
    this->mipFilter = SamplerFilter::Linear;
    this->u = SamplerAddressMode::Clamp;
    this->v = SamplerAddressMode::Clamp;
    this->w = SamplerAddressMode::Clamp;
    this->maxAnisotropy = 0;
    this->mipLodBias = 0.0f;
    this->compareFunc = CompareFunc::Never;
    this->minLod = 0.0f;
    this->maxLod = FLT_MAX;
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

struct Texture2DArrayImpl {
    uint32_t id;
    TextureFormat format;
    glm::ivec2 size;
    uint32_t layers;
};

struct SamplerStateImpl {
    uint32_t id;
    SamplerStateDesc desc;
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
    std::vector<HTexture2DArray> texture2DArrays;
    std::vector<HSamplerState> samplerStates;
    std::vector<HProgram> programs;
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
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        HD_LOG_ERROR("Failed to initialize GLEW. Errors: %s", glewGetErrorString(error));
    }

    if (window.getOpenGLContextSettings().isDebug) {
        if (GLEW_ARB_debug_output) {
            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback(debugCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
        }
        else {
            HD_LOG_WARNING("Failed to enable OpenGL debug output. Extension 'GLEW_ARB_debug_output' not supported");
        }
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
    for (auto &it : impl->texture2DArrays) {
        destroyTexture2DArray(it);
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
    if (depthTestEnabled) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    glDepthFunc(gCompareFuncs[static_cast<size_t>(compareFunc)]);
    glDepthMask(depthMask);
}

void RenderContext::setStencilState(bool enabled, const StencilTestDesc &desc, int ref) {
    if (enabled) {
        glEnable(GL_STENCIL_TEST);
    }
    else {
        glDisable(GL_STENCIL_TEST);
    }
    glStencilFuncSeparate(GL_FRONT, gCompareFuncs[static_cast<size_t>(desc.frontFunc)], ref, desc.readMask);
    glStencilFuncSeparate(GL_BACK, gCompareFuncs[static_cast<size_t>(desc.backFunc)], ref, desc.readMask);
    glStencilOpSeparate(GL_FRONT, gStencilOps[static_cast<size_t>(desc.frontFail)], gStencilOps[static_cast<size_t>(desc.frontDepthFail)], gStencilOps[static_cast<size_t>(desc.frontPass)]);
    glStencilOpSeparate(GL_BACK, gStencilOps[static_cast<size_t>(desc.backFail)], gStencilOps[static_cast<size_t>(desc.backDepthFail)], gStencilOps[static_cast<size_t>(desc.backPass)]);
    glStencilMask(desc.writeMask);
}

void RenderContext::setBlendState(const BlendModeDesc &blendModeDesc, const ColorMask &colorMask) {
    if (blendModeDesc.blendEnable) {
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }
    glBlendFuncSeparate(gBlendFactors[static_cast<size_t>(blendModeDesc.srcBlend)], gBlendFactors[static_cast<size_t>(blendModeDesc.dstBlend)],
        gBlendFactors[static_cast<size_t>(blendModeDesc.srcBlendAlpha)], gBlendFactors[static_cast<size_t>(blendModeDesc.dstBlendAlpha)]);
    glBlendEquationSeparate(gBlendOps[static_cast<size_t>(blendModeDesc.blendOp)], gBlendOps[static_cast<size_t>(blendModeDesc.blendOpAlpha)]);
    glColorMask(colorMask.r, colorMask.g, colorMask.b, colorMask.a);
}

void RenderContext::setBlendState(BlendMode blendMode, const ColorMask &colorMask) {
    setBlendState(gBlendModeDescs[static_cast<uint32_t>(blendMode)], colorMask);
}

void RenderContext::setRasterizerState(CullFace cullFace, FillMode fillMode, FrontFace frontFace, const PolygonOffset &polygonOffset) {
    if (cullFace == CullFace::None) {
        glDisable(GL_CULL_FACE);
    }
    else {
        glEnable(GL_CULL_FACE);
        glCullFace(gCullFaces[static_cast<size_t>(cullFace)]);
    }
    glPolygonMode(GL_FRONT_AND_BACK, gFillModes[static_cast<size_t>(fillMode)]);
    glFrontFace(gFrontFaces[static_cast<size_t>(frontFace)]);
    if (polygonOffset.enabled) {
        glEnable(GL_POLYGON_OFFSET_FILL);
    }
    else {
        glDisable(GL_POLYGON_OFFSET_FILL);
    }
    glPolygonOffset(polygonOffset.factor, polygonOffset.units);
}

void RenderContext::setViewport(int x, int y, int w, int h) {
    glViewport(x, y, w, h);
}

void RenderContext::setViewport(const glm::ivec2 &pos, const glm::ivec2 &size) {
    glViewport(pos.x, pos.y, size.x, size.y);
}

HVertexFormat RenderContext::createVertexFormat(const std::vector<VertexElement> &desc) {
    HD_ASSERT(!desc.empty());
    VertexFormatImpl *obj = new VertexFormatImpl();
    impl->vertexFormats.push_back(HVertexFormat(obj));
    obj->elements = desc;
    glGenVertexArrays(1, &obj->id);
    glBindVertexArray(obj->id);
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
        impl->vertexFormats.erase(std::remove(impl->vertexFormats.begin(), impl->vertexFormats.end(), handle), impl->vertexFormats.end());
        glDeleteVertexArrays(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindVertexFormat(const HVertexFormat &handle) {
    HD_ASSERT(handle);
    glBindVertexArray(handle->id);
}

HVertexBuffer RenderContext::createVertexBuffer(const void *data, uint32_t size, BufferUsage usage) {
    HD_ASSERT(size != 0);
    VertexBufferImpl *obj = new VertexBufferImpl();
    impl->vertexBuffers.push_back(HVertexBuffer(obj));
    obj->size = size;
    obj->usage = usage;
    glGenBuffers(1, &obj->id);
    setVertexBufferData(impl->vertexBuffers.back(), data, size);
    return impl->vertexBuffers.back();
}

void RenderContext::destroyVertexBuffer(HVertexBuffer &handle) {
    if (handle) {
        impl->vertexBuffers.erase(std::remove(impl->vertexBuffers.begin(), impl->vertexBuffers.end(), handle), impl->vertexBuffers.end());
        glDeleteBuffers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::setVertexBufferData(HVertexBuffer &handle, const void *data, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    handle->size = size;
    glBindBuffer(GL_ARRAY_BUFFER, handle->id);
    glBufferData(GL_ARRAY_BUFFER, size, data, gBufferUsages[static_cast<size_t>(handle->usage)]);
}

void RenderContext::updateVertexBufferData(HVertexBuffer &handle, const void *data) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle->id);
    glBufferSubData(GL_ARRAY_BUFFER, 0, handle->size, data);
}

void RenderContext::updateVertexBufferData(HVertexBuffer &handle, const void *data, uint32_t offset, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    HD_ASSERT((offset + size) <= handle->size);
    glBindBuffer(GL_ARRAY_BUFFER, handle->id);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

void *RenderContext::mapVertexBuffer(HVertexBuffer &handle, BufferAccess access) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle->id);
    return glMapBuffer(GL_ARRAY_BUFFER, gBufferAccesses[static_cast<size_t>(access)]);
}

void RenderContext::unmapVertexBuffer(HVertexBuffer &handle) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle->id);
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void RenderContext::bindVertexBuffer(const HVertexBuffer &handle, uint32_t slot, uint32_t offset, uint32_t stride) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_VERTEX_BUFFERS);
    HD_ASSERT(stride != 0);
    glBindVertexBuffer(slot, handle->id, offset, stride);
}

HIndexBuffer RenderContext::createIndexBuffer(const void *data, uint32_t size, BufferUsage usage) {
    HD_ASSERT(size != 0);
    IndexBufferImpl *obj = new IndexBufferImpl();
    impl->indexBuffers.push_back(HIndexBuffer(obj));
    obj->size = size;
    obj->usage = usage;
    glGenBuffers(1, &obj->id);
    setIndexBufferData(impl->indexBuffers.back(), data, size);
    return impl->indexBuffers.back();
}

void RenderContext::destroyIndexBuffer(HIndexBuffer &handle) {
    if (handle) {
        impl->indexBuffers.erase(std::remove(impl->indexBuffers.begin(), impl->indexBuffers.end(), handle), impl->indexBuffers.end());
        glDeleteBuffers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::setIndexBufferData(HIndexBuffer &handle, const void *data, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    handle->size = size;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, gBufferUsages[static_cast<size_t>(handle->usage)]);
}

void RenderContext::updateIndexBufferData(HIndexBuffer &handle, const void *data) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, handle->size, data);
}

void RenderContext::updateIndexBufferData(HIndexBuffer &handle, const void *data, uint32_t offset, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    HD_ASSERT((offset + size) <= handle->size);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->id);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
}

void *RenderContext::mapIndexBuffer(HIndexBuffer &handle, BufferAccess access) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->id);
    return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, gBufferAccesses[static_cast<size_t>(access)]);
}

void RenderContext::unmapIndexBuffer(HIndexBuffer &handle) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->id);
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void RenderContext::bindIndexBuffer(const HIndexBuffer &handle) {
    HD_ASSERT(handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle->id);
}

HConstantBuffer RenderContext::createConstantBuffer(const void *data, uint32_t size, BufferUsage usage) {
    HD_ASSERT(size != 0);
    ConstantBufferImpl *obj = new ConstantBufferImpl();
    impl->constantBuffers.push_back(HConstantBuffer(obj));
    obj->size = size;
    obj->usage = usage;
    glGenBuffers(1, &obj->id);
    setConstantBufferData(impl->constantBuffers.back(), data, size);
    return impl->constantBuffers.back();
}

void RenderContext::destroyConstantBuffer(HConstantBuffer &handle) {
    if (handle) {
        impl->constantBuffers.erase(std::remove(impl->constantBuffers.begin(), impl->constantBuffers.end(), handle), impl->constantBuffers.end());
        glDeleteBuffers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::setConstantBufferData(HConstantBuffer &handle, const void *data, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    handle->size = size;
    glBindBuffer(GL_UNIFORM_BUFFER, handle->id);
    glBufferData(GL_UNIFORM_BUFFER, size, data, gBufferUsages[static_cast<size_t>(handle->usage)]);
}

void RenderContext::updateConstantBufferData(HConstantBuffer &handle, const void *data) {
    HD_ASSERT(handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle->id);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, handle->size, data);
}

void RenderContext::updateConstantBufferData(HConstantBuffer &handle, const void *data, uint32_t offset, uint32_t size) {
    HD_ASSERT(handle);
    HD_ASSERT(size != 0);
    HD_ASSERT((offset + size) <= handle->size);
    glBindBuffer(GL_UNIFORM_BUFFER, handle->id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void *RenderContext::mapConstantBuffer(HConstantBuffer &handle, BufferAccess access) {
    HD_ASSERT(handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle->id);
    return glMapBuffer(GL_UNIFORM_BUFFER, gBufferAccesses[static_cast<size_t>(access)]);
}

void RenderContext::unmapConstantBuffer(HConstantBuffer &handle) {
    HD_ASSERT(handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle->id);
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void RenderContext::bindConstantBuffer(const HConstantBuffer &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_CONSTANT_BUFFERS);
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, handle->id);
}

HTexture2D RenderContext::createTexture2D(const void *data, uint32_t w, uint32_t h, TextureFormat format) {
    HD_ASSERT(w != 0 && h != 0);
    Texture2DImpl *obj = new Texture2DImpl();
    impl->textures2D.push_back(HTexture2D(obj));
    obj->format = format;
    obj->size = glm::ivec2(w, h);
    glGenTextures(1, &obj->id);
    glBindTexture(GL_TEXTURE_2D, obj->id);
    glTexImage2D(GL_TEXTURE_2D, 0, gTextureInternalFormats[static_cast<size_t>(format)], w, h, 0, gTextureExternalFormats[static_cast<size_t>(format)], gTextureDataTypes[static_cast<size_t>(format)], data);
    glGenerateMipmap(GL_TEXTURE_2D);
    return impl->textures2D.back();
}

HTexture2D RenderContext::createTexture2D(const void *data, const glm::ivec2 &size, TextureFormat format) {
    return createTexture2D(data, size.x, size.y, format);
}

HTexture2D RenderContext::createTexture2DFromStream(StreamReader &stream) {
    Image img(stream);
    return createTexture2D(static_cast<const void*>(img.getPixels()), img.getWidth(), img.getHeight(), TextureFormat::RGBA8);
}

HTexture2D RenderContext::createTexture2DFromFile(const std::string &filename) {
    FileReader fs(filename);
    fs.setName(filename);
    return createTexture2DFromStream(fs);
}

void RenderContext::destroyTexture2D(HTexture2D &handle) {
    if (handle) {
        impl->textures2D.erase(std::remove(impl->textures2D.begin(), impl->textures2D.end(), handle), impl->textures2D.end());
        glDeleteTextures(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindTexture2D(const HTexture2D &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_TEXTURES);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, handle->id);
}

HTexture2DArray RenderContext::createTexture2DArray(const void *data, uint32_t w, uint32_t h, TextureFormat format, uint32_t layers) {
    HD_ASSERT(w != 0 && h != 0);
    Texture2DArrayImpl *obj = new Texture2DArrayImpl();
    impl->texture2DArrays.push_back(HTexture2DArray(obj));
    obj->format = format;
    obj->size = glm::ivec2(w, h);
    glGenTextures(1, &obj->id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, obj->id);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, gTextureInternalFormats[static_cast<size_t>(format)], w, h, layers, 0, gTextureExternalFormats[static_cast<size_t>(format)], gTextureDataTypes[static_cast<size_t>(format)], data);
    return impl->texture2DArrays.back();
}

HTexture2DArray RenderContext::createTexture2DArray(const void *data, const glm::ivec2 &size, TextureFormat format, uint32_t layers) {
    return createTexture2DArray(data, size.x, size.y, format, layers);
}

HTexture2DArray RenderContext::createTexture2DArrayFromFiles(const std::vector<std::string> &filenames) {
    HD_ASSERT(!filenames.empty());
    std::vector<hd::Image> images;
    images.reserve(filenames.size());
    for (const auto &filename : filenames) {
        images.emplace_back(filename);
        if (images.size() > 1 && images[0].getWidth() != images[1].getWidth() && images[0].getHeight() != images[1].getHeight()) {
            HD_LOG_ERROR("Failed to create Texture2DArray from files:\n%s\nThe files has a different image sizes", hd::StringUtils::fromVector(filenames, "'", "'", "\n"));
        }
    }
    HTexture2DArray handle = createTexture2DArray(nullptr, images.front().getWidth(), images.front().getHeight(), hd::TextureFormat::RGBA8, static_cast<uint32_t>(filenames.size()));
    for (size_t i = 0; i < images.size(); i++) {
        setTexture2DArrayLayerData(handle, static_cast<uint32_t>(i), images[i].getPixels(), hd::TextureFormat::RGBA8);
    }
    return handle;
}

void RenderContext::setTexture2DArrayLayerData(const HTexture2DArray &handle, uint32_t layer, const void *data, TextureFormat format) {
   HD_ASSERT(handle);
   glBindTexture(GL_TEXTURE_2D_ARRAY, handle->id);
   glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, handle->size.x, handle->size.y, 1, gTextureExternalFormats[static_cast<size_t>(format)], gTextureDataTypes[static_cast<size_t>(format)], data);
   glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void RenderContext::setTexture2DArrayLayerData(const HTexture2DArray &handle, uint32_t layer, StreamReader &stream) {
    Image img(stream);
    setTexture2DArrayLayerData(handle, layer, img.getPixels(), TextureFormat::RGBA8);
}

void RenderContext::setTexture2DArrayLayerData(const HTexture2DArray &handle, uint32_t layer, const std::string &filename) {
    FileReader fs(filename);
    fs.setName(filename);
    setTexture2DArrayLayerData(handle, layer, fs);
}

void RenderContext::destroyTexture2DArray(HTexture2DArray &handle) {
    if (handle) {
        impl->texture2DArrays.erase(std::remove(impl->texture2DArrays.begin(), impl->texture2DArrays.end(), handle), impl->texture2DArrays.end());
        glDeleteTextures(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindTexture2DArray(const HTexture2DArray &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_TEXTURES);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, handle->id);
}

HSamplerState RenderContext::createSamplerState(const SamplerStateDesc& desc) {
    SamplerStateImpl *obj = new SamplerStateImpl();
    impl->samplerStates.push_back(HSamplerState(obj));
    obj->desc = desc;

    bool isMinAniso = false, isMagAniso = false, isMipAniso = false;
    bool isMinComp = false, isMagComp = false, isMipComp = false;
    GLenum minFilter, magFilter, mipFilter;
    samplerFilterToGL(desc.minFilter, minFilter, isMinAniso, isMinComp);
    samplerFilterToGL(desc.magFilter, magFilter, isMagAniso, isMagComp);
    samplerFilterToGL(desc.mipFilter, mipFilter, isMipAniso, isMipComp);
    HD_ASSERT(isMinAniso == isMagAniso && isMagAniso == isMinAniso);
    HD_ASSERT(isMinComp == isMagComp && isMagComp == isMipComp);

    GLenum minMipFilter = 0;
    if(minFilter == GL_NEAREST && mipFilter == GL_NEAREST) {
        minMipFilter = GL_NEAREST_MIPMAP_NEAREST;
    }
    else if(minFilter == GL_LINEAR && mipFilter == GL_NEAREST) {
        minMipFilter = GL_LINEAR_MIPMAP_NEAREST;
    }
    else if(minFilter == GL_NEAREST && mipFilter == GL_LINEAR) {
        minMipFilter = GL_NEAREST_MIPMAP_LINEAR;
    }
    else if(minFilter == GL_LINEAR && mipFilter == GL_LINEAR) {
        minMipFilter = GL_LINEAR_MIPMAP_LINEAR;
    }
    else {
        HD_LOG_ERROR("Unsupported min/mip filter combination");
    }

    glGenSamplers(1, &obj->id);
    glSamplerParameteri(obj->id, GL_TEXTURE_MIN_FILTER, minMipFilter);
    glSamplerParameteri(obj->id, GL_TEXTURE_MAG_FILTER, magFilter);
    glSamplerParameteri(obj->id, GL_TEXTURE_MAX_ANISOTROPY_EXT, isMipAniso ? desc.maxAnisotropy : 1);
    glSamplerParameteri(obj->id, GL_TEXTURE_COMPARE_MODE, isMinComp ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
    glSamplerParameteri(obj->id, GL_TEXTURE_COMPARE_FUNC, gCompareFuncs[static_cast<GLenum>(desc.compareFunc)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_WRAP_S, gSamplerAddressModes[static_cast<size_t>(desc.u)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_WRAP_T, gSamplerAddressModes[static_cast<size_t>(desc.v)]);
    glSamplerParameteri(obj->id, GL_TEXTURE_WRAP_R, gSamplerAddressModes[static_cast<size_t>(desc.w)]);
    glSamplerParameterf(obj->id, GL_TEXTURE_LOD_BIAS, desc.mipLodBias);
    glSamplerParameterfv(obj->id, GL_TEXTURE_BORDER_COLOR, desc.borderColor);
    glSamplerParameterf(obj->id, GL_TEXTURE_MIN_LOD, desc.minLod);
    glSamplerParameterf(obj->id, GL_TEXTURE_MAX_LOD, desc.maxLod);
    return impl->samplerStates.back();
}

void RenderContext::destroySamplerState(HSamplerState &handle) {
    if (handle) {
        impl->samplerStates.erase(std::remove(impl->samplerStates.begin(), impl->samplerStates.end(), handle), impl->samplerStates.end());
        glDeleteSamplers(1, &handle->id);
        HD_DELETE(handle.value);
        handle.invalidate();
    }
}

void RenderContext::bindSamplerState(const HSamplerState &handle, uint32_t slot) {
    HD_ASSERT(handle);
    HD_ASSERT(slot < MAX_SAMPLER_STATES);
    glBindSampler(slot, handle->id);
}

HProgram RenderContext::createProgram(const std::string &name, const std::string &vsCode, const std::string &psCode, const std::string &defines) {
    HD_ASSERT(!vsCode.empty());
    HD_ASSERT(!psCode.empty());
    ProgramImpl *obj = new ProgramImpl();
    impl->programs.push_back(HProgram(obj));
    obj->name = name;
    obj->vsCode = vsCode;
    obj->psCode = psCode;
    obj->defines = defines;
    obj->id = glCreateProgram();

    int result = 0, logLength = 0;
    uint32_t version = impl->settings.majorVersion*100 + impl->settings.minorVersion*10;
    if (version < 300) {
        version -= 90;
    }
    else if (version < 330) {
        version -= 170;
    }
    std::string versionStr = StringUtils::format("#version %d%s\n", version, impl->settings.isCoreProfile ? " core" : "");
    std::string definesStr;
    if (!defines.empty()) {
        for (const auto &it : StringUtils::split(defines, ";", false)) {
            definesStr += "#define" + it + "\n";
        }
    }

    std::string vsSource = versionStr + definesStr + "#line 1\n" + vsCode;
    std::string psSource = versionStr + definesStr + "#line 1\n" + psCode;

    uint32_t vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vsSources[] = { vsSource.data() };
    int vsSourcesLengths[] = { static_cast<int>(vsSource.length()) };
    glShaderSource(vs, HD_ARRAYSIZE(vsSourcesLengths), vsSources, vsSourcesLengths);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> errorStr(static_cast<size_t>(logLength));
        glGetShaderInfoLog(vs, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to compile vertex shader for program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    uint32_t ps = glCreateShader(GL_FRAGMENT_SHADER);
    const char *psSources[] = { psSource.data() };
    int psSourcesLengths[] = { static_cast<int>(psSource.length()) };
    glShaderSource(ps, HD_ARRAYSIZE(psSourcesLengths), psSources, psSourcesLengths);
    glCompileShader(ps);
    glGetShaderiv(ps, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderiv(ps, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> errorStr(static_cast<size_t>(logLength));
        glGetShaderInfoLog(ps, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to compile pixel shader for program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    glAttachShader(obj->id, vs);
    glAttachShader(obj->id, ps);
    glLinkProgram(obj->id);
    glGetProgramiv(obj->id, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramiv(obj->id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> errorStr(static_cast<size_t>(logLength));
        glGetProgramInfoLog(obj->id, logLength, nullptr, errorStr.data());
        HD_LOG_ERROR("Failed to link program '%s'. Errors:\n%s", name.data(), errorStr.data());
    }

    glValidateProgram(obj->id);
    glGetProgramiv(obj->id, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramiv(obj->id, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> errorStr(static_cast<size_t>(logLength));
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
    std::vector<std::string> text = stream.readAllLines();
    std::string code[2];
    int shaderType = 0; // 0 - vs, 1 - ps;
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
    FileReader fs(filename);
    fs.setName(filename);
    return createProgramFromStream(fs, defines);
}

void RenderContext::destroyProgram(HProgram &handle) {
    if (handle) {
        impl->programs.erase(std::remove(impl->programs.begin(), impl->programs.end(), handle), impl->programs.end());
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
    glUseProgram(handle->id);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, int value) {
    HD_ASSERT(id);
    glUniform1i(id.value, value);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, float value) {
    HD_ASSERT(id);
    glUniform1f(id.value, value);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::vec2 &value) {
    HD_ASSERT(id);
    glUniform2f(id.value, value.x, value.y);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::vec3 &value) {
    HD_ASSERT(id);
    glUniform3f(id.value, value.x, value.y, value.z);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::vec4 &value) {
    HD_ASSERT(id);
    glUniform4f(id.value, value.x, value.y, value.z, value.w);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::mat4 &value) {
    HD_ASSERT(id);
    glUniformMatrix4fv(id.value, 1, false, reinterpret_cast<const float*>(&value));
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const int *value, uint32_t count) {
    HD_ASSERT(id);
    glUniform1iv(id.value, count, value);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const float *value, uint32_t count) {
    HD_ASSERT(id);
    glUniform1fv(id.value, count, value);
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::vec2 *value, uint32_t count) {
    HD_ASSERT(id);
    glUniform2fv(id.value, count, reinterpret_cast<const float*>(value));
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::vec3 *value, uint32_t count) {
    HD_ASSERT(id);
    glUniform3fv(id.value, count, reinterpret_cast<const float*>(value));
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::vec4 *value, uint32_t count) {
    HD_ASSERT(id);
    glUniform4fv(id.value, count, reinterpret_cast<const float*>(value));
}

void RenderContext::setProgramConstant(const HProgramConstant &id, const glm::mat4 *value, uint32_t count) {
    HD_ASSERT(id);
    glUniformMatrix4fv(id.value, count, false, reinterpret_cast<const float*>(value));
}

}
