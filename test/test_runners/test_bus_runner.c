#include "unity_fixture.h"

TEST_GROUP_RUNNER(Bus) {
    RUN_TEST_CASE(Bus, BusLoad);
    RUN_TEST_CASE(Bus, BusRead);
    RUN_TEST_CASE(Bus, BusWrite);
}