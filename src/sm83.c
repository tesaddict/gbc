#include "sm83.h"
#include "bus.h"

static sm83_t cpu;

void sm83_init(sm83_t state) {
  cpu = state;
}

void sm83_wait(uint8_t cycles) {
  (void) cycles;
}

void sm83_free() {
  sm83_t free = {0,0,0,0,0,0,0,0,0x100};
  cpu = free;
}

uint16_t sm83_get_opcode() {
  uint16_t opcode = (uint16_t)bus_read(cpu.pc);
  return opcode == 0xCB ? (opcode << 8) | bus_read(cpu.pc + 1) : opcode;
}

static const sm83_instr_t instr_table[256] = {
    [0x00] = (sm83_instr_t){0x00, CTRL, 4, 1, CC_DNC, "NOP"},
    [0x10] = (sm83_instr_t){0x10, CTRL, 4, 1, CC_DNC, "STOP"},
    [0x76] = (sm83_instr_t){0x76, CTRL, 4, 1, CC_DNC, "HALT"},
    [0xF3] = (sm83_instr_t){0xF3, CTRL, 4, 1, CC_DNC, "DI"},
    [0xFB] = (sm83_instr_t){0xFB, CTRL, 4, 1, CC_DNC, "EI"},
    // JRS
    [0x18] = (sm83_instr_t){0x18, JRS, 12, 2, CC_DNC, "JR_r8"},
    // JRSCOND
    [0x20] = (sm83_instr_t){0x20, JRSCOND, 8, 2, CC_NZ, "JR_NZ_r8"},
    [0x28] = (sm83_instr_t){0x28, JRSCOND, 8, 2, CC_Z, "JR_Z_r8"},
    [0x30] = (sm83_instr_t){0x30, JRSCOND, 8, 2, CC_NC, "JR_NC_r8"},
    [0x38] = (sm83_instr_t){0x38, JRSCOND, 8, 2, CC_C, "JR_C_r8"},
    // LDR_DIR
    [0x40] = (sm83_instr_t){0x40, LDR_DIR, 4, 1, CC_DNC, "LDB_B"},
    [0x41] = (sm83_instr_t){0x41, LDR_DIR, 4, 1, CC_DNC, "LDB_C"},
    [0x42] = (sm83_instr_t){0x42, LDR_DIR, 4, 1, CC_DNC, "LDB_D"},
    [0x43] = (sm83_instr_t){0x43, LDR_DIR, 4, 1, CC_DNC, "LDB_E"},
    [0x44] = (sm83_instr_t){0x44, LDR_DIR, 4, 1, CC_DNC, "LDB_H"},
    [0x45] = (sm83_instr_t){0x45, LDR_DIR, 4, 1, CC_DNC, "LDB_L"},
    [0x47] = (sm83_instr_t){0x47, LDR_DIR, 4, 1, CC_DNC, "LDB_A"},
    [0x48] = (sm83_instr_t){0x48, LDR_DIR, 4, 1, CC_DNC, "LDC_B"},
    [0x49] = (sm83_instr_t){0x49, LDR_DIR, 4, 1, CC_DNC, "LDC_C"},
    [0x4a] = (sm83_instr_t){0x4a, LDR_DIR, 4, 1, CC_DNC, "LDC_D"},
    [0x4b] = (sm83_instr_t){0x4b, LDR_DIR, 4, 1, CC_DNC, "LDC_E"},
    [0x4c] = (sm83_instr_t){0x4c, LDR_DIR, 4, 1, CC_DNC, "LDC_H"},
    [0x4d] = (sm83_instr_t){0x4d, LDR_DIR, 4, 1, CC_DNC, "LDC_L"},
    [0x4f] = (sm83_instr_t){0x4f, LDR_DIR, 4, 1, CC_DNC, "LDC_A"},
    [0x50] = (sm83_instr_t){0x50, LDR_DIR, 4, 1, CC_DNC, "LDD_B"},
    [0x51] = (sm83_instr_t){0x51, LDR_DIR, 4, 1, CC_DNC, "LDD_C"},
    [0x52] = (sm83_instr_t){0x52, LDR_DIR, 4, 1, CC_DNC, "LDD_D"},
    [0x53] = (sm83_instr_t){0x53, LDR_DIR, 4, 1, CC_DNC, "LDD_E"},
    [0x54] = (sm83_instr_t){0x54, LDR_DIR, 4, 1, CC_DNC, "LDD_H"},
    [0x55] = (sm83_instr_t){0x55, LDR_DIR, 4, 1, CC_DNC, "LDD_L"},
    [0x57] = (sm83_instr_t){0x57, LDR_DIR, 4, 1, CC_DNC, "LDD_A"},
    [0x58] = (sm83_instr_t){0x58, LDR_DIR, 4, 1, CC_DNC, "LDE_B"},
    [0x59] = (sm83_instr_t){0x59, LDR_DIR, 4, 1, CC_DNC, "LDE_C"},
    [0x5a] = (sm83_instr_t){0x5a, LDR_DIR, 4, 1, CC_DNC, "LDE_D"},
    [0x5b] = (sm83_instr_t){0x5b, LDR_DIR, 4, 1, CC_DNC, "LDE_E"},
    [0x5c] = (sm83_instr_t){0x5c, LDR_DIR, 4, 1, CC_DNC, "LDE_H"},
    [0x5d] = (sm83_instr_t){0x5d, LDR_DIR, 4, 1, CC_DNC, "LDE_L"},
    [0x5f] = (sm83_instr_t){0x5f, LDR_DIR, 4, 1, CC_DNC, "LDE_A"},
    [0x60] = (sm83_instr_t){0x60, LDR_DIR, 4, 1, CC_DNC, "LDH_B"},
    [0x61] = (sm83_instr_t){0x61, LDR_DIR, 4, 1, CC_DNC, "LDH_C"},
    [0x62] = (sm83_instr_t){0x62, LDR_DIR, 4, 1, CC_DNC, "LDH_D"},
    [0x63] = (sm83_instr_t){0x63, LDR_DIR, 4, 1, CC_DNC, "LDH_E"},
    [0x64] = (sm83_instr_t){0x64, LDR_DIR, 4, 1, CC_DNC, "LDH_H"},
    [0x65] = (sm83_instr_t){0x65, LDR_DIR, 4, 1, CC_DNC, "LDH_L"},
    [0x67] = (sm83_instr_t){0x67, LDR_DIR, 4, 1, CC_DNC, "LDH_A"},
    [0x68] = (sm83_instr_t){0x68, LDR_DIR, 4, 1, CC_DNC, "LDL_B"},
    [0x69] = (sm83_instr_t){0x69, LDR_DIR, 4, 1, CC_DNC, "LDL_C"},
    [0x6a] = (sm83_instr_t){0x6a, LDR_DIR, 4, 1, CC_DNC, "LDL_D"},
    [0x6b] = (sm83_instr_t){0x6b, LDR_DIR, 4, 1, CC_DNC, "LDL_E"},
    [0x6c] = (sm83_instr_t){0x6c, LDR_DIR, 4, 1, CC_DNC, "LDL_H"},
    [0x6d] = (sm83_instr_t){0x6d, LDR_DIR, 4, 1, CC_DNC, "LDL_L"},
    [0x6f] = (sm83_instr_t){0x6f, LDR_DIR, 4, 1, CC_DNC, "LDL_A"},
    [0x78] = (sm83_instr_t){0x78, LDR_DIR, 4, 1, CC_DNC, "LDA_B"},
    [0x79] = (sm83_instr_t){0x79, LDR_DIR, 4, 1, CC_DNC, "LDA_C"},
    [0x7a] = (sm83_instr_t){0x7a, LDR_DIR, 4, 1, CC_DNC, "LDA_D"},
    [0x7b] = (sm83_instr_t){0x7b, LDR_DIR, 4, 1, CC_DNC, "LDA_E"},
    [0x7c] = (sm83_instr_t){0x7c, LDR_DIR, 4, 1, CC_DNC, "LDA_H"},
    [0x7d] = (sm83_instr_t){0x7d, LDR_DIR, 4, 1, CC_DNC, "LDA_L"},
    [0x7f] = (sm83_instr_t){0x7f, LDR_DIR, 4, 1, CC_DNC, "LDA_A"},
    // LDR_IND
    [0x46] = (sm83_instr_t){0x46, LDR_IND, 8, 1, CC_DNC, "LDB_HL"},
    [0x4e] = (sm83_instr_t){0x4e, LDR_IND, 8, 1, CC_DNC, "LDC_HL"},
    [0x56] = (sm83_instr_t){0x56, LDR_IND, 8, 1, CC_DNC, "LDD_HL"},
    [0x5e] = (sm83_instr_t){0x5e, LDR_IND, 8, 1, CC_DNC, "LDE_HL"},
    [0x66] = (sm83_instr_t){0x66, LDR_IND, 8, 1, CC_DNC, "LDH_HL"},
    [0x6e] = (sm83_instr_t){0x6e, LDR_IND, 8, 1, CC_DNC, "LDL_HL"},
    [0x7e] = (sm83_instr_t){0x7e, LDR_IND, 8, 1, CC_DNC, "LDA_HL"},
};

