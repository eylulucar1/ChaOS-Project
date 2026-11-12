#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>
#include <drivers/driver.h>

class KeyboardEventHandler {
public:
    KeyboardEventHandler();
    virtual ~KeyboardEventHandler();

    virtual void OnKeyDown(char c);
    virtual void OnKeyUp(char c);
};

class KeyboardDriver : public InterruptHandler, public Driver {
private:
    Port8Bit dataPort;
    Port8Bit commandPort;
    KeyboardEventHandler* handler;
    bool shiftPressed;
    bool capsLock;

public:
    KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler);
    ~KeyboardDriver();

    virtual uint32_t HandleInterrupt(uint32_t esp);
    virtual void Activate();
};

#endif