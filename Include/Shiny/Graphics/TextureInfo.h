#ifndef SHINY_TEXTURE_SPECIFICATION_H
#define SHINY_TEXTURE_SPECIFICATION_H

#include "Shiny/Graphics/OpenGL.h"

namespace Shiny {

namespace Tex {

enum class Target : GLenum {
   // 1D
   k1d = GL_TEXTURE_1D,
   kProxyTexture1d = GL_PROXY_TEXTURE_1D,

   // Buffer
   kBuffer = GL_TEXTURE_BUFFER,

   // 2D
   k2d = GL_TEXTURE_2D,
   kProxy2d = GL_PROXY_TEXTURE_2D,
   k1dArray = GL_TEXTURE_1D_ARRAY,
   kProxy1dArray = GL_PROXY_TEXTURE_1D_ARRAY,
   kRectangle = GL_TEXTURE_RECTANGLE,
   kProxyRectangle = GL_PROXY_TEXTURE_RECTANGLE,

   // 2D multisample
   k2dMultisample = GL_TEXTURE_2D_MULTISAMPLE,
   kProxy2dMultisample = GL_PROXY_TEXTURE_2D_MULTISAMPLE,

   // Cube map
   kCubeMap = GL_TEXTURE_CUBE_MAP,
   kProxyCubeMap = GL_PROXY_TEXTURE_CUBE_MAP,
   kCubeMapPositiveX = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
   kCubeMapNegativeX = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
   kCubeMapPositiveY = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
   kCubeMapNegativeY = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
   kCubeMapPositiveZ = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
   kCubeMapNegativeZ = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,

   // 3D
   k3d = GL_TEXTURE_3D,
   kProxy3d = GL_PROXY_TEXTURE_3D,
   k2dArray = GL_TEXTURE_2D_ARRAY,
   kProxy2dArray = GL_PROXY_TEXTURE_2D_ARRAY,

   // 3D multisample
   k2dMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
   kProxy2dMultisampleArray = GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY
};

enum class InternalFormat : GLint {
   // R
   kR8 = GL_R8,
   kR8I = GL_R8I,
   kR8UI = GL_R8UI,
   kR8SNorm = GL_R8_SNORM,

   kR16F = GL_R16F,
   kR16I = GL_R16I,
   kR16UI = GL_R16UI,
   kR16SNorm = GL_R16_SNORM,

   kR32F = GL_R32F,
   kR32I = GL_R32I,
   kR32UI = GL_R32UI,


   // RG
   kRG8 = GL_RG8,
   kRG8I = GL_RG8I,
   kRG8UI = GL_RG8UI,
   kRG8SNorm = GL_RG8_SNORM,

   kRG16 = GL_RG16,
   kRG16F = GL_RG16F,
   kRG16SNorm = GL_RG16_SNORM,

   kRG32F = GL_RG32F,
   kRG32I = GL_RG32I,
   kRG32UI = GL_RG32UI,


   // RGB
   kRGB8 = GL_RGB8,
   kRGB8I = GL_RGB8I,
   kRGB8UI = GL_RGB8UI,
   kRGB8SNorm = GL_RGB8_SNORM,

   kRGB16 = GL_RGB16,
   kRGB16F = GL_RGB16F,
   kRGB16I = GL_RGB16I,
   kRGB16UI = GL_RGB16UI,
   kRGB16SNorm = GL_RGB16_SNORM,

   kRGB32F = GL_RGB32F,
   kRGB32I = GL_RGB32I,
   kRGB32UI = GL_RGB32UI,


   // RGBA
   kRGBA8 = GL_RGBA8,
   kRGBA8UI = GL_RGBA8UI,
   kRGBA8SNorm = GL_RGBA8_SNORM,

   kRGBA16 = GL_RGBA16,
   kRGBA16F = GL_RGBA16F,
   kRGBA16I = GL_RGBA16I,
   kRGBA16UI = GL_RGBA16UI,
   kRGBA16SNorm = GL_RGBA16_SNORM,

