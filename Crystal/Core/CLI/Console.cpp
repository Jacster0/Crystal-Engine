#include "Console.h"
#include "Platform/Windows/CrystalWindow.h"
#include <iostream>

void Console::Create() noexcept {
    if(!AllocConsole()) {
        return;
    }
    const auto  hCmd = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT cmdSize {
        .Left = 0,
        .Top = 0,
        .Right = 96,
        .Bottom = 19
    };

    CONSOLE_FONT_INFOEX cfi {
        .cbSize = sizeof(cfi),
        .nFont  = 0,
        .dwFontSize {
            .X = 0,
            .Y = 16
        },
        .FontFamily = FF_DONTCARE,
        .FontWeight = FW_NORMAL,
        .FaceName   = L"Consolas"
    };

    SetCurrentConsoleFontEx(hCmd, false, &cfi);
    SetConsoleWindowInfo(hCmd, true, &cmdSize);
    SetConsoleTitleA("Crystal Console");
    Show();
}

void Console::Show() noexcept {
    ShowWindow(GetConsoleWindow(), SW_SHOW);
}

void Console::Hide() noexcept {
    ShowWindow(GetConsoleWindow(), SW_HIDE);  
}

bool Console::Visible() noexcept {
    return IsWindowVisible(GetConsoleWindow()) == true;
}

void Console::Write(std::string_view msg) {
    std::cout << msg;
}

void Console::WriteLine(std::string_view msg) noexcept {
    Write(msg);
    Write("\n");
}

void Console::SetTextColor(ConsoleColor clr) noexcept {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<DWORD>(clr));
}

void Console::Destroy() noexcept {
    Hide();
    FreeConsole();
}
