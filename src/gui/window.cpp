#include <gui/window.h>
#include <common/string.h>

Window::Window(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, const char* title, uint32_t color)
    : CompositeWidget(parent, x, y, w, h, color),
      Dragging(false),
      dragOffsetX(0),
      dragOffsetY(0),
      title(title),
      titleColor(COLOR_TITLEBAR_ACTIVE),
      visible(true),
      minimized(false),
      active(false)
{
}

Window::~Window() {
}

void Window::Draw(GraphicsContext* gc) {
    if (!visible || minimized) {
        return;
    }

    gc->FillRect(x + WINDOW_SHADOW_OFFSET, y + WINDOW_SHADOW_OFFSET, w, h, COLOR_WINDOW_SHADOW);
    gc->FillRect(x, y, w, h, color);
    gc->DrawRect(x, y, w, h, COLOR_WINDOW_BORDER);
    gc->DrawRect(x + 1, y + 1, w - 2, h - 2, COLOR_WINDOW_BORDER);

    uint32_t currentTitleColor = active ? COLOR_TITLEBAR_ACTIVE : COLOR_TITLEBAR_INACTIVE;
    gc->FillRect(x + WINDOW_BORDER_SIZE, y + WINDOW_BORDER_SIZE, w - (2 * WINDOW_BORDER_SIZE), WINDOW_TITLEBAR_HEIGHT, currentTitleColor);

    if (title != 0) {
        gc->DrawString(x + 10, y + 8, title, COLOR_TITLEBAR_TEXT);
    }

    int32_t closeX = x + w - 24;
    int32_t minX = x + w - 44;
    int32_t btnY = y + 8;

    gc->FillRect(closeX, btnY, 16, 16, COLOR_BTN_CLOSE);
    gc->DrawChar(closeX + 4, btnY, 'X', COLOR_TITLEBAR_TEXT);

    gc->FillRect(minX, btnY, 16, 16, COLOR_BTN_MINIMIZE);
    gc->DrawChar(minX + 4, btnY, '-', COLOR_TEXT_DARK);

    DrawClientArea(gc);

    for (int32_t i = numChildren - 1; i >= 0; --i) {
        children[i]->Draw(gc);
    }
}

void Window::DrawClientArea(GraphicsContext* gc) {
    (void)gc;
}

void Window::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    if (!visible || minimized) {
        return;
    }

    if (button == 1) {
        int32_t closeX = this->x + this->w - 24;
        int32_t minX = this->x + this->w - 44;
        int32_t btnY = this->y + 8;

        if (x >= closeX && x < closeX + 16 && y >= btnY && y < btnY + 16) {
            visible = false;
            Dragging = false;
            return;
        }

        if (x >= minX && x < minX + 16 && y >= btnY && y < btnY + 16) {
            minimized = true;
            Dragging = false;
            return;
        }

        if (y >= this->y && y < this->y + WINDOW_TITLEBAR_HEIGHT + WINDOW_BORDER_SIZE) {
            Dragging = true;
            dragOffsetX = x - this->x;
            dragOffsetY = y - this->y;
        }
    }

    CompositeWidget::OnMouseDown(x, y, button);
}

void Window::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    Dragging = false;
    CompositeWidget::OnMouseUp(x, y, button);
}

void Window::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    if (Dragging) {
        this->x = newx - dragOffsetX;
        this->y = newy - dragOffsetY;

        if (this->x < 0) {
            this->x = 0;
        }
        if (this->y < 0) {
            this->y = 0;
        }
        if (this->x + this->w > SCREEN_WIDTH) {
            this->x = SCREEN_WIDTH - this->w;
        }
        if (this->y + WINDOW_TITLEBAR_HEIGHT > SCREEN_HEIGHT - TASKBAR_HEIGHT) {
            this->y = SCREEN_HEIGHT - TASKBAR_HEIGHT - WINDOW_TITLEBAR_HEIGHT;
        }
    }

    CompositeWidget::OnMouseMove(oldx, oldy, newx, newy);
}

void Window::SetVisible(bool val) {
    visible = val;
}

bool Window::IsVisible() {
    return visible;
}

void Window::SetMinimized(bool val) {
    minimized = val;
}

bool Window::IsMinimized() {
    return minimized;
}

void Window::SetActive(bool val) {
    active = val;
}

bool Window::IsActive() {
    return active;
}

const char* Window::GetTitle() {
    return title;
}