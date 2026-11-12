#include <hardware/interrupts.h>

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

InterruptHandler::InterruptHandler(InterruptManager* mgr, uint8_t num) {
    this->InterruptNumber = num;
    this->interruptManager = mgr;
    mgr->handlers[num] = this;
}

InterruptHandler::~InterruptHandler() {
    if (interruptManager->handlers[InterruptNumber] == this) {
        interruptManager->handlers[InterruptNumber] = 0;
    }
}

uint32_t InterruptHandler::HandleInterrupt(uint32_t esp) {
    return esp;
}

void InterruptManager::SetInterruptDescriptorTableEntry(
    uint8_t interrupt,
    uint16_t csOffset,
    void (*handler)(),
    uint8_t dpl,
    uint8_t descType)
{
    uint32_t addr = (uint32_t)handler;
    interruptDescriptorTable[interrupt].handlerAddressLowBits = addr & 0xFFFF;
    interruptDescriptorTable[interrupt].handlerAddressHighBits = (addr >> 16) & 0xFFFF;
    interruptDescriptorTable[interrupt].gdt_codeSegmentSelector = csOffset;
    interruptDescriptorTable[interrupt].access = 0x80 | ((dpl & 3) << 5) | descType;
    interruptDescriptorTable[interrupt].reserved = 0;
}

InterruptManager::InterruptManager(uint16_t offset, GlobalDescriptorTable* gdt)
    : picMasterCommand(0x20),
      picMasterData(0x21),
      picSlaveCommand(0xA0),
      picSlaveData(0xA1)
{
    this->hardwareInterruptOffset = offset;
    uint16_t codeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++) {
        handlers[i] = 0;
        SetInterruptDescriptorTableEntry(i, codeSegment, &InterruptIgnore, 0, IDT_GATE);
    }

    void (*excHandlers[20])() = {
        &HandleException0x00, &HandleException0x01, &HandleException0x02, &HandleException0x03,
        &HandleException0x04, &HandleException0x05, &HandleException0x06, &HandleException0x07,
        &HandleException0x08, &HandleException0x09, &HandleException0x0A, &HandleException0x0B,
        &HandleException0x0C, &HandleException0x0D, &HandleException0x0E, &HandleException0x0F,
        &HandleException0x10, &HandleException0x11, &HandleException0x12, &HandleException0x13
    };

    for (uint8_t i = 0; i < 20; i++) {
        SetInterruptDescriptorTableEntry(i, codeSegment, excHandlers[i], 0, IDT_GATE);
    }

    void (*irqHandlers[16])() = {
        &HandleInterruptRequest0x00, &HandleInterruptRequest0x01, &HandleInterruptRequest0x02, &HandleInterruptRequest0x03,
        &HandleInterruptRequest0x04, &HandleInterruptRequest0x05, &HandleInterruptRequest0x06, &HandleInterruptRequest0x07,
        &HandleInterruptRequest0x08, &HandleInterruptRequest0x09, &HandleInterruptRequest0x0A, &HandleInterruptRequest0x0B,
        &HandleInterruptRequest0x0C, &HandleInterruptRequest0x0D, &HandleInterruptRequest0x0E, &HandleInterruptRequest0x0F
    };

    for (uint8_t i = 0; i < 16; i++) {
        SetInterruptDescriptorTableEntry(offset + i, codeSegment, irqHandlers[i], 0, IDT_GATE);
    }

    SetInterruptDescriptorTableEntry(offset + 0x31, codeSegment, &HandleInterruptRequest0x31, 0, IDT_GATE);

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterData.Write(offset);
    picSlaveData.Write(offset + 8);

    picMasterData.Write(0x04);
    picSlaveData.Write(0x02);

    picMasterData.Write(0x01);
    picSlaveData.Write(0x01);

    picMasterData.Write(0x00);
    picSlaveData.Write(0x00);

    InterruptDescriptorTablePointer idtPtr;
    idtPtr.size = 256 * sizeof(GateDescriptor) - 1;
    idtPtr.base = (uint32_t)interruptDescriptorTable;
    __asm__ volatile("lidt %0" : : "m" (idtPtr));
}

InterruptManager::~InterruptManager() {
    Deactivate();
}

uint16_t InterruptManager::HardwareInterruptOffset() {
    return hardwareInterruptOffset;
}

void InterruptManager::Activate() {
    if (ActiveInterruptManager != 0) {
        ActiveInterruptManager->Deactivate();
    }
    ActiveInterruptManager = this;
    __asm__ volatile("sti");
}

void InterruptManager::Deactivate() {
    if (ActiveInterruptManager == this) {
        ActiveInterruptManager = 0;
        __asm__ volatile("cli");
    }
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp) {
    if (ActiveInterruptManager != 0) {
        return ActiveInterruptManager->DoHandleInterrupt(interrupt, esp);
    }
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint32_t esp) {
    if (handlers[interrupt] != 0) {
        esp = handlers[interrupt]->HandleInterrupt(esp);
    }

    if (hardwareInterruptOffset <= interrupt && interrupt < hardwareInterruptOffset + 16) {
        picMasterCommand.Write(0x20);
        if (hardwareInterruptOffset + 8 <= interrupt) {
            picSlaveCommand.Write(0x20);
        }
    }

    return esp;
}