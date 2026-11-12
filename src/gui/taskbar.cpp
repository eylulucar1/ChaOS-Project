#include <gui/taskbar.h>
#include <common/string.h>

Taskbar::Taskbar(Widget* parent, ProgrammableIntervalTimer* timer)
    : Widget(parent, 0, SCREEN_HEIGHT - TASKBAR_HEIGHT, SCREEN_WIDTH, TASKBAR_HEIGHT, COLOR_TASKBAR_BG),
      startMenuOpen(false),
      hoveredItem(-1),
      timer(timer)
{
    for (int32_t i = 0; i < 4; i++) {
        appWindows[i] = 0;
        appLabels[i] = 0;
    }
}

Taskbar::~Taskbar() {
}

void Taskbar::RegisterAppWindow(int32_t index, Window* win, const char* label) {
    if (index >= 0 && index < 4) {
        appWindows[index] = win;
        appLabels[index] = label;
    }
}

bool Taskbar::IsStartMenuOpen() {
    return startMenuOpen;
}

void Taskbar::CloseStartMenu() {
    startMenuOpen = false;
    hoveredItem = -1;
}

bool Taskbar::ContainsCoordinate(int32_t x, int32_t y) {
    if (x >= this->x && x < this->x + this->w && y >= this->y && y < this->y + this->h) {
        return true;
    }

    if (startMenuOpen) {
        int32_t menuX = 6;
        int32_t menuY = this->y - START_MENU_HEIGHT - 4;
        if (x >= menuX && x < menuX + START_MENU_WIDTH && y >= menuY && y < menuY + START_MENU_HEIGHT) {
            return true;
        }
    }

    return false;
}

void Taskbar::Draw(GraphicsContext* gc) {
    gc->FillRect(0, y, w, h, COLOR_TASKBAR_BG);
    gc->FillRect(0, y, w, 2, COLOR_TASKBAR_TOP_LINE);

    uint32_t startBtnColor = startMenuOpen ? COLOR_START_BTN_ACTIVE : COLOR_START_BTN;
    gc->FillRect(6, y + 6, START_BTN_WIDTH, START_BTN_HEIGHT, startBtnColor);
    gc->DrawRect(6, y + 6, START_BTN_WIDTH, START_BTN_HEIGHT, COLOR_TEXT_LIGHT);
    gc->DrawString(28, y + 13, "START", COLOR_TEXT_LIGHT);

    int32_t tabX = 114;
    for (int32_t i = 0; i < 4; i++) {
        if (appWindows[i] != 0 && appWindows[i]->IsVisible()) {
            uint32_t tabColor = (!appWindows[i]->IsMinimized() && appWindows[i]->IsActive())
                                ? COLOR_TITLEBAR_ACTIVE
                                : COLOR_START_MENU_HOVER;
            gc->FillRect(tabX, y + 6, 130, 30, tabColor);
            gc->DrawRect(tabX, y + 6, 130, 30, COLOR_TASKBAR_TOP_LINE);
            if (appLabels[i] != 0) {
                gc->DrawString(tabX + 10, y + 13, appLabels[i], COLOR_TEXT_LIGHT);
            }
            tabX += 136;
        }
    }

    uint32_t totalSeconds = (timer != 0) ? timer->GetSeconds() : 0;
    uint32_t hours = (totalSeconds / 3600) % 24;
    uint32_t minutes = (totalSeconds / 60) % 60;
    uint32_t seconds = totalSeconds % 60;

    char timeStr[9];
    timeStr[0] = '0' + (hours / 10);
    timeStr[1] = '0' + (hours % 10);
    timeStr[2] = ':';
    timeStr[3] = '0' + (minutes / 10);
    timeStr[4] = '0' + (minutes % 10);
    timeStr[5] = ':';
    timeStr[6] = '0' + (seconds / 10);
    timeStr[7] = '0' + (seconds % 10);
    timeStr[8] = '\0';

    int32_t clockX = w - 88;
    gc->FillRect(clockX, y + 6, 80, 30, COLOR_START_MENU_BG);
    gc->DrawRect(clockX, y + 6, 80, 30, COLOR_TASKBAR_TOP_LINE);
    gc->DrawString(clockX + 8, y + 13, timeStr, COLOR_TEXT_LIGHT);

    if (startMenuOpen) {
        int32_t menuX = 6;
        int32_t menuY = y - START_MENU_HEIGHT - 4;

        gc->FillRect(menuX + 4, menuY + 4, START_MENU_WIDTH, START_MENU_HEIGHT, COLOR_WINDOW_SHADOW);
        gc->FillRect(menuX, menuY, START_MENU_WIDTH, START_MENU_HEIGHT, COLOR_START_MENU_BG);
        gc->DrawRect(menuX, menuY, START_MENU_WIDTH, START_MENU_HEIGHT, COLOR_START_MENU_BORDER);

        gc->FillRect(menuX + 2, menuY + 2, START_MENU_WIDTH - 4, 28, COLOR_TITLEBAR_ACTIVE);
        gc->DrawString(menuX + 12, menuY + 8, "OS_Project Menu", COLOR_TEXT_LIGHT);

        for (int32_t i = 0; i < 4; i++) {
            int32_t itemY = menuY + 36 + (i * 36);
            if (hoveredItem == i) {
                gc->FillRect(menuX + 4, itemY, START_MENU_WIDTH - 8, 32, COLOR_START_MENU_HOVER);
            }
            if (appLabels[i] != 0) {
                gc->DrawString(menuX + 16, itemY + 8, appLabels[i], COLOR_TEXT_LIGHT);
            }
        }

        int32_t hideAllY = menuY + 36 + (4 * 36);
        if (hoveredItem == 4) {
            gc->FillRect(menuX + 4, hideAllY, START_MENU_WIDTH - 8, 32, COLOR_BTN_CLOSE);
        }
        gc->DrawString(menuX + 16, hideAllY + 8, "Minimize All", COLOR_TEXT_LIGHT);
    }
}

