#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>

std::vector<unsigned char> read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }

    return {(std::istreambuf_iterator(file)), std::istreambuf_iterator<char>()};
}

void highlight_hex(const std::string& text, const std::string& highlight = "\033[1;31m", const std::string& reset = "\033[0m") {
    std::cout << highlight << text << reset;
}


std::string interpret_ascii(unsigned char c) {
    if (isprint(c)) {
        return std::string(1, c);
    }
    switch (c) {
        case 0: return "NUL";
        case 1: return "SOH";
        case 2: return "STX";
        case 3: return "ETX";
        case 4: return "EOT";
        case 5: return "ENQ";
        case 6: return "ACK";
        case 7: return "BEL";
        case 8: return "BS";
        case 9: return "TAB";
        case 10: return "LF";
        case 11: return "VT";
        case 12: return "FF";
        case 13: return "CR";
        case 14: return "SO";
        case 15: return "SI";
        case 16: return "DLE";
        case 17: return "DC1";
        case 18: return "DC2";
        case 19: return "DC3";
        case 20: return "DC4";
        case 21: return "NAK";
        case 22: return "SYN";
        case 23: return "ETB";
        case 24: return "CAN";
        case 25: return "EM";
        case 26: return "SUB";
        case 27: return "ESC";
        case 28: return "FS";
        case 29: return "GS";
        case 30: return "RS";
        case 31: return "US";
        case 127: return "DEL";
        default:
            if (isprint(c)) {
                return std::string(1, c);
            } else {
                std::ostringstream oss;
                oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
                return oss.str();
            }
    }
}

void hex_dump_with_ascii(const std::vector<unsigned char>& data, const std::vector<std::string>& highlights) {
    std::cout << "Hex Dump with ASCII Interpretation and Highlighting:\n";

    for (size_t i = 0;i < data.size();++i) {
        std::ostringstream hex_byte;
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        bool is_highlighted = false;
        for (const auto& highlight : highlights) {
            if (hex_byte.str()==highlight) {
                highlight_hex(hex_byte.str());
                is_highlighted = true;
                break;
            }
        }
        if (!is_highlighted) {
            std::cout << hex_byte.str() << " ";
        }

        if ((i+1) % 16 == 0 || i == data.size() - i) {
            size_t padding = 16 - (i%16) - 1;
            if (i==data.size() - 1 && (i-1)% 16 != 0) {
                for (size_t j = 0; j<padding; ++j) {
                    std::cout << "   ";
                }
            }

            std::cout << " | ";
            for (size_t j = i - (i%16);j<=i;++j) {
                std::cout << interpret_ascii(data[j]) << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
size_t find_string(const std::vector<unsigned char>& data, const std::string& target) {
    for (size_t i = 0; i <= data.size() - target.size(); i++) {
    if (std::equal(target.begin(), target.end(), data.begin() + i)) {
        std::cout << "Found string \"" << target << "\" at offset: 0x" << std::hex << i << std::endl;
        return i;
    }

    }
    return std::string::npos;
}
void annotate_with_context(const std::vector<unsigned char>& data, const std::string& description, size_t offset, size_t context_size = 16) {
    if (offset >= data.size()) {
        std::cerr << "Invalid offset: 0x" << std::hex << offset << std::endl;
        return;
    }

    std::cout << "Annotation: " << description << " found at offset 0x" << std::hex << offset << std::endl;

    size_t start_offset = (offset > context_size) ? offset - context_size : 0;
    size_t end_offset = std::min(offset + context_size, data.size());

    for (size_t i = start_offset; i < end_offset; ++i) {
        if (i == offset) {
            highlight_hex("[", "\033[1;33m");
            highlight_hex(std::to_string(static_cast<int>(data[i])), "\033[1;33m");
            highlight_hex("]", "\033[1;33m");
        } else {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
        }
        if ((i + 1) % 16 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}

void interactive_hex_dump(const std::vector<unsigned char>& data) {
    const size_t PAGE_SIZE = 256;
    size_t current_offset = 0;

    while (current_offset <data.size()) {
        size_t end_offset = std::min(current_offset + PAGE_SIZE, data.size());
        for (size_t i = current_offset; i<end_offset;++i) {
            if ((i+1)%16==0){
                std::cout << std::endl;
            }
        }

        current_offset = end_offset;
        if (current_offset < data.size()) {
            std::cout << "Press Enter to continue..." << std::flush;
            std::cin.ignore();
        }
    }
}
bool patch_test_instruction(std::vector<unsigned char>& data, size_t start_offset)
{
    for (size_t i = start_offset; i < data.size() - 1; ++i) {
        if (data[i] == 0x85 && data[i+1]== 0xC0) {
            std::cout << "Found TEST instruction at offset: 0x" << std::hex << i << std::endl;
            data[i] = 0x90;
            data[i+1]=0x90;
            std::cout << "Patched TEST instructions with NOPs." << std::endl;
            return true;
        }
    }
    std::cerr << "No TEST instruction found at offset 0x" << std::hex << start_offset << std::endl;
    return false;
}
void save_file(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    std::cout << "Patched file saved as: " << filename << std::endl;
}
int main()
{
    std::string filename = "../main.exe";
    std::vector<unsigned char> content = read_file(filename);

    if (content.empty()) {
        std::cerr << "File not found or could not be read." << std::endl;
        return 1;
    }

    std::vector<std::string> highlights = {"85", "C0", "90"};
    hex_dump_with_ascii(content, highlights);

    std::string target_string = "Enter the key: ";
    size_t string_offset = find_string(content, target_string);
    if (string_offset != std::string::npos) {
        annotate_with_context(content, "String", string_offset);
    } else {
        std::cerr << "String not found in binary content." << std::endl;
    }

  //  std::cout << "Interactive Hex Dump:\n";
  //  interactive_hex_dump(content);

    return 0;

/*    for (size_t i = 0;i<content.size();++i) {
        std::cout << "Offset:" << std::hex << i
                            << " Byte:" << std::hex << static_cast<int>(content[i])
                            << " ASCII:{" << interpret_ascii(content[i]) << "}" << std::endl;
    }

    size_t string_offset = find_string(content, target_string);
    if (string_offset == std::string::npos) {
        std::cerr << "Target not found!" << std::endl;
        return 1;
    }
    if (!patch_test_instruction(content, string_offset)) {
        std::cerr << "Failed to patch TEST instruction." << std::endl;
        return 1;
    }
    if (!patch_test_instruction(content, string_offset)) {
        std::cerr << "Failed to patch TEST instruction." << std::endl;
        return 1;
    }
    save_file("patched_" + filename, content);
*/
}
