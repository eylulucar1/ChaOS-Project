#include <drivers/vesa.h>
#include <drivers/font.h>
#include <common/string.h>
#include <memory/heap.h>

GraphicsContext::GraphicsContext(multiboot_info_t* mbd) {
    width = (mbd && mbd->framebuffer_width > 0) ? mbd->framebuffer_width : 1024;
    height = (mbd && mbd->framebuffer_height > 0) ? mbd->framebuffer_height : 768;
    pitch = (mbd && mbd->framebuffer_pitch > 0) ? (mbd->framebuffer_pitch / 4) : width;
    bpp = (mbd && mbd->framebuffer_bpp > 0) ? mbd->framebuffer_bpp : 32;

    framebuffer = (uint32_t*)(uintptr_t)mbd->framebuffer_addr;
    backbuffer = new uint32_t[width * height];

    if (backbuffer == 0) {
        backbuffer = framebuffer;
    }
}

GraphicsContext::~GraphicsContext() {
    if (backbuffer != 0 && backbuffer != framebuffer) {
        delete[] backbuffer;
    }
}

void GraphicsContext::PutPixel(int32_t x, int32_t y, uint32_t color) {
    if (x < 0 || x >= (int32_t)width || y < 0 || y >= (int32_t)height) {
        return;
    }
    if (backbuffer == framebuffer) {
        framebuffer[y * pitch + x] = color;
    } else {
        backbuffer[y * width + x] = color;
    }
}

uint32_t GraphicsContext::GetPixel(int32_t x, int32_t y) {
    if (x < 0 || x >= (int32_t)width || y < 0 || y >= (int32_t)height) {
        return 0;
    }
    if (backbuffer == framebuffer) {
        return framebuffer[y * pitch + x];
    }
    return backbuffer[y * width + x];
}

void GraphicsContext::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (w <= 0 || h <= 0) return;

    int32_t x0 = (x < 0) ? 0 : x;
    int32_t y0 = (y < 0) ? 0 : y;
    int32_t x1 = (x + w > (int32_t)width) ? (int32_t)width : (x + w);
    int32_t y1 = (y + h > (int32_t)height) ? (int32_t)height : (y + h);

    if (x0 >= x1 || y0 >= y1) return;

    uint32_t rowWidth = (uint32_t)(x1 - x0);
    uint32_t stride = (backbuffer == framebuffer) ? pitch : width;

    for (int32_t py = y0; py < y1; py++) {
        memset32(&backbuffer[py * stride + x0], color, rowWidth);
    }
}

void GraphicsContext::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    if (w <= 0 || h <= 0) return;
    FillRect(x, y, w, 1, color);
    FillRect(x, y + h - 1, w, 1, color);
    FillRect(x, y, 1, h, color);
    FillRect(x + w - 1, y, 1, h, color);
}

void GraphicsContext::DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
    int32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    int32_t sx = (x0 < x1) ? 1 : -1;
    int32_t dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
    int32_t sy = (y0 < y1) ? 1 : -1;
    int32_t err = dx + dy;

    while (true) {
        PutPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int32_t e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void GraphicsContext::DrawChar(int32_t x, int32_t y, char c, uint32_t color) {
    if (c < 32 || c > 126) return;
    for (uint8_t row = 0; row < 16; row++) {
        uint8_t rowBits = GetFontRow(c, row);
        for (uint8_t col = 0; col < 8; col++) {
            if ((rowBits >> (7 - col)) & 0x01) {
                PutPixel(x + col, y + row, color);
            }
        }
    }
}

void GraphicsContext::DrawString(int32_t x, int32_t y, const char* str, uint32_t color) {
    if (!str) return;
    int32_t cx = x;
    int32_t cy = y;
    while (*str) {
        if (*str == '\n') {
            cx = x;
            cy += 16;
        } else {
            DrawChar(cx, cy, *str, color);
            cx += 8;
        }
        str++;
    }
}

void GraphicsContext::Clear(uint32_t color) {
    FillRect(0, 0, width, height, color);
}

void GraphicsContext::SwapBuffers() {
    if (backbuffer == framebuffer) return;

    if (pitch == width) {
        memcpy(framebuffer, backbuffer, width * height * 4);
    } else {
        for (uint32_t y = 0; y < height; y++) {
            memcpy(&framebuffer[y * pitch], &backbuffer[y * width], width * 4);
        }
    }
}

uint32_t GraphicsContext::GetWidth() {
    return width;
}

uint32_t GraphicsContext::GetHeight() {
    return height;
}