const sm83_instr_t* sm83_decode() {
  uint16_t opcode = sm83_get_opcode();
  return &instr_table[opcode];
}

static inline int8_t signed_8(uint16_t address) {
  return (int8_t)bus_read(address);
}

void sm83_execute() {
  const sm83_instr_t *instr = sm83_decode();
  uint8_t cycle_offset = 0;
  bool advance_pc = true;
  switch (instr->type) {
    case CTRL:
      break;
    case JRS:
      cpu.pc += signed_8(cpu.pc+1);
      advance_pc = false;
      break;
    case JRSCOND:
      if (sm83_check_condition(instr->cc)) {
        cpu.pc += signed_8(cpu.pc+1);
        cycle_offset = 4;
        advance_pc = false;
      }
      break;
    case LDR_DIR:
    case LDR_IND:
      *(sm83_get_register_operator_ldr_dir(instr->opcode)) = 
        sm83_get_register_operand(instr->opcode);
      break;
    case ERROR:
      break;
  }
  if (advance_pc) cpu.pc += instr->length;
  sm83_wait(instr->cycles + cycle_offset);
}

uint16_t sm83_get_pc() {
  return cpu.pc;
}

const sm83_t* sm83_get_state() {
  return &cpu;
}

sm83_t* sm83_get_modifiable_state() {
  return &cpu;
}

