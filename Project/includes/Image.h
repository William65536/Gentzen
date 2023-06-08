#ifndef PICTURE_H
#define PICTURE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Color {
    uint8_t r, g, b;
} Color;

typedef struct Image Image;

/**
 * @param width     width of image in pixels
 * @param height    height of image in pixels
 * @param allocator memory allocator which shares the same type signature as malloc and returns NULL on failure
 * @return          opaque pointer to a newly allocated image
 * @see             Image
*/
Image *Image_new(uint32_t width, uint32_t height, void *(*allocator)(size_t size));

/**
 * @param self          pointer to pointer to image
 * @param deallocator   memory deallocator which shares the same type signature as free
 * @see                 Image
*/
void Image_delete(Image **self, void (*deallocator)(void *memory));

/**
 * @return  image width
 * @see     Image
*/
uint32_t Image_width(const Image *self);

/**
 * @return  image height
 * @see     Image
*/
uint32_t Image_height(const Image *self);

/**
 * @param x valid x-coordinate
 * @param y valid y-coordinate
 * @return  image color at given point
 * @see     Image Color
*/
Color Image_color(const Image *self, uint32_t x, uint32_t y);

/**
 * @param x     valid x-coordinate
 * @param y     valid y-coordinate
 * @param color RGB color
 * @see         Image Color
*/
void Image_point(Image *self, uint32_t x, uint32_t y, Color color);

void Image_clear(Image *self, Color color);

bool Image_to_BMP(const Image *self, FILE *file);

#endif
