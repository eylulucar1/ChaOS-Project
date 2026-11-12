#include <hardware/timer.h>

ProgrammableIntervalTimer::ProgrammableIntervalTimer(InterruptManager* interrupts, uint32_t frequency)
    : InterruptHandler(interrupts, interrupts->HardwareInterruptOffset() + 0),
      ticks(0),
      frequency(frequency),
      dataPort(0x40),
      commandPort(0x43)
{
    if (this->frequency == 0) {
        this->frequency = 100;
    }

    uint32_t divisor = 1193182 / this->frequency;
    commandPort.Write(0x36);
    dataPort.Write((uint8_t)(divisor & 0xFF));
    dataPort.Write((uint8_t)((divisor >> 8) & 0xFF));
}

ProgrammableIntervalTimer::~ProgrammableIntervalTimer() {
}

uint32_t ProgrammableIntervalTimer::HandleInterrupt(uint32_t esp) {
    ticks++;
    return esp;
}

uint32_t ProgrammableIntervalTimer::GetTicks() {
    return ticks;
}

uint32_t ProgrammableIntervalTimer::GetSeconds() {
    return ticks / frequency;
}

void ProgrammableIntervalTimer::Sleep(uint32_t milliseconds) {
    uint32_t targetTicks = ticks + (milliseconds * frequency) / 1000;
    while (ticks < targetTicks) {
        __asm__ volatile("hlt");
    }
}