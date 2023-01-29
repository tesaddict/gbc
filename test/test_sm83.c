#include "unity_fixture.h"
#include "sm83.h"
#include "bus.h"

uint16_t pc = 0x100;
static sm83_t *cpu;

TEST_GROUP(SM83);

TEST_SETUP(SM83) {
  sm83_init((sm83_t){0,0,0,0,0,0,0,false,false,false,false,pc,0});
  cpu = sm83_get_cpu();
}

TEST_TEAR_DOWN(SM83) {
  sm83_clear();
  bus_clear();
}

static uint16_t jrs(uint16_t opcode, int8_t offset) {
  bus_write(pc, opcode);
  bus_write(pc + 1, offset);
  sm83_execute();
  uint16_t pc = cpu->pc;
  sm83_clear();
  return pc;
}

#define JRS_NOP pc + 0x02
#define JRS_OPP pc + 0x10
#define JRS_OPN pc - 0x10

TEST(SM83, SM83_Execute_JRS) {
  TEST_ASSERT(jrs(0x18, 0x10) == JRS_OPP);
  TEST_ASSERT(jrs(0x18, -0x10) == JRS_OPN);
}

TEST(SM83, SM83_Execute_JRSCOND) {
  // JR_NZ_r8
  TEST_ASSERT(jrs(0x20, 0x10) == JRS_NOP);
  cpu->zf = true;
  cpu->nf = true;
  TEST_ASSERT(jrs(0x20, 0x10) == JRS_OPP);
  cpu->zf = true;
  cpu->nf = true;
  TEST_ASSERT(jrs(0x20, -0x10) == JRS_OPN);
  // JR_Z_r8
  TEST_ASSERT(jrs(0x28, 0x10) == JRS_NOP);
  cpu->zf = true;
  TEST_ASSERT(jrs(0x28, 0x10) == JRS_OPP);
  cpu->zf = true;
  TEST_ASSERT(jrs(0x28, -0x10) == JRS_OPN);
  // JR_NC_r8
  TEST_ASSERT(jrs(0x30, 0x10) == JRS_NOP);
  cpu->nf = true;
  cpu->cf = true;
  TEST_ASSERT(jrs(0x30, 0x10) == JRS_OPP);
  cpu->nf = true;
  cpu->cf = true;
  TEST_ASSERT(jrs(0x30, -0x10) == JRS_OPN);
  // JR_C_r8
  TEST_ASSERT(jrs(0x38, 0x10) == JRS_NOP);
  cpu->cf = true;
  TEST_ASSERT(jrs(0x38, 0x10) == JRS_OPP);
  cpu->cf = true;
  TEST_ASSERT(jrs(0x38, -0x10) == JRS_OPN);
}

static bool ldr(uint16_t opcode, uint8_t *dest) {
  bus_write(pc, opcode);
  switch(opcode % 0x8) {
    case 0x0: 
      cpu->b = 0xff;
      break;
    case 0x1: 
      cpu->c = 0xff;
      break;
    case 0x2: 
      cpu->d = 0xff;
      break;
    case 0x3: 
      cpu->e = 0xff;
      break;
    case 0x4: 
      cpu->h = 0xff;
      break;
    case 0x5: 
      cpu->l = 0xff;
      break;
    case 0x6: 
      bus_write((((uint16_t)cpu->h) << 8 | (uint16_t)cpu->l), 0xff);
      break;
    case 0x7: 
      cpu->a = 0xff;
      break;
  }
  sm83_execute();
  bool res = *dest == 0xff;
  res &= cpu->pc = pc + 1;
  sm83_clear();
  return res;
}

TEST(SM83, SM83_Execute_LDRDIR) {
  // LDB_X
  for (uint16_t i = 0x40; i <= 0x47; ++i) {
    if (i == 0x46) continue;
    TEST_ASSERT(ldr(i, &(cpu->b)));
  }
  // LDC_X
  for (uint16_t i = 0x48; i <= 0x4f; ++i) {
    if (i == 0x4e) continue;
    TEST_ASSERT(ldr(i, &(cpu->c)));
  }
  // LDD_X
  for (uint16_t i = 0x50; i <= 0x57; ++i) {
    if (i == 0x56) continue;
    TEST_ASSERT(ldr(i, &(cpu->d)));
  }
  // LDE_X
  for (uint16_t i = 0x58; i <= 0x5f; ++i) {
    if (i == 0x5e) continue;
    TEST_ASSERT(ldr(i, &(cpu->e)));
  }
  // LDH_X
  for (uint16_t i = 0x60; i <= 0x67; ++i) {
    if (i == 0x66) continue;
    TEST_ASSERT(ldr(i, &(cpu->h)));
  }
  // LDL_X
  for (uint16_t i = 0x68; i <= 0x6f; ++i) {
    if (i == 0x6e) continue;
    TEST_ASSERT(ldr(i, &(cpu->l)));
  }
  // LDA_X
  for (uint16_t i = 0x78; i <= 0x7f; ++i) {
    if (i == 0x7e) continue;
    TEST_ASSERT(ldr(i, &(cpu->a)));
  }
}

TEST(SM83, SM83_Execute_LDRIND) {
  cpu->h = 0xbb;
  cpu->l = 0xbb;
  bus_write(0xbbbb, 0xff);
  TEST_ASSERT(ldr(0x46, &(cpu->b)));
  TEST_ASSERT(ldr(0x4e, &(cpu->c)));
  TEST_ASSERT(ldr(0x56, &(cpu->d)));
  TEST_ASSERT(ldr(0x5e, &(cpu->e)));
  TEST_ASSERT(ldr(0x66, &(cpu->h)));
  bus_write(0xffbb, 0xff);
  TEST_ASSERT(ldr(0x6e, &(cpu->l)));
  bus_write(0xffff, 0xff);
  TEST_ASSERT(ldr(0x7e, &(cpu->a)));
}