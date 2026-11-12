#include <drivers/keyboard.h>

KeyboardEventHandler::KeyboardEventHandler() {
}

KeyboardEventHandler::~KeyboardEventHandler() {
}

void KeyboardEventHandler::OnKeyDown(char c) {
    (void)c;
}

void KeyboardEventHandler::OnKeyUp(char c) {
    (void)c;
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
    : InterruptHandler(manager, manager->HardwareInterruptOffset() + 0x01),
      dataPort(0x60),
      commandPort(0x64),
      handler(handler),
      shiftPressed(false),
      capsLock(false)
{
}

KeyboardDriver::~KeyboardDriver() {
}

void KeyboardDriver::Activate() {
    while (commandPort.Read() & 0x01) {
        dataPort.Read();
    }

    uint32_t timeout = 100000;
    while ((commandPort.Read() & 0x02) && timeout--);
    commandPort.Write(0xAE);

    timeout = 100000;
    while ((commandPort.Read() & 0x02) && timeout--);
    commandPort.Write(0x20);

    timeout = 100000;
    while (((commandPort.Read() & 0x01) == 0) && timeout--);
    uint8_t status = (dataPort.Read() | 0x01) & ~0x10;

    timeout = 100000;
    while ((commandPort.Read() & 0x02) && timeout--);
    commandPort.Write(0x60);

    timeout = 100000;
    while ((commandPort.Read() & 0x02) && timeout--);
    dataPort.Write(status);

    timeout = 100000;
    while ((commandPort.Read() & 0x02) && timeout--);
    dataPort.Write(0xF4);

    timeout = 100000;
    while (((commandPort.Read() & 0x01) == 0) && timeout--);
    dataPort.Read();

    while (commandPort.Read() & 0x01) {
        dataPort.Read();
    }
}

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp) {
    uint8_t key = dataPort.Read();

    if (handler == 0) {
        return esp;
    }

    if (key == 0x2A || key == 0x36) {
        shiftPressed = true;
        return esp;
    }
    if (key == 0xAA || key == 0xB6) {
        shiftPressed = false;
        return esp;
    }
    if (key == 0x3A) {
        capsLock = !capsLock;
        return esp;
    }

    if (key & 0x80) {
        return esp;
    }

    static const char scancodeNormal[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' '
    };

    static const char scancodeShift[128] = {
        0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
        0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
        '*', 0, ' '
    };

    if (key < 128) {
        bool upper = shiftPressed ^ capsLock;
        char c = shiftPressed ? scancodeShift[key] : scancodeNormal[key];

        if (c >= 'a' && c <= 'z' && upper) {
            c = c - 'a' + 'A';
        } else if (c >= 'A' && c <= 'Z' && !upper) {
            c = c - 'A' + 'a';
        }

        if (c != 0) {
            handler->OnKeyDown(c);
        }
    }

    return esp;
}