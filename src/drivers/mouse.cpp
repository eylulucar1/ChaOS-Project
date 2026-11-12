#include <drivers/mouse.h>

MouseEventHandler::MouseEventHandler() {
}

MouseEventHandler::~MouseEventHandler() {
}

void MouseEventHandler::OnActivate() {
}

void MouseEventHandler::OnMouseDown(uint8_t button) {
    (void)button;
}

void MouseEventHandler::OnMouseUp(uint8_t button) {
    (void)button;
}

void MouseEventHandler::OnMouseMove(int32_t xoffset, int32_t yoffset) {
    (void)xoffset;
    (void)yoffset;
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
    : InterruptHandler(manager, manager->HardwareInterruptOffset() + 0x0C),
      dataPort(0x60),
      commandPort(0x64),
      offset(0),
      buttons(0),
      handler(handler)
{
}

MouseDriver::~MouseDriver() {
}

void MouseDriver::WaitSignal() {
    uint32_t timeout = 100000;
    while (timeout--) {
        if ((commandPort.Read() & 0x02) == 0) {
            return;
        }
    }
}

void MouseDriver::WaitData() {
    uint32_t timeout = 100000;
    while (timeout--) {
        if ((commandPort.Read() & 0x01) == 1) {
            return;
        }
    }
}

void MouseDriver::WriteMouse(uint8_t data) {
    WaitSignal();
    commandPort.Write(0xD4);
    WaitSignal();
    dataPort.Write(data);
}

void MouseDriver::Activate() {
    offset = 0;
    buttons = 0;

    if (handler != 0) {
        handler->OnActivate();
    }

    while (commandPort.Read() & 0x01) {
        dataPort.Read();
    }

    WaitSignal();
    commandPort.Write(0xA8);

    WaitSignal();
    commandPort.Write(0x20);
    WaitData();
    uint8_t status = (dataPort.Read() | 0x03) & ~0x30;

    WaitSignal();
    commandPort.Write(0x60);
    WaitSignal();
    dataPort.Write(status);

    WriteMouse(0xF6);
    WaitData();
    dataPort.Read();

    WriteMouse(0xF4);
    WaitData();
    dataPort.Read();

    while (commandPort.Read() & 0x01) {
        dataPort.Read();
    }
}

uint32_t MouseDriver::HandleInterrupt(uint32_t esp) {
    uint8_t status = commandPort.Read();
    if ((status & 0x20) == 0 || (status & 0x01) == 0) {
        return esp;
    }

    uint8_t data = dataPort.Read();

    if (handler == 0) {
        return esp;
    }

    if (offset == 0 && (data & 0x08) == 0) {
        return esp;
    }

    buffer[offset] = data;
    offset = (offset + 1) % 3;

    if (offset == 0) {
        if ((buffer[0] & 0xC0) == 0) {
            int32_t dx = (int32_t)((int8_t)buffer[1]);
            int32_t dy = (int32_t)((int8_t)buffer[2]);

            if (dx != 0 || dy != 0) {
                handler->OnMouseMove(dx, -dy);
            }
        }

        for (uint8_t i = 0; i < 3; i++) {
            if ((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i))) {
                if (buttons & (0x01 << i)) {
                    handler->OnMouseUp(i + 1);
                } else {
                    handler->OnMouseDown(i + 1);
                }
            }
        }
        buttons = buffer[0];
    }

    return esp;
}