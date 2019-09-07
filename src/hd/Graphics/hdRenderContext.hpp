#pragma once
#include "../Core/hdMacros.hpp"
#include "../Core/hdHandle.hpp"
#include "../../3rd/include/glm/glm.hpp"
#include <memory>
#include <vector>
#include <string>

namespace hd {

class Stream;
class Window;

enum class BufferUsage {
    Static,
    Dynamic,
    Stream
};

enum class BufferAccess {
    Read,
    Write,
    ReadWrite
};

enum class VertexElementType {
    Float,
    Float2,
    Float3,
    Float4
};

struct VertexElement {
    VertexElement();
    VertexElement(VertexElementType type, uint32_t index, uint32_t slot, uint32_t offset, bool normalized, bool perInstance);

    VertexElementType type;
    uint32_t index, slot, offset;
    bool normalized, perInstance;
};

enum class TextureFormat {
    R8,
    RG8,
    RGB8,
    RGBA8,
    A8,
    R16,
    RG16,
    RGB16,
    RGBA16,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F,
    R32F,
    RG32F,
    RGB32F,
    RGBA32F,
    D16,
    D24,
    D32,
    D24S8
    //DXT1,
    //DXT3,
    //DXT5
};

enum class SamplerFilter {
    Point,
    Linear,
    Anisotropic,
    ComparisonPoint,
    ComparisonLinear,
    ComparisonAnisotropic,
};

enum class SamplerAddressMode {
    Repeat,
    Mirror,
    Clamp,
    Border,
    MirrorOnce
};

enum class PrimitiveType {
    Points,
    LineStrip,
    Lines,
    TriangleStrip,
    Triangles
};

enum class CompareFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreatEqual,
    Always
};

enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrSat,
    DecrSat,
    Invert,
    Incr,
    Decr
};

enum class CullMode {
    None,
    Front,
    Back
};

enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
    SrcAlphaSat
};

enum class BlendOp {
    Add,
    Subtract,
    RevSubtract,
    Min,
    Max
};

enum class BlendMode {
    Replace,
    Add,
    Multiply,
    Alpha,
    AddAlpha,
    PreMulAlpha,
    InvDestAlpha,
    Subtract,
    SubtractAlpha
};

enum class CullFace {
    None,
    Front,
    Back,
    FrontBack
};

enum class FillMode {
    Solid,
    Wireframe
};

enum class FrontFace {
    CCW,
    CW
};

enum class IndexType {
    Uint,
    Ushort,
    Ubyte
};

struct BlendModeDesc {
    BlendModeDesc();
    BlendModeDesc(bool blendEnable, BlendFactor srcBlend, BlendFactor dstBlend, BlendOp blendOp, BlendFactor srcBlendAlpha, BlendFactor dstBlendAlpha, BlendOp blendOpAlpha);

    bool blendEnable;
    BlendFactor srcBlend;
    BlendFactor dstBlend;
    BlendOp blendOp;
    BlendFactor srcBlendAlpha;
    BlendFactor dstBlendAlpha;
    BlendOp blendOpAlpha;
};

struct StencilTestDesc {
    StencilTestDesc();
    StencilTestDesc(CompareFunc frontFunc, StencilOp frontFail, StencilOp frontDepthFail, StencilOp frontPass, CompareFunc backFunc, StencilOp backFail, StencilOp backDepthFail, StencilOp backPass, uint32_t readMask = 0xffffffff, uint32_t writeMask = 0xffffffff);

    CompareFunc frontFunc;
    StencilOp frontFail;
    StencilOp frontDepthFail;
    StencilOp frontPass;
    CompareFunc backFunc;
    StencilOp backFail;
    StencilOp backDepthFail;
    StencilOp backPass;
    uint32_t readMask;
    uint32_t writeMask;
};

struct ColorMask {
    ColorMask();
    explicit ColorMask(bool rgba);
    ColorMask(bool r, bool g, bool b, bool a);

    bool operator==(const ColorMask &rhs) const;
    bool operator!=(const ColorMask &rhs) const;

    bool r, g, b, a;
};

