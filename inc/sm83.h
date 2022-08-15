#ifndef _SHARP_8080_H_
#define _SHARP_8080_H_
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
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
} sm83_t;

typedef enum {
  DNC,
  SET,
  CLR
} sm83_cc_t;

typedef struct {
  sm83_cc_t z;
  sm83_cc_t n;
  sm83_cc_t h;
  sm83_cc_t c;
} sm83_cc_table_t;

#define CC_DNC {DNC,DNC,DNC,DNC}
#define CC_Z {SET,DNC,DNC,DNC}
#define CC_C {DNC,DNC,DNC,SET}
#define CC_NZ {SET,SET,DNC,DNC}
#define CC_NC {DNC,SET,DNC,SET}

typedef enum {
  CTRL,
  JRS,
  JRSCOND,
  LDR_DIR,
  LDR_IND,
  ERROR
} sm83_type_t;

typedef struct {
  uint16_t opcode;
  sm83_type_t type;
  uint8_t cycles;
  uint8_t length;
  sm83_cc_table_t cc;
  char mnemonic[MAX_MNEMONIC_LEN];
} sm83_instr_t;

void sm83_init(sm83_t state);
void sm83_free();
uint16_t sm83_get_opcode();

const sm83_instr_t* sm83_decode();
void sm83_execute();
uint16_t sm83_get_pc();
const sm83_t* sm83_get_state();
sm83_t* sm83_get_modifiable_state();

uint8_t* sm83_get_register_operator_ldr_dir(uint16_t op);

uint8_t sm83_get_register_operand(uint16_t op);

void sm83_z_flag(bool set);
void sm83_n_flag(bool set);
void sm83_h_flag(bool set);
void sm83_c_flag(bool set);
sm83_cc_table_t sm83_get_flags_as_cc_table();
void sm83_set_flags_from_cc_table(sm83_cc_table_t cc);
bool sm83_get_flag_from_pos(uint8_t pos);
bool sm83_check_condition(sm83_cc_table_t cc);
#endif
