#ifndef APPS_TERMINAL_H
#define APPS_TERMINAL_H

#include <common/types.h>
#include <gui/window.h>

#define TERMINAL_MAX_LINES 14
#define TERMINAL_LINE_LEN  52
#define TERMINAL_INPUT_LEN 44

class TerminalWindow : public Window {
private:
    char lines[TERMINAL_MAX_LINES][TERMINAL_LINE_LEN];
    int32_t lineCount;
    char inputBuffer[TERMINAL_INPUT_LEN];
    int32_t inputLen;
    uint32_t textColor;

public:
    TerminalWindow(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h);
    virtual ~TerminalWindow();

    void PrintLine(const char* text);
    void ClearTerminal();
    void ExecuteCommand(const char* cmd);

    virtual void DrawClientArea(GraphicsContext* gc);
    virtual void OnKeyDown(char c);
};

#endif