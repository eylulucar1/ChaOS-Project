#ifndef GUI_WIDGET_H
#define GUI_WIDGET_H

#include <common/types.h>
#include <drivers/vesa.h>
#include <drivers/keyboard.h>

class Widget : public KeyboardEventHandler {
protected:
    Widget* parent;
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    uint32_t color;
    bool Focussable;

public:
    Widget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    virtual ~Widget();

    virtual void GetFocus(Widget* widget);
    virtual void ModelToScreen(int32_t &x, int32_t &y);
    virtual bool ContainsCoordinate(int32_t x, int32_t y);

    virtual void Draw(GraphicsContext* gc);
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);

    int32_t GetX();
    int32_t GetY();
    int32_t GetWidth();
    int32_t GetHeight();
    void SetColor(uint32_t newColor);
};

class CompositeWidget : public Widget {
protected:
    Widget* children[64];
    int32_t numChildren;
    Widget* focusedChild;

public:
    CompositeWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    virtual ~CompositeWidget();

    virtual void GetFocus(Widget* widget);
    virtual bool AddChild(Widget* child);

    virtual void Draw(GraphicsContext* gc);
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);

    virtual void OnKeyDown(char c);
    virtual void OnKeyUp(char c);
};

class ButtonWidget : public Widget {
protected:
    const char* label;
    bool pressed;
    bool hovered;
    void (*onClickHandler)();

public:
    ButtonWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, const char* label, void (*onClick)() = 0);
    virtual ~ButtonWidget();

    virtual void Draw(GraphicsContext* gc);
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseUp(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);
};

#endif