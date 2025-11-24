#include "utils/console.h"

void banner(const char *msg, char borderChar) {
    size_t len = std::strlen(msg);
    for (size_t i = 0; i < len + 4; ++i) {
        std::cout << borderChar;
    }
    std::cout << std::endl;
    std::cout << borderChar << " " << msg << " " << borderChar << std::endl;
    for (size_t i = 0; i < len + 4; ++i) {
        std::cout << borderChar;
    }
    std::cout << std::endl;
}