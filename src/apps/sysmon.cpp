#include <apps/sysmon.h>
#include <memory/heap.h>
#include <common/string.h>
#include <gui/theme.h>

SystemMonitorWindow::SystemMonitorWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, ProgrammableIntervalTimer* timer, Desktop* desktop)
    : Window(parent, x, y, w, h, "System Monitor", COLOR_WINDOW_BG),
      timer(timer),
      desktop(desktop)
{
}

SystemMonitorWindow::~SystemMonitorWindow() {
}

void SystemMonitorWindow::DrawClientArea(GraphicsContext* gc) {
    int32_t clientX = x + WINDOW_BORDER_SIZE;
    int32_t clientY = y + WINDOW_BORDER_SIZE + WINDOW_TITLEBAR_HEIGHT;
    int32_t clientW = w - (2 * WINDOW_BORDER_SIZE);

    size_t usedMem = 0;
    size_t totalMem = 1;
    if (MemoryManager::activeMemoryManager != 0) {
        usedMem = MemoryManager::activeMemoryManager->GetUsedMemory();
        totalMem = MemoryManager::activeMemoryManager->GetTotalMemory();
        if (totalMem == 0) {
            totalMem = 1;
        }
    }

    uint32_t usedKB = (uint32_t)(usedMem / 1024);
    uint32_t totalKB = (uint32_t)(totalMem / 1024);
    uint32_t percent = (uint32_t)((usedMem * 100) / totalMem);

    char lineBuf[64];
    char numBuf[16];

    strcpy(lineBuf, "RAM Usage: ");
    int_to_str((int32_t)usedKB, numBuf);
    strcat(lineBuf, numBuf);
    strcat(lineBuf, " KB / ");
    int_to_str((int32_t)totalKB, numBuf);
    strcat(lineBuf, numBuf);
    strcat(lineBuf, " KB (");
    int_to_str((int32_t)percent, numBuf);
    strcat(lineBuf, numBuf);
    strcat(lineBuf, "%)");

    gc->DrawString(clientX + 12, clientY + 14, lineBuf, COLOR_TEXT_DARK);

    int32_t barX = clientX + 12;
    int32_t barY = clientY + 38;
    int32_t barW = clientW - 24;
    int32_t barH = 22;

    gc->FillRect(barX, barY, barW, barH, COLOR_WIDGET_BG);
    gc->DrawRect(barX, barY, barW, barH, COLOR_WIDGET_BORDER);

    int32_t fillW = ((barW - 4) * (int32_t)percent) / 100;
    if (fillW > 0) {
        uint32_t barColor = (percent > 80) ? COLOR_BTN_CLOSE : COLOR_TITLEBAR_ACTIVE;
        gc->FillRect(barX + 2, barY + 2, fillW, barH - 4, barColor);
    }

    uint32_t uptime = (timer != 0) ? timer->GetSeconds() : 0;
    strcpy(lineBuf, "System Uptime         : ");
    int_to_str((int32_t)uptime, numBuf);
    strcat(lineBuf, numBuf);
    strcat(lineBuf, " sec");
    gc->DrawString(clientX + 12, clientY + 76, lineBuf, COLOR_TEXT_DARK);

    int32_t winCount = (desktop != 0) ? desktop->GetWindowCount() : 0;
    strcpy(lineBuf, "Open Windows          : ");
    int_to_str(winCount, numBuf);
    strcat(lineBuf, numBuf);
    gc->DrawString(clientX + 12, clientY + 102, lineBuf, COLOR_TEXT_DARK);

    int32_t mx = (desktop != 0) ? desktop->GetMouseX() : 0;
    int32_t my = (desktop != 0) ? desktop->GetMouseY() : 0;
    strcpy(lineBuf, "Mouse Position (X, Y) : ");
    int_to_str(mx, numBuf);
    strcat(lineBuf, numBuf);
    strcat(lineBuf, ", ");
    int_to_str(my, numBuf);
    strcat(lineBuf, numBuf);
    gc->DrawString(clientX + 12, clientY + 128, lineBuf, COLOR_TEXT_DARK);

    gc->FillRect(clientX + 12, clientY + 158, clientW - 24, 1, COLOR_WIDGET_BORDER);
    gc->DrawString(clientX + 12, clientY + 170, "Status: 32-Bit Protected Mode Active", COLOR_START_BTN);
}