#ifndef DRIVERS_VESA_H
#define DRIVERS_VESA_H

#include <common/types.h>

class GraphicsContext {
private:
    uint32_t* framebuffer;
    uint32_t* backbuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;

public:
    GraphicsContext(multiboot_info_t* mbd);
    ~GraphicsContext();

    void PutPixel(int32_t x, int32_t y, uint32_t color);
    uint32_t GetPixel(int32_t x, int32_t y);
    void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void DrawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color);
    void DrawChar(int32_t x, int32_t y, char c, uint32_t color);
    void DrawString(int32_t x, int32_t y, const char* str, uint32_t color);
    void Clear(uint32_t color);
    void SwapBuffers();

    uint32_t GetWidth();
    uint32_t GetHeight();
};

#endif