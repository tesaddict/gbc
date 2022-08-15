#include <stdio.h>
#include "sm83.h"

int main() {
    sm83_t init_state = { 0, 0, 0, 0, 0, 0, 0, 0 };
    sm83_init(init_state);
    return 0;
}
