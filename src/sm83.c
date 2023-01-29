#include "sm83.h"
#include "bus.h"

static void            sm83_wait(uint8_t cycles);
static bool            sm83_check_condition(sm83_cc_table_t cc);
static uint8_t*        sm83_get_register_map_ldr(uint16_t op);
static uint8_t         sm83_get_register_operand(uint16_t op);
static sm83_cc_table_t sm83_get_flags_as_cc_table();

static const sm83_instr_t instr_table[256] = {
    [0x00] = (sm83_instr_t){0x00, NOP, 4, 1, CC_DNC, "NOP"},
    // JRS
    [0x18] = (sm83_instr_t){0x18, JRS, 12, 2, CC_DNC, "JR_r8"},
    // JRSCOND
    [0x20] = (sm83_instr_t){0x20, JRSCOND, 8, 2, CC_NZ, "JR_NZ_r8"},
    [0x28] = (sm83_instr_t){0x28, JRSCOND, 8, 2, CC_Z, "JR_Z_r8"},
    [0x30] = (sm83_instr_t){0x30, JRSCOND, 8, 2, CC_NC, "JR_NC_r8"},
    [0x38] = (sm83_instr_t){0x38, JRSCOND, 8, 2, CC_C, "JR_C_r8"},
    // LDRDIR
    [0x40] = (sm83_instr_t){0x40, LDRDIR, 4, 1, CC_DNC, "LDB_B"},
    [0x41] = (sm83_instr_t){0x41, LDRDIR, 4, 1, CC_DNC, "LDB_C"},
    [0x42] = (sm83_instr_t){0x42, LDRDIR, 4, 1, CC_DNC, "LDB_D"},
    [0x43] = (sm83_instr_t){0x43, LDRDIR, 4, 1, CC_DNC, "LDB_E"},
    [0x44] = (sm83_instr_t){0x44, LDRDIR, 4, 1, CC_DNC, "LDB_H"},
    [0x45] = (sm83_instr_t){0x45, LDRDIR, 4, 1, CC_DNC, "LDB_L"},
    [0x47] = (sm83_instr_t){0x47, LDRDIR, 4, 1, CC_DNC, "LDB_A"},
    [0x48] = (sm83_instr_t){0x48, LDRDIR, 4, 1, CC_DNC, "LDC_B"},
    [0x49] = (sm83_instr_t){0x49, LDRDIR, 4, 1, CC_DNC, "LDC_C"},
    [0x4a] = (sm83_instr_t){0x4a, LDRDIR, 4, 1, CC_DNC, "LDC_D"},
    [0x4b] = (sm83_instr_t){0x4b, LDRDIR, 4, 1, CC_DNC, "LDC_E"},
    [0x4c] = (sm83_instr_t){0x4c, LDRDIR, 4, 1, CC_DNC, "LDC_H"},
    [0x4d] = (sm83_instr_t){0x4d, LDRDIR, 4, 1, CC_DNC, "LDC_L"},
    [0x4f] = (sm83_instr_t){0x4f, LDRDIR, 4, 1, CC_DNC, "LDC_A"},
    [0x50] = (sm83_instr_t){0x50, LDRDIR, 4, 1, CC_DNC, "LDD_B"},
    [0x51] = (sm83_instr_t){0x51, LDRDIR, 4, 1, CC_DNC, "LDD_C"},
    [0x52] = (sm83_instr_t){0x52, LDRDIR, 4, 1, CC_DNC, "LDD_D"},
    [0x53] = (sm83_instr_t){0x53, LDRDIR, 4, 1, CC_DNC, "LDD_E"},
    [0x54] = (sm83_instr_t){0x54, LDRDIR, 4, 1, CC_DNC, "LDD_H"},
    [0x55] = (sm83_instr_t){0x55, LDRDIR, 4, 1, CC_DNC, "LDD_L"},
    [0x57] = (sm83_instr_t){0x57, LDRDIR, 4, 1, CC_DNC, "LDD_A"},
    [0x58] = (sm83_instr_t){0x58, LDRDIR, 4, 1, CC_DNC, "LDE_B"},
    [0x59] = (sm83_instr_t){0x59, LDRDIR, 4, 1, CC_DNC, "LDE_C"},
    [0x5a] = (sm83_instr_t){0x5a, LDRDIR, 4, 1, CC_DNC, "LDE_D"},
    [0x5b] = (sm83_instr_t){0x5b, LDRDIR, 4, 1, CC_DNC, "LDE_E"},
    [0x5c] = (sm83_instr_t){0x5c, LDRDIR, 4, 1, CC_DNC, "LDE_H"},
    [0x5d] = (sm83_instr_t){0x5d, LDRDIR, 4, 1, CC_DNC, "LDE_L"},
    [0x5f] = (sm83_instr_t){0x5f, LDRDIR, 4, 1, CC_DNC, "LDE_A"},
    [0x60] = (sm83_instr_t){0x60, LDRDIR, 4, 1, CC_DNC, "LDH_B"},
    [0x61] = (sm83_instr_t){0x61, LDRDIR, 4, 1, CC_DNC, "LDH_C"},
    [0x62] = (sm83_instr_t){0x62, LDRDIR, 4, 1, CC_DNC, "LDH_D"},
    [0x63] = (sm83_instr_t){0x63, LDRDIR, 4, 1, CC_DNC, "LDH_E"},
    [0x64] = (sm83_instr_t){0x64, LDRDIR, 4, 1, CC_DNC, "LDH_H"},
    [0x65] = (sm83_instr_t){0x65, LDRDIR, 4, 1, CC_DNC, "LDH_L"},
    [0x67] = (sm83_instr_t){0x67, LDRDIR, 4, 1, CC_DNC, "LDH_A"},
    [0x68] = (sm83_instr_t){0x68, LDRDIR, 4, 1, CC_DNC, "LDL_B"},
    [0x69] = (sm83_instr_t){0x69, LDRDIR, 4, 1, CC_DNC, "LDL_C"},
    [0x6a] = (sm83_instr_t){0x6a, LDRDIR, 4, 1, CC_DNC, "LDL_D"},
    [0x6b] = (sm83_instr_t){0x6b, LDRDIR, 4, 1, CC_DNC, "LDL_E"},
    [0x6c] = (sm83_instr_t){0x6c, LDRDIR, 4, 1, CC_DNC, "LDL_H"},
    [0x6d] = (sm83_instr_t){0x6d, LDRDIR, 4, 1, CC_DNC, "LDL_L"},
    [0x6f] = (sm83_instr_t){0x6f, LDRDIR, 4, 1, CC_DNC, "LDL_A"},
    [0x78] = (sm83_instr_t){0x78, LDRDIR, 4, 1, CC_DNC, "LDA_B"},
    [0x79] = (sm83_instr_t){0x79, LDRDIR, 4, 1, CC_DNC, "LDA_C"},
    [0x7a] = (sm83_instr_t){0x7a, LDRDIR, 4, 1, CC_DNC, "LDA_D"},
    [0x7b] = (sm83_instr_t){0x7b, LDRDIR, 4, 1, CC_DNC, "LDA_E"},
    [0x7c] = (sm83_instr_t){0x7c, LDRDIR, 4, 1, CC_DNC, "LDA_H"},
    [0x7d] = (sm83_instr_t){0x7d, LDRDIR, 4, 1, CC_DNC, "LDA_L"},
    [0x7f] = (sm83_instr_t){0x7f, LDRDIR, 4, 1, CC_DNC, "LDA_A"},
    // LDR_IND
    [0x46] = (sm83_instr_t){0x46, LDRIND, 8, 1, CC_DNC, "LDB_HL"},
    [0x4e] = (sm83_instr_t){0x4e, LDRIND, 8, 1, CC_DNC, "LDC_HL"},
    [0x56] = (sm83_instr_t){0x56, LDRIND, 8, 1, CC_DNC, "LDD_HL"},
    [0x5e] = (sm83_instr_t){0x5e, LDRIND, 8, 1, CC_DNC, "LDE_HL"},
    [0x66] = (sm83_instr_t){0x66, LDRIND, 8, 1, CC_DNC, "LDH_HL"},
    [0x6e] = (sm83_instr_t){0x6e, LDRIND, 8, 1, CC_DNC, "LDL_HL"},
    [0x7e] = (sm83_instr_t){0x7e, LDRIND, 8, 1, CC_DNC, "LDA_HL"},
};
static sm83_t cpu;

