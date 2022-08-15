#ifndef _BUS_H_
#define _BUS_H_
#include <stdint.h>
#include <stddef.h>

#define MAX_MEMORY_SZ 64 * 1024

typedef enum {
  BUS_STATUS_INVALID_PARAMETER = -1,
  BUS_STATUS_INDEX_OUT_OF_RANGE = -2,
  BUS_STATUS_OK = 0
} BUS_STATUS;

int bus_load(uint8_t *data, size_t data_sz, size_t idx);
uint8_t bus_read(uint16_t address);
void bus_write(uint16_t address, uint8_t value);
uint8_t *bus_ptr();
void bus_clear();
#endif
