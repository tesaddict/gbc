#ifndef _SHARP_8080_H_
#define _SHARP_8080_H_
#include <stdint.h>
#include <stdbool.h>

#define MAX_MNEMONIC_LEN 10

typedef struct {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t h;
  uint8_t l;
  bool zf;
  bool nf;
  bool hf;
  bool cf;
  uint16_t pc;
  uint16_t sp;
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
  NOP,
  JRS,
  JRSCOND,
  LDRDIR,
  LDRIND,
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
void sm83_clear();
sm83_t* sm83_get_cpu();

const sm83_instr_t* sm83_decode();
void sm83_execute();
#endif
