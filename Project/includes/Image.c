#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "Image.h"

struct Image {
    uint32_t width;
    uint32_t height;
    uint8_t data[];
};

Image *Image_new(uint32_t width, uint32_t height, void *(*allocator)(size_t))
{
    Image *ret = allocator(sizeof *ret + width * height * 3 * sizeof *ret->data);

    if (ret == NULL)
        return NULL;

    ret->width = width;
    ret->height = height;

    memset(ret->data, 0xff, ret->width * ret->height * 3 * sizeof *ret->data);

    return ret;
}

void Image_delete(Image **self, void (*deallocator)(void *))
{
    assert(self != NULL && *self != NULL);

    deallocator(*self);
    
    *self = NULL;
}

uint32_t Image_width(const Image *self)
{
    assert(self != NULL);

    return self->width;
}

uint32_t Image_height(const Image *self)
{
    assert(self != NULL);

    return self->height;
}

Color Image_color(const Image *self, uint32_t x, uint32_t y)
{
    assert(self != NULL);
    assert(x < self->width);
    assert(y < self->height);

    return (Color) {
        .r = self->data[(y * self->width + x) * 3 + 0],
        .g = self->data[(y * self->width + x) * 3 + 1],
        .b = self->data[(y * self->width + x) * 3 + 2]
    };
}

void Image_point(Image *self, uint32_t x, uint32_t y, Color color)
{
    assert(self != NULL);
    assert(x < self->width);
    assert(y < self->height);

    self->data[(y * self->width + x) * 3 + 0] = color.r;
    self->data[(y * self->width + x) * 3 + 1] = color.g;
    self->data[(y * self->width + x) * 3 + 2] = color.b;
}

void Image_clear(Image *self, Color color)
{
    assert(self != NULL);

    for (uint32_t x = 0; x < self->width; x++) {
        for (uint32_t y = 0; y < self->height; y++) {
            self->data[(y * self->width + x) * 3 + 0] = color.r;
            self->data[(y * self->width + x) * 3 + 1] = color.g;
            self->data[(y * self->width + x) * 3 + 2] = color.b;
        }
    }
}

bool Image_to_BMP(const Image *self, FILE *file)
{
    assert(self != NULL);
    assert(file != NULL);

    const uint32_t paddingamount = (4 - self->width * 3 % 4) % 4;
    const uint32_t headersize = 14;
    const uint32_t infoheadersize = 40;
    const uint32_t pixelarraysize = self->height * (self->width * 3 + paddingamount);
    const uint32_t filesize = headersize + infoheadersize + pixelarraysize;

    const uint8_t header[] = {
        /* File type */             'B', 'M',
        /* File size */             filesize, filesize >> 8, filesize >> 16, filesize >> 24,
        /* Reserved 1 */            0, 0,
        /* Reserved 2 */            0, 0,
        /* Pixel data offset */     headersize + infoheadersize, 0, 0, 0
    };

    const uint8_t infoheader[] = {
        /* Header size */           infoheadersize, 0, 0, 0,
        /* Image width */           self->width, self->width >> 8, self->width >> 16, self->width >> 24,
        /* Image height */          self->height, self->height >> 8, self->height >> 16, self->height >> 24,
        /* Planes */                1, 0,
        /* Bits per pixel */        24, 0,
        /* Compression */           0, 0, 0, 0,
        /* Image size */            0, 0, 0, 0,
        /* X pixels per meter */    0, 0, 0, 0,
        /* Y pixels per meter */    0, 0, 0, 0,
        /* Total colors */          0, 0, 0, 0,
        /* Important colors */      0, 0, 0, 0
    };

    if (fwrite(header, sizeof header, 1, file) != 1)
        return false;

    if (fwrite(infoheader, sizeof infoheader, 1, file) != 1)
        return false;

    const uint8_t zeros[4] = { 0 };

    for (uint32_t y = 0; y < self->height; y++) {
        for (uint32_t x = 0; x < self->width; x++) {
            for (uint32_t i = 0; i < 3; i++)
                if (fputc(self->data[((self->height - 1 - y) * self->width + x) * 3 + 3 - 1 - i], file) == EOF)
                    return false;

            if (paddingamount != 0 && fwrite(zeros, paddingamount, 1, file) != 1)
                return false;
        }
    }

    return true;
}
