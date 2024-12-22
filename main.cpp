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
//TODO: comparing hex values to hex instructions while looping
void hex_dump(const std::vector<unsigned char>& data) {
    std::cout << "Hex Dump of File Content:\n";
    for (size_t i = 0;i < data.size();++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
        if ((i+1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
}
std::string interpret_ascii(unsigned char c)
{
        switch (c) {
            case 0: break;
            case 1: std::cout << "SOH";
            break;
            case 2: std::cout << "STX";
            break;
            case 3: std::cout << "ETX";
            break;
            case 4: std::cout << "EOT";
            break;
            case 5: std::cout << "ENQ";
            break;
            case 6: std::cout << "ACK";
            break;
            case 7: std::cout << "BEL";
            break;
            case 8: std::cout << "BS";
            break;
            case 9: std::cout << "TAB";
            break;
            case 10: std::cout << "LF";
            break;
            case 11: std::cout << "VT";
            break;
            case 12: std::cout << "FF";
            break;
            case 13: std::cout << "CR";
            break;
            case 14: std::cout << "SO";
            break;
            case 15: std::cout << "SI";
            break;
            case 16: std::cout << "DLE";
            break;
            case 17: std::cout << "DC1";
            break;
            case 18: std::cout << "DC2";
            break;
            case 19: std::cout << "DC3";
            break;
            case 20: std::cout << "DC4";
            break;
            case 21: std::cout << "NAK";
            break;
            case 22: std::cout << "SYN";
            break;
            case 23: std::cout << "ETB";
            break;
            case 24: std::cout << "CAN";
            break;
            case 25: std::cout << "EM";
            break;
            case 26: std::cout << "SUB";
            break;
            case 27: std::cout << "ESC";
            break;
            case 28: std::cout << "FS";
            break;
            case 29: std::cout << "GS";
            break;
            case 30: std::cout << "RS";
            break;
            case 31: std::cout << "US";
            break;
            default:
                if (isprint(c)) {
                    return std::string (1,c);
                }
                else {
                    std::ostringstream oss;
                    oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
                    return oss.str();

                }
        }

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
    std::string target_string = "Enter the key: ";

    std::vector<unsigned char> content = read_file(filename);
    if (content.empty()) {
        return 1;
    }

    hex_dump(content);

    for (size_t i = 0;i<content.size();++i) {
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

    return 0;
}
