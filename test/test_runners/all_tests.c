#include "unity_fixture.h"

static void RunAllTests(void) {
    RUN_TEST_GROUP(Bus);
    RUN_TEST_GROUP(SM83);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, RunAllTests);
}