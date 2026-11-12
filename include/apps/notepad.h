#ifndef APPS_NOTEPAD_H
#define APPS_NOTEPAD_H

#include <common/types.h>
#include <gui/window.h>

#define NOTEPAD_BUFFER_SIZE 1024

class NotepadWindow : public Window {
private:
    char content[NOTEPAD_BUFFER_SIZE];
    int32_t length;

public:
    NotepadWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h);
    virtual ~NotepadWindow();

    void ClearContent();
    void SetText(const char* text);

    virtual void DrawClientArea(GraphicsContext* gc);
    virtual void OnKeyDown(char c);
};

#endif