void sm83_init(sm83_t state) {
  cpu = state;
}

void sm83_clear() {
  cpu = (sm83_t){0,0,0,0,0,0,0,false,false,false,false,0x100,0};
}

sm83_t* sm83_get_cpu() {
  return &cpu;
}

static void sm83_wait(uint8_t cycles) {
  (void) cycles;
}

const sm83_instr_t* sm83_decode() {
  uint16_t opcode = (uint16_t)bus_read(cpu.pc);
  return &instr_table[opcode == 0xCB ? (opcode << 8) | bus_read(cpu.pc + 1) : opcode];
}

static inline int8_t signed_8(uint16_t address) {
  return (int8_t)bus_read(address);
}

void sm83_execute() {
  const sm83_instr_t *instr = sm83_decode();
  uint8_t extra_cycles = 0;
  int16_t pc_offset = instr->length;
  switch (instr->type) {
    case NOP:
      break;
    case JRS:
      pc_offset = signed_8(cpu.pc+1);
      break;
    case JRSCOND:
      if (sm83_check_condition(instr->cc)) {
        pc_offset = signed_8(cpu.pc+1);
        extra_cycles = 4;
      }
      break;
    case LDRDIR:
    case LDRIND:
      *(sm83_get_register_map_ldr(instr->opcode)) = sm83_get_register_operand(instr->opcode);
      break;
    case ERROR:
      break;
  }
  cpu.pc += pc_offset;
  sm83_wait(instr->cycles + extra_cycles);
}

