#include "bus.h"
#include <string.h>

static uint8_t memory[MAX_MEMORY_SZ];

int bus_load(uint8_t *data, size_t data_sz, size_t idx) {
    if (data == NULL || data_sz == 0)
        return BUS_STATUS_INVALID_PARAMETER;
    if (idx + data_sz >= MAX_MEMORY_SZ)
        return BUS_STATUS_INDEX_OUT_OF_RANGE;
    memcpy(memory + idx, data, data_sz);
    return BUS_STATUS_OK;
}

uint8_t bus_read(uint16_t address) {
    return memory[address];
}

void bus_write(uint16_t address, uint8_t value) {
    memory[address] = value;
}

void bus_clear() {
  memset(memory, 0, MAX_MEMORY_SZ);
}