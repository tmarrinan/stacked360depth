#ifndef IMAGEIO_HPP
#define IMAGEIO_HPP

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

uint8_t* iioReadImage(const char *filename, int *width, int *height, int *channels)
{
    return stbi_load(filename, width, height, channels, *channels);
}

void iioFreeImage(uint8_t *image)
{
    stbi_image_free(image);
}

#endif // IMAGEIO_HPP
