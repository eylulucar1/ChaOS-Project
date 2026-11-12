#include <apps/terminal.h>
#include <common/string.h>
#include <memory/heap.h>
#include <gui/theme.h>

TerminalWindow::TerminalWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h)
    : Window(parent, x, y, w, h, "Terminal", COLOR_WINDOW_DARK_BG),
      lineCount(0),
      inputLen(0),
      textColor(COLOR_TERMINAL_GREEN)
{
    ClearTerminal();
    PrintLine("OS_Project Terminal v1.0");
    PrintLine("Type 'help' for available commands.");
}

TerminalWindow::~TerminalWindow() {
}

void TerminalWindow::PrintLine(const char* text) {
    if (lineCount >= TERMINAL_MAX_LINES) {
        for (int32_t i = 0; i < TERMINAL_MAX_LINES - 1; i++) {
            strcpy(lines[i], lines[i + 1]);
        }
        lineCount = TERMINAL_MAX_LINES - 1;
    }

    int32_t idx = 0;
    while (text[idx] != '\0' && idx < TERMINAL_LINE_LEN - 1) {
        lines[lineCount][idx] = text[idx];
        idx++;
    }
    lines[lineCount][idx] = '\0';
    lineCount++;
}

void TerminalWindow::ClearTerminal() {
    for (int32_t i = 0; i < TERMINAL_MAX_LINES; i++) {
        lines[i][0] = '\0';
    }
    lineCount = 0;
    inputBuffer[0] = '\0';
    inputLen = 0;
}

void TerminalWindow::ExecuteCommand(const char* cmd) {
    char promptLine[TERMINAL_LINE_LEN];
    strcpy(promptLine, "> ");
    strcat(promptLine, cmd);
    PrintLine(promptLine);

    if (strcmp(cmd, "help") == 0) {
        PrintLine("Commands: help, clear, about, mem");
        PrintLine("          color, echo <text>");
    } else if (strcmp(cmd, "clear") == 0) {
        ClearTerminal();
    } else if (strcmp(cmd, "about") == 0) {
        PrintLine("OS_Project 32-Bit C++ GUI Kernel");
        PrintLine("Resolution: 1024x768x32 VESA");
    } else if (strcmp(cmd, "mem") == 0) {
        if (MemoryManager::activeMemoryManager != 0) {
            uint32_t usedKB = MemoryManager::activeMemoryManager->GetUsedMemory() / 1024;
            uint32_t freeKB = MemoryManager::activeMemoryManager->GetFreeMemory() / 1024;
            char numBuf[16];
            char memLine[TERMINAL_LINE_LEN];

            strcpy(memLine, "Used RAM: ");
            int_to_str((int32_t)usedKB, numBuf);
            strcat(memLine, numBuf);
            strcat(memLine, " KB");
            PrintLine(memLine);

            strcpy(memLine, "Free RAM: ");
            int_to_str((int32_t)freeKB, numBuf);
            strcat(memLine, numBuf);
            strcat(memLine, " KB");
            PrintLine(memLine);
        }
    } else if (strcmp(cmd, "color") == 0) {
        if (textColor == COLOR_TERMINAL_GREEN) {
            textColor = COLOR_TEXT_LIGHT;
        } else if (textColor == COLOR_TEXT_LIGHT) {
            textColor = COLOR_BTN_MINIMIZE;
        } else {
            textColor = COLOR_TERMINAL_GREEN;
        }
        PrintLine("Terminal text color changed.");
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        PrintLine(cmd + 5);
    } else if (cmd[0] != '\0') {
        PrintLine("Unknown command! Type 'help'.");
    }
}

void TerminalWindow::DrawClientArea(GraphicsContext* gc) {
    int32_t clientX = x + WINDOW_BORDER_SIZE;
    int32_t clientY = y + WINDOW_BORDER_SIZE + WINDOW_TITLEBAR_HEIGHT;
    int32_t clientW = w - (2 * WINDOW_BORDER_SIZE);
    int32_t clientH = h - (2 * WINDOW_BORDER_SIZE) - WINDOW_TITLEBAR_HEIGHT;

    gc->FillRect(clientX, clientY, clientW, clientH, COLOR_WINDOW_DARK_BG);

    for (int32_t i = 0; i < lineCount; i++) {
        gc->DrawString(clientX + 8, clientY + 8 + (i * 16), lines[i], textColor);
    }

    int32_t inputY = clientY + 8 + (lineCount * 16);
    if (inputY + 16 < clientY + clientH) {
        gc->DrawString(clientX + 8, inputY, "> ", COLOR_START_BTN_ACTIVE);
        gc->DrawString(clientX + 24, inputY, inputBuffer, COLOR_TEXT_LIGHT);
        gc->DrawChar(clientX + 24 + (inputLen * 8), inputY, '_', textColor);
    }
}

void TerminalWindow::OnKeyDown(char c) {
    if (!visible || minimized) {
        return;
    }

    if (c == '\n') {
        inputBuffer[inputLen] = '\0';
        ExecuteCommand(inputBuffer);
        inputBuffer[0] = '\0';
        inputLen = 0;
    } else if (c == '\b') {
        if (inputLen > 0) {
            inputLen--;
            inputBuffer[inputLen] = '\0';
        }
    } else if (c >= 32 && c <= 126) {
        if (inputLen < TERMINAL_INPUT_LEN - 1) {
            inputBuffer[inputLen++] = c;
            inputBuffer[inputLen] = '\0';
        }
    }
}