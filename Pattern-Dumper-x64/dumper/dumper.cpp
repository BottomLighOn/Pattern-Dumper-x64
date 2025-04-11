#include "dumper.h"
#include <filesystem>
#include <fstream>
#include "../loguru/loguru.hpp"
#include "../simdjson/simdjson.h"
#include "../pattern_scan/pattern_scan.h"
#ifdef ERROR
#undef ERROR
#endif // ERROR

e_serialization_status dumper::serialize(std::vector<serialized_pattern_data>& out) {
    try {
        simdjson::dom::parser parser;

        auto path = std::filesystem::current_path();
        auto path_str = path.string();
        std::string full_path_str = path_str + "\\" + JSON_FILE_NAME;
        LOG_F(INFO, "[+] Current path: %s", path_str.c_str());
        LOG_F(INFO, "[+] Path to input data: %s", full_path_str.c_str());
        std::ifstream file(full_path_str);
        if (!file.is_open()) {
            LOG_F(ERROR, "[-] Failed to open JSON file: %s", JSON_FILE_NAME);
            return e_serialization_status::file_error;
        }

        std::string json_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if (json_data.empty()) {
            LOG_F(ERROR, "[-] JSON File \" %s\" is Empty.", JSON_FILE_NAME);
            return e_serialization_status::file_error;
        }

        auto content = parser.parse(json_data);
        for (auto pattern_data : content["patterns"]) {
            std::string pattern_name_optional = std::string();
            std::string module_name = std::string();
            std::string pattern = std::string();
            int64_t relative_instruction_size = 0;
            int64_t full_instruction_size = 0;
            int64_t offset_from_pattern = 0;
            bool result_rva = 0;
            
            bool is_pattern_name_null = pattern_data["pattern_name_optional"].is_null();
            bool is_module_name_null = pattern_data["module_name"].is_null();
            bool is_pattern_null = pattern_data["pattern"].is_null();
            bool is_relative_instruction_size_null = pattern_data["relative_instruction_size"].is_null();
            bool is_full_instruction_size_null = pattern_data["full_instruction_size"].is_null();
            bool is_offset_from_pattern_null = pattern_data["offset_from_pattern"].is_null();
            bool is_result_rva_null = pattern_data["result_rva"].is_null();

            if (is_pattern_null || is_relative_instruction_size_null ||
                is_full_instruction_size_null || is_offset_from_pattern_null || is_result_rva_null) {
                LOG_F(ERROR, "[-] Parsing Error: Null Value in Non-Optional Field");
                return e_serialization_status::parsing_error;
            }

            if (!is_pattern_name_null) {
                pattern_name_optional = pattern_data["pattern_name_optional"].get_string().value();
            }

            if (!is_module_name_null) {
                module_name = pattern_data["module_name"].get_string().value();
            }

            pattern = pattern_data["pattern"].get_string().value();
            relative_instruction_size = pattern_data["relative_instruction_size"].get_int64().value();
            full_instruction_size = pattern_data["full_instruction_size"].get_int64().value();
            offset_from_pattern = pattern_data["offset_from_pattern"].get_int64().value();
            result_rva = pattern_data["result_rva"].get_bool().value();

            serialized_pattern_data serialized_data = serialized_pattern_data(
                pattern_name_optional,
                module_name,
                pattern,
                relative_instruction_size,
                full_instruction_size,
                offset_from_pattern,
                result_rva
            );

            out.push_back(serialized_data);
            LOG_F(INFO, "[+] Successfully parsed \"%s\" Pattern", pattern_name_optional.empty() ? "[unnamed]" : pattern_name_optional.c_str());
        }

        return e_serialization_status::success;
    }
    catch (const std::filesystem::filesystem_error& e) {
        LOG_F(ERROR, "[-] Failed to serialize: %s", e.what());
        return e_serialization_status::exception;
    }
}

void dumper::dump() {
    std::vector<serialized_pattern_data> serialized_data {};
    e_serialization_status status = serialize(serialized_data);
    if (status != e_serialization_status::success) {
        LOG_F(ERROR, "[-] Serialization failed. Shuting down.");
        return;
    }

    std::ofstream file(OUTPUT_FILE_NAME, std::ios::trunc);
    if (!file) {
        LOG_F(ERROR, "[-] Failed to create %s", OUTPUT_FILE_NAME);
        return;
    }

    for (auto& pattern_data : serialized_data) {

        const std::string pattern_name = pattern_data.pattern_name_optional;
        const std::string module_name = pattern_data.module_name;
        const std::string pattern = pattern_data.pattern;
        const int64_t relative_instruction_size = pattern_data.relative_instruction_size;
        const int64_t full_instruction_size = pattern_data.full_instruction_size;
        const int64_t offset_from_pattern = pattern_data.offset_from_pattern;
        const bool is_rva = pattern_data.result_rva;

        uintptr_t module_base = reinterpret_cast<uintptr_t>(GetModuleHandleA(module_name.empty() ? 0 : module_name.c_str()));
        if (!module_base) {
            LOG_F(ERROR, "[-] Failed to scan pattern \"%s\" Base module not found", pattern_name.empty() ? "[unnamed]" : pattern_name.c_str());
            continue;
        }

        uintptr_t result = pattern_scan::scan(pattern.c_str(), module_name.empty() ? 0 : module_name.c_str());
        if (!result) {
            LOG_F(ERROR, "[-] Not found value for Pattern \"%s\" \"%s\"", pattern_name.empty() ? "[unnamed]" : pattern_name.c_str(), pattern.c_str());
            continue;
        }

        result += offset_from_pattern;

        if (relative_instruction_size > 0 && full_instruction_size > 0) {
            result = pattern_scan::get_absolute_address<uintptr_t>(result, relative_instruction_size, full_instruction_size);
        }

        if (result == 0) {
            LOG_F(ERROR, "[-] Failed to calculate absolute address for pattern \"%s\"", pattern_name.empty() ? "[unnamed]" : pattern_name.c_str());
            continue;
        }

        if (is_rva) {
            result = result - module_base;
        }

        if (result <= 0) {
            LOG_F(ERROR, "[-] Failed to calculate RVA for pattern \"%s\"", pattern_name.empty() ? "[unnamed]" : pattern_name.c_str());
            continue;
        }

        LOG_F(INFO, "[+] Pattern \"%s\" found at: 0x%llX", pattern_name.empty() ? "[unnamed]" : pattern_name.c_str(), result);
        
        file << "#define DMP_" << pattern_name << " 0x" << std::uppercase << std::hex << result << "/*" << pattern.c_str() << "*/" << std::endl;
    }

    file.close();
    LOG_F(INFO, "[+] Dumping completed. Output saved to %s", OUTPUT_FILE_NAME);
}

#ifndef ERROR
#define ERROR 0
#endif // ERROR