uint8_t* sm83_get_register_operator_ldr_dir(uint16_t op) {
  if (op >= 0x40 && op <= 0x47) return &cpu.b;
  if (op >= 0x48 && op <= 0x4f) return &cpu.c;
  if (op >= 0x50 && op <= 0x57) return &cpu.d;
  if (op >= 0x58 && op <= 0x5f) return &cpu.e;
  if (op >= 0x60 && op <= 0x67) return &cpu.h;
  if (op >= 0x68 && op <= 0x6f) return &cpu.l;
  if (op >= 0x78 && op <= 0x7f) return &cpu.a;
  return NULL;
}

uint8_t sm83_get_register_operand(uint16_t op) {
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

static void sm83_flag_helper(bool set, uint8_t pos) {
  if (set) cpu.f |= (1 << pos);
  else cpu.f &= ~(1 << pos);
}

void sm83_z_flag(bool set) {
  sm83_flag_helper(set, Z_FLAG_POS);
}

void sm83_n_flag(bool set) {
  sm83_flag_helper(set, N_FLAG_POS);
}

void sm83_h_flag(bool set) {
  sm83_flag_helper(set, H_FLAG_POS);
}

void sm83_c_flag(bool set) {
  sm83_flag_helper(set, C_FLAG_POS);
}

sm83_cc_table_t sm83_get_flags_as_cc_table() {
  sm83_cc_table_t cc;
  cc.z = cpu.f & (1 << Z_FLAG_POS) ? SET : CLR;
  cc.n = cpu.f & (1 << N_FLAG_POS) ? SET : CLR;
  cc.h = cpu.f & (1 << H_FLAG_POS) ? SET : CLR;
  cc.c = cpu.f & (1 << C_FLAG_POS) ? SET : CLR;
  return cc;
}

void sm83_set_flags_from_cc_table(sm83_cc_table_t cc) {
  if (cc.z != DNC) sm83_z_flag(cc.z == SET);
  if (cc.n != DNC) sm83_n_flag(cc.n == SET);
  if (cc.h != DNC) sm83_h_flag(cc.h == SET);
  if (cc.c != DNC) sm83_c_flag(cc.c == SET);
}

bool sm83_get_flag_from_pos(uint8_t pos) {
  return cpu.f & (1 << pos);
}

bool sm83_check_condition(sm83_cc_table_t cc) {
  sm83_cc_table_t flags = sm83_get_flags_as_cc_table();
  if (cc.z != DNC && cc.z != flags.z) return false;
  if (cc.n != DNC && cc.n != flags.n) return false;
  if (cc.h != DNC && cc.h != flags.h) return false;
  if (cc.c != DNC && cc.c != flags.c) return false;
  return true;
}