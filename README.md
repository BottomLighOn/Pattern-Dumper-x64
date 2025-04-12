# Pattern-Dumper-x64

**Pattern-Dumper-x64** is a C++ tool that logs memory addresses in a target process based on user-defined signatures. Signatures are configured via a JSON file, allowing flexibility without recompilation.

## 🔧 Features

- Pattern scanning with wildcard support.
- Flexible signature definitions via `input.json`.
- Output saved to `output.txt`.
- Easy to use via DLL injection.
- MIT licensed.

## 🚀 Usage

1. Compile the project using Visual Studio 2022, or download a prebuilt DLL from the [Releases](https://github.com/BottomLighOn/Pattern-Dumper-x64/releases) page.
2. Launch the **target application**.
3. Place your `input.json` file in the same directory as the target executable.
4. Inject the DLL into the target process using any injector.
5. A console window will appear showing logs. The output will be saved to `output.txt` in the target application's folder.

## 📘 JSON Format

The `input.json` file must follow this structure:

```json
{
  "patterns": [
    {
      "pattern_name_optional": "FNC_GET_SOCKET_LOCATION",
      "module_name": null,
      "pattern": "40 53 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? ...",
      "relative_instruction_size": 0,
      "full_instruction_size": 0,
      "offset_from_pattern": 0,
      "result_rva": true
    },
    {
      "pattern_name_optional": "OFFSET_UWORLD",
      "module_name": "Marvel-Win64-Shipping.exe",
      "pattern": "48 8B 05 ? ? ? ? 48 8B 80 ? ? ? ? 48 83 C4 ? 5B C3 CC CC CC CC CC 48 89 5C 24",
      "relative_instruction_size": 3,
      "full_instruction_size": 7,
      "offset_from_pattern": 0,
      "result_rva": true
    }
  ]
}
```
### Field Descriptions

- **`pattern_name_optional`** (`string` or `null`): Optional name of the pattern, useful for logging.
- **`module_name`** (`string` or `null`): Target module for pattern scanning. If `null`, the main module is used.
- **`pattern`** (`string`): Byte pattern to search for. Wildcards are represented as `?`.
- **`relative_instruction_size`** (`int`): Offset from the start of the instruction to the relative value.
- **`full_instruction_size`** (`int`): Full size of the instruction for absolute address calculation.
- **`offset_from_pattern`** (`int`): Additional offset from the pattern base to the actual value.
- **`result_rva`** (`bool`): If `true`, result is saved as an RVA. If `false`, it’s an absolute address.

> If both `relative_instruction_size` and `full_instruction_size` are `0`, the value found at the pattern is treated as an absolute address directly.

⚠️ This is a minimal utility made for personal use and educational purposes.