#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include <common/types.h>
#include <gui/widget.h>
#include <gui/theme.h>

class Window : public CompositeWidget {
protected:
    bool Dragging;
    int32_t dragOffsetX;
    int32_t dragOffsetY;
    const char* title;
    uint32_t titleColor;
    bool visible;
    bool minimized;
    bool active;

public:
    Window(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, const char* title, uint32_t color);
    virtual ~Window();

    virtual void Draw(GraphicsContext* gc);
    virtual void DrawClientArea(GraphicsContext* gc);

    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);

    void SetVisible(bool val);
    bool IsVisible();
    void SetMinimized(bool val);
    bool IsMinimized();
    void SetActive(bool val);
    bool IsActive();
    const char* GetTitle();
};

#endif