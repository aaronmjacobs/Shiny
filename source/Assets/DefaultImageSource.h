#ifndef SHINY_DEFAULT_IMAGE_SOURCE_H
#define SHINY_DEFAULT_IMAGE_SOURCE_H

#include <cstdlib>

namespace Shiny {

namespace {

/**
 * Simple gray / blue 16x16 checkered image (TGA)
 */
const unsigned char kDefaultImageSource[] = {
   0x50, 0x36, 0x0A, 0x31, 0x36, 0x20, 0x31, 0x36, 0x0A, 0x32, 0x35, 0x35, 
   0x0A, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 0x3F, 0x3D, 0x3D, 
   0x3F, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 0xD5, 0x00, 0xAA, 
   0xD5
};

const size_t kDefaultImageSourceSize = sizeof(kDefaultImageSource) / sizeof(unsigned char);

} // namespace

} // namespace Shiny

#endif
