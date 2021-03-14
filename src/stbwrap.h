#pragma once
#include <functional>
#include <stdint.h>

namespace stbwrap {

enum file_format {
    FILE_FORMAT_JPG = 0,
    FILE_FORMAT_PNG,
    FILE_FORMAT_BMP,
    FILE_FORMAT_MAX
};

enum pixel_format {
    PIXEL_FORAMT_GRAY = 0,  //8bpp
    PIXEL_FORAMT_RGB,       //24bpp
    PIXEL_FORAMT_RGBA,      //32bpp
    PIXEL_FORAMT_YUV420P,   //12bpp (alias YU12,IYUV,I420)
    PIXEL_FORAMT_NV12,      //12bpp
    PIXEL_FORAMT_MAX
};

struct frame
{
    pixel_format fmt;
    int width;
    int height;
    int stride[4];
    uint8_t *plane[4];
    uint8_t *data;
    std::function<void (void *)> free_data_func;
};

int get_frame_size(pixel_format fmt, int width, int height, int align);
bool frame_is_contiguous(frame *f);
file_format guess_file_format(const char *filename);

frame* frame_alloc();
int frame_free(frame *f);

int image_read(const char *filename, frame *f);              // PNG的透明通道将忽略
int image_read_from_memory(uint8_t *buf, int len, frame *f); // PNG的透明通道将忽略
int image_write(const char *path, frame *f);                 // 仅支持RGB像素格式存储到文件
int image_show(frame *f);

}
