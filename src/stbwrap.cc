#include <iostream>
#include <string>
#include <algorithm>
#include <ctype.h>
#include "stbwrap.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using std::cout;
using std::endl;
using namespace stbwrap;


#define DATA_ALIGN(x, align)   (((x) + (align) - 1) & (~(align)))

int stbwrap::get_frame_size(pixel_format fmt, int width, int height, int align)
{
    int stride[4] = {0};

    switch (fmt) {
    case PIXEL_FORAMT_GRAY:
        stride[0] = DATA_ALIGN(width, align);
        return stride[0] * height;
    
    case PIXEL_FORAMT_RGB:
        stride[0] = DATA_ALIGN(width * 3, align);
        return stride[0] * height;
    
    case PIXEL_FORAMT_RGBA:
        stride[0] = DATA_ALIGN(width * 4, align);
        return stride[0] * height;
    
    case PIXEL_FORAMT_YUV420P:
        stride[0] = DATA_ALIGN(width, align);
        stride[1] = DATA_ALIGN(width/2, align);
        stride[2] = DATA_ALIGN(width/2, align);
        return stride[0] * height + stride[1] * height/2 + stride[2] * height/2;
    
    case PIXEL_FORAMT_NV12:
        stride[0] = DATA_ALIGN(width, align);
        stride[1] = DATA_ALIGN(width, align);
        return stride[0] * height + stride[1] * height/2;

    default:
        return 0;
    }
}

