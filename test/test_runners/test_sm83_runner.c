#include "unity_fixture.h"

TEST_GROUP_RUNNER(sm83) {
  RUN_TEST_CASE(sm83, sm83_Get_Opcode);

  RUN_TEST_CASE(sm83, sm83_Decode_Misc);
  RUN_TEST_CASE(sm83, sm83_Decode_JRS);
  RUN_TEST_CASE(sm83, sm83_Decode_JRSCOND);
  
  RUN_TEST_CASE(sm83, sm83_Execute_JRS);
  RUN_TEST_CASE(sm83, sm83_Execute_JRSCOND);
  RUN_TEST_CASE(sm83, sm83_Execute_LDR_DIR);
  RUN_TEST_CASE(sm83, sm83_Get_Register_Operator_Ldr_Dir);

  RUN_TEST_CASE(sm83, sm83_Get_Register_Operand);

  RUN_TEST_CASE(sm83, sm83_Set_Clear_Flags);
  RUN_TEST_CASE(sm83, sm83_Set_Get_Flags_From_CC_Table);
  RUN_TEST_CASE(sm83, sm83_Set_Get_Flags);
  RUN_TEST_CASE(sm83, sm83_Check_Condition);
}