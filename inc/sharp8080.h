#ifndef _SHARP_8080_H_
#define _SHARP_8080_H_
#include <stdint.h>
#include <stdbool.h>
#include "bus.h"

#define MAX_MNEMONIC_LEN 10
#define Z_FLAG_POS 7
#define N_FLAG_POS 6
#define H_FLAG_POS 5
#define C_FLAG_POS 4

typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  uint8_t f;
  uint16_t pc;
} sharp8080_t;

typedef enum {
  DNC,
  SET,
  CLR
} sharp8080_cc_t;

typedef struct {
  sharp8080_cc_t z;
  sharp8080_cc_t n;
  sharp8080_cc_t h;
  sharp8080_cc_t c;
} sharp8080_cc_table_t;

#define CC_DNC {DNC,DNC,DNC,DNC}
#define CC_Z {SET,DNC,DNC,DNC}
#define CC_C {DNC,DNC,DNC,SET}
#define CC_NZ {SET,SET,DNC,DNC}
#define CC_NC {DNC,SET,DNC,SET}

typedef enum {
  CTRL,
  JRS,
  JRSCOND,
  ERROR
} sharp8080_type_t;

typedef struct {
  uint16_t opcode;
  sharp8080_type_t type;
  uint8_t cycles;
  uint8_t length;
  sharp8080_cc_table_t cc;
  char mnemonic[MAX_MNEMONIC_LEN];
} sharp8080_instr_t;

void sharp8080_init(sharp8080_t state);
void sharp8080_free();
uint16_t sharp8080_get_opcode();

sharp8080_instr_t sharp8080_decode();
void sharp8080_execute();
uint16_t sharp8080_get_pc();

void sharp8080_z_flag(bool set);
void sharp8080_n_flag(bool set);
void sharp8080_h_flag(bool set);
void sharp8080_c_flag(bool set);
sharp8080_cc_table_t sharp8080_get_flags_as_cc_table();
void sharp8080_set_flags_from_cc_table(sharp8080_cc_table_t cc);
bool sharp8080_get_flag_from_pos(uint8_t pos);
#endif
