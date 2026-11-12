#ifndef GUI_DESKTOP_H
#define GUI_DESKTOP_H

#include <common/types.h>
#include <gui/widget.h>
#include <gui/window.h>
#include <gui/taskbar.h>
#include <drivers/mouse.h>
#include <drivers/vesa.h>

class Desktop : public CompositeWidget, public MouseEventHandler {
private:
    int32_t MouseX;
    int32_t MouseY;
    GraphicsContext* gc;
    Taskbar* taskbar;

public:
    Desktop(int32_t w, int32_t h, uint32_t color, GraphicsContext* gc);
    virtual ~Desktop();

    void SetTaskbar(Taskbar* tb);
    void DrawCursor(GraphicsContext* gc, int32_t x, int32_t y);
    virtual void Draw(GraphicsContext* gc);

    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(int32_t xoffset, int32_t yoffset);

    int32_t GetMouseX();
    int32_t GetMouseY();
    int32_t GetWindowCount();
};

#endif