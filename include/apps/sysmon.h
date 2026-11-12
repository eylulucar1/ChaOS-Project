#ifndef APPS_SYSMON_H
#define APPS_SYSMON_H

#include <common/types.h>
#include <gui/window.h>
#include <gui/desktop.h>
#include <hardware/timer.h>

class SystemMonitorWindow : public Window {
private:
    ProgrammableIntervalTimer* timer;
    Desktop* desktop;

public:
    SystemMonitorWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, ProgrammableIntervalTimer* timer, Desktop* desktop);
    virtual ~SystemMonitorWindow();

    virtual void DrawClientArea(GraphicsContext* gc);
};

#endif