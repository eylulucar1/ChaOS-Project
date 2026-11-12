#include <apps/notepad.h>
#include <common/string.h>
#include <gui/theme.h>

NotepadWindow::NotepadWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h)
    : Window(parent, x, y, w, h, "Notepad", COLOR_WINDOW_BG),
      length(0)
{
    ClearContent();
    SetText("OS_Project Notepad\nType your notes here...\n");
}

NotepadWindow::~NotepadWindow() {
}

void NotepadWindow::ClearContent() {
    for (int32_t i = 0; i < NOTEPAD_BUFFER_SIZE; i++) {
        content[i] = '\0';
    }
    length = 0;
}

void NotepadWindow::SetText(const char* text) {
    ClearContent();
    if (text == 0) {
        return;
    }
    while (text[length] != '\0' && length < NOTEPAD_BUFFER_SIZE - 1) {
        content[length] = text[length];
        length++;
    }
    content[length] = '\0';
}

void NotepadWindow::DrawClientArea(GraphicsContext* gc) {
    int32_t clientX = x + WINDOW_BORDER_SIZE;
    int32_t clientY = y + WINDOW_BORDER_SIZE + WINDOW_TITLEBAR_HEIGHT;
    int32_t clientW = w - (2 * WINDOW_BORDER_SIZE);
    int32_t clientH = h - (2 * WINDOW_BORDER_SIZE) - WINDOW_TITLEBAR_HEIGHT;

    gc->FillRect(clientX, clientY, clientW, 24, COLOR_WIDGET_BG);
    gc->FillRect(clientX, clientY + 24, clientW, 1, COLOR_WIDGET_BORDER);

    char infoText[40];
    char numBuf[16];
    strcpy(infoText, "Character Count: ");
    int_to_str(length, numBuf);
    strcat(infoText, numBuf);
    gc->DrawString(clientX + 8, clientY + 4, infoText, COLOR_TEXT_DARK);

    int32_t textStartX = clientX + 8;
    int32_t textStartY = clientY + 32;
    int32_t maxCols = (clientW - 16) / 8;
    int32_t maxRows = (clientH - 40) / 16;

    int32_t col = 0;
    int32_t row = 0;

    for (int32_t i = 0; i < length; i++) {
        if (content[i] == '\n') {
            col = 0;
            row++;
        } else {
            if (row < maxRows) {
                gc->DrawChar(textStartX + (col * 8), textStartY + (row * 16), content[i], COLOR_TEXT_DARK);
            }
            col++;
            if (col >= maxCols) {
                col = 0;
                row++;
            }
        }
    }

    if (row < maxRows) {
        gc->DrawChar(textStartX + (col * 8), textStartY + (row * 16), '_', COLOR_TITLEBAR_ACTIVE);
    }
}

void NotepadWindow::OnKeyDown(char c) {
    if (!visible || minimized) {
        return;
    }

    if (c == '\b') {
        if (length > 0) {
            length--;
            content[length] = '\0';
        }
    } else if (c == '\n' || (c >= 32 && c <= 126)) {
        if (length < NOTEPAD_BUFFER_SIZE - 1) {
            content[length++] = c;
            content[length] = '\0';
        }
    }
}