void Taskbar::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    if (button != 1) {
        return;
    }

    if (x >= 6 && x < 6 + START_BTN_WIDTH && y >= this->y + 6 && y < this->y + 6 + START_BTN_HEIGHT) {
        startMenuOpen = !startMenuOpen;
        return;
    }

    if (startMenuOpen) {
        int32_t menuX = 6;
        int32_t menuY = this->y - START_MENU_HEIGHT - 4;

        if (x >= menuX && x < menuX + START_MENU_WIDTH && y >= menuY && y < menuY + START_MENU_HEIGHT) {
            for (int32_t i = 0; i < 4; i++) {
                int32_t itemY = menuY + 36 + (i * 36);
                if (y >= itemY && y < itemY + 32) {
                    if (appWindows[i] != 0) {
                        appWindows[i]->SetVisible(true);
                        appWindows[i]->SetMinimized(false);
                        if (parent != 0) {
                            parent->GetFocus(appWindows[i]);
                        }
                    }
                    startMenuOpen = false;
                    return;
                }
            }

            int32_t hideAllY = menuY + 36 + (4 * 36);
            if (y >= hideAllY && y < hideAllY + 32) {
                for (int32_t i = 0; i < 4; i++) {
                    if (appWindows[i] != 0) {
                        appWindows[i]->SetMinimized(true);
                    }
                }
                startMenuOpen = false;
                return;
            }
        }
    }

    int32_t tabX = 114;
    for (int32_t i = 0; i < 4; i++) {
        if (appWindows[i] != 0 && appWindows[i]->IsVisible()) {
            if (x >= tabX && x < tabX + 130 && y >= this->y + 6 && y < this->y + 36) {
                if (appWindows[i]->IsMinimized()) {
                    appWindows[i]->SetMinimized(false);
                    if (parent != 0) {
                        parent->GetFocus(appWindows[i]);
                    }
                } else if (appWindows[i]->IsActive()) {
                    appWindows[i]->SetMinimized(true);
                } else {
                    if (parent != 0) {
                        parent->GetFocus(appWindows[i]);
                    }
                }
                startMenuOpen = false;
                return;
            }
            tabX += 136;
        }
    }

    startMenuOpen = false;
}

void Taskbar::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    (void)oldx;
    (void)oldy;
    hoveredItem = -1;

    if (!startMenuOpen) {
        return;
    }

    int32_t menuX = 6;
    int32_t menuY = this->y - START_MENU_HEIGHT - 4;

    if (newx >= menuX && newx < menuX + START_MENU_WIDTH) {
        for (int32_t i = 0; i < 5; i++) {
            int32_t itemY = menuY + 36 + (i * 36);
            if (newy >= itemY && newy < itemY + 32) {
                hoveredItem = i;
                break;
            }
        }
    }
}