   kRGBA32F = GL_RGBA32F,
   kRGBA32I = GL_RGBA32I,
   kRGBA32UI = GL_RGBA32UI,


   // SRGB
   kSRGB8 = GL_SRGB8,
   kSRGB8Alpha8 = GL_SRGB8_ALPHA8,


   // Compressed
   kCompressedRed = GL_COMPRESSED_RED,
   kCompressedRedRGTC1 = GL_COMPRESSED_RED_RGTC1,
   kCompressedSignedRedRGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1,
   kCompressedRG = GL_COMPRESSED_RG,
   kCompressedRGRGTC2 = GL_COMPRESSED_RG_RGTC2,
   kCompressedSignedRGRGTC2 = GL_COMPRESSED_SIGNED_RG_RGTC2,
   kCompressedRGB = GL_COMPRESSED_RGB,
   kCompressedRGBA = GL_COMPRESSED_RGBA,
   kCompressedSRGB = GL_COMPRESSED_SRGB,
   kCompressedSRGBAlpha = GL_COMPRESSED_SRGB_ALPHA,


   // Depth / stencil
   kDepthComponent16 = GL_DEPTH_COMPONENT16,
   kDepthComponent24 = GL_DEPTH_COMPONENT24,
   kDepthComponent32F = GL_DEPTH_COMPONENT32F,

   kDepth24Stencil8 = GL_DEPTH24_STENCIL8,
   kDepth24Stencil32F = GL_DEPTH32F_STENCIL8,


   // Other
   kRGB10A2 = GL_RGB10_A2,
   kRGB10A2UI = GL_RGB10_A2UI,
   kR11FG11FB10F = GL_R11F_G11F_B10F,
   kRGB9E5 = GL_RGB9_E5
};

enum class ProvidedDataFormat : GLenum {
   kRed = GL_RED,
   kRG = GL_RG,
   kRGB = GL_RGB,
   kBGR = GL_BGR,
   kRGBA = GL_RGBA,
   kBGRA = GL_BGRA,
   kDepthComponent = GL_DEPTH_COMPONENT,
   kDepthStencil = GL_DEPTH_STENCIL
};

enum class ProvidedDataType : GLenum {
   // From specification
   kByte = GL_BYTE,
   kUnsignedByte = GL_UNSIGNED_BYTE,
   kShort = GL_SHORT,
   kUnsignedShort = GL_UNSIGNED_SHORT,
   kInt = GL_INT,
   kUnsignedInt = GL_UNSIGNED_INT,
   kFloat = GL_FLOAT,
   kHalfFloat = GL_HALF_FLOAT,
   kUnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2,
   kUnsignedByte233Rev = GL_UNSIGNED_BYTE_2_3_3_REV,
   kUnsignedShort565 = GL_UNSIGNED_SHORT_5_6_5,
   kUnsignedShort565Rev = GL_UNSIGNED_SHORT_5_6_5_REV,
   kUnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
   kUnsignedShort4444Rev = GL_UNSIGNED_SHORT_4_4_4_4_REV,
   kUnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1,
   kUnsignedShort1555Rev = GL_UNSIGNED_SHORT_1_5_5_5_REV,
   kUnsignedInt8888 = GL_UNSIGNED_INT_8_8_8_8,
   kUnsignedInt8888Rev = GL_UNSIGNED_INT_8_8_8_8_REV,
   kUnsignedInt1010102 = GL_UNSIGNED_INT_10_10_10_2,
   kUnsignedInt2101010Rev = GL_UNSIGNED_INT_2_10_10_10_REV,

   // Missing from specification, but needed?
   kUnsignedInt248 = GL_UNSIGNED_INT_24_8
};

struct Specification {
   // 1D
   static Specification create1d(Target inTarget = Target::k1d, GLint inLevel = 0, InternalFormat inInternalFormat = InternalFormat::kRGB8,
                                 GLsizei inWidth = 256, ProvidedDataFormat inProvidedDataFormat = ProvidedDataFormat::kRGB,
                                 ProvidedDataType inProvidedDataType = ProvidedDataType::kUnsignedByte, const GLvoid* inProvidedData = nullptr) {
      return Specification(inTarget, inLevel, 0, inInternalFormat, inWidth, 0, 0, inProvidedDataFormat, inProvidedDataType, inProvidedData);
   }

