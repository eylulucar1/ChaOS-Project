#include <gui/desktop.h>
#include <gui/theme.h>

Desktop::Desktop(int32_t w, int32_t h, uint32_t color, GraphicsContext* gc)
    : CompositeWidget(0, 0, 0, w, h, color),
      MouseEventHandler(),
      MouseX(w / 2),
      MouseY(h / 2),
      gc(gc),
      taskbar(0)
{
}

Desktop::~Desktop() {
}

void Desktop::SetTaskbar(Taskbar* tb) {
    this->taskbar = tb;
}

void Desktop::DrawCursor(GraphicsContext* gc, int32_t x, int32_t y) {
    static const uint8_t cursorBitmap[16][12] = {
        {1,0,0,0,0,0,0,0,0,0,0,0},
        {1,1,0,0,0,0,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0,0,0,0,0},
        {1,2,2,1,0,0,0,0,0,0,0,0},
        {1,2,2,2,1,0,0,0,0,0,0,0},
        {1,2,2,2,2,1,0,0,0,0,0,0},
        {1,2,2,2,2,2,1,0,0,0,0,0},
        {1,2,2,2,2,2,2,1,0,0,0,0},
        {1,2,2,2,2,2,2,2,1,0,0,0},
        {1,2,2,2,2,2,2,2,2,1,0,0},
        {1,2,2,2,2,2,1,1,1,1,1,0},
        {1,2,2,1,2,2,1,0,0,0,0,0},
        {1,2,1,0,1,2,2,1,0,0,0,0},
        {1,1,0,0,1,2,2,1,0,0,0,0},
        {1,0,0,0,0,1,2,2,1,0,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0}
    };

    for (int32_t row = 0; row < 16; row++) {
        for (int32_t col = 0; col < 12; col++) {
            if (cursorBitmap[row][col] == 1) {
                gc->PutPixel(x + col, y + row, COLOR_CURSOR_OUTLINE);
            } else if (cursorBitmap[row][col] == 2) {
                gc->PutPixel(x + col, y + row, COLOR_CURSOR_FILL);
            }
        }
    }
}

void Desktop::Draw(GraphicsContext* gc) {
    if (gc == 0) {
        return;
    }

    gc->FillRect(0, 0, w, h - TASKBAR_HEIGHT, color);

    for (int32_t gy = 32; gy < h - TASKBAR_HEIGHT; gy += 32) {
        for (int32_t gx = 32; gx < w; gx += 32) {
            gc->PutPixel(gx, gy, COLOR_DESKTOP_GRID);
        }
    }

    gc->DrawString(16, 16, "OS_Project Desktop Environment", COLOR_TEXT_LIGHT);

    if (focusedChild != 0 && numChildren > 1 && children[0] != focusedChild) {
        int32_t focusIdx = -1;
        for (int32_t i = 0; i < numChildren; i++) {
            if (children[i] == focusedChild) {
                focusIdx = i;
                break;
            }
        }
        if (focusIdx > 0) {
            Widget* temp = children[focusIdx];
            for (int32_t j = focusIdx; j > 0; j--) {
                children[j] = children[j - 1];
            }
            children[0] = temp;
        }
    }

    for (int32_t i = 0; i < numChildren; i++) {
        Window* win = (Window*)children[i];
        win->SetActive(children[i] == focusedChild && win->IsVisible() && !win->IsMinimized());
    }

    for (int32_t i = numChildren - 1; i >= 0; --i) {
        children[i]->Draw(gc);
    }

    if (taskbar != 0) {
        taskbar->Draw(gc);
    }

    DrawCursor(gc, MouseX, MouseY);
}

void Desktop::OnMouseDown(uint8_t button) {
    if (taskbar != 0 && taskbar->ContainsCoordinate(MouseX, MouseY)) {
        taskbar->OnMouseDown(MouseX, MouseY, button);
        return;
    }

    if (taskbar != 0 && taskbar->IsStartMenuOpen()) {
        taskbar->CloseStartMenu();
    }

    for (int32_t i = 0; i < numChildren; i++) {
        Window* win = (Window*)children[i];
        if (!win->IsVisible() || win->IsMinimized()) {
            continue;
        }

        if (win->ContainsCoordinate(MouseX, MouseY)) {
            if (i > 0) {
                for (int32_t j = i; j > 0; j--) {
                    children[j] = children[j - 1];
                }
                children[0] = win;
            }
            focusedChild = win;
            win->OnMouseDown(MouseX, MouseY, button);
            return;
        }
    }
}

void Desktop::OnMouseUp(uint8_t button) {
    if (focusedChild != 0) {
        focusedChild->OnMouseUp(MouseX, MouseY, button);
    }
}

void Desktop::OnMouseMove(int32_t xoffset, int32_t yoffset) {
    int32_t oldMouseX = MouseX;
    int32_t oldMouseY = MouseY;

    MouseX += xoffset;
    MouseY += yoffset;

    if (MouseX < 0) {
        MouseX = 0;
    }
    if (MouseX >= w) {
        MouseX = w - 1;
    }
    if (MouseY < 0) {
        MouseY = 0;
    }
    if (MouseY >= h) {
        MouseY = h - 1;
    }

    if (taskbar != 0) {
        taskbar->OnMouseMove(oldMouseX, oldMouseY, MouseX, MouseY);
    }

    if (focusedChild != 0) {
        focusedChild->OnMouseMove(oldMouseX, oldMouseY, MouseX, MouseY);
    }
}

int32_t Desktop::GetMouseX() {
    return MouseX;
}

int32_t Desktop::GetMouseY() {
    return MouseY;
}

int32_t Desktop::GetWindowCount() {
    int32_t count = 0;
    for (int32_t i = 0; i < numChildren; i++) {
        Window* win = (Window*)children[i];
        if (win->IsVisible()) {
            count++;
        }
    }
    return count;
}