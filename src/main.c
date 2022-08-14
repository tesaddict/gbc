#include <stdio.h>
#include "sharp8080.h"

int main() {
    sharp8080_t init_state = { 0, 0, 0, 0, 0, 0, 0, 0 };
    sharp8080_init(init_state);
    return 0;
}