   // Buffer
   static Specification createBuffer(InternalFormat inInternalFormat = InternalFormat::kRGB8, GLuint inBuffer = 0) {
      return Specification(Target::kBuffer, 0, 0, inInternalFormat, 0, 0, 0, ProvidedDataFormat::kRGB, ProvidedDataType::kUnsignedByte, nullptr, false, inBuffer);
   }

   // 2D
   static Specification create2d(Target inTarget = Target::k2d, GLint inLevel = 0, InternalFormat inInternalFormat = InternalFormat::kRGB8,
                                 GLsizei inWidth = 256, GLsizei inHeight = 256, ProvidedDataFormat inProvidedDataFormat = ProvidedDataFormat::kRGB,
                                 ProvidedDataType inProvidedDataType = ProvidedDataType::kUnsignedByte, const GLvoid* inProvidedData = nullptr) {
      return Specification(inTarget, inLevel, 0, inInternalFormat, inWidth, inHeight, 0, inProvidedDataFormat, inProvidedDataType, inProvidedData);
   }

   // 2D multisample
   static Specification create2dMultisample(Target inTarget = Target::k2dMultisample, GLsizei inSamples = 4, InternalFormat inInternalFormat = InternalFormat::kRGB8,
                                            GLsizei inWidth = 256, GLsizei inHeight = 256, bool inFixedSampleLocations = false) {
      return Specification(inTarget, 0, inSamples, inInternalFormat, inWidth, inHeight, 0, ProvidedDataFormat::kRGB, ProvidedDataType::kUnsignedByte, nullptr,
                           inFixedSampleLocations);
   }

   // Cube map
   static Specification createCubeMap(Target inTarget = Target::kCubeMap, GLint inLevel = 0, InternalFormat inInternalFormat = InternalFormat::kRGB8,
                                      GLsizei inWidth = 256, GLsizei inHeight = 256, ProvidedDataFormat inProvidedDataFormat = ProvidedDataFormat::kRGB,
                                      ProvidedDataType inProvidedDataType = ProvidedDataType::kUnsignedByte,
                                      const GLvoid* inPositiveXData = nullptr, const GLvoid* inNegativeXData = nullptr,
                                      const GLvoid* inPositiveYData = nullptr, const GLvoid* inNegativeYData = nullptr,
                                      const GLvoid* inPositiveZData = nullptr, const GLvoid* inNegativeZData = nullptr) {
      return Specification(inTarget, inLevel, 0, inInternalFormat, inWidth, inHeight, 0, inProvidedDataFormat, inProvidedDataType, nullptr, false, 0,
                           inPositiveXData, inNegativeXData, inPositiveYData, inNegativeYData, inPositiveZData, inNegativeZData);
   }

   // 3D
   static Specification create3d(Target inTarget = Target::k3d, GLint inLevel = 0, InternalFormat inInternalFormat = InternalFormat::kRGB8,
                                 GLsizei inWidth = 256, GLsizei inHeight = 256, GLsizei inDepth = 256, ProvidedDataFormat inProvidedDataFormat = ProvidedDataFormat::kRGB,
                                 ProvidedDataType inProvidedDataType = ProvidedDataType::kUnsignedByte, const GLvoid* inProvidedData = nullptr) {
      return Specification(inTarget, inLevel, 0, inInternalFormat, inWidth, inHeight, inDepth, inProvidedDataFormat, inProvidedDataType, inProvidedData);
   }

   // 3D multisample
   static Specification create3dMultisample(Target inTarget = Target::k2dMultisample, GLsizei inSamples = 4, InternalFormat inInternalFormat = InternalFormat::kRGB8,
                                            GLsizei inWidth = 256, GLsizei inHeight = 256, GLsizei inDepth = 256, bool inFixedSampleLocations = false) {
      return Specification(inTarget, 0, inSamples, inInternalFormat, inWidth, inHeight, inDepth, ProvidedDataFormat::kRGB, ProvidedDataType::kUnsignedByte, nullptr,
                           inFixedSampleLocations);
   }

