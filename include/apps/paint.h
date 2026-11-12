#ifndef APPS_PAINT_H
#define APPS_PAINT_H

#include <common/types.h>
#include <gui/window.h>

#define PAINT_COLS 84
#define PAINT_ROWS 50
#define PAINT_CELL_SIZE 4
#define PAINT_PALETTE_COUNT 7

class PaintWindow : public Window {
private:
    uint32_t canvas[PAINT_ROWS][PAINT_COLS];
    uint32_t palette[PAINT_PALETTE_COUNT];
    uint32_t currentColor;
    bool drawing;

    void PlotCell(int32_t mouseX, int32_t mouseY);

public:
    PaintWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h);
    virtual ~PaintWindow();

    void ClearCanvas();

    virtual void DrawClientArea(GraphicsContext* gc);
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);
};

#endif