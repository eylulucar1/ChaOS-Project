#include <common/types.h>
#include <hardware/gdt.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <hardware/timer.h>
#include <memory/heap.h>
#include <drivers/driver.h>
#include <drivers/vesa.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <gui/theme.h>
#include <gui/desktop.h>
#include <gui/taskbar.h>
#include <apps/terminal.h>
#include <apps/notepad.h>
#include <apps/paint.h>
#include <apps/sysmon.h>

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" uint32_t kernel_end;

extern "C" void callConstructors() {
    for (constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

extern "C" void kernelMain(multiboot_info_t* mbd, uint32_t magic) {
    (void)magic;

    GlobalDescriptorTable gdt;

    size_t heapStart = (size_t)&kernel_end;
    if (heapStart < 0x400000) {
        heapStart = 0x400000;
    }
    MemoryManager memoryManager(heapStart, 16 * 1024 * 1024);

    InterruptManager interrupts(0x20, &gdt);
    ProgrammableIntervalTimer timer(&interrupts, 100);

    GraphicsContext gc(mbd);
    Desktop desktop(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DESKTOP_BG, &gc);

    Taskbar taskbar(&desktop, &timer);
    desktop.SetTaskbar(&taskbar);

    TerminalWindow* terminalApp = new TerminalWindow(&desktop, 40, 40, 450, 290);
    SystemMonitorWindow* sysmonApp = new SystemMonitorWindow(&desktop, 520, 40, 420, 230, &timer, &desktop);
    NotepadWindow* notepadApp = new NotepadWindow(&desktop, 80, 360, 410, 260);
    PaintWindow* paintApp = new PaintWindow(&desktop, 530, 300, 370, 310);

    desktop.AddChild(terminalApp);
    desktop.AddChild(sysmonApp);
    desktop.AddChild(notepadApp);
    desktop.AddChild(paintApp);

    taskbar.RegisterAppWindow(0, terminalApp, "Terminal");
    taskbar.RegisterAppWindow(1, notepadApp, "Notepad");
    taskbar.RegisterAppWindow(2, paintApp, "Paint");
    taskbar.RegisterAppWindow(3, sysmonApp, "System Monitor");

    desktop.GetFocus(terminalApp);

    DriverManager drvManager;
    KeyboardDriver keyboard(&interrupts, &desktop);
    MouseDriver mouse(&interrupts, &desktop);

    drvManager.AddDriver(&keyboard);
    drvManager.AddDriver(&mouse);

    PeripheralComponentInterconnectController pciController;
    pciController.SelectDrivers(&drvManager, &interrupts);

    drvManager.ActivateAll();
    interrupts.Activate();

    while (true) {
        desktop.Draw(&gc);
        gc.SwapBuffers();
        __asm__ volatile("hlt");
    }
}