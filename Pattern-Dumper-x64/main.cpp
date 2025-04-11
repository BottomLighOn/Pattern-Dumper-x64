#include <iostream>
#include <Windows.h>
#include "loguru/loguru.hpp"
#include "dumper/dumper.h"

void init() {
    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    SetConsoleTitleA("Pattern Dumper");

    loguru::g_colorlogtostderr = true;
    LOG_F(INFO, "Injected, starting...");
    dumper::dump();
}

bool DllMain(HINSTANCE hinst, int reason, void* reserved) {
    if (reason == 1) {
        init();
    }
    return true;
}