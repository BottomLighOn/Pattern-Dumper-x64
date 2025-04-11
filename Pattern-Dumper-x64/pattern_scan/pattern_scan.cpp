#include "pattern_scan.h"
#define InRange(x, a, b) (x >= a && x <= b) 
#define GetBits(x) (InRange(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define GetByte(x) ((BYTE)(GetBits(x[0]) << 4 | GetBits(x[1])))


uintptr_t pattern_scan::get_absolute_address_internal(uintptr_t address, int64_t offset, int64_t size) {
    uintptr_t addrs = address + *(int32_t*)(address + offset) + size;
    return addrs;
}

uintptr_t pattern_scan::scan(const char* pattern, const char* module) {

    PBYTE ModuleStart = (PBYTE)GetModuleHandleA(module);
    if (!ModuleStart)
        return 0;

    PIMAGE_NT_HEADERS NtHeader = ((PIMAGE_NT_HEADERS)(ModuleStart + ((PIMAGE_DOS_HEADER)ModuleStart)->e_lfanew));
    PBYTE ModuleEnd = (PBYTE)(ModuleStart + NtHeader->OptionalHeader.SizeOfImage - 0x1000); ModuleStart += 0x1000;

    PBYTE FirstMatch = nullptr;
    const char* CurPatt = pattern;
    for (; ModuleStart < ModuleEnd; ++ModuleStart) {
        bool SkipByte = (*CurPatt == '\?');
        if (SkipByte || *ModuleStart == GetByte(CurPatt)) {
            if (!FirstMatch) FirstMatch = ModuleStart;
            SkipByte ? CurPatt += 2 : CurPatt += 3;
            if (CurPatt[-1] == 0) return (uintptr_t)FirstMatch;
        }

        else if (FirstMatch) {
            ModuleStart = FirstMatch;
            FirstMatch = nullptr;
            CurPatt = pattern;
        }
    }

    return 0;
}
