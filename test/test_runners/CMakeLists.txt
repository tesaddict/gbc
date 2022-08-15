add_executable(test all_tests.c
                    test_bus_runner.c
                    test_sm83_runner.c
                    ../test_bus.c
                    ../test_sm83.c
                    ../unity/unity.c
                    ../unity/unity_fixture.c
                    ${GBC_SOURCE_DIR}/src/bus.c
                    ${GBC_SOURCE_DIR}/src/sm83.c)
target_compile_options(test PRIVATE -Werror -Wall -Wextra)
target_compile_definitions(test PRIVATE UNITY_FIXTURE_NO_EXTRAS)