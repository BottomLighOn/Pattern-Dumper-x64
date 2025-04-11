#ifndef SERIALIZATION_H
#define SERIALIZATION_H
#include <string>
struct serialized_pattern_data
{
    const std::string pattern_name_optional;
    const std::string module_name;
    const std::string pattern;
    int64_t relative_instruction_size;
    int64_t full_instruction_size;
    int64_t offset_from_pattern;
    bool result_rva;

    serialized_pattern_data(const std::string& pattern_name_optional, const std::string& module_name,
                            const std::string& pattern, int64_t relative_instruction_size,
                            int64_t full_instruction_size, int64_t offset_from_pattern, bool result_rva) :
        pattern_name_optional(pattern_name_optional),
        module_name(module_name),
        pattern(pattern),
        relative_instruction_size(relative_instruction_size),
        full_instruction_size(full_instruction_size),
        offset_from_pattern(offset_from_pattern),
        result_rva(result_rva)
    {};
};
#endif // !SERIALIZATION_H
