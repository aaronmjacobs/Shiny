#include "Shiny/ShinyAssert.h"
#include "Shiny/Graphics/Texture.h"

#include <glm/gtc/type_ptr.hpp>

namespace Shiny {

namespace {

template<typename T>
bool matchesAny(T item) {
   return false;
}

template<typename T, T First, T... Rest>
bool matchesAny(T item) {
   return item == First || matchesAny<T, Rest...>(item);
}

void verifySpecification(const Tex::Specification& spec) {
#if SHINY_DEBUG
   ASSERT(spec.level >= 0);
   ASSERT(spec.samples >= 0);
   GLint maxTextureSize = Context::current()->getMaxTextureSize();
   ASSERT(spec.width >= 0 && spec.height >= 0 && spec.depth >= 0 &&
          spec.width <= maxTextureSize && spec.height <= maxTextureSize && spec.depth <= maxTextureSize);

   switch (spec.target) {
   case Tex::Target::k1d:
   case Tex::Target::kProxyTexture1d:
      ASSERT((matchesAny<GLenum,
         GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_COMPRESSED_SRGB,
         GL_COMPRESSED_SRGB_ALPHA, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32,
         GL_R3_G3_B2, GL_RED, GL_RG, GL_RGB, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB10, GL_RGB12, GL_RGB16, GL_RGBA, GL_RGBA2,
         GL_RGBA4, GL_RGB5_A1, GL_RGBA8, GL_RGB10_A2, GL_RGBA12, GL_RGBA16, GL_SRGB, GL_SRGB8, GL_SRGB_ALPHA,
         GL_SRGB8_ALPHA8>(static_cast<GLenum>(spec.internalFormat)), "Invalid internal format for glTexImage1D()"));
      break;
   case Tex::Target::kBuffer:
      ASSERT((matchesAny<GLenum, GL_R8, GL_R16, GL_R16F, GL_R32F, GL_R8I, GL_R16I, GL_R32I, GL_R8UI, GL_R16UI, GL_R32UI,
         GL_RG8, GL_RG16, GL_RG16F, GL_RG32F, GL_RG8I, GL_RG16I, GL_RG32I, GL_RG8UI, GL_RG16UI, GL_RG32UI, GL_RGBA8,
         GL_RGBA16, GL_RGBA16F, GL_RGBA32F, GL_RGBA8I, GL_RGBA16I, GL_RGBA32I, GL_RGBA8UI, GL_RGBA16UI,
         GL_RGBA32UI>(static_cast<GLenum>(spec.internalFormat)), "Invalid internal format for glTexBuffer()"));
      break;
   case Tex::Target::k2d:
   case Tex::Target::kProxy2d:
   case Tex::Target::k1dArray:
   case Tex::Target::kProxy1dArray:
   case Tex::Target::kRectangle:
   case Tex::Target::kProxyRectangle:
      ASSERT((matchesAny<GLenum,
         GL_RGBA32F, GL_RGBA32I, GL_RGBA32UI, GL_RGBA16, GL_RGBA16F, GL_RGBA16I, GL_RGBA16UI, GL_RGBA8, GL_RGBA8UI,
         GL_SRGB8_ALPHA8, GL_RGB10_A2, GL_RGB10_A2UI, GL_R11F_G11F_B10F, GL_RG32F, GL_RG32I, GL_RG32UI, GL_RG16,
         GL_RG16F, GL_RGB16I, GL_RGB16UI, GL_RG8, GL_RG8I, GL_RG8UI, GL_R32F, GL_R32I, GL_R32UI, GL_R16F, GL_R16I,
         GL_R16UI, GL_R8, GL_R8I, GL_R8UI, GL_RGBA16_SNORM, GL_RGBA8_SNORM, GL_RGB32F, GL_RGB32I, GL_RGB32UI,
         GL_RGB16_SNORM, GL_RGB16F, GL_RGB16I, GL_RGB16UI, GL_RGB16, GL_RGB8_SNORM, GL_RGB8, GL_RGB8I, GL_RGB8UI,
         GL_SRGB8, GL_RGB9_E5, GL_RG16_SNORM, GL_RG8_SNORM, GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2,
         GL_R16_SNORM, GL_R8_SNORM, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_DEPTH_COMPONENT32F,
         GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT16, GL_DEPTH32F_STENCIL8, GL_DEPTH24_STENCIL8>
         (static_cast<GLenum>(spec.internalFormat)), "Invalid internal format for glTexImage2D()"));
      break;
   case Tex::Target::k2dMultisample:
   case Tex::Target::kProxy2dMultisample:
      break;
   case Tex::Target::kCubeMap:
   case Tex::Target::kProxyCubeMap:
      ASSERT((matchesAny<GLenum,
         GL_RGBA32F, GL_RGBA32I, GL_RGBA32UI, GL_RGBA16, GL_RGBA16F, GL_RGBA16I, GL_RGBA16UI, GL_RGBA8, GL_RGBA8UI,
         GL_SRGB8_ALPHA8, GL_RGB10_A2, GL_RGB10_A2UI, GL_R11F_G11F_B10F, GL_RG32F, GL_RG32I, GL_RG32UI, GL_RG16,
         GL_RG16F, GL_RGB16I, GL_RGB16UI, GL_RG8, GL_RG8I, GL_RG8UI, GL_R32F, GL_R32I, GL_R32UI, GL_R16F, GL_R16I,
         GL_R16UI, GL_R8, GL_R8I, GL_R8UI, GL_RGBA16_SNORM, GL_RGBA8_SNORM, GL_RGB32F, GL_RGB32I, GL_RGB32UI,
         GL_RGB16_SNORM, GL_RGB16F, GL_RGB16I, GL_RGB16UI, GL_RGB16, GL_RGB8_SNORM, GL_RGB8, GL_RGB8I, GL_RGB8UI,
         GL_SRGB8, GL_RGB9_E5, GL_RG16_SNORM, GL_RG8_SNORM, GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2,
         GL_R16_SNORM, GL_R8_SNORM, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_DEPTH_COMPONENT32F,
         GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT16, GL_DEPTH32F_STENCIL8, GL_DEPTH24_STENCIL8>
         (static_cast<GLenum>(spec.internalFormat)), "Invalid internal format for glTexImage2D()"));
      break;
   case Tex::Target::k3d:
   case Tex::Target::kProxy3d:
   case Tex::Target::k2dArray:
   case Tex::Target::kProxy2dArray:
      ASSERT((matchesAny<GLenum,
         GL_RGBA32F, GL_RGBA32I, GL_RGBA32UI, GL_RGBA16, GL_RGBA16F, GL_RGBA16I, GL_RGBA16UI, GL_RGBA8, GL_RGBA8UI,
         GL_SRGB8_ALPHA8, GL_RGB10_A2, GL_RGB10_A2UI, GL_R11F_G11F_B10F, GL_RG32F, GL_RG32I, GL_RG32UI, GL_RG16,
         GL_RG16F, GL_RGB16I, GL_RGB16UI, GL_RG8, GL_RG8I, GL_RG8UI, GL_R32F, GL_R32I, GL_R32UI, GL_R16F, GL_R16I,
         GL_R16UI, GL_R8, GL_R8I, GL_R8UI, GL_RGBA16_SNORM, GL_RGBA8_SNORM, GL_RGB32F, GL_RGB32I, GL_RGB32UI,
         GL_RGB16_SNORM, GL_RGB16F, GL_RGB16I, GL_RGB16UI, GL_RGB16, GL_RGB8_SNORM, GL_RGB8, GL_RGB8I, GL_RGB8UI,
         GL_SRGB8, GL_RGB9_E5, GL_RG16_SNORM, GL_RG8_SNORM, GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_SIGNED_RG_RGTC2,
         GL_R16_SNORM, GL_R8_SNORM, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_DEPTH_COMPONENT32F,
         GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT16, GL_DEPTH32F_STENCIL8, GL_DEPTH24_STENCIL8>
         (static_cast<GLenum>(spec.internalFormat)), "Invalid internal format for glTexImage3D()"));
      break;
   case Tex::Target::k2dMultisampleArray:
   case Tex::Target::kProxy2dMultisampleArray:
      break;
   default:
      ASSERT(false, "Invalid texture target: %u", spec.target);
      break;
   }
#endif // SHINY_DEBUG
}

#if SHINY_DEBUG
void verifyParamTarget(Tex::Target target) {
   ASSERT(target == Tex::Target::k1d ||
          target == Tex::Target::k2d ||
          target == Tex::Target::k3d ||
          target == Tex::Target::k1dArray ||
          target == Tex::Target::k2dArray ||
          target == Tex::Target::kRectangle ||
          target == Tex::Target::kCubeMap,
          "Invalid texture target for setting texture parameters: %u", target);
}

void verifySwizzleValue(GLint value) {
   ASSERT(value == GL_RED || value == GL_GREEN || value == GL_BLUE || value == GL_ALPHA || value == GL_ZERO ||
          value == GL_ONE, "Invalid texture swizzle value: 0x%X", value);
}
#endif // SHINY_DEBUG

void verifyParam(Tex::Target target, Tex::FloatParam param, GLfloat value) {
#if SHINY_DEBUG
   verifyParamTarget(target);

   switch (param) {
   case Tex::FloatParam::kLodBias:
   case Tex::FloatParam::kMinLod:
   case Tex::FloatParam::kMaxLod:
      break;
   default:
      ASSERT(false, "Invalid texture float param: 0x%X", param);
      break;
   }
#endif // SHINY_DEBUG
}

void verifyParam(Tex::Target target, Tex::IntParam param, GLint value) {
#if SHINY_DEBUG
   verifyParamTarget(target);

   switch (param) {
   case Tex::IntParam::kBaseLevel:
   case Tex::IntParam::kMaxLevel:
      ASSERT(value >= 0, "Texture base / max levels should not be negative: %d", value);
      break;
   case Tex::IntParam::kCompareFunc:
      ASSERT(value == GL_LEQUAL || value == GL_GEQUAL || value == GL_LESS || value == GL_GREATER || value == GL_EQUAL ||
             value == GL_NOTEQUAL || value == GL_ALWAYS || value == GL_NEVER,
             "Invalid texture compare func: 0x%X", value);
      break;
   case Tex::IntParam::kCompareMode:
      ASSERT(value == GL_COMPARE_REF_TO_TEXTURE || value == GL_NONE, "Invalid texture compare mode: 0x%X", value);
      break;
   case Tex::IntParam::kMinFilter:
      ASSERT(value == GL_NEAREST || value == GL_LINEAR || value == GL_NEAREST_MIPMAP_NEAREST ||
             value == GL_LINEAR_MIPMAP_NEAREST || value == GL_NEAREST_MIPMAP_LINEAR || value == GL_LINEAR_MIPMAP_LINEAR,
             "Invalid texture min filter: 0x%X", value);
      break;
   case Tex::IntParam::kMagFilter:
      ASSERT(value == GL_NEAREST || value == GL_LINEAR, "Invalid texture mag filter: 0x%X", value);
      break;
   case Tex::IntParam::kSwizzleR:
   case Tex::IntParam::kSwizzleG:
   case Tex::IntParam::kSwizzleB:
   case Tex::IntParam::kSwizzleA:
      verifySwizzleValue(value);
      break;
   case Tex::IntParam::kWrapS:
   case Tex::IntParam::kWrapT:
   case Tex::IntParam::kWrapR:
      ASSERT(value == GL_CLAMP_TO_EDGE || value == GL_CLAMP_TO_BORDER || value == GL_MIRRORED_REPEAT ||
             value == GL_REPEAT, "Invalid texture wrap value: 0x%X", value);
      break;
   default:
      ASSERT(false, "Invalid texture int param: 0x%X", param);
      break;
   }
#endif // SHINY_DEBUG
}

void verifyParam(Tex::Target target, Tex::FloatArrayParam param, glm::vec4 value) {
#if SHINY_DEBUG
   verifyParamTarget(target);

   switch (param) {
   case Tex::FloatArrayParam::kBorderColor:
      break;
   default:
      ASSERT(false, "Invalid texture float array param: 0x%X", param);
   }
#endif // SHINY_DEBUG
}

void verifyParam(Tex::Target target, Tex::IntArrayParam param, glm::ivec4 value) {
#if SHINY_DEBUG
   verifyParamTarget(target);

   switch (param) {
   case Tex::IntArrayParam::kBorderColor:
      break;
   case Tex::IntArrayParam::kSwizzleRGBA:
      verifySwizzleValue(value.r);
      verifySwizzleValue(value.g);
      verifySwizzleValue(value.b);
      verifySwizzleValue(value.a);
      break;
   default:
      ASSERT(false, "Invalid texture int array param: 0x%X", param);
   }
#endif // SHINY_DEBUG
}

void verifyParam(Tex::Target target, Tex::InternalIntArrayParam param, glm::ivec4 value) {
#if SHINY_DEBUG
   verifyParamTarget(target);

   switch (param) {
   case Tex::InternalIntArrayParam::kBorderColor:
      break;
   default:
      ASSERT(false, "Invalid texture internal int array param: 0x%X", param);
   }
#endif // SHINY_DEBUG
}

void verifyParam(Tex::Target target, Tex::InternalUintArrayParam param, glm::uvec4 value) {
#if SHINY_DEBUG
   verifyParamTarget(target);

   switch (param) {
   case Tex::InternalUintArrayParam::kBorderColor:
      break;
   default:
      ASSERT(false, "Invalid texture internal uint array param: 0x%X", param);
   }
#endif // SHINY_DEBUG
}

} // namespace

Texture::Texture(const Tex::Specification& textureSpecification)
   : id(0), specification(textureSpecification) {
   glGenTextures(1, &id);
   updateSpecification(textureSpecification);
}

Texture::Texture(Texture &&other) {
   move(std::move(other));
}

Texture& Texture::operator=(Texture &&other) {
   release();
   move(std::move(other));
   return *this;
}

Texture::~Texture() {
   release();
}

void Texture::release() {
   glDeleteTextures(1, &id);
}

void Texture::move(Texture &&other) {
   id = other.id;
   specification = other.specification;

   other.id = 0;
}

void Texture::updateSpecification(const Tex::Specification& textureSpecification) {
   ASSERT(textureSpecification.target == specification.target, "Can not change texture target after it is initially set");
   verifySpecification(textureSpecification);

   specification = textureSpecification;
   bind();

   GLenum target = static_cast<GLenum>(specification.target);
   GLint level = specification.level;
   GLsizei samples = specification.samples;
   GLint internalFormat = static_cast<GLint>(specification.internalFormat);
   GLsizei width = specification.width;
   GLsizei height = specification.height;
   GLsizei depth = specification.depth;
   GLint border = 0;
   GLenum format = static_cast<GLenum>(specification.providedDataFormat);
   GLenum type = static_cast<GLenum>(specification.providedDataType);
   const GLvoid* data = specification.providedData;
   bool fixedSampleLocations = specification.fixedSampleLocations;
   GLuint buffer = specification.buffer;

   switch (specification.target) {
   case Tex::Target::k1d:
   case Tex::Target::kProxyTexture1d:
      glTexImage1D(target, level, internalFormat, width, border, format, type, data);
      break;
   case Tex::Target::kBuffer:
      glTexBuffer(target, internalFormat, buffer);
      break;
   case Tex::Target::k2d:
   case Tex::Target::kProxy2d:
   case Tex::Target::k1dArray:
   case Tex::Target::kProxy1dArray:
   case Tex::Target::kRectangle:
   case Tex::Target::kProxyRectangle:
      glTexImage2D(target, level, internalFormat, width, height, border, format, type, data);
      break;
   case Tex::Target::k2dMultisample:
   case Tex::Target::kProxy2dMultisample:
      glTexImage2DMultisample(target, samples, internalFormat, width, height, fixedSampleLocations);
      break;
   case Tex::Target::kCubeMap:
   case Tex::Target::kProxyCubeMap:
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, level, internalFormat, width, height, border, format, type, specification.positiveXData);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, level, internalFormat, width, height, border, format, type, specification.negativeXData);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, level, internalFormat, width, height, border, format, type, specification.positiveYData);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, level, internalFormat, width, height, border, format, type, specification.negativeYData);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, level, internalFormat, width, height, border, format, type, specification.positiveZData);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, level, internalFormat, width, height, border, format, type, specification.negativeZData);
      break;
   case Tex::Target::k3d:
   case Tex::Target::kProxy3d:
   case Tex::Target::k2dArray:
   case Tex::Target::kProxy2dArray:
      glTexImage3D(target, level, internalFormat, width, height, depth, border, format, type, data);
      break;
   case Tex::Target::k2dMultisampleArray:
   case Tex::Target::kProxy2dMultisampleArray:
      glTexImage3DMultisample(target, samples, internalFormat, width, height, depth, fixedSampleLocations);
      break;
   default:
      ASSERT(false, "Invalid texture target: %u", specification.target);
      break;
   }
}

