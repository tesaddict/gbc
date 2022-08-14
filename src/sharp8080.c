#include "sharp8080.h"
#include "bus.h"

static sharp8080_t cpu;

void sharp8080_init(sharp8080_t state) {
  cpu = state;
}

void sharp8080_wait(uint8_t cycles) {
  (void) cycles;
}

void sharp8080_free() {
  sharp8080_t free = {0,0,0,0,0,0,0,0,0x100};
  cpu = free;
}

uint16_t sharp8080_get_opcode() {
  uint16_t opcode = (uint16_t)bus_read(cpu.pc);
  return opcode == 0xCB ? (opcode << 8) | bus_read(cpu.pc + 1) : opcode;
}

sharp8080_instr_t sharp8080_decode() {
  uint16_t opcode = sharp8080_get_opcode();
  switch (opcode) {
    // CTRL
    case 0x00: return (sharp8080_instr_t){opcode,CTRL,4,1,CC_DNC,"NOP"};
    case 0x10: return (sharp8080_instr_t){opcode,CTRL,4,1,CC_DNC,"STOP"};
    case 0x76: return (sharp8080_instr_t){opcode,CTRL,4,1,CC_DNC,"HALT"};
    case 0xF3: return (sharp8080_instr_t){opcode,CTRL,4,1,CC_DNC,"DI"};
    case 0xFB: return (sharp8080_instr_t){opcode,CTRL,4,1,CC_DNC,"EI"};
    // JRS
    case 0x18: return (sharp8080_instr_t){opcode,JRS,12,2,CC_DNC,"JR_r8"};
    // JRSCOND
    case 0x20: return (sharp8080_instr_t){opcode,JRSCOND,12,2,CC_NZ,"JR_NZ_r8"};
    case 0x28: return (sharp8080_instr_t){opcode,JRSCOND,12,2,CC_Z,"JR_Z_r8"};
    case 0x30: return (sharp8080_instr_t){opcode,JRSCOND,12,2,CC_NC,"JR_NC_r8"};
    case 0x38: return (sharp8080_instr_t){opcode,JRSCOND,12,2,CC_C,"JR_C_r8"};

  }
  return (sharp8080_instr_t){0x00,ERROR,0,0,CC_DNC,"ERROR"};
}

static inline int8_t signed_8(uint16_t address) {
  return (int8_t)bus_read(address);
}

void sharp8080_execute() {
  sharp8080_instr_t instr = sharp8080_decode();
  switch (instr.type) {
    case CTRL:
      break;
    case JRS:
      cpu.pc += signed_8(cpu.pc+1);
      break;
    case JRSCOND:
      break;
    case ERROR:
      break;
  }
  sharp8080_wait(instr.cycles);
}

uint16_t sharp8080_get_pc() {
  return cpu.pc;
}

static void sharp8080_flag_helper(bool set, uint8_t pos) {
  if (set) cpu.f |= (1 << pos);
  else cpu.f &= ~(1 << pos);
}

void sharp8080_z_flag(bool set) {
  sharp8080_flag_helper(set, Z_FLAG_POS);
}

void sharp8080_n_flag(bool set) {
  sharp8080_flag_helper(set, N_FLAG_POS);
}

void sharp8080_h_flag(bool set) {
  sharp8080_flag_helper(set, H_FLAG_POS);
}

void sharp8080_c_flag(bool set) {
  sharp8080_flag_helper(set, C_FLAG_POS);
}

sharp8080_cc_table_t sharp8080_get_flags_as_cc_table() {
  sharp8080_cc_table_t cc;
  cc.z = cpu.f & (1 << Z_FLAG_POS) ? SET : CLR;
  cc.n = cpu.f & (1 << N_FLAG_POS) ? SET : CLR;
  cc.h = cpu.f & (1 << H_FLAG_POS) ? SET : CLR;
  cc.c = cpu.f & (1 << C_FLAG_POS) ? SET : CLR;
  return cc;
}

void sharp8080_set_flags_from_cc_table(sharp8080_cc_table_t cc) {
  sharp8080_z_flag(cc.z == SET);
  sharp8080_n_flag(cc.n == SET);
  sharp8080_h_flag(cc.h == SET);
  sharp8080_c_flag(cc.c == SET);
}

bool sharp8080_get_flag_from_pos(uint8_t pos) {
  return cpu.f & (1 << pos);
}