struct PolygonOffset {
    PolygonOffset();
    PolygonOffset(bool enabled, float factor, float units);

    bool operator==(const PolygonOffset &rhs) const;
    bool operator!=(const PolygonOffset &rhs) const;

    bool enabled;
    float factor, units;
};

struct SamplerStateDesc {
    SamplerStateDesc();

    SamplerFilter minFilter;
    SamplerFilter magFilter;
    SamplerFilter mipFilter;
    SamplerAddressMode u;
    SamplerAddressMode v;
    SamplerAddressMode w;
    uint32_t maxAnisotropy;
    float mipLodBias;
    CompareFunc compareFunc;
    float borderColor[4];
    float minLod;
    float maxLod;
};

using HVertexFormat = Handle<struct VertexFormatImpl*, struct TAG_VertexFormatImpl>;
using HVertexBuffer = Handle<struct VertexBufferImpl*, struct TAG_VertexBufferImpl>;
using HIndexBuffer = Handle<struct IndexBufferImpl*, struct TAG_IndexBufferImpl>;
using HConstantBuffer = Handle<struct ConstantBufferImpl*, struct TAG_ConstantBufferImpl>;
using HTexture2D = Handle<struct Texture2DImpl*, struct TAG_Texture2DImpl>;
using HTexture2DArray = Handle<struct Texture2DArrayImpl*, struct TAG_Texture2DArrayImpl>;
using HSamplerState = Handle<struct SamplerStateImpl*, struct TAG_SamplerStateImpl>;
using HProgram = Handle<struct ProgramImpl*, struct TAG_ProgramImpl>;
using HProgramConstant = Handle<int, struct TAG_ProgramConstant, -1>;

class RenderContext {
    HD_NONCOPYABLE_CLASS(RenderContext);
public:
    RenderContext();
    explicit RenderContext(const Window &window);
    ~RenderContext();

    void create(const Window &window);
    void destroy();

    void clearRenderTarget(const glm::vec4 &rgba);
    void clearDepthStencil(float depth);
    void draw(PrimitiveType primType, uint32_t vertexCount, uint32_t firstVertex);
    void drawInstanced(PrimitiveType primType, uint32_t vertexCountPerInstance, uint32_t firstVertex, uint32_t instanceCount);
    void drawIndexed(PrimitiveType primType, uint32_t indexCount, uint32_t firstIndex, IndexType indexType);
    void drawIndexedInstanced(PrimitiveType primType, uint32_t indexCountPerInstance, uint32_t firstIndex, IndexType indexType, uint32_t instanceCount);

    void setDepthState(bool depthTestEnabled, CompareFunc compareFunc, bool depthMask);
    void setStencilState(bool enabled, const StencilTestDesc &desc, int ref);
    void setBlendState(const BlendModeDesc &blendModeDesc, const ColorMask &colorMask);
    void setBlendState(BlendMode blendMode, const ColorMask &colorMask);
    void setRasterizerState(CullFace cullFace, FillMode fillMode, FrontFace frontFace, const PolygonOffset &polygonOffset);
    void setViewport(const glm::ivec2 &pos, const glm::ivec2 &size);

    HVertexFormat createVertexFormat(const std::vector<VertexElement> &desc);
    void destroyVertexFormat(HVertexFormat &handle);
    void bindVertexFormat(const HVertexFormat &handle);

    HVertexBuffer createVertexBuffer(const void *data, uint32_t size, BufferUsage usage);
    void destroyVertexBuffer(HVertexBuffer &handle);
    void setVertexBufferData(HVertexBuffer &handle, const void *data, uint32_t size);
    void updateVertexBufferData(HVertexBuffer &handle, const void *data);
    void updateVertexBufferData(HVertexBuffer &handle, const void *data, uint32_t offset, uint32_t size);
    void *mapVertexBuffer(HVertexBuffer &handle, BufferAccess access);
    void unmapVertexBuffer(HVertexBuffer &handle);
    void bindVertexBuffer(const HVertexBuffer &handle, uint32_t slot, uint32_t offset, uint32_t stride);

