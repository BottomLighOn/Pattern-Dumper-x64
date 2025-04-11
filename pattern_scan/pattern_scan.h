#ifndef PATTERN_SCAN_H
#define PATTERN_SCAN_H
#include <Windows.h>
#include <cstdint>
class pattern_scan
{
    static uintptr_t get_absolute_address_internal(uintptr_t address, int64_t offset, int64_t size);
public:
    template< typename Type = uintptr_t >
    static Type get_absolute_address(uintptr_t address, int64_t offset, int64_t size) {
        return Type(get_absolute_address_internal(address, offset, size));
    }
    static uintptr_t scan(const char* pattern, const char* module);
};
#endif // !PATTERN_SCAN_H
