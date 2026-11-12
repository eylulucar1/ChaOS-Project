#ifndef GUI_TASKBAR_H
#define GUI_TASKBAR_H

#include <common/types.h>
#include <gui/widget.h>
#include <gui/window.h>
#include <gui/theme.h>
#include <hardware/timer.h>

class Taskbar : public Widget {
private:
    bool startMenuOpen;
    int32_t hoveredItem;
    ProgrammableIntervalTimer* timer;
    Window* appWindows[4];
    const char* appLabels[4];

public:
    Taskbar(Widget* parent, ProgrammableIntervalTimer* timer);
    virtual ~Taskbar();

    void RegisterAppWindow(int32_t index, Window* win, const char* label);
    bool IsStartMenuOpen();
    void CloseStartMenu();

    virtual bool ContainsCoordinate(int32_t x, int32_t y);
    virtual void Draw(GraphicsContext* gc);
    virtual void OnMouseDown(int32_t x, int32_t y, uint8_t button);
    virtual void OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy);
};

#endif