    HIndexBuffer createIndexBuffer(const void *data, uint32_t size, BufferUsage usage);
    void destroyIndexBuffer(HIndexBuffer &handle);
    void setIndexBufferData(HIndexBuffer &handle, const void *data, uint32_t size);
    void updateIndexBufferData(HIndexBuffer &handle, const void *data);
    void updateIndexBufferData(HIndexBuffer &handle, const void *data, uint32_t offset, uint32_t size);
    void *mapIndexBuffer(HIndexBuffer &handle, BufferAccess access);
    void unmapIndexBuffer(HIndexBuffer &handle);
    void bindIndexBuffer(const HIndexBuffer &handle);

    HConstantBuffer createConstantBuffer(const void *data, uint32_t size, BufferUsage usage);
    void destroyConstantBuffer(HConstantBuffer &handle);
    void setConstantBufferData(HConstantBuffer &handle, const void *data, uint32_t size);
    void updateConstantBufferData(HConstantBuffer &handle, const void *data);
    void updateConstantBufferData(HConstantBuffer &handle, const void *data, uint32_t offset, uint32_t size);
    void *mapConstantBuffer(HConstantBuffer &handle, BufferAccess access);
    void unmapConstantBuffer(HConstantBuffer &handle);
    void bindConstantBuffer(const HConstantBuffer &handle, uint32_t slot);

    HTexture2D createTexture2D(const void *data, const glm::ivec2 &size, TextureFormat format);
    HTexture2D createTexture2DFromStream(Stream &stream);
    HTexture2D createTexture2DFromFile(const std::string &filename);
    void destroyTexture2D(HTexture2D &handle);
    void bindTexture2D(const HTexture2D &handle, uint32_t slot);
    
    HTexture2DArray createTexture2DArray(const void *data, const glm::ivec2 &size, TextureFormat format, uint32_t layers);
    HTexture2DArray createTexture2DArrayFromFiles(const std::vector<std::string> &filenames);
    void setTexture2DArrayLayerData(const HTexture2DArray &handle, uint32_t layer, const void *data, TextureFormat format);
    void setTexture2DArrayLayerData(const HTexture2DArray &handle, uint32_t layer, Stream &stream);
    void setTexture2DArrayLayerData(const HTexture2DArray &handle, uint32_t layer, const std::string &filename);
    void destroyTexture2DArray(HTexture2DArray &handle);
    void bindTexture2DArray(const HTexture2DArray &handle, uint32_t slot);

    HSamplerState createSamplerState(const SamplerStateDesc &desc);
    void destroySamplerState(HSamplerState &handle);
    void bindSamplerState(const HSamplerState &handle, uint32_t slot);

    HProgram createProgram(const std::string &name, const std::string &vsCode, const std::string &psCode, const std::string &defines = "");
    HProgram createProgramFromStream(Stream &stream, const std::string &defines = "");
    HProgram createProgramFromFile(const std::string &filename, const std::string &defines = "");
    void destroyProgram(HProgram &handle);
    HProgramConstant getProgramConstantID(const HProgram &handle, const std::string &name);
    void bindProgram(const HProgram &handle);
    void setProgramConstant(const HProgramConstant &id, int value);
    void setProgramConstant(const HProgramConstant &id, float value);
    void setProgramConstant(const HProgramConstant &id, const glm::vec2 &value);
    void setProgramConstant(const HProgramConstant &id, const glm::vec3 &value);
    void setProgramConstant(const HProgramConstant &id, const glm::vec4 &value);
    void setProgramConstant(const HProgramConstant &id, const glm::mat4 &value);
    void setProgramConstant(const HProgramConstant &id, const int *value, uint32_t count);
    void setProgramConstant(const HProgramConstant &id, const float *value, uint32_t count);
    void setProgramConstant(const HProgramConstant &id, const glm::vec2 *value, uint32_t count);
    void setProgramConstant(const HProgramConstant &id, const glm::vec3 *value, uint32_t count);
    void setProgramConstant(const HProgramConstant &id, const glm::vec4 *value, uint32_t count);
    void setProgramConstant(const HProgramConstant &id, const glm::mat4 *value, uint32_t count);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

}
