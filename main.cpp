#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

std::vector<unsigned char> read_file(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return {};
    }
    return {std::istreambuf_iterator(file), std::istreambuf_iterator<char>()};
}

void highlight_hex(const std::string &text, const std::string &highlight = "\033[1;31m",
                   const std::string &reset = "\033[0m") {
    std::cout << highlight << text << reset;
}

std::string interpret_ascii(const unsigned char c) {
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
            }
            std::ostringstream oss;
            oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
            return oss.str();
    }
}

void hex_dump_with_ascii(const std::vector<unsigned char> &data, const std::vector<std::string> &highlights) {
    for (size_t i = 0; i < data.size(); ++i) {
        std::ostringstream hex_byte;
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
        bool is_highlighted = false;
        for (const auto &highlight: highlights) {
            if (hex_byte.str() == highlight) {
                highlight_hex(hex_byte.str());
                is_highlighted = true;
                break;
            }
        }
        if (!is_highlighted) {
            std::cout << hex_byte.str() << " ";
        }

        if ((i + 1) % 16 == 0 || i == data.size() - i) {
            const size_t padding = 16 - (i % 16) - 1;
            if (i == data.size() - 1 && (i - 1) % 16 != 0) {
                for (size_t j = 0; j < padding; ++j) {
                    std::cout << "   ";
                }
            }

            std::cout << " | ";
            for (size_t j = i - (i % 16); j <= i; ++j) {
                std::cout << interpret_ascii(data[j]) << " ";
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}


void annotate_with_context(const std::vector<unsigned char> &data, const std::string &description, size_t offset,
                           size_t context_size = 16) {
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

bool patch_test_instruction(std::vector<unsigned char> &data, size_t start_offset, const std::vector<unsigned char> &pattern) {
for (size_t i = start_offset; i <= data.size() - pattern.size(); ++i) {
    if (std::equal(pattern.begin(), pattern.end(), data.begin() + i)) {
        std::cout << "Found pattern at offset: 0x" << std::hex << i << ". Patching it..." << std::endl;
        std::fill(data.begin() + i, data.begin() + i +  pattern.size(), 0x90);
        std::cout << "Patched the instruction successfully." << std::endl;
        return true;
    }
}
    return false;
}

void save_file(const std::string &filename, const std::vector<unsigned char> &data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    std::cout << "Patched file saved as: " << filename << std::endl;
}

size_t find_string(const std::vector<unsigned char> &data, const std::string &target) {
    std::vector<unsigned char> target_bytes(target.begin(), target.end());
    for (size_t i = 0; i <= data.size() - target_bytes.size(); ++i) {
        bool found = true;
        for (size_t j = 0; j < target_bytes.size(); ++j) {
            if (data[i + j] != target_bytes[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            std::cout << "Found string \"" << target << "\" at offset: 0x" << std::hex << i << std::endl;
            return i;
        }
    }


    std::cerr << "String not found: " << target << std::endl;
    return std::string::npos;
}

void print_hex(const std::vector<unsigned char> &data, size_t start_offset, size_t length = 64) {
    for (size_t i = start_offset; i < std::min(start_offset + length, data.size()); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
}

void search_hex_pattern(std::vector<unsigned char> &data, std::vector<unsigned char> pattern) {
    for (size_t i = 0; i <= data.size() - pattern.size(); ++i) {
        if (std::equal(pattern.begin(), pattern.end(), data.begin() + i)) {
            std::cout << "Found pattern at offset: 0x" << std::hex << i << std::endl;
        }
    }
}
std::vector<unsigned char> parse_hex_input(const std::string &hex_input) {
    std::vector<unsigned char> bytes;
    std::istringstream stream(hex_input);
    std::string byte_str;
    while (stream >> byte_str) {
        bytes.push_back(static_cast<unsigned char>(std::stoul(byte_str, nullptr, 16)));
    }
    return bytes;
}
int main() {
    std::cout << "File to Analyze in cpp Folder named main.exe" << std::endl;
    const std::string filename = "../main.exe";
    std::vector<unsigned char> content = read_file(filename);
    int choice = 0;

    if (content.empty()) {
        std::cerr << "File not found or could not be read." << std::endl;
        return 1;
    }
    std::cout << "Choose an Option" << std::endl << "1 -> CLI Hex-Dump" << std::endl << "2 -> Find String in Hex-Dump (returns offset)"
    << std::endl << "3 -> Patches given instruction with NOP" << std::endl;
    std::cin >> choice;
    switch (choice) {
        case 1: {
            const std::vector<std::string> highlights = {"85", "C0", "90"};
            hex_dump_with_ascii(content, highlights);
            break;
        }
        case 2: {
            std::string input;
            std::cout << "Put in String to search for:" << std::endl;
            std::cin >> input;
            std::cout << std::endl;
            const std::string target_string = input;
            if (size_t string_offset = find_string(content, target_string); string_offset != std::string::npos) {
               break;
            } else {
                std::cerr << "String not found in binary content." << std::endl;
                break;
            }
            break;
        }
        case 3: {
            std::string hex_pattern;
            std::cout << "Put in The hex values that need to be patched (0x0000): " << std::endl;
            std::cin.ignore();
            std::getline(std::cin, hex_pattern);
            std::vector<unsigned char> pattern = parse_hex_input(hex_pattern);
            if (pattern.empty()) {
                std::cerr << "Invalid pattern input." << std::endl;
                break;
            }
            if (patch_test_instruction(content, 0, pattern)) {
                save_file("patched_binary.exe", content);
                const std::string newfilename = "patched_binary.exe";
                std::vector<unsigned char> newcontent = read_file(newfilename);
                std::cout << "Searching for pattern in patched File..." << std::endl;
                search_hex_pattern(newcontent, pattern);
            }
        }
        default: {
            break;
        }

    }
    std::cin >> choice;
    return main();
}