bool stbwrap::frame_is_contiguous(frame *f)
{
    switch (f->fmt) {
    case PIXEL_FORAMT_GRAY:
        if (f->stride[0] == f->width)
            return true;
        break;
    
    case PIXEL_FORAMT_RGB:
        if (f->stride[0] == f->width * 3)
            return true;
        break;
    
    case PIXEL_FORAMT_RGBA:
        if (f->stride[0] == f->width * 4)
            return true;
        break;
    
    case PIXEL_FORAMT_YUV420P:
        if (f->stride[0] == f->width   && 
            f->stride[1] == f->width/2 &&
            f->stride[2] == f->width/2 &&
            (f->plane[1] - f->plane[0]) == f->stride[0] * f->height &&
            (f->plane[2] - f->plane[1]) == f->stride[0] * f->height/2)
        {
            return true;
        }
        break;

    case PIXEL_FORAMT_NV12:
        if (f->stride[0] == f->width   && 
            f->stride[1] == f->width   &&
            (f->plane[1] - f->plane[0]) == f->stride[0] * f->height)
        {
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

frame* stbwrap::frame_alloc()
{
    frame *f = new frame;
    f->data = nullptr;
    f->free_data_func = nullptr;
    for (int i= 0; i < 4; ++i) {
        f->plane[i] = nullptr;
        f->stride[i] = 0;
    }
    return f;
}

int stbwrap::frame_free(frame *f)
{
    if (f->free_data_func != nullptr) {
        f->free_data_func(f->data);
    }
    delete f;
    return 0;
}

static int image_read_common(int width, int height, int channel, uint8_t *data, frame *f)
{
    cout << "width:" << width << "  height:" << height << "  channel:" << channel << endl;

    f->width = width;
    f->height = height;
    const int pixel_num = width * height;

    switch (channel) {
        case 1: // gray
        {
            f->fmt = PIXEL_FORAMT_GRAY;
            f->stride[0] = width;
            f->plane[0] = data;
            f->data = data;
            f->free_data_func = stbi_image_free;
            break;
        }
        case 2: // gray alpha
        {
            uint8_t *new_data = new uint8_t[get_frame_size(PIXEL_FORAMT_GRAY, width, height, 1)];
            uint8_t *src = data;
            uint8_t *dst = new_data;
            for (int i = 0; i < pixel_num; ++i) {
                *dst++ = *src++;
                ++src; // ingore alpha
            }
            f->fmt = PIXEL_FORAMT_GRAY;
            f->stride[0] = width;
            f->plane[0] = new_data;
            f->data = new_data;
            f->free_data_func = [](void *data) { delete[] (uint8_t *)data; };
            stbi_image_free(data);
            break;
        }
        case 3: // rgb
        {
            f->fmt = PIXEL_FORAMT_RGB;
            f->stride[0] = width * 3;
            f->plane[0] = data;
            f->data = data;
            f->free_data_func = stbi_image_free;
            break;
        }
        case 4: // rgb alpha
        {
            uint8_t *new_data = new uint8_t[get_frame_size(PIXEL_FORAMT_RGB, width, height, 1)];
            uint8_t *src = data;
            uint8_t *dst = new_data;
            for (int i = 0; i < pixel_num; ++i) {
                *dst++ = *src++; // r
                *dst++ = *src++; // g
                *dst++ = *src++; // b
                ++src; // ingore alpha
            }
            f->fmt = PIXEL_FORAMT_RGB;
            f->stride[0] = width;
            f->plane[0] = new_data;
            f->data = new_data;
            f->free_data_func = [](void *data) { delete[] (uint8_t *)data; };
            stbi_image_free(data);
            break;
        }
        default:
            cout << "Not support chanenl:" << channel << endl;
            stbi_image_free(data);
            return -1;
    }

    return 0;
}

int stbwrap::image_read_from_memory(uint8_t *buf, int len, frame *f)
{
    int width, height, channel;
    uint8_t *data = stbi_load_from_memory(buf, len, &width, &height, &channel, 0);
    if (data == nullptr) {
        cout << "stbi_load failed" << endl;
        return -1;
    }

    return image_read_common(width, height, channel, data, f); 
}

int stbwrap::image_read(const char *filename, frame *f)
{
    int width, height, channel;
    uint8_t *data = stbi_load(filename, &width, &height, &channel, 0);
    if (data == nullptr) {
        cout << "stbi_load failed" << endl;
        return -1;
    }

    return image_read_common(width, height, channel, data, f); 
}

bool endswith(const std::string& str, const std::string& end)
{
    int srclen = str.size();
    int endlen = end.size();
    if (srclen >= endlen) {
        std::string temp = str.substr(srclen - endlen, endlen);
        if(temp == end)
            return true;
    }

    return false;
}

file_format stbwrap::guess_file_format(const char *filename)
{
    std::string str = filename;

    std::for_each(str.begin(), str.end(), [](char &c) { c = ::tolower(c); });

    if (endswith(str, std::string{".jpg"}) || endswith(str, std::string{".jpeg"})) {
        return FILE_FORMAT_JPG;
    } else if (endswith(str, std::string{".png"})) {
        return FILE_FORMAT_PNG;
    } else if (endswith(str, std::string{".bmp"})) {
        return FILE_FORMAT_BMP;
    } else {
        return FILE_FORMAT_MAX;
    }
}

int stbwrap::image_write(const char *path, frame *f)
{
    file_format file_fmt = guess_file_format(path);
    if (file_fmt == FILE_FORMAT_MAX)
        return -1;
    if (f->fmt != PIXEL_FORAMT_RGB)
        return -1;
    if (!frame_is_contiguous(f)) {
        //TODO: 如果内存不连续，可以将原数据拷贝出来，生成一个新的frame结构体
    }
    
    int ret = 0;

    switch (file_fmt) {
    case FILE_FORMAT_JPG:
        ret = stbi_write_jpg(path, f->width, f->height, 3, f->plane[0], 100);
        break;
    case FILE_FORMAT_PNG:
        ret = stbi_write_png(path, f->width, f->height, 3, f->plane[0], f->stride[0]);
        break;
    case FILE_FORMAT_BMP:
        ret = stbi_write_bmp(path, f->width, f->height, 3, f->plane[0]);
        break;
    default:
        break;
    }

    if (ret == 1)
        return 0;
    else
        return -1;
}

int stbwrap::image_show(frame *f)
{
    //TODO: 使用SDL2渲染
    return 0;
}