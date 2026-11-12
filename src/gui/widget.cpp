#include <gui/widget.h>
#include <gui/theme.h>
#include <common/string.h>

Widget::Widget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
    : KeyboardEventHandler(),
      parent(parent),
      x(x),
      y(y),
      w(w),
      h(h),
      color(color),
      Focussable(true)
{
}

Widget::~Widget() {
}

void Widget::GetFocus(Widget* widget) {
    if (parent != 0) {
        parent->GetFocus(widget);
    }
}

void Widget::ModelToScreen(int32_t &x, int32_t &y) {
    if (parent != 0) {
        parent->ModelToScreen(x, y);
    }
    x += this->x;
    y += this->y;
}

bool Widget::ContainsCoordinate(int32_t x, int32_t y) {
    return this->x <= x && x < this->x + this->w
        && this->y <= y && y < this->y + this->h;
}

void Widget::Draw(GraphicsContext* gc) {
    int32_t X = 0;
    int32_t Y = 0;
    ModelToScreen(X, Y);
    gc->FillRect(X, Y, w, h, color);
}

void Widget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    (void)x;
    (void)y;
    (void)button;
    if (Focussable) {
        GetFocus(this);
    }
}

void Widget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    (void)x;
    (void)y;
    (void)button;
}

void Widget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    (void)oldx;
    (void)oldy;
    (void)newx;
    (void)newy;
}

int32_t Widget::GetX() {
    return x;
}

int32_t Widget::GetY() {
    return y;
}

int32_t Widget::GetWidth() {
    return w;
}

int32_t Widget::GetHeight() {
    return h;
}

void Widget::SetColor(uint32_t newColor) {
    color = newColor;
}

CompositeWidget::CompositeWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
    : Widget(parent, x, y, w, h, color),
      numChildren(0),
      focusedChild(0)
{
}

CompositeWidget::~CompositeWidget() {
}

void CompositeWidget::GetFocus(Widget* widget) {
    this->focusedChild = widget;
    if (parent != 0) {
        parent->GetFocus(this);
    }
}

bool CompositeWidget::AddChild(Widget* child) {
    if (numChildren >= 64) {
        return false;
    }
    children[numChildren++] = child;
    return true;
}

void CompositeWidget::Draw(GraphicsContext* gc) {
    Widget::Draw(gc);
    for (int32_t i = numChildren - 1; i >= 0; --i) {
        children[i]->Draw(gc);
    }
}

void CompositeWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    for (int32_t i = 0; i < numChildren; ++i) {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    for (int32_t i = 0; i < numChildren; ++i) {
        if (children[i]->ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }
}

void CompositeWidget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    int32_t firstChild = -1;
    for (int32_t i = 0; i < numChildren; ++i) {
        if (children[i]->ContainsCoordinate(oldx - this->x, oldy - this->y)) {
            children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            firstChild = i;
            break;
        }
    }

    for (int32_t i = 0; i < numChildren; ++i) {
        if (children[i]->ContainsCoordinate(newx - this->x, newy - this->y)) {
            if (firstChild != i) {
                children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            }
            break;
        }
    }
}

void CompositeWidget::OnKeyDown(char c) {
    if (focusedChild != 0) {
        focusedChild->OnKeyDown(c);
    }
}

void CompositeWidget::OnKeyUp(char c) {
    if (focusedChild != 0) {
        focusedChild->OnKeyUp(c);
    }
}

ButtonWidget::ButtonWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, const char* label, void (*onClick)())
    : Widget(parent, x, y, w, h, COLOR_WIDGET_BG),
      label(label),
      pressed(false),
      hovered(false),
      onClickHandler(onClick)
{
}

ButtonWidget::~ButtonWidget() {
}

void ButtonWidget::Draw(GraphicsContext* gc) {
    int32_t X = 0;
    int32_t Y = 0;
    ModelToScreen(X, Y);

    uint32_t bgColor = color;
    if (pressed) {
        bgColor = COLOR_WIDGET_FOCUS;
    } else if (hovered) {
        bgColor = COLOR_START_BTN_ACTIVE;
    }

    gc->FillRect(X, Y, w, h, bgColor);
    gc->DrawRect(X, Y, w, h, COLOR_WIDGET_BORDER);

    if (label != 0) {
        int32_t textLen = (int32_t)strlen(label);
        int32_t textX = X + (w - textLen * 8) / 2;
        int32_t textY = Y + (h - 16) / 2;
        uint32_t textColor = pressed ? COLOR_TEXT_LIGHT : COLOR_TEXT_DARK;
        gc->DrawString(textX, textY, label, textColor);
    }
}

void ButtonWidget::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    Widget::OnMouseDown(x, y, button);
    if (button == 1) {
        pressed = true;
    }
}

void ButtonWidget::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    (void)x;
    (void)y;
    if (button == 1 && pressed) {
        pressed = false;
        if (onClickHandler != 0) {
            onClickHandler();
        }
    }
}

void ButtonWidget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    (void)oldx;
    (void)oldy;
    hovered = ContainsCoordinate(newx, newy);
    if (!hovered) {
        pressed = false;
    }
}