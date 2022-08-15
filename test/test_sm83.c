#include "unity_fixture.h"
#include "sm83.h"

uint16_t pc;
const sm83_t *cpu;

TEST_GROUP(sm83);

static sm83_cc_table_t cc_dnc = {DNC,DNC,DNC,DNC};

TEST_SETUP(sm83) {
  pc = 0x100;
  sm83_t cpu_temp = {0,0,0,0,0,0,0,0,pc};
  sm83_init(cpu_temp);
  cpu = sm83_get_state();
}

TEST_TEAR_DOWN(sm83) {
  sm83_free();
  bus_clear();
}

TEST(sm83, sm83_Get_Opcode) {
  bus_write(pc, 0xCB);
  bus_write(pc + 1, 0x00);
  TEST_ASSERT(sm83_get_opcode() == 0xCB00);
  bus_write(pc, 0x00);
  TEST_ASSERT(sm83_get_opcode() == 0x0000);
  bus_write(pc, 0xFD);
  TEST_ASSERT(sm83_get_opcode() == 0x00FD);
}

static void dh(uint16_t opcode,
               sm83_type_t type,
               uint8_t cycles,
               uint8_t length,
               sm83_cc_table_t cc,
               char mnemonic[MAX_MNEMONIC_LEN])
{
  bus_write(pc, opcode);
  const sm83_instr_t *instr = sm83_decode();
  TEST_ASSERT(instr->opcode == opcode);
  TEST_ASSERT(instr->type == type);
  TEST_ASSERT(instr->cycles == cycles);
  TEST_ASSERT(instr->length == length);
  TEST_ASSERT(instr->cc.z == cc.z);
  TEST_ASSERT(instr->cc.n == cc.n);
  TEST_ASSERT(instr->cc.h == cc.h);
  TEST_ASSERT(instr->cc.c == cc.c);
  for (size_t i = 0; i < MAX_MNEMONIC_LEN; ++i) {
    if (instr->mnemonic[i] == '\0' && mnemonic[i] == '\0')
      break;
    TEST_ASSERT(instr->mnemonic[i] == mnemonic[i]);
  }
}

TEST(sm83, sm83_Decode_Misc) {
  dh(0x00,CTRL,4,1,cc_dnc,"NOP");
  dh(0x10,CTRL,4,1,cc_dnc,"STOP");
  dh(0x76,CTRL,4,1,cc_dnc,"HALT");
  dh(0xF3,CTRL,4,1,cc_dnc,"DI");
  dh(0xFB,CTRL,4,1,cc_dnc,"EI");
}

TEST(sm83, sm83_Decode_JRS) {
  dh(0x18,JRS,12,2,cc_dnc,"JR_r8");
}

TEST(sm83, sm83_Decode_JRSCOND) {
  dh(0x20,JRSCOND,8,2,(sm83_cc_table_t){SET,SET,DNC,DNC},"JR_NZ_r8");
  dh(0x28,JRSCOND,8,2,(sm83_cc_table_t){SET,DNC,DNC,DNC},"JR_Z_r8");
  dh(0x30,JRSCOND,8,2,(sm83_cc_table_t){DNC,SET,DNC,SET},"JR_NC_r8");
  dh(0x38,JRSCOND,8,2,(sm83_cc_table_t){DNC,DNC,DNC,SET},"JR_C_r8");
}

TEST(sm83, sm83_Execute_JRS) {
  bus_write(pc, 0x18);
  bus_write(pc + 1, -10);
  sm83_execute();
  TEST_ASSERT(sm83_get_pc() == pc-10);
}

TEST(sm83, sm83_Execute_JRSCOND) {
  bus_write(pc, 0x20);
  bus_write(pc + 1, 0);
  sm83_execute();
  TEST_ASSERT(sm83_get_pc() == pc + 2);
  sm83_n_flag(true);
  sm83_z_flag(true); 
  bus_write(pc + 2, 0x20);
  bus_write(pc + 3, 2);
  sm83_execute();
  TEST_ASSERT(sm83_get_pc() == pc + 4);
}

TEST(sm83, sm83_Execute_LDR_DIR) {
  for (uint8_t i = 0x41; i < 0x70; i+=3) {
    bus_write(pc, i);
    *(sm83_get_register_operator_ldr_dir(i)) = i;
    sm83_execute();
    TEST_ASSERT(cpu->pc == pc + 1);
    TEST_ASSERT(*(sm83_get_register_operator_ldr_dir(i)) == 0);
    sm83_free();
  }
}

