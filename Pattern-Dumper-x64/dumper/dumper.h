#ifndef DUMPER_H
#define DUMPER_H
#include <vector>
#include "serialization/serialization.h"
#ifndef JSON_FILE_NAME
#define JSON_FILE_NAME "input.json"
#endif // !JSON_FILE_NAME
#ifndef OUTPUT_FILE_NAME
#define OUTPUT_FILE_NAME "output.txt"
#endif // !OUTPUT_FILE_NAME

enum class e_serialization_status : int
{
    success,
    exception,
    file_error,
    parsing_error
};

class dumper
{
    static e_serialization_status serialize(std::vector<serialized_pattern_data>& out);
public:
    static void dump();
};
#endif // !DUMPER_H