void Texture::setParam(Tex::FloatParam param, GLfloat value) {
   verifyParam(specification.target, param, value);
   bind();

   glTexParameterf(static_cast<GLenum>(specification.target), static_cast<GLenum>(param), value);
}

void Texture::setParam(Tex::IntParam param, GLint value) {
   verifyParam(specification.target, param, value);
   bind();

   glTexParameteri(static_cast<GLenum>(specification.target), static_cast<GLenum>(param), value);
}

void Texture::setParam(Tex::FloatArrayParam param, glm::vec4 value) {
   verifyParam(specification.target, param, value);
   bind();

   glTexParameterfv(static_cast<GLenum>(specification.target), static_cast<GLenum>(param), glm::value_ptr(value));
}

void Texture::setParam(Tex::IntArrayParam param, glm::ivec4 value) {
   verifyParam(specification.target, param, value);
   bind();

   glTexParameteriv(static_cast<GLenum>(specification.target), static_cast<GLenum>(param), glm::value_ptr(value));
}

void Texture::setParam(Tex::InternalIntArrayParam param, glm::ivec4 value) {
   verifyParam(specification.target, param, value);
   bind();

   glTexParameterIiv(static_cast<GLenum>(specification.target), static_cast<GLenum>(param), glm::value_ptr(value));
}

void Texture::setParam(Tex::InternalUintArrayParam param, glm::uvec4 value) {
   verifyParam(specification.target, param, value);
   bind();

   glTexParameterIuiv(static_cast<GLenum>(specification.target), static_cast<GLenum>(param), glm::value_ptr(value));
}

void Texture::generateMipMaps() {
   ASSERT(specification.target == Tex::Target::k1d ||
          specification.target == Tex::Target::k2d ||
          specification.target == Tex::Target::k3d ||
          specification.target == Tex::Target::k1dArray ||
          specification.target == Tex::Target::k2dArray ||
          specification.target == Tex::Target::kCubeMap,
          "Invalid texture target for generating mip maps: %u", specification.target);

   bind();
   glGenerateMipmap(static_cast<GLenum>(specification.target));
}

} // namespace Shiny