static uint8_t* sm83_get_register_map_ldr(uint16_t op) {
  if (op >= 0x40 && op <= 0x47) return &cpu.b;
  if (op >= 0x48 && op <= 0x4f) return &cpu.c;
  if (op >= 0x50 && op <= 0x57) return &cpu.d;
  if (op >= 0x58 && op <= 0x5f) return &cpu.e;
  if (op >= 0x60 && op <= 0x67) return &cpu.h;
  if (op >= 0x68 && op <= 0x6f) return &cpu.l;
  if (op >= 0x78 && op <= 0x7f) return &cpu.a;
  return NULL;
}

static uint8_t sm83_get_register_operand(uint16_t op) {
  switch(op % 0x8) {
    case 0x0: return cpu.b;
    case 0x1: return cpu.c;
    case 0x2: return cpu.d;
    case 0x3: return cpu.e;
    case 0x4: return cpu.h;
    case 0x5: return cpu.l;
    case 0x6: return bus_read(((uint16_t)cpu.h) << 8 | (uint16_t)cpu.l);
    case 0x7: return cpu.a;
  }
  return 0;
}

static sm83_cc_table_t sm83_get_flags_as_cc_table() {
  sm83_cc_table_t cc;
  cc.z = cpu.zf ? SET : CLR;
  cc.n = cpu.nf ? SET : CLR;
  cc.h = cpu.hf ? SET : CLR;
  cc.c = cpu.cf ? SET : CLR;
  return cc;
}

static bool sm83_check_condition(sm83_cc_table_t cc) {
  sm83_cc_table_t flags = sm83_get_flags_as_cc_table();
  if (cc.z != DNC && cc.z != flags.z) return false;
  if (cc.n != DNC && cc.n != flags.n) return false;
  if (cc.h != DNC && cc.h != flags.h) return false;
  if (cc.c != DNC && cc.c != flags.c) return false;
  return true;
}