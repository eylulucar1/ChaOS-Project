#ifndef DRIVERS_MOUSE_H
#define DRIVERS_MOUSE_H

#include <common/types.h>
#include <hardware/port.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>

class MouseEventHandler {
public:
    MouseEventHandler();
    virtual ~MouseEventHandler();

    virtual void OnActivate();
    virtual void OnMouseDown(uint8_t button);
    virtual void OnMouseUp(uint8_t button);
    virtual void OnMouseMove(int32_t xoffset, int32_t yoffset);
};

class MouseDriver : public InterruptHandler, public Driver {
private:
    Port8Bit dataPort;
    Port8Bit commandPort;
    uint8_t buffer[3];
    uint8_t offset;
    uint8_t buttons;
    MouseEventHandler* handler;

    void WaitSignal();
    void WaitData();
    void WriteMouse(uint8_t data);

public:
    MouseDriver(InterruptManager* manager, MouseEventHandler* handler);
    ~MouseDriver();

    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
};

#endif