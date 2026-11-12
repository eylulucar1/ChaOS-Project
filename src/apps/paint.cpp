#include <apps/paint.h>
#include <gui/theme.h>

PaintWindow::PaintWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h)
    : Window(parent, x, y, w, h, "Paint", COLOR_WINDOW_BG),
      drawing(false)
{
    palette[0] = 0x2E3440;
    palette[1] = 0xBF616A;
    palette[2] = 0xA3BE8C;
    palette[3] = 0xEBCB8B;
    palette[4] = 0x5E81AC;
    palette[5] = 0xB48EAD;
    palette[6] = 0xFFFFFF;

    currentColor = palette[1];
    ClearCanvas();
}

PaintWindow::~PaintWindow() {
}

void PaintWindow::ClearCanvas() {
    for (int32_t r = 0; r < PAINT_ROWS; r++) {
        for (int32_t c = 0; c < PAINT_COLS; c++) {
            canvas[r][c] = 0xFFFFFF;
        }
    }
}

void PaintWindow::PlotCell(int32_t mouseX, int32_t mouseY) {
    int32_t canvasX = x + WINDOW_BORDER_SIZE + 8;
    int32_t canvasY = y + WINDOW_BORDER_SIZE + WINDOW_TITLEBAR_HEIGHT + 36;

    int32_t relX = mouseX - canvasX;
    int32_t relY = mouseY - canvasY;

    if (relX < 0 || relY < 0) {
        return;
    }

    int32_t col = relX / PAINT_CELL_SIZE;
    int32_t row = relY / PAINT_CELL_SIZE;

    if (col >= 0 && col < PAINT_COLS && row >= 0 && row < PAINT_ROWS) {
        canvas[row][col] = currentColor;
        if (col + 1 < PAINT_COLS) {
            canvas[row][col + 1] = currentColor;
        }
        if (row + 1 < PAINT_ROWS) {
            canvas[row + 1][col] = currentColor;
        }
    }
}

void PaintWindow::DrawClientArea(GraphicsContext* gc) {
    int32_t clientX = x + WINDOW_BORDER_SIZE;
    int32_t clientY = y + WINDOW_BORDER_SIZE + WINDOW_TITLEBAR_HEIGHT;
    int32_t clientW = w - (2 * WINDOW_BORDER_SIZE);

    gc->FillRect(clientX, clientY, clientW, 30, COLOR_WIDGET_BG);
    gc->FillRect(clientX, clientY + 30, clientW, 1, COLOR_WIDGET_BORDER);

    for (int32_t i = 0; i < PAINT_PALETTE_COUNT; i++) {
        int32_t swatchX = clientX + 8 + (i * 28);
        int32_t swatchY = clientY + 5;
        gc->FillRect(swatchX, swatchY, 20, 20, palette[i]);
        if (palette[i] == currentColor) {
            gc->DrawRect(swatchX - 2, swatchY - 2, 24, 24, COLOR_TEXT_DARK);
        } else {
            gc->DrawRect(swatchX, swatchY, 20, 20, COLOR_WIDGET_BORDER);
        }
    }

    int32_t clearBtnX = clientX + clientW - 76;
    int32_t clearBtnY = clientY + 4;
    gc->FillRect(clearBtnX, clearBtnY, 68, 22, COLOR_BTN_CLOSE);
    gc->DrawRect(clearBtnX, clearBtnY, 68, 22, COLOR_WINDOW_BORDER);
    gc->DrawString(clearBtnX + 14, clearBtnY + 3, "Clear", COLOR_TEXT_LIGHT);

    int32_t canvasX = clientX + 8;
    int32_t canvasY = clientY + 36;
    int32_t canvasW = PAINT_COLS * PAINT_CELL_SIZE;
    int32_t canvasH = PAINT_ROWS * PAINT_CELL_SIZE;

    gc->DrawRect(canvasX - 1, canvasY - 1, canvasW + 2, canvasH + 2, COLOR_WIDGET_BORDER);

    for (int32_t r = 0; r < PAINT_ROWS; r++) {
        for (int32_t c = 0; c < PAINT_COLS; c++) {
            gc->FillRect(
                canvasX + (c * PAINT_CELL_SIZE),
                canvasY + (r * PAINT_CELL_SIZE),
                PAINT_CELL_SIZE,
                PAINT_CELL_SIZE,
                canvas[r][c]
            );
        }
    }
}

void PaintWindow::OnMouseDown(int32_t x, int32_t y, uint8_t button) {
    Window::OnMouseDown(x, y, button);
    if (!visible || minimized || Dragging || button != 1) {
        return;
    }

    int32_t clientX = this->x + WINDOW_BORDER_SIZE;
    int32_t clientY = this->y + WINDOW_BORDER_SIZE + WINDOW_TITLEBAR_HEIGHT;
    int32_t clientW = this->w - (2 * WINDOW_BORDER_SIZE);

    for (int32_t i = 0; i < PAINT_PALETTE_COUNT; i++) {
        int32_t swatchX = clientX + 8 + (i * 28);
        int32_t swatchY = clientY + 5;
        if (x >= swatchX && x < swatchX + 20 && y >= swatchY && y < swatchY + 20) {
            currentColor = palette[i];
            return;
        }
    }

    int32_t clearBtnX = clientX + clientW - 76;
    int32_t clearBtnY = clientY + 4;
    if (x >= clearBtnX && x < clearBtnX + 68 && y >= clearBtnY && y < clearBtnY + 22) {
        ClearCanvas();
        return;
    }

    drawing = true;
    PlotCell(x, y);
}

void PaintWindow::OnMouseUp(int32_t x, int32_t y, uint8_t button) {
    drawing = false;
    Window::OnMouseUp(x, y, button);
}

void PaintWindow::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    Window::OnMouseMove(oldx, oldy, newx, newy);
    if (drawing && !Dragging) {
        PlotCell(newx, newy);
    }
}