TEST(sm83, sm83_Get_Register_Operator_Ldr_Dir) {
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x40) == &(cpu->b));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x44) == &(cpu->b));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x47) == &(cpu->b));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x48) == &(cpu->c));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x4c) == &(cpu->c));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x4f) == &(cpu->c));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x50) == &(cpu->d));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x54) == &(cpu->d));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x57) == &(cpu->d));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x58) == &(cpu->e));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x5c) == &(cpu->e));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x5f) == &(cpu->e));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x60) == &(cpu->h));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x64) == &(cpu->h));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x67) == &(cpu->h));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x68) == &(cpu->l));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x6c) == &(cpu->l));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x6f) == &(cpu->l));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x78) == &(cpu->a));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x7c) == &(cpu->a));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x7f) == &(cpu->a));
  TEST_ASSERT(sm83_get_register_operator_ldr_dir(0x00) == NULL);
}

TEST(sm83, sm83_Get_Register_Operand) {
  sm83_t *cm = sm83_get_modifiable_state();
  cm->b = 1;
  cm->c = 2;
  cm->d = 3;
  cm->e = 4;
  cm->h = 5;
  cm->l = 6;
  bus_write(0x0506, 7);
  cm->a = 8;
  TEST_ASSERT(sm83_get_register_operand(0x40) == 1);
  TEST_ASSERT(sm83_get_register_operand(0x41) == 2);
  TEST_ASSERT(sm83_get_register_operand(0x42) == 3);
  TEST_ASSERT(sm83_get_register_operand(0x43) == 4);
  TEST_ASSERT(sm83_get_register_operand(0x44) == 5);
  TEST_ASSERT(sm83_get_register_operand(0x45) == 6);
  TEST_ASSERT(sm83_get_register_operand(0x46) == 7);
  TEST_ASSERT(sm83_get_register_operand(0x47) == 8);
}

static void get_flag_helper(sm83_cc_table_t expected) {
  sm83_z_flag(expected.z == SET ? true : false);
  sm83_n_flag(expected.n == SET ? true : false);
  sm83_h_flag(expected.h == SET ? true : false);
  sm83_c_flag(expected.c == SET ? true : false);
  sm83_cc_table_t result = sm83_get_flags_as_cc_table();
  TEST_ASSERT(expected.z == result.z);
  TEST_ASSERT(expected.n == result.n);
  TEST_ASSERT(expected.h == result.h);
  TEST_ASSERT(expected.c == result.c);
}

TEST(sm83, sm83_Set_Clear_Flags) {
  get_flag_helper((sm83_cc_table_t){SET,SET,SET,SET});
  get_flag_helper((sm83_cc_table_t){SET,SET,SET,CLR});
  get_flag_helper((sm83_cc_table_t){SET,SET,CLR,CLR});
  get_flag_helper((sm83_cc_table_t){SET,CLR,CLR,CLR});
  get_flag_helper((sm83_cc_table_t){CLR,CLR,CLR,CLR});
}

TEST(sm83, sm83_Set_Get_Flags_From_CC_Table) {
  sm83_set_flags_from_cc_table((sm83_cc_table_t){SET,CLR,SET,CLR});
  sm83_cc_table_t cc = sm83_get_flags_as_cc_table();
  TEST_ASSERT(cc.z == SET);
  TEST_ASSERT(cc.n == CLR);
  TEST_ASSERT(cc.h == SET);
  TEST_ASSERT(cc.c == CLR);
}

TEST(sm83, sm83_Set_Get_Flags) {
  sm83_set_flags_from_cc_table((sm83_cc_table_t){SET,CLR,SET,CLR});
  TEST_ASSERT(sm83_get_flag_from_pos(Z_FLAG_POS) == true);
  TEST_ASSERT(sm83_get_flag_from_pos(N_FLAG_POS) == false);
  TEST_ASSERT(sm83_get_flag_from_pos(H_FLAG_POS) == true);
  TEST_ASSERT(sm83_get_flag_from_pos(C_FLAG_POS) == false);
}

TEST(sm83, sm83_Check_Condition) {
  sm83_set_flags_from_cc_table((sm83_cc_table_t){SET,CLR,SET,CLR});
  TEST_ASSERT(sm83_check_condition((sm83_cc_table_t){SET,CLR,SET,SET}) == false);
}