#include <fstream>
#include <iostream>
#include <string>

std::string read_file(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());
    return content;
}

int main()
{
    for (std::string content = read_file("../main.exe"); const char c : content)
    {
        std::cout << "{"; // Prefix
        switch (static_cast<unsigned char>(c))
        {
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
            std::cout << static_cast<int>(c) << std::hex;
            break;
        }
        std::cout << "} "; // Suffix
    }
    std::cout << std::endl;
    return 0;
}