   Specification(Target inTarget = Target::k2d, GLint inLevel = 0, GLsizei inSamples = 0,
                 InternalFormat inInternalFormat = InternalFormat::kRGB8, GLsizei inWidth = 256, GLsizei inHeight = 256, GLsizei inDepth = 256,
                 ProvidedDataFormat inProvidedDataFormat = ProvidedDataFormat::kRGB,
                 ProvidedDataType inProvidedDataType = ProvidedDataType::kUnsignedByte, const GLvoid* inProvidedData = nullptr,
                 bool inFixedSampleLocations = false, GLuint inBuffer = 0, const GLvoid* inPositiveXData = nullptr, const GLvoid* inNegativeXData = nullptr,
                 const GLvoid* inPositiveYData = nullptr, const GLvoid* inNegativeYData = nullptr, const GLvoid* inPositiveZData = nullptr, const GLvoid* inNegativeZData = nullptr)
      : target(inTarget), level(inLevel), samples(inSamples), internalFormat(inInternalFormat), width(inWidth), height(inHeight), depth(inDepth),
        providedDataFormat(inProvidedDataFormat), providedDataType(inProvidedDataType), providedData(inProvidedData), fixedSampleLocations(inFixedSampleLocations), buffer(inBuffer),
        positiveXData(inPositiveXData), negativeXData(inNegativeXData), positiveYData(inPositiveYData), negativeYData(inNegativeYData),
        positiveZData(inPositiveZData), negativeZData(inNegativeZData) {
   }

   Target target;
   GLint level;
   GLsizei samples;
   InternalFormat internalFormat;
   GLsizei width;
   GLsizei height;
   GLsizei depth;
   ProvidedDataFormat providedDataFormat;
   ProvidedDataType providedDataType;
   const GLvoid* providedData;
   bool fixedSampleLocations;
   GLuint buffer;
   const GLvoid* positiveXData;
   const GLvoid* negativeXData;
   const GLvoid* positiveYData;
   const GLvoid* negativeYData;
   const GLvoid* positiveZData;
   const GLvoid* negativeZData;
};

enum class FloatParam : GLenum {
   kLodBias = GL_TEXTURE_LOD_BIAS,
   kMinLod = GL_TEXTURE_MIN_LOD,
   kMaxLod = GL_TEXTURE_MAX_LOD,
};

enum class IntParam : GLenum {
   kBaseLevel = GL_TEXTURE_BASE_LEVEL,
   kMaxLevel = GL_TEXTURE_MAX_LEVEL,
   kCompareFunc = GL_TEXTURE_COMPARE_FUNC,
   kCompareMode = GL_TEXTURE_COMPARE_MODE,
   kMinFilter = GL_TEXTURE_MIN_FILTER,
   kMagFilter = GL_TEXTURE_MAG_FILTER,
   kSwizzleR = GL_TEXTURE_SWIZZLE_R,
   kSwizzleG = GL_TEXTURE_SWIZZLE_G,
   kSwizzleB = GL_TEXTURE_SWIZZLE_B,
   kSwizzleA = GL_TEXTURE_SWIZZLE_A,
   kWrapS = GL_TEXTURE_WRAP_S,
   kWrapT = GL_TEXTURE_WRAP_T,
   kWrapR = GL_TEXTURE_WRAP_R,
};

enum class FloatArrayParam : GLenum {
   kBorderColor = GL_TEXTURE_BORDER_COLOR,
};

enum class IntArrayParam : GLenum {
   kBorderColor = GL_TEXTURE_BORDER_COLOR,
   kSwizzleRGBA = GL_TEXTURE_SWIZZLE_RGBA,
};

enum class InternalIntArrayParam : GLenum {
   kBorderColor = GL_TEXTURE_BORDER_COLOR,
};

enum class InternalUintArrayParam : GLenum {
   kBorderColor = GL_TEXTURE_BORDER_COLOR,
};

} // namespace Tex

} // namespace Shiny

#endif
