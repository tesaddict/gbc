#include "unity_fixture.h"
#include "sharp8080.h"

uint16_t pc;

TEST_GROUP(Sharp8080);

static sharp8080_cc_table_t cc_dnc = {DNC,DNC,DNC,DNC};

TEST_SETUP(Sharp8080) {
  pc = 0x100;
  sharp8080_t cpu = {0,0,0,0,0,0,0,0,pc};
  sharp8080_init(cpu);
}

TEST_TEAR_DOWN(Sharp8080) {
  sharp8080_free();
}

TEST(Sharp8080, Sharp8080_Get_Opcode) {
  bus_write(pc, 0xCB);
  bus_write(pc + 1, 0x00);
  TEST_ASSERT(sharp8080_get_opcode() == 0xCB00);
  bus_write(pc, 0x00);
  TEST_ASSERT(sharp8080_get_opcode() == 0x0000);
  bus_write(pc, 0xFD);
  TEST_ASSERT(sharp8080_get_opcode() == 0x00FD);
}

static sharp8080_instr_t wad(uint8_t opcode) {
  bus_write(pc, opcode);
  return sharp8080_decode();
}

static void dh(uint16_t opcode,
               sharp8080_type_t type,
               uint8_t cycles,
               uint8_t length,
               sharp8080_cc_table_t cc,
               char mnemonic[MAX_MNEMONIC_LEN])
{
  sharp8080_instr_t instr = wad(opcode);
  TEST_ASSERT(instr.opcode == opcode);
  TEST_ASSERT(instr.type == type);
  TEST_ASSERT(instr.cycles == cycles);
  TEST_ASSERT(instr.length == length);
  TEST_ASSERT(instr.cc.z == cc.z);
  TEST_ASSERT(instr.cc.n == cc.n);
  TEST_ASSERT(instr.cc.h == cc.h);
  TEST_ASSERT(instr.cc.c == cc.c);
  for (size_t i = 0; i < MAX_MNEMONIC_LEN; ++i) {
    if (instr.mnemonic[i] == '\0' && mnemonic[i] == '\0')
      break;
    TEST_ASSERT(instr.mnemonic[i] == mnemonic[i]);
  }
}

TEST(Sharp8080, Sharp8080_Decode_Misc) {
  dh(0x00,CTRL,4,1,cc_dnc,"NOP");
  dh(0x10,CTRL,4,1,cc_dnc,"STOP");
  dh(0x76,CTRL,4,1,cc_dnc,"HALT");
  dh(0xF3,CTRL,4,1,cc_dnc,"DI");
  dh(0xFB,CTRL,4,1,cc_dnc,"EI");
}

TEST(Sharp8080, Sharp8080_Decode_JRS) {
  dh(0x18,JRS,12,2,cc_dnc,"JR_r8");
}

TEST(Sharp8080, Sharp8080_Decode_JRSCOND) {
  dh(0x20,JRSCOND,12,2,(sharp8080_cc_table_t){SET,SET,DNC,DNC},"JR_NZ_r8");
  dh(0x28,JRSCOND,12,2,(sharp8080_cc_table_t){SET,DNC,DNC,DNC},"JR_Z_r8");
  dh(0x30,JRSCOND,12,2,(sharp8080_cc_table_t){DNC,SET,DNC,SET},"JR_NC_r8");
  dh(0x38,JRSCOND,12,2,(sharp8080_cc_table_t){DNC,DNC,DNC,SET},"JR_C_r8");
}

TEST(Sharp8080, Sharp8080_Execute_JRS) {
  bus_write(pc, 0x18);
  bus_write(pc + 1, -10);
  sharp8080_execute();
  TEST_ASSERT(sharp8080_get_pc() == pc-10);
}

TEST(Sharp8080, Sharp8080_Execute_JRSCOND) {
  bus_write(pc, 0x20);
  bus_write(pc + 1, -10);
  sharp8080_execute();
  TEST_ASSERT(sharp8080_get_pc() == pc);
}

static void get_flag_helper(sharp8080_cc_table_t expected) {
  sharp8080_z_flag(expected.z == SET ? true : false);
  sharp8080_n_flag(expected.n == SET ? true : false);
  sharp8080_h_flag(expected.h == SET ? true : false);
  sharp8080_c_flag(expected.c == SET ? true : false);
  sharp8080_cc_table_t result = sharp8080_get_flags_as_cc_table();
  TEST_ASSERT(expected.z == result.z);
  TEST_ASSERT(expected.n == result.n);
  TEST_ASSERT(expected.h == result.h);
  TEST_ASSERT(expected.c == result.c);
}

TEST(Sharp8080, Sharp8080_Set_Clear_Flags) {
  get_flag_helper((sharp8080_cc_table_t){SET,SET,SET,SET});
  get_flag_helper((sharp8080_cc_table_t){SET,SET,SET,CLR});
  get_flag_helper((sharp8080_cc_table_t){SET,SET,CLR,CLR});
  get_flag_helper((sharp8080_cc_table_t){SET,CLR,CLR,CLR});
  get_flag_helper((sharp8080_cc_table_t){CLR,CLR,CLR,CLR});
}

TEST(Sharp8080, Sharp8080_Set_Get_Flags_From_CC_Table) {
  sharp8080_set_flags_from_cc_table((sharp8080_cc_table_t){SET,CLR,SET,CLR});
  sharp8080_cc_table_t cc = sharp8080_get_flags_as_cc_table();
  TEST_ASSERT(cc.z == SET);
  TEST_ASSERT(cc.n == CLR);
  TEST_ASSERT(cc.h == SET);
  TEST_ASSERT(cc.c == CLR);
}

TEST(Sharp8080, Sharp8080_Set_Get_Flags) {
  sharp8080_set_flags_from_cc_table((sharp8080_cc_table_t){SET,CLR,SET,CLR});
  TEST_ASSERT(sharp8080_get_flag_from_pos(Z_FLAG_POS) == true);
  TEST_ASSERT(sharp8080_get_flag_from_pos(N_FLAG_POS) == false);
  TEST_ASSERT(sharp8080_get_flag_from_pos(H_FLAG_POS) == true);
  TEST_ASSERT(sharp8080_get_flag_from_pos(C_FLAG_POS) == false);
}