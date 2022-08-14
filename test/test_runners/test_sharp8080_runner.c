#include "unity_fixture.h"

TEST_GROUP_RUNNER(Sharp8080) {
    RUN_TEST_CASE(Sharp8080, Sharp8080_Get_Opcode);

    RUN_TEST_CASE(Sharp8080, Sharp8080_Decode_Misc);
    RUN_TEST_CASE(Sharp8080, Sharp8080_Decode_JRS);
    RUN_TEST_CASE(Sharp8080, Sharp8080_Decode_JRSCOND);

    RUN_TEST_CASE(Sharp8080, Sharp8080_Execute_JRS);

    RUN_TEST_CASE(Sharp8080, Sharp8080_Set_Clear_Flags);
    RUN_TEST_CASE(Sharp8080, Sharp8080_Set_Get_Flags_From_CC_Table);
    RUN_TEST_CASE(Sharp8080, Sharp8080_Set_Get_Flags);
}