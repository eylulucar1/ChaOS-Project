#ifndef HARDWARE_TIMER_H
#define HARDWARE_TIMER_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>

class ProgrammableIntervalTimer : public InterruptHandler {
private:
    uint32_t ticks;
    uint32_t frequency;
    Port8Bit dataPort;
    Port8Bit commandPort;

public:
    ProgrammableIntervalTimer(InterruptManager* interrupts, uint32_t frequency = 100);
    ~ProgrammableIntervalTimer();

    virtual uint32_t HandleInterrupt(uint32_t esp);
    uint32_t GetTicks();
    uint32_t GetSeconds();
    void Sleep(uint32_t milliseconds);
};

#endif