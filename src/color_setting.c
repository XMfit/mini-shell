#include "../include/shell.h"

void setColor(const char* color) {
    printf("%s", color);
}

void resetColor() {
    printf("\033[0m");
}
