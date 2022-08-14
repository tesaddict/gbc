#include <string.h>
#include "unity_fixture.h"
#include "bus.h"

TEST_GROUP(Bus);

#define DATA_SZ 10
size_t test_idx_1;
size_t test_idx_2;
uint8_t data[DATA_SZ];

TEST_SETUP(Bus) {
    test_idx_1 = 1000;
    test_idx_2 = MAX_MEMORY_SZ - 1 - DATA_SZ;
    for(size_t i = 0; i < DATA_SZ; ++i) 
        data[i] = i;
}

TEST_TEAR_DOWN(Bus) {
    for(size_t i = 0; i < MAX_MEMORY_SZ; ++i)
        bus_write(i, 0);
    memset(&data[0], 0, DATA_SZ);
}

TEST(Bus, BusLoad) {
    TEST_ASSERT(bus_load(NULL, 0, 0) == BUS_STATUS_INVALID_PARAMETER);
    TEST_ASSERT(bus_load(&data[0], DATA_SZ, MAX_MEMORY_SZ) 
                         == BUS_STATUS_INDEX_OUT_OF_RANGE);
    TEST_ASSERT(bus_load(&data[0], DATA_SZ, MAX_MEMORY_SZ - DATA_SZ) 
                         == BUS_STATUS_INDEX_OUT_OF_RANGE);
    TEST_ASSERT(bus_load(&data[0], 0, 0) == BUS_STATUS_INVALID_PARAMETER);
    TEST_ASSERT(bus_load(&data[0], DATA_SZ, 0) == BUS_STATUS_OK);
    for(size_t i = 0; i < DATA_SZ; ++i)
        TEST_ASSERT(bus_read(i) == i);
    TEST_ASSERT(bus_load(&data[0], DATA_SZ, test_idx_1) == BUS_STATUS_OK);
    for(size_t i = test_idx_1; i < DATA_SZ + test_idx_1; ++i)
        TEST_ASSERT(bus_read(i) == i - test_idx_1);
}

TEST(Bus, BusRead) {
    bus_load(&data[0], DATA_SZ, 0);
    for(size_t i = 0; i < DATA_SZ; ++i)
        TEST_ASSERT(bus_read(i) == data[i]);
}

TEST(Bus, BusWrite) {
    for(size_t i = 0; i < DATA_SZ; ++i) {
        bus_write(i, data[i]);
        TEST_ASSERT(bus_read(i) == data[i]);
    }
}